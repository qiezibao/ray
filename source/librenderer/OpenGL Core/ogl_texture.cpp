// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2014.
// +----------------------------------------------------------------------
// | * Redistribution and use of this software in source and binary forms,
// |   with or without modification, are permitted provided that the following
// |   conditions are met:
// |
// | * Redistributions of source code must retain the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer.
// |
// | * Redistributions in binary form must reproduce the above
// |   copyright notice, this list of conditions and the
// |   following disclaimer in the documentation and/or other
// |   materials provided with the distribution.
// |
// | * Neither the name of the ray team, nor the names of its
// |   contributors may be used to endorse or promote products
// |   derived from this software without specific prior
// |   written permission of the ray team.
// |
// | THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// | "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// | LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// | A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// | OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// | SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// | LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// | DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// | THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// | (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// | OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// +----------------------------------------------------------------------
#include "ogl_texture.h"

_NAME_BEGIN

#define MAX_COLOR_ATTACHMENTS 15

OGLTexture::OGLTexture() noexcept
	: _texture(0)
	, _textureAddr(0)
{
}

OGLTexture::~OGLTexture() noexcept
{
	this->close();
}

bool
OGLTexture::setup() except
{
	auto target = OGLTypes::asOGLTarget(this->getTexDim());
	auto internalFormat = OGLTypes::asOGLInternalformat(this->getTexFormat());
	auto stream = this->getStream();

	if (target == GL_INVALID_ENUM)
		return false;

	if (internalFormat == GL_INVALID_ENUM)
		return false;

	glGenTextures(1, &_texture);
	glBindTexture(target, _texture);

	GLsizei w = (GLsizei)this->getWidth();
	GLsizei h = (GLsizei)this->getHeight();
	GLsizei depth = (GLsizei)this->getDepth();

	applySamplerWrap(target, this->getSamplerWrap());
	applySamplerFilter(target, this->getSamplerFilter());
	applyTextureAnis(target, this->getSamplerAnis());

	GLsizei level = target == GL_TEXTURE_CUBE_MAP ? 6 : 1;
	level = std::max(level, this->getMipLevel());

	if (target == GL_TEXTURE_2D)
	{
		if (this->isMultiSample())
			glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, level, internalFormat, w, h, GL_FALSE);
		else
			glTexStorage2D(target, level, internalFormat, w, h);
	}
	else if (target == GL_TEXTURE_2D_ARRAY)
	{
		if (this->isMultiSample())
			glTexStorage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE, level, internalFormat, w, h, depth, GL_FALSE);
		else
			glTexStorage3D(target, level, internalFormat, w, h, depth);
	}
		
	else if (target == GL_TEXTURE_3D)
	{
		if (this->isMultiSample())
			glTexStorage3DMultisample(GL_TEXTURE_2D_MULTISAMPLE, level, internalFormat, w, h, depth, GL_FALSE);
		else
			glTexStorage3D(target, level, internalFormat, w, h, depth);
	}
	else if (target == GL_TEXTURE_CUBE_MAP)
	{
		if (this->isMultiSample())
			glTexStorage2DMultisample(GL_TEXTURE_CUBE_MAP, level, internalFormat, w, h, GL_FALSE);
		else
			glTexStorage2D(GL_TEXTURE_CUBE_MAP, level, internalFormat, w, h);
	}

	if (stream)
	{
		if (internalFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||
			internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ||
			internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT ||
			internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT ||
			internalFormat == GL_COMPRESSED_RG_RGTC2 ||
			internalFormat == GL_COMPRESSED_SIGNED_RG_RGTC2)
		{
			GLint level = (GLint)this->getMipLevel();
			GLsizei size = this->getMipSize();
			std::size_t offset = 0;
			std::size_t blockSize = internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ? 8 : 16;

			for (GLint mip = 0; mip < level; mip++)
			{
				auto mipSize = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

				glCompressedTexSubImage2D(GL_TEXTURE_2D, mip, 0, 0, w, h, internalFormat, mipSize, (char*)stream + offset);

				w = std::max(w >> 1, 1);
				h = std::max(h >> 1, 1);

				offset += mipSize;
			}
		}
		else
		{
			auto level = 0;
			auto format = OGLTypes::asOGLFormat(this->getTexFormat());
			auto type = OGLTypes::asOGLType(this->getTexFormat());

			switch (target)
			{
			case GL_TEXTURE_2D:
				glTexSubImage2D(target, level, 0, 0, w, h, format, type, stream);
				break;
			case GL_TEXTURE_2D_ARRAY:
				glTexSubImage3D(target, level, 0, 0, 0, w, h, depth, format, type, stream);
				break;
			case GL_TEXTURE_3D:
				glTexSubImage3D(target, level, 0, 0, 0, w, h, depth, format, type, stream);
				break;
			case GL_TEXTURE_CUBE_MAP:
				glTexSubImage3D(target, level, 0, 0, 0, w, h, 6, format, type, stream);
				break;
			break;
			default:
				assert(false);
				break;
			}
		}
	}

	if (this->isMipmap())
	{
		glGenerateMipmap(target);		
	}

	_textureAddr = glGetTextureHandleARB(_texture);
	glMakeTextureHandleResidentARB(_textureAddr);

	glBindTexture(target, GL_NONE);
	return true;
}

void
OGLTexture::close() noexcept
{
	if (_texture)
	{
		glDeleteTextures(1, &_texture);
		_texture = GL_NONE;
		_textureAddr = GL_NONE;
	}
}

GLuint
OGLTexture::getInstanceID() noexcept
{
	return _texture;
}

GLuint64
OGLTexture::getInstanceAddr() noexcept
{
	return _textureAddr;
}

void
OGLTexture::applySamplerWrap(GLenum target, SamplerWrap wrap) noexcept
{
	if (SamplerWrap::Repeat == wrap)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else if (SamplerWrap::ClampToEdge == wrap)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	else if (SamplerWrap::Mirror == wrap)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	}
}

void
OGLTexture::applySamplerFilter(GLenum target, SamplerFilter filter) noexcept
{
	if (filter == SamplerFilter::Nearest)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else if (filter == SamplerFilter::Linear)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else if (filter == SamplerFilter::NearestMipmapLinear)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	}
	else if (filter == SamplerFilter::NearestMipmapNearest)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	}
	else if (filter == SamplerFilter::LinearMipmapNearest)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	}
	else if (filter == SamplerFilter::LinearMipmapLinear)
	{
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
}

void
OGLTexture::applyTextureAnis(GLenum target, SamplerAnis anis) noexcept
{
	if (anis == SamplerAnis::Anis1)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1);
	else if (anis == SamplerAnis::Anis2)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2);
	else if (anis == SamplerAnis::Anis4)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
	else if (anis == SamplerAnis::Anis8)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
	else if (anis == SamplerAnis::Anis16)
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
}

OGLRenderTexture::OGLRenderTexture() noexcept
	: _fbo(GL_NONE)
	, _layer(GL_NONE)
{
}

OGLRenderTexture::~OGLRenderTexture() noexcept
{
	this->close();
}

bool
OGLRenderTexture::setup(TexturePtr texture) except
{
	assert(!_fbo);
	assert(texture);

	OGLCheck::checkError();

	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	auto sharedDepthTarget = this->getSharedDepthTexture();
	auto sharedStencilTarget = this->getSharedStencilTexture();

	if (sharedDepthTarget == sharedStencilTarget)
	{
		if (sharedDepthTarget)
			this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_STENCIL_ATTACHMENT);
	}
	else
	{
		if (sharedDepthTarget)
			this->bindRenderTexture(sharedDepthTarget->getResolveTexture(), GL_DEPTH_ATTACHMENT);

		if (sharedStencilTarget)
			this->bindRenderTexture(sharedStencilTarget->getResolveTexture(), GL_STENCIL_ATTACHMENT);
	}

	auto resolveFormat = this->getTexFormat();

	if (resolveFormat == TextureFormat::DEPTH24_STENCIL8 || resolveFormat == TextureFormat::DEPTH32_STENCIL8)
		this->bindRenderTexture(texture, GL_DEPTH_STENCIL_ATTACHMENT);
	else if (resolveFormat == TextureFormat::DEPTH_COMPONENT16 || resolveFormat == TextureFormat::DEPTH_COMPONENT24 || resolveFormat == TextureFormat::DEPTH_COMPONENT32)
		this->bindRenderTexture(texture, GL_DEPTH_ATTACHMENT);
	else if (resolveFormat == TextureFormat::STENCIL8)
		this->bindRenderTexture(texture, GL_STENCIL_ATTACHMENT);
	else
		this->bindRenderTexture(texture, GL_COLOR_ATTACHMENT0);

	_resolveTexture = texture;

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void
OGLRenderTexture::setup(std::size_t w, std::size_t h, TextureDim dim, TextureFormat format) except
{
	_resolveTexture = std::make_shared<OGLTexture>();
	_resolveTexture->setWidth(w);
	_resolveTexture->setHeight(h);
	_resolveTexture->setTexDim(dim);
	_resolveTexture->setTexFormat(format);
	_resolveTexture->setup();

	this->setup(_resolveTexture);
}

void
OGLRenderTexture::setup(std::size_t w, std::size_t h, std::size_t d, TextureDim dim, TextureFormat format) except
{
	_resolveTexture = std::make_shared<OGLTexture>();
	_resolveTexture->setWidth(w);
	_resolveTexture->setHeight(h);
	_resolveTexture->setDepth(d);
	_resolveTexture->setTexDim(dim);
	_resolveTexture->setTexFormat(format);
	_resolveTexture->setup();

	this->setup(_resolveTexture);
}

void
OGLRenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

void
OGLRenderTexture::bindRenderTexture(TexturePtr texture, GLenum attachment) noexcept
{
	auto handle = std::dynamic_pointer_cast<OGLTexture>(texture)->getInstanceID();

	switch (texture->getTexDim())
	{
		case TextureDim::DIM_2D:
		{
			if (texture->isMultiSample())
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0);
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0);
			break;
		}
		case TextureDim::DIM_2D_ARRAY:
		{
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, handle, 0, 0);
			break;
		}
		case TextureDim::DIM_CUBE:
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X, handle, 0);
			break;
		}
		case TextureDim::DIM_3D:
		{
			glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, handle, 0, 0);
			break;
		}
	}
}

void
OGLRenderTexture::onSetRenderTextureAfter(RenderTexturePtr target) noexcept
{
	if (_fbo)
	{
		auto resolveFormat = target->getTexFormat();
		if (resolveFormat == TextureFormat::DEPTH24_STENCIL8 || resolveFormat == TextureFormat::DEPTH32_STENCIL8)
			this->bindRenderTexture(this->getResolveTexture(), GL_STENCIL_ATTACHMENT);
		else if (resolveFormat == TextureFormat::DEPTH_COMPONENT16 || resolveFormat == TextureFormat::DEPTH_COMPONENT24 || resolveFormat == TextureFormat::DEPTH_COMPONENT32)
			this->bindRenderTexture(this->getResolveTexture(), GL_DEPTH_ATTACHMENT);
		else if (resolveFormat == TextureFormat::STENCIL8)
			this->bindRenderTexture(this->getResolveTexture(), GL_STENCIL_ATTACHMENT);
		else
			this->bindRenderTexture(this->getResolveTexture(), GL_COLOR_ATTACHMENT0);
	}
}

GLuint
OGLRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void 
OGLRenderTexture::setLayer(GLuint layer) noexcept
{
	_layer = layer;
}

GLuint 
OGLRenderTexture::getLayer() const noexcept
{
	return _layer;
}

OGLMultiRenderTexture::OGLMultiRenderTexture() noexcept
	: _fbo(GL_NONE)
{
}

OGLMultiRenderTexture::~OGLMultiRenderTexture() noexcept
{
	this->close();
}

bool
OGLMultiRenderTexture::setup() noexcept
{
	assert(GL_NONE == _fbo);

	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	auto sharedDepthTarget = this->getSharedDepthTexture();
	auto sharedStencilTarget = this->getSharedStencilTexture();

	if (sharedDepthTarget == sharedStencilTarget)
	{
		if (sharedDepthTarget)
			this->bindRenderTexture(sharedDepthTarget, GL_DEPTH_STENCIL_ATTACHMENT);
	}
	else
	{
		if (sharedDepthTarget)
		{
			this->bindRenderTexture(sharedDepthTarget, GL_DEPTH_ATTACHMENT);
		}

		if (sharedStencilTarget)
		{
			this->bindRenderTexture(sharedStencilTarget, GL_STENCIL_ATTACHMENT);
		}
	}

	GLenum draw[MAX_COLOR_ATTACHMENTS] = { 0 };
	GLenum attachment = GL_COLOR_ATTACHMENT0;
	GLsizei count = 0;

	for (auto& target : this->getRenderTextures())
	{
		this->bindRenderTexture(target, attachment);
		draw[count++] = attachment++;
	}

	glDrawBuffers(count, draw);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void
OGLMultiRenderTexture::close() noexcept
{
	if (_fbo != GL_NONE)
	{
		glDeleteFramebuffers(1, &_fbo);
		_fbo = GL_NONE;
	}
}

GLuint
OGLMultiRenderTexture::getInstanceID() noexcept
{
	return _fbo;
}

void
OGLMultiRenderTexture::bindRenderTexture(RenderTexturePtr target, GLenum attachment) noexcept
{
	auto handle = std::dynamic_pointer_cast<OGLTexture>(target->getResolveTexture())->getInstanceID();

	switch (target->getTexDim())
	{
		case TextureDim::DIM_2D:
		{
			if (target->isMultiSample())
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, handle, 0);
			else
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, handle, 0);
			break;
		}
		case TextureDim::DIM_2D_ARRAY:
		{
			glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, handle, 0, 0);
			break;
		}
		case TextureDim::DIM_3D:
		{
			glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, handle, 0, 0);
			break;
		}
	}
}

_NAME_END
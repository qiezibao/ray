// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2016.
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
#include "egl2_pipeline.h"
#include "egl2_state.h"
#include "egl2_shader.h"
#include "egl2_input_layout.h"
#include "egl2_descriptor.h"
#include "egl2_graphics_data.h"

_NAME_BEGIN

__ImplementSubClass(EGL2Pipeline, GraphicsPipeline, "EGL2Pipeline")

EGL2Pipeline::EGL2Pipeline() noexcept
{
}

EGL2Pipeline::~EGL2Pipeline() noexcept
{
	this->close();
}

bool
EGL2Pipeline::setup(const GraphicsPipelineDesc& pipelineDesc) noexcept
{
	assert(pipelineDesc.getGraphicsState());
	assert(pipelineDesc.getGraphicsProgram());
	assert(pipelineDesc.getGraphicsInputLayout());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout());
	assert(pipelineDesc.getGraphicsState()->isInstanceOf<EGL2GraphicsState>());
	assert(pipelineDesc.getGraphicsProgram()->isInstanceOf<EGL2Program>());
	assert(pipelineDesc.getGraphicsInputLayout()->isInstanceOf<EGL2InputLayout>());
	assert(pipelineDesc.getGraphicsDescriptorSetLayout()->isInstanceOf<EGL2DescriptorSetLayout>());

	GLuint offset = 0;

	auto& components = pipelineDesc.getGraphicsInputLayout()->getGraphicsInputLayoutDesc().getGraphicsVertexLayouts();
	for (auto& it : components)
	{
		GLuint attribIndex = GL_INVALID_INDEX;
		GLenum type = EGL2Types::asVertexFormat(it.getVertexFormat());

		auto& semantic = it.getSemantic();
		if (semantic.empty())
		{
			GL_PLATFORM_LOG("Empty semantic");
			return false;
		}

		for (auto& ch : semantic)
		{
			if (ch < 'a' && ch > 'z')
			{
				GL_PLATFORM_LOG("Error semantic describe : %s", semantic);
				return false;
			}
		}

		auto& attributes = pipelineDesc.getGraphicsProgram()->getActiveAttributes();
		for (auto& attrib : attributes)
		{
			if (attrib->getSemanticIndex() == it.getSemanticIndex() && attrib->getSemantic() == it.getSemantic())
			{
				attribIndex = attrib->downcast<EGL2GraphicsAttribute>()->getBindingPoint();
				break;
			}
		}

		if (attribIndex != GL_INVALID_INDEX)
		{
			VertexAttrib attrib;
			attrib.index = attribIndex;
			attrib.count = it.getVertexCount();
			attrib.type = type;
			attrib.offset = offset;

			_attributes.push_back(attrib);

			offset += it.getVertexSize();
		}
	}

	_pipelineDesc = pipelineDesc;
	return true;
}

void
EGL2Pipeline::close() noexcept
{
	_attributes.clear();
}

void
EGL2Pipeline::apply() noexcept
{
}

void
EGL2Pipeline::bindVbo(const EGL2GraphicsDataPtr& vbo, GLsizei startVertices) noexcept
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo->getInstanceID());

	GLuint stride = vbo->getGraphicsDataDesc().getStride();
	for (auto& attrib : _attributes)
	{
		glEnableVertexAttribArray(attrib.index);
		glVertexAttribPointer(attrib.index, attrib.count, attrib.type, GL_FALSE, stride, (GLbyte*)nullptr + attrib.offset);
	}
}

void
EGL2Pipeline::bindIbo(const EGL2GraphicsDataPtr& ibo) noexcept
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->getInstanceID());
}

const GraphicsPipelineDesc&
EGL2Pipeline::getGraphicsPipelineDesc() const noexcept
{
	return _pipelineDesc;
}

void
EGL2Pipeline::setDevice(GraphicsDevicePtr device) noexcept
{
	_device = device;
}

GraphicsDevicePtr
EGL2Pipeline::getDevice() noexcept
{
	return _device.lock();
}

_NAME_END
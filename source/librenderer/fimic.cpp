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
// +-----------------------------------------------------------------------
#include "fimic.h"
#include <ray/material.h>

#include <ray/graphics_framebuffer.h>
#include <ray/graphics_texture.h>
#include <ray/render_pipeline.h>

_NAME_BEGIN

#define SAMPLE_COUNT 6
#define SAMPLE_LOG_SIZE 32
#define SAMPLE_LOG_COUNT (SAMPLE_LOG_SIZE * SAMPLE_LOG_SIZE)

float EyeAdaption(float lum)
{
	return math::lerp(0.2f, lum, 0.5f);
}

float ToneExposure(float avgLum)
{
	return 3 / (std::max(0.1f, 1 + 10 * EyeAdaption(avgLum)));
}

FimicToneMapping::Setting::Setting() noexcept
	: bloomThreshold(0.5f)
	, bloomIntensity(1.0f)
	, lumKey(0.98f)
	, lumDelta(30.0f)
	, lumExposure(1.5f)
{
}

FimicToneMapping::FimicToneMapping() noexcept
	: _lum(0.0f)
	, _lumAdapt(0.0f)
	, _lumRate(0.0f)
	, _lumFrequency(1.0f)
{
}

FimicToneMapping::~FimicToneMapping() noexcept
{
}

void
FimicToneMapping::setSetting(const Setting& setting) noexcept
{
	_bloomThreshold->uniform1f(_setting.bloomThreshold);
	_bloomIntensity->uniform1f(_setting.bloomIntensity);

	_toneLumExposure->uniform1f(setting.lumExposure);

	_setting = setting;
}

const FimicToneMapping::Setting&
FimicToneMapping::getSetting() const noexcept
{
	return _setting;
}

void
FimicToneMapping::measureLuminance(RenderPipeline& pipeline, GraphicsTexturePtr source) noexcept
{
	_timer->update();

	_lumRate += _timer->delta();
	if (_lumRate > _lumFrequency)
	{
		this->sunLumLog(pipeline, source, _texSampleLogView);

		float* data;
		if (_texSampleLogMap->map(0, 0, SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, (void**)&data))
		{
			float lum = 0.0f;
			for (std::size_t i = 0; i < SAMPLE_LOG_COUNT; ++i)
				lum += data[i];

			_lum = lum / (float)SAMPLE_LOG_COUNT;
			_lum = std::exp(_lum);

			_lumRate = 0.0f;
		}

		_texSampleLogMap->unmap();
	}

	_lumAdapt = _lumAdapt + ((_lum - _lumAdapt) * (1.0f - pow(_setting.lumKey, _setting.lumDelta * _timer->delta())));

	_toneLumExposure->uniform1f(_setting.lumExposure * ToneExposure(_lumAdapt));
}

void
FimicToneMapping::sunLum(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(float2(1.0f / source->getGraphicsTextureDesc().getWidth(), 1.0f / source->getGraphicsTextureDesc().getHeight()));

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_sunLum);
}

void
FimicToneMapping::sunLumLog(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);
	_texSourceSizeInv->uniform2f(float2(1.0 / source->getGraphicsTextureDesc().getWidth(), 1.0 / source->getGraphicsTextureDesc().getHeight()));

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_sunLumLog);
}

void
FimicToneMapping::generateBloom(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_bloom);
}

void
FimicToneMapping::blurh(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_blurh);
}

void
FimicToneMapping::blurv(RenderPipeline& pipeline, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_blurv);
}

void
FimicToneMapping::generateToneMapping(RenderPipeline& pipeline, GraphicsTexturePtr bloom, GraphicsTexturePtr source, GraphicsFramebufferPtr dest) noexcept
{
	_texSource->uniformTexture(source);
	_toneBloom->uniformTexture(bloom);

	GraphicsAttachmentType attachment[] = { GraphicsAttachmentType::GraphicsAttachmentTypeColor0 };

	pipeline.setFramebuffer(dest);
	pipeline.discradRenderTexture(attachment, 1);
	pipeline.drawScreenQuad(*_tone);
}

void
FimicToneMapping::onActivate(RenderPipeline& pipeline) noexcept
{
	std::uint32_t width, height;
	pipeline.getWindowResolution(width, height);

	_texSample4Map = pipeline.createTexture(width / 2.0f, height / 2.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texSample8Map = pipeline.createTexture(width / 4.0f, width / 4.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);
	_texCombieMap = pipeline.createTexture(width / 4.0f, height / 4.0f, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm);

	_texSampleLogMap = pipeline.createTexture(SAMPLE_LOG_SIZE, SAMPLE_LOG_SIZE, GraphicsTextureDim::GraphicsTextureDim2D, GraphicsFormat::GraphicsFormatR16SFloat);

	GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
	framebufferLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR8G8B8A8UNorm));
	_sampleViewLayout = pipeline.createFramebufferLayout(framebufferLayoutDesc);

	GraphicsFramebufferLayoutDesc framebufferLogLayoutDesc;
	framebufferLogLayoutDesc.addComponent(GraphicsAttachment(0, GraphicsImageLayout::GraphicsImageLayoutColorAttachmentOptimal, GraphicsFormat::GraphicsFormatR16SFloat));
	_sampleLogViewLayout = pipeline.createFramebufferLayout(framebufferLogLayoutDesc);

	GraphicsFramebufferDesc sample4ViewDesc;
	sample4ViewDesc.setWidth(width / 2.0f);
	sample4ViewDesc.setHeight(height / 2.0f);
	sample4ViewDesc.attach(_texSample4Map);
	sample4ViewDesc.setGraphicsFramebufferLayout(_sampleViewLayout);
	_texSample4View = pipeline.createFramebuffer(sample4ViewDesc);

	GraphicsFramebufferDesc sample8ViewDesc;
	sample8ViewDesc.setWidth(width / 4.0f);
	sample8ViewDesc.setHeight(width / 4.0f);
	sample8ViewDesc.attach(_texSample8Map);
	sample8ViewDesc.setGraphicsFramebufferLayout(_sampleViewLayout);
	_texSample8View = pipeline.createFramebuffer(sample8ViewDesc);

	GraphicsFramebufferDesc sampleLogViewDesc;
	sampleLogViewDesc.setWidth(SAMPLE_LOG_SIZE);
	sampleLogViewDesc.setHeight(SAMPLE_LOG_SIZE);
	sampleLogViewDesc.attach(_texSampleLogMap);
	sampleLogViewDesc.setGraphicsFramebufferLayout(_sampleLogViewLayout);
	_texSampleLogView = pipeline.createFramebuffer(sampleLogViewDesc);

	GraphicsFramebufferDesc sampleCombieViewDesc;
	sampleCombieViewDesc.setWidth(width / 4.0f);
	sampleCombieViewDesc.setHeight(height / 4.0f);
	sampleCombieViewDesc.attach(_texCombieMap);
	sampleCombieViewDesc.setGraphicsFramebufferLayout(_sampleViewLayout);
	_texCombieView = pipeline.createFramebuffer(sampleCombieViewDesc);

	_fimic = pipeline.createMaterial("sys:fx/fimic.fxml");

	_sunLum = _fimic->getTech("SumLum");
	_sunLumLog = _fimic->getTech("SumLumLog");
	_bloom = _fimic->getTech("GenerateBloom");
	_blurh = _fimic->getTech("BlurBloomh");
	_blurv = _fimic->getTech("BlurBloomv");
	_tone = _fimic->getTech("FimicTongMapping");

	_bloomThreshold = _fimic->getParameter("bloomThreshold");
	_bloomIntensity = _fimic->getParameter("bloomIntensity");
	_bloomOffset = _fimic->getParameter("bloomOffset");
	_bloomWeight = _fimic->getParameter("bloomWeight");

	_toneBloom = _fimic->getParameter("texBloom");
	_toneLumExposure = _fimic->getParameter("exposure");

	_texSource = _fimic->getParameter("texSource");
	_texSourceSizeInv = _fimic->getParameter("texSourceSizeInv");

	_timer = std::make_shared<Timer>();

	float2 offset[] = {
		float2(-5.0f / (width / 4.0f), -5.0f / (height / 4.0f)),
		float2(-4.0f / (width / 4.0f), -4.0f / (height / 4.0f)),
		float2(-3.0f / (width / 4.0f), -3.0f / (height / 4.0f)),
		float2(-2.0f / (width / 4.0f), -2.0f / (height / 4.0f)),
		float2(-1.0f / (width / 4.0f), -1.0f / (height / 4.0f)),
		float2(1.0f / (width / 4.0f), 1.0f / (height / 4.0f)),
		float2(2.0f / (width / 4.0f), 2.0f / (height / 4.0f)),
		float2(3.0f / (width / 4.0f), 3.0f / (height / 4.0f)),
		float2(4.0f / (width / 4.0f), 4.0f / (height / 4.0f)),
		float2(5.0f / (width / 4.0f), 5.0f / (height / 4.0f)),
	};

	float weight[] = { 0.2f,0.02f,0.044f,0.0716f,0.1046f,0.1686f,0.1686f,0.1046f,0.0716f,0.044f,0.02f };

	_bloomOffset->uniform2fv(sizeof(offset) / sizeof(offset[0]), (float*)offset);
	_bloomWeight->uniform1fv(sizeof(weight) / sizeof(weight[0]), weight);

	this->setSetting(_setting);
}

void
FimicToneMapping::onDeactivate(RenderPipeline& pipeline) noexcept
{
	_texSample4Map.reset();
	_texSample8Map.reset();
	_texSampleLogMap.reset();
	_texCombieMap.reset();

	_texSample4View.reset();
	_texSample8View.reset();
	_texSampleLogView.reset();
	_texCombieView.reset();
}

bool
FimicToneMapping::onRender(RenderPipeline& pipeline, RenderQueue queue, GraphicsFramebufferPtr& source, GraphicsFramebufferPtr& swap) noexcept
{
	if (queue != RenderQueue::RenderQueuePostprocess)
		return false;

	auto texture = source->getGraphicsFramebufferDesc().getTextures().front();

	this->sunLum(pipeline, texture, _texSample4View);
	this->sunLum(pipeline, _texSample4Map, _texSample8View);

	this->measureLuminance(pipeline, _texSample8Map);

	this->generateBloom(pipeline, _texSample8Map, _texCombieView);

	this->blurh(pipeline, _texCombieMap, _texSample8View);
	this->blurv(pipeline, _texSample8Map, _texCombieView);

	this->generateToneMapping(pipeline, _texCombieMap, texture, swap);

	return true;
}

_NAME_END
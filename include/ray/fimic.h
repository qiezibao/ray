// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2015.
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
#ifndef _H_FIMIC_H_
#define _H_FIMIC_H_

#include <ray/render_post_process.h>
#include <ray/timer.h>

_NAME_BEGIN

class EXPORT FimicToneMapping final : public RenderPostProcess
{
public:
	struct EXPORT Setting
	{
		float bloomThreshold;
		float bloomIntensity;

		float lumAve;
		float lumKey;
		float lumDelta;
		float lumExposure;

		float illuminationNumber;
		float illuminationWeight;
		float illuminationDecay;

		float burnout;

		Setting() noexcept;
	};

public:
	FimicToneMapping() noexcept;
	~FimicToneMapping() noexcept;

	void setSetting(const Setting& setting) noexcept;
	const Setting& getSetting() const noexcept;

private:

	void sample4(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
	void sample8(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
	void sampleLog(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;

	void measureLuminance(RenderPipeline& pipeline, RenderTexturePtr source) noexcept;

	void generateBloom(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
	void generateToneMapping(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;

	void blurh(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;
	void blurv(RenderPipeline& pipeline, RenderTexturePtr source, RenderTexturePtr dest) noexcept;

private:

	void onActivate(RenderPipeline& pipeline) except;
	void onDeactivate(RenderPipeline& pipeline) except;

	void onRender(RenderPipeline& pipeline, RenderTexturePtr source) noexcept;

private:

	float _lumAdapt;

	TimerPtr _timer;

	Setting _setting;

	RenderTexturePtr _texSample4;
	RenderTexturePtr _texSample8;
	RenderTexturePtr _texSampleLog;
	RenderTexturePtr _texBloom;

	MaterialPtr _fimic;
	MaterialPassPtr _sample4;
	MaterialPassPtr _sample8;
	MaterialPassPtr _samplelog;
	MaterialPassPtr _bloom;
	MaterialPassPtr _blurh;
	MaterialPassPtr _blurv;
	MaterialPassPtr _tone;

	MaterialParamPtr _bloomThreshold;
	MaterialParamPtr _bloomIntensity;
	MaterialParamPtr _bloomTexSizeInv;

	MaterialParamPtr _toneSource;
	MaterialParamPtr _toneBloom;
	MaterialParamPtr _toneLumAve;
	MaterialParamPtr _toneLumKey;
	MaterialParamPtr _toneLumExposure;
	MaterialParamPtr _toneBurnout;
	MaterialParamPtr _toneDefocus;
};

_NAME_END

#endif
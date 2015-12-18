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
#ifndef _H_RENDER_PIPELINE_H_
#define _H_RENDER_PIPELINE_H_

#include <ray/graphics_data.h>
#include <ray/graphics_layout.h>
#include <ray/render_data_manager_base.h>

_NAME_BEGIN

class EXPORT RenderPipeline
{
public:
	RenderPipeline() noexcept;
	virtual ~RenderPipeline() noexcept;

	void open(WindHandle window, std::uint32_t w, std::uint32_t h) except;
	void close() noexcept;

	void setDefaultGraphicsContext(GraphicsContextPtr context) noexcept;
	GraphicsContextPtr getDefaultGraphicsContext() const noexcept;

	void setCamera(CameraPtr renderer) noexcept;
	CameraPtr getCamera() const noexcept;

	void setViewport(const Viewport& view) noexcept;
	const Viewport& getViewport() const noexcept;

	void setRenderDataManager(RenderDataManagerPtr manager) noexcept;
	RenderDataManagerPtr getRenderDataManagerPtr() const noexcept;

	void setWireframeMode(bool enable) noexcept;
	bool getWireframeMode() const noexcept;

	void setSwapInterval(SwapInterval interval) noexcept;
	SwapInterval getSwapInterval() const noexcept;

	GraphicsStatePtr createGraphicsState() noexcept;
	void setGraphicsState(GraphicsStatePtr state) noexcept;
	GraphicsStatePtr getGraphicsState() const noexcept;

	void setWindowResolution(std::uint32_t w, std::uint32_t h) noexcept;
	void getWindowResolution(std::uint32_t& w, std::uint32_t& h) const noexcept;

	void addRenderData(RenderQueue queue, RenderPass pass, RenderObjectPtr object) noexcept;
	RenderObjects& getRenderData(RenderQueue queue, RenderPass pass) noexcept;

	RenderTexturePtr createRenderTexture() noexcept;
	MultiRenderTexturePtr createMultiRenderTexture() noexcept;
	void setRenderTexture(RenderTexturePtr target) noexcept;
	void setMultiRenderTexture(MultiRenderTexturePtr target) noexcept;
	void setRenderTextureLayer(RenderTexturePtr target, int layer) noexcept;
	void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil) noexcept;
	void clearRenderTexture(ClearFlags flags, const Vector4& color, float depth, std::int32_t stencil, std::size_t i) noexcept;
	void discradRenderTexture() noexcept;
	void readRenderTexture(RenderTexturePtr target, TextureFormat pfd, std::size_t w, std::size_t h, void* data) noexcept;
	void blitRenderTexture(RenderTexturePtr srcTarget, const Viewport& src, RenderTexturePtr destTarget, const Viewport& dest) noexcept;

	TexturePtr createTexture() noexcept;
	TexturePtr createTexture(const std::string& name) except;

	MaterialPtr createMaterial(const std::string& name) except;
	void setMaterialPass(MaterialPassPtr pass) noexcept;
	void setMaterialManager(MaterialManagerPtr manager) noexcept;
	MaterialPassPtr getMaterialPass() noexcept;
	MaterialManagerPtr getMaterialManager() noexcept;

	ShaderPtr createShader() noexcept;
	ShaderObjectPtr createShaderObject() noexcept;
	void setShaderObject(ShaderObjectPtr progarm) noexcept;
	ShaderObjectPtr getShaderObject() const noexcept;

	GraphicsLayoutPtr createGraphicsLayout(const GraphicsLayoutDesc& desc) noexcept;

	GraphicsDataPtr createGraphicsData(const GraphicsDataDesc& desc) noexcept;
	bool updateBuffer(GraphicsDataPtr& data, void* str, std::size_t cnt) noexcept;
	void* mapBuffer(GraphicsDataPtr& data, std::uint32_t access) noexcept;
	void unmapBuffer(GraphicsDataPtr& data) noexcept;

	RenderBufferPtr createRenderBuffer(GraphicsDataPtr vb, GraphicsDataPtr ib) noexcept;
	RenderBufferPtr createRenderBuffer(const MeshProperty& mesh) except;
	RenderBufferPtr createRenderBuffer(const MeshPropertys& mesh) except;

	void setRenderBuffer(RenderBufferPtr buffer) except;
	void drawRenderBuffer(const RenderIndirect& renderable) except;
	void drawRenderBuffer(const RenderIndirects& renderable) except;

	void drawCone(MaterialPassPtr pass) noexcept;
	void drawSphere(MaterialPassPtr pass) noexcept;
	void drawSceneQuad(MaterialPassPtr pass) noexcept;
	void drawMesh(MaterialPassPtr pass, RenderBufferPtr mesh, const RenderIndirect& renderable) noexcept;
	void drawRenderQueue(RenderQueue type, RenderPass pass, MaterialPassPtr material = nullptr, RenderTexturePtr target = nullptr) noexcept;

	void addPostProcess(RenderPostProcessPtr postprocess) except;
	void removePostProcess(RenderPostProcessPtr postprocess) noexcept;
	void renderPostProcess(RenderTexturePtr renderTexture) except;

	void renderBegin() noexcept;
	void renderEnd() noexcept;

protected:

	virtual void onRenderObjectPre(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except;
	virtual void onRenderObject(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except;
	virtual void onRenderObjectPost(RenderObject& object, RenderQueue queue, RenderPass type, MaterialPassPtr pass) except;

private:
	RenderPipeline(const RenderPipeline&) noexcept = delete;
	RenderPipeline& operator=(const RenderPipeline&) noexcept = delete;

private:
	
	std::uint32_t _width;
	std::uint32_t _height;

	CameraPtr _camera;

	GraphicsDevicePtr _graphicsDevice;
	GraphicsContextPtr _graphicsContext;

	RenderBufferPtr _renderSceneQuad;
	RenderBufferPtr _renderSphere;
	RenderBufferPtr _renderCone;

	RenderIndirect _renderConeIndirect;
	RenderIndirect _renderSphereIndirect;
	RenderIndirect _renderSceneQuadIndirect;

	MaterialManagerPtr _materialManager;

	RenderDataManagerPtr _dataManager;
	RenderPostProcessor _postprocessors[RenderQueue::RQ_NUMS];
};

_NAME_END

#endif
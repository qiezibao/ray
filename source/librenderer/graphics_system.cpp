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
#include <ray/graphics_system.h>

#if defined(_BUILD_OPENGL_CORE)
#	include "OpenGL Core/ogl_device.h"
#endif
#if defined(_BUILD_OPENGL_ES2)
#	include "OpenGL ES2/egl2_device.h"
#endif
#if defined(_BUILD_OPENGL_ES3)
#	include "OpenGL ES3/egl3_device.h"
#endif
#if defined(_BUILD_VULKAN)
#   include "Vulkan/vk_system.h"
#	include "Vulkan/vk_device.h"
#endif

_NAME_BEGIN

__ImplementSubClass(GraphicsSystem, rtti::Interface, "GraphicsSystem")
__ImplementSingleton(GraphicsSystem)

GraphicsSystem::GraphicsSystem() noexcept
{
}

GraphicsSystem::~GraphicsSystem() noexcept
{
}

GraphicsDevicePtr
GraphicsSystem::createDevice(const GraphicsDeviceDesc& desc) noexcept
{
	auto deviceType = desc.getDeviceType();

#if defined(_BUILD_OPENGL_CORE)
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLCore ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGL)
	{
		auto device = std::make_shared<OGLDevice>();
		if (device->setup(desc))
			return device;
		return nullptr;
	}

#endif
#if defined(_BUILD_OPENGL_ES2)
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES2)
	{
		auto device = std::make_shared<EGL2Device>();
		if (device->setup(desc))
			return device;
		return nullptr;
	}
#endif
#if defined(_BUILD_OPENGL_ES3)
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES3 ||
		deviceType == GraphicsDeviceType::GraphicsDeviceTypeOpenGLES31)
	{
		auto device = std::make_shared<EGL3Device>();
		if (device->setup(desc))
			return device;
		return nullptr;
	}
#endif
#if defined(_BUILD_VULKAN)
	if (deviceType == GraphicsDeviceType::GraphicsDeviceTypeVulkan)
	{
		if (!VulkanSystem::instance()->open())
			return nullptr;

		GraphicsDeviceDesc deviceDesc = desc;
		deviceDesc.setPhysicalDevice(VulkanSystem::instance()->getPhysicalDevices().front());

		auto device = std::make_shared<VulkanDevice>();
		if (device->setup(deviceDesc))
			return device;

		return nullptr;
	}
#endif
	return nullptr;
}

_NAME_END
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
#include "al_sound_device.h"
#include "al_sound_buffer.h"
#include "al_sound_source.h"
#include "al_sound_listener.h"

_NAME_BEGIN

ALSoundDevice::ALSoundDevice() noexcept
	: _device(nullptr)
	, _context(nullptr)
{
}

ALSoundDevice::~ALSoundDevice() noexcept
{
	this->close();
}

bool
ALSoundDevice::open() noexcept
{
	_device = ::alcOpenDevice(nullptr);
	if (!_device)
		return false;

	_context = ::alcCreateContext(_device, nullptr);
	if (!::alcMakeContextCurrent(_context))
		return false;

	return true;
}

void
ALSoundDevice::close() noexcept
{
	::alcMakeContextCurrent(AL_NONE);

	if (_context)
	{
		::alcDestroyContext(_context);
		_context = nullptr;
	}

	if (_device)
	{
		::alcCloseDevice(_device);
		_device = nullptr;
	}
}

bool
ALSoundDevice::isOpen() const noexcept
{
	return _device ? true : false;
}

void
ALSoundDevice::setDistanceModel(bool enable) noexcept
{
	if (_distanceModel != enable)
	{
		if (enable)
			::alEnable(AL_SOURCE_DISTANCE_MODEL);
		else
			::alDisable(AL_SOURCE_DISTANCE_MODEL);
		_distanceModel = enable;
	}
}

bool
ALSoundDevice::getDistanceModel() const noexcept
{
	return _distanceModel;
}

SoundSourcePtr
ALSoundDevice::createSoundSource()
{
	return std::make_shared<ALSoundSource>();
}

SoundListenerPtr
ALSoundDevice::createSoundListener() noexcept
{
	return std::make_shared<ALSoundListener>();
}

_NAME_END
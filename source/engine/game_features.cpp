// +----------------------------------------------------------------------
// | Project : ray.
// | All rights reserved.
// +----------------------------------------------------------------------
// | Copyright (c) 2013-2017.
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
#include <ray/game_features.h>
#include <ray/game_server.h>
#include <ray/game_scene.h>

_NAME_BEGIN

__ImplementSubInterface(GameFeature, MessageListener, "GameFeature")

GameFeature::GameFeature() noexcept
	: _isActive(false)
	, _gameServer(nullptr)
{
}

GameFeature::~GameFeature() noexcept
{
}

void
GameFeature::setActive(bool active) except
{
	if (_isActive != active)
	{
		if (active)
			this->onActivate();
		else
			this->onDeactivate();

		_isActive = active;
	}
}
bool
GameFeature::getActive() noexcept
{
	return _isActive;
}

void
GameFeature::setGameListener(const GameListenerPtr& listener) noexcept
{
	if (_gameListener != listener)
	{
		this->onListenerChangeBefore();

		_gameListener = listener;

		this->onListenerChangeAfter();
	}
}

const GameListenerPtr&
GameFeature::getGameListener() const noexcept
{
	return _gameListener;
}

void
GameFeature::load(const archivebuf& reader) noexcept
{
}

void
GameFeature::save(archivebuf& write) noexcept
{
}

void
GameFeature::sendMessage(const MessagePtr& message) except
{
	assert(message);

	auto& features = this->getGameServer()->getGameFeatures();
	for (auto& it : features)
	{
		if (it.get() != this)
		{
			if (it->getActive())
				it->onMessage(message);
		}
	}

	auto& scene = this->getGameServer()->getScenes();
	for (auto& it : scene)
	{
		it->sendMessage(message);
	}
}

void
GameFeature::_setGameServer(GameServer* server) noexcept
{
	_gameServer = server;
}

GameServer*
GameFeature::getGameServer() noexcept
{
	return _gameServer;
}

GameComponentPtr
GameFeature::onSerialization(iarchive&) except
{
	return nullptr;
}

void
GameFeature::onActivate() except
{
}

void
GameFeature::onDeactivate() noexcept
{
}

void
GameFeature::onListenerChangeBefore() noexcept
{
}

void
GameFeature::onListenerChangeAfter() noexcept
{
}

void
GameFeature::onOpenScene(const GameScenePtr& scene) except
{
}

void
GameFeature::onCloseScene(const GameScenePtr& scene) noexcept
{
}

void
GameFeature::onReset() except
{
}

void
GameFeature::onFrameBegin() except
{
}

void
GameFeature::onFrame() except
{
}

void
GameFeature::onFrameEnd() except
{
}

_NAME_END
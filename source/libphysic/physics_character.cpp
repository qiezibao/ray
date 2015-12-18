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
#include <ray/physics_character.h>
#include <ray/physics_scene.h>

#include <BulletDynamics\Character\btKinematicCharacterController.h>
#include <BulletCollision\CollisionShapes\btCapsuleShape.h>
#include <BulletCollision\CollisionDispatch\btGhostObject.h>

_NAME_BEGIN

class CharacterController : public btKinematicCharacterController
{
public:
    CharacterController(btPairCachingGhostObject* ghostObject, btConvexShape* convexShape, btScalar stepHeight) noexcept
        : btKinematicCharacterController(ghostObject, convexShape, stepHeight)
    {
    }

    ~CharacterController() noexcept
    {
    }

    virtual void setStepHeight(float height) noexcept
    {
        m_stepHeight = height;
    }

    virtual bool wasJumping() const noexcept
    {
        return m_wasJumping;
    }

    virtual void setJumpSpeed(float speed)
    {
        m_jumpSpeed = speed;
    }

    virtual void jump() noexcept
    {
        m_verticalVelocity = m_jumpSpeed;
        m_wasJumping = true;
    }
};

PhysicsCharacter::PhysicsCharacter() noexcept
    : _ghostObject(nullptr)
    , _character(nullptr)
    , _capsule(nullptr)
    , _scene(nullptr)
    , _stepHeight(1)
    , _radius(1)
    , _height(1)
    , _walkDirection(Vector3::Zero)
{
}

PhysicsCharacter::~PhysicsCharacter() noexcept
{
}

void
PhysicsCharacter::setup() noexcept
{
    assert(!_character);

    _ghostObject = new btPairCachingGhostObject;

    _capsule = new btCapsuleShape(_radius, _height);
    _ghostObject->setCollisionShape(_capsule);
    _ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    _character = new CharacterController(_ghostObject, _capsule, _stepHeight);
}

void
PhysicsCharacter::close() noexcept
{
    if (_character)
    {
        this->setPhysicsScene(nullptr);
        delete _character;
        _character = nullptr;
    }

    if (_ghostObject)
    {
        delete _ghostObject;
        _ghostObject = nullptr;
    }

    if (_capsule)
    {
        delete _capsule;
        _capsule = nullptr;
    }
}

void
PhysicsCharacter::setRadius(float radius) noexcept
{
    _radius = radius;
}

void
PhysicsCharacter::setHeight(float height) noexcept
{
    _height = height;
}

void
PhysicsCharacter::setMovePosition(const Vector3& pos) noexcept
{
    if (_character)
    {
        btVector3 origin;
        origin.setX(pos.x);
        origin.setY(pos.y);
        origin.setZ(pos.z);

        _character->warp(origin);
    }

    _translate = pos;
}

void
PhysicsCharacter::setWalkDirection(const Vector3& direction) noexcept
{
    if (_character)
    {
        btVector3 walkDirection;
        walkDirection.setX(direction.x);
        walkDirection.setY(direction.y);
        walkDirection.setZ(direction.z);

        _character->setWalkDirection(walkDirection);

        _walkDirection = direction;
    }
}

const Vector3&
PhysicsCharacter::getWalkDirection() const noexcept
{
    return _walkDirection;
}

const Vector3&
PhysicsCharacter::getMovePosition() const noexcept
{
    if (_character)
    {
        auto transform = _character->getGhostObject()->getWorldTransform();
        auto origin = transform.getOrigin();

        _translate.x = origin.x();
        _translate.y = origin.y();
        _translate.z = origin.z();
    }

    return _translate;
}

bool
PhysicsCharacter::canJumping() const noexcept
{
    return _character->onGround() && !this->wasJumping();
}

bool
PhysicsCharacter::wasJumping() const noexcept
{
    assert(_character);
    return _character->wasJumping();
}

void
PhysicsCharacter::jump(float speed) noexcept
{
    assert(_character);
    _character->setJumpSpeed(10);
    _character->jump();
}

void
PhysicsCharacter::setPhysicsScene(PhysicsScene* scene) noexcept
{
    assert(_character);

    if (_scene)
    {
        _scene->removeAction(_character);
        _scene->removeCharacter(_ghostObject);
    }

    _scene = scene;

    if (_scene)
    {
        _scene->addAction(_character);
        _scene->addCharacter(_ghostObject);
    }
}

_NAME_END
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
#include <ray/graphics_shader.h>

_NAME_BEGIN

__ImplementSubInterface(GraphicsParam, rtti::Interface, "GraphicsParam")
__ImplementSubInterface(GraphicsAttribute, GraphicsParam, "GraphicsAttribute")
__ImplementSubInterface(GraphicsUniform, GraphicsParam, "GraphicsUniform")
__ImplementSubInterface(GraphicsUniformBlock, GraphicsParam, "GraphicsUniformBlock")
__ImplementSubInterface(GraphicsShader, GraphicsChild, "GraphicsShader")
__ImplementSubInterface(GraphicsProgram, GraphicsChild, "GraphicsProgram")

GraphicsShaderDesc::GraphicsShaderDesc() noexcept
	:_type(GraphicsShaderStage::GraphicsShaderStageNone)
{
}

GraphicsShaderDesc::GraphicsShaderDesc(GraphicsShaderStage type, const std::string& code) noexcept
{
	this->setType(type);
	_bytecodes.insert(_bytecodes.begin(), code.begin(), code.end());
}

GraphicsShaderDesc::GraphicsShaderDesc(GraphicsShaderStage type, const std::vector<char>& code) noexcept
{
	this->setType(type);
	this->setByteCodes(code);
}

GraphicsShaderDesc::~GraphicsShaderDesc() noexcept
{
}

void
GraphicsShaderDesc::setType(GraphicsShaderStage type) noexcept
{
	_type = type;
}

GraphicsShaderStage
GraphicsShaderDesc::getType()const noexcept
{
	return _type;
}

void
GraphicsShaderDesc::setByteCodes(const std::vector<char>& codes) noexcept
{
	_bytecodes = codes;
}

const std::vector<char>&
GraphicsShaderDesc::getByteCodes() const noexcept
{
	return _bytecodes;
}

GraphicsProgramDesc::GraphicsProgramDesc() noexcept
{
}

GraphicsProgramDesc::~GraphicsProgramDesc() noexcept
{
}

bool
GraphicsProgramDesc::addShader(GraphicsShaderPtr shader) noexcept
{
	assert(shader);

	auto comp = [&](const GraphicsShaderPtr& it) 
	{
		return it->getGraphicsShaderDesc().getType() == shader->getGraphicsShaderDesc().getType();
	};

	auto it = std::find_if(_shaders.begin(), _shaders.end(), comp);
	if (it != _shaders.end())
		return false;

	_shaders.push_back(shader);
	return true;
}

void
GraphicsProgramDesc::removeShader(GraphicsShaderPtr shader) noexcept
{
	auto it = std::find(_shaders.begin(), _shaders.end(), shader);
	if (it != _shaders.end())
		_shaders.erase(it);
}

const GraphicsShaders&
GraphicsProgramDesc::getShaders() const noexcept
{
	return _shaders;
}

GraphicsParam::GraphicsParam() noexcept
{
}

GraphicsParam::~GraphicsParam() noexcept
{
}

GraphicsAttribute::GraphicsAttribute() noexcept
{
}

GraphicsAttribute::~GraphicsAttribute() noexcept
{
}

GraphicsUniform::GraphicsUniform() noexcept
{
}

GraphicsUniform::~GraphicsUniform() noexcept
{
}

GraphicsUniformBlock::GraphicsUniformBlock() noexcept
{
}

GraphicsUniformBlock::~GraphicsUniformBlock() noexcept
{
}

GraphicsShader::GraphicsShader() noexcept
{
}

GraphicsShader::~GraphicsShader() noexcept
{
}

GraphicsProgram::GraphicsProgram() noexcept
{
}

GraphicsProgram::~GraphicsProgram() noexcept
{
}

_NAME_END
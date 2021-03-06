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
#ifndef _H_MUTEX_H_
#define _H_MUTEX_H_

#include <ray/platform.h>

#include <mutex>
#include <thread>

_NAME_BEGIN

template<typename _Mutex>
class reentrant_lock
{
public:
	typedef typename _Mutex::native_handle_type native_handle_type;

	reentrant_lock(_Mutex& mutex, bool is_lock = true)
		:_mutex(&mutex)
		, _count(0)
	{
		if (is_lock)
		{
			this->lock();
		}
	}

	~reentrant_lock()
	{
		this->unlock();
	}

	bool try_lock()
	{
		return _mutex->try_lock();
	}

	native_handle_type native_handle()
	{
		return _mutex->native_handle();
	}

	void lock()
	{
		std::thread::id hash = std::this_thread::get_id();
		if (_thread_id == hash && _count > 0)
			++_count;
		else
		{
			_mutex->lock();
			_thread_id = hash;
			_count = 1;
		}
	}

	void unlock()
	{
		if (_count > 0)
		{
			--_count;
			if (_count == 0)
			{
				_thread_id = 0;
				_mutex->unlock();
			}
		}
	}

private:
	_Mutex* _mutex;
	std::thread::id _thread_id;
	std::size_t _count;
};

_NAME_END

#endif
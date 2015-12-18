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
#if defined(_BUILD_PLATFORM_WINDOWS)
#include <ray/msw_input_device.h>

_NAME_BEGIN

__ImplementSubInterface(MSWInputDevice, InputDevice, "MSWInputDevice")

InputKey::Code VirtualKeyToScanCode(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
	UINT flags = HIWORD(lParam);

	if (flags & 0x100)
	{
		switch (MapVirtualKey(flags & 0xFF, 1))
		{
		case VK_INSERT:   return InputKey::Code::INSERT;
		case VK_END:      return InputKey::Code::END;
		case VK_DOWN:     return InputKey::Code::DOWN;
		case VK_NEXT:     return InputKey::Code::PGDN;
		case VK_LEFT:     return InputKey::Code::LEFT;
		case VK_CLEAR:    return InputKey::Code::CLEAR;
		case VK_RIGHT:    return InputKey::Code::RIGHT;
		case VK_HOME:     return InputKey::Code::HOME;
		case VK_UP:       return InputKey::Code::UP;
		case VK_PRIOR:    return InputKey::Code::PGUP;
		case VK_DIVIDE:   return InputKey::Code::DIVIDE;
		case VK_MULTIPLY: return InputKey::Code::MULTIPLY;
		case VK_SUBTRACT: return InputKey::Code::SUB;
		case VK_ADD:      return InputKey::Code::ADD;
		case VK_DELETE:   return InputKey::Code::DELETE;
		}
	}

	DWORD scan_code;

	switch (wParam)
	{
	case VK_SHIFT:
	{
		scan_code = MapVirtualKey(VK_RSHIFT, 0);
		if ((DWORD)(lParam & 0x01ff0000 >> 16) == scan_code)
		{
			return InputKey::Code::RSHIFT;
		}

		return InputKey::Code::LSHIFT;
	}
	case VK_CONTROL:
	{
		if (lParam & 0x01000000)
		{
			return InputKey::Code::RCTRL;
		}

		DWORD time = (DWORD)::GetMessageTime();

		MSG msg;
		if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
		{
			if (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
			{
				if (msg.wParam == VK_MENU &&
					msg.lParam & 0x01000000 &&
					msg.time == time)
				{
					return InputKey::Code::None;
				}
			}
		}

		return InputKey::Code::LCTRL;
	}

	case VK_MENU:
	{
		if (lParam & 0x01000000)
			return InputKey::Code::RALT;

		return InputKey::Code::LALT;
	}

	case VK_RETURN:
	{
		if (lParam & 0x01000000)
		{
			return InputKey::Code::ENTER;
		}

		return InputKey::Code::ENTER;
	}
	case VK_ESCAPE:        return InputKey::Code::ESCAPE;
	case VK_TAB:           return InputKey::Code::TAB;
	case VK_BACK:          return InputKey::Code::BACKSPACE;
	case VK_HOME:          return InputKey::Code::HOME;
	case VK_END:           return InputKey::Code::END;
	case VK_PRIOR:         return InputKey::Code::PGUP;
	case VK_NEXT:          return InputKey::Code::PGDN;
	case VK_INSERT:        return InputKey::Code::INSERT;
	case VK_DELETE:        return InputKey::Code::DELETE;
	case VK_LEFT:          return InputKey::Code::LEFT;
	case VK_UP:            return InputKey::Code::UP;
	case VK_RIGHT:         return InputKey::Code::RIGHT;
	case VK_DOWN:          return InputKey::Code::DOWN;
	case VK_F1:            return InputKey::Code::F1;
	case VK_F2:            return InputKey::Code::F2;
	case VK_F3:            return InputKey::Code::F3;
	case VK_F4:            return InputKey::Code::F4;
	case VK_F5:            return InputKey::Code::F5;
	case VK_F6:            return InputKey::Code::F6;
	case VK_F7:            return InputKey::Code::F7;
	case VK_F8:            return InputKey::Code::F8;
	case VK_F9:            return InputKey::Code::F9;
	case VK_F10:           return InputKey::Code::F10;
	case VK_F11:           return InputKey::Code::F11;
	case VK_F12:           return InputKey::Code::F12;
	case VK_F13:           return InputKey::Code::F13;
	case VK_F14:           return InputKey::Code::F14;
	case VK_F15:           return InputKey::Code::F15;
	case VK_F16:           return InputKey::Code::F16;
	case VK_F17:           return InputKey::Code::F17;
	case VK_F18:           return InputKey::Code::F18;
	case VK_F19:           return InputKey::Code::F19;
	case VK_F20:           return InputKey::Code::F20;
	case VK_F21:           return InputKey::Code::F21;
	case VK_F22:           return InputKey::Code::F22;
	case VK_F23:           return InputKey::Code::F23;
	case VK_F24:           return InputKey::Code::F24;

	case VK_SPACE:         return InputKey::Code::SPACE;

	case VK_NUMPAD0:       return InputKey::Code::NP_0;
	case VK_NUMPAD1:       return InputKey::Code::NP_1;
	case VK_NUMPAD2:       return InputKey::Code::NP_2;
	case VK_NUMPAD3:       return InputKey::Code::NP_3;
	case VK_NUMPAD4:       return InputKey::Code::NP_4;
	case VK_NUMPAD5:       return InputKey::Code::NP_5;
	case VK_NUMPAD6:       return InputKey::Code::NP_6;
	case VK_NUMPAD7:       return InputKey::Code::NP_7;
	case VK_NUMPAD8:       return InputKey::Code::NP_8;
	case VK_NUMPAD9:       return InputKey::Code::NP_9;

	case VK_DIVIDE:        return InputKey::Code::DIVIDE;
	case VK_MULTIPLY:      return InputKey::Code::MULTIPLY;
	case VK_SUBTRACT:      return InputKey::Code::SUBTRACT;
	case VK_ADD:           return InputKey::Code::ADD;
	case VK_DECIMAL:       return InputKey::Code::DECIMAL;
	case VK_NUMLOCK:       return InputKey::Code::NUMLOCK;

	case VK_CAPITAL:       return InputKey::Code::CAPSLOCK;
	case VK_SCROLL:        return InputKey::Code::SCROLLLOCK;
	case VK_PAUSE:         return InputKey::Code::PAUSE;

	case VK_LWIN:          return InputKey::Code::LWIN;
	case VK_RWIN:          return InputKey::Code::RWIN;
	case VK_APPS:          return InputKey::Code::APPS;
	}

	if (wParam >= 'a' && wParam <= 'z')
	{
		return (InputKey::Code)(InputKey::Code::A + wParam - 'a');
	}

	if (wParam >= 'A' && wParam <= 'Z')
	{
		return (InputKey::Code)(InputKey::Code::A + wParam - 'A');
	}

	if (wParam >= '0' && wParam <= '9')
	{
		return (InputKey::Code)(InputKey::Code::Key0 + wParam - '0');
	}

	return InputKey::Code::None;
}

int VirtualKeyToText(WPARAM _virtualKey)
{
	static WCHAR deadKey = 0;

	BYTE keyState[256];
	HKL  layout = GetKeyboardLayout(0);
	if (GetKeyboardState(keyState) == 0)
		return 0;

	WCHAR buff[3] = { 0, 0, 0 };
	int ascii = ToUnicodeEx((UINT)_virtualKey, 0, keyState, buff, 3, 0, layout);
	if (ascii == 1 && deadKey != '\0')
	{
		// A dead key is stored and we have just converted a character key
		// Combine the two into a single character
		WCHAR wcBuff[3] = { buff[0], deadKey, '\0' };
		WCHAR out[3];

		deadKey = '\0';
		if (FoldStringW(MAP_PRECOMPOSED, (LPWSTR)wcBuff, 3, (LPWSTR)out, 3))
			return out[0];
	}
	else if (ascii == 1)
	{
		// We have a single character
		deadKey = '\0';
		return buff[0];
	}
	else if (ascii == 2)
	{
		// Convert a non-combining diacritical mark into a combining diacritical mark
		// Combining versions range from 0x300 to 0x36F; only 5 (for French) have been mapped below
		// http://www.fileformat.info/info/unicode/block/combining_diacritical_marks/images.htm
		switch (buff[0])
		{
		case 0x5E: // Circumflex accent: ��
			deadKey = 0x302;
			break;
		case 0x60: // Grave accent: ��
			deadKey = 0x300;
			break;
		case 0xA8: // Diaeresis: ��
			deadKey = 0x308;
			break;
		case 0xB4: // Acute accent: ��
			deadKey = 0x301;
			break;
		case 0xB8: // Cedilla: ��
			deadKey = 0x327;
			break;
		default:
			deadKey = buff[0];
			break;
		}
	}
	return 0;
}

MSWInputDevice::MSWInputDevice() noexcept
	: _window(nullptr)
{
}

void
MSWInputDevice::setCaptureObject(CaptureObject window) noexcept
{
	assert(::IsWindow(window));
	_window = (HWND)window;
}

CaptureObject
MSWInputDevice::getCaptureObject() const noexcept
{
	return _window;
}

void
MSWInputDevice::update() noexcept
{
	MSG msg;

	while (::PeekMessage(&msg, _window, 0, 0, PM_REMOVE | PM_QS_INPUT))
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			auto inputEvent = make_message<InputEvent>();

			inputEvent->event = (msg.message == WM_KEYDOWN) ? InputEvent::KeyDown : InputEvent::KeyUp;
			inputEvent->key.timestamp = ::timeGetTime();
			inputEvent->key.state = (msg.message == WM_KEYDOWN) ? true : false;

			if (msg.wParam == VK_PROCESSKEY)
			{
				UINT virtualKey = ::ImmGetVirtualKey(msg.hwnd);
				if (virtualKey != VK_PROCESSKEY)
				{
					inputEvent->key.keysym.raw = virtualKey;
					inputEvent->key.keysym.sym = VirtualKeyToScanCode(msg.hwnd, virtualKey, msg.lParam);
					if (msg.message == WM_KEYDOWN)
						inputEvent->key.keysym.unicode = VirtualKeyToText(virtualKey);
					else
						inputEvent->key.keysym.unicode = 0;
				}
			}
			else
			{
				inputEvent->key.keysym.raw = msg.wParam;
				inputEvent->key.keysym.sym = VirtualKeyToScanCode(_window, msg.wParam, msg.lParam);
				if (msg.message == WM_KEYDOWN)
					inputEvent->key.keysym.unicode = VirtualKeyToText(msg.wParam);
				else
					inputEvent->key.keysym.unicode = 0;
			}

			this->postEvent(inputEvent);
		}
		break;
		case WM_MOUSEMOVE:
		{
			POINT pt;
			GetCursorPos(&pt);

			auto inputEvent = make_message<InputEvent>();

			inputEvent->event = InputEvent::MouseMotion;
			inputEvent->motion.x = LOWORD(msg.lParam);
			inputEvent->motion.y = HIWORD(msg.lParam);
			inputEvent->motion.xrel = pt.x;
			inputEvent->motion.yrel = pt.y;
			inputEvent->motion.state = _isButtonPress;
			inputEvent->motion.timestamp = ::timeGetTime();
			inputEvent->button.button = _button;

			_mouseX = LOWORD(msg.lParam);
			_mouseY = HIWORD(msg.lParam);

			this->postEvent(inputEvent);
		}
		break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{
			SetCapture(msg.hwnd);

			auto inputEvent = make_message<InputEvent>();

			inputEvent->event = InputEvent::MouseButtonDown;
			inputEvent->button.clicks = true;
			inputEvent->button.x = _mouseX;
			inputEvent->button.y = _mouseY;
			inputEvent->button.timestamp = ::timeGetTime();
			switch (msg.message)
			{
			case WM_LBUTTONDOWN:
				inputEvent->button.button = InputButton::LEFT;
				break;
			case WM_MBUTTONDOWN:
				inputEvent->button.button = InputButton::MIDDLE;
				break;
			case WM_RBUTTONDOWN:
				inputEvent->button.button = InputButton::RIGHT;
				break;
			default:
				assert(false);
			}

			_isButtonPress = true;
			_button = inputEvent->button.button;

			this->postEvent(inputEvent);
		}
		break;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		{
			ReleaseCapture();

			auto inputEvent = make_message<InputEvent>();

			inputEvent->event = InputEvent::MouseButtonUp;
			inputEvent->button.clicks = false;
			inputEvent->button.x = _mouseX;
			inputEvent->button.y = _mouseY;
			inputEvent->button.timestamp = ::timeGetTime();
			switch (msg.message)
			{
			case WM_LBUTTONUP:
				inputEvent->button.button = InputButton::LEFT;
				break;
			case WM_MBUTTONUP:
				inputEvent->button.button = InputButton::MIDDLE;
				break;
			case WM_RBUTTONUP:
				inputEvent->button.button = InputButton::RIGHT;
				break;
			default:
				assert(false);
			}

			_isButtonPress = false;
			_button = inputEvent->button.button;

			this->postEvent(inputEvent);
		}
		break;
		}

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

void
MSWInputDevice::peekEvents(InputEventPtr&) noexcept
{
	this->update();
}

bool
MSWInputDevice::pollEvents(InputEventPtr&) noexcept
{
	this->update();
	return true;
}

bool
MSWInputDevice::waitEvents(InputEventPtr&) noexcept
{
	this->update();
	return true;
}

bool
MSWInputDevice::waitEvents(InputEventPtr&, int) noexcept
{
	this->update();
	return true;
}

void
MSWInputDevice::flushEvent() noexcept
{
	this->update();
}

_NAME_END

#endif
#include <pch.h>
#include "KeyCode.h"

KEY_CODE& operator++(KEY_CODE& e) {
	using enum KEY_CODE;
	switch (e)
	{
	case(MOUSE_BUTTON_8):
	{
		e = (KEY_SPACE);
		break;
	}
	case(KEY_SPACE):
	{
		e = (KEY_APOSTROPHE);
		break;
	}
	case(KEY_APOSTROPHE):
	{
		e = (KEY_COMMA);
		break;
	}
	case(KEY_9):
	{
		e = (KEY_SEMICOLON);
		break;
	}
	case(KEY_SEMICOLON):
	{
		e = (KEY_EQUAL);
		break;
	}
	case(KEY_EQUAL):
	{
		e = (KEY_A);
		break;
	}
	case(KEY_RIGHT_BRACKET):
	{
		e = (KEY_GRAVE_ACCENT);
		break;
	}
	case(KEY_GRAVE_ACCENT):
	{
		e = (KEY_WORLD_1);
		break;
	}
	case(KEY_WORLD_2):
	{
		e = (KEY_ESCAPE);
		break;
	}
	case(KEY_END):
	{
		e = (KEY_CAPS_LOCK);
		break;
	}
	case(KEY_PAUSE):
	{
		e = (KEY_F1);
		break;
	}
	case(KEY_F25):
	{
		e = (KEY_KP_0);
		break;
	}
	case(KEY_KP_EQUAL):
	{
		e = (KEY_LEFT_SHIFT);
		break;
	}
	case(KEY_COUNTS):
	{
		e = (MOUSE_BUTTON_1);
		break;
	}
	default:
	{
		e = static_cast<KEY_CODE>(static_cast<int>(e) + 1);
		break;
	}
	}
	return e;
}

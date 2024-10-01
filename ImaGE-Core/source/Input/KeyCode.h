#pragma once
/*!*********************************************************************
\file   KeyCode.h
\author han.q\@digipen.edu
\date   17-September-2024
\brief
	Keycode enums for Input Manager. The value of the key enums will be the same as 
	GLFW defined keys

Copyright (C) 2024 DigiPen Institute of Technology. All rights reserved.
************************************************************************/
//#include <pch.h>
#include <glad/glad.h> // for access to OpenGL API declarations
#include <GLFW/glfw3.h>

/*!******************************************************************
Define/Enums for Mouse Input
********************************************************************/

#define IK_MOUSE1												KEY_CODE::MOUSE_BUTTON_1
#define IK_MOUSE2												KEY_CODE::MOUSE_BUTTON_2
#define IK_MOUSE3												KEY_CODE::MOUSE_BUTTON_3
#define IK_MOUSE4												KEY_CODE::MOUSE_BUTTON_4
#define IK_MOUSE5												KEY_CODE::MOUSE_BUTTON_5
#define IK_MOUSE6												KEY_CODE::MOUSE_BUTTON_6
#define IK_MOUSE7												KEY_CODE::MOUSE_BUTTON_7
#define IK_MOUSE8												KEY_CODE::MOUSE_BUTTON_8
#define IK_MOUSE_LAST										IK_MOUSE8
#define IK_MOUSE_LEFT										IK_MOUSE1
#define IK_MOUSE_RIGHT									IK_MOUSE2
#define IK_MOUSE_MIDDLE									IK_MOUSE3

/*!*****************************************************************
Define/Enums for Basic keys
********************************************************************/
#define IK_SPACE           KEY_CODE::KEY_SPACE
#define IK_APOSTROPHE      KEY_CODE::KEY_APOSTROPHE      /* ' */
#define IK_COMMA           KEY_CODE::KEY_COMMA           /* , */
#define IK_MINUS           KEY_CODE::KEY_MINUS           /* - */
#define IK_PERIOD          KEY_CODE::KEY_PERIOD          /* . */
#define IK_SLASH           KEY_CODE::KEY_SLASH           /* / */
#define IK_0               KEY_CODE::KEY_0
#define IK_1               KEY_CODE::KEY_1
#define IK_2               KEY_CODE::KEY_2
#define IK_3               KEY_CODE::KEY_3
#define IK_4               KEY_CODE::KEY_4
#define IK_5               KEY_CODE::KEY_5
#define IK_6               KEY_CODE::KEY_6
#define IK_7               KEY_CODE::KEY_7
#define IK_8               KEY_CODE::KEY_8
#define IK_9               KEY_CODE::KEY_9
#define IK_SEMICOLON       KEY_CODE::KEY_SEMICOLON      /* ; */
#define IK_EQUAL           KEY_CODE::KEY_EQUAL          /* = */
#define IK_A               KEY_CODE::KEY_A
#define IK_B               KEY_CODE::KEY_B
#define IK_C               KEY_CODE::KEY_C
#define IK_D               KEY_CODE::KEY_D
#define IK_E               KEY_CODE::KEY_E
#define IK_F               KEY_CODE::KEY_F
#define IK_G               KEY_CODE::KEY_G
#define IK_H               KEY_CODE::KEY_H
#define IK_I               KEY_CODE::KEY_I
#define IK_J               KEY_CODE::KEY_J
#define IK_K               KEY_CODE::KEY_K
#define IK_L               KEY_CODE::KEY_L
#define IK_M               KEY_CODE::KEY_M
#define IK_N               KEY_CODE::KEY_N
#define IK_O               KEY_CODE::KEY_O
#define IK_P               KEY_CODE::KEY_P
#define IK_Q               KEY_CODE::KEY_Q
#define IK_R               KEY_CODE::KEY_R
#define IK_S               KEY_CODE::KEY_S
#define IK_T               KEY_CODE::KEY_T
#define IK_U               KEY_CODE::KEY_U
#define IK_V               KEY_CODE::KEY_V
#define IK_W               KEY_CODE::KEY_W
#define IK_X               KEY_CODE::KEY_X
#define IK_Y               KEY_CODE::KEY_Y
#define IK_Z               KEY_CODE::KEY_Z
#define IK_LEFT_BRACKET    KEY_CODE::KEY_LEFT_BRACKET    /* [ */
#define IK_BACKSLASH       KEY_CODE::KEY_BACKSLASH       /* \ */
#define IK_RIGHT_BRACKET   KEY_CODE::KEY_RIGHT_BRACKET   /* ] */
#define IK_GRAVE_ACCENT    KEY_CODE::KEY_GRAVE_ACCENT    /* ` */
#define IK_WORLD_1         KEY_CODE::KEY_WORLD_1         /* non-US #1 */
#define IK_WORLD_2         KEY_CODE::KEY_WORLD_2         /* non-US #2 */

/*!******************************************************************
Define/Enum for Function Keys
********************************************************************/
#define IK_ESCAPE          KEY_CODE::KEY_ESCAPE
#define IK_ENTER           KEY_CODE::KEY_ENTER
#define IK_TAB             KEY_CODE::KEY_TAB
#define IK_BACKSPACE       KEY_CODE::KEY_BACKSPACE
#define IK_INSERT          KEY_CODE::KEY_INSERT
#define IK_DELETE          KEY_CODE::KEY_DELETE
#define IK_RIGHT           KEY_CODE::KEY_RIGHT
#define IK_LEFT            KEY_CODE::KEY_LEFT
#define IK_DOWN            KEY_CODE::KEY_DOWN
#define IK_UP              KEY_CODE::KEY_UP
#define IK_PAGE_UP         KEY_CODE::KEY_PAGE_UP
#define IK_PAGE_DOWN       KEY_CODE::KEY_PAGE_DOWN
#define IK_HOME            KEY_CODE::KEY_HOME
#define IK_END             KEY_CODE::KEY_END
#define IK_CAPS_LOCK       KEY_CODE::KEY_CAPS_LOCK
#define IK_SCROLL_LOCK     KEY_CODE::KEY_SCROLL_LOCK
#define IK_NUM_LOCK        KEY_CODE::KEY_NUM_LOCK
#define IK_PRINT_SCREEN    KEY_CODE::KEY_PRINT_SCREEN
#define IK_PAUSE           KEY_CODE::KEY_PAUSE
#define IK_F1              KEY_CODE::KEY_F1
#define IK_F2              KEY_CODE::KEY_F2
#define IK_F3              KEY_CODE::KEY_F3
#define IK_F4              KEY_CODE::KEY_F4
#define IK_F5              KEY_CODE::KEY_F5
#define IK_F6              KEY_CODE::KEY_F6
#define IK_F7              KEY_CODE::KEY_F7
#define IK_F8              KEY_CODE::KEY_F8
#define IK_F9              KEY_CODE::KEY_F9
#define IK_F10             KEY_CODE::KEY_F10
#define IK_F11             KEY_CODE::KEY_F11
#define IK_F12             KEY_CODE::KEY_F12
#define IK_F13             KEY_CODE::KEY_F13
#define IK_F14             KEY_CODE::KEY_F14
#define IK_F15             KEY_CODE::KEY_F15
#define IK_F16             KEY_CODE::KEY_F16
#define IK_F17             KEY_CODE::KEY_F17
#define IK_F18             KEY_CODE::KEY_F18
#define IK_F19             KEY_CODE::KEY_F19
#define IK_F20             KEY_CODE::KEY_F20
#define IK_F21             KEY_CODE::KEY_F21
#define IK_F22             KEY_CODE::KEY_F22
#define IK_F23             KEY_CODE::KEY_F23
#define IK_F24             KEY_CODE::KEY_F24
#define IK_F25             KEY_CODE::KEY_F25
#define IK_KP_0            KEY_CODE::KEY_KP_0
#define IK_KP_1            KEY_CODE::KEY_KP_1
#define IK_KP_2            KEY_CODE::KEY_KP_2
#define IK_KP_3            KEY_CODE::KEY_KP_3
#define IK_KP_4            KEY_CODE::KEY_KP_4
#define IK_KP_5            KEY_CODE::KEY_KP_5
#define IK_KP_6            KEY_CODE::KEY_KP_6
#define IK_KP_7            KEY_CODE::KEY_KP_7
#define IK_KP_8            KEY_CODE::KEY_KP_8
#define IK_KP_9            KEY_CODE::KEY_KP_9
#define IK_DECIMAL         KEY_CODE::KP_DECIMAL

#define IK_KP_DIVIDE				KEY_CODE::KEY_KP_DIVIDE
#define IK_KP_MULTIPLY			KEY_CODE::KEY_KP_MULTIPLY
#define IK_KP_SUBTRACT			KEY_CODE::KEY_KP_SUBTRACT
#define IK_KP_ADD					KEY_CODE::KEY_KP_ADD
#define IK_KP_ENTER				KEY_CODE::KEY_KP_ENTER
#define IK_KP_EQUAL				KEY_CODE::KEY_KP_EQUAL
#define IK_LEFT_SHIFT			KEY_CODE::KEY_LEFT_SHIFT
#define IK_LEFT_CONTROL		KEY_CODE::KEY_LEFT_CONTROL
#define IK_LEFT_ALT				KEY_CODE::KEY_LEFT_ALT
#define IK_LEFT_SUPER			KEY_CODE::KEY_LEFT_SUPER
#define IK_RIGHT_SHIFT			KEY_CODE::KEY_RIGHT_SHIFT
#define IK_RIGHT_CONTROL		KEY_CODE::KEY_RIGHT_CONTROL
#define IK_RIGHT_SUPER			KEY_CODE::KEY_RIGHT_SUPER
#define IK_KEY_COUNT       KEY_CODE::KEY_COUNTS



enum class KEY_CODE
{
	/*!******************************************************************
	Enums for Mouse Input
	********************************************************************/

	MOUSE_BUTTON_1 = GLFW_MOUSE_BUTTON_1,
	MOUSE_BUTTON_2,
	MOUSE_BUTTON_3,
	MOUSE_BUTTON_4,
	MOUSE_BUTTON_5,
	MOUSE_BUTTON_6,
	MOUSE_BUTTON_7,
	MOUSE_BUTTON_8,
	//MOUSE_BUTTON_LAST      MOUSE_BUTTON_8
	//MOUSE_BUTTON_LEFT      MOUSE_BUTTON_1
	//MOUSE_BUTTON_RIGHT     MOUSE_BUTTON_2
	//MOUSE_BUTTON_MIDDLE    MOUSE_BUTTON_3

	/*!*****************************************************************
	Enums for Basic keys
	********************************************************************/
	KEY_SPACE = GLFW_KEY_SPACE,
	KEY_APOSTROPHE = GLFW_KEY_APOSTROPHE,  /* ' */
	KEY_COMMA = GLFW_KEY_COMMA,  /* , */
	KEY_MINUS,						 /* - */
	KEY_PERIOD,            /* . */
	KEY_SLASH,             /* / */
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_SEMICOLON = GLFW_KEY_SEMICOLON,  /* ; */
	KEY_EQUAL = GLFW_KEY_EQUAL,  /* = */
	KEY_A = GLFW_KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_LEFT_BRACKET,						/* [ */
	KEY_BACKSLASH,							/* \ */
	KEY_RIGHT_BRACKET,					/* ] */
	KEY_GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,  /* ` */
	KEY_WORLD_1 = GLFW_KEY_WORLD_1, /* non-US #1 */
	KEY_WORLD_2,							 /* non-US #2 */


	/*!******************************************************************
	Enums for Function Keys
	********************************************************************/
	KEY_ESCAPE = GLFW_KEY_ESCAPE,
	KEY_ENTER,
	KEY_TAB,
	KEY_BACKSPACE,
	KEY_INSERT,
	KEY_DELETE,
	KEY_RIGHT,
	KEY_LEFT,
	KEY_DOWN,
	KEY_UP,
	KEY_PAGE_UP,
	KEY_PAGE_DOWN,
	KEY_HOME,
	KEY_END,
	KEY_CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
	KEY_SCROLL_LOCK,
	KEY_NUM_LOCK,
	KEY_PRINT_SCREEN,
	KEY_PAUSE,
	KEY_F1 = GLFW_KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_F13,
	KEY_F14,
	KEY_F15,
	KEY_F16,
	KEY_F17,
	KEY_F18,
	KEY_F19,
	KEY_F20,
	KEY_F21,
	KEY_F22,
	KEY_F23,
	KEY_F24,
	KEY_F25,
	KEY_KP_0 = GLFW_KEY_KP_0,
	KEY_KP_1,
	KEY_KP_2,
	KEY_KP_3,
	KEY_KP_4,
	KEY_KP_5,
	KEY_KP_6,
	KEY_KP_7,
	KEY_KP_8,
	KEY_KP_9,
	KEY_KP_DECIMAL,
	KEY_KP_DIVIDE,
	KEY_KP_MULTIPLY,
	KEY_KP_SUBTRACT,
	KEY_KP_ADD,
	KEY_KP_ENTER,
	KEY_KP_EQUAL,
	KEY_LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
	KEY_LEFT_CONTROL,
	KEY_LEFT_ALT,
	KEY_LEFT_SUPER,
	KEY_RIGHT_SHIFT,
	KEY_RIGHT_CONTROL,
	KEY_RIGHT_ALT,
	KEY_RIGHT_SUPER,
	KEY_MENU,

	//KEY_LAST               KEY_MENU

	KEY_COUNTS


};



//KEY_CODE& operator++(KEY_CODE& e) {
//	using enum KEY_CODE;
//	switch (e) 
//	{
//		case(MOUSE_BUTTON_8): 
//		{
//			e = (KEY_SPACE);
//			break;
//		}
//		case(KEY_SPACE):
//		{
//			e = (KEY_APOSTROPHE);
//			break;
//		}
//		case(KEY_APOSTROPHE):
//		{
//			e = (KEY_COMMA);
//			break;
//		}
//		case(KEY_9):
//		{
//			e = (KEY_SEMICOLON);
//			break;
//		}
//		case(KEY_SEMICOLON):
//		{
//			e = (KEY_EQUAL);
//			break;
//		}
//		case(KEY_EQUAL):
//		{
//			e = (KEY_A);
//			break;
//		}
//		case(KEY_RIGHT_BRACKET):
//		{
//			e = (KEY_GRAVE_ACCENT);
//			break;
//		}
//		case(KEY_GRAVE_ACCENT):
//		{
//			e = (KEY_WORLD_1);
//			break;
//		}
//		case(KEY_WORLD_2):
//		{
//			e = (KEY_ESCAPE);
//			break;
//		}
//		case(KEY_END):
//		{
//			e = (KEY_CAPS_LOCK);
//			break;
//		}
//		case(KEY_PAUSE):
//		{
//			e = (KEY_F1);
//			break;
//		}
//		case(KEY_F25):
//		{
//			e = (KEY_KP_0);
//			break;
//		}
//		case(KEY_KP_EQUAL):
//		{
//			e = (KEY_LEFT_SHIFT);
//			break;
//		}
//		case(KEY_COUNTS):
//		{
//			e = (MOUSE_BUTTON_1);
//			break;
//		}
//		default:
//		{
//			e = static_cast<KEY_CODE>(static_cast<int>(e) + 1);
//			break;
//		}
//	}
//	return e;
//}
//

/*
Simple DirectMedia Layer
Java source code (C) 2009-2012 Sergii Pylypenko
  
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:
  
1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required. 
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

package com.octodev.cannonball;

import android.app.Activity;
import android.content.Context;
import java.util.Vector;
import android.view.KeyEvent;

class Globals
{
	public static String ApplicationName = "Cannonball";
	public static String AppLibraries[] = { "sdl-1.2", };
	public static String AppMainLibraries[] = { "cannonball" };
	public static int VideoDepthBpp = 16;
	public static boolean SwVideoMode = false;
	public static boolean NeedDepthBuffer = false;
	public static boolean NeedStencilBuffer = false;
	public static boolean NeedGles2 = false;
	public static boolean CompatibilityHacksVideo = false;
	public static boolean CompatibilityHacksStaticInit = false;
	public static boolean CompatibilityHacksTextInputEmulatesHwKeyboard = false;
	public static boolean HorizontalOrientation = true;
	public static boolean KeepAspectRatioDefaultSetting = false;
	public static boolean InhibitSuspend = false;
	public static boolean CreateService = false;
	public static String CommandLine = "";
	public static boolean AppUsesMouse = false;
	public static boolean AppNeedsTwoButtonMouse = false;
	public static boolean RightMouseButtonLongPress = true;
	public static boolean ForceRelativeMouseMode = false; // If both on-screen keyboard and mouse are needed, this will only set the default setting, user may override it later
	public static boolean ShowMouseCursor = false;
	public static boolean GenerateSubframeTouchEvents = false;
	public static boolean AppNeedsArrowKeys = true;
	public static boolean AppNeedsTextInput = true;
	public static boolean AppUsesJoystick = false;
	public static boolean AppUsesSecondJoystick = false;
	public static boolean AppUsesThirdJoystick = false;
	public static boolean AppUsesAccelerometer = false;
	public static boolean AppUsesGyroscope = false;
	public static boolean AppUsesMultitouch = false;
	public static boolean NonBlockingSwapBuffers = false;
	public static boolean ImmersiveMode = true;
	public static boolean ResetSdlConfigForThisVersion = false;
	public static String DeleteFilesOnUpgrade = "";
	public static int AppTouchscreenKeyboardKeysAmount = 4;
	public static int AppMinimumRAM = 0;

	// Phone-specific config, modified by user in "Change phone config" startup dialog, TODO: move this to settings
	public static boolean DownloadToSdcard = true;
	public static boolean PhoneHasTrackball = false;
	public static boolean PhoneHasArrowKeys = false;
	public static boolean UseAccelerometerAsArrowKeys = false;
	public static boolean UseTouchscreenKeyboard = true;
	public static int TouchscreenKeyboardSize = 1;
	public static final int TOUCHSCREEN_KEYBOARD_CUSTOM = 4;
	public static int TouchscreenKeyboardDrawSize = 1;
	public static int TouchscreenKeyboardTheme = 2;
	public static int TouchscreenKeyboardTransparency = 2;
	public static int AccelerometerSensitivity = 2;
	public static int AccelerometerCenterPos = 2;
	public static int TrackballDampening = 0;
	public static int AudioBufferConfig = 0;
	public static int LeftClickMethod = ForceRelativeMouseMode ? Mouse.LEFT_CLICK_WITH_TAP_OR_TIMEOUT : Mouse.LEFT_CLICK_NORMAL;
	public static int LeftClickKey = KeyEvent.KEYCODE_DPAD_CENTER;
	public static int LeftClickTimeout = 3;
	public static int RightClickTimeout = 4;
	public static int RightClickMethod = AppNeedsTwoButtonMouse ? Mouse.RIGHT_CLICK_WITH_MULTITOUCH : Mouse.RIGHT_CLICK_NONE;
	public static int RightClickKey = KeyEvent.KEYCODE_MENU;
	public static boolean MoveMouseWithJoystick = false;
	public static int MoveMouseWithJoystickSpeed = 0;
	public static int MoveMouseWithJoystickAccel = 0;
	public static boolean ClickMouseWithDpad = false;
	public static boolean RelativeMouseMovement = ForceRelativeMouseMode; // Laptop touchpad mode
	public static int RelativeMouseMovementSpeed = 2;
	public static int RelativeMouseMovementAccel = 0;
	public static int ShowScreenUnderFinger = Mouse.ZOOM_NONE;
	public static boolean KeepAspectRatio = KeepAspectRatioDefaultSetting;
	public static int ClickScreenPressure = 0;
	public static int ClickScreenTouchspotSize = 0;
	public static boolean HoverJitterFilter = true;
	public static int RemapHwKeycode[] = new int[SDL_Keys.JAVA_KEYCODE_LAST];
	public static int RemapScreenKbKeycode[] = new int[6];
	public static int ScreenKbControlsLayout[][] =	AppUsesThirdJoystick ? // Values for 800x480 resolution
													new int[][] { { 0, 303, 177, 480 }, { 0, 0, 48, 48 }, { 400, 392, 488, 480 }, { 312, 392, 400, 480 }, { 400, 304, 488, 392 }, { 312, 304, 400, 392 }, { 400, 216, 488, 304 }, { 312, 216, 400, 304 }, { 623, 303, 800, 480 }, { 623, 126, 800, 303 } } :
													AppUsesSecondJoystick ?
													new int[][] { { 0, 303, 177, 480 }, { 0, 0, 48, 48 }, { 400, 392, 488, 480 }, { 312, 392, 400, 480 }, { 400, 304, 488, 392 }, { 312, 304, 400, 392 }, { 400, 216, 488, 304 }, { 312, 216, 400, 304 }, { 623, 303, 800, 480 } } :
													new int[][] { { 0, 303, 177, 480 }, { 0, 0, 48, 48 }, { 712, 392, 800, 480 }, { 624, 392, 712, 480 }, { 712, 304, 800, 392 }, { 624, 304, 712, 392 }, { 712, 216, 800, 304 }, { 624, 216, 712, 304 } };
	public static boolean ScreenKbControlsShown[] = new boolean[ScreenKbControlsLayout.length]; /* Also joystick and text input button added */
	public static int RemapMultitouchGestureKeycode[] = new int[4];
	public static boolean MultitouchGesturesUsed[] = new boolean[4];
	public static int MultitouchGestureSensitivity = 1;
	public static int TouchscreenCalibration[] = new int[4];
	public static String DataDir = new String("");
	public static boolean VideoLinearFilter = true;
	public static boolean MultiThreadedVideo = false;
	public static boolean BrokenLibCMessageShown = false;
	// Gyroscope calibration
	public static float gyro_x1, gyro_x2, gyro_xc, gyro_y1, gyro_y2, gyro_yc, gyro_z1, gyro_z2, gyro_zc;

	public static boolean RedirectStdout = false; // For debugging
}

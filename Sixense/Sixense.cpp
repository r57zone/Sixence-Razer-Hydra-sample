#include "stdafx.h"
#include <Windows.h>
#include <Math.h>

#define SIXENSE_SUCCESS 0
#define SIXENSE_FAILURE -1

#define SIXENSE_BUTTON_BUMPER   (0x01<<7)
#define SIXENSE_BUTTON_JOYSTICK (0x01<<8)
#define SIXENSE_BUTTON_1        (0x01<<5)
#define SIXENSE_BUTTON_2        (0x01<<6)
#define SIXENSE_BUTTON_3        (0x01<<3)
#define SIXENSE_BUTTON_4        (0x01<<4)
#define SIXENSE_BUTTON_START    (0x01<<0)

#define SIXENSE_SUCCESS 0
#define SIXENSE_FAILURE -1

typedef struct _sixenseControllerData {
	float pos[3];
	float rot_mat[3][3];
	float joystick_x;
	float joystick_y;
	float trigger;
	unsigned int buttons;
	unsigned char sequence_number;
	float rot_quat[4];
	unsigned short firmware_revision;
	unsigned short hardware_revision;
	unsigned short packet_type;
	unsigned short magnetic_frequency;
	int enabled;
	int controller_index;
	unsigned char is_docked;
	unsigned char which_hand;
	unsigned char hemi_tracking_enabled;
} sixenseControllerData;

typedef int(__cdecl *_sixenseInit)(void);
typedef int(__cdecl *_sixenseExit)(void);
typedef int(__cdecl *_sixenseGetData)(int which, int index_back, sixenseControllerData *);

_sixenseInit sixenseInit;
_sixenseGetData sixenseGetData;
_sixenseExit sixenseExit;

HMODULE hDll;

double RadToDeg(double r) {
	return r * (180 / 3.14159265358979323846); //180 / PI
}

int main()
{
	SetConsoleTitle(_T("Sixence Razer Hydra get data"));
	hDll = LoadLibrary(_T("sixense.dll"));

	if (hDll != NULL) {

		sixenseInit = (_sixenseInit)GetProcAddress(hDll, "sixenseInit");
		sixenseExit = (_sixenseExit)GetProcAddress(hDll, "sixenseExit");
		sixenseGetData = (_sixenseGetData)GetProcAddress(hDll, "sixenseGetData");

		if (sixenseInit == NULL || sixenseGetData == NULL || sixenseExit == NULL)
			hDll = NULL;
	}

	if (hDll != NULL) {
		sixenseInit();
	}
	else {
		printf("Can't load library \"sixense.dll\"");
	}

	Sleep(50);
	
	sixenseControllerData data;
	double yaw, pitch, roll, sinr, cosr, sinp, siny, cosy;

	while (hDll != NULL) {
		system("cls");
		
		//Controller1
		sixenseGetData(1, 0, &data);
		printf("Controller 1\n");
		printf("X=%.2f, Y=%.2f, Z=%.2f\n",data.pos[0], data.pos[1], data.pos[2]);

		//Convert quaternion to ypr
		// roll (x-axis rotation)
		sinr = 2.0 * (data.rot_quat[0] * data.rot_quat[1] + data.rot_quat[2] * data.rot_quat[3]);
		cosr = 1.0 - 2.0 * (data.rot_quat[1] * data.rot_quat[1] + data.rot_quat[2] * data.rot_quat[2]);
		roll = RadToDeg(atan2(sinr, cosr));

		// pitch (y-axis rotation)
		sinp = 2.0 * (data.rot_quat[0] * data.rot_quat[2] - data.rot_quat[3] * data.rot_quat[1]);
		if (fabs(sinp) >= 1)
			pitch = RadToDeg(copysign(3.14159265358979323846 / 2, sinp)); // use 90 degrees if out of range
		else
			pitch = RadToDeg(asin(sinp));

		// yaw (z-axis rotation)
		siny = 2.0 * (data.rot_quat[0] * data.rot_quat[3] + data.rot_quat[1] * data.rot_quat[2]);
		cosy = 1.0 - 2.0 * (data.rot_quat[2] * data.rot_quat[2] + data.rot_quat[3] * data.rot_quat[3]);
		yaw = RadToDeg(atan2(siny, cosy));


		printf("Yaw=%.2f, Pitch=%.2f, Roll=%.2f\n", yaw, pitch, roll);
		printf("Buttons=%d, Trigger=%.2f, ThumbX=%.2f, ThumbY=%.2f\n", data.buttons, data.trigger, data.joystick_x, data.joystick_y);
		printf("Hand=%d, Index=%d\n\n", data.which_hand, data.controller_index);


		//Controller2
		sixenseGetData(0, 1, &data);
		printf("Controller 2\n");
		printf("X=%.2f, Y=%.2f, Z=%.2f\n", data.pos[0], data.pos[1], data.pos[2]);

		//Convert quaternion to ypr
		// roll (x-axis rotation)
		sinr = 2.0 * (data.rot_quat[0] * data.rot_quat[1] + data.rot_quat[2] * data.rot_quat[3]);
		cosr = 1.0 - 2.0 * (data.rot_quat[1] * data.rot_quat[1] + data.rot_quat[2] * data.rot_quat[2]);
		roll = RadToDeg(atan2(sinr, cosr));

		// pitch (y-axis rotation)
		sinp = 2.0 * (data.rot_quat[0] * data.rot_quat[2] - data.rot_quat[3] * data.rot_quat[1]);
		if (fabs(sinp) >= 1)
			pitch = RadToDeg(copysign(3.14159265358979323846 / 2, sinp)); // use 90 degrees if out of range
		else
			pitch = RadToDeg(asin(sinp));

		// yaw (z-axis rotation)
		siny = 2.0 * (data.rot_quat[0] * data.rot_quat[3] + data.rot_quat[1] * data.rot_quat[2]);
		cosy = 1.0 - 2.0 * (data.rot_quat[2] * data.rot_quat[2] + data.rot_quat[3] * data.rot_quat[3]);
		yaw = RadToDeg(atan2(siny, cosy));


		printf("Yaw=%.2f, Pitch=%.2f, Roll=%.2f\n", yaw, pitch, roll);
		printf("Buttons=%d, Trigger=%.2f, ThumbX=%.2f, ThumbY=%.2f\n", data.buttons, data.trigger, data.joystick_x, data.joystick_y);
		printf("Hand=%d, Index=%d\n\n", data.which_hand, data.controller_index);

		Sleep(50);

		if ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) != 0 || (GetAsyncKeyState(VK_RETURN) & 0x8000)) break;
	}

	if (hDll != NULL)
		sixenseExit();

	FreeLibrary(hDll);
	hDll = nullptr;

    return 0;
}


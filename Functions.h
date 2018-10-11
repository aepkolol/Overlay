#pragma once
#include <vector>
#include <dwrite.h>
#include <d2d1.h>
#include "ProcMem.h"
#include "Vector.h"
#include "Offsets.h"
#include "FindPattern.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

extern int width;
extern int height;

ULONG_PTR GNames;
ProcMem mem;
DWORD pid;
bool FirstRun = true;
uintptr_t BASE, NamesAddress, WorldAddress, ObjectsAddress;
char SoTGame[256] = "SoTGame.exe";

Vector3 myLocation, myAngles, CameraCachePOV;
float DefaultFOV;

void ReadData()
{
	if (FirstRun)
	{
		mem.Process(SoTGame);

		/* Set Module and store it in Client */
		BASE = mem.Module(SoTGame);
		FirstRun = false;

		auto address = IgroWidgets::FindPatternExternal(mem.hProcess, reinterpret_cast<HMODULE>(BASE),
			reinterpret_cast<const unsigned char*>("\x48\x8B\x3D\x00\x00\x00\x00\x48\x85\xFF\x75\x00\xB9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xF8\x48\x89\x44"),
			"xxx????xxxx?x????x????xxxxxx");
		if (address > 0)
		{
			NamesAddress = IgroWidgets::ReadRIPAddress(mem.hProcess, address, 3, 7);
		}

		address = IgroWidgets::FindPatternExternal(mem.hProcess, reinterpret_cast<HMODULE>(BASE),
			reinterpret_cast<const unsigned char*>("\x48\x8B\x0D\x00\x00\x00\x00\x48\x8B\x01\xFF\x90\x00\x00\x00\x00\x48\x8B\xF8\x33\xD2\x48\x8D\x4E"),
			"xxx????xxxxx????xxxxxxxx");
		if (address > 0)
		{
			WorldAddress = IgroWidgets::ReadRIPAddress(mem.hProcess, address, 3, 7);
		}

		address = IgroWidgets::FindPatternExternal(mem.hProcess, reinterpret_cast<HMODULE>(BASE),
			reinterpret_cast<const unsigned char*>("\x89\x1D\x00\x00\x00\x00\x48\x8B\xDF\x48\xC1\xE3\x04\x33\xD2"),
			"xx????xxxxxxxxx");
		if (address > 0)
		{
			ObjectsAddress = IgroWidgets::ReadRIPAddress(mem.hProcess, address, 2, 6);
		}
	}
}

class text
{
public:
	char word[64];
};

enum type
{
	player,
	ship,
	common,
	rare,
	legendary,
	mythical,
	cloud,
	crate,
	enemy,
	limited,
	other
};

struct AActors
{
	int id;
	std::string name, item;
	Vector3 Location;
	int type, namesize;
	int distance;
};

std::vector<AActors> ActorArray;
#define Assert( _exp ) ((void)0)

struct vMatrix
{
	vMatrix() {}
	vMatrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	float *operator[](int i) { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	const float *operator[](int i) const { Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	float *Base() { return &m_flMatVal[0][0]; }
	const float *Base() const { return &m_flMatVal[0][0]; }

	float m_flMatVal[3][4];
};

vMatrix Matrix(Vector3 rot, Vector3 origin)
{
	origin = Vector3(0, 0, 0);
	float radPitch = (rot.x * float(PI) / 180.f);
	float radYaw = (rot.y * float(PI) / 180.f);
	float radRoll = (rot.z * float(PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	vMatrix matrix;
	matrix[0][0] = CP * CY;
	matrix[0][1] = CP * SY;
	matrix[0][2] = SP;
	matrix[0][3] = 0.f;

	matrix[1][0] = SR * SP * CY - CR * SY;
	matrix[1][1] = SR * SP * SY + CR * CY;
	matrix[1][2] = -SR * CP;
	matrix[1][3] = 0.f;

	matrix[2][0] = -(CR * SP * CY + SR * SY);
	matrix[2][1] = CY * SR - CR * SP * SY;
	matrix[2][2] = CR * CP;
	matrix[2][3] = 0.f;

	return matrix;
}

bool WorldToScreen(Vector3 origin, Vector2 * out) {
	Vector3 Screenlocation = Vector3(0, 0, 0);
	Vector3 Rotation = myAngles; // FRotator
	vMatrix tempMatrix = Matrix(Rotation, Vector3(0, 0, 0)); // Matrix
	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix[0][0], tempMatrix[0][1], tempMatrix[0][2]);
	vAxisY = Vector3(tempMatrix[1][0], tempMatrix[1][1], tempMatrix[1][2]);
	vAxisZ = Vector3(tempMatrix[2][0], tempMatrix[2][1], tempMatrix[2][2]);

	Vector3 vDelta = origin - CameraCachePOV;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float FovAngle = DefaultFOV;
	float ScreenCenterX = width / 2.0f;
	float ScreenCenterY = height / 2.0f;

	out->x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * static_cast<float>(PI) / 360.f)) / vTransformed.z;
	out->y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * static_cast<float>(PI) / 360.f)) / vTransformed.z;

	return true;
}

Vector2 RotatePoint(Vector2 pointToRotate, Vector2 centerPoint, float angle, bool angleInRadians = false)
{
	if (!angleInRadians)
		angle = static_cast<float>(angle * (PI / 180.f));
	float cosTheta = static_cast<float>(cos(angle));
	float sinTheta = static_cast<float>(sin(angle));
	Vector2 returnVec = Vector2(cosTheta * (pointToRotate.x - centerPoint.x) - sinTheta * (pointToRotate.y - centerPoint.y), sinTheta * (pointToRotate.x - centerPoint.x) + cosTheta * (pointToRotate.y - centerPoint.y)
	);
	returnVec += centerPoint;
	return returnVec;
}

float getAngle(Vector2 initial, Vector2 final)
{
	return atan2(initial.y - final.y, initial.x - final.x) * 180 / PI;
}

template< class T > struct TArray
{
public:
	T* Data;
	int Count;
	int Max;

public:
	TArray()
	{
		Data = NULL;
		Count = Max = 0;
	};
};

std::string GetActorName(int ID)
{
	try {
		DWORD_PTR fNamePtr = mem.Read<DWORD_PTR>(GNames + int(ID / 0x4000) * 0x8);
		DWORD_PTR fName = mem.Read<DWORD_PTR>(fNamePtr + 0x8 * int(ID % 0x4000));
		return mem.Read<text>(fName + 16).word;
	}
	catch (...)
	{
		return std::string("");
	}
}

class AActor
{
public:
	char pad_0000[2120]; //0x0000
}; //Size: 0x0848

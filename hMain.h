#ifndef HMAIN_H
#define HMAIN_H

#include "hDirectX.h"
#include <d3dx9.h>
#include <dwmapi.h>
#include <iostream>
#include <windows.h>

extern int Width;
extern int Height;
extern char lWindowName[256];
extern HWND hWnd;
extern char tWindowName[256];
extern HWND tWnd;
extern RECT tSize;
extern MSG Message;
extern bool Debug_Border;

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hSecInstance, LPSTR nCmdLine, INT nCmdShow);
void SetWindowToTarget();
#endif
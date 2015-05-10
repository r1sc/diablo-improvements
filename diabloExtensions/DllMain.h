#define WIN32_LEAN_AND_MEAN
#ifndef DLLMAIN_H_
#define DLLMAIN_H_
#include <windows.h>

void MyInit();
void MyCleanUp();

void UpdateItems();

void PatchAddr(int dwAddressToPatch, int data);
void PlaceDetour(DWORD dwAddressToPatch, DWORD dwDetourAddress, DWORD dwPadSize, bool bFlag);
void PatchMemory(DWORD dwAddressToPatch, BYTE *pData, DWORD dwDataLength);

static HMODULE hmodMyDLL;
#endif
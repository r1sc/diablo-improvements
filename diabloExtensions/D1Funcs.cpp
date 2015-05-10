#include "D1Funcs.h"
#include <stdio.h>
#include <windows.h>

DITEM curItm;

int GetExperience(int level)
{
	if(level <= 0)
		return 0;
	return *(int*)(0x48703C + ((level-1) * 4));
}

int NumMonstersLeftOnCurrentLevel()
{
	return *(int*)0x526880;
}

int NumMouseClicks()
{
	return *(int*)0x15031CA8;
}

int NumItemsOnGround()
{
	return *(int*)0x49CC28;
}

DITEM* GetItemFromGround(int i)
{
	int itemAddr = 0x6a49CC + (i * 0x170);
	//+0x24 = x
	//+0x28 = y
	curItm.y = *(int*)(itemAddr + 0x30);
	curItm.x = *(int*)(itemAddr + 0x34);
	curItm.pickedUp = *(bool*)(itemAddr + 0x50);
	curItm.rarity = *(char*)(itemAddr + 0x60);
	curItm.text = (char*)(itemAddr + 0x61);
	return &curItm;
}

void CloseMenu()
{
	int* curMenu = (int*)0x563EF8;
	*curMenu = 0;
}

void FinishMenu(int id, int firstRow)
{
	int* curRow = (int*)0x562218;
	int* curMenu = (int*)0x563EF8;
	*curRow = firstRow;
	*curMenu = id+1;
}

void D1DrawMenu(int id)
{
	static int clearMenu = 0x438CCA;
	static int drawMenu = 0x43B37D;
	
	__asm
	{
		MOV ECX, id
		MOV EDX, 0
		CALL drawMenu
		MOV ECX, 0
		MOV EDX, 24
		CALL clearMenu;
	}
}

void D1DrawSeparator(int row)
{
	static int funcCall = 0x438D09;

	__asm
	{
		MOV ECX, row
		CALL funcCall
	}
}

void FormatMenuItem(int x, int y, int color, int mode, int textOrientation, const char* format, ...)
{
	static char szBuffer[80];
	va_list vaArgs;
	va_start(vaArgs, format);
	vsprintf(szBuffer, format, vaArgs);
	va_end(vaArgs);

	D1DrawMenuItem(x, y, color, mode, textOrientation, szBuffer);
}

void D1DrawMenuItem(int x, int y, int color, int mode, int textOrientation, char* text)
{
	/*
	Draw menuitem - 0x438D48

	ECX - x-offset
	EDX - y-offset

	Stack:
	text-orientation (1 = center, 0 = left)
	text
	color
	mode (selectable = 1, just text = 0)
	*/
	static int funcCall = 0x438D48;

	__asm
	{
		MOV ECX, x
		MOV EDX, y
		PUSH mode
		PUSH color
		PUSH text
		PUSH textOrientation
		CALL funcCall
	}

}

void D1TextLabel(int x, int y, int color, const char* text)
{
	static int addr = 0x00458A03;
	/*
	Colors
	------
	0 - white
	1 - blue
	2 - red
	3 - gold
	*/

	__asm
	{
		MOV ECX, x
		MOV EDX, y
		MOV EBX, 0
		PUSH color
		PUSH text
		call addr;
	}
}

int GetCurrentDungeonLevel()
{
	return *(int*)0x5330EC;
}

bool InGame()
{
	int* ingamePtr = (int*)0x49AAA0;
	return *ingamePtr == 1;
}

int* GetSelectedUnit()
{
	//0x556814
	//monsterstruct_plats = 0x52688C + (monstertal * 0x0E4)
	int* unittal = (int*)0x556814;
	if(*unittal == -1)
		return 0;
	int* unit = (int*)(0x52688C + (*unittal * 0x0E4));	
	return unit;
}

short GetMonsterHealth(int* unit)
{
	short* maxHealthPtr;
	short* curHealthPtr;
	__asm
	{
		push eax
		mov eax, unit
		add eax, 0x8C
		mov maxHealthPtr, eax
		add eax, 0x04
		mov curHealthPtr, eax
		pop eax
	}

	return ((float)*curHealthPtr / (float)*maxHealthPtr) * 100;
}

void D1ChatTextOut(const char* text)
{
	static int addr = 0x44DC63;
	static int noob = strlen(text);

	__asm
	{
		mov eax, noob
		mov ecx, 0
		mov edx, DWORD PTR [text]
		mov edi, eax
		call addr;
	}
}

void WriteChatText(const char* format, ...)
{
	char szBuffer[80];
	va_list vaArgs;
	va_start(vaArgs, format);
	vsprintf(szBuffer, format, vaArgs);
	va_end(vaArgs);
	D1ChatTextOut(szBuffer);
}
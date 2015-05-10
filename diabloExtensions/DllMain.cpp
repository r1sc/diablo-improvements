#include "DllMain.h"
#include "D1API.h"
#include "D1Funcs.h"
#include <math.h>

HANDLE thread;

static BYTE* level = (BYTE*)0x533270;
static int* experience = (int*)0x533274;
static int* nextLevel = (int*)0x53327C;
static BYTE* pX = (BYTE*)0x005268D9;
static BYTE* pY = (BYTE*)0x005268D8;
static short* offsetX = (short*)0x533118;//0x5332A8;//0x53311C; //6245cc
static short* offsetY = (short*)0x53311C;//0x53311C; //6245cc
static DITEM* curItm = NULL;
static bool* AutoMapVisible = (bool*)0x54A200;
static bool welcomeShown = false;
static bool showMonsterHealth = true;
static bool showItemsOnGround = true;
static bool showLevelProgress = true;

DLabel* experienceLabel;
DLabel* monsterLabel;
DLabel* monstersLeftLabel;
DLabelManager* itemLabelMgr;
DLabelManager* lblMgr;
DConsole* console;

bool WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {

	hmodMyDLL = hModule;

	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			MyInit();
			break;
		case DLL_PROCESS_DETACH:
			TerminateThread(thread, 0);
			MyCleanUp();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		default:
			break;
	}

	return true;

}

void DrawOptionsMenu()
{
	D1DrawMenu(25);
	D1DrawMenuItem(0, 2, GOLD, 0, 1, "Risc's Diablo Improvements\0");
	D1DrawMenuItem(0, 3, GOLD, 0, 1, "Version 1.0\0");
	D1DrawSeparator(5);
	D1DrawMenuItem(0, 7, WHITE, 1, 1, "Toggle Level progress\0");
	D1DrawMenuItem(0, 9, WHITE, 1, 1, "Toggle Item's on ground\0");
	D1DrawMenuItem(0, 11, WHITE, 1, 1, "Toggle Monster healthbar\0");
	D1DrawMenuItem(0, 13, WHITE, 1, 1, "Close options\0");
	FinishMenu(25, 7);
}

void DrawMainMenu()
{
	D1DrawMenu(24);
	D1DrawMenuItem(0, 2, GOLD, 0, 1, "Risc's Diablo Improvements\0");
	D1DrawMenuItem(0, 3, GOLD, 0, 1, "Version 1.0\0");
	D1DrawSeparator(5);
	FormatMenuItem(0, 7, BLUE, 0, 1, "Level progress = %d\0", showLevelProgress);
	FormatMenuItem(0, 9, BLUE, 0, 1, "Item's on ground = %d\0", showItemsOnGround);
	FormatMenuItem(0, 11, BLUE, 0, 1, "Monster healthbar = %d\0", showMonsterHealth);
	D1DrawSeparator(13);
	D1DrawMenuItem(0, 16, WHITE, 1, 1, "Options\0");
	D1DrawMenuItem(0, 18, WHITE, 1, 1, "I have to carry on\0");
	FinishMenu(24, 16);
}

DWORD WINAPI MyThread(LPVOID param)
{
	int* curUnit = 0;
	while(true)
	{
		if(InGame())
		{
			if(!welcomeShown)
			{
				welcomeShown = true;
				Sleep(2000);
				*AutoMapVisible = true;
				DrawMainMenu();
				console->Print("Risc's Diablo Improvements Initialized.");
			}
			if(GetCurrentDungeonLevel() > 0)
			{
				if(showMonsterHealth)
				{
					curUnit = GetSelectedUnit();
					if(curUnit != 0)
					{
						monsterLabel->SetText("Monster health: %d %%\0", GetMonsterHealth(curUnit));
						monsterLabel->Show();
					}
					else
						monsterLabel->Hide();
				}
				monstersLeftLabel->SetText("Left: %d", NumMonstersLeftOnCurrentLevel());
				monstersLeftLabel->Show();

				if(showItemsOnGround)
				{
					//update items
					UpdateItems();
				}
			}
			else
			{
				//In town
				//--------
				monstersLeftLabel->Hide();
				//clear items
				itemLabelMgr->Clear();
			}
			
			int expNow = *experience - GetExperience(*level-1);
			int expNext = *nextLevel - GetExperience(*level-1);
			experienceLabel->SetText("Level %d: %d %%", *level + 1, (int)(((float)expNow / (float)expNext) * 100));
		}
		Sleep(50);
	}
	return 0;
}

void LabelSet3DCoord(DLabel* label, int gx, int gy)
{
	int diffX = gy - *pY;
	int diffY = gx - *pX;
	int finalX = 300;
	int finalY = 160;
	int directionX = 1;
	int directionY = 1;

	if(diffX < 0)
		directionX = -1;
	if(diffY < 0)
		directionY = -1;


	diffX = abs(diffX);
	diffY = abs(diffY);

	//start by going in the y-direction
	while(diffY > 0)
	{
		//going down
		finalY += directionY * 16;
		finalX += -directionY * 32;
		diffY--;
	}

	//now go in the x-direction
	while(diffX > 0)
	{
		//going right
		finalX += directionX * 32;
		finalY += directionX * 16;
		diffX--;
	}

	label->SetPosition(finalX - *offsetX, finalY - *offsetY);
}

void UpdateItems()
{
	itemLabelMgr->Clear();
	for(int i = 1; i <= 326; i++)
	{
		curItm = GetItemFromGround(i);
		if(!curItm->pickedUp)
		{
			if(*pX > curItm->x - 7 && *pX < curItm->x + 7)
				if(*pY > curItm->y - 7 && *pY < curItm->y + 7)
				{
					DLabel* temp = new DLabel(10, 10, (curItm->rarity), curItm->text);
					if(curItm->rarity == 2)
						temp->SetColor(GOLD);
					LabelSet3DCoord(temp, curItm->x, curItm->y);
					itemLabelMgr->Add(temp);
				}
		}
	}
}

void __declspec(naked) MyVSync()
{
	static int retAddr = 0x42A885;
	static int funcCall = 0x0042B594;
	
	__asm
	{
		pushad
	}
	//do all text-writing here
	lblMgr->Render();
	itemLabelMgr->Render();
	console->Render();
	
	__asm
	{
		popad
		CALL funcCall
		JMP retAddr
	}
}

void MainMenuCallback()
{
	int rowClicked = *(int*)0x562218;

	if(rowClicked == 16)
	{
		//open options menu
		DrawOptionsMenu();
	}

	if(rowClicked == 18)
	{
		//close
		CloseMenu();
	}
}

void OptionsMenuCallback()
{
	int rowClicked = *(int*)0x562218;

	if(rowClicked == 7)
	{
		console->Print("Toggle level progress indicator");
		showLevelProgress = !showLevelProgress;
		if(!showLevelProgress)
			experienceLabel->Hide();
		else
			experienceLabel->Show();
	}

	if(rowClicked == 9)
	{
		showItemsOnGround = !showItemsOnGround;
		console->Print("Toggle items on ground");
	}

	if(rowClicked == 11)
	{
		console->Print("Toggle monster healthbar");
		showMonsterHealth = !showMonsterHealth;
	}

	if(rowClicked == 13)
	{
		//close
		DrawMainMenu();
	}
}


static void __declspec(naked) MyMenuCallback()
{
	static int menuId;
	static int retAddr = 0x43D404;
	__asm
	{
		pushad
		MOV menuId, EAX
	}

	if(menuId == 24)
		MainMenuCallback();
	if(menuId == 25)
		OptionsMenuCallback();

	__asm
	{
		popad
		JMP retAddr
	}
}

void MyInit()
{
	console = new DConsole(10, 335);
	monsterLabel = new DLabel(210, 20, RED);
	monsterLabel->Hide();
	monstersLeftLabel = new DLabel(10, 80, GOLD);
	experienceLabel = new DLabel(270, 350, GOLD, "Next level: N/A");
	
	itemLabelMgr = new DLabelManager();
	lblMgr = new DLabelManager();
	lblMgr->Add(monsterLabel);
	lblMgr->Add(monstersLeftLabel);
	lblMgr->Add(experienceLabel);
	
	CloseMenu();
	//patch switch-table @ 0x43D326
	int memOffset = (int)MyMenuCallback - 0x43D32C;
	PatchMemory(0x43D328, (BYTE*)&memOffset, 4);
	PlaceDetour(0x42A880, (DWORD)MyVSync, 0, true);
	thread = CreateThread(NULL, 0, MyThread, NULL, 0, 0);
}

void MyCleanUp()
{
	//remove vsync-patch
	BYTE data[5] = {0xE8, 0x0F, 0x0D, 0x00, 0x00};
	PatchMemory(0x42A880, data, 5);
	//remove menu-patch
	BYTE menuData[4] = {0xD8, 0x00, 0x00, 0x00};
	PatchMemory(0x43D328, menuData, 4);
}

void PlaceDetour(DWORD dwAddressToPatch, DWORD dwDetourAddress, DWORD dwPadSize, bool bFlag) {

	DWORD dwOldProtect = NULL;

	VirtualProtect((LPVOID)dwAddressToPatch,(dwPadSize+5),PAGE_EXECUTE_READWRITE,&dwOldProtect);

	if (bFlag) {
		*(BYTE*)(dwAddressToPatch) = 0xE9;
	} else {
		*(BYTE*)(dwAddressToPatch) = 0xE8;
	}
	*(DWORD*)(dwAddressToPatch+1) = ((dwDetourAddress-dwAddressToPatch)-5);
	for (DWORD i = dwPadSize; i > 0; i--) {
		*(BYTE*)(dwAddressToPatch+5+i) = 0x90;
	}

	VirtualProtect((LPVOID)dwAddressToPatch,(dwPadSize+5),dwOldProtect,&dwOldProtect);

}

void MakeAddrReadable(DWORD addr)
{
	VirtualProtect((LPVOID)addr, 4, PAGE_READWRITE, NULL);
}

void PatchMemory(DWORD dwAddressToPatch, BYTE *pData, DWORD dwDataLength) {

	DWORD dwOldProtect = NULL;

	VirtualProtect((LPVOID)dwAddressToPatch,dwDataLength,PAGE_EXECUTE_READWRITE,&dwOldProtect);

	for (DWORD i = 0; i < dwDataLength; i++) {
		*(BYTE*)(dwAddressToPatch+(i)) = pData[i];
	}

	VirtualProtect((LPVOID)dwAddressToPatch,dwDataLength,dwOldProtect,&dwOldProtect);

}
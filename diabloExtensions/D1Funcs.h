#ifndef D1FUNCS_H
#define D1FUNCS_H

typedef struct DITEM
{
	int x;
	int y;
	bool pickedUp;
	char rarity;
	char* text;
}DITEM;

void FormatMenuItem(int x, int y, int color, int mode, int textOrientation, const char* format, ...);
void CloseMenu();
void FinishMenu(int id, int firstRow);
void D1DrawSeparator(int row);
void D1DrawMenu(int id);
void D1DrawMenuItem(int x, int y, int color, int mode, int textOrientation, char* text);
int GetExperience(int level);
void D1TextLabel(int x, int y, int color, const char* text);
void D1ChatTextOut(const char* text);
void WriteChatText(const char* format, ...);
DITEM* GetItemFromGround(int i);
int NumItemsOnGround();
int NumMonstersLeftOnCurrentLevel();
int NumMouseClicks();
int GetCurrentDungeonLevel();
short GetMonsterHealth(int* unit);
int* GetSelectedUnit();
bool InGame();

#endif
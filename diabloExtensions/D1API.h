#ifndef D1API_H
#define D1API_H

#define NULL 0

enum DColor
{
	WHITE,
	BLUE,
	RED,
	GOLD
};

class DLabel
{
private:
	int _color;
	int _x;
	int _y;
	bool _visible;
	unsigned int _time;
	char _text[255];
public:
	DLabel(int x, int y, int color, const char* text);
	DLabel(int x, int y, int color);
	unsigned int GetTime();
	int GetX();
	int GetY();
	void Hide();
	void Show();
	void SetColor(int color);
	void SetText(const char* format, ...);
	void SetPosition(int x, int y);
	void Render();
};

class DLabelManager
{
private:
	int _count;
	DLabel* _labels[50];
public:
	void Add(DLabel* label);
	void Clear();
	int Count();
	DLabel* GetLabel(int i);
	void Remove(DLabel* label);
	void Render();
};

class DConsole
{
private:
	int _x;
	int _y;
	DLabelManager* labelMgr;
public:
	DConsole(int x, int y);
	void Render();
	void Print(const char* format, ...);
};

#endif
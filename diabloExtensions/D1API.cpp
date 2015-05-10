#include "D1API.h"
#include "D1Funcs.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

void DLabelManager::Add(DLabel *label)
{
	this->_labels[this->_count] = label;
	this->_count++;
}

void DLabelManager::Remove(DLabel *label)
{
	for(int i = 0; i < this->_count; i++)
	{
		if(this->_labels[i] == label)
		{
			//null this and move all labels above down 1 step
			this->_labels[i] = NULL;
			for(int k = i; k < this->_count - 1; k++)
			{
				this->_labels[k] = this->_labels[k + 1];
				this->_labels[k]->SetPosition(this->_labels[k]->GetX(), this->_labels[k]->GetY() + 20);
			}
			this->_count--;
			this->_labels[this->_count] = 0;
			break;
		}
	}
}

void DLabelManager::Render()
{
	for(int i = 0; i < this->_count; i++)
	{
		this->_labels[i]->Render();
	}
}

int DLabelManager::Count()
{
	return this->_count;
}

void DLabelManager::Clear()
{
	//for(int i = 0; i < this->_count; i++)
	//	this->_labels[i] = NULL;
	this->_count = 0;
}

DLabel* DLabelManager::GetLabel(int i)
{
	return this->_labels[i];
}

DLabel::DLabel(int x, int y, int color)
{
	this->_visible = true;
	this->_time = time(NULL);
	this->_x = x;
	this->_y = y;
	this->_color = color;
	strcpy(this->_text, "Initializing...\0");
}

DLabel::DLabel(int x, int y, int color, const char *text)
{
	this->_visible = true;
	this->_time = time(NULL);
	this->_x = x;
	this->_y = y;
	this->_color = color;
	strcpy(this->_text, text);
}

void DLabel::SetText(const char *format, ...)
{
	va_list vaArgs;
	va_start(vaArgs, format);
	vsprintf(this->_text, format, vaArgs);
	va_end(vaArgs);
}

void DLabel::SetColor(int color)
{
	this->_color = color;
}

void DLabel::SetPosition(int x, int y)
{
	this->_x = x;
	this->_y = y;
}

void DLabel::Hide()
{
	this->_visible = false;
}

void DLabel::Show()
{
	this->_visible = true;
}

int DLabel::GetX()
{
	return _x;
}

int DLabel::GetY()
{
	return _y;
}

unsigned int DLabel::GetTime()
{
	return this->_time;
}

void DLabel::Render()
{
	if(this->_visible)
		D1TextLabel(this->_x, this->_y, this->_color, this->_text);
}

DConsole::DConsole(int x, int y)
{
	this->_x = x;
	this->_y = y;
	this->labelMgr = new DLabelManager();
}

void DConsole::Print(const char *format, ...)
{
	char szBuffer[128];
	va_list vaArgs;
	va_start(vaArgs, format);
	vsprintf(szBuffer, format, vaArgs);
	va_end(vaArgs);
	this->labelMgr->Add(new DLabel(this->_x, this->_y - (this->labelMgr->Count() * 20), GOLD, szBuffer));
}

void DConsole::Render()
{
	DLabel* curLabel;
	for(int i = 0; i < this->labelMgr->Count(); i++)
	{
		curLabel = this->labelMgr->GetLabel(i);
		if(time(NULL) > curLabel->GetTime() + 5)
		{
			//remove this
			this->labelMgr->Remove(curLabel);
			break;
		}
	}
	this->labelMgr->Render();
}
// BadAppleC++.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <windows.h>
#include <thread>
#include <math.h>
#include<mmsystem.h>
typedef void(*plso)(char*,char*,DWORD);
#define sl 6577
using namespace std;
void clearScreen()
{
	HANDLE hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO bufInfo;
	SMALL_RECT scroll;
	COORD newCursorPointer;
	CHAR_INFO ciFill;

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (!GetConsoleScreenBufferInfo(hStdOut, &bufInfo))
		return;

	scroll.Left = 0;
	scroll.Top = 0;
	scroll.Right = bufInfo.dwSize.X;
	scroll.Bottom = bufInfo.dwSize.Y;
	newCursorPointer.X = 0;
	newCursorPointer.Y = -bufInfo.dwSize.Y;
	ciFill.Char.UnicodeChar = L' ';
	ciFill.Attributes = bufInfo.wAttributes;
	ScrollConsoleScreenBufferW(hStdOut, &scroll, NULL, newCursorPointer, &ciFill);
	newCursorPointer.Y = 0;
	SetConsoleCursorPosition(hStdOut, newCursorPointer);
}
LARGE_INTEGER getTime()
{
	LARGE_INTEGER ret;
	if (!QueryPerformanceCounter(&ret))
		throw new string("Could not get current time");
	return ret;
}
ifstream openFile_Read(string path)
{
	ifstream ret(path.c_str(),ios::in | ios::binary);
	if (!ret)
		throw new string((string("Could not open file ")+path).c_str());
	return ret;
}
char* readTextFile(string path)
{
	ifstream temp = openFile_Read(path);
	temp.seekg(0, ios::end);
	int length = (int)temp.tellg();
	temp.seekg(0, ios::beg);
	char* ret = new char[length];
	for (int i = 0; i < length; i++)
		ret[i] = 0;
	temp.read(ret,length);
	temp.close();
	for (int i = 0; i < length; i++)
		if (ret[i] < 0)
			ret[i] = 0;
	return ret;
}
void clearAndPrint(char* s,int size)
{
	clearScreen();
	cout.write(s, size);
}
long long getDistance(long long FPS)
{
	long long ret = 1;
	for (ret = 1; (FPS*ret)<32 ; ret++ );
	return ret;
}
int main()
{
	HANDLE handle_console = GetStdHandle(STD_OUTPUT_HANDLE);
	if (INVALID_HANDLE_VALUE == handle_console)
		return GetLastError();
	CONSOLE_SCREEN_BUFFER_INFOEX bufi;
	bufi.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	if (!GetConsoleScreenBufferInfoEx(handle_console, &bufi))
		return GetLastError();
	CONSOLE_FONT_INFOEX font;
	font.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	if (!GetCurrentConsoleFontEx(handle_console,false,&font))
		return GetLastError();
	font.dwFontSize.X = 13;
	font.dwFontSize.Y = 13;
	if (!SetCurrentConsoleFontEx(handle_console, false, &font))
		return GetLastError();
	if (!SetConsoleScreenBufferInfoEx(handle_console, &bufi))
		return GetLastError();
	SMALL_RECT tempsr = { 0,0, (short)(bufi.dwMaximumWindowSize.X-1),(short)(bufi.dwMaximumWindowSize.Y-1)};
	if (!SetConsoleWindowInfo(handle_console, true, &tempsr))
		return GetLastError();
	char* nouse = new char[0];
	LARGE_INTEGER freq;
	if (!QueryPerformanceFrequency(&freq))
		return GetLastError();
	string screens[sl];
	for (int i = 1; i < sl+1; i++)
	{
		screens[i - 1] = readTextFile(string("text\\") + string(_itoa(i,nouse,10)) + string(".txt"));
		cout<<(string("已加载: ") + string(_itoa(i,nouse,10)) + string(string(" / ")+string(_itoa(sl,nouse,10))))<<endl;
	}
	char* screens2[sl];
	for (int i = 0; i < sl; i++)
		screens2[i] = (char*) screens[i].c_str();
	int screenSize[sl];
	for (int i = 0; i < sl; i++)
		screenSize[i] = screens[i].size();
	font.dwFontSize.X = 10;
	font.dwFontSize.Y = 10;
	if (!SetCurrentConsoleFontEx(handle_console, false, &font))
		return GetLastError();
	if (!SetConsoleScreenBufferInfoEx(handle_console, &bufi))
		return GetLastError();
	SMALL_RECT tempsr2 = { 0,0, (short)(bufi.dwMaximumWindowSize.X - 1),(short)(bufi.dwMaximumWindowSize.Y - 1) };
	if (!SetConsoleWindowInfo(handle_console, true, &tempsr))
		return GetLastError();
	if (!SetConsoleMode(handle_console, CONSOLE_FULLSCREEN_MODE))
		return GetLastError();
	cout << "载入已完成 将在5秒后开始播放" << endl;
	Sleep(5000);
	clearScreen();
                HMODULE hDll=LoadLibrary("winmm.dll");
                if(hDll==NULL)
                  return -1;
                plso play=plso(GetProcAddress(hDll,"PlaySoundA"));
                if(play==NULL)
                  return -1;
	play(TEXT("BadApple.wav"), NULL, SND_FILENAME|SND_ASYNC);
	LARGE_INTEGER startTime = getTime();
	long long lastFrame = -1;
	long long repeatCount = 0;
	long long realLastFrame = -1;
	long long ft = 0;
	long long FPS = 30;
	long long distance = getDistance(FPS);
	bool hasDelayed=false;
	while (true)
	{
		long long sleeptime = 33;
		long long framecount = round((((double)(getTime().QuadPart) - (double)(startTime.QuadPart)) / (double)(freq.QuadPart) * (double)30));
		if (framecount >= sl)
			break;
		if (framecount + repeatCount >= sl)
			break;
		if (framecount == lastFrame)
			repeatCount++;
		else
			repeatCount = 0;
		if (repeatCount > 29)
			repeatCount = 29;
		if (distance > ((framecount + repeatCount) - realLastFrame))
			sleeptime = 7;
		for (long long i = 1; (i < (framecount + repeatCount) - realLastFrame) && ((framecount + repeatCount) - realLastFrame > 1); i+=distance)
		{
			clearAndPrint(screens2[realLastFrame + i],screenSize[realLastFrame+i]);
			ft++;
			FPS = round((double)ft / (((double)(getTime().QuadPart) - (double)(startTime.QuadPart)) / (double)(freq.QuadPart)));
			distance = getDistance(FPS);
			SetWindowTextA(GetConsoleWindow(), ((string("frame: ") + string(_itoa(realLastFrame + i, nouse, 10) + string(" jumpDistance: ") + string(_itoa(distance, nouse, 10)) + string(" FPS: ") + string(_itoa(FPS, nouse, 10)))+string(" patchFrame")).c_str()));
			Sleep(7);
		}
		clearAndPrint(screens2[framecount + repeatCount],screenSize[framecount+repeatCount]);
		ft++;
		FPS = round((double)ft / (((double)(getTime().QuadPart) - (double)(startTime.QuadPart)) / (double)(freq.QuadPart)));
		distance = getDistance(FPS);
		SetWindowTextA(GetConsoleWindow(), ((string("frame: ") + string(_itoa(framecount + repeatCount, nouse, 10) + string(" jumpDistance: ") + string(_itoa(distance, nouse, 10)) + string(" FPS: ") + string(_itoa(FPS, nouse, 10)))).c_str()));
		lastFrame = framecount;
		realLastFrame = lastFrame + repeatCount;
		Sleep(sleeptime);
	}
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

//#include <string>
//#include<vector>
//#include <conio.h>
//#include<easyx.h>
//#include <iostream>
//using namespace std;
//const float pix = 40;
//int height = 640;
//int width = 800;
//const clock_t FPS = 1000 / 60;
//float seed = 300;
//int x = 400;
//int y = 320;
//int speed = 6;
//static bool inArea(const ExMessage& msg, int left, int top, int right, int bottom)
//{
//	if (msg.x >= left && msg.x <= right && msg.y < bottom && msg.y > top)
//		return true;
//	else
//		return false;
//}
//
//static void drawButton(ExMessage& msg, int left, int top, int right, int bottom, char* str)
//{
//	setlinestyle(PS_SOLID | PS_JOIN_BEVEL, 5);
//	fillroundrect(left, top, right, bottom, 20, 20);
//	settextcolor(WHITE);
//	setbkmode(TRANSPARENT);
//	settextstyle(30, 10, "萝莉体");
//	LOGFONT f;
//	gettextstyle(&f); 
//	f.lfHeight = 35; 
//	f.lfWeight = 800;
//	_tcscpy_s(f.lfFaceName, _T("萝莉体")); 
//	f.lfQuality = ANTIALIASED_QUALITY; 
//	settextstyle(&f);
//	int xSpace = (right - left - textwidth(str)) / 2;
//	int ySpace = (bottom - top - textheight(str)) / 2;
//	outtextxy(left + xSpace, top + ySpace, str);
//}
//
//static void goodButton(ExMessage& msg, int left, int top, int right, int bottom, char* str)
//{
//	peekmessage(&msg, EX_MOUSE);
//	if (inArea(msg, left, top, right, bottom))
//	{
//		setlinecolor(RGB(195, 220, 195));
//		setfillstyle(BS_SOLID);
//		setfillcolor(RGB(0, 80, 20));
//		if (msg.message == WM_LBUTTONDOWN)
//		{
//			cout << "start game" << endl;
//		}
//	}
//	else
//	{
//		setlinecolor(RGB(15, 85, 5));
//		setfillstyle(BS_SOLID);
//		setfillcolor(RGB(0, 155, 20));
//	}
//	drawButton(msg, left, top, right, bottom, str);
//}
//
//static void showTest(int k)
//{
//	settextcolor(BLACK);
//	setbkmode(TRANSPARENT);
//	char str[10];
//	_itoa_s(k, str, 10);
//	outtextxy(200, 200, str);
//}
//
//void showStart(float seed)
//{
//	setbkmode(TRANSPARENT);
//	settextcolor(RGB(10,50,10));
//	char str[] = "按鼠标左键开始！";
//	LOGFONT f;
//	gettextstyle(&f);
//	f.lfHeight = 48;
//	f.lfWeight = seed;
//	_tcscpy_s(f.lfFaceName, _T("萝莉体"));
//	f.lfQuality = ANTIALIASED_QUALITY;
//	settextstyle(&f);
//	outtextxy(400-textwidth(str)/2,500 , str);
//
//}
//
//void inoutEffect (const IMAGE& pic,int speed,bool flag)
//{
//	if (flag==0)
//	{
//		for (int i=0; speed*i<=400;i++)
//		{
//			clock_t startTime = clock();
//			clock_t procedureTime = 0;
//			cleardevice();
//			BeginBatchDraw();
//			putimage(speed * i, speed * 0.8*i, 800-2* speed * i, 640-speed *1.6*i, &pic, speed *i, speed * 0.8*i);
//			EndBatchDraw();
//			procedureTime = clock() - startTime;
//			if (FPS - procedureTime > 0)
//				Sleep(FPS - procedureTime);
//		}
//	}
//	else
//	{
//		for (int i = 400/speed;speed*i>=0; i--)
//		{
//			clock_t startTime = clock();
//			clock_t procedureTime = 0;
//			cleardevice();
//			BeginBatchDraw();
//			putimage(i*speed, 0.8*i*speed, (400-i*speed)*2, (320 - 0.8*i*speed) * 2, &pic, i*speed, 0.8 * i*speed);
//			EndBatchDraw();
//			procedureTime = clock() - startTime;
//			if (FPS - procedureTime > 0)
//				Sleep(FPS - procedureTime);
//		}
//	}
//}
//
//void startEffect(int speed) {
//	
//	IMAGE temp1;
//	IMAGE temp2;
//	loadimage(&temp1, _T("assets\\bg1.png"));
//	loadimage(&temp2, _T("assets\\start.png"));
//	inoutEffect(temp2, 20,false);
//	inoutEffect(temp1, 20, true);
//}
//
//void putTransparentImage( IMAGE* image,IMAGE* mask,int x,int y)
//{
//	putimage(x, y, mask, NOTSRCERASE);
//	putimage(x, y, image, SRCINVERT);
//
//}
//
//void sheepHeadController(IMAGE* image, IMAGE* mask,int& x,int& y,int speed)
//{
//	putTransparentImage(image, mask, x, y);
//	int key = _getch();
//	if (x<800-image->getwidth()&&x>0&&y>0&&y<640-image->getheight())
//	{
//		switch (key)
//		{
//		case'w':
//			{
//				y -= speed;
//				break;
//			}
//		case's':
//			{
//				y += speed;
//				break;
//			}
//		case'a':
//			{
//				x -= speed;
//				break;
//			}
//		case'd':
//			{
//				x += speed;
//				break;
//			}
//		default:break;
//		}
//	}
//}
//
//int main()
//{
//	
//	initgraph(width, height, 1 | 8); //use "|" to apply different flags at the same time
//	IMAGE start;
//	loadimage(&start, _T("assets\\start.png"));
//	IMAGE sheep;
//	loadimage(&sheep, _T("assets\\sheep1.png"));
//	IMAGE sheep_mask;
//	loadimage(&sheep_mask, _T("assets\\sheep_mask.png"));
//	IMAGE bk1;
//	loadimage(&bk1, _T("assets\\bg1.png"));
//	IMAGE sheep0;
//	loadimage(&sheep0, _T("assets\\sheep0.png"));
//	IMAGE sheep_mask0;
//	loadimage(&sheep_mask0, _T("assets\\sheep_mask0.png"));
//	IMAGE sheep15;
//	loadimage(&sheep15, _T("assets\\sheep15.png"));
//	IMAGE sheep_mask15;
//	loadimage(&sheep_mask15, _T("assets\\sheep_mask15.png"));
//	IMAGE sheep2;
//	loadimage(&sheep2, _T("assets\\sheep2.png"));
//	IMAGE sheep_mask2;
//	loadimage(&sheep_mask2, _T("assets\\sheep_mask2.png"));
//	clock_t startTime = 0;
//	clock_t procedureTime = 0;
//	int k = 0;
//	
//	int flag=0;
//	int stage = 0;
//	
//	
//	while (true)
//	{
//		{
//			if (seed >= 900)
//				flag = 1;
//			if (seed <= 300)
//				flag = 0;
//			
//		}
//		
//		
//
//		switch (stage)
//		{
//		case 0:
//			{
//			BeginBatchDraw();
//			startTime = clock();
//			putimage(0, 0, &start);
//			showStart(seed);
//			ExMessage msg;
//			peekmessage(&msg);
//			if (msg.message == WM_LBUTTONDOWN)
//			{
//				setbkcolor(RGB(95, 117, 64));
//				startEffect(1);
//				stage++;
//				setbkmode(TRANSPARENT);
//			}
//			EndBatchDraw();
//			procedureTime = clock() - startTime;
//			if (FPS - procedureTime > 0)
//				Sleep(FPS - procedureTime);
//			break;
//			}
//		case 1:
//			{
//			BeginBatchDraw();
//			startTime = clock();
//			cleardevice();
//			putimage(0, 0, &bk1);
//			sheepHeadController(&sheep15, &sheep_mask15, x, y, speed);
//			
//
//			EndBatchDraw();
//			
//		
//			procedureTime = clock() - startTime;
//			if (FPS - procedureTime > 0)
//				Sleep(FPS - procedureTime);
//			}
//		default:break;
//		}
//		
//		
//
//		{
//			
//			flag?seed -= pix: seed += pix;
//		}
//	}
//
//
//	system("pause");
//	return 0;
//}

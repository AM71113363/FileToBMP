// Copyright (c) 2021 AM71113363

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


char szClassName[ ] = "WindowsAppFIleToBMP";


void CenterOnScreen();
HINSTANCE ins;
HWND hWnd; 
UCHAR DropFile[MAX_PATH];

#define SMS(_x_)  MessageBox(hWnd,_x_,"#Error",MB_OK |MB_ICONERROR)

void ConvertToBmp()
{
     BITMAPFILEHEADER f; UINT full, width, height, extra, extra1, bytesize;
     BITMAPINFOHEADER i;
     DWORD fSize=0, dwRead=0, bytes=0;
     HANDLE sss,vvv;
     UCHAR *p; UCHAR BmpFile[1024];
     
     strcpy(BmpFile,DropFile);
     
     p=strrchr(BmpFile,'\\'); //p=strrchr(BmpFile,'/');
     if(!p)
        return;
     
     p=strrchr(BmpFile,'.');
     if(p) //file have EXT
     {
        strcpy(p,".bmp\0");
     }
     else //no EXT
     {
        strcat(BmpFile,".bmp");
     }
//open file to READ
         
     sss = CreateFile(DropFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
     if(sss == INVALID_HANDLE_VALUE)
     {
        SMS("OPen FIle FAILED!!!");                                 
        return;
     }
     fSize = GetFileSize(sss, NULL);
     if(fSize == 0xFFFFFFFF)
     {
        SMS("GetFileSize FAILED!!!");                                  
        CloseHandle(sss);
        return;
     }            
//open the new BMP file
     vvv = CreateFile(BmpFile, GENERIC_WRITE, 0, NULL,OPEN_ALWAYS, 0, 0);
     if(vvv == INVALID_HANDLE_VALUE)
     {
        SMS("Create BMP File FAILED!!!");                                  
        CloseHandle(sss);
        return;
     }
     
     ////////////////////////////////////////////////////////
      f.bfType = 0x4d42;   
      f.bfSize = 0;     
      f.bfReserved1 = 0;
      f.bfReserved2 = 0;
      f.bfOffBits = 0x36;
     
      full=(fSize+(3-fSize%3))/3;
      width=floor(sqrt(full))+1;
      extra = (4 - (width * 3) % 4) % 4;
      height=floor(full/width)+1;
      extra1=(width*height)-full;
      bytesize = (width * 3 + extra) * height;
      extra=bytesize-(width*height*3)+(extra1*3);
     
      i.biSize = sizeof(BITMAPINFOHEADER);
      i.biWidth = width;
      i.biHeight = height;
      i.biPlanes = 1;
      i.biBitCount = 24;           
      i.biCompression = BI_RGB;
      i.biSizeImage = bytesize;     
      i.biXPelsPerMeter = 0;
      i.biYPelsPerMeter = 0;
      i.biClrUsed = 0;
      i.biClrImportant = 0;
///////////////////////////////////////////////
      if(WriteFile(vvv,&f,sizeof(BITMAPFILEHEADER),&bytes,NULL) == 0)
      {
         SMS("WriteFile FAILED!!!\nDelete the BMP file.");                                  
         CloseHandle(sss);
         CloseHandle(vvv);
         return;
      }
      if(WriteFile(vvv,&i,sizeof(BITMAPINFOHEADER),&bytes,NULL) == 0)
      {
         SMS("WriteFile FAILED!!!\nDelete the BMP file.");                                  
         CloseHandle(sss);
         CloseHandle(vvv);
         return;
      }
//put the real file to BMP

      do
      {
          if(ReadFile(sss, BmpFile, 1024, &dwRead, NULL) == 0)
          {
             SMS("ReadFile FAILED!!!\nDelete the BMP file.");                                  
             CloseHandle(sss);
             CloseHandle(vvv);
             return;
          }
          if(dwRead>0)
          {
                if(WriteFile(vvv,BmpFile,dwRead,&bytes,NULL) == 0)
                {
                   SMS("WriteFile!!!\nDelete the BMP file.");                                  
                   CloseHandle(sss);
                   CloseHandle(vvv);
                   return;
                }
          }else{ break; }
      }while(dwRead>0);
    
//add extra 0x00
     dwRead = extra+3;
     
     memset(BmpFile,0,dwRead+1);
	
     if(WriteFile(vvv,BmpFile,dwRead,&bytes,NULL) == 0)
     {
          SMS("WriteFile!!!\nDelete the BMP file.");                                  
          CloseHandle(sss);
          CloseHandle(vvv);
          return;
    }
    CloseHandle(sss);
    CloseHandle(vvv);
    SetWindowText(hWnd,"DONE");
    _sleep(1000);
    SetWindowText(hWnd,"File To BMP");
       
 }


LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
        {
             hWnd = hwnd;
             CreateWindow("STATIC","Drop a File here",WS_CHILD|WS_VISIBLE,12,8,109,20,hwnd,NULL,ins,NULL);
             CenterOnScreen();
             DragAcceptFiles(hwnd,1);
        }
        break; 
        case WM_DROPFILES: 
        {
		     memset(DropFile,0,MAX_PATH);
		     DragQueryFile((HDROP)wParam, 0, DropFile, MAX_PATH);
		     DragFinish((HDROP) wParam);
		     if((GetFileAttributes(DropFile) & FILE_ATTRIBUTE_DIRECTORY)==FILE_ATTRIBUTE_DIRECTORY)
             {
                SMS("Drop a File and not a folder!!!");
                break;
             }
		     CreateThread(0,0,(LPTHREAD_START_ROUTINE)ConvertToBmp,0,0,0);
       }
       break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
            
    MSG messages;    
    WNDCLASSEX wincl;  
    HWND hwnd;       
    ins=hThisInstance;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;  
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (ins,MAKEINTRESOURCE(200));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;  
    wincl.cbClsExtra = 0;  
    wincl.cbWndExtra = 0;      
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND+1;

    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx(WS_EX_TOPMOST,szClassName,"File To BMP",WS_SYSMENU,CW_USEDEFAULT,CW_USEDEFAULT,
    140,70,HWND_DESKTOP,NULL,hThisInstance,NULL );
    
    ShowWindow (hwnd, nFunsterStil);

    while (GetMessage (&messages, NULL, 0, 0))
    {
         TranslateMessage(&messages);
         DispatchMessage(&messages);
    }

     return messages.wParam;
}

void CenterOnScreen()
{
     RECT rcClient, rcDesktop;
	 int nX, nY;
     SystemParametersInfo(SPI_GETWORKAREA, 0, &rcDesktop, 0);
     GetWindowRect(hWnd, &rcClient);
	 nX=((rcDesktop.right - rcDesktop.left) / 2) -((rcClient.right - rcClient.left) / 2);
	 nY=((rcDesktop.bottom - rcDesktop.top) / 2) -((rcClient.bottom - rcClient.top) / 2);
SetWindowPos(hWnd, NULL, nX, nY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


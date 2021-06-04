// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "ShellWindowActor.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Widgets/SWindow.h"
#include "Windows/WindowsPlatformProcess.h"
#include "GenericPlatform/GenericWindow.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"
#include "UnrealClient.h"

HWND AShellWindowActor::hWnd_win;
TMap<int, TArray<HWND>> AShellWindowActor::ThirdWndArray;

void AShellWindowActor::Test()
{
	//const TCHAR* WindowClassName = TEXT("TestWindow");

	//// Register a dummy window class.
	//static bool bInitializedWindowClass = false;
	//if (!bInitializedWindowClass)
	//{
	//	WNDCLASS wc;

	//	bInitializedWindowClass = true;
	//	FMemory::Memzero(wc);
	//	wc.style = CS_OWNDC;
	//	wc.lpfnWndProc = MainWndProc;
	//	wc.cbClsExtra = 0;
	//	wc.cbWndExtra = 0;
	//	wc.hInstance = NULL;
	//	wc.hIcon = NULL;
	//	wc.hCursor = NULL;
	//	wc.hbrBackground = (HBRUSH)(COLOR_MENUTEXT);
	//	wc.lpszMenuName = NULL;
	//	wc.lpszClassName = WindowClassName;
	//	ATOM ClassAtom = ::RegisterClass(&wc);
	//	check(ClassAtom);
	//}

	//// Create a dummy window.
	//HWND WindowHandle = CreateWindowEx(
	//	WS_EX_WINDOWEDGE,
	//	WindowClassName,
	//	TEXT("TestWindowName"),
	//	WS_POPUP,
	//	500,500, 1, 1,
	//	//CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	//	NULL, NULL, NULL, NULL);
	//check(WindowHandle);







	/*HINSTANCE HInstance = (HINSTANCE)GetModuleHandle(NULL);

	
	ZeroMemory(&WndClassEx, sizeof(WndClassEx));
	WndClassEx.cbSize = sizeof(WndClassEx);
	WndClassEx.style = CS_HREDRAW | CS_VREDRAW;
	WndClassEx.lpfnWndProc = &AShellWindowActor::MainWndProc;
	WndClassEx.hIcon = NULL//LoadIcon(HInstance, MAKEINTRESOURCE(FWindowsPlatformApplicationMisc::GetAppIcon()));
	WndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClassEx.hInstance = HInstance;
	WndClassEx.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	WndClassEx.lpszClassName = TEXT("TestWindow");
	ATOM WndClassAtom = RegisterClassEx(&WndClassEx);

	DWORD WindowStyle = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;

	const TCHAR* WindowClassName = MAKEINTATOM(WndClassAtom);
	hWnd_win = CreateWindow(WindowClassName, TEXT("TestWindowName"), WindowStyle, 0, 0, 1280, 720, NULL, NULL, HInstance, NULL);

	if (!hWnd_win)
	{
		return;
	}
	::ShowWindow(hWnd_win, SW_MAXIMIZE);

	HWND hWnd_ue = (HWND)GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();

	::SetParent(hWnd_ue, hWnd_win);

	::ShowWindow(hWnd_ue, SW_MAXIMIZE);*/


	//HWND hWnd_ue = (HWND)GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	//::ShowWindow(hWnd_ue, SW_MAXIMIZE);

	FString CmdParams;
	for (const FString& Param : CmdArgs)
	{
		CmdParams += Param + " ";
	}
	ProcHandle = FPlatformProcess::CreateProc(*URL, *CmdParams, true, true, false, &ProcessId, 0, nullptr, nullptr, nullptr);
	::WaitForInputIdle(ProcHandle.Get(), 3000);

	if (ProcHandle.IsValid())
	{
		if (TimeToWaitWindow > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AShellWindowActor::TimerCallback, TimeToWaitWindow, true);
		}
		else
		{
			TimerCallback();
		}
	}
	

	//SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)&Params);
	//SendMessageW(hWnd, WM_SETFONT, (WPARAM)hFont, 0);
}

void AShellWindowActor::LaunchExe()
{
	Test();
}

void AShellWindowActor::BeginPlay()
{
	Super::BeginPlay();

	//GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(this, &AShellWindowActor::OnWindowSizeChanged);
	//FSlateApplication::Get().OnSizeChanged(this, &AShellWindowActor::OnWindowSizeChanged);
		//FSlateApplication::Get().OnResizingWindow
}

void AShellWindowActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	//DestroyWindow(hWnd_win);
	//UnregisterClass(TEXT("TestWindow"), (HINSTANCE)GetModuleHandle(NULL));

	if (ProcHandle.IsValid())
	{
		//FPlatformProcess::CloseProc(ProcHandle);
		if (ThirdWndArray.Contains(ProcessId))
		{
			ThirdWndArray.Remove(ProcessId);
		}
		FPlatformProcess::TerminateProc(ProcHandle);
		//ShowErrorMessage();
	}
	Super::EndPlay(EndPlayReason);
}

LRESULT AShellWindowActor::MainWndProc(HWND hWnd, uint32 Msg, WPARAM wParam, LPARAM lParam)
{
	TArray<APlayerController*> PlayerControllerArray;
	GEngine->GetAllLocalPlayerControllers(PlayerControllerArray);
	switch (Msg)
	{
	case WM_CLOSE:
		UE_LOG(LogTemp, Warning, TEXT("main window receive close message"));
		if (PlayerControllerArray.Num() > 0 && PlayerControllerArray[0])
		{
			PlayerControllerArray[0]->ConsoleCommand("quit");
			return 0;
		}
		break;
	case WM_PAINT:
		UE_LOG(LogTemp, Warning, TEXT("main window receive paint message"));
		break;
	case WM_MOVE:
		break;
	case WM_ERASEBKGND:
		UE_LOG(LogTemp, Warning, TEXT("main window receive erase bkgnd message"));
		//RedrawWindow(hWnd_win, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
		break;
	case WM_SIZE:
		/*for (HWND ThirdWnd : ThirdWndArray)
		{
			::UpdateWindow(ThirdWnd);
			RedrawWindow(ThirdWnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
		}*/
		break;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

BOOL AShellWindowActor::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD TargetProcessId = (DWORD)lParam;
	DWORD WindowProcessId;
	GetWindowThreadProcessId(hwnd, &WindowProcessId);

	//TCHAR WindowText[256];
	//GetWindowText(hwnd, WindowText, 1024);
	//UE_LOG(LogTemp, Warning, TEXT("target processId: %d, window processId: %d, WindowText: %s"), TargetProcessId, WindowProcessId, WindowText);
	if (WindowProcessId == TargetProcessId)
	{
		UE_LOG(LogTemp, Warning, TEXT("find target process and set parent"));
		//::EnumChildWindows(hwnd, EnumChildWindowsProc, lParam);
		//::SetWindowPos(hwnd, HWND_TOPMOST, 700, 0, 300, 200, NULL);

		
		LONG_PTR Style = ::GetWindowLongPtr(hwnd, GWL_STYLE);
		Style = Style & ~WS_CAPTION &~WS_SYSMENU & ~WS_SIZEBOX;
		::SetWindowLongPtr(hwnd, GWL_STYLE, Style | WS_CLIPSIBLINGS);

		::SetWindowLongPtr(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);

		HWND hWnd_ue = (HWND)GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
		//::ShowWindow(hwnd, SW_HIDE);
		::SetParent(hwnd, hWnd_ue);

		//::BringWindowToTop(hwnd);
		//ShowErrorMessage();

		//::InvalidateRect(hwnd, NULL, false);
		//::UpdateWindow(hwnd);

		if (ThirdWndArray.Contains(TargetProcessId))
		{
			ThirdWndArray[TargetProcessId].Add(hwnd);
		}
		else
		{
			TArray<HWND> NewProcessArray;
			NewProcessArray.Add(hwnd);
			ThirdWndArray.Add(TargetProcessId, NewProcessArray);
		}

		//::ShowWindow(hWnd_ue, SW_MAXIMIZE);
		//::MoveWindow(hwnd, PosX, PosY, Width, Height, true);

		//::ShowWindow(hwnd, SW_SHOW);
		//RedrawWindow(hWnd_win, NULL, NULL, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME);
		//return FALSE;
	}

	//::EnumChildWindows(hwnd, EnumWindowsProc, lParam);

	return TRUE;
}

BOOL AShellWindowActor::EnumChildWindowsProc(HWND hwnd, LPARAM lParam)
{
	DWORD TargetProcessId = (DWORD)lParam;
	DWORD WindowProcessId;
	GetWindowThreadProcessId(hwnd, &WindowProcessId);

	TCHAR WindowText[256];
	GetWindowText(hwnd, WindowText, 1024);
	UE_LOG(LogTemp, Warning, TEXT("child window, target processId: %d, window processId: %d, WindowText: %s"), TargetProcessId, WindowProcessId, WindowText);

	//HWND hWnd_ue = (HWND)GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
	//::SetParent(hwnd, hWnd_ue);

	::SetParent(hwnd, hWnd_win);
	::MoveWindow(hwnd, 700, 0, 640, 360, true);
	return FALSE;
}

void AShellWindowActor::ShowErrorMessage()
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	LPTSTR lpszFunction = TEXT("TestErrorMessage");

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

void AShellWindowActor::TimerCallback()
{
	UE_LOG(LogTemp, Warning, TEXT("timer start to find window"));

	if (FPlatformProcess::IsProcRunning(ProcHandle))
	{
		//GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
		::EnumChildWindows(NULL, &AShellWindowActor::EnumWindowsProc, (LPARAM)ProcessId);
	}
}

void AShellWindowActor::MoveThirdWindowTo(FVector2D NewPos, int NewWidth, int NewHeight)
{
	if (ThirdWndArray.Contains(ProcessId))
	{
		HDWP hdwp = ::BeginDeferWindowPos(ThirdWndArray[ProcessId].Num());
		for (HWND ThirdWnd : ThirdWndArray[ProcessId])
		{
			//UE_LOG(LogTemp, Warning, TEXT("move third window to, x:%f, y:%f, width: %d, height: %d"), NewPos.X, NewPos.Y, NewWidth, NewHeight);
			//::ShowWindow(ThirdWnd, SW_SHOWNA);
			//::MoveWindow(ThirdWnd, NewPos.X, NewPos.Y, NewWidth, NewHeight, true);

			::SetWindowLongPtr(ThirdWnd, GWL_EXSTYLE, GetWindowLongPtr(ThirdWnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT);
			::DeferWindowPos(hdwp, ThirdWnd, HWND_TOPMOST, NewPos.X, NewPos.Y, NewWidth, NewHeight, SWP_NOZORDER);
		}
		::EndDeferWindowPos(hdwp);

	}
}

void AShellWindowActor::OnWindowSizeChanged(FViewport * Viewport, uint32 Meaningless)
{
	if (Viewport)
	{
		FIntPoint NewSize = Viewport->GetSizeXY();
		if (ThirdWndArray.Contains(ProcessId))
		{
			for (HWND ThirdWnd : ThirdWndArray[ProcessId])
			{
				RECT Rect;
				::GetWindowRect(ThirdWnd, &Rect);
				::InvalidateRect(NULL, &Rect, false);
				//::UpdateWindow(ThirdWnd);
				//::MoveWindow(ThirdWnd, 300, 200, 1000, 600, true);
			}
			//UE_LOG(LogTemp, Warning, TEXT("new size, x:%d, y:%d"), NewSize.X, NewSize.Y);
		}
	}
}

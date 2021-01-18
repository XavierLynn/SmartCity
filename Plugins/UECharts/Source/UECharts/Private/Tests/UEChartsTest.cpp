// Copyright Epic Games, Inc. All Rights Reserved.

#include "UECharts.h"

DEFINE_LOG_CATEGORY_STATIC(TestLog,Log,All);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiThreadTest,"MyTest.PublicTest.MultiThreadTest",EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMultiThreadTest::RunTest(const FString& Parameters)
{
	
	UE_LOG(TestLog,Log,TEXT("Hello"));
	return true;
}

class FRunnableTestThread :public FRunnable {

public:
	FRunnableTestThread(int16 _Index):Index(_Index){}

	virtual bool Init() override
	{
		UE_LOG(TestLog, Log, TEXT("Thread %d Init"), Index);
		return true;
	}

	virtual uint32 Run() override
	{
	
		UE_LOG(TestLog, Log, TEXT("Thread %d Run:1"), Index);
		FPlatformProcess::Sleep(10.0f);
		UE_LOG(TestLog, Log, TEXT("Thread %d Run:1"), Index);
		FPlatformProcess::Sleep(10.0f);
		UE_LOG(TestLog, Log, TEXT("Thread %d Run:1"), Index);
		FPlatformProcess::Sleep(10.0f);
		return 0;
	}
	virtual void Exit() override
	{
		UE_LOG(TestLog, Log, TEXT("Thread %d Exit"), Index);
	}
private:
	int16 Index;
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFRunnableTest, "MyTest.PublicTest.RunnableTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FFRunnableTest::RunTest(const FString& Parameters)
{
	FRunnableThread::Create(new FRunnableTestThread(0), TEXT("TestThread0")) ;
	FRunnableThread::Create(new FRunnableTestThread(1), TEXT("TestThread1"));
	FRunnableThread::Create(new FRunnableTestThread(2), TEXT("TestThread2"));
	return true;
}
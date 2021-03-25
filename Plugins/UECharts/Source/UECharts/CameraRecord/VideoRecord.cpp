// Fill out your copyright notice in the Description page of Project Settings.

//
//#include "VideoRecord.h"
//#include "VideoRecordingSystem.h"
//#include "PlatformFeatures.h"
//bool UVideoRecord::StartRecording(const FString& Name)
//{
//	IVideoRecordingSystem* VideoRecordSystem = IPlatformFeaturesModule::Get().GetVideoRecordingSystem();
//	VideoRecordSystem->EnableRecording(true);
//	FVideoRecordingParameters param;
//	param.RecordingLengthSeconds = 6000;
//	VideoRecordSystem->NewRecording(*Name, param);
//	VideoRecordSystem->StartRecording();
//	return true;
//}
//bool UVideoRecord::StopRecording()
//{
//	IVideoRecordingSystem* VideoRecordSystem = IPlatformFeaturesModule::Get().GetVideoRecordingSystem();
//	VideoRecordSystem->FinalizeRecording(true, FText::FromString("Hello"), FText::FromString("World"));
//	return true;
//}
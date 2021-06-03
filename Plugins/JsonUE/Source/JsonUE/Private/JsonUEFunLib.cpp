// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonUEFunLib.h"
#include "JsonPath.h"

bool UJsonUEFunLib::Parse(UJsonPath*& JsonObject, FString jsonStr) {
	if(!JsonObject)JsonObject = NewObject<UJsonPath>();
	return jsonStr.Len() ? JsonObject->FromString(jsonStr) : true;
}

/*
FString path;
FString string;
FFileHelper::SaveStringToFile(string, *path);
¶ÁÈ¡ÎÄ¼þ
FString path;
FString string;
FFileHelper::LoadFileToString(string, *path);
	*/
bool UJsonUEFunLib::ParseFile(UJsonPath*& JsonObject, FString jsonFile) {
	if (!JsonObject)JsonObject = NewObject<UJsonPath>();
	FString content;
	if (0 == jsonFile.Len())return true;
	FFileHelper::LoadFileToString(content, *jsonFile);
	if (0 == content.Len())return true;
	return Parse(JsonObject, content);
}
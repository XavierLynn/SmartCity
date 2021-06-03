// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "jsonue/jsonue.h"

#include "JsonPath.generated.h"

/**
 * path: {name}[0][didian]  OR  name}0]didian}
 *			} : is object ] : is array
 * example:
 *         
 */
UCLASS(BlueprintType, Blueprintable, meta = (ToolTip = "json format or parse tools"))
class JSONUE_API UJsonPath : public UObject
{
	GENERATED_BODY()
public:
	/**
	 * Delete one node
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	void Del(FString path);

	/**
	 * Add one null node 
	 * path: 
	 *       }/] is object/array 
	 * example: {name}[0] <==> name}0] 
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	void Add(FString path);

	void AddUInt(FString path, uint32 v);

	void AddUInt64(FString path, uint64 v);

	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	void AddInt(FString path, int v);
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	void AddInt64(FString path, int64 v);
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	void AddString(FString path, FString v);
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	void AddDouble(FString path, float v, uint8 precision = 5);
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	void AddDoubleS(FString path, FString v, uint8 precision = 8);
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	void AddBool(FString path, bool v);


	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	FString asString(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	int asInt(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	int64 asInt64(FString path) const;

	//UFUNCTION(BlueprintCallable, Category = "JsonUE")
	uint64 asUInt64(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	float asFloat(FString path) const;

	double asDouble(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool asBool(FString path) const;

	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isNull(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isBool(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isInt64(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isUInt64(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isIntegral(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isDouble(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isNumeric(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isString(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isArray(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isObject(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool isContain(FString path) const;
	/**
	 * path:
	 *       }/] is object/array
	 * example: {name}[0] <==> name}0]
	 */
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	int32 size(FString path) const;

	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	FString ToString() const;
	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "path: }/]=>object/array e.g {name}[0] <==> name}0] "))
	bool FromString(FString jsonString);

	UFUNCTION(BlueprintCallable, Category = "JsonUE", meta = (ToolTip = "translate to string and write into file"))
	bool ToFile(FString filePath);

	UPROPERTY(EditAnywhere, Category = "JsonUE", meta = (ToolTip = "format is true , otherwise false , defalut is false. "))
	bool isFormat = false ; 
private:
	JsonUE::Value& find(const TCHAR* begin, JsonUE::Value& root) const;
	bool exist(const TCHAR* begin, JsonUE::Value& root) const;
	bool Del(const TCHAR* begin, JsonUE::Value& root) ;
private:
	mutable JsonUE::Value json;
};

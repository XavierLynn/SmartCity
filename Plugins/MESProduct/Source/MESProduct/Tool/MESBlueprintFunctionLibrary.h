// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MediaAssets/Public/MediaTexture.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine\Classes\Components\PrimitiveComponent.h"
#include "MESBlueprintFunctionLibrary.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWindowActionDelegate, FString, action);
DECLARE_DYNAMIC_DELEGATE_OneParam(FWindowActionSingleDelegate, FString, data);
/**
 * 
 */
UCLASS()
class MESPRODUCT_API UMESBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		UFUNCTION(BlueprintPure, Category = "MESUtil")
		static bool RegexMatch(const FString& Str, const FString& Pattern,TArray<FString>& StringResult);

	    UFUNCTION(BlueprintPure, Category = "MESUtil")
		static FLinearColor HexToRGB(FText Hex);

	    UFUNCTION(BlueprintPure, Category = "MESUtil")
		static FText RGBToHex(FLinearColor Color);
	
		UFUNCTION(BlueprintCallable, Category = "MESUtil")
        static void SetWindowMaxSize();

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
		static void SetWindowRestore();

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
		static void SetWindowMinSize();

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
				static FString DateTimeToDateString(FDateTime date);

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
			static FString DateTimeToTimeString(FDateTime date);

		UFUNCTION(BlueprintCallable, Category = "MESFunc")
				static TArray<FString>  GetAllStreamLevel(UObject* Outer, FString LevelPath);

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
				static void CallPrimitiClick(UPrimitiveComponent* primitive);

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
			static void SetStaticObject(FString ObjKey, UObject* obj);

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
			static void RemoiveStaticObject(FString ObjKey);

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
			static UObject* GetStaticObject(FString ObjKey);
private:

	static FDelegateHandle WindowActionNotificationHandle;
	static TMap<FString, UObject*> StaticObjectMap;

public:
		static FWindowActionDelegate windowAction;

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
        static void BindWindowAction(FWindowActionSingleDelegate action);

		UFUNCTION(BlueprintCallable, Category = "MESUtil")
		static void UnBindWindowAction(FWindowActionSingleDelegate action);

		UFUNCTION(Category = "MESUtil", BlueprintPure, Meta = (DisplayName = "IsEmpty"))
			static bool StringIsEmpty(const FString& Target);

		UFUNCTION(Category = "MESUtil", BlueprintPure)
			static void Int64ToString(int64 Source, FString& OutString);

		UFUNCTION(Category = "MESUtil", BlueprintPure)
			static int64 StringToInt64(FString SourceString);

		UFUNCTION(Category = "MESUtil", BlueprintCallable, BlueprintCosmetic, Meta = (DefaultToSelf = "ParentWidget", DeterminesOutputType = "WidgetClass", DynamicOutputParam = "ChildWidgets"))
			static void WidgetGetChildrenOfClass(class UWidget* ParentWidget, TArray<class UUserWidget*>& ChildWidgets, TSubclassOf<class UUserWidget> WidgetClass, bool bImmediateOnly);

		//创建视频的材质
		UFUNCTION(BlueprintCallable, Category = "MESUtil")
		static UMediaTexture* CreateMediaTexture(UObject* Outer, UMediaPlayer* MediaPlayer);

		static bool GenericArray_SortCompare(const FProperty* LeftProperty, void* LeftValuePtr, const FProperty* RightProperty, void* RightValuePtr);
		/**
 *	Sort the elements of an array by FString, FName, FText, float, int or boolean.
 *	Supports struct and object based arrays.
 *
 *	@param	TargetArray		The array to sort.
 *	@param	bAscendingOrder	If true, sort by ascending order.
 *	@param	VariableName	If a struct or object based array, the name of the variable to sort by.
 */
		UFUNCTION(Category = "Victory BP Library|Utilities|Array", BlueprintCallable, CustomThunk, Meta = (DisplayName = "Sort", ArrayParm = "TargetArray", AdvancedDisplay = "bAscendingOrder,VariableName"))
		static void Array_Sort(const TArray<int32>& TargetArray, bool bAscendingOrder = true, FName VariableName = NAME_None);

	static void GenericArray_Sort(void* TargetArray, const FArrayProperty* ArrayProp, bool bAscendingOrder = true, FName VariableName = NAME_None);

	DECLARE_FUNCTION(execArray_Sort)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* ArrayAddr = Stack.MostRecentPropertyAddress;
		FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);
		if (!ArrayProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_GET_UBOOL(bAscendingOrder);

		P_GET_PROPERTY(FNameProperty, VariableName);

		P_FINISH;
		P_NATIVE_BEGIN;
		GenericArray_Sort(ArrayAddr, ArrayProperty, bAscendingOrder, VariableName);
		P_NATIVE_END;
	}
	

};

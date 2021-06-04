// Fill out your copyright notice in the Description page of Project Settings.


#include "MESBlueprintFunctionLibrary.h"
#include "Engine/GameEngine.h"
#include "Components/Widget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Framework/Application/SlateApplication.h"
#include "EngineGlobals.h"
#include "Internationalization\Regex.h"

bool UMESBlueprintFunctionLibrary::RegexMatch(const FString& Str, const FString& Pattern,TArray<FString>& StringResult)
{
	FRegexPattern MatherPatter(Pattern);
	FRegexMatcher Matcher(MatherPatter, Str);

	while (Matcher.FindNext())
	{
		StringResult.Add(Matcher.GetCaptureGroup(0));

	}

	return StringResult.Num() == 0 ? false : true;
	
}

FLinearColor UMESBlueprintFunctionLibrary::HexToRGB(FText Hex)
{
	if (!Hex.IsEmpty())
	{
		FColor Color = FColor::FromHex(Hex.ToString());
		float red = Color.R / 255.0f;
		float green = Color.G / 255.0f;
		float blue = Color.B / 255.0f;
		float alpha = Color.A / 255.0f;

		red = red <= 0.04045f ? red / 12.92f : FMath::Pow((red + 0.055f) / 1.055f, 2.4f);
		green = green <= 0.04045f ? green / 12.92f : FMath::Pow((green + 0.055f) / 1.055f, 2.4f);
		blue = blue <= 0.04045f ? blue / 12.92f : FMath::Pow((blue + 0.055f) / 1.055f, 2.4f);

		return FLinearColor(red, green, blue);
	}
	return FLinearColor(0, 0, 0);
}

FText UMESBlueprintFunctionLibrary::RGBToHex(FLinearColor Color)
{
	return FText::FromString(Color.ToFColor(true).ToHex());
}

void UMESBlueprintFunctionLibrary::SetWindowMaxSize()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
	{
		TSharedPtr<SWindow> windowPtr = GEngine->GameViewport->GetWindow();
		if (windowPtr.IsValid())
		{
			TSharedPtr<FGenericWindow> NativeWIndow = windowPtr->GetNativeWindow();
			if (NativeWIndow.IsValid())
			{
				if (!NativeWIndow->IsMaximized())
				{
					NativeWIndow->Maximize();
				}
			}
		}
	}
}

void UMESBlueprintFunctionLibrary::SetWindowRestore()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
	{
		TSharedPtr<SWindow> windowPtr = GEngine->GameViewport->GetWindow();
		if (windowPtr.IsValid())
		{
			TSharedPtr<FGenericWindow> NativeWIndow = windowPtr->GetNativeWindow();
			if (NativeWIndow.IsValid())
			{
				if (NativeWIndow->IsMaximized())
				{
					NativeWIndow->Restore();
				}
			}
		}
	}
}

void UMESBlueprintFunctionLibrary::SetWindowMinSize()
{
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->GetWindow().IsValid())
	{
		TSharedPtr<SWindow> windowPtr = GEngine->GameViewport->GetWindow();
		if (windowPtr.IsValid())
		{
			TSharedPtr<FGenericWindow> NativeWIndow = windowPtr->GetNativeWindow();
			if (NativeWIndow.IsValid())
			{
				NativeWIndow->Minimize();
			}
		}
	}
}

FString UMESBlueprintFunctionLibrary::DateTimeToDateString(FDateTime date)
{
	return date.ToString(TEXT("%Y.%m.%d"));
}

FString UMESBlueprintFunctionLibrary::DateTimeToTimeString(FDateTime date)
{
	return date.ToString(TEXT("%H:%M:%S"));
}

TArray<FString> UMESBlueprintFunctionLibrary::GetAllStreamLevel(UObject* Outer, FString LevelPath)
{
	TArray<FString> AllPath;
	FString symbol = TEXT("'");
	int FirstIndex = LevelPath.Find(symbol);
	if (FirstIndex != -1)
	{
		int LastIndex = LevelPath.FindLastCharByPredicate([](const TCHAR& c) {return c == '.'; });
		LevelPath = LevelPath.Left(LevelPath.Len() - 1).Right(LevelPath.Len() - FirstIndex - 2);
	}
	UObject* result = LoadObject<UObject>(NULL, *LevelPath, nullptr, LOAD_Quiet | LOAD_NoWarn);
	if (result == nullptr)return AllPath;
	AllPath.Add(LevelPath);
	UWorld* World = Cast<UWorld>(result);
	if (World == nullptr)return AllPath;
	TArray<ULevelStreaming*> levels = World->GetStreamingLevels();
	for (int i = 0; i < levels.Num(); i++)
	{
		ULevelStreaming* A = levels[i];
		FString Path = A->GetWorldAssetPackageFName().ToString();
		if (!AllPath.Contains(Path))
		{
			AllPath.Add(Path);
		}
	}
	return AllPath;
}

void UMESBlueprintFunctionLibrary::CallPrimitiClick(UPrimitiveComponent* primitive)
{
	primitive->DispatchOnClicked();
}

void UMESBlueprintFunctionLibrary::SetStaticObject(FString ObjKey, UObject* obj)
{
	if (StaticObjectMap.Contains(ObjKey))
	{
		StaticObjectMap[ObjKey] = obj;
	}
	else
	{
		StaticObjectMap.Add(ObjKey, obj);
	}
}

void UMESBlueprintFunctionLibrary::RemoiveStaticObject(FString ObjKey)
{
	StaticObjectMap.Remove(ObjKey);
}

UObject* UMESBlueprintFunctionLibrary::GetStaticObject(FString ObjKey)
{
	if (StaticObjectMap.Contains(ObjKey))
	{
		return StaticObjectMap[ObjKey];
	}
	return nullptr;
}

FDelegateHandle UMESBlueprintFunctionLibrary::WindowActionNotificationHandle;

TMap<FString, UObject*> UMESBlueprintFunctionLibrary::StaticObjectMap = TMap<FString, UObject*>();

FWindowActionDelegate UMESBlueprintFunctionLibrary::windowAction;

static bool HandleWindowAction(const TSharedRef<FGenericWindow>& PlatformWindow, EWindowAction::Type WindowAction)
{
	FString res = TEXT("");
	if (UMESBlueprintFunctionLibrary::windowAction.IsBound())
	{
		if (WindowAction == EWindowAction::Restore)
		{
			UMESBlueprintFunctionLibrary::windowAction.Broadcast("Restore");
				return false;
		}
		if (WindowAction == EWindowAction::Maximize)
		{
			UMESBlueprintFunctionLibrary::windowAction.Broadcast("Maximize");
				return false;
		}
	}
	return false;
}

void UMESBlueprintFunctionLibrary::BindWindowAction(FWindowActionSingleDelegate action)
{
	UMESBlueprintFunctionLibrary::windowAction.Add(action);
	if (WindowActionNotificationHandle.IsValid())
	{
		return;
	}
	FOnWindowAction actionDel;
	actionDel.BindStatic(HandleWindowAction);
	WindowActionNotificationHandle = FSlateApplication::Get().RegisterOnWindowActionNotification(actionDel);
}

void UMESBlueprintFunctionLibrary::UnBindWindowAction(FWindowActionSingleDelegate action)
{
	UMESBlueprintFunctionLibrary::windowAction.Remove(action);
}

bool UMESBlueprintFunctionLibrary::StringIsEmpty(const FString & Target)
{
	return Target.IsEmpty();
}

void UMESBlueprintFunctionLibrary::Int64ToString(int64 Source, FString & OutString)
{
	TArray<FStringFormatArg> FormatArg;
	FormatArg.Add(Source);
	OutString = FString::Format(TEXT("{0}"), FormatArg);
}

int64 UMESBlueprintFunctionLibrary::StringToInt64(FString SourceString)
{
	return FCString::Atoi64(*SourceString);
}

void UMESBlueprintFunctionLibrary::WidgetGetChildrenOfClass(UWidget * ParentWidget, TArray<UUserWidget*>& ChildWidgets, TSubclassOf<UUserWidget> WidgetClass, bool bImmediateOnly)
{
	ChildWidgets.Empty();

	if (ParentWidget && WidgetClass)
	{
		// Current set of widgets to check
		TInlineComponentArray<UWidget*> WidgetsToCheck;

		// Set of all widgets we have checked
		TInlineComponentArray<UWidget*> CheckedWidgets;

		WidgetsToCheck.Push(ParentWidget);

		// While still work left to do
		while (WidgetsToCheck.Num() > 0)
		{
			// Get the next widgets off the queue
			const bool bAllowShrinking = false;
			UWidget* PossibleParent = WidgetsToCheck.Pop(bAllowShrinking);

			// Add it to the 'checked' set, should not already be there!
			if (!CheckedWidgets.Contains(PossibleParent))
			{
				CheckedWidgets.Add(PossibleParent);

				TArray<UWidget*> Widgets;

				UWidgetTree::GetChildWidgets(PossibleParent, Widgets);

				for (UWidget* Widget : Widgets)
				{
					if (!CheckedWidgets.Contains(Widget))
					{
						// Add any widget that is a child of the class specified.
						if (Widget->GetClass()->IsChildOf(WidgetClass))
						{
							ChildWidgets.Add(Cast<UUserWidget>(Widget));
						}

						// If we're not just looking for our immediate children,
						// add this widget to list of widgets to check next.
						if (!bImmediateOnly)
						{
							WidgetsToCheck.Push(Widget);
						}
					}
				}

				if (bImmediateOnly)
				{
					break;
				}
			}
		}
	}
}

UMediaTexture* UMESBlueprintFunctionLibrary::CreateMediaTexture(UObject* Outer, UMediaPlayer* MediaPlayer)
{
	UMediaTexture* MediaTexture = NewObject<UMediaTexture>(Outer, UMediaTexture::StaticClass(), NAME_None, RF_Transient | RF_Public);

	if (MediaTexture != nullptr)
	{
		MediaTexture->SetMediaPlayer(MediaPlayer);
#undef UpdateResource
		MediaTexture->UpdateResource();
	}

	return MediaTexture;
}

bool UMESBlueprintFunctionLibrary::GenericArray_SortCompare(const FProperty* LeftProperty, void* LeftValuePtr, const FProperty* RightProperty, void* RightValuePtr)
{
	bool bResult = false;

	if (const FNumericProperty *LeftNumericProperty = CastField<const FNumericProperty>(LeftProperty))
	{
		if (LeftNumericProperty->IsFloatingPoint())
		{
			bResult = (LeftNumericProperty->GetFloatingPointPropertyValue(LeftValuePtr) < CastField<const FNumericProperty>(RightProperty)->GetFloatingPointPropertyValue(RightValuePtr));
		}
		else if (LeftNumericProperty->IsInteger())
		{
			bResult = (LeftNumericProperty->GetSignedIntPropertyValue(LeftValuePtr) < CastField<const FNumericProperty>(RightProperty)->GetSignedIntPropertyValue(RightValuePtr));
		}
	}
	else if (const FBoolProperty* LeftBoolProperty = CastField<const FBoolProperty>(LeftProperty))
	{
		bResult = (!LeftBoolProperty->GetPropertyValue(LeftValuePtr) && CastField<const FBoolProperty>(RightProperty)->GetPropertyValue(RightValuePtr));
	}
	else if (const FNameProperty* LeftNameProperty = CastField<const FNameProperty>(LeftProperty))
	{
		bResult = (LeftNameProperty->GetPropertyValue(LeftValuePtr).ToString() < CastField<const FNameProperty>(RightProperty)->GetPropertyValue(RightValuePtr).ToString());
	}
	else if (const FStrProperty* LeftStringProperty = CastField<const FStrProperty>(LeftProperty))
	{
		bResult = (LeftStringProperty->GetPropertyValue(LeftValuePtr) < CastField<const FStrProperty>(RightProperty)->GetPropertyValue(RightValuePtr));
	}
	else if (const FTextProperty* LeftTextProperty = CastField<const FTextProperty>(LeftProperty))
	{
		bResult = (LeftTextProperty->GetPropertyValue(LeftValuePtr).ToString() < CastField<const FTextProperty>(RightProperty)->GetPropertyValue(RightValuePtr).ToString());
	}

	return bResult;
}

void UMESBlueprintFunctionLibrary::Array_Sort(const TArray<int32>& TargetArray, bool bAscendingOrder /*= true*/, FName VariableName /*= NAME_None*/)
{
	check(0);
}

void UMESBlueprintFunctionLibrary::GenericArray_Sort(void* TargetArray, const FArrayProperty* ArrayProp, bool bAscendingOrder /*= true*/, FName VariableName /*= NAME_None*/)
{
	if (TargetArray)
	{
		FScriptArrayHelper ArrayHelper(ArrayProp, TargetArray);
		const int32 LastIndex = ArrayHelper.Num();

		if (const FObjectProperty* ObjectProperty = CastField<const FObjectProperty>(ArrayProp->Inner))
		{
			for (int32 i = 0; i < LastIndex; ++i)
			{
				for (int32 j = 0; j < LastIndex - 1 - i; ++j)
				{
					UObject* LeftObject = ObjectProperty->GetObjectPropertyValue(ArrayHelper.GetRawPtr(j));
					UObject* RightObject = ObjectProperty->GetObjectPropertyValue(ArrayHelper.GetRawPtr(j + 1));

					FProperty* LeftProperty = FindFProperty<FProperty>(LeftObject->GetClass(), VariableName);
					FProperty* RightProperty = FindFProperty<FProperty>(RightObject->GetClass(), VariableName);

					if (LeftProperty && RightProperty)
					{
						void* LeftValuePtr = LeftProperty->ContainerPtrToValuePtr<void>(LeftObject);
						void* RightValuePtr = RightProperty->ContainerPtrToValuePtr<void>(RightObject);

						if (GenericArray_SortCompare(LeftProperty, LeftValuePtr, RightProperty, RightValuePtr) != bAscendingOrder)
						{
							ArrayHelper.SwapValues(j, j + 1);
						}
					}
				}
			}
		}
		else
		{
			FProperty* Property = nullptr;

			if (const FStructProperty* StructProperty = CastField<const FStructProperty>(ArrayProp->Inner))
			{
				Property = FindFProperty<FProperty>(StructProperty->Struct, VariableName);
			}
			else
			{
				Property = ArrayProp->Inner;
			}

			if (Property)
			{
				for (int32 i = 0; i < LastIndex; ++i)
				{
					for (int32 j = 0; j < LastIndex - 1 - i; ++j)
					{
						void* LeftValuePtr = Property->ContainerPtrToValuePtr<void>(ArrayHelper.GetRawPtr(j));
						void* RightValuePtr = Property->ContainerPtrToValuePtr<void>(ArrayHelper.GetRawPtr(j + 1));

						if (GenericArray_SortCompare(Property, LeftValuePtr, Property, RightValuePtr) != bAscendingOrder)
						{
							ArrayHelper.SwapValues(j, j + 1);
						}
					}
				}
			}
		}
	}
}

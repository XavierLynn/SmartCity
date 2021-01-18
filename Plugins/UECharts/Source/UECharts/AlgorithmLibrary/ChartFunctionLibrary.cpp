// Fill out your copyright notice in the Description page of Project Settings.


#include "ChartFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


float UChartFunctionLibrary::CeilNumber(float Number)
{
	int Bite = 0;
	if (0<Number && Number <10)
	{
		return 10;
	}
	while (Number>=10)
	{
		Number /= 10;
		Bite += 1;
	}

	

	Number = Number == (int)Number ? Number + 1 : Number;
	return FMath::CeilToFloat(Number) * FMath::Pow(10, Bite);
}


FString  UChartFunctionLibrary::NumberToString(float Number, int DecimalDigits) 
{

	int scaleN = FMath::Pow(10, DecimalDigits);
	float scaleF = scaleN;
	return FString::SanitizeFloat(FMath::RoundToInt(Number * scaleN)/scaleF, DecimalDigits);


}

void UChartFunctionLibrary::ChartDrawLine(FPaintContext& Context, FVector2D PositionA, FVector2D PositionB, FLinearColor Tint /*= FLinearColor::White*/, bool bAntiAlias /*= true*/, float Thickness /*= 1.0f*/, int SubLayer)
{
	Context.MaxLayer -= SubLayer;

	
	TArray<FVector2D> Points;
	Points.Add(PositionA);
	Points.Add(PositionB);

	FSlateDrawElement::MakeLines(
		Context.OutDrawElements,
		Context.MaxLayer,
		Context.AllottedGeometry.ToPaintGeometry(),
		Points,
		ESlateDrawEffect::None,
		Tint,
		bAntiAlias,
		Thickness);
}
void UChartFunctionLibrary::MathStringSortMap(TMap<FString, FString>& FStringMap, bool InReverseOrder, bool InByValue/*= true*/)
{
	if (InByValue)
	{
		FStringMap.ValueSort([InReverseOrder](const FString& A, const FString& B) {

			return InReverseOrder ? FCString::Atoi(*A) > FCString::Atoi(*B) :FCString::Atoi(*A) < FCString::Atoi(*B);
			});
	}
	else {


		FStringMap.KeySort([InReverseOrder](const FString& A, const FString& B) {

			return InReverseOrder ? FCString::Atoi(*A) > FCString::Atoi(*B) : FCString::Atoi(*A) < FCString::Atoi(*B);
			});
	}


}
bool UChartFunctionLibrary::Regular(const FString CurStr, const FString Re)
{
	FRegexPattern Pattern(Re);
	FRegexMatcher RegMatcher(Pattern, CurStr);
	RegMatcher.SetLimits(0, CurStr.Len());
	return RegMatcher.FindNext();
};

FString UChartFunctionLibrary::StringLimiteLen(FString CurStr, int Len)
{
	FString ReturnStr;
	ReturnStr = (CurStr.Len() > Len) ? (CurStr.Left(Len)+"...") : CurStr;
	return ReturnStr;
}

//AActor UChartFunctionLibrary::FindNearSplineActor(AActor CurSplline)
//{
//	TArray<AActor*> Actors;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
//}

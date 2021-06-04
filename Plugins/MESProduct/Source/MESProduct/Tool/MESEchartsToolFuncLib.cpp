// Fill out your copyright notice in the Description page of Project Settings.


#include "MESEchartsToolFuncLib.h"

TArray<float> UMESEchartsToolFuncLib::LiRegulate(float MinValue, float MaxValue, int32 SpliteNum /*= 5*/)
{
	TArray<float> OutArray;
	double  Min = MinValue;
	double	Max = MaxValue;
	if (Min>Max)
	{
		return OutArray;
	}
	if (Min ==Max)
	{
		for (int32 i=0;i<SpliteNum+1;i++)
		{
			OutArray.Add(i*Min / SpliteNum);
		}
		return OutArray;
	}
	double StepLen = (Max - Min) / SpliteNum;
	double TmpStep;
	int32 StepPower = (int32)FMath::LogX(10, StepLen);
	int32 NextStepPower = StepPower + 1;
	if (FMath::Pow(10,StepPower) == StepLen)
	{
		TmpStep = FMath::Pow(10, StepPower);
	}
	else
	{
		TmpStep = FMath::Pow(10, NextStepPower);
	}
	double NewStepRate = StepLen / TmpStep;
	if (NewStepRate >= 0 && NewStepRate <= 0.05)
	{
		//NewStepRate = 0.05;
	}
	else if (NewStepRate >=0.050001 && NewStepRate <=0.1)
	{
		NewStepRate = 0.1;
	}
	else if (NewStepRate >=0.100001 && NewStepRate <=0.2)
	{
		NewStepRate = 0.2;
	}
	else if (NewStepRate >=0.200001 && NewStepRate <=0.25)
	{
		NewStepRate = 0.25;
	}
	else if (NewStepRate >=0.250001&& NewStepRate <=0.5)
	{
		NewStepRate = 0.5;
	}
	else
	{
		NewStepRate = 1;
	}
	//NewStepRate *= StepPower;
	double NewStep = NewStepRate * TmpStep;
	if ((int32)(Min/ NewStep) != (Min/ NewStep))
	{
		if (Min<0)
		{
			Min = -FMath::CeilToDouble(FMath::Abs(Min / NewStep))*NewStep;
		}
		else
		{
			Min = (int32)(FMath::Abs(Min / NewStep))*NewStep;
		}
	}
	if ((int32)(Max/ NewStep)!= (Max/ NewStep))
	{
		Max = (int32)(FMath::Abs(Max / NewStep + 1))*NewStep;
	}
	int32 TmpNum = (int32)((Max - Min) / NewStep);
	int32 ExtraNum = 0;
	if (TmpNum<SpliteNum)
	{
		ExtraNum = SpliteNum - TmpNum;
		TmpNum = SpliteNum;
		if (ExtraNum%2==0)
		{
			Max += NewStep * (ExtraNum / 2);
		}
		else
		{
			Max += NewStep * (ExtraNum / 2 + 1);
		}
		double TmpMin = Min;
		TmpMin -= NewStep * (ExtraNum / 2);
		if (Min>=0 && TmpMin<0)
		{
			Min = 0;
			Max += NewStep * (ExtraNum / 2);
		}
		else
		{
			Min = TmpMin;
		}
	}
	if (FMath::IsNearlyEqual(Min,(double)MinValue, 0.000001)&& !FMath::IsNearlyEqual(Min,0, 0.000001))
	{
		Min -= NewStep;
		TmpNum++;
		if (Min<0)
		{
			Max += -Min;
			Min = 0;
		}
	}
	if (FMath::IsNearlyEqual(Max,(double)MaxValue, 0.000001))
	{
		Max += NewStep;
		TmpNum++;
	}
	SpliteNum = TmpNum + 1;
	for (int32 i=0;i<SpliteNum;i++)
	{
		OutArray.Add((float)(Min + i * NewStep));
	}
	return OutArray;
}

TArray<float> UMESEchartsToolFuncLib::BLiRegulate(float MinValue, float MaxValue, int32 SpliteNum /*= 5*/)
{
	TArray<float> OutArray;
	double  Min = MinValue;
	double	Max = MaxValue;
	if (Min > Max)
	{
		return OutArray;
	}
	double StepLen = (Max - Min) / SpliteNum;
	double TmpStep;
	int32 StepPower = (int32)FMath::LogX(10, StepLen);
	int32 NextStepPower = StepPower + 1;
	if (FMath::Pow(10, StepPower) == StepLen)
	{
		TmpStep = FMath::Pow(10, StepPower);
	}
	else
	{
		TmpStep = FMath::Pow(10, NextStepPower);
	}
	double NewStepRate = StepLen / TmpStep;
	if (NewStepRate >= 0 && NewStepRate <= 0.1)
	{
		NewStepRate = 0.1;
	}
	else if (NewStepRate >= 0.100001 && NewStepRate <= 0.2)
	{
		NewStepRate = 0.2;
	}
	else if (NewStepRate >= 0.200001 && NewStepRate <= 0.25)
	{
		NewStepRate = 0.25;
	}
	else if (NewStepRate >= 0.250001&& NewStepRate <= 0.5)
	{
		NewStepRate = 0.5;
	}
	else
	{
		NewStepRate = 1;
	}
	//NewStepRate *= StepPower;
	double NewStep = NewStepRate * TmpStep;
	if ((int32)(Min / NewStep) != (Min / NewStep))
	{
		if (Min < 0)
		{
			Min = -FMath::CeilToDouble(FMath::Abs(Min / NewStep))*NewStep;
		}
		else
		{
			Min = (int32)(FMath::Abs(Min / NewStep))*NewStep;
		}
	}
	if ((int32)(Max / NewStep) != (Max / NewStep))
	{
		Max = (int32)(FMath::Abs(Max / NewStep + 1))*NewStep;
	}
	int32 TmpNum = (int32)((Max - Min) / NewStep);
	int32 ExtraNum = 0;
	if (TmpNum < SpliteNum)
	{
		ExtraNum = SpliteNum - TmpNum;
		TmpNum = SpliteNum;
		if (ExtraNum % 2 == 0)
		{
			Max += NewStep * (ExtraNum / 2);
		}
		else
		{
			Max += NewStep * (ExtraNum / 2 + 1);
		}
		double TmpMin = Min;
		TmpMin -= NewStep * (ExtraNum / 2);
		if (Min >= 0 && TmpMin < 0)
		{
			Min = 0;
			Max += NewStep * (ExtraNum / 2);
		}
		else
		{
			Min = TmpMin;
		}
	}
	SpliteNum = TmpNum;
	for (int32 i = 0; i < SpliteNum; i++)
	{
		OutArray.Add((float)(Min + i * NewStep));
	}
	return OutArray;
}

bool UMESEchartsToolFuncLib::IsPointInCircle(FVector2D SrcPoint, FVector2D StartDir, FVector2D EndPointVec, float InnerRad, float OutRad, float Angle, float& OutAngle)
{
	if (FVector2D::Distance(EndPointVec,SrcPoint)<InnerRad || FVector2D::Distance(EndPointVec, SrcPoint) >OutRad)
	{
		OutAngle = -1.f;
		return false;
	}
	FVector2D StartVec = StartDir * OutRad;
	FVector2D EndVec = EndPointVec - SrcPoint;
	EndVec *= FVector2D(1, -1);
	float StartVecDis = FVector2D::Distance(FVector2D(0), StartVec);
	float EndVecDis = FVector2D::Distance(FVector2D(0), EndVec);
	float CrossValue = FVector2D::CrossProduct(StartVec, EndVec);
	float DotValue = FVector2D::DotProduct(StartVec, EndVec);
	OutAngle = (180.f) / PI * FMath::Acos(DotValue/(EndVecDis*StartVecDis));

	if (CrossValue < 0)
	{
		OutAngle = 360 - OutAngle;
	}
	return true;

}

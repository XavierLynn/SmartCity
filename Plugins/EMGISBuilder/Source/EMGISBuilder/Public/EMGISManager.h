// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class EMGISBUILDER_API EMGISManager
{
public:
	EMGISManager();
	~EMGISManager();
	
	FVector2D LoadFromGIS(double Latitude, double Longitude, double RelativeToLatitude, double RelativeToLongitude);
	double translate_lon(double lon, double lat);
	double translate_lat(double lon, double lat);

	FVector2D GCJTransformToWGS(double lon, double lat);
	FVector2D WGSTransformToGCJ(double wgs_lon, double wgs_lat);

};

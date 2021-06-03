// Fill out your copyright notice in the Description page of Project Settings.


#include "EMGISManager.h"



static const double EarthCircumference = 40075036.0;
const double LatitudeLongitudeScale = EarthCircumference / 360.0; // meters per degree

EMGISManager::EMGISManager()
{
}

EMGISManager::~EMGISManager()
{
}

FVector2D EMGISManager::LoadFromGIS(double Latitude, double Longitude, double RelativeToLatitude, double RelativeToLongitude)
{
	//const float OSMToCentimetersScaleFactor = 100.0f;
	const float OSMToCentimetersScaleFactor = 96.0f;

	FVector2D BoundsMin(TNumericLimits<float>::Max(), TNumericLimits<float>::Max());
	FVector2D BoundsMax(TNumericLimits<float>::Lowest(), TNumericLimits<float>::Lowest());

	// Converts latitude to meters
	auto ConvertLatitudeToMeters = [](const double Latitude) -> double
	{
		return -Latitude * LatitudeLongitudeScale;
	};
	// Converts longitude to meters
	auto ConvertLongitudeToMeters = [](const double Longitude, const double Latitude) -> double
	{
		return Longitude * LatitudeLongitudeScale * FMath::Cos(FMath::DegreesToRadians(Latitude));
	};
	auto ConvertLatLongToMetersRelative = [ConvertLatitudeToMeters, ConvertLongitudeToMeters](
		const double Latitude,
		const double Longitude,
		const double RelativeToLatitude,
		const double RelativeToLongitude) -> FVector2D
	{
		// Applies Sanson-Flamsteed (sinusoidal) Projection (see http://www.progonos.com/furuti/MapProj/Normal/CartHow/HowSanson/howSanson.html)
		return FVector2D(
			(float)(ConvertLongitudeToMeters(Longitude, Latitude) - ConvertLongitudeToMeters(RelativeToLongitude, Latitude)),
			(float)(ConvertLatitudeToMeters(Latitude) - ConvertLatitudeToMeters(RelativeToLatitude)));
	};
	const FVector2D NodePos = ConvertLatLongToMetersRelative(
		Latitude,
		Longitude,
		RelativeToLatitude,
		RelativeToLongitude) * OSMToCentimetersScaleFactor;

	// Update bounding box
	{
		if (NodePos.X < BoundsMin.X)
		{
			BoundsMin.X = NodePos.X;
		}
		if (NodePos.Y < BoundsMin.Y)
		{
			BoundsMin.Y = NodePos.Y;
		}
		if (NodePos.X > BoundsMax.X)
		{
			BoundsMax.X = NodePos.X;
		}
		if (NodePos.Y > BoundsMax.Y)
		{
			BoundsMax.Y = NodePos.Y;
		}
	}

	return NodePos;
}
double EMGISManager::translate_lon(double lon, double lat)
{
	double ret = 300.0 + lon +2.0*lat + 0.1*lon*lon +0.1*lon*lat + 0.1*sqrt(abs(lon));
	ret += (20.0 * sin(6.0*lon*PI) + 20.0*sin(2.0*lon*PI)) *2.0 / 3.0;
	ret += (20.0 * sin(lon*PI) + 40.0*sin(lon/3.0 *PI)) *2.0 /3.0;
	ret += (150 * sin(lon/12.0 *PI) + 300.0*sin(lon/30.0 * PI)) *2.0 /3.0;
	return ret;
}
double EMGISManager::translate_lat(double lon, double lat)
{
	double ret = -100 + 2.0*lon + 3.0*lat + 0.2*lat*lat + 0.1*lon*lat + 0.2*sqrt((abs(lon)));
	ret += (20.0 *sin(6.0*lon*PI) + 20*sin(2.0*lon*PI)) *2.0 /3.0;
	ret += (20.0 *sin(lat*PI) + 40.0*sin(lat/3.0*PI)) *2.0 /3.0;
	ret += (160.0*sin(lat/12.0*PI) + 320.0*sin(lat/30.0 *PI)) *2.0 /3.0;
	return ret;
}


FVector2D EMGISManager::GCJTransformToWGS(double lon, double lat)
{
	double ee = 0.00669342162296594323;
	double a = 6378245.0;
	double x_PI = 3.14159265358979323846 * 3000.0 / 180.0;
	FVector2D ret;
	double dlat = translate_lat(lon - 105.0, lat -35.0);
	double dlon = translate_lon(lon - 105.0,lat -35.0);
	double radlat = lat/180.0 *PI;
	double magic = sin(radlat);
	magic = 1 - ee * magic * magic;
	double squrtmagic = sqrt(magic);
	dlon = (dlon *180.0)/(a/squrtmagic*cos(radlat)*PI);
	dlat = (dlat *180.0)/((a*(1-ee))/(magic * squrtmagic)*PI);
	ret.X = lon - dlon;
	ret.Y = lat - dlat;
	return ret;
}

FVector2D EMGISManager::WGSTransformToGCJ(double wgs_lon, double wgs_lat)
{
	double ee = 0.00669342162296594323;
	double a = 6378245.0;
	double x_PI = 3.14159265358979323846 * 3000.0 / 180.0;
	FVector2D ret;
	double dlat = translate_lat(wgs_lon - 105.0,wgs_lat - 35.0);
	double dlon = translate_lon(wgs_lon - 105.0,wgs_lat - 35.0);
	double radlat = wgs_lat/180.0 * PI;
	double magic = sin(radlat);
	magic = 1 - ee*magic*magic;
	double squrtmagic = sqrt(magic);
	dlon = (dlon *180.0)/(a/squrtmagic*cos(radlat)*PI);
	dlat = (dlat *180.0)/((a*(1-ee))/(magic * squrtmagic)*PI);
	ret.X = wgs_lon + dlon;
	ret.Y = wgs_lat +dlat;
	return ret;
	
}


//BEGIN_EXPORT_CLASS(EMGISManager)
//ADD_FUNCTION_EX("LoadFromGIS", FVector2D, LoadFromGIS, double, double, double, double);
//ADD_FUNCTION_EX("GCJTransformToWGS", FVector2D, GCJTransformToWGS, double, double);
//ADD_FUNCTION_EX("WGSTransformToGCJ", FVector2D, WGSTransformToGCJ, double, double);
//END_EXPORT_CLASS()
//IMPLEMENT_EXPORTED_CLASS(EMGISManager)
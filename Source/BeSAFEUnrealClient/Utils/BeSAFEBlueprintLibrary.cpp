// Fill out your copyright notice in the Description page of Project Settings.

#include "BeSAFEBlueprintLibrary.h"

#include "BeSAFEUnrealClient/BeSAFEUnrealClient.h"


FVector UBeSAFEBlueprintLibrary::ToUnrealVector(float X, float Y, float Z)
{
	return FVector(X, Y, -Z) * 100;
	// return FVector(-Z, X, Y) * 100;
}



FQuat UBeSAFEBlueprintLibrary::ToUnrealQuat(float X, float Y, float Z, float W)
{
	// FQuat Quat(X, Y, Z, W);
	// FRotator Rotator = Quat.Rotator();
	// Quat = FQuat(Rotator);
	FQuat Quat(X, -Y, -Z, W);
	// FRotator Rotator = Quat.Rotator();
	// Quat = FQuat(Rotator);
	return Quat;
}

FVector UBeSAFEBlueprintLibrary::HabitatToUnityVector(float X, float Y, float Z)
{
	return FVector(X, Y, -Z) * 100;
}

FQuat UBeSAFEBlueprintLibrary::HabitatToUnityQuat(float X, float Y, float Z, float W)
{
	FQuat Quat(X, -Y, -Z, W);
	return DefaultUnityRotation * Quat;
}

TArray<float> UBeSAFEBlueprintLibrary::UnityToHabitatVector(const FVector &InVec)
{
	FVector3f ConvertedVec = FVector3f(InVec);
	return {ConvertedVec.X, ConvertedVec.Y, -ConvertedVec.Z};
}

TArray<float> UBeSAFEBlueprintLibrary::UnityToHabitatQuat(const FQuat &InQuat)
{
	FQuat4f ConvertedQuat = FQuat4f(InvDefaultUnityRotation * InQuat);
	return {ConvertedQuat.X, ConvertedQuat.Y, ConvertedQuat.Z, ConvertedQuat.W};
}

FVector UBeSAFEBlueprintLibrary::HabitatToUnrealVector(float X, float Y, float Z)
{
	return FVector(-Z, X, Y) * 100;
}

FQuat UBeSAFEBlueprintLibrary::HabitatToUnrealQuat(float X, float Y, float Z, float W)
{
	FQuat HabitatQuat = FQuat(Z, -X, -Y, W);
	return DefaultRotation * HabitatQuat;
}

TArray<float> UBeSAFEBlueprintLibrary::UnrealToHabitatVector(const FVector &InVec)
{
	FVector3f ConvertedVec = FVector3f(InVec.Y, InVec.Z, -InVec.X) / 100;
	return {ConvertedVec.X, ConvertedVec.Y, ConvertedVec.Z};
}

TArray<float> UBeSAFEBlueprintLibrary::UnrealToHabitatQuat(const FQuat &InQuat)
{
	FQuat4f ConvertedQuat = FQuat4f(InvDefaultRotation * InQuat);
	return {ConvertedQuat.W, -ConvertedQuat.Y, -ConvertedQuat.Z, ConvertedQuat.X};
}

void UBeSAFEBlueprintLibrary::GetPoseFromComponent(FBeSAFEPoseData &PoseData, USceneComponent *Component)
{
	if (!Component)
	{
		UE_LOG(LogBeSAFE, Error, TEXT("GetPoseFromComponent: Component is null"));
		return;
	}

	FVector Location = Component->GetComponentLocation();
	FQuat Rotation = Component->GetComponentQuat();

	TArray<float> PositionArr, RotationArr;
	PoseData.Position = UnrealToHabitatVector(Location);
	PoseData.Rotation = UnrealToHabitatQuat(Rotation);
}

void UBeSAFEBlueprintLibrary::CoordinateTest()
{
	UE_LOG(LogBeSAFE, Log, TEXT("=============Coordinate Test Start==================="));
	TArray<FVector> TestPoint = {FVector(1, 1, 0), FVector(1, 2, 3), FVector(4, 5, 6)};
	TArray<FVector> TestAxis = {FVector(0, 1, 0), FVector(0, 1, 0), FVector(1, 2, 3)};
	TArray<float> TestDegree = {90, 180, 66};
	for (int i = 0; i < TestPoint.Num(); i++)
	{
		UE_LOG(LogBeSAFE, Log, TEXT("Test Case %d"), i);

		TestAxis[i].Normalize();
		FQuat HabitatRotation = FQuat(TestAxis[i], FMath::DegreesToRadians(TestDegree[i]));
		FVector HabitatRotationResult = HabitatRotation.RotateVector(TestPoint[i]);

		UE_LOG(LogBeSAFE, Log,
		       TEXT("Habitat Rotation: Rotate point (%.3f, %.3f, %.3f) around axis (%.3f, %.3f, %.3f) %.3f degrees, Quat: (%.3f, %.3f, %.3f, %.3f)"),
		       TestPoint[i].X, TestPoint[i].Y, TestPoint[i].Z, TestAxis[i].X, TestAxis[i].Y, TestAxis[i].Z, TestDegree[i], HabitatRotation.X,
		       HabitatRotation.Y, HabitatRotation.Z, HabitatRotation.W);

		FVector UnrealPoint = HabitatToUnrealVector(TestPoint[i].X, TestPoint[i].Y, TestPoint[i].Z);
		FQuat UnrealRotation = HabitatToUnrealQuat(HabitatRotation.X, HabitatRotation.Y, HabitatRotation.Z, HabitatRotation.W);
		FVector UnrealRotationAxis = HabitatToUnrealVector(TestAxis[i].X, TestAxis[i].Y, TestAxis[i].Z);
		FQuat UnrealRotationCalc = FQuat(UnrealRotationAxis, FMath::DegreesToRadians(TestDegree[i]));
		FVector UnrealRotationResult = UnrealRotation.RotateVector(UnrealPoint);
		UE_LOG(LogBeSAFE, Log, TEXT("Unreal Converted Rotation: (%.3f, %.3f, %.3f, %.3f), Unreal Calculated Rotation: (%.3f, %.3f, %.3f, %.3f)"),
		       UnrealRotation.X,
		       UnrealRotation.Y, UnrealRotation.Z, UnrealRotation.W, UnrealRotationCalc.X, UnrealRotationCalc.Y, UnrealRotationCalc.Z,
		       UnrealRotationCalc.W);

		UE_LOG(LogBeSAFE, Log, TEXT("Habitat Result: (%.3f, %.3f, %.3f), Unreal Result: (%.3f, %.3f, %.3f)"), HabitatRotationResult.X,
		       HabitatRotationResult.Y,
		       HabitatRotationResult.Z, UnrealRotationResult.X, UnrealRotationResult.Y, UnrealRotationResult.Z);
	}

	UE_LOG(LogBeSAFE, Log, TEXT("=============Coordinate Test End====================="));
}

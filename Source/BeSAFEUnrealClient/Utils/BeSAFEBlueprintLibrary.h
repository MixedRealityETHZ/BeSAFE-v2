// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeSAFEUnrealClient/Network/ClientState.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BeSAFEBlueprintLibrary.generated.h"

/**
 * Utility functions for converting between Unreal and Habitat types.
 * Habitat Coordinate System: X left, Y up, Z back
 * Unity Coordinate System: X left, Y up, Z forward
 * Unreal Coordinate System: X forward, Y left, Z up
 */
UCLASS()
class BESAFEUNREALCLIENT_API UBeSAFEBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/// By default, Habitat will try to rotate the object 180 degrees around the Y axis
	/// to make the object face front
	inline static FQuat DefaultRotation = FQuat(FRotator(0, 180, 0));

	inline static FQuat DefaultUnityRotation = FQuat(FRotator(180, 0, 0));

	inline static FQuat InvDefaultRotation = DefaultRotation.Inverse();

	inline static FQuat InvDefaultUnityRotation = DefaultUnityRotation.Inverse();
	
	inline static FMatrix HabitatToUnreal = FMatrix(
		FPlane(0, 0, -1, 0),
		FPlane(1, 0,  0, 0),
		FPlane(0, 1,  0, 0),
		FPlane(0, 0,  0, 1)
	);

	inline static FMatrix UnrealToHabitat = HabitatToUnreal.Inverse();

	inline static FMatrix UnityToUnreal = FMatrix(
		FPlane(0, 0, 1, 0),
		FPlane(1, 0, 0, 0),
		FPlane(0, 1, 0, 0),
		FPlane(0, 0, 0, 1)
	);

	inline static FMatrix UnrealToUnity = UnityToUnreal.Inverse();

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static FVector ToUnrealVector(float X, float Y, float Z);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static FQuat ToUnrealQuat(float X, float Y, float Z, float W);


	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static FVector HabitatToUnityVector(float X, float Y, float Z);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static FQuat HabitatToUnityQuat(float X, float Y, float Z, float W);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static TArray<float> UnityToHabitatVector(const FVector &InVec);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static TArray<float> UnityToHabitatQuat(const FQuat &InQuat);
	
	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static FVector HabitatToUnrealVector(float X, float Y, float Z);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static FQuat HabitatToUnrealQuat(float X, float Y, float Z, float W);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static TArray<float> UnrealToHabitatVector(const FVector &InVec);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static TArray<float> UnrealToHabitatQuat(const FQuat &InQuat);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Library")
	static void GetPoseFromComponent(FBeSAFEPoseData &PoseData, USceneComponent *Component);

	UFUNCTION()
	static void CoordinateTest();
};

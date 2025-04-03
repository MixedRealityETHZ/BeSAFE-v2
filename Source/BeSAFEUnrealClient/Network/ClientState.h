#pragma once

#include "CoreMinimal.h"
#include "ClientState.generated.h"

USTRUCT(BlueprintType)
struct FHabitatVector
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Habitat")
	float X = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Habitat")
	float Y = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Habitat")
	float Z = 0;
};

USTRUCT(BlueprintType)
struct FHabitatQuat
{
	GENERATED_BODY()
	UPROPERTY(BlueprintReadWrite, Category = "Habitat")
	float X = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Habitat")
	float Y = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Habitat")
	float Z = 0;
	UPROPERTY(BlueprintReadWrite, Category = "Habitat")
	float W = 0;
};

USTRUCT(BlueprintType)
struct FBeSAFEPoseData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "BeSAFE")
	TArray<float> Position;

	UPROPERTY(BlueprintReadWrite, Category = "BeSAFE")
	TArray<float> Rotation;
};

USTRUCT(BlueprintType)
struct FBeSAFEAvatarData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "BeSAFE")
	FBeSAFEPoseData Root;
	UPROPERTY(BlueprintReadWrite, Category = "BeSAFE")
	TArray<FBeSAFEPoseData> Hands;
};

USTRUCT(BlueprintType)
struct FButtonInputData
{
	GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "BeSAFE")
    TArray<int32> ButtonHeld;

    UPROPERTY(BlueprintReadWrite, Category = "BeSAFE")
    TArray<int32> ButtonDown;

    UPROPERTY(BlueprintReadWrite, Category = "BeSAFE")
    TArray<int32> ButtonUp;
};

USTRUCT(BlueprintType)
struct FClientState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "BeSAFE", meta = (DisplayName = "avatar"))
	FBeSAFEAvatarData Avatar;
	UPROPERTY(BlueprintReadWrite, Category = "BeSAFE", meta = (DisplayName = "input"))
	FButtonInputData Input;
	UPROPERTY(BlueprintReadWrite, Category = "BeSAFE", meta = (DisplayName = "connection_params"))
	TMap<FString, FString> connection_params;
};

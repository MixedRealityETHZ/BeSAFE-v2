// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeSAFEUnrealClient/Network/ClientState.h"
#include "GameFramework/Character.h"

#include "BeSAFECharacter.generated.h"

class UMotionControllerComponent;
class UCameraComponent;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSceneLoaded);

UCLASS()
class BESAFEUNREALCLIENT_API ABeSAFECharacter : public ACharacter
{
	GENERATED_BODY()

	friend class UNetworkClientComponent;

public:
	// Sets default values for this character's properties
	ABeSAFECharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Client")
	void UpdateClientState(FClientState &ClientState);

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Client")
	void OnEndFrame();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Move(const FInputActionValue &Value);

	virtual void Look(const FInputActionValue &Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	USceneComponent* VROrigin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent *MotionControllerLeftGrip;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent *MotionControllerRightGrip;

	/** Input Settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext *MovementMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext *DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *LookAction;

	UPROPERTY(BlueprintAssignable, Category = "BeSAFE Client", meta = (AllowPrivateAccess = "true"))
	FOnSceneLoaded OnSceneLoaded;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BeSAFE Client", meta = (AllowPrivateAccess = "true"))
	class UNetworkClientComponent *NetworkClient;

	UFUNCTION(BlueprintCallable, Category = "BeSAFE Client")
	void KeyPressed(int32 Key, bool bKeyDown);

	bool bIsKeyHeld[4];

	FButtonInputData ButtonInputBuffer;

	const static int NumKeys = 4;
};

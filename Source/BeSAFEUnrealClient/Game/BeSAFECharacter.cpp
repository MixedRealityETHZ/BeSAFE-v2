// Fill out your copyright notice in the Description page of Project Settings.

#include "BeSAFECharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MotionControllerComponent.h"
#include "BeSAFEUnrealClient/BeSAFEUnrealClient.h"
#include "BeSAFEUnrealClient/Network/NetworkClient.h"
#include "BeSAFEUnrealClient/Utils/BeSAFEBlueprintLibrary.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABeSAFECharacter::ABeSAFECharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to
	// improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	NetworkClient = CreateDefaultSubobject<UNetworkClientComponent>(TEXT("NetworkClient"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	MotionControllerLeftGrip = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerLeftGrip"));
	MotionControllerRightGrip = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionControllerRightGrip"));

	if (RootComponent)
	{
		VROrigin->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		MotionControllerLeftGrip->AttachToComponent(VROrigin, FAttachmentTransformRules::KeepRelativeTransform);
		MotionControllerRightGrip->AttachToComponent(VROrigin, FAttachmentTransformRules::KeepRelativeTransform);
		Camera->AttachToComponent(VROrigin, FAttachmentTransformRules::KeepRelativeTransform);
	}

	for (int32 i = 0; i < 4; i++)
	{
		bIsKeyHeld[i] = false;
	}
}

// Called when the game starts or when spawned
void ABeSAFECharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABeSAFECharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABeSAFECharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MovementMappingContext, 0);
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABeSAFECharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABeSAFECharacter::Look);
	}
	else
	{
		UE_LOG(LogBeSAFE, Error,
		       TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input "
			       "system."
			       "If you intend to use the legacy system, then you will need to update this C++ file."),
		       *GetNameSafe(this));
	}
}

void ABeSAFECharacter::UpdateClientState(FClientState &ClientState)
{
	// Update Avatar Pose
	FBeSAFEAvatarData &AvatarData = ClientState.Avatar;
	UBeSAFEBlueprintLibrary::GetPoseFromComponent(AvatarData.Root, Camera);
	AvatarData.Hands.SetNum(2);
	UBeSAFEBlueprintLibrary::GetPoseFromComponent(AvatarData.Hands[0], MotionControllerLeftGrip);
	UBeSAFEBlueprintLibrary::GetPoseFromComponent(AvatarData.Hands[1], MotionControllerRightGrip);

	// Update the button input key
	TArray<int32> KeyHeld;
	for (int32 i = 0; i < NumKeys; i++)
	{
		if (bIsKeyHeld[i])
		{
			KeyHeld.Add(i);
		}
	}
	ButtonInputBuffer.ButtonHeld = KeyHeld;
	ClientState.Input = ButtonInputBuffer;
}

void ABeSAFECharacter::OnEndFrame()
{
	ButtonInputBuffer.ButtonHeld.Empty();
	ButtonInputBuffer.ButtonDown.Empty();
	ButtonInputBuffer.ButtonUp.Empty();
}


void ABeSAFECharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (MovementVector.SquaredLength() > 0.1f && Camera != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Yellow,
		                                 FString::Printf(TEXT("Movement Vector (%f, %f)"), MovementVector.X, MovementVector.Y));
		auto CameraForwardVector = Camera->GetForwardVector();
		CameraForwardVector.Z = 0;
		AddMovementInput(CameraForwardVector, MovementVector.Y);
		auto CameraRightVector = Camera->GetRightVector();
		CameraRightVector.Z = 0;
		AddMovementInput(CameraRightVector, MovementVector.X);
	}
}

void ABeSAFECharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Red, FString::Printf(TEXT("Look (%f, %f)"), LookAxisVector.X, LookAxisVector.Y));
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ABeSAFECharacter::KeyPressed(int32 Key, bool bKeyDown)
{
	if (Key < 0 || Key >= NumKeys)
	{
		UE_LOG(LogBeSAFE, Warning, TEXT("Invalid Key Pressed!"));
		return;
	}

	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Blue, FString::Printf(TEXT("Key Event: %d"), Key));

	if (bKeyDown)
	{
		bIsKeyHeld[Key] = true;
		ButtonInputBuffer.ButtonDown.Add(Key);
	}
	else
	{
		bIsKeyHeld[Key] = false;
		ButtonInputBuffer.ButtonUp.Add(Key);
	}
}

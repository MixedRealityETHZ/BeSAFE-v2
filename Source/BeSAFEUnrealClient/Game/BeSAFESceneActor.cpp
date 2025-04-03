// Fill out your copyright notice in the Description page of Project Settings.

#include "BeSAFESceneActor.h"
#include "Components/TextRenderComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"


// Sets default values
ABeSAFESceneActor::ABeSAFESceneActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	if (SceneRootComponent)
	{
		RootComponent = SceneRootComponent;
		SceneRootComponent->SetMobility(EComponentMobility::Movable);
	}
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	if (MeshComponent)
	{
		MeshComponent->SetupAttachment(RootComponent);
	}

	TextRenderComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TextComponent"));
	if (TextRenderComponent)
	{
		TextRenderComponent->SetupAttachment(RootComponent);
		//TextRenderComponent->AttachTo(this);
		TextRenderComponent->SetXScale(2.0);
		TextRenderComponent->SetXScale(2.0);
		TextRenderComponent->SetTextRenderColor(FColor::Black);
		TextRenderComponent->SetText(FText());
	}

}

void ABeSAFESceneActor::CreateHighlight(UNiagaraSystem* MyCoolFX) {


	// Spawn the Niagara component and attach it
	Highlight = UNiagaraFunctionLibrary::SpawnSystemAttached(
		MyCoolFX,                // The Niagara System to spawn
		RootComponent,                // Attach to the root component
		NAME_None,                    // No specific attachment point
		FVector::ZeroVector,          // Position offset
		FRotator::ZeroRotator,        // Rotation offset
		EAttachLocation::KeepRelativeOffset,
		true                          // Auto destroy after the effect
	);


}

void ABeSAFESceneActor::UpdateText(double UserYaw) {
	if (Highlight) {
		// The first time / initialization	
		if (PrePos == FVector(0)) {
			PrePos = GetActorLocation();
		}
		// update the text
		else {
			int32 dist = (GetActorLocation() - PrePos).Length();
			TextRenderComponent->SetText(FText::AsNumber(dist));
			TextRenderComponent->SetWorldRotation(FRotator(0, UserYaw + 180, 0));
		}
	}
}

// Called when the game starts or when spawned
void ABeSAFESceneActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABeSAFESceneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABeSAFESceneActor::SetMesh(UStaticMesh *Mesh)
{
	if (MeshComponent)
	{
		MeshComponent->SetStaticMesh(Mesh);
	}
}

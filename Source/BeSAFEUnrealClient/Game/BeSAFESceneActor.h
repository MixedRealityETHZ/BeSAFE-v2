// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BeSAFESceneActor.generated.h"

UCLASS()
class BESAFEUNREALCLIENT_API ABeSAFESceneActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABeSAFESceneActor();

	UFUNCTION()
	void SetMesh(UStaticMesh* Mesh);

	UFUNCTION()
	void SetMeshLocalTransform(FTransform Transform) { MeshComponent->SetRelativeTransform(Transform); }

	UFUNCTION()
	UStaticMeshComponent* GetMeshComponent() { return MeshComponent; }

	UFUNCTION()
	void CreateHighlight(UNiagaraSystem* MyCoolFX);

	UFUNCTION()
	void UpdateText(double UserYaw);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "BeSAFE Scene")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "BeSAFE Scene")
	USceneComponent* SceneRootComponent;

	UPROPERTY(VisibleAnywhere, Category = "BeSAFE Scene")
	UNiagaraComponent* Highlight;

	UPROPERTY(VisibleAnywhere, Category = "BeSAFE Scene")
	UTextRenderComponent* TextRenderComponent;

	FVector PrePos = FVector(0);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClientState.h"
#include "IWebSocket.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NetworkClient.generated.h"

class ABeSAFECharacter;

UCLASS()
class BESAFEUNREALCLIENT_API UNetworkClientComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UNetworkClientComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the actor is being removed from a level
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "BeSAFE Client", meta = (AllowPrivateAccess = "true"))
	FString ServerURL;
	UPROPERTY(EditDefaultsOnly, Category = "BeSAFE Client", meta = (AllowPrivateAccess = "true"))
	FString ServerProtocol;

	UPROPERTY(EditDefaultsOnly, Category = "BeSAFE Client", meta = (AllowPrivateAccess = "true"))
	FRotator HabitatRotator;

	UPROPERTY(EditDefaultsOnly, Category = "BeSAFE Client", meta = (AllowPrivateAccess = "true"))
	FRotator UnrealRotator;

	UPROPERTY(EditDefaultsOnly, Category = "BeSAFE Client", meta = (AllowPrivateAccess = "true"))
	FRotator WholeRotator;

	TSharedPtr<IWebSocket> WebSocket;

	void ConnectToServer();
	void OnConnected();
	void OnConnectionError(const FString &Error);
	void OnClosed(int32 StatusCode, const FString &Reason, bool bWasClean);
	void OnMessage(const FString &Message);
	void SendMessage(const FString &Message);
	bool IsConnected() const;

	void UpdateObject(int instanceKey, TSharedPtr<FJsonObject> absTramsform);
	void UpdateObjectUnity(int instanceKey, TSharedPtr<FJsonObject> absTramsform);
	void ProcessKeyframe(TSharedPtr<FJsonObject> keyframe);
	void ProcessStateUpdate(TSharedPtr<FJsonObject> keyframe);
	void ProcessStateUpdatesImmediate(TSharedPtr<FJsonObject> keyframe);

	void SendClientState();
	void CleanupActorDictionary();

	bool bFirstTransmission;
	FTimerHandle TimerHandle;
	FClientState ClientState;
	TMap<FString, TSharedPtr<FJsonValue>> LoadDictionary;
	TMap<int, FTransform> TransformDictionary;
	TMap<int, class ABeSAFESceneActor* > InstanceDictionary;
	FMatrix CoordUnityToUnreal;
	FMatrix CoordUnrealToUnity;
	long update_count = 0;

	//int idx = 0;
	//FTimerHandle MyTimerHandle;
	//TArray<TSharedPtr<FJsonValue>> creations;

	UPROPERTY(EditAnywhere, Category = "BeSAFE Scene")
	class UNiagaraSystem* MyCoolFX = LoadObject<UNiagaraSystem>(nullptr, TEXT("/Script/Niagara.NiagaraSystem'/Game/VRTemplate/VFX/NS_TeleportRing.NS_TeleportRing'"), nullptr, LOAD_None, nullptr);


	UPROPERTY(Transient)
	ABeSAFECharacter *Player;
};

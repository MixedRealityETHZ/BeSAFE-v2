// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkClient.h"

#include "JsonObjectConverter.h"
#include "WebSocketsModule.h"
#include "TimerManager.h"
#include "BeSAFEUnrealClient/BeSAFEUnrealClient.h"
#include "BeSAFEUnrealClient/Game/BeSAFECharacter.h"

#include "GameFramework/Actor.h"
#include "BeSAFEUnrealClient/Game/BeSAFESceneActor.h"
#include "BeSAFEUnrealClient/Utils/BeSAFEBlueprintLibrary.h"

// Sets default values
UNetworkClientComponent::UNetworkClientComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bFirstTransmission = true;
	Player = nullptr;

	ServerURL = TEXT("ws://127.0.0.1:8888");
	ServerProtocol = TEXT("ws");

	CoordUnityToUnreal.SetIdentity();
	CoordUnityToUnreal.M[0][0] = 0;
	CoordUnityToUnreal.M[0][2] = 1;
	CoordUnityToUnreal.M[1][1] = 0;
	CoordUnityToUnreal.M[1][0] = 1; // x
	CoordUnityToUnreal.M[2][1] = 1; // y
	CoordUnityToUnreal.M[2][2] = 0;
	CoordUnrealToUnity = Inverse(CoordUnityToUnreal);

}

void UNetworkClientComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Called when the game starts or when spawned
void UNetworkClientComponent::BeginPlay()
{
	Super::BeginPlay();

	bFirstTransmission = true;
	Player = Cast<ABeSAFECharacter>(GetOwner());
	if (!Player)
	{
		UE_LOG(LogBeSAFE, Error, TEXT("Owner is not a BeSAFECharacter!"));
	}

	UE_LOG(LogBeSAFE, Log, TEXT("Network Begin play"));
	ConnectToServer();

	// Set a repeating timer to send client state every 0.1s (similar to Unity's InvokeRepeating)
	if (UWorld *World = GetWorld())
	{
		World->GetTimerManager().SetTimer(TimerHandle, this, &UNetworkClientComponent::SendClientState, 0.1f, true);
	}
}

// Connect to WebSocket server
void UNetworkClientComponent::ConnectToServer()
{
	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	WebSocket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);

	if (WebSocket)
	{
		WebSocket->OnConnected().AddUObject(this, &UNetworkClientComponent::OnConnected);
		WebSocket->OnConnectionError().AddUObject(this, &UNetworkClientComponent::OnConnectionError);
		WebSocket->OnClosed().AddUObject(this, &UNetworkClientComponent::OnClosed);
		WebSocket->OnMessage().AddUObject(this, &UNetworkClientComponent::OnMessage);

		WebSocket->Connect();
	}
	else
	{
		UE_LOG(LogBeSAFE, Warning, TEXT("Failed to create WebSocket!"));
	}
}

// Called when connected to the server
void UNetworkClientComponent::OnConnected()
{
	SendMessage(TEXT("client ready!"));
	CleanupActorDictionary();
	UE_LOG(LogBeSAFE, Log, TEXT("Connected to the server! Sent message: client ready!"));
}

// Called when there's a connection error
void UNetworkClientComponent::OnConnectionError(const FString &Error)
{
	UE_LOG(LogBeSAFE, Warning, TEXT("Connection error: %s"), *Error);
}

// Called when the connection is closed
void UNetworkClientComponent::OnClosed(int32 StatusCode, const FString &Reason, bool bWasClean)
{
	UE_LOG(LogBeSAFE, Log, TEXT("Connection closed. Reason: %s"), *Reason);
}


void UNetworkClientComponent::UpdateObjectUnity(int instanceKey, TSharedPtr<FJsonObject> absTramsform)
{
	// Now you can work with the actor
	if (InstanceDictionary.Contains(instanceKey))
	{
		ABeSAFESceneActor* Actor = *InstanceDictionary.Find(instanceKey);
		//UE_LOG(LogTemp, Warning, TEXT("Update Actor: %s"), *Actor->GetName());

		// get the transform
		auto translation = absTramsform->GetArrayField(TEXT("translation"));
		check(translation.Num() == 3);
		TArray TranslationArr = { translation[0].Get()->AsNumber(), translation[1].Get()->AsNumber(), translation[2].Get()->AsNumber() };
		FVector TranslationVector = UBeSAFEBlueprintLibrary::ToUnrealVector(translation[0].Get()->AsNumber(), translation[1].Get()->AsNumber(),
			translation[2].Get()->AsNumber());

		auto rotation = absTramsform->GetArrayField(TEXT("rotation"));
		check(rotation.Num() == 4);

		FQuat RotationVector = UBeSAFEBlueprintLibrary::ToUnrealQuat(rotation[1].Get()->AsNumber(), rotation[2].Get()->AsNumber(),
			rotation[3].Get()->AsNumber(), rotation[0].Get()->AsNumber());
		FRotator Rotator = RotationVector.Rotator();

		UE_LOG(LogBeSAFE, Log, TEXT("Transformed Rotation: (Roll: %f, Pitch: %f, Yaw: %f), Translation: (%f, %f, %f)"), Rotator.Roll, Rotator.Pitch,
			Rotator.Yaw, TranslationVector.X, TranslationVector.Y, TranslationVector.Z);


		FTransform TranslationTransform(RotationVector, TranslationVector, FVector::OneVector);
		Actor->SetActorTransform(TranslationTransform);
		
	}
}

// why is there another identical function?
void UNetworkClientComponent::UpdateObject(int instanceKey, TSharedPtr<FJsonObject> absTramsform)
{
	// Now you can work with the actor
	if (InstanceDictionary.Contains(instanceKey))
	{
		ABeSAFESceneActor* Actor = *InstanceDictionary.Find(instanceKey);

		// get the transform
		auto translation = absTramsform->GetArrayField(TEXT("translation"));
		check(translation.Num() == 3);
		TArray TranslationArr = {translation[0].Get()->AsNumber(), translation[1].Get()->AsNumber(), translation[2].Get()->AsNumber()};
		FVector TranslationVector = UBeSAFEBlueprintLibrary::HabitatToUnityVector(translation[0].Get()->AsNumber(), translation[1].Get()->AsNumber(),
		                                                                          translation[2].Get()->AsNumber());

		auto rotation = absTramsform->GetArrayField(TEXT("rotation"));
		check(rotation.Num() == 4);
		FQuat RotationVector = UBeSAFEBlueprintLibrary::HabitatToUnityQuat(rotation[1].Get()->AsNumber(), rotation[2].Get()->AsNumber(),
		                                                                   rotation[3].Get()->AsNumber(), rotation[0].Get()->AsNumber());

		FTransform TranslationTransform(RotationVector, TranslationVector, FVector::OneVector);

		FMatrix TransformMatrix = TranslationTransform.ToMatrixWithScale();
		TransformMatrix = UBeSAFEBlueprintLibrary::UnityToUnreal * TransformMatrix * UBeSAFEBlueprintLibrary::UnrealToUnity;
		TranslationTransform.SetFromMatrix(TransformMatrix);
		Actor->SetActorTransform(TranslationTransform);

		Actor->UpdateText(Player->GetActorRotation().Yaw);
	}
}



void UNetworkClientComponent::ProcessStateUpdatesImmediate(TSharedPtr<FJsonObject> keyframe)
{
	// Handle State Updates
	if (keyframe->TryGetField("stateUpdates"))
	{
		for (auto &update : keyframe->GetArrayField("stateUpdates"))
		{
			int instanceKey = update->AsObject()->GetIntegerField("instanceKey");
			TSharedPtr<FJsonObject> absTramsform = update->AsObject()->GetObjectField("state")->GetObjectField("absTransform");
			int semanticId = update->AsObject()->GetObjectField("state")->GetIntegerField("semanticId");

			// search the object for instanceKey
			// very naive key
			UpdateObject(instanceKey, absTramsform);
		}
	}
}


void UNetworkClientComponent::ProcessStateUpdate(TSharedPtr<FJsonObject> keyframe)
{
	// TODO: add interpolation version
	ProcessStateUpdatesImmediate(keyframe);
}

void UNetworkClientComponent::ProcessKeyframe(TSharedPtr<FJsonObject> keyframe)
{
	// Handle message
	if (keyframe->TryGetField(TEXT("message"))) {}

	// Handle loads
	if (keyframe->TryGetField(TEXT("loads")))
	{
		for (TSharedPtr<FJsonValue> Load : keyframe->GetArrayField(TEXT("loads")))
		{
			FString filepath = Load->AsObject()->GetStringField(TEXT("filepath"));
			LoadDictionary.Emplace(filepath, Load);
		}
	}

	// Handle creation, there is instance key inside creation
	if (keyframe->TryGetField("creations"))
	{
		int count = 0;

		for (auto &creation : keyframe->GetArrayField("creations"))
		{

			count++;
			//if (count > 100 && count < 250) continue;
			
			// Get instance key
			int InstanceKey = creation->AsObject()->GetIntegerField("instanceKey");

			// Object already created
			if (InstanceDictionary.Contains(InstanceKey)) continue;

			// Get creation information
			TSharedPtr<FJsonObject> CreationDetail = creation->AsObject()->GetObjectField("creation");
			FString HabitatFilePath = CreationDetail->GetStringField("filepath");
			FString filepath = CreationDetail->GetStringField("filepath");
			UE_LOG(LogBeSAFE, Log, TEXT("Creation filepath: %s"), *filepath);

			filepath.RemoveFromEnd(".glb");
			// For robot strange file name
			filepath.ReplaceInline(TEXT("/urdf/.."), TEXT(""));
			filepath.ReplaceInline(TEXT("/configs/.."), TEXT(""));
			int slash_index = -1;
			filepath.FindLastChar('/', slash_index);
			FString filename = filepath.Mid(slash_index + 1);

			// For robot strange file name
			filename.ReplaceInline(TEXT("."), TEXT("_"), ESearchCase::IgnoreCase);
			filepath.ReplaceInline(TEXT("."), TEXT("_"), ESearchCase::IgnoreCase);

			// filepath += TEXT(".glb");

			FString UnrealFilePath = FString::Format(TEXT("/Script/Engine.StaticMesh'/Game/Resources/{0}.{1}'"), {filepath, filename});

			// Scale
			double Scale0 = 1, Scale1 = 1, Scale2 = 1;
			if (CreationDetail->TryGetField(TEXT("scale")))
			{
				auto Scale = CreationDetail->GetArrayField(TEXT("scale"));
				Scale0 = Scale[0]->AsNumber();
				Scale1 = Scale[1]->AsNumber();
				Scale2 = Scale[2]->AsNumber();
			}

			bool bIsStatic = false;
			if (CreationDetail->TryGetField(TEXT("isStatic")))
			{
				bIsStatic = CreationDetail->GetBoolField(TEXT("isStatic"));
			}

			// Path to the imported static mesh asset
			UStaticMesh *LoadedAsset = static_cast<UStaticMesh*>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *UnrealFilePath));
			if (LoadedAsset)
			{
				// create actor and store actor reference in InstanceDictionary

				TSharedPtr<FJsonValue> Load = nullptr;
				if (LoadDictionary.Contains(HabitatFilePath))
				{
					Load = *LoadDictionary.Find(HabitatFilePath);
				}

				UWorld *World = GetWorld();
				if (World)
				{
					FVector Location = FVector(0.0f, 0.0f, 0.0f);
					FRotator Rotation = FRotator::ZeroRotator;

					// Spawn the actor
					ABeSAFESceneActor *SceneActor = World->SpawnActor<ABeSAFESceneActor>(ABeSAFESceneActor::StaticClass());
					if (SceneActor)
					{
						SceneActor->SetMesh(LoadedAsset);
						UE_LOG(LogTemp, Warning, TEXT("GLB Actor spawned successfully!"));
						FTransform TranslationTransform(Rotation, Location, FVector(Scale0, Scale1, Scale2));
						SceneActor->AddActorLocalTransform(TranslationTransform);

						if (bIsStatic)
						{
							// Static object is the room, currently set no collision to make player walk in the room
							SceneActor->GetMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						}

						if (Load != nullptr)
						{
							TSharedPtr<FJsonObject> frame = Load->AsObject()->GetObjectField(TEXT("frame"));
							FString OutputString;
							TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
							FJsonSerializer::Serialize(frame.ToSharedRef(), Writer);
							UE_LOG(LogBeSAFE, Log, TEXT("Load Frame: %s"), *OutputString);

							TArray<TSharedPtr<FJsonValue>> up = frame->GetArrayField(TEXT("up"));
							FVector upVector(up[0]->AsNumber(), up[1]->AsNumber(), up[2]->AsNumber());

							if (upVector == FVector(0, 1, 0))
							{
								SceneActor->SetMeshLocalTransform(FTransform(HabitatRotator));
							}
							else if (upVector == FVector(0, 0, 1))
							{
								SceneActor->SetMeshLocalTransform(FTransform(UnrealRotator));
							}
							else
							{
								UE_LOG(LogBeSAFE, Error, TEXT("Unknown up vector"));
							}
						}
						else
						{
							UE_LOG(LogBeSAFE, Error, TEXT("Load is null"));
						}

						// add the highlight here
						if (HabitatFilePath.Contains("google_16k")) {
							SceneActor->CreateHighlight(MyCoolFX);
						}
						InstanceDictionary.Emplace(InstanceKey, SceneActor);
					}
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, TEXT("Failed to load GLB asset !"));
			}

		}

		if (Player)
		{
			Player->OnSceneLoaded.Broadcast();
		}
		FString KeyFrameInfoString = FString::Printf(TEXT("Keyframe processed! Create %d instances, Total size %llu bytes"),
		                                             InstanceDictionary.Num(), InstanceDictionary.GetAllocatedSize());
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Blue, KeyFrameInfoString);
		UE_LOG(LogBeSAFE, Log, TEXT("%s"), *KeyFrameInfoString);
	}

	// Handle state update
	ProcessStateUpdate(keyframe);

	// TODO: There are also deletion in the original version, but we abandon it for now
}


// Called when a message is received from the server
void UNetworkClientComponent::OnMessage(const FString &Message)
{
	// Here, you can process the message and update your state or game logic.
	// parse the string to json
	TSharedPtr<FJsonObject> keyframes;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Message);

	if (FJsonSerializer::Deserialize(Reader, keyframes) && keyframes.IsValid())
	{
		for (auto &keyframe : keyframes->GetArrayField("keyframes"))
		{
			ProcessKeyframe(keyframe->AsObject());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON string"));
	}
}

// Send a message to the server
void UNetworkClientComponent::SendMessage(const FString &Message)
{
	if (!IsConnected())
	{
		UE_LOG(LogBeSAFE, Warning, TEXT("Not connected to server!"));
		return;
	}
	WebSocket->Send(Message);
}

bool UNetworkClientComponent::IsConnected() const
{
	return WebSocket.IsValid() && WebSocket->IsConnected();
}

// This function will be called every 0.1 seconds to send the client state
void UNetworkClientComponent::SendClientState()
{
	if (Player == nullptr)
	{
		return;
	}

	// Update Client State
	Player->UpdateClientState(ClientState);

	FString ClientStateString;
	FJsonObjectConverter::UStructToJsonObjectString(FClientState::StaticStruct(), &ClientState, ClientStateString);
	// UE_LOG(LogBeSAFE, Log, TEXT("Sending client state: %s"), *ClientStateString);

	Player->OnEndFrame();

	SendMessage(ClientStateString);
}

void UNetworkClientComponent::CleanupActorDictionary()
{
	// Destroy all spawned actors
	for (auto &Pair : InstanceDictionary)
	{
		Pair.Value->Destroy();
	}
	InstanceDictionary.Empty();
}

// Called when the game ends or the actor component is destroyed
void UNetworkClientComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsConnected())
	{
		WebSocket->Close();
	}

	if (UWorld *World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimerHandle);
	}

	CleanupActorDictionary();

	Super::EndPlay(EndPlayReason);
}

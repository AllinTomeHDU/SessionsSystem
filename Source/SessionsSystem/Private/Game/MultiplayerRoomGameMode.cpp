// Copyright: Jichao Luo


#include "Game/MultiplayerRoomGameMode.h"
#include "GameFramework/GameStateBase.h"


void AMultiplayerRoomGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState->PlayerArray.Num();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, 
		FString::Printf(TEXT("PostLogin:%d"), NumberOfPlayers));
}

void AMultiplayerRoomGameMode::PostSeamlessTravel()
{
	Super::PostSeamlessTravel();

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString(TEXT("PostSeamlessTravel")));

	FString CurrentMap = GetWorld()->GetMapName();
	if (CurrentMap.Contains("ThirdPersonExampleMap"))
	{
		TSubclassOf<AGameModeBase> NewGMClass = LoadClass<AGameModeBase>(
			nullptr, 
			TEXT("/Game/ExtraPackages/ThirdPersionPackage/ThirdPersonBP/Blueprints/ThirdPersonGameMode.ThirdPersonGameMode_C")
		);
		if (NewGMClass)
		{
			AGameModeBase* NewGM = GetWorld()->SpawnActor<AGameModeBase>(NewGMClass);
			//GetWorld()->SetGameMode(NewGM);
			//GetWorld()->GetAuthGameMode();
			//NewGM->StartPlay();
		}
	}
}

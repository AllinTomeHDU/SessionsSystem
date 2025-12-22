// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/Character.h"
#include "MultiplayerGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API UMultiplayerGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:


private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACharacter> SelectedChrClass;

public:
	FORCEINLINE void SetSelectedChrClass(TSubclassOf<ACharacter> InClass) { SelectedChrClass = InClass; }
	FORCEINLINE UClass* GetSelectedChrClass() const { return SelectedChrClass; }
};

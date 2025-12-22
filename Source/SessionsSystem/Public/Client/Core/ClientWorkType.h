// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "ClientWorkType.generated.h"

class ACharacter;
class UTexture;


USTRUCT(BlueprintType)
struct SESSIONSSYSTEM_API FClientUserAssets
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SpiritStone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ImmortalJade;
};

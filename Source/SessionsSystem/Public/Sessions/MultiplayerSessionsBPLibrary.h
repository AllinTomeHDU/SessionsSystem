// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Definition/MultiplayerSessionsType.h"
#include "MultiplayerSessionsBPLibrary.generated.h"


/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API UMultiplayerSessionsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static bool MultiplayerServerTravel(UObject* WorldContextObject, const FString& Address, bool bAbsolute = false, bool bShouldSkipGameNotify = false);

};

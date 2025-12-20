// Copyright: Jichao Luo


#include "Sessions/MultiplayerSessionsBPLibrary.h"


bool UMultiplayerSessionsBPLibrary::MultiplayerServerTravel(UObject* WorldContextObject, const FString& Address, bool bAbsolute, bool bShouldSkipGameNotify)
{
	if (!IsValid(WorldContextObject)) return false;

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (IsValid(World))
	{
		return World->ServerTravel(Address, bAbsolute, bShouldSkipGameNotify);
	}
	return false;
}

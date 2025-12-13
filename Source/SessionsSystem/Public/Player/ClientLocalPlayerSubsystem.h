// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ClientLocalPlayerSubsystem.generated.h"

class FNetChannelManager;
class FNetChannelBase;

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API UClientLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
	FNetChannelManager* Client;

public:
	FORCEINLINE FNetChannelManager* GetClient() const { return Client; }

	void ClientLoginOrRegister(FString& UserID, FString& UserName);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void RecvProtocol(uint32 ProtocolNumber, FNetChannelBase* Channel);

private:

};

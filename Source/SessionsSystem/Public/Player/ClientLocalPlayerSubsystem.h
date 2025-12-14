// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "ClientLocalPlayerSubsystem.generated.h"

class FNetChannelManager;
class FNetChannelBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FClientLoginCompleteDelegate, bool);


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

	FClientLoginCompleteDelegate OnClientLoginComplete;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void JoinCompleteCallback(bool bWasSuccess);
	void RecvProtocolCallback(uint32 ProtocolNumber, FNetChannelBase* Channel);

private:
	void TryLoginOrRegister();

};

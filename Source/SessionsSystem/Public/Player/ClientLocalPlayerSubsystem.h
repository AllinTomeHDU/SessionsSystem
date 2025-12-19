// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "DS_NetChannel/Core/NetChannelType.h"
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

private:
	void TryLogin();

	void JoinCompleteCallback(bool bWasSuccess);

	void RecvGateCallback(uint32 ProtocolNumber, FNetChannelBase* Channel);
	void RecvHallCallback(uint32 ProtocolNumber, FNetChannelBase* Channel);
};

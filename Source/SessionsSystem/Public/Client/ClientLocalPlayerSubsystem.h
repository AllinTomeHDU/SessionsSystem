// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "DS_NetChannel/Core/NetChannelType.h"
#include "SteamHelperType.h"
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
	
public:
	FClientLoginCompleteDelegate OnClientLoginComplete;

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	void TryLoginGate();
	void TryLoginHall();

	void JoinGateCallback(bool bWasSuccess);
	void JoinHallCallback(bool bWasSuccess);

	void RecvGateCallback(uint32 ProtocolNumber, FNetChannelBase* Channel);
	void RecvHallCallback(uint32 ProtocolNumber, FNetChannelBase* Channel);


	void ClientTick();
	float TickDelta{ 0.1f };

private:
	FNetChannelManager* Client;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FSteamUserInfo PersonaUserInfo;

	int32 ReLoginTimes{ 0 };
	int32 ReLoginTimesThreshold{ 3 };

	float LoginWaitTime{ 0.f };
	float LoginWaitTimeTreshold{ 5.f };
	bool bIsLoginComplete{ false };

public:
	FORCEINLINE FNetChannelManager* GetClient() const { return Client; }
	FORCEINLINE FSteamUserInfo GetSteamUserInfo() const { return PersonaUserInfo; }
};

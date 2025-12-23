// Copyright: Jichao Luo


#include "Client/ClientLocalPlayerSubsystem.h"
#include "Client/ClientObjectController.h"
#include "Game/MultiplayerSessionsHUD.h"
#include "SteamHelperBPLibrary.h"
#include "DS_NetChannel/NetChannelGlobalInfo.h"
#include "DS_NetChannel/NetChannelManager.h"
#include "DS_NetChannel/Core/NetChannelProtocols.h"
#include "DS_NetChannel/Connection/Base/NetConnectionBase.h"
#include "DS_NetChannel/Channel/NetChannelBase.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"


void UClientLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FNetConfigInfo NetConfigInfo;
	FNetChannelGlobalInfo::Get()->SetConfigInfo(NetConfigInfo);
	FNetChannelBase::SimpleControllerDelegate.BindLambda(
		[]()->UClass* { return UClientObjectController::StaticClass(); }
	);

	Client = FNetChannelManager::CreateNetChannelManager(ENetLinkState::Connect, ENetSocketType::UDP);
	if (!Client || !Client->Init())
	{
		delete Client;
		OnClientLoginComplete.Broadcast(false);
		return;
	}

	if (Client->GetController())
	{
		Client->GetController()->JoinDelegate.AddUObject(this, &ThisClass::JoinGateCallback);
		Client->GetController()->RecvDelegate.AddUObject(this, &ThisClass::RecvGateCallback);
	}

	// 此处也可作平台权限认定，限制登录平台
	if (IOnlineSubsystem::Get()->GetSubsystemName().ToString() == TEXT("STEAM"))
	{
		USteamHelperBPLibrary::GetPersonalUserInfo(PersonaUserInfo);
	}
	else
	{
		PersonaUserInfo.SteamID = TEXT("123123");
		PersonaUserInfo.PersonaName = TEXT("Unreal");
		PersonaUserInfo.IPCountry = TEXT("CN");
	}

	if (GetWorld())
	{
		/*
		* 1.接收服务器的信息
		* 2.发送心跳协议
		* 3.LocalConnect与MainChannel更新Tick逻辑
		*/
		FTimerHandle ClientTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			ClientTimerHandle,
			this,
			&ThisClass::ClientTick,
			TickDelta,
			true
		);
	}
}

void UClientLocalPlayerSubsystem::ClientTick()
{
	Client->Tick(TickDelta);

	if (!bIsLoginComplete)
	{
		LoginWaitTime += TickDelta;
		if (LoginWaitTime > LoginWaitTimeTreshold)
		{
			OnClientLoginComplete.Broadcast(false);
			bIsLoginComplete = true;
		}
	}
}

void UClientLocalPlayerSubsystem::TryLoginGate()
{
	if (Client && Client->GetController())
	{
		if (auto Channel = Client->GetController()->GetChannel())
		{
			NETCHANNEL_PROTOCOLS_SEND(P_Login, PersonaUserInfo.SteamID, PersonaUserInfo.PersonaName, PersonaUserInfo.IPCountry);
		}
	}
}

void UClientLocalPlayerSubsystem::TryLoginHall()
{
	if (Client && Client->GetController())
	{
		if (auto Channel = Client->GetController()->GetChannel())
		{
			NETCHANNEL_PROTOCOLS_SEND(P_Login, PersonaUserInfo.SteamID);
		}
	}
}

void UClientLocalPlayerSubsystem::JoinGateCallback(bool bWasSuccess)
{
	if (bWasSuccess)
	{
		TryLoginGate();
		ReLoginTimes = 0;
		LoginWaitTime = 0;
	}
	else
	{
		if (++ReLoginTimes > ReLoginTimesThreshold)
		{
			OnClientLoginComplete.Broadcast(false);
			return;
		}

		if (Client && Client->GetLocalConnection().IsValid())
		{
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this]() { Client->GetLocalConnection()->Verify(); },
				1.f,
				false
			);
		}
	}
}

void UClientLocalPlayerSubsystem::JoinHallCallback(bool bWasSuccess)
{
	if (bWasSuccess)
	{
		TryLoginHall();
		ReLoginTimes = 0;
		LoginWaitTime = 0;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Client Join Gate Failed, try join again..."));
		if (Client && Client->GetLocalConnection().IsValid())
		{
			if (++ReLoginTimes > ReLoginTimesThreshold)
			{
				OnClientLoginComplete.Broadcast(false);
				return;
			}

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this]() { Client->GetLocalConnection()->Verify(); },
				1.f,
				false
			);
		}
	}
}

void UClientLocalPlayerSubsystem::RecvGateCallback(uint32 ProtocolNumber, FNetChannelBase* Channel)
{
	switch (ProtocolNumber)
	{
		case P_LoginSuccess:
		{
			FNetServerInfo HallServerInfo;
			NETCHANNEL_PROTOCOLS_RECV(P_LoginSuccess, HallServerInfo);

			Channel->CloseConnect();
			if (Client->Bind(HallServerInfo.Addr))
			{
				Client->GetController()->JoinDelegate.AddUObject(this, &ThisClass::JoinHallCallback);
				Client->GetController()->RecvDelegate.AddUObject(this, &ThisClass::RecvHallCallback);
			}
			ReLoginTimes = 0;
			LoginWaitTime = 0;
			break;
		}
		case P_LoginFailure:
		{
			FString ErrorMsg;
			NETCHANNEL_PROTOCOLS_RECV(P_LoginFailure, ErrorMsg);
			UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
			if (++ReLoginTimes > ReLoginTimesThreshold)
			{
				OnClientLoginComplete.Broadcast(false);
				break;
			}

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle, 
				[this]() { TryLoginGate(); }, 
				1.f, 
				false
			);
			break;
		}
		default:
			break;
	}
}

void UClientLocalPlayerSubsystem::RecvHallCallback(uint32 ProtocolNumber, FNetChannelBase* Channel)
{
	switch (ProtocolNumber)
	{
		case P_LoginSuccess:
		{
			bIsLoginSuccess = true;
			OnClientLoginComplete.Broadcast(true);
			bIsLoginComplete = true;
			break;
		}
		case P_LoginFailure:
		{
			FString ErrorMsg;
			NETCHANNEL_PROTOCOLS_RECV(P_LoginFailure, ErrorMsg);
			UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
			if (++ReLoginTimes > ReLoginTimesThreshold)
			{
				OnClientLoginComplete.Broadcast(false);
				break;
			}

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this]() { TryLoginHall(); },
				1.f,
				false
			);
			break;
		}
	}
}



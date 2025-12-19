// Copyright: Jichao Luo


#include "Player/ClientLocalPlayerSubsystem.h"
#include "Player/ClientObjectController.h"
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


#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
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
		UE_LOG(LogTemp, Error, TEXT("Client Create Failed"));
		return;
	}

	if (Client->GetController())
	{
		Client->GetController()->JoinDelegate.AddUObject(this, &ThisClass::JoinGateCallback);
		Client->GetController()->RecvDelegate.AddUObject(this, &ThisClass::RecvGateCallback);
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
			[&]() { Client->Tick(0.1f); },
			0.1f,
			true
		);
	}
}

void UClientLocalPlayerSubsystem::TryLoginGate()
{
	// 此处也可作Steam权限认定，限制登录平台
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
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Client Join Gate Failed, try join again..."));
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
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Client Join Gate Failed, try join again..."));
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
			break;
		}
		case P_LoginFailure:
		{
			OnClientLoginComplete.Broadcast(false);

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
			FNetUserAssetInfo UserAssets;
			NETCHANNEL_PROTOCOLS_RECV(P_ResponseUserAssetInfo, UserAssets);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
				FString::Printf(TEXT("Rank:%s, SpiritStone:%d, ImmortalJade:%d"),
					UTF8_TO_TCHAR(UserAssets.Rank), UserAssets.SpiritStone, UserAssets.ImmortalJade));

			OnClientLoginComplete.Broadcast(true);
			break;
		}
		case P_LoginFailure:
		{
			OnClientLoginComplete.Broadcast(false);
			break;
		}
		case P_ResponseUserAssetInfo:
		{
			//FNetUserAssetInfo UserAssets;
			//NETCHANNEL_PROTOCOLS_RECV(P_ResponseUserAssetInfo, UserAssets);

			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			//	FString::Printf(TEXT("Rank:%s, SpiritStone:%d, ImmortalJade:%d"),
			//	UTF8_TO_TCHAR(UserAssets.Rank), UserAssets.SpiritStone, UserAssets.ImmortalJade));
			break;
		}
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif


// Copyright: Jichao Luo


#include "Player/ClientLocalPlayerSubsystem.h"
#include "Player/ClientObjectController.h"
#include "Game/MultiplayerSessionsHUD.h"
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
		Client->GetController()->JoinDelegate.AddUObject(this, &ThisClass::JoinCompleteCallback);
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

void UClientLocalPlayerSubsystem::TryLogin()
{
	//if (IOnlineSubsystem::Get()->GetSubsystemName().ToString() != TEXT("STEAM"))
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Please Login in Steam"));
	//	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
	//}

	auto IdentityPtr = IOnlineSubsystem::Get()->GetIdentityInterface();
	if (!IdentityPtr.IsValid()) return;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	//FString UserID = IdentityPtr->GetUniquePlayerId(0).ToSharedRef().Get().ToString();
	//FString UserName = IdentityPtr->GetPlayerNickname(*LocalPlayer->GetPreferredUniqueNetId());
	FString UserID = TEXT("123123");
	FString UserName = TEXT("Luo");
	if (Client && Client->GetController())
	{
		if (auto Channel = Client->GetController()->GetChannel())
		{
			NETCHANNEL_PROTOCOLS_SEND(P_Login, UserID, UserName);
		}
	}
}

void UClientLocalPlayerSubsystem::JoinCompleteCallback(bool bWasSuccess)
{
	if (bWasSuccess)
	{
		TryLogin();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Client Join Failed, try join again..."));
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
				Client->GetController()->JoinDelegate.AddUObject(this, &ThisClass::JoinCompleteCallback);
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
				[this]() { TryLogin(); }, 
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
			OnClientLoginComplete.Broadcast(true);
			break;
		}
		case P_LoginFailure:
		{
			OnClientLoginComplete.Broadcast(false);
			break;
		}
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif


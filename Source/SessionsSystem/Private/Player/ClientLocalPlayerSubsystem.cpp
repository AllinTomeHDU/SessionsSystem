// Copyright: Jichao Luo


#include "Player/ClientLocalPlayerSubsystem.h"
#include "Player/ClientObjectController.h"
#include "Game/MultiplayerSessionsHUD.h"
#include "DSUNetChannel/NetChannelGlobalInfo.h"
#include "DSUNetChannel/NetChannelManager.h"
#include "DSUNetChannel/Core/NetChannelProtocols.h"
#include "DSUNetChannel/Connection/Base/NetConnectionBase.h"
#include "DSUNetChannel/Channel/NetChannelBase.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"


void UClientLocalPlayerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	FNetChannelGlobalInfo::Get()->Init();

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
		Client->GetController()->JoinDelegate.AddLambda(
			[this](bool bWasSuccess)
			{
				JoinCompleteCallback(bWasSuccess);
			}
		);
		Client->GetController()->RecvDelegate.AddLambda(
			[this](uint32 ProtocolNumber, FNetChannelBase* Channel)
			{
				RecvProtocolCallback(ProtocolNumber, Channel);
			}
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Client->GetController() is nullptr"));
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

void UClientLocalPlayerSubsystem::JoinCompleteCallback(bool bWasSuccess)
{
	if (bWasSuccess)
	{
		TryLoginOrRegister();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Client Join Failed, try join again..."));
		if (Client && Client->GetLocalConnection().IsValid())
		{
			Client->GetLocalConnection()->Verify();
		}
	}
}

#if PLATFORM_WINDOWS
#pragma optimize("",off)
#endif
void UClientLocalPlayerSubsystem::RecvProtocolCallback(uint32 ProtocolNumber, FNetChannelBase* Channel)
{
	switch (ProtocolNumber)
	{
		case P_LoginSuccess:
		{
			FNetServerInfo HallServerInfo;
			NETCHANNEL_PROTOCOLS_RECV(P_LoginSuccess, HallServerInfo);

			UE_LOG(LogTemp, Display, TEXT("ID: %d, Name: %s"), 
				HallServerInfo.ID, UTF8_TO_TCHAR(HallServerInfo.Name));

			if (OnClientLoginComplete.IsBound())
			{
				OnClientLoginComplete.Broadcast(true);
			}
			break;
		}
		case P_LoginFailure:
		{
			if (OnClientLoginComplete.IsBound())
			{
				OnClientLoginComplete.Broadcast(false);
			}
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle,
				[this]() { TryLoginOrRegister(); },
				1.f,
				false
			);
			break;
		}
		default:
			break;
	}
}
#if PLATFORM_WINDOWS
#pragma optimize("",on)
#endif

void UClientLocalPlayerSubsystem::TryLoginOrRegister()
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


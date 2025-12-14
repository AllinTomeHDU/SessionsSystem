// Copyright: Jichao Luo


#include "Player/ClientLocalPlayerSubsystem.h"
#include "Player/ClientObjectController.h"
#include "Game/MultiplayerSessionsHUD.h"
#include "DSUNetChannel/NetChannelGlobalInfo.h"
#include "DSUNetChannel/NetChannelManager.h"
#include "DSUNetChannel/Core/NetChannelProtocols.h"
#include "DSUNetChannel/Channel/NetChannelBase.h"
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
		UE_LOG(LogTemp, Log, TEXT("RecvDelegate.AddLambda"));
		Client->GetController()->RecvDelegate.AddLambda(
			[this](uint32 ProtocolNumber, FNetChannelBase* Channel)
			{
				RecvProtocol(ProtocolNumber, Channel);
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

void UClientLocalPlayerSubsystem::RecvProtocol(uint32 ProtocolNumber, FNetChannelBase* Channel)
{
	UE_LOG(LogTemp, Log, TEXT("RecvProtocol: %d"), ProtocolNumber);

	auto PC = GetWorld()->GetFirstPlayerController();
	auto HUD = Cast<AMultiplayerSessionsHUD>(PC->GetHUD());
	if (!HUD) return;

	switch (ProtocolNumber)
	{
		case P_LoginSuccess:
		{
			HUD->OnLoginCompleted(true);
			break;
		}
		case P_LoginFailure:
		{
			FString ErrorMsg;
			NETCHANNEL_PROTOCOLS_RECV(P_LoginFailure, ErrorMsg);
			UE_LOG(LogTemp, Error, TEXT("%s"), *ErrorMsg);
			HUD->OnLoginCompleted(false);
			break;
		}
	}
}

void UClientLocalPlayerSubsystem::ClientLoginOrRegister(FString& UserID, FString& UserName)
{
	if (!Client || !Client->GetController() || 
		Client->GetLocalConnectState() != ENetConnectionState::Join)
	{
		auto PC = GetWorld()->GetFirstPlayerController();
		auto HUD = Cast<AMultiplayerSessionsHUD>(PC->GetHUD());
		HUD->OnLoginCompleted(false);
		return;
	}

	if (auto Channel = Client->GetController()->GetChannel())
	{
		NETCHANNEL_PROTOCOLS_SEND(P_Login, UserID, UserName);
	}
}

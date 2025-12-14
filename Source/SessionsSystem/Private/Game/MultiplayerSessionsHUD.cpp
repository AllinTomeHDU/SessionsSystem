// Copyright: Jichao Luo


#include "Game/MultiplayerSessionsHUD.h"
#include "Player/ClientLocalPlayerSubsystem.h"
#include "UI/ClientLoginWidget.h"
#include "UI/MultiplayerSessionsWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"


void AMultiplayerSessionsHUD::BeginPlay()
{
	Super::BeginPlay();

	checkf(LoginWidgetClass, TEXT("LoginWidgetClass is nullptr"));

	LoginWidget = CreateWidget<UClientLoginWidget>(GetWorld(), LoginWidgetClass);
	if (LoginWidget)
	{
		LoginWidget->AddToViewport();
	}

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	auto IdentityPtr = IOnlineSubsystem::Get()->GetIdentityInterface();
	if (IdentityPtr.IsValid())
	{
		FString UserID = IdentityPtr->GetUniquePlayerId(0).ToSharedRef().Get().ToString();
		FString UserName = IdentityPtr->GetPlayerNickname(*LocalPlayer->GetPreferredUniqueNetId());
		LoginWidget->SetUserName(UserName);
	}

	if (auto ClientSubsystem = LocalPlayer->GetSubsystem<UClientLocalPlayerSubsystem>())
	{
		ClientSubsystem->OnClientLoginComplete.AddLambda(
			[this](bool bWasSuccess) 
			{
				OnLoginCompleted(bWasSuccess);
			}
		);
	}
}

void AMultiplayerSessionsHUD::OnLoginCompleted(const bool bWasSuccess)
{
	UE_LOG(LogTemp, Log, TEXT("On Login Completed: %d"), bWasSuccess);

	if (bWasSuccess)
	{
		checkf(HallWidgetClass, TEXT("HallWidgetClass is nullptr"));
		LoginWidget->RemoveFromParent();
		HallWidget = CreateWidget<UMultiplayerSessionsWidget>(GetWorld(), HallWidgetClass);
		if (HallWidget)
		{
			HallWidget->AddToViewport();
		}
	}
	else
	{

	}
}

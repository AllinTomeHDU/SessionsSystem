// Copyright: Jichao Luo


#include "Game/MultiplayerSessionsHUD.h"
#include "Client/ClientLocalPlayerSubsystem.h"
#include "UI/ClientLoginWidget.h"
#include "UI/GameMainPageWidget.h"
#include "UI/MultiplayerSessionsWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"


void AMultiplayerSessionsHUD::BeginPlay()
{
	Super::BeginPlay();

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (auto ClientSubsystem = LocalPlayer->GetSubsystem<UClientLocalPlayerSubsystem>())
	{
		if (ClientSubsystem->IsLoginComplete())
		{
			if (ClientSubsystem->IsLoginSuccess())
			{
				checkf(HallWidgetClass, TEXT("HallWidgetClass is nullptr"));
				HallWidget = CreateWidget<UMultiplayerSessionsWidget>(GetWorld(), HallWidgetClass);
				HallWidget->AddToViewport();
			}
			else
			{
				CreateMainPageWidget(false);
			}
		}
		else
		{
			checkf(LoginWidgetClass, TEXT("LoginWidgetClass is nullptr"));
			LoginWidget = CreateWidget<UClientLoginWidget>(GetWorld(), LoginWidgetClass);
			LoginWidget->AddToViewport();

			auto IdentityPtr = IOnlineSubsystem::Get()->GetIdentityInterface();
			if (IdentityPtr.IsValid())
			{
				FString UserID = IdentityPtr->GetUniquePlayerId(0).ToSharedRef().Get().ToString();
				FString UserName = IdentityPtr->GetPlayerNickname(*LocalPlayer->GetPreferredUniqueNetId());
				LoginWidget->SetUserName(UserName);
			}

			ClientSubsystem->OnClientLoginComplete.AddLambda(
				[this](bool bWasSuccess)
				{
					OnLoginCompleted(bWasSuccess);
				}
			);
		}
	}

	
}

void AMultiplayerSessionsHUD::OnLoginCompleted(const bool bWasSuccess)
{
	UE_LOG(LogTemp, Log, TEXT("On Login Completed: %d"), bWasSuccess);

	LoginWidget->RemoveFromParent();
	CreateMainPageWidget(true);
}

void AMultiplayerSessionsHUD::CreateMainPageWidget(bool bLoginSuccess)
{
	checkf(MainPageWidgetClass, TEXT("MainPageWidgetClass is nullptr"));
	MainPageWidget = CreateWidget<UGameMainPageWidget>(GetWorld(), MainPageWidgetClass);
	MainPageWidget->Init(bLoginSuccess);
	MainPageWidget->AddToViewport();
}
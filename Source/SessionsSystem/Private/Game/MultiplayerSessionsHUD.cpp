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

	TryLoginOrRegister();
}

void AMultiplayerSessionsHUD::TryLoginOrRegister()
{
	//if (IOnlineSubsystem::Get()->GetSubsystemName().ToString() != TEXT("STEAM"))
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Please Login in Steam"));
	//	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, true);
	//}

	auto IdentityPtr = IOnlineSubsystem::Get()->GetIdentityInterface();
	if (!IdentityPtr.IsValid()) return;

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	FString UserID = IdentityPtr->GetUniquePlayerId(0).ToSharedRef().Get().ToString();
	FString UserName = IdentityPtr->GetPlayerNickname(*LocalPlayer->GetPreferredUniqueNetId());
	LoginWidget->SetUserName(UserName);

	if (auto ClientSubsystem = LocalPlayer->GetSubsystem<UClientLocalPlayerSubsystem>())
	{
		ClientSubsystem->ClientLoginOrRegister(UserID, UserName);
		//FTimerHandle TimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(
		//	TimerHandle,
		//	[&]()
		//	{
		//		ClientSubsystem->ClientLoginOrRegister(UserID, UserName);
		//	},
		//	1.f,
		//	false
		//);
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
		// ³¢ÊÔÖØÐÂµÇÂ¼
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("try login again...")));

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[this]()
			{
				TryLoginOrRegister();
			},
			2.f,
			false
		);
	}
}

// Copyright: Jichao Luo


#include "Game/MultiplayerSessionsHUD.h"
#include "UI/ClientLoginWidget.h"
#include "UI/GameMainPageWidget.h"
#include "UI/MultiplayerSessionsWidget.h"
#include "OnlineSubsystem.h"
//#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "SteamHelperBPLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

//#ifdef CLIENT_SYSTEM
#include "ClientLocalPlayerSubsystem.h"
#include "ClientLoginInterface.h"
//#endif


void AMultiplayerSessionsHUD::BeginPlay()
{
	Super::BeginPlay();

//#ifdef CLIENT_SYSTEM
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (auto ClientSubsystem = LocalPlayer->GetSubsystem<UClientLocalPlayerSubsystem>())
	{
		ClientSubsystem->OnClientJoinGateComplete.AddUObject(this, &ThisClass::OnJoinGageCompleted);

		ClientSubsystem->OnClientAccountAlreadyExits.AddUObject(this, &ThisClass::OnAccountAlreadyExits);
		ClientSubsystem->OnClientRegisterComplete.AddUObject(this, &ThisClass::OnRegisterCompleted);

		ClientSubsystem->OnClientAbsentAccount.AddUObject(this, &ThisClass::OnAbsentAccount);
		ClientSubsystem->OnClientLoginComplete.AddUObject(this, &ThisClass::OnLoginCompleted);
	}
//#endif
}

//#ifdef CLIENT_SYSTEM
void AMultiplayerSessionsHUD::OnJoinGageCompleted()
{
	checkf(LoginWidgetClass, TEXT("LoginWidgetClass is nullptr"));
	LoginWidget = CreateWidget<UUserWidget>(GetWorld(), LoginWidgetClass);
	if (IsValid(LoginWidget))
	{
		LoginWidget->AddToViewport();
	}

	// 可根据不同平台做特殊逻辑处理，比如：通过Steam验证成功则自动登录
	Platform = IOnlineSubsystem::Get()->GetSubsystemName().ToString();
	if (LoginWidget->GetClass()->ImplementsInterface(UClientLoginInterface::StaticClass()))
	{
		IOnlineIdentityPtr Identity = IOnlineSubsystem::Get()->GetIdentityInterface();
		FString Account = Identity->GetUniquePlayerId(0)->ToString();
		IClientLoginInterface::Execute_SetLoginInfo(LoginWidget, Account, TEXT(""), Platform);
		if (Platform == TEXT("STEAM"))
		{
			IClientLoginInterface::Execute_ClickLogin(LoginWidget);
		}
	}
}
//#endif

//#ifdef CLIENT_SYSTEM
void AMultiplayerSessionsHUD::OnAccountAlreadyExits()
{
	if (LoginWidget->GetClass()->ImplementsInterface(UClientLoginInterface::StaticClass()))
	{
		IClientLoginInterface::Execute_AccountAlreadyExits(LoginWidget);
	}
}

void AMultiplayerSessionsHUD::OnRegisterCompleted(const bool bWasSuccess)
{
	if (LoginWidget->GetClass()->ImplementsInterface(UClientLoginInterface::StaticClass()))
	{
		IClientLoginInterface::Execute_RegisterComplete(LoginWidget, bWasSuccess);

		if (Platform == TEXT("STEAM"))
		{
			IClientLoginInterface::Execute_ClickLogin(LoginWidget);
		}
	}
}

//#endif

//#ifdef CLIENT_SYSTEM
void AMultiplayerSessionsHUD::OnAbsentAccount()
{
	if (LoginWidget->GetClass()->ImplementsInterface(UClientLoginInterface::StaticClass()))
	{
		IClientLoginInterface::Execute_SetPlatform(LoginWidget, Platform);
		if (Platform == TEXT("STEAM"))
		{
			FClientUserInfo UserInfo;
			UserInfo.Account = USteamHelperBPLibrary::GetSteamID();
			UserInfo.Name = USteamHelperBPLibrary::GetPersonaName();
			UserInfo.IPCountry = USteamHelperBPLibrary::GetIPCountry();
			UserInfo.Platform = Platform;
			IClientLoginInterface::Execute_SetRegisterInfo(LoginWidget, UserInfo);
			IClientLoginInterface::Execute_ClickRegister(LoginWidget);
		}
		else
		{
			IClientLoginInterface::Execute_AbsentAccount(LoginWidget);
		}
	}
}

void AMultiplayerSessionsHUD::OnLoginCompleted(const bool bWasSuccess)
{
	LoginWidget->RemoveFromParent();
	CreateMainPageWidget(bWasSuccess);
}
//#endif

void AMultiplayerSessionsHUD::CreateMainPageWidget(bool bLoginSuccess)
{
	checkf(MainPageWidgetClass, TEXT("MainPageWidgetClass is nullptr"));
	MainPageWidget = CreateWidget<UGameMainPageWidget>(GetWorld(), MainPageWidgetClass);
	MainPageWidget->Init(bLoginSuccess);
	MainPageWidget->AddToViewport();
}
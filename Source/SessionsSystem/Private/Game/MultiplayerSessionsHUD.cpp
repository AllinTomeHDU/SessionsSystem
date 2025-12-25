// Copyright: Jichao Luo


#include "Game/MultiplayerSessionsHUD.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

#ifdef CLIENT_SYSTEM
#include "ClientLocalPlayerSubsystem.h"
#endif


void AMultiplayerSessionsHUD::BeginPlay()
{
	Super::BeginPlay();

	if (auto PC = GetWorld()->GetFirstPlayerController())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

#ifdef CLIENT_SYSTEM
	checkf(RegisterLoginWidgetClass, TEXT("RegisterLoginWidgetClass IS NULL"));
	RegisterLoginWidget = CreateWidget<UClientWidgetBase>(GetWorld(), RegisterLoginWidgetClass);
	if (IsValid(RegisterLoginWidget))
	{
		RegisterLoginWidget->AddToViewport();
		RegisterLoginWidget->OnClientWidgetDestruct.AddUObject(this, &ThisClass::OnRegisterLoginWidgetDestruct);
	}
#else
	CreateMainPageWidget();
#endif
}

#ifdef CLIENT_SYSTEM
void AMultiplayerSessionsHUD::OnRegisterLoginWidgetDestruct()
{
	CreateMainPageWidget();
}
#endif

void AMultiplayerSessionsHUD::CreateMainPageWidget()
{
	checkf(MainPageWidgetClass, TEXT("MainPageWidgetClass is nullptr"));
	MainPageWidget = CreateWidget<UUserWidget>(GetWorld(), MainPageWidgetClass);
	MainPageWidget->AddToViewport();
}



// Copyright: Jichao Luo


#include "UI/GameMainPageWidget.h"
#include "GameFramework/PlayerController.h"


void UGameMainPageWidget::Init(const bool bHasNetwork)
{
	if (auto PC = GetWorld()->GetFirstPlayerController())
	{
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
	}

	OnInitBP(bHasNetwork);
}

// Copyright: Jichao Luo


#include "Room/MultiplayerRoomSubsystem.h"
#include "TimerManager.h"


UUserWidget* UMultiplayerRoomSubsystem::ShowTransitionWidget(
	TSubclassOf<UUserWidget> InWidgetClass, float InMinDisplayTime, float InMaxDisplayTime, bool bAutoClose, int32 InZOrder)
{
	if (!GetWorld() || !IsValid(InWidgetClass)) return nullptr;
	if (IsValid(TransitionWidget))
	{
		TransitionWidget->RemoveFromParent();
		TransitionWidget = nullptr;
	}

	TransitionWidget = CreateWidget<UUserWidget>(GetGameInstance(), InWidgetClass);
	if (IsValid(TransitionWidget))
	{
		if (GEngine && GEngine->GameViewport)
		{
			GEngine->GameViewport->AddViewportWidgetContent(TransitionWidget->TakeWidget(), InZOrder);
		}

		bCanBeClosed = false;
		bPendingClose = false;
		bAutoCloseEnabled = bAutoClose;

		if (InMinDisplayTime > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				MinDisplayTimerHandle,
				this,
				&ThisClass::OnMinDisplayTimeReached,
				InMinDisplayTime,
				false
			);
		}
		else
		{
			OnMinDisplayTimeReached();
		}

		if (bAutoClose && InMaxDisplayTime > 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				MaxDisplayTimerHandle,
				this,
				&ThisClass::OnMaxDisplayTimeReached,
				InMaxDisplayTime,
				false
			);
		}
	}

	return TransitionWidget;
}

void UMultiplayerRoomSubsystem::RequestCloseTransitionWidget()
{
	if (bCanBeClosed)
	{
		PerformCloseTransitionWidget();
	}
	else
	{
		bPendingClose = true;
	}
}

void UMultiplayerRoomSubsystem::OnMinDisplayTimeReached()
{
	bCanBeClosed = true;

	if (bPendingClose)
	{
		PerformCloseTransitionWidget();
	}
}

void UMultiplayerRoomSubsystem::OnMaxDisplayTimeReached()
{
	PerformCloseTransitionWidget();
}

void UMultiplayerRoomSubsystem::PerformCloseTransitionWidget()
{
	if (!IsValid(TransitionWidget))
		return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MinDisplayTimerHandle);
		World->GetTimerManager().ClearTimer(MaxDisplayTimerHandle);
	}

	if (GEngine && GEngine->GameViewport && TransitionWidget)
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(TransitionWidget->TakeWidget());
	}

	bCanBeClosed = false;
	bPendingClose = false;
}

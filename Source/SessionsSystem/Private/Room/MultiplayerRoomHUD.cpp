// Copyright: Jichao Luo


#include "Room/MultiplayerRoomHUD.h"
#include "UI/MultiplayerRoomWidget.h"


void AMultiplayerRoomHUD::BeginPlay()
{
	Super::BeginPlay();

	checkf(RoomWidgetClass, TEXT("RoomWidgetClass is nullptr"));
	RoomWidget = CreateWidget<UMultiplayerRoomWidget>(GetWorld(), RoomWidgetClass);
	if (IsValid(RoomWidget))
	{
		RoomWidget->AddToViewport();
	}
}

// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiplayerRoomHUD.generated.h"

class UMultiplayerRoomWidget;

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API AMultiplayerRoomHUD : public AHUD
{
	GENERATED_BODY()
	
public:

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SessionsSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMultiplayerRoomWidget> RoomWidgetClass;

	UPROPERTY()
	UMultiplayerRoomWidget* RoomWidget;
};

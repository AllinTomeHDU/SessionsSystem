// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiplayerSessionsHUD.generated.h"

class UClientLoginWidget;
class UGameMainPageWidget;
class UMultiplayerSessionsWidget;
class FNetChannelBase;

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API AMultiplayerSessionsHUD : public AHUD
{
	GENERATED_BODY()

public:
	void OnLoginCompleted(const bool bWasSuccess);
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SessionsSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UClientLoginWidget> LoginWidgetClass;

	UPROPERTY()
	UClientLoginWidget* LoginWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SessionsSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameMainPageWidget> MainPageWidgetClass;

	UPROPERTY()
	UGameMainPageWidget* MainPageWidget;
};

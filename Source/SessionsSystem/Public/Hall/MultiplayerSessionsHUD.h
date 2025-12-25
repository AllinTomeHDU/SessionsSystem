// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ClientWidgetBase.h"

#ifdef CLIENT_SYSTEM
#include "ClientWidgetBase.h"
#endif

#include "MultiplayerSessionsHUD.generated.h"

class UGameMainPageWidget;

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API AMultiplayerSessionsHUD : public AHUD
{
	GENERATED_BODY()

public:
	
protected:
	virtual void BeginPlay() override;

	void CreateMainPageWidget();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SessionsSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UClientWidgetBase> RegisterLoginWidgetClass;

	UPROPERTY()
	UClientWidgetBase* RegisterLoginWidget;

#ifdef CLIENT_SYSTEM
	void OnRegisterLoginWidgetDestruct();
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SessionsSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> MainPageWidgetClass;

	UPROPERTY()
	UUserWidget* MainPageWidget;
};

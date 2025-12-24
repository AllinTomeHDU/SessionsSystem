// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MultiplayerSessionsHUD.generated.h"

class UGameMainPageWidget;
class UMultiplayerSessionsWidget;

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

	void OnJoinGageCompleted();

	void OnAccountAlreadyExits();
	void OnRegisterCompleted(const bool bWasSuccess);

	void OnAbsentAccount();
	void OnLoginCompleted(const bool bWasSuccess);

	void CreateMainPageWidget(bool bLoginSuccess);

private:
//#ifdef CLIENT_SYSTEM
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SessionsSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LoginWidgetClass;

	UPROPERTY()
	UUserWidget* LoginWidget;

	FString Platform;
//#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SessionsSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameMainPageWidget> MainPageWidgetClass;

	UPROPERTY()
	UGameMainPageWidget* MainPageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SessionsSystem", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UMultiplayerSessionsWidget> HallWidgetClass;

	UPROPERTY()
	UMultiplayerSessionsWidget* HallWidget;

};

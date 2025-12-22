// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "MultiplayerRoomController.generated.h"

class UInputMappingContext;
class UInputAction;

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API AMultiplayerRoomController : public APlayerController
{
	GENERATED_BODY()

public:
	AMultiplayerRoomController();
	
protected:
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;

	void OnClickAction();


private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* IMC_Default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* IA_Click;


};

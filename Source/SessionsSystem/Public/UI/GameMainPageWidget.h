// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMainPageWidget.generated.h"

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API UGameMainPageWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void Init(const bool bHasNetwork = true);


protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitBP(const bool bHasNetwork);

private:

};

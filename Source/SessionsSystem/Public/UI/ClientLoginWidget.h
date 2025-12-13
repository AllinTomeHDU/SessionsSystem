// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientLoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API UClientLoginWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString UserName;

public:
	FORCEINLINE const FString& GetUserName() const { return UserName; }
	FORCEINLINE void SetUserName(const FString& InName) { UserName = InName; }
};

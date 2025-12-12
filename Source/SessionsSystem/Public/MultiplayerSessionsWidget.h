// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiplayerSessionsBPLibrary.h"
#include "MultiplayerSessionsWidget.generated.h"

/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API UMultiplayerSessionsWidget : public UUserWidget
{
	GENERATED_BODY()
	
	UPROPERTY()
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

public:


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnCreateSession();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDestroySession();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFindSessions(const TMap<FString, FSSSearchResult>& SearchResults);

	UFUNCTION(BlueprintImplementableEvent)
	void OnJoinSession(const FString& JoinResult);

private:
	void OnCreateSessionComplete(bool bWasSuccessful);
	void OnDestroySessionComplete(bool bWasSuccessful);
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSeccessful);
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type ResultType);
};

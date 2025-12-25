// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Sessions/MultiplayerSessionsBPLibrary.h"
#include "Interfaces/OnlineSessionInterface.h"
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

	UPROPERTY()
	class UClientLocalPlayerSubsystem* ClientPlayerSubsystem;

public:


protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnCreateSession();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDestroySession();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFindSessions(const TMap<FString, FSessionsSearchResult>& SearchResults);

	UFUNCTION(BlueprintImplementableEvent)
	void OnJoinSession(const ESessionsJoinResult JoinResult);

private:
	void OnCreateSessionComplete(bool bWasSuccessful);
	void OnDestroySessionComplete(bool bWasSuccessful);
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSeccessful);
	void OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type ResultType);

private:
	FTimerHandle FindSessionsHandle;
	float FindSeesionsInterval = 5.f;

	/*
	* 大厅界面通常会展示用户的个人信息或好友信息，这些信息和平台有关，也可能和数据库相关联，
	* 也就是说，不同的业务层需要的数据信息来源不一致，甚至需要从一些位置模块获取，
	* 所以此处底层C++不定义这些数据，这些数据由上层的蓝图Widget自己获取，上层的蓝图Widget仅做演示。
	*/
};

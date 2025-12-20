// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsBPLibrary.h"
#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMultiplayerSessionCreate, bool bWasSeccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMultiplayerSessionDestroy, bool bWasSeccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMultiplayerSessionStart, bool bWasSeccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMultiplayerSessionEnd, bool bWasSeccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMultiplayerSessionUpdate, bool bWasSeccessful);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMultiplayerSessionsFind, const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSeccessful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMultiplayerSessionJoin, EOnJoinSessionCompleteResult::Type Result);


/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
	IOnlineSessionPtr SessionInterface;

public:
	UMultiplayerSessionsSubsystem();

	// Debug
	FNamedOnlineSession* GetNamedSession(FName SessionName)
	{
		if (SessionInterface.IsValid())
		{
			return SessionInterface->GetNamedSession(SessionName);
		}
		return nullptr;
	}


	/*
	* Create Session
	*/
public:
	UFUNCTION(BlueprintCallable)
	void MultiplayerSessionCreate(const FSessionsCreateParams& Params);

	FOnMultiplayerSessionCreate OnMultiplayerSessionCreateDelegate;

private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteHandle;


	/*
	* Destroy Session
	*/
public:
	UFUNCTION(BlueprintCallable)
	void MultiplayerSessionDestroy();

	FOnMultiplayerSessionDestroy OnMultiplayerSessionDestroyDelegate;

private:
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FDelegateHandle DestroySessionCompleteHandle;


	/*
	* Start Session
	*/
public:
	UFUNCTION(BlueprintCallable)
	void MultiplayerSessionStart();

	FOnMultiplayerSessionStart OnMultiplayerSessionStartDelegate;

private:
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FDelegateHandle StartSessionCompleteHandle;


	/*
	* End Session
	*/
public:
	UFUNCTION(BlueprintCallable)
	void MultiplayerSessionEnd();

	FOnMultiplayerSessionDestroy OnMultiplayerSessionEndDelegate;

private:
	void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);
	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;
	FDelegateHandle EndSessionCompleteHandle;


	/*
	* Update Session
	*/
public:
	UFUNCTION(BlueprintCallable)
	void MultiplayerSessionUpdate(const FSessionsCreateParams& Params);

	FOnMultiplayerSessionUpdate OnMultiplayerSessionUpdateDelegate;

private:
	void OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful);
	FOnUpdateSessionCompleteDelegate OnUpdateSessionCompleteDelegate;
	FDelegateHandle UpdateSessionCompleteHandle;


	/*
	* Find Sessions
	*/
public:
	UFUNCTION(BlueprintCallable)
	void MultiplayerSessionsFind(int32 MaxSearchResults = 10000);
	FOnMultiplayerSessionsFind OnMultiplayerSessionsFindDelegate;

private:
	void OnFindSessionsComplete(bool bWasSuccessful);
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteHandle;
	TSharedPtr<FOnlineSessionSearch> LastSessionSearch;


	/*
	* Join Session
	*/
public:
	UFUNCTION(BlueprintCallable)
	void MultiplayerSessionsJoin(const FSessionsSearchResult& SearchResult);
	FOnMultiplayerSessionJoin OnMultiplayerSessionJoinDelegate;

private:
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteHandle;

};

// Copyright: Jichao Luo


#include "UI/MultiplayerSessionsWidget.h"
#include "Sessions/MultiplayerSessionsSubsystem.h"
#include "SteamHelperBPLibrary.h"


void UMultiplayerSessionsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MultiplayerSessionsSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerSessionsSubsystem>();
	if (IsValid(MultiplayerSessionsSubsystem))
	{
		MultiplayerSessionsSubsystem->OnMultiplayerSessionCreateDelegate.AddUObject(this, &ThisClass::OnCreateSessionComplete);
		MultiplayerSessionsSubsystem->OnMultiplayerSessionDestroyDelegate.AddUObject(this, &ThisClass::OnDestroySessionComplete);
		//MultiplayerSessionsSubsystem->OnMultiplayerSessionStartDelegate.AddDynamic(this, &ThisClass::OnStartSession);
		//MultiplayerSessionsSubsystem->OnMultiplayerSessionEndDelegate.AddDynamic(this, &ThisClass::OnStartSession);
		//MultiplayerSessionsSubsystem->OnMultiplayerSessionUpdateDelegate.AddDynamic(this, &ThisClass::OnUpdateSession);

		MultiplayerSessionsSubsystem->OnMultiplayerSessionsFindDelegate.AddUObject(this, &ThisClass::OnFindSessionsComplete);
		MultiplayerSessionsSubsystem->OnMultiplayerSessionJoinDelegate.AddUObject(this, &ThisClass::OnJoinSessionComplete);
	}

	if (!USteamHelperBPLibrary::GetPersonalUserInfo(UserInfo))
	{
		UE_LOG(LogTemp, Display, TEXT("GetSteamUserInfo Failed"));
	}

	TArray<FSteamFriendInfo> FriendsInfo;
	if (!USteamHelperBPLibrary::GetFriendsInfo(FriendsInfo))
	{
		UE_LOG(LogTemp, Display, TEXT("GetFriendsInfo Failed"));
	}
}

void UMultiplayerSessionsWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UMultiplayerSessionsWidget::OnCreateSessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("Success to Create Session in Widget Base"));
		OnCreateSession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Create Session in Widget Base"));
	}
}

void UMultiplayerSessionsWidget::OnDestroySessionComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Success to Destroy Session in Widget Base"));
		OnDestroySession();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Destroy Session in Widget Base"));
	}
}

void UMultiplayerSessionsWidget::OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSeccessful)
{
	if (bWasSeccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("Success to Find Sessions in Widget Base: %d"), SessionResults.Num());
		TMap<FString, FSessionsSearchResult> SearchResultsMap;
		for (auto& Result : SessionResults)
		{
			// Temp for 5.5, force the values if epic isn't setting them, lobbies should always have these true
			UE_LOG(LogTemp, Log, TEXT("bUsesPresence:%d, bUseLobbiesIfAvailable:%d"),
				Result.Session.SessionSettings.bUsesPresence, Result.Session.SessionSettings.bUseLobbiesIfAvailable);

			FSessionsSearchResult BPResult;
			BPResult.SessionResult = Result;
			Result.Session.SessionSettings.Get(FName("RoomName"), BPResult.RoomName);
			BPResult.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			BPResult.CurrentPlayers = BPResult.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Result.Session.SessionSettings.Get(FName("DifficultyLevel"), BPResult.DifficultyLevel);
			Result.Session.SessionSettings.Get(FName("IsEntertainmentMode"), BPResult.bIsEntertainmentMode);
			Result.Session.SessionSettings.Get(FName("IsPublic"), BPResult.bIsPublic);
			Result.Session.SessionSettings.Get(FName("Password"), BPResult.Password);

			FString MatchType;
			Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, FString::Printf(TEXT("RoomName: %s, UserName: %s"),
				*BPResult.RoomName, *Result.Session.OwningUserName));

			if (MatchType == TEXT("AllinTome"))
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Matched AllinTome"));
				SearchResultsMap.Add(Result.GetSessionIdStr(), BPResult);
			}
		}
		OnFindSessions(SearchResultsMap);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Find Sessions in Widget Base"));
	}
}

void UMultiplayerSessionsWidget::OnJoinSessionComplete(EOnJoinSessionCompleteResult::Type ResultType)
{
	OnJoinSession(static_cast<ESessionsJoinResult>(ResultType));
}


// Copyright: Jichao Luo


#include "UI/MultiplayerSessionsWidget.h"
#include "Sessions/MultiplayerSessionsSubsystem.h"
#include "SteamHelperBPLibrary.h"
#include "TimerManager.h"

#include "ClientLocalPlayerSubsystem.h"
#include "ClientObjectController.h"
#include "DS_NetChannel/NetChannelManager.h"

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
	
		GetWorld()->GetTimerManager().SetTimer(
			FindSessionsHandle,
			[&](){ MultiplayerSessionsSubsystem->MultiplayerSessionsFind(); },
			5.f,
			true,
			0.1f
		);
	}

	//if (auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	//{
	//	ClientPlayerSubsystem = LocalPlayer->GetSubsystem<UClientLocalPlayerSubsystem>();
	//	if (IsValid(ClientPlayerSubsystem))
	//	{
	//		UserInfo = ClientPlayerSubsystem->GetClientUserInfo();

	//		if (auto Client = ClientPlayerSubsystem->GetClient())
	//		{
	//			//if (auto ClientController = Cast<UClientObjectController>(Client->GetController()))
	//			//{
	//			//	UserAssets = ClientController->GetClientUserAssets();
	//			//}
	//		}
	//	}
	//}

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

void UMultiplayerSessionsWidget::NativeDestruct()
{
	Super::NativeDestruct();

	GetWorld()->GetTimerManager().ClearTimer(FindSessionsHandle);
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
			FSessionsSearchResult BPResult;
			BPResult.SessionResult = Result;
			BPResult.SessionID = Result.GetSessionIdStr();
			BPResult.PingMs = Result.PingInMs;
			Result.Session.SessionSettings.Get(FName("RoomName"), BPResult.RoomName);
			BPResult.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			BPResult.CurrentPlayers = BPResult.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Result.Session.SessionSettings.Get(FName("DifficultyLevel"), BPResult.DifficultyLevel);
			Result.Session.SessionSettings.Get(FName("IsEntertainmentMode"), BPResult.bIsEntertainmentMode);
			Result.Session.SessionSettings.Get(FName("IsPublic"), BPResult.bIsPublic);
			Result.Session.SessionSettings.Get(FName("Password"), BPResult.Password);

			FString MatchType;
			Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

			if (MatchType == TEXT("AllinTome"))
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Matched AllinTome"));
				SearchResultsMap.Add(Result.GetSessionIdStr(), BPResult);
			}
			UE_LOG(LogTemp, Log, TEXT("SessionID: %s"), *BPResult.SessionID);
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


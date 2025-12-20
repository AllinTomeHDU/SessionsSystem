// Copyright: Jichao Luo


#include "Sessions/MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"


UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() : 
	OnCreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	OnDestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	OnStartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete)),
	OnEndSessionCompleteDelegate(FOnEndSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnEndSessionComplete)),
	OnUpdateSessionCompleteDelegate(FOnUpdateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnUpdateSessionComplete)),
	OnFindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	OnJoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
	IOnlineSubsystem* SubsystemInterface = IOnlineSubsystem::Get();
	if (SubsystemInterface)
	{
		SessionInterface = SubsystemInterface->GetSessionInterface();
	}
}

#pragma region Create Session
void UMultiplayerSessionsSubsystem::MultiplayerSessionCreate(const FSessionsCreateParams& Params)
{
	if (!SessionInterface.IsValid()) return;
	if (FNamedOnlineSession* ExistingSeesion = SessionInterface->GetNamedSession(NAME_GameSession))
	{
		SessionInterface->DestroySession(NAME_GameSession);
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			FString::Printf(TEXT("SubsystemName: %s"), *IOnlineSubsystem::Get()->GetSubsystemName().ToString()));
	}

	CreateSessionCompleteHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");
	SessionSettings->bUsesPresence = true;
	SessionSettings->bUseLobbiesIfAvailable = true;
	SessionSettings->NumPublicConnections = Params.MaxPlayers;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->Set(FName("RoomName"), Params.RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName("DifficultyLevel"), Params.DifficultyLevel, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName("IsEntertainmentMode"), Params.bIsEntertainmentMode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings->Set(FName("IsPublic"), Params.bIsPublic, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	if (!Params.bIsPublic)
	{
		SessionSettings->Set(FName("Password"), Params.Password, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	}

	// Debug
	SessionSettings->Set(FName("MatchType"), FString("AllinTome"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
		OnMultiplayerSessionCreateDelegate.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteHandle);
	}
	OnMultiplayerSessionCreateDelegate.Broadcast(bWasSuccessful);
}
#pragma endregion

#pragma region Destroy Sessions
void UMultiplayerSessionsSubsystem::MultiplayerSessionDestroy()
{
	if (!SessionInterface.IsValid()) return;

	DestroySessionCompleteHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
		OnMultiplayerSessionDestroyDelegate.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow,
		FString::Printf(TEXT("OnDestroySessionComplete: %s, %d"), *SessionName.ToString(), bWasSuccessful));
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteHandle);
	}
	OnMultiplayerSessionDestroyDelegate.Broadcast(bWasSuccessful);
}
#pragma endregion

#pragma region Start Session
void UMultiplayerSessionsSubsystem::MultiplayerSessionStart()
{
	if (!SessionInterface.IsValid()) return;

	StartSessionCompleteHandle = SessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteHandle);
		OnMultiplayerSessionStartDelegate.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(StartSessionCompleteHandle);
	}
	OnMultiplayerSessionStartDelegate.Broadcast(bWasSuccessful);
}
#pragma endregion

#pragma region End Session
void UMultiplayerSessionsSubsystem::MultiplayerSessionEnd()
{
	if (!SessionInterface.IsValid()) return;

	EndSessionCompleteHandle = SessionInterface->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);
	if (!SessionInterface->EndSession(NAME_GameSession))
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(EndSessionCompleteHandle);
		OnMultiplayerSessionDestroyDelegate.Broadcast(false);
	}
}

void UMultiplayerSessionsSubsystem::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnEndSessionCompleteDelegate_Handle(EndSessionCompleteHandle);
	}
	OnMultiplayerSessionEndDelegate.Broadcast(bWasSuccessful);
}
#pragma endregion

#pragma region Update Session
void UMultiplayerSessionsSubsystem::MultiplayerSessionUpdate(const FSessionsCreateParams& Params)
{
	if (!SessionInterface.IsValid()) return;

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("UpdateSession")));
	//}
	//FNamedOnlineSession* CurrentSession = SessionInterface->GetNamedSession(NAME_GameSession);
	//if (!CurrentSession) return;
	//CurrentSession->SessionSettings.bShouldAdvertise = true;
	//CurrentSession->SessionSettings.NumPublicConnections = Params.MaxPlayers;
	//CurrentSession->SessionSettings.Set(FName("RoomName"), Params.RoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//CurrentSession->SessionSettings.Set(FName("DifficultyLevel"), static_cast<int32>(Params.DifficultyLevel), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//CurrentSession->SessionSettings.Set(FName("IsEntertainmentMode"), Params.bIsEntertainmentMode, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//CurrentSession->SessionSettings.Set(FName("IsPublic"), Params.bIsPublic, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//if (Params.bIsPublic)
	//{
	//	CurrentSession->SessionSettings.Remove(FName("Password"));
	//}
	//else
	//{
	//	CurrentSession->SessionSettings.Set(FName("Password"), Params.Password, EOnlineDataAdvertisementType::ViaOnlineService);
	//}

	//UpdateSessionCompleteHandle = SessionInterface->AddOnUpdateSessionCompleteDelegate_Handle(OnUpdateSessionCompleteDelegate);
	//if (!SessionInterface->UpdateSession(NAME_GameSession, CurrentSession->SessionSettings, true))
	//{
	//	SessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateSessionCompleteHandle);
	//	OnMultiplayerSessionUpdateDelegate.Broadcast(false);
	//}
}

void UMultiplayerSessionsSubsystem::OnUpdateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnUpdateSessionCompleteDelegate_Handle(UpdateSessionCompleteHandle);
	}
	OnMultiplayerSessionUpdateDelegate.Broadcast(bWasSuccessful);
}
#pragma endregion

#pragma region Find Sessions
void UMultiplayerSessionsSubsystem::MultiplayerSessionsFind(int32 MaxSearchResults)
{
	if (!SessionInterface.IsValid()) return;

	FindSessionsCompleteHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults;
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	LastSessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		UE_LOG(LogTemp, Warning, TEXT("FindSessions failed to start"));
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
		OnMultiplayerSessionsFindDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteHandle);
	}

	if (!LastSessionSearch.IsValid() || LastSessionSearch->SearchResults.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("LastSessionSearch->SearchResults.Num() <= 0"));
		OnMultiplayerSessionsFindDelegate.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
		FString::Printf(TEXT("Find Sessions Num: %d"), LastSessionSearch->SearchResults.Num()));

	OnMultiplayerSessionsFindDelegate.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}
#pragma endregion

#pragma region Join Session
void UMultiplayerSessionsSubsystem::MultiplayerSessionsJoin(const FSessionsSearchResult& SearchResult)
{
	if (!SessionInterface.IsValid())
	{
		OnMultiplayerSessionJoinDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	JoinSessionCompleteHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
		FString::Printf(TEXT("Join OwningUserName: %s"), *SearchResult.SessionResult.Session.OwningUserName));

	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult.SessionResult))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			FString::Printf(TEXT("SessionInterface->JoinSession Faild")));
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);
		OnMultiplayerSessionJoinDelegate.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteHandle);

		FString ConnectString;
		if (SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectString))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
				FString::Printf(TEXT("Join to: %s"), *ConnectString));
			if (auto PC = GetWorld()->GetFirstPlayerController())
			{
				PC->ClientTravel(ConnectString, TRAVEL_Absolute);
			}
		}
	}

	OnMultiplayerSessionJoinDelegate.Broadcast(Result);
}
#pragma endregion

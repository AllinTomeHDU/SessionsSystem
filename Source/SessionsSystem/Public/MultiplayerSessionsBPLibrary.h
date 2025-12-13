// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsBPLibrary.generated.h"


UENUM(BlueprintType)
enum class ESessionsDifficultyLevel : uint8
{
	None,
	EASY,
	MIDDLE,
	DIFFICULT
};

USTRUCT(BlueprintType)
struct SESSIONSSYSTEM_API FSessionsCreateParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RoomName = TEXT("XXX's Game");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESessionsDifficultyLevel DifficultyLevel = ESessionsDifficultyLevel::EASY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "5", UIMin = "1", UIMax = "5"))
	int32 MaxPlayers = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEntertainmentMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPublic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Password;
};

USTRUCT(BlueprintType)
struct SESSIONSSYSTEM_API FSessionsSearchParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RoomName = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESessionsDifficultyLevel DifficultyLevel = ESessionsDifficultyLevel::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEntertainmentMode = false;
};

USTRUCT(BlueprintType)
struct SESSIONSSYSTEM_API FSessionsSearchResult
{
	GENERATED_BODY()

	FOnlineSessionSearchResult SessionResult;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString RoomName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentPlayers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MaxPlayers;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESessionsDifficultyLevel DifficultyLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsEntertainmentMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsPublic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Password;
};

UENUM(BlueprintType)
enum class ESessionsJoinResult : uint8
{
	Success,
	SessionIsFull,
	SessionDoesNotExist,
	CouldNotRetrieveAddress,
	AlreadyInSession,
	UnknownError
};


/**
 * 
 */
UCLASS()
class SESSIONSSYSTEM_API UMultiplayerSessionsBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static bool MultiplayerServerTravel(UObject* WorldContextObject, const FString& Address, bool bAbsolute = false, bool bShouldSkipGameNotify = false);

};

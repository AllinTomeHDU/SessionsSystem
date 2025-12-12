// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsBPLibrary.generated.h"


UENUM(BlueprintType)
enum class ESSDifficultyLevel : uint8
{
	None,
	EASY,
	MIDDLE,
	DIFFICULT
};

USTRUCT(BlueprintType)
struct SESSIONSSYSTEM_API FSSCreateParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RoomName = TEXT("XXX's Game");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESSDifficultyLevel DifficultyLevel = ESSDifficultyLevel::EASY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1", ClampMax = "5", UIMin = "0", UIMax = "5"))
	int32 MaxPlayers = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEntertainmentMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPublic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Password;
};

USTRUCT(BlueprintType)
struct SESSIONSSYSTEM_API FSSSearchParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RoomName = TEXT("");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESSDifficultyLevel DifficultyLevel = ESSDifficultyLevel::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEntertainmentMode = false;
};

USTRUCT(BlueprintType)
struct SESSIONSSYSTEM_API FSSSearchResult
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
	ESSDifficultyLevel DifficultyLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsEntertainmentMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsPublic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString Password;

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

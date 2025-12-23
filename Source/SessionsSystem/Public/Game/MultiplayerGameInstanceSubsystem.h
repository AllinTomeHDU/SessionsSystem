// Copyright: Jichao Luo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameFramework/Character.h"
#include "Blueprint/UserWidget.h"
#include "MultiplayerGameInstanceSubsystem.generated.h"

class UUserWidget;

/**
 * 处理一些跨关卡逻辑、数据
 */
UCLASS()
class SESSIONSSYSTEM_API UMultiplayerGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	UUserWidget* ShowTransitionWidget(
		TSubclassOf<UUserWidget> InWidgetClass, 
		float InMinDisplayTime = 5.f,
		float InMaxDisplayTime = 10.f,
		bool bAutoClose = true,
		int32 InZOrder = 10
	);

	UFUNCTION(BlueprintCallable)
	void RequestCloseTransitionWidget();

protected:
	void OnMinDisplayTimeReached();
	void OnMaxDisplayTimeReached();
	void PerformCloseTransitionWidget();

private:
	UPROPERTY()
	UUserWidget* TransitionWidget;

	FTimerHandle MinDisplayTimerHandle;
	FTimerHandle MaxDisplayTimerHandle;
	bool bCanBeClosed = false;      // 达到最短显示时间
	bool bPendingClose = false;     // 是否已经收到手动关闭请求
	bool bAutoCloseEnabled = false; // 是否启用最长时间自动关闭

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACharacter> SelectedChrClass;

public:
	FORCEINLINE void SetSelectedChrClass(TSubclassOf<ACharacter> InClass) { SelectedChrClass = InClass; }
	FORCEINLINE UClass* GetSelectedChrClass() const { return SelectedChrClass; }
};

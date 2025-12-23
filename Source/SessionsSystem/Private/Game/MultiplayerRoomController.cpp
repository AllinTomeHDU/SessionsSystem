// Copyright: Jichao Luo


#include "Game/MultiplayerRoomController.h"
#include "Game/MultiplayerGameInstanceSubsystem.h"
#include "Interface/CharacterSelectInterface.h"
#include "Client/Core/ClientWorkType.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "Kismet/KismetSystemLibrary.h"


AMultiplayerRoomController::AMultiplayerRoomController()
{
	bReplicates = true;
}

void AMultiplayerRoomController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (auto EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComp->BindAction(IA_Click, ETriggerEvent::Started, this, &ThisClass::OnClickAction);
	}
}

void AMultiplayerRoomController::BeginPlay()
{
	Super::BeginPlay();

	if (auto GameSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerGameInstanceSubsystem>())
	{
		if (IsLocalController())
		{
			GameSubsystem->RequestCloseTransitionWidget();
		}
	}

	if (auto EnhancedSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		EnhancedSubsystem->AddMappingContext(IMC_Default, 0);
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AMultiplayerRoomController::OnClickAction()
{
	FVector WorldLocation, WorldDirection;
	float MouseX, MouseY;
	if (GetMousePosition(MouseX, MouseY))
	{
		DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

		FHitResult HitResult;
		UKismetSystemLibrary::LineTraceSingle(
			GetWorld(),
			WorldLocation,
			WorldLocation + (WorldDirection * 10000.0f),
			UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false,
			{},
			EDrawDebugTrace::ForDuration,
			HitResult,
			true
		);
		if (HitResult.IsValidBlockingHit())
		{
			if (auto Chr = Cast<ACharacter>(HitResult.GetActor()))
			{
				if (Chr->GetClass()->ImplementsInterface(UCharacterSelectInterface::StaticClass()))
				{
					bool bIsSelected = ICharacterSelectInterface::Execute_IsSelected(Chr);
					if (!bIsSelected)
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, 
							FString::Printf(TEXT("Selecte Character : %s"), *HitResult.GetActor()->GetName()));
						
						Server_SelectCharacter(Chr, true);
						if (IsValid(SelectedCharacter))
						{
							Server_SelectCharacter(SelectedCharacter, false);
						}
						SelectedCharacter = Chr;

						if (auto GameSubsystem = GetGameInstance()->GetSubsystem<UMultiplayerGameInstanceSubsystem>())
						{
							GameSubsystem->SetSelectedChrClass(Chr->GetClass());
						}
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString(TEXT("Character is selected")));
					}
				}
			}
		}
	}
}

void AMultiplayerRoomController::Server_SelectCharacter_Implementation(ACharacter* TargetCharacter, bool bIsSelected)
{
	if (TargetCharacter && TargetCharacter->GetClass()->ImplementsInterface(UCharacterSelectInterface::StaticClass()))
	{
		ICharacterSelectInterface::Execute_SetSelected(TargetCharacter, bIsSelected);
	}
}

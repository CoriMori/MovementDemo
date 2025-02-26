// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ClimbAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerBase.h"

UClimbAbility::UClimbAbility()
{

}

void UClimbAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AttachToWall();
	Climb();
}


void UClimbAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	GetAvatarCharacter()->GetCharacterMovement()->MaxFlySpeed = ClimbSpeed;
	GetAvatarCharacter()->GetCharacterMovement()->BrakingDecelerationFlying = ClimbBreak;
}

void UClimbAbility::Climb()
{
	APlayerBase* Player = Cast<APlayerBase>(GetAvatarCharacter());
	FHitResult TraceResult;
	bool bHitDetected = ClimbTrace(TraceResult, ClimbTraceDistance + 20);
	if (!bHitDetected) return;

	//FVector2D ClimbDirection = Player->GetMovementVector();
	//float ScaleValue = 1.0f;
	//Player->AddMovementInput(ClimbDirection, ScaleValue);

}

void UClimbAbility::AttachToWall()
{
	FHitResult TraceResult;
	bool bHitDetected = ClimbTrace(TraceResult, ClimbTraceDistance);
	if (!bHitDetected) return;
	GetAvatarCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	GetAvatarCharacter()->GetCharacterMovement()->bOrientRotationToMovement = false;

	FVector AttachmentLocation = (GetAvatarCharacter()->GetCapsuleComponent()->GetScaledCapsuleRadius() * TraceResult.Normal) + TraceResult.Location;
	FRotator AttachmentRotation = FRotationMatrix::MakeFromX(TraceResult.Normal * -1.0f).Rotator();
	FLatentActionInfo LatentInfo;

	UKismetSystemLibrary::MoveComponentTo(GetAvatarCharacter()->GetRootComponent(), AttachmentLocation, AttachmentRotation, false, false, 0.2f, false, EMoveComponentAction::Move, LatentInfo);
}

bool UClimbAbility::ClimbTrace(FHitResult& OutResult, float TraceDistance)
{
	const UWorld* World = GetWorld();
	FVector StartLocation = GetAvatarCharacter()->GetActorLocation();
	FVector EndLocation = StartLocation + (GetAvatarCharacter()->GetActorForwardVector() * TraceDistance);
	return World->LineTraceSingleByChannel(OutResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility);
}


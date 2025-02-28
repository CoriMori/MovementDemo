// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ClimbAbility.h"
#include "PlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerBase.h"
#include "AbilitySystemComponent.h"

UClimbAbility::UClimbAbility()
{
	EGameplayAbilityReplicationPolicy::ReplicateYes; //allows RPCs to fire on this ability
}

void UClimbAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	Climb();
}

void UClimbAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	EndClimb();
}

void UClimbAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	EndClimb();
}


void UClimbAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	GetAvatarCharacter()->GetCharacterMovement()->MaxFlySpeed = ClimbSpeed;
	GetAvatarCharacter()->GetCharacterMovement()->BrakingDecelerationFlying = ClimbBreak;

	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("SmoothClimbRotation"));

	ClimbRotationTimeline.AddInterpFloat(ClimbRoationCurve, ProgressUpdate);
	ClimbRotationTimeline.SetLooping(true);

	float MinTimeRange;
	float MaxTimeRange;
	ClimbRoationCurve->GetTimeRange(MinTimeRange, MaxTimeRange);
}

void UClimbAbility::Climb()
{
	AttachToWall();
	FHitResult TraceResult;
	bool bHitDetected = ClimbTrace(TraceResult, ClimbTraceDistance + 20);
	if (!bHitDetected) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	//add timeline to smooth out alignment
	UAbilityTaskOnTick* Task = UAbilityTaskOnTick::AbilityTaskOnTick(this, NAME_None);
	Task->OnTick.AddDynamic(this, &UClimbAbility::OnTick);

	Task->ReadyForActivation();
	//handle climbing rotation
	ClimbRotationTimeline.PlayFromStart();

	//ability will be canceled via another ability so no need to end
}

void UClimbAbility::AttachToWall()
{
	FHitResult TraceResult;
	bool bHitDetected = ClimbTrace(TraceResult, ClimbTraceDistance);
	if (!bHitDetected) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	GetAvatarCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_Climb);
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
	//DrawDebugLine(World, StartLocation, EndLocation, FColor::Red, false, 0.0f);
	return World->LineTraceSingleByChannel(OutResult, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1);
}

void UClimbAbility::OnTick(float DeltaTime)
{
	AttachToWall();
	ClimbRotationTimeline.TickTimeline(DeltaTime);
}

void UClimbAbility::SmoothClimbRotation(float Alpha)
{
	FHitResult TraceResult;
	bool bHitDetected = ClimbTrace(TraceResult, ClimbTraceDistance + 50);
	if (!bHitDetected) return;
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("Rotating"));

	FRotator TargetRotation = FRotationMatrix::MakeFromX(TraceResult.Normal * -1.0f).Rotator();
	FRotator CurrentRotation = GetAvatarCharacter()->GetActorRotation();

	FRotator NewClimbRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, Alpha, 0.1f);
	GetAvatarCharacter()->SetActorRotation(NewClimbRotation);
}

//called when ability is cancelled
void UClimbAbility::EndClimb()
{
	GetAvatarCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetAvatarCharacter()->GetCharacterMovement()->bOrientRotationToMovement = true;
}


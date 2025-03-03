// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ClimbAbility.h"
#include "PlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerBase.h"
#include "AbilitySystemComponent.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
	DetectLedge();
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

void UClimbAbility::MantleLedge(FHitResult LedgeTraceResult)
{
	const UWorld* World = GetWorld();
	APlayerBase* OwningPlayer = Cast<APlayerBase>(GetAvatarCharacter());
	if (!OwningPlayer) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	bClimbingLedge = true;
	
	FVector StartLocation = GetAvatarCharacter()->GetActorLocation();
	StartLocation.X = StartLocation.X + (GetAvatarCharacter()->GetActorForwardVector().X * 48.0f);
	StartLocation.Y = StartLocation.Y + (GetAvatarCharacter()->GetActorForwardVector().Y * 48.0f);
	StartLocation.Z = LedgeTraceResult.Location.Z;
	OwningPlayer->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation("LedgeClimbStart", StartLocation, OwningPlayer->GetActorRotation());

	FVector EndLocation = LedgeTraceResult.Location;
	EndLocation.Z = GetAvatarCharacter()->GetActorLocation().Z + 115.0f;
	OwningPlayer->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation("LedgeClimbEnd", EndLocation, OwningPlayer->GetActorRotation());

	GetAvatarCharacter()->SetActorEnableCollision(false);
	OwningPlayer->GetCameraBoom()->bDoCollisionTest = false;

	UAnimInstance* AnimInstance = OwningPlayer->GetMesh()->GetAnimInstance();
	AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UClimbAbility::OnMontageNotify);

	UPlayMontageAndWaitForEvent* MontageTask = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, LedgeClimbAnimation, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	MontageTask->ReadyForActivation();
}

void UClimbAbility::DetectLedge()
{
	const UWorld* World = GetWorld();
	
	//forward edge detection
	FHitResult TraceResult;
	FVector StartLocation = GetAvatarCharacter()->GetActorLocation();
	StartLocation.Z = StartLocation.Z + EdgeTraceHeight;
	FVector EndLocation = (GetAvatarCharacter()->GetActorForwardVector() * 100.0f) + StartLocation;
	
	//DrawDebugLine(World, StartLocation, EndLocation, FColor::Red, false, 0.0f);
	bool bHitDetected = World->LineTraceSingleByChannel(TraceResult, StartLocation, EndLocation, ECollisionChannel::ECC_GameTraceChannel1);
	if (bHitDetected) return;

	//top down edge detection
	bool bLoopHitDetected = false;
	FHitResult LoopTraceResult;
	for (int32 I = 0; I < 20; I++)
	{
		int32 LineTraceSpacing = I * 3;
		FVector LoopStartLocation = (GetAvatarCharacter()->GetActorForwardVector() * LineTraceSpacing) + (TraceResult.TraceStart + FVector(0.0f, 0.0f, 90.0f));
		FVector LoopEndLocation = LoopStartLocation - FVector(0.0f, 0.0f, 100.0f);
		//DrawDebugLine(World, LoopStartLocation, LoopEndLocation, FColor::Blue, false, 0.0f);
		bLoopHitDetected = World->LineTraceSingleByChannel(LoopTraceResult, LoopStartLocation, LoopEndLocation, ECollisionChannel::ECC_GameTraceChannel1);
		if (bLoopHitDetected) break;
	}

	if (!bLoopHitDetected || bClimbingLedge) return;
	MantleLedge(LoopTraceResult);
}

void UClimbAbility::DetectEdges()
{
	const UWorld* World = GetWorld();
	APlayerBase* OwningPlayer = Cast<APlayerBase>(GetAvatarCharacter());

	//right edge detection
	FHitResult TraceResultRight;
	FVector StartLocationRight = GetAvatarCharacter()->GetActorLocation() + (GetAvatarCharacter()->GetActorRightVector() * EdgeTrace);
	FVector EndLocationRight = (GetAvatarCharacter()->GetActorForwardVector() * 100.0f) + StartLocationRight;

	//DrawDebugLine(World, StartLocationRight, EndLocationRight, FColor::Blue, false, 0.0f);
	bool bHitDetectedRight = World->LineTraceSingleByChannel(TraceResultRight, StartLocationRight, EndLocationRight, ECollisionChannel::ECC_GameTraceChannel1);
	OwningPlayer->GetPlayerMovement()->SetRightEdgeDetected(!bHitDetectedRight);
	if (!bHitDetectedRight) return;

	//left edge detection
	FVector StartLocationLeft = GetAvatarCharacter()->GetActorLocation() + (GetAvatarCharacter()->GetActorRightVector() * -EdgeTrace);
	FVector EndLocationLeft = (GetAvatarCharacter()->GetActorForwardVector() * 100.0f) + StartLocationLeft;

	//DrawDebugLine(World, StartLocationLeft, EndLocationLeft, FColor::Green, false, 0.0f);
	bool bHitDetectedLeft = World->LineTraceSingleByChannel(TraceResultRight, StartLocationLeft, EndLocationLeft, ECollisionChannel::ECC_GameTraceChannel1);
	OwningPlayer->GetPlayerMovement()->SetLeftEdgeDetected(!bHitDetectedLeft);
}

void UClimbAbility::AttachToWall()
{
	FHitResult TraceResult;
	bool bHitDetected = ClimbTrace(TraceResult, ClimbTraceDistance);
	if (!bHitDetected) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	if (!bClimbingLedge) {
		GetAvatarCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_Climb);
		GetAvatarCharacter()->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else {
		GetAvatarCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::MOVE_LedgeClimb);
		GetAvatarCharacter()->GetCharacterMovement()->bOrientRotationToMovement = false;
	}

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
	if (bClimbingLedge) return;
	DetectEdges();
	DetectLedge();
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
	bClimbingLedge = false;
}

void UClimbAbility::OnMontageNotify(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	APlayerBase* OwningPlayer = Cast<APlayerBase>(GetAvatarCharacter());
	GetAvatarCharacter()->SetActorEnableCollision(true);
	OwningPlayer->GetCameraBoom()->bDoCollisionTest = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


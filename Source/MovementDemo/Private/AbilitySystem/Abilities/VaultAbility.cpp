// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/VaultAbility.h"
#include "Player/PlayerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MotionWarpingComponent.h"

UVaultAbility::UVaultAbility()
{
}

void UVaultAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Vault();
}

void UVaultAbility::Vault()
{
	APlayerBase* Player = Cast<APlayerBase>(GetAvatarCharacter());
	TArray<FVector> VaultingPath = TraceVaultPath();

	//check if the landing location is within vaulting range
	float LandingMin = Player->GetMesh()->GetComponentToWorld().GetLocation().Z - 50.0f;
	float LandingMax = Player->GetMesh()->GetComponentToWorld().GetLocation().Z + 50.0f;
	bool bCanLand = FMath::IsWithinInclusive(LandingLocation.Z, LandingMin, LandingMax);

	if (!bCanWarp || !bCanLand || !VaultAnimation) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	Player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	Player->SetActorEnableCollision(false);

	//set up the warping locations
	FName TargetName = "VaultStart";
	for (int32 I = 0; I < VaultingPath.Num(); I++) {
		if (I < VaultingPath.Num() - 1) TargetName = "VaultMiddle";
		else TargetName = "VaultEnd";
		Player->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(TargetName, VaultingPath[I], Player->GetActorRotation());
	}
	//play the animation montage
	UPlayMontageAndWaitForEvent* MontageTask = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, VaultAnimation, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	MontageTask->OnCompleted.AddDynamic(this, &UVaultAbility::OnCompleted);

	MontageTask->ReadyForActivation();
}

void UVaultAbility::HandleVaultEnd()
{
	APlayerBase* Player = Cast<APlayerBase>(GetAvatarCharacter());
	Player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	Player->SetActorEnableCollision(true);
	bCanWarp = false;
	LandingLocation = FVector(0.0f, 0.0f, 20000.0f);
}

void UVaultAbility::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	HandleVaultEnd();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UVaultAbility::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	HandleVaultEnd();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

TArray<FVector> UVaultAbility::TraceVaultPath()
{
	TArray<FVector> VaultingPath;
	const UWorld* World = GetWorld();
	FHitResult ForwardTraceResult;
	//Check for a vaultable object in front of us
	for (int32 I = 0; I <= 2; I++) {
		//calculate actor offset for each trace position
		int32 ActorZOffset = I * VaultTraceForwardSpacing;

		//set the start location
		FVector StartLocation = GetAvatarCharacter()->GetActorLocation();
		StartLocation.Z += ActorZOffset;

		//set the end location
		FVector EndLocation = GetAvatarCharacter()->GetActorForwardVector() * VaultDetectionDistance + StartLocation;

		//trace for objects in front of the player
		FCollisionShape SphereTrace = FCollisionShape::MakeSphere(5.0f);
		bool bHitDetected = World->SweepSingleByChannel(ForwardTraceResult, StartLocation, EndLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, SphereTrace);

		if (bHitDetected) break; // break if we hit something
	}

	//if we didn't hit anything don't bother checking anything else
	if (!ForwardTraceResult.bBlockingHit) return VaultingPath;

	//Calculate the vaulting path
	for (int32 I = 0; I <= 5; I++) {
		int32 ActorOffset = I * VaultTraceDownwardSpacing;

		FVector StartLocation = (ForwardTraceResult.Location + FVector(0.0f, 0.0f, 100.0f)) + (GetAvatarCharacter()->GetActorForwardVector() * ActorOffset);
		FVector EndLocation = StartLocation - FVector(0.0f, 0.0f, 100.0f);

		FHitResult DownwardTraceResult;
		FCollisionShape SphereTrace = FCollisionShape::MakeSphere(10.0f);
		bool bHitDetected = World->SweepSingleByChannel(DownwardTraceResult, StartLocation, EndLocation, FQuat::Identity, ECC_Visibility, SphereTrace);

		if (bHitDetected) {
			if (I == 0) {
				VaultingPath.Push(DownwardTraceResult.Location); //add the vault starting point
				//DrawDebugSphere(GetWorld(), DownwardTraceResult.Location, 10.0f, 16, FColor::Purple, false, 10.0f);
			}
			else {
				VaultingPath.Push(DownwardTraceResult.Location); //add the vault middle points
				bCanWarp = true; //let the system know that we are cleard to attempt the warp ->can be buggy without this
			}

		}
		else { //calculate the landing location
			FHitResult LandingTraceResult;

			FVector LandingStartLocation = DownwardTraceResult.TraceStart + (GetAvatarCharacter()->GetActorForwardVector() * 80); //80 is a hardcoded landing distance -> will convert to variable
			FVector LandingEndLocation = LandingStartLocation - FVector(0.0f, 0.0f, 1000.0f);
			bool bEndDetected = World->LineTraceSingleByChannel(LandingTraceResult, LandingStartLocation, LandingEndLocation, ECC_Visibility);

			if (bEndDetected) {
				LandingLocation = LandingTraceResult.Location;
				VaultingPath.Push(LandingTraceResult.Location); //add the vault landing point
				break;
			}
		}

	}
	return VaultingPath;
}

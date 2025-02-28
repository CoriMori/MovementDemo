// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/VaultAbility.h"
#include "PlayerMovementComponent.h"
#include "MotionWarpingComponent.h"

UVaultAbility::UVaultAbility()
{
	EGameplayAbilityReplicationPolicy::ReplicateYes; //allows RPCs to fire on this ability
}

void UVaultAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Vault(ActorInfo);
}

void UVaultAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	if (GetAvatarCharacter()) {
		OwningPlayer = Cast<APlayerBase>(GetAvatarCharacter());
	}
}

void UVaultAbility::Vault(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (!OwningPlayer) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	TArray<FVector> VaultingPath = TraceVaultPath();

	//check if the landing location is within vaulting range
	float LandingMin = GetAvatarCharacter()->GetMesh()->GetComponentToWorld().GetLocation().Z - 50.0f;
	float LandingMax = GetAvatarCharacter()->GetMesh()->GetComponentToWorld().GetLocation().Z + 50.0f;
	bool bCanLand = FMath::IsWithinInclusive(LandingLocation.Z, LandingMin, LandingMax);

	if (!bCanWarp || !bCanLand || !VaultAnimation) {
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	//update player movement mode and collision->called on both client and server
	OwningPlayer->GetPlayerMovement()->SetIsWarping(true);
	GetAvatarCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	GetAvatarCharacter()->SetActorEnableCollision(false);

	//set up the warping locations
	FName TargetName = "VaultStart";
	for (int32 I = 0; I < VaultingPath.Num(); I++) {
		if (I < VaultingPath.Num() - 1) TargetName = "VaultMiddle";
		else TargetName = "VaultEnd";
		OwningPlayer->GetMotionWarpingComponent()->AddOrUpdateWarpTargetFromLocationAndRotation(TargetName, VaultingPath[I], OwningPlayer->GetActorRotation());
	}

	//play the animation montage -> only called on owning client / server
	UPlayMontageAndWaitForEvent* MontageTask = UPlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, VaultAnimation, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	MontageTask->OnCompleted.AddDynamic(this, &UVaultAbility::OnCompleted);

	MontageTask->ReadyForActivation();
}

/*
* Handle Vault End only gets called on the owning client/server due to the task event.
* So when a client activates the ability, this function only runs for the client.
*/

void UVaultAbility::HandleVaultEnd()
{
	OwningPlayer->GetPlayerMovement()->SetIsWarping(false);
	GetAvatarCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetAvatarCharacter()->SetActorEnableCollision(true);

	if (!GetAvatarCharacter()->HasAuthority()) UpdateServer();

	bCanWarp = false;
	LandingLocation = FVector(0.0f, 0.0f, 20000.0f);
}

void UVaultAbility::UpdateServer_Implementation()
{
	OwningPlayer->GetPlayerMovement()->SetIsWarping(false);
	GetAvatarCharacter()->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetAvatarCharacter()->SetActorEnableCollision(true);
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
		bool bHitDetected = World->SweepSingleByChannel(DownwardTraceResult, StartLocation, EndLocation, FQuat::Identity, ECollisionChannel::ECC_Visibility, SphereTrace);

		//prevent vaulting through stacked objects
		if (DownwardTraceResult.bStartPenetrating) {
			bCanWarp = false;
			LandingLocation = FVector(0.0f, 0.0f, 20000.0f);
			return VaultingPath;
		}
		
		if (bHitDetected) {
			if (I == 0) {
				VaultingPath.Push(DownwardTraceResult.Location); //add the vault starting point
				//DrawDebugSphere(GetWorld(), DownwardTraceResult.Location, 10.0f, 16, FColor::Purple, false, 10.0f);
			}
			else {
				VaultingPath.Push(DownwardTraceResult.Location); //add the vault middle points
				bCanWarp = true; //let the system know that we are cleard to attempt the warp ->can be buggy without this
				//DrawDebugSphere(GetWorld(), DownwardTraceResult.Location, 10.0f, 16, FColor::Yellow, false, 10.0f);
			}

		}
		else { //calculate the landing location
			FHitResult LandingTraceResult;

			FVector LandingStartLocation = DownwardTraceResult.TraceStart + (GetAvatarCharacter()->GetActorForwardVector() * 80); //80 is a hardcoded landing distance -> will convert to variable
			FVector LandingEndLocation = LandingStartLocation - FVector(0.0f, 0.0f, 1000.0f);
			bool bEndDetected = World->LineTraceSingleByChannel(LandingTraceResult, LandingStartLocation, LandingEndLocation, ECollisionChannel::ECC_Visibility);

			if (bEndDetected) {
				LandingLocation = LandingTraceResult.Location;
				VaultingPath.Push(LandingTraceResult.Location); //add the vault landing point
				//DrawDebugSphere(GetWorld(), LandingTraceResult.Location, 10.0f, 16, FColor::Blue, false, 10.0f);
				break;
			}
		}

	}
	return VaultingPath;
}

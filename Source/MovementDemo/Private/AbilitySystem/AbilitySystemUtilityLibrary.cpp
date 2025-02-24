// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemUtilityLibrary.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"

FTransform UAbilitySystemUtilityLibrary::CalculateTransformFromGroundTarget(UCameraComponent* Camera, float MaxRange, bool bDrawDebugLines)
{
	const UWorld* World = Camera->GetWorld();
	if (World) {
		const FRotator SpawnRotation = Camera->GetComponentRotation();
		const FVector CameraLocation = Camera->GetComponentLocation();
		const FVector CameraForward = Camera->GetForwardVector();

		const FVector TraceEndLocation = CameraLocation + (CameraForward * MaxRange);

		if (bDrawDebugLines) {
			DrawDebugLine(World, Camera->GetOwner()->GetActorLocation(), TraceEndLocation, FColor::Red, true);
		}

		FHitResult HitResult;
		World->LineTraceSingleByChannel(HitResult, Camera->GetOwner()->GetActorLocation(), TraceEndLocation, ECollisionChannel::ECC_Visibility);
		return FTransform(SpawnRotation, HitResult.Location, FVector(1.0f, 1.0f, 1.0f));
	}
	return FTransform();
}

FTransform UAbilitySystemUtilityLibrary::CalculateTransformFromPlayer(UCameraComponent* Camera, ACharacter* Actor, FName SocketName, bool bDrawDebugLines)
{
	const UWorld* World = Camera->GetOwner()->GetWorld();
	if (World) {
		const FRotator SpawnRotation = Camera->GetComponentRotation();
		const FVector SpawnLocation = Actor->GetMesh()->GetSocketLocation(SocketName);

		if (bDrawDebugLines) {
			DrawDebugLine(World, SpawnLocation, SpawnLocation + Camera->GetForwardVector() * 100, FColor::Red, true);
		}

		return FTransform(SpawnRotation, SpawnLocation, FVector(1.0f, 1.0f, 1.0f));
	}
	FString DebugMessage = !Actor ? "Actor is invalid" : !World ? "World is invalid" : "Unknown";
	DisplayDebugMessage("An Error Occured in Calcuate Transform From Player: " + DebugMessage);
	return FTransform();
}

void UAbilitySystemUtilityLibrary::DisplayDebugMessage(FString Message, float DisplayDuration, FColor MessageColor)
{
	GEngine->AddOnScreenDebugMessage(-1, DisplayDuration, MessageColor, Message);
}

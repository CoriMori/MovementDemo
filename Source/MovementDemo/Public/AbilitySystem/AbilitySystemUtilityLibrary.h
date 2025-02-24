// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemUtilityLibrary.generated.h"


UCLASS(BlueprintType, Blueprintable)
class MOVEMENTDEMO_API UAbilitySystemUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//calculates the spawn transform for an actor that would spawn on the ground
	UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Ability")
	static FTransform CalculateTransformFromGroundTarget(UCameraComponent* Camera, float MaxRange, bool bDrawDebugLines = false);

	//calculates the spawn transform for an actor that would spawn from a specific socket on the ability avatar
	UFUNCTION(BlueprintCallable, Category = "Custom Gameplay Ability")
	static FTransform CalculateTransformFromPlayer(UCameraComponent* Camera, ACharacter* Actor, FName SocketName, bool bDrawDebugLines = false);

private:
	static void DisplayDebugMessage(FString Message, float DisplayDuration = 5.0f, FColor MessageColor = FColor::Magenta);
};

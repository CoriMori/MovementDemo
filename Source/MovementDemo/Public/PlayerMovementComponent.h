// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerMovementComponent.generated.h"

/**
 * Custom player component to handle custom movement modes
 */
DECLARE_LOG_CATEGORY_EXTERN(LogCharacterMovement, Log, All);

UCLASS()
class MOVEMENTDEMO_API UPlayerMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:
	UPlayerMovementComponent();
	
	//animation functions
	UFUNCTION(BlueprintPure)
	float GetSpeed() const { return Velocity.Length(); }

	UFUNCTION(BlueprintPure)
	FVector GetVelocity() const { return Velocity; }
	
	UFUNCTION(BlueprintPure)
	float GetLeanDirection(float DeltaTime, float YawDelta, float LeanAmount = 30.0f, float LeanSpeed = 5.0f);
	
	UFUNCTION(BlueprintPure)
	FRotator GetLastFrameRotation() const { return LastFrameRotation; }
	
	UFUNCTION(BlueprintPure)
	bool GetIsClimbing() const { return bIsClimbing; }

	UFUNCTION(BlueprintPure)
	bool GetIsWarping() const { return bIsWarping; }

	void SetIsWarping(bool bWarping) { bIsWarping = bWarping; }

	bool GetRightEdgeDetected() const { return bRightEdgeDetected; }
	bool GetLeftEdgeDetected() const { return bLeftEdgeDetected; }

	void SetRightEdgeDetected(bool bEdgeDetected) { bRightEdgeDetected = bEdgeDetected; }
	void SetLeftEdgeDetected(bool bEdgeDetected) { bLeftEdgeDetected = bEdgeDetected; }

	UFUNCTION(BlueprintPure)
	FQuat GetLookRotation(APawn* OwningPawn, ACharacter* Character);

protected:
	virtual void InitializeComponent() override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual void SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode = 0) override;
	float GetCustomMaxBrakingDeceleration() const;
private:
	UPROPERTY(Transient)
	TObjectPtr<AActor> ComponentOwner;

	FRotator LastFrameRotation; //used for body leaning
	FRotator SpineRotation; //used for head looking

	bool bIsClimbing = false; //tracks climbing animation state

	//edge detection for climbing system
	bool bRightEdgeDetected = false;
	bool bLeftEdgeDetected = false;

	bool bIsWarping = false; //tracks warping state for vaulting -> prevents foot iks from being used while motion warping
};

//custom movement modes to prevent overlapping issues
UENUM(BlueprintType)
enum ECustomMovementMode : uint8
{
	MOVE_Climb UMETA(DisplayName = "Climbing"),
	MOVE_LedgeClimb UMETA(DisplayName = "Ledge Climbing"),
};

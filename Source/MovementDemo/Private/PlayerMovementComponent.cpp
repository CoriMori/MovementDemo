// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PhysicsVolume.h"

DEFINE_LOG_CATEGORY(LogCharacterMovement);

UPlayerMovementComponent::UPlayerMovementComponent()
{
	bOrientRotationToMovement = true; // Character moves in the direction of input...	
	RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	JumpZVelocity = 700.f;
	AirControl = 0.35f;
	MaxWalkSpeed = 500.f;
	MinAnalogWalkSpeed = 20.f;
	BrakingDecelerationWalking = 2000.f;
	BrakingDecelerationFalling = 1500.0f;

	// Set up crouching values
	GetNavAgentPropertiesRef().bCanCrouch = true;
}

float UPlayerMovementComponent::GetLeanDirection(float DeltaTime, float YawDelta, float LeanAmount, float LeanSpeed)
{
	FRotator Delta = LastFrameRotation - GetOwner()->GetActorRotation();
	Delta.Normalize();
	float TargetLeanDirection = (Delta.Yaw / DeltaTime) / LeanAmount;
	float LeanInterp = FMath::FInterpTo(YawDelta, TargetLeanDirection, DeltaTime, LeanSpeed);
	LastFrameRotation = GetOwner()->GetActorRotation();
	return FMath::FInterpTo(YawDelta, TargetLeanDirection, DeltaTime, LeanSpeed);
}

FQuat UPlayerMovementComponent::GetLookRotation(APawn* OwningPawn, ACharacter* Character)
{
	if (!OwningPawn && !Character) return FQuat();
	FRotator Delta = OwningPawn->GetControlRotation() - Character->GetCapsuleComponent()->GetComponentRotation();
	Delta.Normalize();

	Delta.Pitch = Delta.Pitch / -5.0f;
	FMath::Clamp(Delta.Pitch, -20.0f, 20.0f);

	Delta.Yaw = Delta.Yaw / -5.0f;
	FMath::Clamp(Delta.Yaw, -20.0f, 20.0f);

	Delta.Roll = Delta.Roll / -5.0f;
	FMath::Clamp(Delta.Roll, -20.0f, 20.0f);

	FRotator AdjustedDelta = FRotator(Delta.Roll, Delta.Yaw, Delta.Pitch); //Pitch and Roll swapped on purpose for bone movement
	SpineRotation = FMath::RInterpTo(SpineRotation, AdjustedDelta, 1.0f, 0.1f);
	FRotator AdjustedRotation = FRotator(SpineRotation.Pitch, SpineRotation.Roll, SpineRotation.Yaw); //Roll and Yaw swapped on purpose for bone movement
	return AdjustedRotation.Quaternion();

	return FQuat();
}

void UPlayerMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	ComponentOwner = GetOwner();
}

void UPlayerMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);
	switch (CustomMovementMode)
	{
	case ECustomMovementMode::MOVE_Climb:
		bIsClimbing = true;
		MaxCustomMovementSpeed = MaxFlySpeed;
		PhysFlying(DeltaTime, Iterations);
		break;
	case ECustomMovementMode::MOVE_LedgeClimb:
		bIsClimbing = false;
		MaxCustomMovementSpeed = MaxFlySpeed;
		PhysFlying(DeltaTime, Iterations);
		break;
	default:
		bIsClimbing = false;
		UE_LOG(LogCharacterMovement, Error, TEXT("Invalid Movement Mode"));
		break;
	}
}

float UPlayerMovementComponent::GetMaxBrakingDeceleration() const
{
	Super::GetMaxBrakingDeceleration();
	switch (MovementMode)
	{
	case MOVE_Custom:
		return GetCustomMaxBrakingDeceleration();
	default:
		return 0.0f;
	}
}

void UPlayerMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	Super::SetMovementMode(NewMovementMode, NewCustomMode);
	bIsClimbing = false;
}

float UPlayerMovementComponent::GetCustomMaxBrakingDeceleration() const
{
	switch (CustomMovementMode)
	{
	case ECustomMovementMode::MOVE_LedgeClimb:
	case ECustomMovementMode::MOVE_Climb:
		return BrakingDecelerationFlying;
	default:
		return 0.0f;
	}
}

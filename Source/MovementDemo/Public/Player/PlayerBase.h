// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "GameplayEffectTypes.h"
#include "Components/TimelineComponent.h"
#include "PlayerBase.generated.h"

/**
 * Base class for client controlled characters that need to implement GAS
 * Ability system component lives on the Player State Base
 */
class UCurveFloat;
class UMotionWarpingComponent;
UCLASS()
class MOVEMENTDEMO_API APlayerBase : public ACharacterBase
{
	GENERATED_BODY()
public:
	explicit APlayerBase(const FObjectInitializer& ObjectInitializer);
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	UFUNCTION(BlueprintPure)
	FORCEINLINE class UPlayerMovementComponent* GetPlayerMovement() const { return PlayerMovementComponent; }
	
	class UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }

	UFUNCTION(BlueprintCallable)
	FVector2D GetMovementVector() const { return MovementVector; }

	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Toggle Crouching
	void ToggleCrouch();

	UFUNCTION()
	void TransitionCamera(float Alpha);

	//Called on Server
	virtual void PossessedBy(AController* NewController) override;

	//Called on Client
	virtual void OnRep_PlayerState() override;

	virtual void Tick(float DeltaTime) override;

	void HandleCustomMovement(FVector2D InputMovementVector);
private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	/** Motion Warping Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Advanced Movement|Vault", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Advanced Movement|Crouch", meta = (AllowPrivateAccess = "true"))
	float DefaultTargetArmLength = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Advanced Movement|Crouch", meta = (AllowPrivateAccess = "true"))
	float CrouchedTargetArmLength = 550.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Advanced Movement|Crouch", meta = (AllowPrivateAccess = "true"))
	float CameraTransitionDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Advanced Movement|Crouch", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> CrouchCameraCurve;

	FTimeline CrouchCameraTimeline;

	FVector2D MovementVector;

	TObjectPtr<UPlayerMovementComponent> PlayerMovementComponent;
};


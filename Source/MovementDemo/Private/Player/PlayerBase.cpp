// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerBase.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "AbilitySystem/AbilitySystemUtilityLibrary.h"
#include "MotionWarpingComponent.h"

APlayerBase::APlayerBase(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlayerMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true; //enable tick on this character -> required for timelines
	SetReplicates(true); //set the replication status of this class -> disable for single player games
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = DefaultTargetArmLength; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create the Motion Warping Component->Actor Component so no need to setup attachment
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
}

void APlayerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	PlayerMovementComponent = Cast<UPlayerMovementComponent>(Super::GetMovementComponent());
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//crouch camera timeline setup
	FOnTimelineFloat ProgressUpdate;
	ProgressUpdate.BindUFunction(this, FName("TransitionCamera"));

	CrouchCameraTimeline.AddInterpFloat(CrouchCameraCurve, ProgressUpdate);
}

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)){

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APlayerBase::ToggleCrouch);

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Missing Enhanced Input Component"));
	}
}

void APlayerBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	MovementVector = Value.Get<FVector2D>();

	//
	if (Controller != nullptr){
		switch (GetCharacterMovement()->MovementMode)
		{
			case EMovementMode::MOVE_Custom:
				HandleCustomMovement(MovementVector);
				break;

			default:
				//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Purple, TEXT("Default Movement"));
				// find out which way is forward
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);

				// get forward vector
				const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

				// get right vector 
				const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

				// add movement 
				AddMovementInput(ForwardDirection, MovementVector.Y);
				AddMovementInput(RightDirection, MovementVector.X);
				break;
		}
	}
}

void APlayerBase::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr){
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerBase::ToggleCrouch()
{
	CrouchCameraTimeline.PlayFromStart();
	if (bIsCrouched) {
		UnCrouch();
		return;
	}

	Crouch();
}

void APlayerBase::TransitionCamera(float Alpha)
{
	float CurrentArmLength = CameraBoom->TargetArmLength;
	float TargetLength = DefaultTargetArmLength;
	if (bIsCrouched) {
		TargetLength = CrouchedTargetArmLength;
	}
	float Speed = (CurrentArmLength - TargetLength) * CameraTransitionDuration;
	
	CameraBoom->TargetArmLength = FMath::FInterpConstantTo(CurrentArmLength, TargetLength, Alpha, Speed);
}

void APlayerBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPlayerState());

	UAbilitySystemComponentBase* ASC = Cast<UAbilitySystemComponentBase>(AbilitySystemComponent);
	if (ASC) {
		//set up reference and init data -> server
		ASC->InitializeAbilitySystemData(AbilitySystemInitData, this, this);
	}
}

void APlayerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//set ASC pointer using AS global
	AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetPlayerState());
	
	UAbilitySystemComponentBase* ASC = Cast<UAbilitySystemComponentBase>(AbilitySystemComponent);
	if (ASC) {
		//set up reference and init data -> client
		ASC->InitializeAbilitySystemData(AbilitySystemInitData, this, this);
	}
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CrouchCameraTimeline.TickTimeline(DeltaTime);
}

void APlayerBase::HandleCustomMovement(FVector2D InputMovementVector)
{
	switch (GetCharacterMovement()->CustomMovementMode)
	{
		case ECustomMovementMode::MOVE_Climb:
			if (GetPlayerMovement()->GetRightEdgeDetected() && InputMovementVector.X >= 1.0f || GetPlayerMovement()->GetLeftEdgeDetected() && InputMovementVector.X <= -1.0f) {
				InputMovementVector.X = 0.0f;
			}
			AddMovementInput(GetActorRightVector(), InputMovementVector.X);
			AddMovementInput(GetActorUpVector(), InputMovementVector.Y);
			break;
		default:
			break;
	}
}




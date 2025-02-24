// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUDBase.h"
#include "UI/WidgetBase.h"
#include "Player/PlayerBase.h"

void AHUDBase::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetClass) {
		Widget = CreateWidget<UWidgetBase>(GetWorld(), WidgetClass);

		if (Widget) {
			InitWidget();
		}
	}
}

void AHUDBase::DrawHUD()
{
	Super::DrawHUD();
}

//Initialize the widget
void AHUDBase::InitWidget()
{
	//set the widget owner
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	Widget->SetOwningPlayer(PlayerController);
	
	//check if the pawn has an Ability System Component
	APlayerBase* PlayerPawn = Cast<APlayerBase>(PlayerController->GetPawn());
	if (!PlayerPawn || !PlayerPawn->GetAbilitySystemComponent()) return;

	//attempt to initialize the widget
	bool bWidgetInit = Widget->InitWidget(PlayerPawn->GetAbilitySystemComponent());

	//delay until next tick and try again if we fail to initialize
	if (!bWidgetInit) {
		GetOwner()->GetWorldTimerManager().SetTimerForNextTick(this, &AHUDBase::InitWidget);
		return;
	}

	//add to viewport on success
	Widget->AddToViewport();
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUDBase.generated.h"

/**
 * 
 */
class UWidgetBase;
UCLASS()
class MOVEMENTDEMO_API AHUDBase : public AHUD
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void DrawHUD() override;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UWidgetBase> WidgetClass;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UWidgetBase> Widget;

	void InitWidget();
};

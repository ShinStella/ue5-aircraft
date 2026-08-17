// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftPlayerController.h"
#include "EnhancedInputSubsystems.h"

AAircraftPlayerController::AAircraftPlayerController()
	: InputMappingContext(nullptr),
	  MoveAction(nullptr),
	  StartUpDownAction(nullptr),
	  StopUpDownAction(nullptr),
	  LookAction(nullptr),
	  RollAction(nullptr)
{
}

void AAircraftPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

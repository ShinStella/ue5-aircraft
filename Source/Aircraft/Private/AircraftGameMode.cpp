// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftGameMode.h"
#include "AircraftPlayerController.h"
#include "AircraftPawn.h"

AAircraftGameMode::AAircraftGameMode()
{
	PlayerControllerClass = AAircraftPlayerController::StaticClass();
	DefaultPawnClass = AAircraftPawn::StaticClass();
}

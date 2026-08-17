// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "AircraftPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;

UCLASS()
class AIRCRAFT_API AAircraftPawn : public APawn
{
	GENERATED_BODY()

public:
	AAircraftPawn();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Pawn")
	void Move(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Pawn")
	void Look(const FInputActionValue& Value);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	UBoxComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	USkeletalMeshComponent* SkeletonMeshComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pawn")
	UCameraComponent* CameraComp;
};

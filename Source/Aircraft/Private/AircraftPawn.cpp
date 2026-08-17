// Fill out your copyright notice in the Description page of Project Settings.


#include "AircraftPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "AircraftPlayerController.h"

AAircraftPawn::AAircraftPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Capsule Component"));
	SetRootComponent(CapsuleComp);

	SkeletonMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeleton Mesh"));
	SkeletonMeshComp->SetupAttachment(RootComponent);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	CameraComp->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
}

void AAircraftPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAircraftPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAircraftPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AAircraftPlayerController* PlayerController = Cast<AAircraftPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AAircraftPawn::Move);
			}
			
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AAircraftPawn::Look);
			}
		}
	}
}

void AAircraftPawn::Move(const FInputActionValue& Value)
{
	if (!GetController())
	{
		return;
	}

	FVector2D MoveInput = Value.Get<FVector2D>();
	FVector AddLocation{};

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		FVector ActorForward = GetActorForwardVector();
		AddLocation += ActorForward * MoveInput.X;
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		FVector ActorRight = GetActorRightVector();
		AddLocation += ActorRight * MoveInput.Y;
	}

	AddLocation.Z = 0.0f;
	AddActorWorldOffset(AddLocation);
}

void AAircraftPawn::Look(const FInputActionValue& Value)
{
	if (!GetController())
	{
		return;
	}

	AAircraftPlayerController* PlayerController = Cast<AAircraftPlayerController>(GetController());

	if (!PlayerController)
	{
		return;
	}

	FVector2D LookInput = Value.Get<FVector2D>();
	FRotator ActorRotator = GetActorRotation();

	if (!FMath::IsNearlyZero(LookInput.X))
	{
		ActorRotator.Yaw += LookInput.X;
	}

	if (!FMath::IsNearlyZero(LookInput.Y))
	{
		ActorRotator.Pitch += LookInput.Y;
	}

	PlayerController->SetControlRotation(ActorRotator);
}


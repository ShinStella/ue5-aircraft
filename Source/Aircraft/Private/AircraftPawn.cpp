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

	MoveSpeed = 10.0f;
	FallingSpeed = 0.0f;
	IsFlying = false;
	IsUp = false;
	GForce = 9.8f;

	bUseControllerRotationRoll = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
}

void AAircraftPawn::BeginPlay()
{
	Super::BeginPlay();
	
	ZOnGound = GetActorLocation().Z;
}

void AAircraftPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsFlying && !IsUp)
	{
		ApplyGravity(DeltaTime);
	}
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

			if (PlayerController->StartUpDownAction)
			{
				EnhancedInput->BindAction(PlayerController->StartUpDownAction, ETriggerEvent::Triggered, this, &AAircraftPawn::StartUpDown);
			}

			if (PlayerController->StopUpDownAction)
			{
				EnhancedInput->BindAction(PlayerController->StopUpDownAction, ETriggerEvent::Triggered, this, &AAircraftPawn::StopUpDown);
			}
						
			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AAircraftPawn::Look);
			}

			if (PlayerController->RollAction)
			{
				EnhancedInput->BindAction(PlayerController->RollAction, ETriggerEvent::Triggered, this, &AAircraftPawn::Roll);
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
	float Speed = MoveSpeed;

	if (IsFlying)
	{
		Speed /= 2;
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		FVector ActorForward = GetActorForwardVector();
		AddLocation += ActorForward * MoveInput.X * Speed;
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		FVector ActorRight = GetActorRightVector();
		AddLocation += ActorRight * MoveInput.Y * Speed;
	}

	AddLocation.Z = 0.0f;
	AddActorWorldOffset(AddLocation);
}

void AAircraftPawn::StartUpDown(const FInputActionValue& Value)
{
	if (!GetController())
	{
		return;
	}

	float UpDownInput = Value.Get<float>();
	
	if (UpDownInput < 0.0f && !IsFlying)
	{
		return;
	}

	FVector ActorLocation = GetActorLocation();
	float Speed = MoveSpeed / 2;
	IsFlying = true;

	if (!FMath::IsNearlyZero(UpDownInput))
	{
		if (0 < UpDownInput)
		{
			IsUp = true;
		}

		FVector ActorUp = GetActorUpVector();
		ActorLocation.Z += ActorUp.Z * UpDownInput * Speed;
	}

	SetActorLocation(ActorLocation);
}

void AAircraftPawn::StopUpDown(const FInputActionValue& Value)
{
	IsUp = false;
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
	FRotator ActorRotation = GetActorRotation();

	if (!FMath::IsNearlyZero(LookInput.X))
	{
		ActorRotation.Yaw += LookInput.X;
	}

	if (!FMath::IsNearlyZero(LookInput.Y))
	{
		ActorRotation.Pitch += LookInput.Y;
	}
	
	PlayerController->SetControlRotation(ActorRotation);
}

void AAircraftPawn::Roll(const FInputActionValue& Value)
{
	if (!GetController() || !IsFlying)
	{
		return;
	}

	AAircraftPlayerController* PlayerController = Cast<AAircraftPlayerController>(GetController());

	if (!PlayerController)
	{
		return;
	}

	float RollInput = Value.Get<float>();
	FRotator ActorRotation = GetActorRotation();

	if (!FMath::IsNearlyZero(RollInput))
	{
		if (ActorRotation.Roll < 30.0f && 0 < RollInput || -30.0f < ActorRotation.Roll && RollInput < 0)
		{
			ActorRotation.Roll += RollInput;
		}
	}

	PlayerController->SetControlRotation(ActorRotation);
}

void AAircraftPawn::ApplyGravity(float DeltaTime)
{
	FallingSpeed += GForce * DeltaTime;
	
	if (!CheckLanding(FallingSpeed))
	{
		FVector ActorLocation = GetActorLocation();
		ActorLocation.Z -= FallingSpeed;
		SetActorLocation(ActorLocation);
	}
	else
	{
		OnLanding();
	}
}

bool AAircraftPawn::CheckLanding(float Distance)
{
	FVector ActorLocation = GetActorLocation();

	FVector BoxExtent = CapsuleComp->GetScaledBoxExtent();
	FVector TraceStart = CapsuleComp->GetComponentLocation();
	FVector TraceEnd = TraceStart - BoxExtent.Z - Distance - ZOnGound;
	FHitResult HitResult{};
	FCollisionQueryParams Params{};
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_WorldStatic, Params);

	if (bHit)
	{
		ActorLocation.Z = HitResult.GetActor()->GetActorLocation().Z;
		SetActorLocation(ActorLocation);
		IsFlying = false;

		return true;
	}
	
	IsFlying = true;

	return false;
}

void AAircraftPawn::OnLanding()
{
	FallingSpeed = 0.0f;

	AAircraftPlayerController* PlayerController = Cast<AAircraftPlayerController>(GetController());

	if (!PlayerController)
	{
		return;
	}

	FRotator ActorRotation = GetActorRotation();
	ActorRotation.Roll = 0.0f;
	SetActorRotation(ActorRotation);
	PlayerController->SetControlRotation(ActorRotation);
}

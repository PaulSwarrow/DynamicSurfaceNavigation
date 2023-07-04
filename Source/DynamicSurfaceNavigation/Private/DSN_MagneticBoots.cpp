// Fill out your copyright notice in the Description page of Project Settings.

#include "DSN_MagneticBoots.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DSN_Ghost.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "DynamicNavSurfaceComponent.h"

// Sets default values for this component's properties
UDSN_MagneticBoots::UDSN_MagneticBoots()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UDSN_MagneticBoots::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (ACharacter *Character = Cast<ACharacter>(GetOwner()))
	{
		CurrentCharacter = Character;
		MovementComponent = Character->GetCharacterMovement();
	}
}

// Called every frame
void UDSN_MagneticBoots::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UDynamicNavSurfaceComponent *Surface = nullptr;
	if (MovementComponent->CurrentFloor.IsWalkableFloor())
	{
		// Get UDynamicNavSurfaceComponent from CurrentFloor.HitResult.GetActor() and set it to Surface
		if (AActor *HitActor = MovementComponent->CurrentFloor.HitResult.GetActor())
		{
			Surface = HitActor->FindComponentByClass<UDynamicNavSurfaceComponent>();
		}
	}

	if (Surface != CurrentSurface)
	{
		if (CurrentSurface != nullptr)
		{
			OnLooseSurface();
		}
		if (Surface != nullptr)
		{
			OnReceiveSurface(Surface);
		}
	}

	if (Ghost != nullptr)
	{

		GetOwner()->SetActorTransform(Surface->TransformVirtual2World(Ghost->GetActorTransform(), true));
		auto velocity = MovementComponent->Velocity;
		auto GhostMovementComponent = Ghost->GetCharacterMovement();
		GhostMovementComponent->MaxCustomMovementSpeed = MovementComponent->GetMaxSpeed();
		// Ghost->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Custom);
		// Ghost->GetCharacterMovement()->MaxAcceleration = MovementComponent->GetMaxAcceleration();

		// Copy component setups
		GhostMovementComponent->RotationRate = MovementComponent->RotationRate;
		GhostMovementComponent->bOrientRotationToMovement = MovementComponent->bOrientRotationToMovement;
		GhostMovementComponent->JumpZVelocity = MovementComponent->JumpZVelocity;
		GhostMovementComponent->AirControl = MovementComponent->AirControl;
		GhostMovementComponent->MaxWalkSpeed = MovementComponent->MaxWalkSpeed;
		// copy character properties
		Ghost->bUseControllerRotationPitch = CurrentCharacter->bUseControllerRotationPitch;
		Ghost->bUseControllerRotationYaw = CurrentCharacter->bUseControllerRotationYaw;
		Ghost->bUseControllerRotationRoll = CurrentCharacter->bUseControllerRotationRoll;

		// Translate input:
		auto Input = CurrentSurface->TransformDirectionWorld2Virtual(MovementComponent->GetLastInputVector());
		GhostMovementComponent->AddInputVector(Input);
		// Restore velocity (if it is caused by ghost's AI)
		MovementComponent->Velocity = CurrentSurface->TransformDirectionVirtual2World(GhostMovementComponent->Velocity);
		// GhostMovementComponent->Velocity = velocity;

		// WHILE NAVIGATING:
		// auto controller = Cast<ACharacter>(GetOwner())->GetController();
		// controller->SetIgnoreMoveInput(true);
		// MovementComponent->SetMovementMode(EMovementMode::MOVE_Custom);
	}

	// ...
}

void UDSN_MagneticBoots::MoveTo(const FVector &Destination)
{
	// Get the AI controller of the actor
	AAIController *NewAIController;
	FVector Location = Destination;
	if (Ghost != nullptr)
	{
		NewAIController = Cast<AAIController>(Ghost->GetController());
		Location = CurrentSurface->TransformPositionWorld2Virtual(Destination);
	}
	else
	{
		NewAIController = Cast<AAIController>(Cast<ACharacter>(GetOwner())->GetController());
	}

	if (NewAIController != nullptr)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(Location);
		NewAIController->MoveTo(MoveRequest);
		// Call AIMoveTo on the AI controller
		// UAIBlueprintHelperLibrary::SimpleMoveToLocation(AIController, Destination);
		CurrentController = NewAIController;
	}
}

void UDSN_MagneticBoots::OnReceiveSurface(UDynamicNavSurfaceComponent *Surface)
{
	CurrentSurface = Surface;
	// TODO Create Character actor
	const FTransform Location = Surface->TransformWorld2Virtual(GetOwner()->GetActorTransform(), true);
	Ghost = GetWorld()->SpawnActor<ADSN_Ghost>(ADSN_Ghost::StaticClass(), Location, FActorSpawnParameters());

	AAIController *AIController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass(), Location, FActorSpawnParameters());
	if (AIController)
	{
		AIController->Possess(Ghost);
	}
}

void UDSN_MagneticBoots::OnLooseSurface()
{
	if (Ghost != nullptr)
	{
		// MovementComponent->Velocity += CurrentSurface->GetVelocityAtPosition(GetOwner()->GetActorLocation());
		Ghost->Destroy();
		Ghost = nullptr;
	}
	CurrentSurface = nullptr;
}

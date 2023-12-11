// Copyright Pagan Games. All rights reserved.

#include "DSN_MagneticBoots.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DSN_Ghost.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NavigationSystem.h"
#include "DynamicNavSurfaceComponent.h"	
#include "Components/CapsuleComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"

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
		//Get Virtual Transform (with feet level adjustement):
		FTransform GhostTransform;
		GhostTransform.SetLocation(Ghost->GetActorLocation()- FVector:: UpVector * Ghost->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		GhostTransform.SetRotation(Ghost->GetTransform().GetRotation());
		GhostTransform.SetScale3D(FVector::OneVector);
		//Transform Virtual to World and restore Upwards direction:
		FTransform DesiredTransform = Surface->TransformVirtual2World(GhostTransform, true);
		
		DesiredTransform.SetLocation(DesiredTransform.GetLocation() - GetOwner()->GetActorTransform().TransformVector(FeetOffset));
		//Apply Transform (with feet level adjustement):
		//SetFeetPosition(DesiredTransform.GetLocation()); //feet position correction (if pivot is in the center o the actor)
		//GetOwner()->SetActorRotation(DesiredTransform.GetRotation());
		GetOwner()->SetActorTransform(DesiredTransform, Sweep, nullptr, TeleportType);
		//No scaling required

		
		//GetOwner()->SetActorTransform(DesiredTransform);
		//auto velocity = MovementComponent->Velocity;
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
		if(Input.Length() > 0.01f) {
			GhostMovementComponent->AddInputVector(Input);
		}
		// Restore velocity (if it is caused by ghost's AI)
		MovementComponent->Velocity = CurrentSurface->TransformDirectionVirtual2World(GhostMovementComponent->Velocity);
		// GhostMovementComponent->Velocity = velocity;
	}

}



void UDSN_MagneticBoots::OnReceiveSurface(UDynamicNavSurfaceComponent *Surface)
{
	CurrentSurface = Surface;
	// TODO Create Character actor
	FTransform CurrentTransform(GetOwner()->GetActorRotation(), GetFeetPosition());
	//const FTransform Location = Surface->TransformWorld2Virtual(GetOwner()->GetActorTransform(), true);
	const FTransform VirtualTransform = Surface->TransformWorld2Virtual(CurrentTransform, true);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Ghost = GetWorld()->SpawnActor<ADSN_Ghost>(ADSN_Ghost::StaticClass(), VirtualTransform, SpawnParams);

	Ghost->OnSmartLinkReached.AddDynamic(this, &UDSN_MagneticBoots::HandleSmartLinkReached);
	GhostController = GetWorld()->SpawnActor<AAIController>(AAIController::StaticClass(), VirtualTransform, FActorSpawnParameters());
	GhostController->Possess(Ghost);
	DynamicSurfaceRegistered = true;
	
	UE_LOG(LogTemp, Warning, TEXT("Character stepped on a dynamic surface: %s"), *CurrentSurface->GetName());
	
}

void UDSN_MagneticBoots::OnLooseSurface()
{
	if (Ghost != nullptr)
	{
		Ghost->OnSmartLinkReached.RemoveAll(this);	
		// MovementComponent->Velocity += CurrentSurface->GetVelocityAtPosition(GetOwner()->GetActorLocation());
		Ghost->Destroy();
		Ghost = nullptr;

		GhostController->GetPathFollowingComponent()->OnRequestFinished.RemoveAll(this);
		GhostController->Destroy();
		GhostController = nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("Character lost a dynamic surface: %s"), *CurrentSurface->GetName());
	CurrentSurface = nullptr;
	DynamicSurfaceRegistered = false;
}

FVector UDSN_MagneticBoots::GetFeetPosition()
{
    return GetOwner()->GetTransform().TransformPosition(FeetOffset);
}

void UDSN_MagneticBoots::SetFeetPosition(FVector WorldPosition)
{

	FVector Position = WorldPosition - GetOwner()->GetActorTransform().TransformVector(FeetOffset);
	GetOwner()->SetActorLocation(Position);
}

void UDSN_MagneticBoots::HandleSmartLinkReached(ANavLinkProxy* Link, const FVector& DestinationPoint)
{	
	const auto RealWorldDestination = CurrentSurface->TransformPositionVirtual2World(DestinationPoint);
	OnSmartLinkReached.Broadcast(Link, RealWorldDestination);
}



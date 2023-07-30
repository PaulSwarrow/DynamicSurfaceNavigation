// Copyright Pagan Games. All rights reserved.

#include "DSN_Ghost.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ADSN_Ghost::ADSN_Ghost()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

// Called when the game starts or when spawned
void ADSN_Ghost::BeginPlay()
{
	Super::BeginPlay();
	RootComponent = GetCapsuleComponent();
	// TODO add static mesh component matching Capsule component form and size
}

// Called every frame
void ADSN_Ghost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if WITH_EDITOR		
	auto capsule = GetCapsuleComponent();
	UKismetSystemLibrary::DrawDebugCapsule(GetWorld(), capsule->GetComponentLocation(), capsule->GetScaledCapsuleHalfHeight(), capsule->GetScaledCapsuleRadius(), capsule->GetComponentRotation(), FColor::Blue, 0.1f, 6.f);
#endif
}

// Called to bind functionality to input
void ADSN_Ghost::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

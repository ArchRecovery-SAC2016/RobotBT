#include "Robot.h"

#include "GameFramework/CharacterMovementComponent.h"

ARobot::ARobot() {
	PrimaryActorTick.bCanEverTick = true;

}

void ARobot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ARobot::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = 50;
}

bool ARobot::ProcessAction() {
	return true;
}


#include "Robot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RobotBT/Controllers/RobotController.h"

ARobot::ARobot() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARobot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (BatteryLevel <= 0) return;

	if (IsMoving) ConsumeBattery(BatteryDischargeRate * DeltaTime);
}

void ARobot::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void ARobot::ProcessAction() {
	if (ActionBatteryDischargeRate.Num() > 0) {
		// Pega um índice aleatório do array
		int32 RandomIndex = FMath::RandRange(0, ActionBatteryDischargeRate.Num() - 1);

		// Pega o valor de descarregamento de bateria aleatório
		float DischargeAmount = ActionBatteryDischargeRate[RandomIndex];

		// Chama o método ConsumeBattery com o valor aleatório
		ConsumeBattery(DischargeAmount);
	}
}

void ARobot::ConsumeBattery(float DischargeAmount) {
	BatteryLevel -= DischargeAmount;

	if (BatteryLevel <= 0) {
		OnBatteryEnd.Broadcast();
	}
}

bool ARobot::MoveToRoomLocation(float DeltaTime) {
	IsMoving = true;
	FVector Location = CurrentRoomInstace->Path->GetLocationByKey(0);
	IsAtRoomLocation = GetRobotController()->MoveToNewLocation(Location, DeltaTime);
	if (IsAtRoomLocation) {
		IsMoving = false;
	}

	return IsAtRoomLocation;
}

bool ARobot::MoveAlongPath(float DeltaTime) {
	IsMoving = true;
	IsFinishedMovingAlongPath = GetRobotController()->MoveAlongSpline(CurrentRoomInstace->Path->Spline, 0, CurrentRoomInstace->Path->Spline->GetNumberOfSplinePoints(), DeltaTime);
	if (IsFinishedMovingAlongPath) {
		IsMoving = false;
	}

	return IsFinishedMovingAlongPath;
}

ARobotController* ARobot::GetRobotController() {
	RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

	return RobotController;
}


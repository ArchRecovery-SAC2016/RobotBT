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
	ASplinePath* Path = GetRoomPath();
	if (Path == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::MoveToRoomLocation] Path is nullptr"));
		return false;
	}

	IsMoving = true;
	FVector Location = Path->GetLocationByKey(0);
	IsAtRoomLocation = GetRobotController()->MoveToNewLocation(Location, DeltaTime);
	if (IsAtRoomLocation) {
		IsMoving = false;
	}

	return IsAtRoomLocation;
}

ASplinePath* ARobot::GetRoomPath() {
	if (CurrentRoomInstace == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::GetRoomPath] CurrentRoomInstace is nullptr"));
		return nullptr;
	}

	if (CurrentRoomInstace->Path.Num() < PathIndex) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::GetRoomPath] CurrentRoomInstace->Path don't have the path"));
		return nullptr;
	}

	return  CurrentRoomInstace->Path[PathIndex];
}

bool ARobot::MoveAlongPath(float DeltaTime) {
	ASplinePath* Path = GetRoomPath();
	if (Path == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::MoveToRoomLocation] Path is nullptr"));
		return false;
	}


	IsMoving = true;
	IsFinishedMovingAlongPath = GetRobotController()->MoveAlongSpline(Path->Spline, 0, Path->Spline->GetNumberOfSplinePoints() -1, DeltaTime);
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


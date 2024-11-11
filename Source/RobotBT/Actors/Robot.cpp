#include "Robot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RobotBT/Controllers/RobotController.h"

ARobot::ARobot() {
	PrimaryActorTick.bCanEverTick = true;
}

void ARobot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsMoving) ConsumeBattery(BatteryDischargeRate * DeltaTime);
}

void ARobot::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void ARobot::ProcessAction() {
	if (ActionBatteryDischargeRate.Num() > 0) {
		// Pega um �ndice aleat�rio do array
		int32 RandomIndex = FMath::RandRange(0, ActionBatteryDischargeRate.Num() - 1);

		// Pega o valor de descarregamento de bateria aleat�rio
		float DischargeAmount = ActionBatteryDischargeRate[RandomIndex];

		// Chama o m�todo ConsumeBattery com o valor aleat�rio
		ConsumeBattery(DischargeAmount);
	}
}

void ARobot::ConsumeBattery(float DischargeAmount) {
	BatteryLevel -= DischargeAmount;

	if (BatteryLevel <= 0) {
		OnBatteryOver.Broadcast(true);
	}
}

ARobotController* ARobot::GetRobotController() {
	RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

	return RobotController;
}


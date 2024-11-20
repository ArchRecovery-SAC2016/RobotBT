#include "Robot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RobotBT/Controllers/RobotController.h"
#include "RobotBT/Widget/RobotWidget.h"

ARobot::ARobot() {
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	WidgetComponent->SetupAttachment(RootComponent);
}

void ARobot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (BatteryLevel <= 0) return;

	if (IsMoving) ConsumeBattery(BatteryDischargeRate * DeltaTime);

	UpdateRobotWidget();
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
	if (GetRoom() == nullptr) return nullptr;
		
	IsMoving = true;
	FVector Location = GetRoom()->GetDoorEntrance();
	IsAtRoomLocation = GetRobotController()->MoveToNewLocation(Location, DeltaTime);
	if (IsAtRoomLocation) {
		IsMoving = false;
	}

	return IsAtRoomLocation;
}

bool ARobot::MoveAlongPath(float DeltaTime) {
	IsMoving = true;
	IsFinishedMovingAlongPath = GetRobotController()->MoveAlongSpline(GetRoomPath(), 0, GetRoomPath()->GetNumberOfSplinePoints() -1, DeltaTime);
	if (IsFinishedMovingAlongPath) {
		IsMoving = false;
	}

	return IsFinishedMovingAlongPath;
}

void ARobot::UpdateCurrentActionText(FString NewAction) {
	CurrentAction = FText::FromString(NewAction);
}

USplineComponent* ARobot::GetRoomPath() {
	if (GetRoom() == nullptr) return nullptr;

	return GetRoom()->GetRoomPath();
}

void ARobot::UpdateRobotWidget() {
	if (ShowWidget == false) return;
	if (GetRobotWidget() == nullptr) return;

	GetRobotWidget()->SetBattery(BatteryLevel);
	GetRobotWidget()->SetAction(CurrentAction);
}


URobotWidget* ARobot::GetRobotWidget() {
	if (RobotWidget != nullptr) return RobotWidget;

	if (WidgetComponent != nullptr && RobotWidgetWBP != nullptr) {
		WidgetComponent->SetWidgetClass(RobotWidgetWBP);
		UUserWidget* UserWidget = WidgetComponent->GetUserWidgetObject();
		if (UserWidget) {
			RobotWidget = Cast<URobotWidget>(UserWidget);
		}
	}

	return RobotWidget;
}

ARoom* ARobot::GetRoom() {
	if (RoomInstace == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobot::GetRoom] Room is nullptr"));
	}

	return RoomInstace;
}

ARobotController* ARobot::GetRobotController() {
	RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

	return RobotController;
}
#include "Robot.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SplineComponent.h"
#include "RobotBT/Controllers/RobotController.h"
#include "RobotBT/Util/UtilMethods.h"
#include "RobotBT/Widget/RobotWidget.h"

ARobot::ARobot() {
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget Component"));
	WidgetComponent->SetupAttachment(RootComponent);
}

void ARobot::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsMoving) ConsumeBattery(RobotProperties.Battery.DischargeRate * DeltaTime);
}

void ARobot::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = RobotProperties.Speed;
	UpdateRobotWidget();
}

void ARobot::ProcessAction(FSkill Skill) {
	// Chama o método ConsumeBattery com o valor aleatório
	ConsumeBattery(Skill.BatteryConsumeDischargeRate);
}

void ARobot::ProcessAction(FString SkillName) {
	for (FSkill Skill : RobotProperties.Skills) {
		if (Skill.Name == SkillName) {
			ProcessAction(Skill);
			return;
		}
	}

	OnTaskFailed.Broadcast(EFailureReasonEnum::SkillFailure, RobotProperties);
}

void ARobot::ConsumeBattery(float DischargeAmount) {
	if (RobotProperties.Battery.Charge <= RobotProperties.Battery.MinimumUsefulLevel) {
		OnTaskFailed.Broadcast(EFailureReasonEnum::LowBattery, RobotProperties);
		return;
	}

	RobotProperties.Battery.Charge -= DischargeAmount ;
	UpdateRobotWidget();
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

	GetRobotWidget()->SetBattery(RobotProperties.Battery.Charge);
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
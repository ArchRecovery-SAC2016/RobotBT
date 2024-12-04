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

	if (IsMoving) {
		ConsumeBattery(RobotProperties.Battery.DischargeRate * DeltaTime);
	}
}

void ARobot::BeginPlay() {
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = RobotProperties.Speed;
	UpdateRobotWidget();
}

void ARobot::PostInitializeComponents() {
	Super::PostInitializeComponents();

	// Salva o Transform inicial
	if (RootComponent) {
		EditorRobotProperties = RobotProperties;
		EditorRobotProperties.InitialTransform = RootComponent->GetComponentTransform();
	}
}

void ARobot::StartNewTask(ESkillEnum SkillEnum, ARoom* Room) {
	if (Room == nullptr) {
		TaskFailed(EFailureReasonEnum::InvalidLocation);
		return;
	}
	RoomInstace = Room;
	CurrentAction = FText::FromString(ESkillEnumHelper::GetDisplayName(SkillEnum));

	FTaskResult NewTaskResult; // need to create a new one
	NewTaskResult.Location = Room->Name;
	NewTaskResult.InitialRobotsProperties = RobotProperties;
	NewTaskResult.SkillUsed = ESkillEnumHelper::GetDisplayName(SkillEnum);
	NewTaskResult.TimeSpentOnTask = GetWorld()->GetTimeSeconds(); // saves the current time
	NewTaskResult.BatterySpentOnTask = RobotProperties.Battery.Charge; // saves the battery level before the task
	TaskResult = NewTaskResult;

	FSkill SkillSelected;

	bool SkillFound = false;
	for (FSkill Skill : RobotProperties.Skills) {
		if (Skill.SkillEnum == SkillEnum) {
			SkillFound = true;
			SkillSelected = Skill;
			break;
		}
	}

	if (SkillFound == false) {
		TaskFailed(EFailureReasonEnum::SkillNotFound);
		return;
	}

	TaskResult.SkillChanceToFail = SkillSelected.ChanceToFail;
	TaskResult.SkillBatteryConsumeDischargeRate = SkillSelected.BatteryConsumeDischargeRate;

	FString SkillName = UEnum::GetValueAsString(SkillEnum);
	FString Message = FString::Printf(TEXT("Initiating skill: %s"), *SkillName);
	UUtilMethods::ShowLogMessage(Message, EMessageColorEnum::INFO);

	// primeiro testo se a skill vai falhar:
	float RandomValue = FMath::FRand();
	if (RandomValue < SkillSelected.ChanceToFail) {
		TaskFailed(EFailureReasonEnum::SkillFailure);
		return;
	}

	// Chama o método ConsumeBattery com o definido na skill
	ConsumeBattery(SkillSelected.BatteryConsumeDischargeRate);
	IsAtRoomLocation = false;
	TaskAllocated = SkillEnum;
	UpdateRobotWidget();
}

void ARobot::GoIdle() {
	CurrentAction = FText::FromString("Idle");
	IsMoving = false;
	UpdateRobotWidget();
}

void ARobot::ConsumeBattery(float DischargeAmount) {
	if (RobotProperties.Battery.Charge <= RobotProperties.Battery.MinimumUsefulLevel) {
		TaskFailed(EFailureReasonEnum::LowBattery);
		return;
	}

	float SpeedFactor = RobotProperties.Speed / 100.0f; // Factor tha impact the discharge rate. The higher the speed, the higher the discharge rate

	float DischargeRate = RobotProperties.Battery.DischargeRate * SpeedFactor;

	RobotProperties.Battery.Charge -= DischargeRate;
	UpdateRobotWidget();
}

bool ARobot::MoveToRoomEntrance() {
	if (GetRoom() == nullptr) return nullptr;
	if (IsAtRoomLocation) return true;

	IsMoving = true;

	IsAtRoomLocation = GetRobotController()->MoveToNewLocation(GetRoom()->GetDoorEntrance());
	if (!IsAtRoomLocation) IsMoving = false;

	return  IsAtRoomLocation;
}

bool ARobot::MoveAlongPath() {
	if (GetRoom() == nullptr) return nullptr;

	IsMoving = true;
	IsFinishedMovingAlongPath = GetRobotController()->MoveAlongSpline(GetRoomPath(), 0, GetRoomPath()->GetNumberOfSplinePoints() - 1);
	if (IsFinishedMovingAlongPath) {
		IsMoving = false;
	}

	return IsFinishedMovingAlongPath;
}

void ARobot::UpdateCurrentActionText(FString NewAction) {
	CurrentAction = FText::FromString(NewAction);
}

void ARobot::GenerateRandomProperties() {
	// reset the base properties
	IsMoving = false;
	IsAtRoomLocation = false;
	IsFinishedMovingAlongPath = false;
	RobotProperties.Skills.Empty();

	TArray<float> Speed{ 80, 100, 110, 120, 150 };
	int32 SpeedIndex = FMath::RandRange(0, Speed.Num() - 1);
	RobotProperties.Speed = Speed[SpeedIndex];

	TArray<FString> Colors{ "Red", "Blue", "Green", "Yellow", "Black" };
	int32 ColorIndex = FMath::RandRange(0, Colors.Num() - 1);
	RobotProperties.Color = Colors[ColorIndex];

	TArray<FBattery> Batteries{
	{ 1.0f, 0.0f, 0.0001f },
	{ 0.9f, 0.0f, 0.0001f },
	{ 0.8f, 0.0f, 0.0001f },
	{ 0.7f, 0.0f, 0.0001f },
	{ 0.6f, 0.0f, 0.0001f },
	{ 1.0f, 0.0f, 0.001f },
	{ 1.0f, 0.0f, 0.0015f },
	{ 1.0f, 0.0f, 0.002f },
	{ 1.0f, 0.0f, 0.0025f },
	{ 1.0f, 0.0f, 0.003f }
	};
	int32 BatteryIndex = FMath::RandRange(0, Batteries.Num() - 1);
	RobotProperties.Battery = Batteries[BatteryIndex];
}

void ARobot::TaskFailed(EFailureReasonEnum FailureReason) {
	TaskResult.SuccessResult = false;
	TaskResult.FailureReasonEnum = FailureReason;
	TaskResult.EndRobotsProperties = RobotProperties;
	OnTaskFinished.Broadcast(TaskResult);
}

void ARobot::TaskFinished() {
	TaskResult.BatterySpentOnTask = RobotProperties.Battery.Charge - TaskResult.BatterySpentOnTask;
	// calcula o tempo gasto. Pega o tempo atual e subtrai pelo tempo que foi salvo ao iniciar a task
	TaskResult.TimeSpentOnTask = GetWorld()->GetTimeSeconds() - TaskResult.TimeSpentOnTask;
	IsAtRoomLocation = false;
	IsFinishedMovingAlongPath = false;
	TaskAllocated = ESkillEnum::NONE;
	TaskResult.SuccessResult = true;
	TaskResult.EndRobotsProperties = RobotProperties;
	GoIdle();
	OnTaskFinished.Broadcast(TaskResult);
}

bool ARobot::TaskExecution() {
	// the child implement this task
	return true;
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

void ARobot::Initiate(bool bGenerateRandomProperties) {
	if (bGenerateRandomProperties) {
		GenerateRandomProperties();
	} else {
		RobotProperties = EditorRobotProperties;
	}

	IsMoving = false;
	IsFinishedMovingAlongPath = false;
	IsAtRoomLocation = false;
	TaskAllocated = ESkillEnum::NONE;

	GoIdle();

	// move to the editor position
	SetActorTransform(EditorRobotProperties.InitialTransform);
}

ARoom* ARobot::GetRoom() {
	if (RoomInstace == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobot::GetRoom] Room is nullptr"));
	}

	return RoomInstace;
}

USplineComponent* ARobot::GetRoomPath() {
	if (GetRoom() == nullptr) return nullptr;

	USplineComponent* RoomPath = GetRoom()->GetRoomPath();
	if (RoomPath == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobot::GetRoomPath] RoomPath is nullptr"));
	}

	return RoomPath;
}

ARobotController* ARobot::GetRobotController() {
	RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

	return RobotController;
}

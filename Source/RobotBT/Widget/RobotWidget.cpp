#include "RobotWidget.h"
#include "RoomPropertiesWidget.h"

void URobotWidget::NativeConstruct() {
	Super::NativeConstruct();

}

void URobotWidget::SetBattery(float BatteryLevel) {
	Battery = BatteryLevel;
}

float URobotWidget::GetBattery() {
	return Battery;
}

void URobotWidget::SetAction(FText NewAction) {
	Action = NewAction;
}

FText URobotWidget::GetAction() {
	return Action;
}


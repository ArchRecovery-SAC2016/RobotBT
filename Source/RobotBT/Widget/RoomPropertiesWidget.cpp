#include "RoomPropertiesWidget.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "RobotBT/RobotBTGameMode.h"

void URoomPropertiesWidget::NativeConstruct() {
	Super::NativeConstruct();

}

void URoomPropertiesWidget::ChangeProperties(bool bIsClean, bool bIsPrepared, bool bDoorOpen)	{
	SetIsClear(bIsClean);
	SetIsPrepared(bIsPrepared);
	SetDoorOpen(bDoorOpen);
}

void URoomPropertiesWidget::SetRoomName(FString NewValue) {
	RoomName->SetText(FText::FromString(NewValue));
}

void URoomPropertiesWidget::SetLocation(FString NewValue) {
	RoomLocation->SetText(FText::FromString(NewValue));
}

void URoomPropertiesWidget::SetIsClear(bool NewValue) {
	IsClear_Check->SetCheckedState(NewValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

void URoomPropertiesWidget::SetIsPrepared(bool NewValue) {
	IsPrepared_Check->SetCheckedState(NewValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

void URoomPropertiesWidget::SetDoorOpen(bool NewValue) {
	DoorOpen_Check->SetCheckedState(NewValue ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}



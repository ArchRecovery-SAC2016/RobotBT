#include "RoomPropertiesWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/RobotBTGameMode.h"

void URoomPropertiesWidget::NativeConstruct() {
	Super::NativeConstruct();

	if (ApplyButton != nullptr) {
		ApplyButton->OnClicked.AddDynamic(this, &URoomPropertiesWidget::ChangeWorldKnowledge);
	}

}

void URoomPropertiesWidget::Initiate(const FRoomPreparationStruct& RoomPreparation) {
	RoomName->SetText(FText::FromString(RoomPreparation.Name));
	RoomLocation->SetText(FText::FromString(RoomPreparation.Location));
	SetIsClear(RoomPreparation.bIsClean);
	SetIsPrepared(RoomPreparation.bIsPrepared);
	SetDoorOpen(RoomPreparation.bDoorOpen);
}
	
void URoomPropertiesWidget::ChangeWorldKnowledge() {
	ARobotBTGameMode* GameMode = Cast<ARobotBTGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode != nullptr) {
		FRoomPreparationStruct RoomPreparation;
		RoomPreparation.Name = RoomName->GetText().ToString();
		RoomPreparation.Location = RoomLocation->GetText().ToString();
		RoomPreparation.bIsClean = IsClear_Check->IsChecked();
		RoomPreparation.bIsPrepared = IsPrepared_Check->IsChecked();
		RoomPreparation.bDoorOpen = DoorOpen_Check->IsChecked();
	} else {
		UE_LOG(LogTemp, Error, TEXT("[URoomPropertiesWidget::ChangeWorldKnowledge] GameMode is null"));
	}
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

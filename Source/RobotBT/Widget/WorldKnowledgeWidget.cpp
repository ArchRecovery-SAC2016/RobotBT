#include "WorldKnowledgeWidget.h"
#include "RoomPropertiesWidget.h"

void UWorldKnowledgeWidget::NativeConstruct() {
	Super::NativeConstruct();

}

void UWorldKnowledgeWidget::SynchronizeProperties() {
	Super::SynchronizeProperties();
}

URoomPropertiesWidget* UWorldKnowledgeWidget::GetRoomByName(FString Name) {
	if (Name == "RoomA") return RoomA;
	if (Name == "RoomB") return RoomB;
	if (Name == "RoomC") return RoomC;
	if (Name == "RoomD") return RoomD;

	UE_LOG(LogTemp, Error, TEXT("[URoomPropertiesWidget* UWorldKnowledgeWidget] Invalid Room Name. Must be RoomA, RoomB, RoomC, RoomD"));

	return nullptr;
}

void UWorldKnowledgeWidget::ChangeRoomProperties(FString Name, bool bIsClean, bool bIsPrepared, bool bDoorOpen) {
	URoomPropertiesWidget* Room = GetRoomByName(Name);

	if (Room == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[UWorldKnowledgeWidget::ChangeRoomProperties] Room is null"));
		return;
	}

	Room->ChangeProperties(bIsClean, bIsPrepared, bDoorOpen);
}

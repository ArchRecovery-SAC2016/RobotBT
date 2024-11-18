#include "WorldKnowledgeWidget.h"
#include "RoomPropertiesWidget.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/Actors/RoomPreparation/RoomPreparation.h"

void UWorldKnowledgeWidget::NativeConstruct() {
	Super::NativeConstruct();

	// Load all Doors Sensors, so we can watch it
	TArray<AActor*> RoomsOnMap;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARoomPreparation::StaticClass(), RoomsOnMap);

	for (AActor* Actor : RoomsOnMap) {
		ARoomPreparation* Room = Cast<ARoomPreparation>(Actor);
		URoomPropertiesWidget* RoomWidget = GetRoomByName(Room->Name);
		if (Room != nullptr) RoomWidget->SetRoomInstance(Room);
		else UE_LOG(LogTemp, Error, TEXT("[UWorldKnowledgeWidget::NativeConstruct] Room is null"));
	}
}

void UWorldKnowledgeWidget::SynchronizeProperties() {
	Super::SynchronizeProperties();
}

URoomPropertiesWidget* UWorldKnowledgeWidget::GetRoomByName(FString Name) {
	if (Name == "RoomA") return RoomA;
	if (Name == "RoomB") return RoomB;
	if (Name == "RoomC") return RoomC;
	if (Name == "SanitizationRoom") return RoomD;

	UE_LOG(LogTemp, Error, TEXT("[URoomPropertiesWidget* UWorldKnowledgeWidget] Invalid Room Name. Must be RoomA, RoomB, RoomC, SanitizationRoom"));

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

// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldKnowledgeWidget.h"

#include "RoomPropertiesWidget.h"
#include "Kismet/GameplayStatics.h"
#include "RobotBT/RobotBTGameMode.h"

void UWorldKnowledgeWidget::NativeConstruct() {
	Super::NativeConstruct();

	InitialLoad();

	if (Rooms.Num() == 0) {
		UE_LOG(LogTemp, Error, TEXT("[UWorldKnowledgeWidget::SynchronizeProperties] Rooms is empty"));
		return;
	}
}

void UWorldKnowledgeWidget::SynchronizeProperties() {
	Super::SynchronizeProperties();
}

void UWorldKnowledgeWidget::InitialLoad() {
	ARobotBTGameMode* GameMode = Cast<ARobotBTGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GameMode == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[UWorldKnowledgeWidget::InitialLoad] GameMode is null. You mus se the game mode on the project settings"));
		return;
	}

	Rooms = GameMode->GetRooms();

	if (Rooms.IsValidIndex(0) && RoomA != nullptr) {
		RoomA->Initiate(Rooms[0]);
	}

	if (Rooms.IsValidIndex(1) && RoomB != nullptr) {
		RoomB->Initiate(Rooms[1]);
	}

	if (Rooms.IsValidIndex(2) && RoomC != nullptr) {
		RoomC->Initiate(Rooms[2]);
	}

	if (Rooms.IsValidIndex(3) && RoomD != nullptr) {
		RoomD->Initiate(Rooms[3]);
	}
}
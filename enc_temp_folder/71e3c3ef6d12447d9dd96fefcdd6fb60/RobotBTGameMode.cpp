// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotBTGameMode.h"
#include "RobotBTPlayerController.h"
#include "RobotBTCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARobotBTGameMode::ARobotBTGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARobotBTPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

    LoadRoomsFromFile();
}


void ARobotBTGameMode::BeginPlay() {
    Super::BeginPlay();
}

void ARobotBTGameMode::ChangeRoom(const FRoomPreparationStruct& RoomConfiguration) {
	for (int i = 0; i < Rooms.Num(); i++) {
		if (Rooms[i].Name == RoomConfiguration.Name) {
			Rooms[i].bIsClean = RoomConfiguration.bIsClean;
			Rooms[i].bDoorOpen = RoomConfiguration.bDoorOpen;
			Rooms[i].bIsPrepared = RoomConfiguration.bIsPrepared;

            UE_LOG(LogTemp, Display, TEXT("Room %s isClean: %d, DoorOpen: %d, IsPrepared: %d"), *Rooms[i].Name, Rooms[i].bIsClean, Rooms[i].bDoorOpen, Rooms[i].bIsPrepared);
		}
	}
}

void ARobotBTGameMode::LoadRoomsFromFile() {
    FRoomPreparationStruct RoomA = FRoomPreparationStruct(TEXT("RoomA"), TEXT("c3"),
        false,
        false,
        false);

    FRoomPreparationStruct RoomB = FRoomPreparationStruct(TEXT("RoomB"), TEXT("c6"),
        false,
        false,
        true);

    FRoomPreparationStruct RoomC = FRoomPreparationStruct(TEXT("RoomC"), TEXT("c8"),
        true,
        true,
        true);

    FRoomPreparationStruct SanitizationRoom = FRoomPreparationStruct(TEXT("SanitizationRoom"), TEXT("c10"),
        true,
        true,
        true);

   
    Rooms.Add(RoomA);
    Rooms.Add(RoomB);
    Rooms.Add(RoomC);
    Rooms.Add(SanitizationRoom);
}


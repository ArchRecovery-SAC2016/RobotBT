#include "RobotCleaner.h"

#include "Components/SplineComponent.h"
#include "RobotBT/Util/UtilMethods.h"

ARobotCleaner::ARobotCleaner() {
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("CleanerRobot"));
}

void ARobotCleaner::BeginPlay() {
	Super::BeginPlay();

	RobotController = Cast<ARobotController>(GetController());
	if (RobotController == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] RobotController is nullptr"));
	}

	if (MovePlan.Num() > 0) {
		CurrentSplinePath = GetPathByIndentifier(MovePlan[0].Identifier);
	}else {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::BeginPlay] You need to specifie a Move Plan"));
	}
}

void ARobotCleaner::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (CurrentSplinePath != nullptr && CurrentSplinePath->Spline) {
		if (ShowStartMoveMessage) {
			FString Message = FString::Printf(TEXT("Se movendo de %d até %d, do Path %s"),
				MovePlan[CurrentMovePlanIndex].StartKey,
				MovePlan[CurrentMovePlanIndex].EndKey,
				*CurrentSplinePath->Identifier);
			UUtilMethods::ShowLogMessage(Message, EMessageColorEnum::INFO);
			ShowStartMoveMessage = false;
		}

		// Incrementa a distância ao longo do spline
		DistanceAlongSpline += Speed * DeltaTime; // Use a variável de membro

		// Verifique se chegou ao final do spline
		bool bReachedEnd = MoveAlongSpline(CurrentSplinePath->Spline,
			MovePlan[CurrentMovePlanIndex].StartKey,
			MovePlan[CurrentMovePlanIndex].EndKey,
			DeltaTime);

		if (bReachedEnd) {
			ShowStartMoveMessage = true;
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Chegou no destino"));
			ExecuteCurrentMovePlan();

			// Reinicializa a distância para o próximo movimento
			DistanceAlongSpline = 0.0f; // Reseta para o próximo movimento
		}
	}
	else {
		ExecuteCurrentMovePlan();
	}
}

bool ARobotCleaner::MoveAlongSpline(USplineComponent* SplineComponent, int32 StartIndex, int32 EndIndex, float DeltaTime) {
	if (!SplineComponent) {
		return false; // Verificação de segurança caso o spline seja inválido
	}

	// Calcula as distâncias de início e fim ao longo do spline
	float StartDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(StartIndex);
	float EndDistance = SplineComponent->GetDistanceAlongSplineAtSplinePoint(EndIndex);

	// Garante que a distância inicial seja menor que a final
	if (EndDistance < StartDistance) {
		Swap(StartDistance, EndDistance);
	}

	// Verifica se chegou ao final
	if (DistanceAlongSpline >= EndDistance) {
		DistanceAlongSpline = EndDistance;
		return true; // Retorna true ao atingir o ponto final
	}

	// Atualiza a posição e rotação do ator ao longo do spline
	FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	FRotator NewRotation = SplineComponent->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	// Move o próprio ator
	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);

	return false; // Não chegou ao final ainda
}

void ARobotCleaner::StartCleaningRoom(ADoorSensor* _RoomSelected) {
	if (_RoomSelected == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::StartCleaningRoom] RoomSelected is nullptr"));
		return;
	}

	IsCleaning = true;
	this->RoomSelected = _RoomSelected;
}

void ARobotCleaner::StartSanitize(ADoorSensor* _RoomSelected) {
	if (_RoomSelected == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::StartCleaningRoom] RoomSelected is nullptr"));
		return;
	}

	IsSanitizing = true;
	this->RoomSelected = _RoomSelected;
}

void ARobotCleaner::StartOpeningDoor(ADoorSensor* _RoomSelected) {
	if (_RoomSelected == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::StartCleaningRoom] RoomSelected is nullptr"));
		return;
	}

	IsOpeningDoor = true;
	this->RoomSelected = _RoomSelected;
	this->RoomSelected->OnDoorOpen.AddDynamic(this, &ARobotCleaner::DoorOpenCompleted);
}



bool ARobotCleaner::ExecuteCurrentMovePlan() {
	if (CurrentSplinePath == nullptr) return false;

	// Pega o caminho baseado no identificador atual
	CurrentSplinePath = GetPathByIndentifier(MovePlan[CurrentMovePlanIndex].Identifier);

	if (CurrentSplinePath == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[ARobotCleaner::ExecuteCurrentMovePlan] CurrentPath is nullptr"));
		return false;
	}

	return true;
}

void ARobotCleaner::CleanRoom() {
	// first we need to move inside the room
	if (bFrontOfRoom == false) {
		GoFrontOfRoom(RoomSelected->FrontRoomLocation);
	}

	// if get here, we are inside the room,and now we need to clean the trash
	bool AllClean = true;
	for (auto &RoomTrash : RoomSelected->RoomTrash) {
		if (RoomTrash->IsTrashClean == false) {
			bool AtLocation  = GetRobotController()->MoveToActorLocation(RoomTrash);
			if (AtLocation == false) {
				AllClean = false;
			}
		}
	}

	// if everything is clean, we need to go outside the room
	if (AllClean == true && bOutsideRoom == false) {
		GoOutsideOfRoom(RoomSelected->OutsideRoomLocation);
	}

	// if all clean and we are outside room, we finished here and notifi the success
	if (AllClean == true && bOutsideRoom == true) {
		IsCleaning = false;
		IsRobotSanitized = false;

		// reset the variables
		bFrontOfRoom = false;
		bCenterOfRoom = false;
		bOutsideRoom = false;

		OnRoomCleaned.Broadcast(true);

	}
}

void  ARobotCleaner::SanitizeRoom() {
	if (bFrontOfRoom == false) {
		GoFrontOfRoom(RoomSelected->FrontRoomLocation);
	}

	// if is already in front, and not in the center, go to the center
	if (bFrontOfRoom == true && bCenterOfRoom == false) {
		GoCenterOfRoom(RoomSelected->CenterRoomLocation);
	}

	// if is already in front, and not in the center, got to the center
	if (bFrontOfRoom == true && bCenterOfRoom == true && bOutsideRoom == false) {
		GoOutsideOfRoom(RoomSelected->OutsideRoomLocation);
	}

	// if is already outside and inside, we are done
	if (bOutsideRoom) {
		IsSanitizing = false;

		bFrontOfRoom = false;
		bCenterOfRoom = false;
		bOutsideRoom = false;

		OnRobotSanitized.Broadcast(true);
	}
}

ASplinePath* ARobotCleaner::GetPathByIndentifier(FString Identifier) {
	for (auto& Path : SplinePath) {
		if (Path->Identifier == Identifier) {
			return Path;
		}
	}

	return nullptr;
}

void ARobotCleaner::OnReachedEnd() {
	if (CurrentMovePlanIndex + 1 < MovePlan.Num()) {
		CurrentMovePlanIndex++;
		ExecuteCurrentMovePlan();
	} else {
		CurrentSplinePath = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Movimento completo!"));
	}
}

void ARobotCleaner::OpenRoom() {
	// just try to get inside. When we touch the door, the door will open and the event DoorOpenCompleted will broadcast the success of this action
	if (bFrontOfRoom == false) {
		GoFrontOfRoom(RoomSelected->FrontRoomLocation);
	}

}

void ARobotCleaner::DoorOpenCompleted(bool bNewState) {
	IsOpeningDoor = false;

	OnDoorOpened.Broadcast(true);
}


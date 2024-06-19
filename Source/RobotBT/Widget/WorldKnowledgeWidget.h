#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RobotBT/Struct/RoomPreparationStruct.h"
#include "WorldKnowledgeWidget.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTBT_API UWorldKnowledgeWidget : public UUserWidget {
	GENERATED_BODY()

protected:
	// Doing setup in the C++ constructor is not as
	virtual void NativeConstruct() override;


public:
	UPROPERTY(EditAnywhere)
	TArray<FRoomPreparationStruct> Rooms;

	UFUNCTION()
	void SetRooms(TArray<FRoomPreparationStruct> NewRooms);

	UFUNCTION()
	virtual void SynchronizeProperties() override;

	UFUNCTION()
	void InitialLoad();


};

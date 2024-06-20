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
	virtual void SynchronizeProperties() override;

	UFUNCTION()
	void InitialLoad();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class URoomPropertiesWidget* RoomA;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class URoomPropertiesWidget* RoomB;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class URoomPropertiesWidget* RoomC;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class URoomPropertiesWidget* RoomD;

	void ChangeRoomProperties(FString Name, bool bIsClean, bool bIsPrepared, bool bDoorOpen);

	URoomPropertiesWidget* GetRoomByName(FString Name);
};

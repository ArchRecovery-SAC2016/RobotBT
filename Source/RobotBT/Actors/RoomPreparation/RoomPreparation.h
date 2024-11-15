#pragma once

#include "CoreMinimal.h"
#include "Furniture.h"
#include "RoomTrash.h"
#include "GameFramework/Actor.h"
#include "RobotBT/Actors/Room.h"
#include "RoomPreparation.generated.h"

UCLASS()
class ROBOTBT_API ARoomPreparation : public ARoom {
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:	
	ARoomPreparation();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room")
	TArray<ARoomTrash*> Trashes;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room")
	TArray<AFurniture*> Furnitures;

	/* Return if the run is prepared */
	bool IsRoomPrepared();
		
};

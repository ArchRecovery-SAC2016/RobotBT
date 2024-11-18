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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	class USplineComponent* OrganizePath1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	class USplineComponent* OrganizePath2;

	/* Return if the run is prepared */
	bool IsRoomPrepared();

	USplineComponent* GetOrganizePath(int32 index);

	// Initiate this room configuration. 
	void Initiate(bool DoorOpen, bool WithTrash, bool FurnitureInPlace);

	bool IsTrashClean();

	bool IsFurnitureOrganized();
		
};

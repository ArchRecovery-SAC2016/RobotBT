#pragma once

#include "CoreMinimal.h"
#include "RoomTrash.h"
#include "GameFramework/Actor.h"
#include "DoorSensor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorStateChange, FString, Name);

UCLASS()
class ROBOTBT_API ADoorSensor : public AActor {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ADoorSensor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	bool Opened = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	class UBoxComponent* Collision;

	// saves the furnitures of this room
	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	TArray<class AFurniturePlace*> FurnituresPlace;

	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	TArray<ARoomTrash*> RoomTrash;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor RedColor;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor GreenColor;

	// Used to move the robot to the inside of the room. Before the robot starts to clean the room, it will move to this location	
	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	FVector InsideRoomLocation;

	// Used to move the robot to the outside of the room. After all actions finish, the robot will move to this location
	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	FVector OutsideRoomLocation;

	UFUNCTION()
	FVector3d GetNextClearPosition();

	UFUNCTION()
	FVector3d GetNextForniturePosition();

	UFUNCTION(BlueprintCallable)
	bool IsPrepared();

	UFUNCTION()
	bool CheckFornitureIsArranged();

	UFUNCTION()
	bool CheckIsRooomClean();

	UPROPERTY()
	FOnDoorStateChange OnDoorChange;

	UPROPERTY()
	bool is_sanitized = false;

	bool is_prepared = false;

private:
	void ChangeColor(bool NewValue);



	
};

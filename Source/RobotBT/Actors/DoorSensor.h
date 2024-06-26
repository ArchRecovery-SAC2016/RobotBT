#pragma once

#include "CoreMinimal.h"
#include "RoomTrash.h"
#include "GameFramework/Actor.h"
#include "DoorSensor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorOpenChange, bool, NewState);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	bool Opened = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "Room")
	class UBoxComponent* Collision;

	// saves the furnitures position of this room
	UPROPERTY(EditInstanceOnly, Category = "Room")
	TArray<class AFurniturePlace*> FurnituresPlace;

	UPROPERTY(EditInstanceOnly, Category = "Room")
	TArray<ARoomTrash*> RoomTrash;

	UPROPERTY(EditDefaultsOnly, Category = "Room")
	FLinearColor RedColor;

	UPROPERTY(EditDefaultsOnly, Category = "Room")
	FLinearColor GreenColor;

	// Used to move the robot to the front of the room. Before the robot starts to clean the room, it will move to this location	
	UPROPERTY(EditInstanceOnly, Category = "Room")
	FVector FrontRoomLocation;

	// Used to move the robot to the center of the room. Used by sanitization robot to move to the center of the room
	UPROPERTY(EditInstanceOnly, Category = "Room")
	FVector CenterRoomLocation;

	// Used to move the robot to the outside of the room. After all actions finish, the robot will move to this location
	UPROPERTY(EditInstanceOnly, Category = "Room")
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
	FOnDoorOpenChange OnDoorOpen;
private:
	void ChangeColor(bool NewValue);

	// this event is called when the robot wants to open the door.
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void ControlDoorOpen();
	
};

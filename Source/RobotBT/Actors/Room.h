#pragma once

#include "CoreMinimal.h"
#include "SplinePath.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDoorOpenChange, bool, NewState);

UCLASS()
class ROBOTBT_API ARoom : public AActor {
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

public:	
	ARoom();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	UArrowComponent* BaseLocation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Room")
	class UBoxComponent* DoorCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
	bool DoorOpened = true;

	UPROPERTY(EditDefaultsOnly, Category = "Room")
	FLinearColor RedColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "Room")
	FLinearColor GreenColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	class USplineComponent* MainPath;

	UPROPERTY()
	FOnDoorOpenChange OnDoorOpen;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual FVector GetDoorEntrance();

	UFUNCTION()
	void OpenDoor(bool NewValue);

	UFUNCTION()
	void ChangeColorToOK(bool NewValue);

	UFUNCTION()
	virtual USplineComponent* GetRoomPath();
private:
	UFUNCTION()
	void ControlDoorOpen();


};

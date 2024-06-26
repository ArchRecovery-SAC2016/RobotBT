#pragma once

#include "CoreMinimal.h"
#include "Furniture.h"
#include "GameFramework/Actor.h"
#include "FurniturePlace.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFurnitureStateChange, bool, NewState);

UCLASS()
class ROBOTBT_API AFurniturePlace : public AActor {
	GENERATED_BODY()
	
public:	
	AFurniturePlace();

protected:
	
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	class UBoxComponent* Collision;

	// the furniture that belogns to this place. Is setted on the map
	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	AFurniture* AcceptedFurniture;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor RedColor;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor GreenColor;

	UPROPERTY()
	bool FurnitureInPlace = false;

	UPROPERTY(BlueprintAssignable, Category = "MyComponentsDispatcher")
	FOnFurnitureStateChange OnFurnitureStateChange;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ChangeColor(bool NewValue);

};

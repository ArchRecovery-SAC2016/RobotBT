#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FurniturePlace.generated.h"

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

	/** Witch furniture place is the correct */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	int32 AcceptedFurnitureID = 0;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor RedColor;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	FLinearColor GreenColor;

	UPROPERTY()
	bool FurnitureInPlace = false;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ChangeColor(bool NewValue);

};

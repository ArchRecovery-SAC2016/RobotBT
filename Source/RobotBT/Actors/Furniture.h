#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Furniture.generated.h"

UCLASS()
class ROBOTBT_API AFurniture : public AActor {
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Sets default values for this actor's properties
	AFurniture();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Id do Furniture. Eh obrigatorio! */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "MyComponent")
	int32 ID = 0;

	/** Base Mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MyComponent")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	class UBoxComponent* Collision;

	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	FVector GoToPosition;

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool ShouldMove = false;
};

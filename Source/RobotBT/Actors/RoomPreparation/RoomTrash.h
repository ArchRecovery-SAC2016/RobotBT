#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomTrash.generated.h"


UCLASS()
class ROBOTBT_API ARoomTrash : public AActor {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:	
	ARoomTrash();

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	UStaticMeshComponent* TrashMesh;

	UPROPERTY(EditDefaultsOnly, Category = "MyComponent")
	class UBoxComponent* Collision;

	UPROPERTY(EditInstanceOnly, Category = "MyComponent")
	bool IsTrashClean = false; 

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

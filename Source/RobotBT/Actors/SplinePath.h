#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplinePath.generated.h"


UCLASS()
class ROBOTBT_API ASplinePath : public AActor {
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	

public:	
	ASplinePath();

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyComponent")
	class USplineComponent* Spline;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "MyComponent")
	FString Identifier;

	UFUNCTION()
	FVector GetLocationByKey(int32 KeyIndex);

};

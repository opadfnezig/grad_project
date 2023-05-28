#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AI_ZombieCPP.generated.h"

UCLASS()
class A3681_API AAI_ZombieCPP : public ACharacter
{
	GENERATED_BODY()

public:
	AAI_ZombieCPP();

	UFUNCTION(BlueprintCallable)
		void ActivateAI();
		void CallAttackEvent();
	UFUNCTION(BlueprintCallable)
		void MoveTowardsTarget();
		


public:
	UPROPERTY(BlueprintReadWrite)
		bool bIsAIActivated;
		float DistanceToPlayer;
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	ACharacter* PlayerCharacter;
};

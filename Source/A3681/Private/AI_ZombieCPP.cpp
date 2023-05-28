#include "AI_ZombieCPP.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AAI_ZombieCPP::AAI_ZombieCPP()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AAI_ZombieCPP::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AAI_ZombieCPP::ActivateAI()
{
	bIsAIActivated = true;
}

void AAI_ZombieCPP::CallAttackEvent()
{
	if (UFunction* AttackEvent = FindFunction(FName(TEXT("AttackEvent"))))
	{
		ProcessEvent(AttackEvent, nullptr);
	}
}

void AAI_ZombieCPP::MoveTowardsTarget()
{
	FVector Direction = PlayerCharacter->GetActorLocation() - GetActorLocation();
	Direction.Normalize();
	GetCharacterMovement()->AddInputVector(Direction);

	FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	SetActorRotation(TargetRotation);
}

void AAI_ZombieCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsAIActivated)
	{
		float DistanceT = FVector::Distance(GetActorLocation(), PlayerCharacter->GetActorLocation());

		if (DistanceT > 170.0f)
		{
			MoveTowardsTarget();
		}
		if (DistanceT < 175.0f)
		{
			CallAttackEvent();
		}
	}
	
}

void AAI_ZombieCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


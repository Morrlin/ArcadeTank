#include "HealthComponent.h"
#include "ToonTanks/Gamemode/TankGamemodeBase.h"
#include "ToonTanks/Pawn/PawnTank.h"
#include "Kismet/GameplayStatics.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Health = DefaultHealth;
	GamemodeRef = Cast<ATankGamemodeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	PlayerTank = Cast<APawnTank>(UGameplayStatics::GetPlayerPawn(this, 0));

	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::TakeDamage);

	UE_LOG(LogTemp, Warning, TEXT("%s Health is %f"), *GetOwner()->GetName(), Health);

	if (Cast<APawnTank>(GetOwner()) == PlayerTank)
	{
		PlayerTank->TankDefaultHealth = DefaultHealth;
		PlayerTank->TankCurrentHealth = DefaultHealth;
	}
}

void UHealthComponent::TakeDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage == 0 || Health <= 0)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s Health is %f"), *GetOwner()->GetName(),  Health);   

	if (Health <= 0)
	{
		if (GamemodeRef)
		{
			GamemodeRef->ActorDied(GetOwner());
		}
		else if (!GamemodeRef)
		{
			UE_LOG(LogTemp, Error, TEXT("Health component has no reference to the gamemode"));
		}
	}

	if (Cast<APawnTank>(GetOwner()) == PlayerTank)
	{
		PlayerTank->TankCurrentHealth = Health;
	}
}



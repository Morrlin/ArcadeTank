#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TankGamemodeBase.generated.h"

class APawnTank;
class APawnTurret;
class APlayerControllerBase;

UCLASS()
class TOONTANKS_API ATankGamemodeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:

	APawnTank* PlayerTank;
	APlayerControllerBase* PlayerControllerRef;

	void HandleGameStart();
	void HandleGameOver(bool PlayerWon);

	void SetSpawnActorTimer(APawnTurret* DestroyedTurret);
	void SpawnActor();

	void SetActorLocationRotation(APawnTurret* DestroyedTurret, FVector LastTurretLocation, FRotator LastTurretRotation);

	void RefreshHealth();

	void CheckForCountdown();

	int32 LastHighScore;

	APawnTurret* TempTurret;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float RespawnDelay = 5.f;

	bool DidCheckedTurretLocation = false;

	FVector ThisTurretLocation;
	FRotator ThisTurretRotation;

	FVector DestroyedTurretLocation;
	FRotator DestroyedTurretRotation;

	TArray<FVector> TurretLocationArray;
	TArray<FRotator> TurretRotationArray;

public:

	void ActorDied(AActor* DeadActor);

	float GetRespawnDelay();

	float GetTurretRange();
	float GetFireRate();

	UFUNCTION(BlueprintImplementableEvent)
	void FirstShowTimer();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowScore();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowTimer();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHealthBar();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowReloadBar();

	UPROPERTY( BlueprintReadOnly, Category = "Tank Health", meta = (AllowPrivateAccess = "true"))
	float DefaultTankHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Tank Health", meta = (AllowPrivateAccess = "true"))
	float CurrentTankHealth;

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void GameStart();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool EndOfTime);

	void SetCurrentreloadTime();

	void PlayStartCountdownSound();

	int32 GetScore();
	int32 GetGamePeriod();

	void SetActorLocationRotation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret Type", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APawnTurret> TurretClass;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* CountdownSound;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundBase* StartCountdownSound;

	APawnTurret* TurretPawn;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float TurretFireRate = 2.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float TurretRange = 1000.f;

	UPROPERTY(EditAnywhere, Category = "Main")
	float UpgradeMultiply = 1.05f;

	UPROPERTY(EditAnywhere, Category = "Main")
	float PointsPerTurret = 100;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameLoop")
	int32 Score = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameLoop")
	int32 StartDelay = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameLoop")
	int32 GamePeriod = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TankFullReloadTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TankCurrentReloadTime;

	bool IsFinished = false;
};

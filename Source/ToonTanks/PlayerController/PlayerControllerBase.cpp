#include "PlayerControllerBase.h"
#include "ToonTanks/Gamemode/TankGamemodeBase.h"
#include "Kismet/GameplayStatics.h"

APlayerControllerBase::APlayerControllerBase()
{
	GamemodeRef = Cast<ATankGamemodeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

void APlayerControllerBase::SetPlayerEnabledState(bool SetPlayerEnabled)
{
	if (SetPlayerEnabled)
	{
		GetPawn()->EnableInput(this);
		if (GamemodeRef)
		{
			GamemodeRef->ShowScore();
			GamemodeRef->FirstShowTimer();
			GamemodeRef->ShowTimer();
			GamemodeRef->ShowHealthBar();
			GamemodeRef->ShowReloadBar();
		}
	}
	else if (!SetPlayerEnabled)
	{
		GetPawn()->DisableInput(this);
	}

	bShowMouseCursor = SetPlayerEnabled;
}
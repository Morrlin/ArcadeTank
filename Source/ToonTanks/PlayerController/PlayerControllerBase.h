#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

class ATankGamemodeBase;

UCLASS()
class TOONTANKS_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
public:

	void SetPlayerEnabledState(bool SetPlayerEnabled);

private:

	APlayerControllerBase();

	ATankGamemodeBase* GamemodeRef;
};

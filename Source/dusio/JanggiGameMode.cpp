#include "JanggiGameMode.h"

#include "JanggiPlayerController.h"

AJanggiGameMode::AJanggiGameMode()
{
	PlayerControllerClass = AJanggiPlayerController::StaticClass();
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
	SpectatorClass = nullptr;
}

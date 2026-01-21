#include "BoomrGameModeBase.h"
#include "BoomrHUD.h"
#include "BooPlayerController.h"

ABoomrGameModeBase::ABoomrGameModeBase()
{
	HUDClass = ABoomrHUD::StaticClass();
	PlayerControllerClass = ABooPlayerController::StaticClass();
}

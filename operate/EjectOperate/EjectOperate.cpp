#include "EjectOperate.h"
#include "../../player/Player.h"

void EjectOperate::execute(Player& player)
{
    player.eject();
}
#include "MergeOperate.h"
#include "../../player/Player.h"

void MergeOperate::execute(Player& player)
{
    player.merge();
}
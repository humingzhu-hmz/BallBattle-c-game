#include "OperateManager.h"

#include "../SplitOperate/SplitOperate.h"
#include "../EjectOperate/EjectOperate.h"
#include "../MergeOperate/MergeOperate.h"

void OperateManager::split(Player& player)
{
    SplitOperate op;
    op.execute(player);
}

void OperateManager::eject(Player& player)
{
    EjectOperate op;
    op.execute(player);
}

void OperateManager::merge(Player& player)
{
    MergeOperate op;
    op.execute(player);
}
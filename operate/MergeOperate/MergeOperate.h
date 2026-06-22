#pragma once

#include "../ioperate.h"

class MergeOperate : public IOperate
{
public:
    void execute(Player& player) override;
};
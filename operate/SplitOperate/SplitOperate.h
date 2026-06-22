#pragma once

#include "../ioperate.h"

class SplitOperate : public IOperate
{
public:
    void execute(Player& player) override;
};
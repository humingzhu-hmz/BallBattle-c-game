#pragma once

#include "../ioperate.h"

class EjectOperate : public IOperate
{
public:
    void execute(Player& player) override;
};
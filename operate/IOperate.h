#pragma once

class Player;

class IOperate
{
public:
    virtual ~IOperate() = default;

    virtual void execute(Player& player) = 0;
};
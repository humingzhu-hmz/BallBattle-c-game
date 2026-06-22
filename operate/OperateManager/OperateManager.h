#pragma once

class Player;

class OperateManager
{
public:

    static void split(Player& player);

    static void eject(Player& player);

    static void merge(Player& player);
};
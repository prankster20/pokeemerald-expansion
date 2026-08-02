#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Abrasive increases poison damage suffered by foes by fifty percent")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_ABRASIVE});
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: 30);
    } THEN {
        EXPECT_EQ(player->hp, 130);
    }
}

SINGLE_BATTLE_TEST("pranks Abrasive increases burn damage suffered by foes by fifty percent")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_ABRASIVE});
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: 15);
    } THEN {
        EXPECT_EQ(player->hp, 145);
    }
}

DOUBLE_BATTLE_TEST("pranks Abrasive does not increase indirect damage suffered by its ally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MIENFOO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MIENFOO) { MaxHP(160); HP(160); Status1(STATUS1_POISON); }
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_ABRASIVE});
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(opponentRight, damage: 20);
    }
}

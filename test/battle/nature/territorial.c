#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Territorial damages a foe that switches out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(120); HP(120); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_TERRITORIAL);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        HP_BAR(player, damage: 10);
    } THEN {
        EXPECT_EQ(player->hp, 110);
    }
}

SINGLE_BATTLE_TEST("pranks Territorial does not damage its ally switching out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(120); HP(120); }
        OPPONENT(SPECIES_WYNAUT);
        SetTestNature(&OPPONENT_PARTY[0], NATURE_TERRITORIAL);
    } WHEN {
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(GetMonData(&OPPONENT_PARTY[0], MON_DATA_HP), 120);
    }
}

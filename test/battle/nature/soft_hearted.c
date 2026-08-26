#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Soft-Hearted loses one-twelfth max HP after knocking out a Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); MaxHP(120); HP(120); Attack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(1); HP(1); Defense(1); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_SOFT_HEARTED);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player, damage: 10);
    } THEN {
        EXPECT_EQ(player->hp, 110);
    }
}

TEST("pranks Soft-Hearted boosts Special Defense by ten percent")
{
    EXPECT_EQ(ModifyStatByNature(NATURE_SOFT_HEARTED, 200, STAT_SPDEF, 0), 220);
    EXPECT_EQ(ModifyStatByNature(NATURE_SOFT_HEARTED, 200, STAT_DEF, 0), 200);
}

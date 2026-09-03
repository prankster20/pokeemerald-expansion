#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_STRING_SHOT) == EFFECT_STAT_CHANGE);
}

SINGLE_BATTLE_TEST("pranks Energetic's Speed cannot be lowered")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_STRING_SHOT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_OLD_ENERGETIC;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_STRING_SHOT); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_GROWL) == EFFECT_STAT_CHANGE);
}

SINGLE_BATTLE_TEST("Hardy's stats can be lowered to -1 normally")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_HARDY;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); }
    } SCENE {
        MESSAGE("Wobbuffet's Attack fell!");
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Hardy cannot have a stat lowered past -1")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_HARDY;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); }
        TURN { MOVE(player, MOVE_GROWL); }
    } SCENE {
        MESSAGE("Wobbuffet's Attack fell!");
        ABILITY_POPUP(opponent);
        MESSAGE("Wobbuffet's Attack won't go any lower!");
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_OPPONENT_LEFT].statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

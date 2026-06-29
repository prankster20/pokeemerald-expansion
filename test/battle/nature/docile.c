#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_CONFUSE_RAY) == EFFECT_CONFUSE);
    ASSUME(GetMoveEffect(MOVE_ATTRACT) == EFFECT_ATTRACT);
}

SINGLE_BATTLE_TEST("Docile prevents Confusion")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CONFUSE_RAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_DOCILE;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); }
    } SCENE {
        ABILITY_POPUP(opponent);
    } THEN {
        u32 confusionTurns = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.confusionTurns;
        EXPECT_EQ(confusionTurns, 0);
    }
}

SINGLE_BATTLE_TEST("Docile prevents Infatuation")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_ATTRACT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_DOCILE;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); }
    } SCENE {
        ABILITY_POPUP(opponent);
    } THEN {
        u32 infatuation = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.infatuation;
        EXPECT_EQ(infatuation, 0);
    }
}

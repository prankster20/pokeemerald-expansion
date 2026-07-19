#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_CONFUSE_RAY) == EFFECT_CONFUSE);
    ASSUME(GetMoveEffect(MOVE_ATTRACT) == EFFECT_ATTRACT);
}

SINGLE_BATTLE_TEST("pranks Docile does not prevent Confusion")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CONFUSE_RAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_DOCILE;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); }
    } THEN {
        u32 confusionTurns = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.confusionTurns;
        EXPECT(confusionTurns > 0);
    }
}

SINGLE_BATTLE_TEST("pranks Docile does not prevent Infatuation")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_ATTRACT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_DOCILE;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); }
    } THEN {
        u32 infatuation = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.infatuation;
        EXPECT(infatuation != 0);
    }
}

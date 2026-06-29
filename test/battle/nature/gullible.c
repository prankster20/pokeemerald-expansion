#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("Gullible makes Taunt last one additional turn")
{
    u32 turn;

    GIVEN {
        WITH_CONFIG(B_TAUNT_TURNS, GEN_5);
        ASSUME(GetMoveEffect(MOVE_TAUNT) == EFFECT_TAUNT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_GROWL, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_TAUNT, MOVE_CELEBRATE); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_GULLIBLE);
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_TAUNT); }
        for (turn = 0; turn < 4; turn++)
            TURN { MOVE(player, MOVE_GROWL, allowed: FALSE); MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    }
}

SINGLE_BATTLE_TEST("Gullible makes fixed-duration confusion last one additional turn")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_CONFUSE_RAY) == EFFECT_CONFUSE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CONFUSE_RAY); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_GULLIBLE);
    } WHEN {
        TURN { MOVE(opponent, MOVE_CONFUSE_RAY, WITH_RNG(RNG_CONFUSION_TURNS, B_CONFUSION_TURNS - 1)); MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_CONFUSION, FALSE)); }
    } THEN {
        EXPECT_EQ((u32)player->volatiles.confusionTurns, B_CONFUSION_TURNS);
    }
}

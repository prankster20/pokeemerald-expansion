#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Ambient extends newly-created weather by one turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_AMBIENT);
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
    } THEN {
        // Rain Dance starts at 5, Ambient raises it to 6, and end-turn
        // processing consumes one turn.
        EXPECT_EQ(gBattleStruct->weatherDuration, 5);
    }
}

SINGLE_BATTLE_TEST("pranks Ambient extends newly-created terrain by one turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ELECTRIC_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_AMBIENT);
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRIC_TERRAIN); }
    } THEN {
        EXPECT_EQ(gFieldTimers.terrainTimer, 5);
    }
}

SINGLE_BATTLE_TEST("pranks Ambient extends newly-created Trick Room by one turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TRICK_ROOM); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_AMBIENT);
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK_ROOM); }
    } THEN {
        EXPECT_EQ(gFieldTimers.trickRoomTimer, 5);
    }
}

SINGLE_BATTLE_TEST("pranks Ambient extends newly-created Reflect by one turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_REFLECT); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_AMBIENT);
    } WHEN {
        TURN { MOVE(player, MOVE_REFLECT); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].reflectTimer, 5);
    }
}

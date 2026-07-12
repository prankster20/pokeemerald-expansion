#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("Rugged fully ignores entry hazards")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_STICKY_WEB, MOVE_TOXIC_SPIKES); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
        SetTestNature(&OPPONENT_PARTY[1], NATURE_RUGGED);
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        NOT HP_BAR(opponent);
        NOT STATUS_ICON(opponent, poison: TRUE);
        NOT MESSAGE("The opposing Wynaut was caught in a sticky web!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Rugged Poison-types still absorb Toxic Spikes")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_EKANS, 0) == TYPE_POISON);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC_SPIKES); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_EKANS);
        SetTestNature(&OPPONENT_PARTY[1], NATURE_RUGGED);
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        MESSAGE("The poison spikes disappeared from the ground around the opposing team!");
        NOT STATUS_ICON(opponent, poison: TRUE);
    } THEN {
        EXPECT(!IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_TOXIC_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Rugged does not ignore passive Sandstorm damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_RUGGED);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SANDSTORM); }
    } SCENE {
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, 160);
    }
}

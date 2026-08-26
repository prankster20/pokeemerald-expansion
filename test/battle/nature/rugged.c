#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Rugged fully ignores entry hazards")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
        SetTestNature(&OPPONENT_PARTY[1], NATURE_RUGGED);
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("pranks Rugged Poison-types still absorb Toxic Spikes")
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

SINGLE_BATTLE_TEST("pranks Rugged does not ignore passive Sandstorm damage")
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

SINGLE_BATTLE_TEST("pranks Rugged ignores move recoil")
{
    GIVEN {
        ASSUME(GetMoveRecoil(MOVE_DOUBLE_EDGE) == 33);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DOUBLE_EDGE); MaxHP(200); HP(200); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_RUGGED);
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_EDGE); }
    } SCENE {
        HP_BAR(opponent);
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 200);
    }
}

SINGLE_BATTLE_TEST("pranks Rugged ignores crash damage")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_JUMP_KICK) == EFFECT_RECOIL_IF_MISS);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_JUMP_KICK); MaxHP(200); HP(200); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_RUGGED);
    } WHEN {
        TURN { MOVE(player, MOVE_JUMP_KICK, hit: FALSE); }
    } SCENE {
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 200);
    }
}

#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Stubborn prevents Knock Off from removing its held item")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_KNOCK_OFF); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_OLD_STUBBORN);
    } WHEN {
        TURN { MOVE(player, MOVE_KNOCK_OFF); }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_LEFTOVERS);
    }
}

SINGLE_BATTLE_TEST("pranks Stubborn prevents Roar from forcing it out")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_ROAR) == EFFECT_ROAR);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ROAR); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
        SetTestNature(&OPPONENT_PARTY[0], NATURE_OLD_STUBBORN);
    } WHEN {
        TURN { MOVE(player, MOVE_ROAR); }
    } SCENE {
        MESSAGE("Wobbuffet used Roar!");
        MESSAGE("But it failed!");
        NOT MESSAGE("The opposing Wynaut was dragged out!");
    }
}

SINGLE_BATTLE_TEST("pranks Unforgiving guarantees its next critical hit after surviving one")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_FROST_BREATH) == EFFECT_ALWAYS_CRITICAL_HIT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_SCRATCH); MaxHP(999); HP(999); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FROST_BREATH, MOVE_CELEBRATE); Speed(50); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_UNFORGIVING);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FROST_BREATH); }
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("A critical hit!");
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("pranks Unforgiving does not arm if the critical hit knocks it out")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_FROST_BREATH) == EFFECT_ALWAYS_CRITICAL_HIT);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(1); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FROST_BREATH); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_UNFORGIVING);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FROST_BREATH); }
    } THEN {
        EXPECT(!gBattleStruct->battlerState[B_BATTLER_0].unforgivingCritReady);
    }
}

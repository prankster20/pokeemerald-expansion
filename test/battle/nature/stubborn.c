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
        SetTestNature(&OPPONENT_PARTY[0], NATURE_STUBBORN);
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
        SetTestNature(&OPPONENT_PARTY[0], NATURE_STUBBORN);
    } WHEN {
        TURN { MOVE(player, MOVE_ROAR); }
    } SCENE {
        MESSAGE("Wobbuffet used Roar!");
        MESSAGE("But it failed!");
        NOT MESSAGE("The opposing Wynaut was dragged out!");
    }
}

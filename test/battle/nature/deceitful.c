#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Deceitful activates a held Weakness Policy after neutral damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_WEAKNESS_POLICY); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_DECEITFUL);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("pranks Deceitful does not provide a Weakness Policy effect without holding one")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&OPPONENT_PARTY[0], NATURE_DECEITFUL);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("pranks Deceitful activates a held Room Service without Trick Room")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ROOM_SERVICE); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_DECEITFUL);
    } WHEN {
        TURN { }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("pranks Deceitful activates a held Blunder Policy even when its move succeeds")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Item(ITEM_BLUNDER_POLICY); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_DECEITFUL);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("pranks Deceitful activates a held Adrenaline Orb without Intimidate")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ADRENALINE_ORB); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_DECEITFUL);
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

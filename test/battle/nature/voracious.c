#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Voracious boosts Leftovers healing by fifty percent")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); MaxHP(160); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_VORACIOUS);
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: -15);
    } THEN {
        EXPECT_EQ(player->hp, 16);
    }
}

SINGLE_BATTLE_TEST("pranks Voracious does not boost Black Sludge healing")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_GRIMER, 0) == TYPE_POISON);
        PLAYER(SPECIES_GRIMER) { Item(ITEM_BLACK_SLUDGE); MaxHP(160); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_VORACIOUS);
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: -10);
    } THEN {
        EXPECT_EQ(player->hp, 11);
    }
}

#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Voracious boosts Leftovers healing by twenty percent")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); MaxHP(160); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_VORACIOUS);
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: -12);
    } THEN {
        EXPECT_EQ(player->hp, 13);
    }
}

SINGLE_BATTLE_TEST("pranks Voracious boosts Black Sludge healing by twenty percent")
{
    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_GRIMER, 0) == TYPE_POISON);
        PLAYER(SPECIES_GRIMER) { Item(ITEM_BLACK_SLUDGE); MaxHP(160); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_VORACIOUS);
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: -12);
    } THEN {
        EXPECT_EQ(player->hp, 13);
    }
}

SINGLE_BATTLE_TEST("pranks Voracious consumes pinch Berries at half HP")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_LIECHI_BERRY].holdEffect == HOLD_EFFECT_ATTACK_UP);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LIECHI_BERRY); MaxHP(80); HP(80); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_VORACIOUS);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

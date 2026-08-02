#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Resourceful makes Leftovers twenty-five percent more effective")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); MaxHP(160); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_RESOURCEFUL});
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: -12);
    } THEN {
        EXPECT_EQ(player->hp, 13);
    }
}

SINGLE_BATTLE_TEST("pranks Resourceful makes healing Berries twenty-five percent more effective")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_ORAN_BERRY].holdEffect == HOLD_EFFECT_RESTORE_HP);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ORAN_BERRY); MaxHP(100); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_RESOURCEFUL});
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: -12);
    } THEN {
        EXPECT_EQ(player->hp, 13);
    }
}

SINGLE_BATTLE_TEST("pranks Resourceful makes Shell Bell twenty-five percent more effective")
{
    GIVEN {
        ASSUME(gItemsInfo[ITEM_SHELL_BELL].holdEffect == HOLD_EFFECT_SHELL_BELL);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_SHELL_BELL); MaxHP(100); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(100); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_RESOURCEFUL});
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(opponent, damage: 40);
        HP_BAR(player, damage: -6);
    } THEN {
        EXPECT_EQ(player->hp, 7);
    }
}

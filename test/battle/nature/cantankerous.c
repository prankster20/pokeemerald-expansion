#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Cantankerous damages every active Pokemon on entry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_CANTANKEROUS);
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->hp, 150);
        EXPECT_EQ(opponent->hp, 150);
    }
}

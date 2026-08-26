#include "global.h"
#include "money.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Greedy steals a foe's held item after attacking")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_GREEDY});
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("pranks Greedy increases trainer prize money by five percent", u32 money)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_HARDY; }
    PARAMETRIZE { nature = NATURE_GREEDY; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Attack(999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Defense(1); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        SetMoney(&gSaveBlock1Ptr->money, 0);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    } THEN {
        results[i].money = GetMoney(&gSaveBlock1Ptr->money);
    } FINALLY {
        EXPECT_EQ(results[1].money, results[0].money * 105 / 100);
    }
}

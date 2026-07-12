#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItemsInfo[ITEM_LIFE_ORB].holdEffect == HOLD_EFFECT_LIFE_ORB);
}

SINGLE_BATTLE_TEST("Finicky prevents held item effects")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LIFE_ORB); }
        OPPONENT(SPECIES_WOBBUFFET);
        u32 nature = NATURE_FINICKY;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_POUND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, player);
        HP_BAR(opponent);
        NONE_OF {
            HP_BAR(player);
            MESSAGE("Wobbuffet was hurt by the Life Orb!");
        }
    }
}

SINGLE_BATTLE_TEST("Finicky cannot use Fling")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_IRON_BALL); Moves(MOVE_FLING); }
        OPPONENT(SPECIES_WOBBUFFET);
        u32 nature = NATURE_FINICKY;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_FLING); }
    } SCENE {
        // NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        // NOT HP_BAR(opponent);
        NONE_OF {
            MESSAGE("Mienfoo used Fling!");
            HP_BAR(opponent);
        }
    }
}

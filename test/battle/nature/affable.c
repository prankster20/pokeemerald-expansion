#include "global.h"
#include "test/battle.h"

WILD_BATTLE_TEST("pranks Affable fully heals a newly caught Pokemon and sets its Friendship to 150")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE) { HP(1); Status1(STATUS1_POISON); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_AFFABLE});
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_HARDY});
    } WHEN {
        TURN { USE_ITEM(player, ITEM_POKE_BALL, WITH_RNG(RNG_BALLTHROW_SHAKE, 0)); }
    } SCENE {
        ONE_OF {
            ANIMATION(ANIM_TYPE_SPECIAL, B_ANIM_CRITICAL_CAPTURE_THROW);
            ANIMATION(ANIM_TYPE_SPECIAL, B_ANIM_BALL_THROW);
        }
    } THEN {
        struct Pokemon *caughtMon = &gParties[B_TRAINER_PLAYER][1];

        EXPECT_EQ(GetMonData(caughtMon, MON_DATA_HP), GetMonData(caughtMon, MON_DATA_MAX_HP));
        EXPECT_EQ(GetMonData(caughtMon, MON_DATA_STATUS), STATUS1_NONE);
        EXPECT_EQ(GetMonData(caughtMon, MON_DATA_FRIENDSHIP), 150);
    }
}

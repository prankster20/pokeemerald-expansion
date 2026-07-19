#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Perfectionist always acts last regardless of Speed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(999); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_PERFECTIONIST);
    } WHEN {
        TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("pranks Perfectionist makes inaccurate moves hit")
{
    GIVEN {
        ASSUME(GetMoveAccuracy(MOVE_ZAP_CANNON) < 100);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ZAP_CANNON); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_PERFECTIONIST);
    } WHEN {
        TURN { MOVE(player, MOVE_ZAP_CANNON); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, player);
        HP_BAR(opponent);
    }
}

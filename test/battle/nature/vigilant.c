#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Vigilant reduces a targeting priority move to neutral priority")
{
    GIVEN {
        ASSUME(GetMovePriority(MOVE_QUICK_ATTACK) == 1);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_QUICK_ATTACK); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(200); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_VIGILANT);
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
    }
}

DOUBLE_BATTLE_TEST("pranks Vigilant protects its ally from elevated priority")
{
    GIVEN {
        ASSUME(GetMovePriority(MOVE_QUICK_ATTACK) == 1);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_QUICK_ATTACK); Speed(100); }
        PLAYER(SPECIES_WYNAUT) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(200); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); Speed(50); }
        SetTestNature(&OPPONENT_PARTY[1], NATURE_VIGILANT);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft); MOVE(opponentLeft, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, playerLeft);
    }
}

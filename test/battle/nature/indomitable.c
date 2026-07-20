#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Indomitable completes its selected move before fainting")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Attack(999); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(1); Moves(MOVE_CELEBRATE); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_INDOMITABLE);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        MESSAGE("The opposing Wobbuffet fainted!");
    }
}

SINGLE_BATTLE_TEST("pranks Indomitable does not save a Pokemon after it has acted")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Attack(999); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1); Moves(MOVE_CELEBRATE); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_INDOMITABLE);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        MESSAGE("The opposing Wobbuffet fainted!");
    }
}

#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Cowardly Wimps Out only when crossing one-quarter HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUPER_FANG); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(100); }
        OPPONENT(SPECIES_WYNAUT);
        SetTestNature(&OPPONENT_PARTY[0], NATURE_COWARDLY);
    } WHEN {
        TURN { MOVE(player, MOVE_SUPER_FANG); }
        TURN { MOVE(player, MOVE_SUPER_FANG); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, player);
        HP_BAR(opponent);
        ABILITY_POPUP(opponent);
    }
}

#include "global.h"
#include "test/battle.h"

// Custom Natures are stored in MON_DATA_HIDDEN_NATURE.  This helper avoids
// relying on the test runner's vanilla-only Nature(...) personality table.
static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Benevolent boosts Recover healing immediately on the HP bar")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_RECOVER) == EFFECT_RESTORE_HP);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(100); HP(1); Speed(2); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_BENEVOLENT);
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        HP_BAR(player, damage: -60);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Lazy recovers 1/8 max HP after losing a turn to sleep")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(100); HP(50); Status1(STATUS1_SLEEP_TURN(2)); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_LAZY);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        s32 maxHP = GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP);
        MESSAGE("Wobbuffet is fast asleep.");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        HP_BAR(player, damage: -maxHP / 8);
    }
}

SINGLE_BATTLE_TEST("pranks Lazy recovers 1/8 max HP after losing a turn to flinching")
{
    GIVEN {
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_FAKE_OUT, MOVE_EFFECT_FLINCH, 100));
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(100); HP(50); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_FAKE_OUT); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_LAZY);
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        s32 maxHP = GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponent);
        MESSAGE("Wobbuffet flinched and couldn't move!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        HP_BAR(player, damage: -maxHP / 8);
    }
}

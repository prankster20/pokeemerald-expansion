#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Leech restores 25 percent of the damage dealt")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LEECH); MaxHP(100); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, player);
        HP_BAR(opponent, damage: 40);
        ABILITY_POPUP(player, ABILITY_LEECH);
        HP_BAR(player, damage: -10);
    }
}

SINGLE_BATTLE_TEST("Leech and Shell Bell restore HP independently")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_DRAGON_RAGE) == 40);
        ASSUME(gItemsInfo[ITEM_SHELL_BELL].holdEffect == HOLD_EFFECT_SHELL_BELL);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LEECH); Item(ITEM_SHELL_BELL); MaxHP(100); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, player);
        HP_BAR(opponent, damage: 40);
        ABILITY_POPUP(player, ABILITY_LEECH);
        HP_BAR(player, damage: -10);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        HP_BAR(player, damage: -5);
    }
}

SINGLE_BATTLE_TEST("Leech activates in addition to a draining move")
{
    s16 damage;
    s16 drainHeal;
    s16 leechHeal;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LEECH); MaxHP(200); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAIN_PUNCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, player);
        HP_BAR(opponent, captureDamage: &damage);
        HP_BAR(player, captureDamage: &drainHeal);
        ABILITY_POPUP(player, ABILITY_LEECH);
        HP_BAR(player, captureDamage: &leechHeal);
    } THEN {
        EXPECT_EQ(-damage / 2, drainHeal);
        EXPECT_EQ(-damage / 4, leechHeal);
    }
}

SINGLE_BATTLE_TEST("Leech does not activate if the move deals no damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LEECH); MaxHP(100); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_FALSE_SWIPE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FALSE_SWIPE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_LEECH);
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Heal Block prevents Leech recovery")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_LEECH); MaxHP(100); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_LEECH);
            HP_BAR(player);
        }
    }
}

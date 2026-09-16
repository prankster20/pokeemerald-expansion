#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Esoteric uses Sp. Def for special attacks", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_ESOTERIC; }

    GIVEN {
        ASSUME(GetMoveCategory(MOVE_CONFUSION) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); SpAttack(50); SpDefense(200); Moves(MOVE_CONFUSION); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSION); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Esoteric uses the Wonder Room-adjusted Sp. Def for special attacks", s16 damage)
{
    enum Move setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_WONDER_ROOM; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_WONDER_ROOM) == EFFECT_WONDER_ROOM);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_ESOTERIC); SpAttack(50); Defense(50); SpDefense(200); Moves(MOVE_CONFUSION, MOVE_WONDER_ROOM); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_CONFUSION); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}
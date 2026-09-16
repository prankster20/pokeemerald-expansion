#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Radiance boosts Light-based moves by 30%", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_RADIANCE; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_LIGHT_OF_RUIN) != TYPE_NONE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_LIGHT_OF_RUIN); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_LIGHT_OF_RUIN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.3), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Radiance halves damage from Light-based moves", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_RADIANCE; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_LIGHT_OF_RUIN) != TYPE_NONE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_LIGHT_OF_RUIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); MaxHP(999); HP(999); }
    } WHEN {
        TURN { MOVE(player, MOVE_LIGHT_OF_RUIN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(0.5), results[0].damage);
    }
}
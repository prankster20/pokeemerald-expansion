#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetSpeciesType(SPECIES_TANGELA, 0) == TYPE_GRASS);
    ASSUME(GetSpeciesType(SPECIES_TANGELA, 1) == TYPE_GRASS);
    ASSUME(GetSpeciesType(SPECIES_SQUIRTLE, 0) == TYPE_WATER);
    ASSUME(GetSpeciesType(SPECIES_SQUIRTLE, 1) == TYPE_WATER);
}

SINGLE_BATTLE_TEST("pranks Zealous makes a primary-type weakness 2.4-times effective", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_ZEALOUS; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); SpAttack(100); }
        OPPONENT(SPECIES_TANGELA) { Level(50); SpDefense(100); MaxHP(999); HP(999); }
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Zealous makes a primary-type resistance 0.4-times effective", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_ZEALOUS; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); SpAttack(100); }
        OPPONENT(SPECIES_SQUIRTLE) { Level(50); SpDefense(100); MaxHP(999); HP(999); }
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.8), results[1].damage);
    }
}

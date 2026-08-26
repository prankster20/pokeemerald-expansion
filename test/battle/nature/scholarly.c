#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Scholarly boosts damaging moves outside the species level-up line", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_SCHOLARLY; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); SpDefense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
    }
}

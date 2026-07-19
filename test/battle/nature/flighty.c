#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_U_TURN) == EFFECT_HIT_ESCAPE);
}

SINGLE_BATTLE_TEST("pranks Flighty boosts damaging pivot moves by 10%", s16 damage)
{
    u32 nature;

    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_FLIGHTY; }

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Attack(100); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); SEND_OUT(player, 1); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
    }
}

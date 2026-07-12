#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMovePower(MOVE_SCRATCH) == GetMovePower(MOVE_POUND));
    ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
    ASSUME(GetMoveCategory(MOVE_POUND) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("Tactical boosts the base power of only move slot 1", s16 damage)
{
    u32 nature;
    enum Move move;

    PARAMETRIZE { nature = NATURE_HARDY;      move = MOVE_SCRATCH; }
    PARAMETRIZE { nature = NATURE_TACTICAL; move = MOVE_SCRATCH; }
    PARAMETRIZE { nature = NATURE_TACTICAL; move = MOVE_POUND; }

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_SCRATCH, MOVE_POUND); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Defense(50); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

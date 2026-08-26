#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Bad-Tempered boosts the next damaging move after a move fails", s16 damage)
{
    u32 nature;
    PARAMETRIZE(nature = NATURE_HARDY);
    PARAMETRIZE(nature = NATURE_BAD_TEMPERED);

    GIVEN {
        ASSUME(GetSpeciesType(SPECIES_SANDSHREW, 0) == TYPE_GROUND);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_THUNDER_WAVE, MOVE_TACKLE); Attack(100); }
        OPPONENT(SPECIES_SANDSHREW) { Moves(MOVE_CELEBRATE); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Bad-Tempered does not boost damage after a successful move", s16 damage)
{
    u32 nature;
    PARAMETRIZE(nature = NATURE_HARDY);
    PARAMETRIZE(nature = NATURE_BAD_TEMPERED);

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

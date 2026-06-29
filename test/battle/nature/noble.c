#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

WILD_BATTLE_TEST("Noble always spares a wild Pokemon from fainting, once per battle")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Attack(999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Defense(1); }
        u32 nature = NATURE_NOBLE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("Wobbuffet fainted!");
        ABILITY_POPUP(player);
        HP_BAR(opponent, hp: 1);
    }
}

SINGLE_BATTLE_TEST("Noble's mercy does not apply against trainer battles")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Attack(999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Defense(1); }
        u32 nature = NATURE_NOBLE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Wobbuffet fainted!");
    }
}

SINGLE_BATTLE_TEST("Noble boosts Defense by 10%", s16 damage)
{
    u32 nature;
    PARAMETRIZE(nature = NATURE_HARDY); // neutral baseline
    PARAMETRIZE(nature = NATURE_NOBLE);
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Level(50); Defense(50); MaxHP(999); HP(999); }
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.1), results[0].damage);
    }
}

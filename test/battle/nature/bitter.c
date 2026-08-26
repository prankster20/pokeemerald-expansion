#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Bitter boosts its next damaging move after a foe heals with a move", s16 damage)
{
    u32 nature;
    PARAMETRIZE(nature = NATURE_HARDY);
    PARAMETRIZE(nature = NATURE_BITTER);

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); Attack(100); Speed(200); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RECOVER, MOVE_CELEBRATE); Defense(100); Speed(100); MaxHP(200); HP(100); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(opponent, MOVE_RECOVER); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, damage: -100);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Bitter is not charged by held-item healing", s16 damage)
{
    u32 nature;
    PARAMETRIZE(nature = NATURE_HARDY);
    PARAMETRIZE(nature = NATURE_BITTER);

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Item(ITEM_LEFTOVERS); Defense(100); MaxHP(160); HP(80); }
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

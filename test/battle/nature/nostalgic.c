#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Nostalgic boosts moves in its first two slots by thirty percent", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_HARDY; }
    PARAMETRIZE { nature = NATURE_NOSTALGIC; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
    }
}

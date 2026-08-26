#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Shortsighted deals twenty percent more damage on its first turn", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_SHORTSIGHTED; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[1], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[1].damage);
    }
}

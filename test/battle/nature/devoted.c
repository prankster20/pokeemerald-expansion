#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("pranks Devoted boosts own Atk by 6% in battle when closely bonded", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_DEVOTED; }

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); }
        PLAYER(SPECIES_WYNAUT) { Level(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        u32 personality = GetMonData(&PLAYER_PARTY[0], MON_DATA_PERSONALITY);
        SetMonData(&PLAYER_PARTY[1], MON_DATA_PERSONALITY, &personality);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.06), results[1].damage);
    }
}

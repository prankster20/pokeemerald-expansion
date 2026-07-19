#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("pranks Arrogant boosts damage dealt by 10% vs higher-level targets", s16 damage)
{
    u32 level;
    PARAMETRIZE(level = 40);
    PARAMETRIZE(level = 50);
    PARAMETRIZE(level = 100);

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_SCRATCH); Level(50); Speed(2); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Level(level); Speed(1); Defense(50); MaxHP(100); HP(100); }
        u32 nature = NATURE_ARROGANT;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (level > 50) {
            ABILITY_POPUP(player);
            MESSAGE("Mienfoo is feeling rather confident…");
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[2].damage);
    }
}

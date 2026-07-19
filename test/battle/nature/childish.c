#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("pranks Childish deals 10% less damage", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; } // neutral baseline
    PARAMETRIZE { nature = NATURE_CHILDISH; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.9), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Childish gains 20% more EXP", u32 exp)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_CHILDISH; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(5); Attack(999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Defense(1); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    } THEN {
        results[i].exp = GetMonData(&PLAYER_PARTY[0], MON_DATA_EXP);
    } FINALLY {
        u32 startingExp = gExperienceTables[gSpeciesInfo[SPECIES_MIENFOO].growthRate][5];
        u32 docileGain = results[0].exp - startingExp;
        u32 childishGain = results[1].exp - startingExp;
        EXPECT_MUL_EQ(docileGain, Q_4_12(1.2), childishGain);
    }
}

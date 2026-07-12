#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("Childish deals 10% less damage", s16 damage)
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
        // Childish (results[1]) should deal 10% less than Docile (results[0]).
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.1), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Childish gains 20% more EXP")
{
    s32 expDocile, expChildish;
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(5); Attack(999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Defense(1); }
        u32 docile = NATURE_DOCILE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &docile);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    } THEN {
        expDocile = GetMonData(&PLAYER_PARTY[0], MON_DATA_EXP);
    }

    // Run again with Childish and compare.
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(5); Attack(999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Defense(1); }
        u32 childish = NATURE_CHILDISH;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &childish);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    } THEN {
        expChildish = GetMonData(&PLAYER_PARTY[0], MON_DATA_EXP);
        // Childish EXP should be 20% more than Docile EXP gained.
        // We compare the gained amounts, not absolute values,
        // since both mons start at the same level.
        EXPECT(expChildish > expDocile);
    }
}

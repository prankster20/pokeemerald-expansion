#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_SWORDS_DANCE) == EFFECT_STAT_CHANGE);
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("pranks Humble's stats can be raised to +1 normally")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_HUMBLE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("pranks Humble cannot have a stat raised past +1")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_HUMBLE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("pranks Humble boosts all non-HP stats by 5%", s16 damage)
{
    u32 nature;
    PARAMETRIZE(nature = NATURE_HARDY); // a no-op classic nature as the baseline
    PARAMETRIZE(nature = NATURE_HUMBLE);
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Level(50); Defense(50); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.05), results[1].damage);
    }
}

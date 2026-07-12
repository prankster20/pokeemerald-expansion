#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("Apathetic attackers ignore weather damage boosts", s16 damage)
{
    u32 setupMove;
    u32 nature;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; nature = NATURE_HARDY; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; nature = NATURE_HARDY; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; nature = NATURE_APATHETIC; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_WATER_GUN, setupMove); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(100); }
        SetTestNature(&PLAYER_PARTY[0], nature);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Apathetic attackers ignore weather damage reductions", s16 damage)
{
    u32 setupMove;
    u32 nature;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; nature = NATURE_HARDY; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; nature = NATURE_HARDY; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; nature = NATURE_APATHETIC; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); Moves(MOVE_EMBER, setupMove); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(100); }
        SetTestNature(&PLAYER_PARTY[0], nature);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_EMBER, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Apathetic targets never take Rising Voltage's grounded double damage", s16 damage)
{
    u32 setupMove;
    u32 nature;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; nature = NATURE_HARDY; }
    PARAMETRIZE { setupMove = MOVE_ELECTRIC_TERRAIN; nature = NATURE_HARDY; }
    PARAMETRIZE { setupMove = MOVE_ELECTRIC_TERRAIN; nature = NATURE_APATHETIC; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_RISING_VOLTAGE) == EFFECT_TERRAIN_BOOST);
        ASSUME(GetMoveTerrainBoost_Percent(MOVE_RISING_VOLTAGE) == 100);
        ASSUME(GetMoveTerrainBoost_GroundCheck(MOVE_RISING_VOLTAGE) == GROUND_CHECK_TARGET);
        PLAYER(SPECIES_PIDGEY) { SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(100); }
        SetTestNature(&OPPONENT_PARTY[0], nature);
    } WHEN {
        TURN { MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_RISING_VOLTAGE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Apathetic ignores passive Sandstorm damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_APATHETIC);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SANDSTORM); }
    } SCENE {
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 160);
    }
}

SINGLE_BATTLE_TEST("Apathetic ignores passive Hail damage")
{
    GIVEN {
        ASSUME(B_PREFERRED_ICE_WEATHER != B_ICE_WEATHER_SNOW);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HAIL); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_APATHETIC);
    } WHEN {
        TURN { MOVE(opponent, MOVE_HAIL); }
    } SCENE {
        NOT HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 160);
    }
}

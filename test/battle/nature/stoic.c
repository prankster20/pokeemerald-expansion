#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Stoic ignores Burn's physical damage penalty", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_STOIC; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Stoic ignores Frostbite's special damage penalty", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_STOIC; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); SpAttack(100); Status1(STATUS1_FROSTBITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); SpDefense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Stoic ignores the Speed penalty from Paralysis")
{
    GIVEN {
        WITH_CONFIG(B_PARALYSIS_SPEED, GEN_7);
        PLAYER(SPECIES_WOBBUFFET) { Speed(60); Status1(STATUS1_PARALYSIS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_STOIC});
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ONE_OF {
            MESSAGE("Wobbuffet used Celebrate!");
            MESSAGE("Wobbuffet is paralyzed, so it may be unable to move!");
        }
        MESSAGE("The opposing Wobbuffet used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("pranks Stoic is immune to Infatuation")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_ATTRACT) == EFFECT_ATTRACT);
        PLAYER(SPECIES_MIENFOO) { Gender(MON_MALE); Moves(MOVE_ATTRACT); }
        OPPONENT(SPECIES_WOBBUFFET) { Gender(MON_FEMALE); }
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_STOIC});
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); }
    } THEN {
        EXPECT(opponent->volatiles.infatuation == 0);
    }
}

TEST("pranks Stoic permanently lowers Speed by five percent")
{
    EXPECT_EQ(ModifyStatByNature(NATURE_STOIC, 200, STAT_SPEED, 0), 190);
    EXPECT_EQ(ModifyStatByNature(NATURE_STOIC, 200, STAT_ATK, 0), 200);
}

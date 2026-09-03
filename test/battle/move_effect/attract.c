#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_ATTRACT) == EFFECT_ATTRACT);
    ASSUME(gSpeciesInfo[SPECIES_NIDOKING].genderRatio == MON_MALE);
    ASSUME(gSpeciesInfo[SPECIES_NIDOQUEEN].genderRatio == MON_FEMALE);
}

SINGLE_BATTLE_TEST("Attract causes the target to become infatuated regardless of gender")
{
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN);
        OPPONENT(SPECIES_NIDOKING);
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
        MESSAGE("The opposing Nidoking fell in love!");
    } THEN {
        EXPECT(opponent->volatiles.infatuation);
    }
}

SINGLE_BATTLE_TEST("Attract ignores type immunity")
{
    GIVEN {
        ASSUME(GetMoveType(MOVE_ATTRACT) == TYPE_NORMAL);
        PLAYER(SPECIES_NIDOQUEEN);
        OPPONENT(SPECIES_MISDREAVUS) { Gender(MON_MALE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
        MESSAGE("The opposing Misdreavus fell in love!");
    } THEN {
        EXPECT(opponent->volatiles.infatuation);
    }
}

SINGLE_BATTLE_TEST("Attract bypasses Substitute")
{
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN) { Speed(90); }
        OPPONENT(SPECIES_NIDOKING) { Speed(100); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); }
        TURN { MOVE(player, MOVE_ATTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
        MESSAGE("The opposing Nidoking fell in love!");
    } THEN {
        EXPECT(opponent->volatiles.infatuation);
    }
}

SINGLE_BATTLE_TEST("Attract fails if the target is already infatuated")
{
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN);
        OPPONENT(SPECIES_NIDOKING);
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); }
        TURN { MOVE(player, MOVE_ATTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
        MESSAGE("The opposing Nidoking fell in love!");
        MESSAGE("Nidoqueen used Attract!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT(opponent->volatiles.infatuation);
    }
}

SINGLE_BATTLE_TEST("Attract works on a Pokémon of the same gender")
{
    GIVEN {
        PLAYER(SPECIES_NIDOQUEEN);
        OPPONENT(SPECIES_NIDOQUEEN);
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
        MESSAGE("The opposing Nidoqueen fell in love!");
    } THEN {
        EXPECT(opponent->volatiles.infatuation);
    }
}

SINGLE_BATTLE_TEST("Attract works on a genderless Pokémon")
{
    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_STARMIE].genderRatio == MON_GENDERLESS);
        PLAYER(SPECIES_NIDOQUEEN);
        OPPONENT(SPECIES_STARMIE);
    } WHEN {
        TURN { MOVE(player, MOVE_ATTRACT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
        MESSAGE("The opposing Starmie fell in love!");
    } THEN {
        EXPECT(opponent->volatiles.infatuation);
    }
}

SINGLE_BATTLE_TEST("Infatuation lowers physical and special damage by 25 percent without preventing actions", s16 damage)
{
    u32 move;
    bool32 infatuated;

    PARAMETRIZE { move = MOVE_SCRATCH; infatuated = FALSE; }
    PARAMETRIZE { move = MOVE_SCRATCH; infatuated = TRUE; }
    PARAMETRIZE { move = MOVE_SWIFT; infatuated = FALSE; }
    PARAMETRIZE { move = MOVE_SWIFT; infatuated = TRUE; }
    GIVEN {
        ASSUME(GetMoveCategory(MOVE_SCRATCH) == DAMAGE_CATEGORY_PHYSICAL);
        ASSUME(GetMoveCategory(MOVE_SWIFT) == DAMAGE_CATEGORY_SPECIAL);
        PLAYER(SPECIES_NIDOQUEEN) { Attack(100); SpAttack(100); }
        OPPONENT(SPECIES_NIDOKING) { Defense(100); SpDefense(100); }
    } WHEN {
        if (infatuated)
            TURN { MOVE(opponent, MOVE_ATTRACT); }
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        NONE_OF { MESSAGE("Nidoqueen is immobilized by love!"); }
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(0.75), results[3].damage);
    }
}

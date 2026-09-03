#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Cute Charm inflicts infatuation on contact")
{
    enum Move move;
    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SWIFT; }
    GIVEN {
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        ASSUME(!MoveMakesContact(MOVE_SWIFT));
        PLAYER(SPECIES_WOBBUFFET) { Gender(MON_MALE); }
        OPPONENT(SPECIES_CLEFAIRY) { Gender(MON_FEMALE); Ability(ABILITY_CUTE_CHARM); }
    } WHEN {
        TURN { MOVE(player, move); }
        TURN { MOVE(player, move); }
    } SCENE {
        if (MoveMakesContact(move)) {
            ABILITY_POPUP(opponent, ABILITY_CUTE_CHARM);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_INFATUATION, player);
            MESSAGE("Wobbuffet fell in love!");
            MESSAGE("Wobbuffet is in love with the opposing Clefairy!");
        } else {
            NONE_OF {
                ABILITY_POPUP(opponent, ABILITY_CUTE_CHARM);
                ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_INFATUATION, player);
                MESSAGE("Wobbuffet fell in love!");
                MESSAGE("Wobbuffet is in love with the opposing Clefairy!");
            }
        }
    }
}

SINGLE_BATTLE_TEST("Cute Charm can infatuate the same gender")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Gender(MON_MALE); }
        OPPONENT(SPECIES_CLEFAIRY) { Gender(MON_MALE); Ability(ABILITY_CUTE_CHARM); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        ABILITY_POPUP(opponent, ABILITY_CUTE_CHARM);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_INFATUATION, player);
        MESSAGE("Wobbuffet fell in love!");
    }
}

SINGLE_BATTLE_TEST("Cute Charm can infatuate a genderless Pokémon")
{
    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_STARMIE].genderRatio == MON_GENDERLESS);
        PLAYER(SPECIES_STARMIE);
        OPPONENT(SPECIES_CLEFAIRY) { Ability(ABILITY_CUTE_CHARM); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_CUTE_CHARM);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_INFATUATION, player);
        MESSAGE("Starmie fell in love!");
    }
}

SINGLE_BATTLE_TEST("Cute Charm triggers 1/3 times (Gen3) or 30% (Gen 4+) of the time")
{
    u32 config, passes, trials;
    PARAMETRIZE { config = GEN_3; passes = 1; trials = 3; }  // 33.3%
    PARAMETRIZE { config = GEN_4; passes = 3; trials = 10; } // 30%
    PASSES_RANDOMLY(passes, trials, RNG_CUTE_CHARM);
    GIVEN {
        WITH_CONFIG(B_ABILITY_TRIGGER_CHANCE, config);
        ASSUME(MoveMakesContact(MOVE_SCRATCH));
        PLAYER(SPECIES_WOBBUFFET) { Gender(MON_MALE); }
        OPPONENT(SPECIES_CLEFAIRY) { Gender(MON_FEMALE); Ability(ABILITY_CUTE_CHARM); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_CUTE_CHARM);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_INFATUATION, player);
        MESSAGE("Wobbuffet fell in love!");
        MESSAGE("Wobbuffet is in love with the opposing Clefairy!");
    }
}

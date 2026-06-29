#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
    ASSUME(GetMovePower(MOVE_THUNDERPUNCH) > 0);
}

SINGLE_BATTLE_TEST("Relentless deals 10% more move damage", s16 damage)
{
    u32 nature;
    PARAMETRIZE(nature = NATURE_HARDY); // neutral baseline
    PARAMETRIZE(nature = NATURE_RELENTLESS);
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Level(50); Defense(50); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Relentless adds 10 percentage points to secondary effect chance")
{
    // Thunder Punch has a 10% chance to paralyze - Relentless should push
    // that to 20%, so forcing the roll to the 11th-20th percentile range
    // should now succeed where it normally wouldn't.
    PASSES_RANDOMLY(20, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_THUNDERPUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_RELENTLESS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERPUNCH); }
    } SCENE {
        STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Relentless's moves cost 2 PP per use")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_RELENTLESS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(GetMonData(&PLAYER_PARTY[0], MON_DATA_PP1), GetMovePP(MOVE_TACKLE) - 2);
    }
}

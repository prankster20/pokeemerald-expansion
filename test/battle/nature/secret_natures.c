#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Cynical lowers an opposing thirty-percent secondary effect to ten percent")
{
    PASSES_RANDOMLY(10, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_BODY_SLAM, MOVE_EFFECT_PARALYSIS, 30));
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BODY_SLAM); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_CYNICAL);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BODY_SLAM); }
    } SCENE {
        STATUS_ICON(player, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("pranks Realistic raises a thirty-percent secondary effect to forty percent")
{
    PASSES_RANDOMLY(40, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        ASSUME(MoveHasAdditionalEffectWithChance(MOVE_BODY_SLAM, MOVE_EFFECT_PARALYSIS, 30));
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_BODY_SLAM); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&PLAYER_PARTY[0], NATURE_REALISTIC);
    } WHEN {
        TURN { MOVE(player, MOVE_BODY_SLAM); }
    } SCENE {
        STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("pranks Realistic damage rolls never enter the lower half of the normal range", s16 damage)
{
    u32 nature, lowestRoll;
    PARAMETRIZE { nature = NATURE_HARDY;     lowestRoll = 15; }
    PARAMETRIZE { nature = NATURE_REALISTIC; lowestRoll = 7; }

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_SCRATCH); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); MaxHP(999); HP(999); }
        SetTestNature(&PLAYER_PARTY[0], nature);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_DAMAGE_MODIFIER, lowestRoll)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Idealistic blocks critical hits made at the base critical-hit rate")
{
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&OPPONENT_PARTY[0], NATURE_IDEALISTIC);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_CRITICAL_HIT, 0)); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("pranks Idealistic does not block an elevated critical-hit rate")
{
    PASSES_RANDOMLY(1, 8, RNG_CRITICAL_HIT);
    GIVEN {
        WITH_CONFIG(B_CRIT_CHANCE, GEN_9);
        ASSUME(GetMoveCriticalHitStage(MOVE_SLASH) == 1);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_SLASH); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetTestNature(&OPPONENT_PARTY[0], NATURE_IDEALISTIC);
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

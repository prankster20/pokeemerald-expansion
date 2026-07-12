#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_RECOVER) == EFFECT_RESTORE_HP);
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

// --- BENEVOLENT: boosts healing for ALL mons while on field ---

SINGLE_BATTLE_TEST("Benevolent boosts Recover healing by 20% for its user", s16 healAmt)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_BENEVOLENT; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_RECOVER); MaxHP(100); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); }
    } SCENE {
        HP_BAR(player, captureHP: &results[i].healAmt);
    } FINALLY {
        // Benevolent heals 20% more: base Recover = 50 HP (on 100 max).
        // Docile: 50, Benevolent: 60.
        EXPECT_MUL_EQ(results[0].healAmt, Q_4_12(1.2), results[1].healAmt);
    }
}

SINGLE_BATTLE_TEST("Benevolent boosts opponent's Recover healing too")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RECOVER); MaxHP(100); HP(1); }
        u32 nature = NATURE_BENEVOLENT;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        // Opponent's heal is also boosted: should restore more than 50 HP
        HP_BAR(opponent, hp: 61); // 1 + 60 (50 base * 1.2)
    }
}

// --- DELICATE: 1.5x indirect damage + 1.3x self-healing ---

SINGLE_BATTLE_TEST("Delicate boosts own Recover healing by 30%", s16 healAmt)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_DELICATE; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_RECOVER); MaxHP(100); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); }
    } SCENE {
        HP_BAR(player, captureHP: &results[i].healAmt);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].healAmt, Q_4_12(1.3), results[1].healAmt);
    }
}

SINGLE_BATTLE_TEST("Delicate takes 1.5x damage from Spikes entry hazard")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_SPIKES); }
        PLAYER(SPECIES_WYNAUT)  { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); MaxHP(100); HP(100); }
        OPPONENT(SPECIES_WYNAUT)    { Moves(MOVE_CELEBRATE); MaxHP(100); HP(100); }
        u32 nature = NATURE_DELICATE;
        SetMonData(&OPPONENT_PARTY[1], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        // Spikes: 1/8 max HP = 12 normally, 18 with Delicate (12 * 1.5).
        HP_BAR(opponent, damage: 18);
        ABILITY_POPUP(opponent);
    }
}

SINGLE_BATTLE_TEST("Benevolent and Delicate stack multiplicatively")
{
    // Benevolent on field (+20%) AND Delicate self (+30%) = 1.2 * 1.3 = 1.56x.
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RECOVER); MaxHP(100); HP(1); }
        u32 benevolent = NATURE_BENEVOLENT;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &benevolent);
        u32 delicate = NATURE_DELICATE;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &delicate);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        // 50 * 1.2 * 1.3 = 78 HP healed. Start HP=1, so ends at 79.
        HP_BAR(opponent, hp: 79);
    }
}

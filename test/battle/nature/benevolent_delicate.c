#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_RECOVER) == EFFECT_RESTORE_HP);
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("pranks Delicate suffers fifty percent more poison damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET);
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_DELICATE});
    } WHEN {
        TURN { }
    } SCENE {
        HP_BAR(player, damage: 30);
    }
}

// --- BENEVOLENT: boosts healing for ALL mons while on field ---

SINGLE_BATTLE_TEST("pranks Benevolent boosts Recover healing by 20% for its user", u16 healAmt)
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
        // captureHP records resulting HP rather than the amount restored.
        EXPECT_EQ(results[0].healAmt, 51);
        EXPECT_EQ(results[1].healAmt, 61);
    }
}

SINGLE_BATTLE_TEST("pranks Benevolent boosts opponent's Recover healing too")
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

SINGLE_BATTLE_TEST("pranks Delicate boosts own Recover healing by 30%", u16 healAmt)
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
        EXPECT_EQ(results[0].healAmt, 51);
        EXPECT_EQ(results[1].healAmt, 66);
    }
}

SINGLE_BATTLE_TEST("pranks Delicate takes 1.5x damage from Spikes entry hazard")
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

SINGLE_BATTLE_TEST("pranks Benevolent and Delicate stack multiplicatively")
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

#include "global.h"
#include "test/battle.h"

// Devoted bonds to whichever party member has the closest personality % 100.
// Boost tiers (same for Devoted's Atk/SpA and the bond target's Def/SpDef):
//   difference 0-9:   +10%
//   difference 10-24: +6%
//   difference 25-49: +3%
//   difference 50+:   no boost
//
// KEY TEST DESIGN NOTES:
// - Never call SetMonData(MON_DATA_PERSONALITY) on a mon that has Moves() set.
//   Setting personality internally calls CalculateMonStats which wipes the move
//   slots, causing "Missing explicit X" / "Illegal MOVE" framework errors.
// - Always register extra party slots via PLAYER() blocks. SetMonData on an
//   unregistered slot leaves ghost data that persists across tests, causing the
//   "alone" test to find a spurious partner.
// - For the damage test: read slot 0's actual assigned personality and copy it
//   to slot 1 (diff=0, guaranteed +10%), without modifying slot 0.

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

// --- Logic tests: verify GetDevotedBondData returns correct tier ---
// These use MOVE_CELEBRATE (no Moves() call = no explicitMoves = no slot issue),
// and only call SetMonData(MON_DATA_PERSONALITY) on slot 1, never slot 0.

SINGLE_BATTLE_TEST("Devoted bonds at +10% when personality difference is 0-9")
{
    u32 diff;
    PARAMETRIZE { diff = 0; }
    PARAMETRIZE { diff = 9; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); }
        PLAYER(SPECIES_WYNAUT)  { Level(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); MaxHP(999); HP(999); }
        u32 nature = NATURE_DEVOTED;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        // Read slot 0's actual personality, set slot 1 to exactly (mod100 + diff)
        u32 p0 = GetMonData(&PLAYER_PARTY[0], MON_DATA_PERSONALITY);
        u32 p1 = (p0 / 100) * 100 + ((p0 % 100) + diff) % 100;
        SetMonData(&PLAYER_PARTY[1], MON_DATA_PERSONALITY, &p1);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
    } THEN {
        s32 slot = -1;
        u32 boost = GetDevotedBondData(&PLAYER_PARTY[0], &slot);
        EXPECT_EQ(boost, 10);
        EXPECT_EQ(slot, 1);
    }
}

SINGLE_BATTLE_TEST("Devoted bonds at +6% when personality difference is 10-24")
{
    u32 diff;
    PARAMETRIZE { diff = 10; }
    PARAMETRIZE { diff = 24; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); }
        PLAYER(SPECIES_WYNAUT)  { Level(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); MaxHP(999); HP(999); }
        u32 nature = NATURE_DEVOTED;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        u32 p0 = GetMonData(&PLAYER_PARTY[0], MON_DATA_PERSONALITY);
        u32 p1 = (p0 / 100) * 100 + ((p0 % 100) + diff) % 100;
        SetMonData(&PLAYER_PARTY[1], MON_DATA_PERSONALITY, &p1);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
    } THEN {
        s32 slot = -1;
        u32 boost = GetDevotedBondData(&PLAYER_PARTY[0], &slot);
        EXPECT_EQ(boost, 6);
        EXPECT_EQ(slot, 1);
    }
}

SINGLE_BATTLE_TEST("Devoted bonds at +3% when personality difference is 25-49")
{
    u32 diff;
    PARAMETRIZE { diff = 25; }
    PARAMETRIZE { diff = 49; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); }
        PLAYER(SPECIES_WYNAUT)  { Level(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); MaxHP(999); HP(999); }
        u32 nature = NATURE_DEVOTED;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        u32 p0 = GetMonData(&PLAYER_PARTY[0], MON_DATA_PERSONALITY);
        u32 p1 = (p0 / 100) * 100 + ((p0 % 100) + diff) % 100;
        SetMonData(&PLAYER_PARTY[1], MON_DATA_PERSONALITY, &p1);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
    } THEN {
        s32 slot = -1;
        u32 boost = GetDevotedBondData(&PLAYER_PARTY[0], &slot);
        EXPECT_EQ(boost, 3);
        EXPECT_EQ(slot, 1);
    }
}

SINGLE_BATTLE_TEST("Devoted receives no boost when difference is 50+")
{
    u32 diff;
    PARAMETRIZE { diff = 50; }
    PARAMETRIZE { diff = 75; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); }
        PLAYER(SPECIES_WYNAUT)  { Level(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); MaxHP(999); HP(999); }
        u32 nature = NATURE_DEVOTED;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        u32 p0 = GetMonData(&PLAYER_PARTY[0], MON_DATA_PERSONALITY);
        u32 p1 = (p0 / 100) * 100 + ((p0 % 100) + diff) % 100;
        SetMonData(&PLAYER_PARTY[1], MON_DATA_PERSONALITY, &p1);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
    } THEN {
        s32 slot = -1;
        u32 boost = GetDevotedBondData(&PLAYER_PARTY[0], &slot);
        EXPECT_EQ(boost, 0);
        EXPECT_EQ(slot, 1); // still bonds to closest, just 0% boost
    }
}

SINGLE_BATTLE_TEST("Devoted receives 0 boost and slot -1 when alone in party")
{
    // Only one PLAYER registered - slots 1..5 are zero-initialized by the
    // framework since no PLAYER() block declared them.
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); MaxHP(999); HP(999); }
        u32 nature = NATURE_DEVOTED;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
    } THEN {
        s32 slot = -1;
        u32 boost = GetDevotedBondData(&PLAYER_PARTY[0], &slot);
        EXPECT_EQ(boost, 0);
        EXPECT_EQ(slot, -1);
    }
}

SINGLE_BATTLE_TEST("Devoted bonds to the CLOSEST personality match, not just any")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); }
        PLAYER(SPECIES_WYNAUT)  { Level(1); } // will be set to diff=30
        PLAYER(SPECIES_RALTS)   { Level(1); } // will be set to diff=7 - should win
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); MaxHP(999); HP(999); }
        u32 nature = NATURE_DEVOTED;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        u32 p0 = GetMonData(&PLAYER_PARTY[0], MON_DATA_PERSONALITY);
        u32 base = p0 % 100;
        u32 p1 = (p0 / 100) * 100 + (base + 30) % 100; // diff=30, 3% tier
        u32 p2 = (p0 / 100) * 100 + (base + 7)  % 100; // diff=7,  10% tier -> wins
        SetMonData(&PLAYER_PARTY[1], MON_DATA_PERSONALITY, &p1);
        SetMonData(&PLAYER_PARTY[2], MON_DATA_PERSONALITY, &p2);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
    } THEN {
        s32 slot = -1;
        u32 boost = GetDevotedBondData(&PLAYER_PARTY[0], &slot);
        EXPECT_EQ(slot, 2); // Ralts (diff=7), not Wynaut (diff=30)
        EXPECT_EQ(boost, 10);
    }
}

// --- Damage test: prove the boost actually fires in battle ---
// No Moves() on the player (auto-assigned), so SetMonData(MON_DATA_PERSONALITY)
// on slot 1 is safe. We copy slot 0's personality to slot 1 (diff=0, +10%).

SINGLE_BATTLE_TEST("Devoted boosts own Atk by 10% in battle when closely bonded", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE;  } // neutral baseline (no boost)
    PARAMETRIZE { nature = NATURE_DEVOTED; } // diff=0 partner -> +10%

    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); }
        PLAYER(SPECIES_WYNAUT)  { Level(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        // Copy slot 0's personality to slot 1: diff=0, guaranteed +10% tier.
        // We do NOT modify slot 0's personality - that would reset moves.
        u32 p0 = GetMonData(&PLAYER_PARTY[0], MON_DATA_PERSONALITY);
        SetMonData(&PLAYER_PARTY[1], MON_DATA_PERSONALITY, &p0);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // Docile (results[0]) baseline, Devoted (results[1]) +10%.
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
    }
}

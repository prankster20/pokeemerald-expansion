#include "global.h"
#include "test/battle.h"

// ===== CALCULATING =====
// Uses the average of Def and SpDef when attacking.

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
    ASSUME(GetMoveCategory(MOVE_SWIFT) == DAMAGE_CATEGORY_SPECIAL);
    ASSUME(GetMoveEffect(MOVE_CONFUSE_RAY) == EFFECT_CONFUSE);
    ASSUME(GetMoveEffect(MOVE_TAUNT) == EFFECT_TAUNT);
    ASSUME(GetMoveEffect(MOVE_ENCORE) == EFFECT_ENCORE);
    ASSUME(GetMoveEffect(MOVE_TORMENT) == EFFECT_TORMENT);
}

SINGLE_BATTLE_TEST("Calculating uses lower of Def/SpDef when SpDef is lower", s16 damage)
{
    // High Defense, low SpDef → Calculating hits the average, not just Def.
    // Physical move normally uses Def; Calculating averages both.
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_CALCULATING; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(200); SpDefense(50); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // Docile uses Def (200). Calculating averages (200+50)/2 = 125.
        // Calculating should deal MORE damage (lower effective defense).
        EXPECT(results[1].damage > results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Calculating uses lower of Def/SpDef when Def is lower", s16 damage)
{
    // Low Defense, high SpDef → special move hits average, not just SpDef.
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_CALCULATING; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(50); SpDefense(200); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT(results[1].damage > results[0].damage);
    }
}

// ===== AFFECTIONATE =====
// At max Friendship, 20% chance to survive a lethal hit at 1 HP.

SINGLE_BATTLE_TEST("Affectionate does not endure without max Friendship")
{
    // At < max Friendship the check never fires — this is deterministic.
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); MaxHP(100); HP(100); Friendship(199); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); Attack(9999); }
        u32 nature = NATURE_AFFECTIONATE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, hp: 0);
        MESSAGE("Mienfoo fainted!");
    }
}

// ===== REBELLIOUS =====
// Ignores Encore, Taunt, Torment, Disable, Heal Block.

SINGLE_BATTLE_TEST("Rebellious ignores Taunt and can still use status moves")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CONFUSE_RAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TAUNT); }
        u32 nature = NATURE_REBELLIOUS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); MOVE(opponent, MOVE_TAUNT); }
        TURN { MOVE(player, MOVE_CONFUSE_RAY); }
    } SCENE {
        // Even after being Taunted, Confuse Ray works.
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, player);
    }
}

SINGLE_BATTLE_TEST("Rebellious ignores Encore and can use different moves")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE, MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ENCORE); }
        u32 nature = NATURE_REBELLIOUS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_ENCORE); }
        TURN { MOVE(player, MOVE_SWIFT); } // should be allowed despite Encore
    } SCENE {
        MESSAGE("Mienfoo used Swift!"); // not forced to repeat Tackle
    }
}

// ===== FORGIVING =====
// Friendship never decreases, +10% Return. Cannot use vengeance moves.

SINGLE_BATTLE_TEST("Forgiving cannot learn Revenge")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_REVENGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_FORGIVING;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_REVENGE); }
    } SCENE {
        // Forgiving refuses vengeance moves — Revenge should fail/be illegal.
        MESSAGE("Mienfoo can't use that move!");
    }
}

SINGLE_BATTLE_TEST("Forgiving cannot use Destiny Bond")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_DESTINY_BOND); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_FORGIVING;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_DESTINY_BOND); }
    } SCENE {
        MESSAGE("Mienfoo can't use that move!");
    }
}

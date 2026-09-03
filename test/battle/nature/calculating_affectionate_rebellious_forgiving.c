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

SINGLE_BATTLE_TEST("pranks Calculating uses the average of Def and SpDef for physical damage", s16 damage)
{
    u32 nature;
    u16 defense, spDefense;
    PARAMETRIZE { nature = NATURE_DOCILE;      defense = 200; spDefense = 50;  }
    PARAMETRIZE { nature = NATURE_CALCULATING; defense = 200; spDefense = 50;  }
    PARAMETRIZE { nature = NATURE_DOCILE;      defense = 125; spDefense = 125; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(defense); SpDefense(spDefense); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT(results[1].damage > results[0].damage);
        EXPECT_EQ(results[1].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Calculating uses the average of Def and SpDef for special damage", s16 damage)
{
    u32 nature;
    u16 defense, spDefense;
    PARAMETRIZE { nature = NATURE_DOCILE;      defense = 50;  spDefense = 200; }
    PARAMETRIZE { nature = NATURE_CALCULATING; defense = 50;  spDefense = 200; }
    PARAMETRIZE { nature = NATURE_DOCILE;      defense = 125; spDefense = 125; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Level(50); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(defense); SpDefense(spDefense); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT(results[1].damage > results[0].damage);
        EXPECT_EQ(results[1].damage, results[2].damage);
    }
}

// ===== AFFECTIONATE =====
// At max Friendship, 20% chance to survive a lethal hit at 1 HP.

SINGLE_BATTLE_TEST("pranks Affectionate does not endure without max Friendship")
{
    // At < max Friendship the check never fires — this is deterministic.
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); MaxHP(100); HP(1); Friendship(199); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); Attack(100); }
        u32 nature = NATURE_AFFECTIONATE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, hp: 0);
        MESSAGE("Mienfoo fainted!");
    }
}

SINGLE_BATTLE_TEST("pranks Affectionate has a 20 percent chance to endure at max Friendship")
{
    PASSES_RANDOMLY(20, 100, RNG_NATURE_AFFECTIONATE);
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); MaxHP(100); HP(1); Friendship(MAX_FRIENDSHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); Attack(100); }
        u32 nature = NATURE_AFFECTIONATE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, hp: 1);
        NOT MESSAGE("Mienfoo fainted!");
    }
}

// ===== REBELLIOUS =====
// Ignores Encore, Taunt, Torment, Disable, Heal Block.

SINGLE_BATTLE_TEST("pranks Rebellious ignores Taunt and can still use status moves")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CONFUSE_RAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TAUNT); }
        u32 nature = NATURE_OLD_REBELLIOUS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); MOVE(opponent, MOVE_TAUNT); }
        TURN { MOVE(player, MOVE_CONFUSE_RAY); }
    } SCENE {
        // Even after being Taunted, Confuse Ray works.
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, player);
    }
}

SINGLE_BATTLE_TEST("pranks Rebellious ignores Encore and can use different moves")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE, MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ENCORE); }
        u32 nature = NATURE_OLD_REBELLIOUS;
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

SINGLE_BATTLE_TEST("pranks Forgiving refuses Revenge when learning moves")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_OLD_FORGIVING;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        // Forgiving refuses vengeance moves — Revenge should fail/be illegal.
        EXPECT(DoesBoxMonNatureRefuseMove(&PLAYER_PARTY[0].box, MOVE_REVENGE));
    }
}

SINGLE_BATTLE_TEST("pranks Forgiving refuses Destiny Bond when learning moves")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_OLD_FORGIVING;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(DoesBoxMonNatureRefuseMove(&PLAYER_PARTY[0].box, MOVE_DESTINY_BOND));
    }
}

SINGLE_BATTLE_TEST("pranks Forgiving cannot land even a guaranteed critical hit")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_FROST_BREATH) == EFFECT_ALWAYS_CRITICAL_HIT);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_FROST_BREATH); }
        OPPONENT(SPECIES_WOBBUFFET);
        u32 nature = NATURE_FORGIVING;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_FROST_BREATH); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("pranks Forgiving ignores a foe's positive offensive stat stages", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_HARDY; }
    PARAMETRIZE { nature = NATURE_FORGIVING; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Defense(100); MaxHP(999); HP(999); }
        OPPONENT(SPECIES_MIENFOO) { Moves(MOVE_SWORDS_DANCE, MOVE_SCRATCH); Attack(100); Speed(100); }
        u32 setNature = nature;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &setNature);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}

#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
    ASSUME(GetMoveCategory(MOVE_SWIFT) == DAMAGE_CATEGORY_SPECIAL);
    // Storm Throw: always-crit Fighting move
    ASSUME(GetMoveCategory(MOVE_STORM_THROW) == DAMAGE_CATEGORY_PHYSICAL);
    // Night Slash is a Dark move — Special in the old split, Physical in modern
    ASSUME(GetMoveCategory(MOVE_NIGHT_SLASH) == DAMAGE_CATEGORY_PHYSICAL);
}

// ===== ANXIOUS =====
// -20% Speed at full HP. +20% Speed at ≤half HP.

SINGLE_BATTLE_TEST("Anxious moves last at full HP due to -20% Speed")
{
    GIVEN {
        // Player (Anxious, 100 Speed) should be slower than opponent (85 Speed)
        // at full HP due to -20% penalty: 100 * 0.8 = 80 < 85.
        PLAYER(SPECIES_MIENFOO) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(85); Moves(MOVE_TACKLE); }
        u32 nature = NATURE_ANXIOUS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        // Opponent (85) should move first, beating the penalized Anxious (80).
        MESSAGE("The opposing Wobbuffet used Tackle!");
        HP_BAR(player);
        MESSAGE("Mienfoo used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("Anxious moves first at half HP due to +20% Speed")
{
    GIVEN {
        // At half HP, Anxious (100 * 1.2 = 120) outspeed opponent (110 Speed).
        PLAYER(SPECIES_MIENFOO) { Speed(100); MaxHP(100); HP(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(110); Moves(MOVE_TACKLE); }
        u32 nature = NATURE_ANXIOUS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        // Anxious at half HP (120) moves before opponent (110).
        MESSAGE("Mienfoo used Celebrate!");
        MESSAGE("The opposing Wobbuffet used Tackle!");
    }
}

// ===== DIPLOMATIC =====
// No Pokémon can land critical hits while a Diplomatic mon is on the field.
// Overrides even always-crit moves like Storm Throw.

SINGLE_BATTLE_TEST("Diplomatic prevents random critical hits")
{
    PASSES_RANDOMLY(0, 1, RNG_CRITICAL_HIT); // force a crit roll that would normally hit
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_DIPLOMATIC;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Diplomatic prevents Storm Throw from always critting", s16 damage)
{
    // Storm Throw uses CRITICAL_HIT_ALWAYS — Diplomatic must override this.
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }   // baseline: crits every time
    PARAMETRIZE { nature = NATURE_DIPLOMATIC; } // should NOT crit
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_STORM_THROW); Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_STORM_THROW); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // With Diplomatic (results[1]): no crit, so less damage than Docile (results[0]).
        // Without crit: damage is noticeably lower since crits ignore Def stage boosts
        // and deal 1.5x. results[1] should be less than results[0].
        EXPECT(results[1].damage < results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Diplomatic blocks crits even on the opponent's attacks")
{
    PASSES_RANDOMLY(0, 1, RNG_CRITICAL_HIT);
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        u32 nature = NATURE_DIPLOMATIC;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT MESSAGE("A critical hit!");
        HP_BAR(player);
    }
}

// ===== OLD-FASHIONED =====
// Uses Gen 1-3 type-based Physical/Special split.
// Dark is Special in the old split, Physical in modern GEN 4+ split.
// So Night Slash (Dark, Physical in GEN4+) should use SpAtk/SpDef for Old-Fashioned.

SINGLE_BATTLE_TEST("Old-Fashioned uses SpAtk for Dark-type Night Slash", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }      // uses modern Atk/Def (Physical)
    PARAMETRIZE { nature = NATURE_OLD_FASHIONED; } // uses SpAtk/SpDef (Special in old split)
    GIVEN {
        // Low Attack, high SpAttack — Night Slash should deal MORE with Old-Fashioned
        // if it uses SpAtk. Low Defense opponent, high SpDefense opponent.
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(50); SpAttack(150); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); Defense(100); SpDefense(50); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_NIGHT_SLASH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // Old-Fashioned (results[1]): SpAtk=150 vs SpDef=50 → more damage.
        // Docile (results[0]): Atk=50 vs Def=100 → less damage.
        EXPECT(results[1].damage > results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Old-Fashioned does not change Fire-type moves (Special in both splits)", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_OLD_FASHIONED; }
    GIVEN {
        // Both splits agree Fire is Special — identical damage expected.
        PLAYER(SPECIES_MIENFOO) { Level(50); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); SpDefense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

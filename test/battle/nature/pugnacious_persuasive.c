#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
    ASSUME(GetMoveEffect(MOVE_REFLECT) == EFFECT_REFLECT);
    ASSUME(GetMoveEffect(MOVE_TAUNT) == EFFECT_TAUNT);
    ASSUME(GetMoveEffect(MOVE_CONFUSE_RAY) == EFFECT_CONFUSE);
}

// ===== PUGNACIOUS =====
// Infiltrates: ignores Screens and Substitute.

SINGLE_BATTLE_TEST("pranks Pugnacious ignores Reflect when attacking", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_DOCILE; }
    PARAMETRIZE { nature = NATURE_OLD_PUGNACIOUS; }
    GIVEN {
        WITH_CONFIG(B_TAUNT_TURNS, GEN_5);
        PLAYER(SPECIES_MIENFOO) { Level(50); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_REFLECT, MOVE_CELEBRATE); Level(50); Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFLECT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // Docile (results[0]) gets halved by Reflect.
        // Pugnacious (results[1]) bypasses it — should deal ~2x as much.
        EXPECT(results[1].damage > results[0].damage);
    }
}

SINGLE_BATTLE_TEST("pranks Pugnacious ignores Substitute when attacking")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CONFUSE_RAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUBSTITUTE); MaxHP(100); HP(100); }
        u32 nature = NATURE_OLD_PUGNACIOUS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_CONFUSE_RAY); }
    } SCENE {
        // Substitute should NOT block Confuse Ray for Pugnacious.
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, player);
    } THEN {
        u32 confused = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.confusionTurns;
        EXPECT(confused > 0);
    }
}

// ===== PERSUASIVE =====
// Volatile statuses inflicted by this Pokémon last 1 extra turn.
// B_TAUNT_TIMER = 5 normally, Persuasive makes it 6.

SINGLE_BATTLE_TEST("pranks Persuasive extends Taunt duration by 1 turn")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TAUNT); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
        u32 nature = NATURE_PERSUASIVE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TAUNT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
    } THEN {
        u32 tauntTurns = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.tauntTimer;
        // The timer has already ticked once at end of turn: Gen 5+ leaves
        // 2 turns normally and Persuasive leaves 3.
        EXPECT_EQ(tauntTurns, 3);
    }
}

SINGLE_BATTLE_TEST("pranks Without Persuasive, Taunt lasts the normal duration")
{
    GIVEN {
        WITH_CONFIG(B_TAUNT_TURNS, GEN_5);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TAUNT); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
        u32 nature = NATURE_DOCILE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TAUNT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
    } THEN {
        u32 tauntTurns = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.tauntTimer;
        EXPECT_EQ(tauntTurns, 2);
    }
}

SINGLE_BATTLE_TEST("pranks Persuasive extends Confusion duration by 1 turn")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CONFUSE_RAY); Speed(2); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(1); }
        u32 nature = NATURE_PERSUASIVE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, player);
    } THEN {
        u32 confusionTurns = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.confusionTurns;
        // Confusion is 2-5 turns normally; Persuasive adds 1, making it 3-6.
        EXPECT(confusionTurns >= 3);
    }
}

#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Resolute cleanses harmful volatiles at one third HP but preserves nonvolatile status")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_TAUNT) == EFFECT_TAUNT);
        ASSUME(GetMoveEffect(MOVE_SONIC_BOOM) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_SONIC_BOOM) == 20);
        ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(120); HP(41); Speed(1); Status1(STATUS1_PARALYSIS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_TAUNT, MOVE_SONIC_BOOM); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_RESOLUTE);
    } WHEN {
        // Turn 1: opponent Taunts, player uses damaging Tackle (Taunt allows it).
        // HP stays at 41/120 (35%), above the 1/3 threshold (40/120).
        TURN { MOVE(opponent, MOVE_TAUNT); MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_PARALYSIS, FALSE)); }
        // Turn 2: Sonic Boom deals 20 fixed damage -> 21/120 (17.5%), crossing below
        // the 1/3 threshold. Resolute fires here, clearing Taunt but keeping Paralysis.
        TURN { MOVE(opponent, MOVE_SONIC_BOOM); MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, opponent);
        HP_BAR(player, damage: 20);
        ABILITY_POPUP(player);
    } THEN {
        EXPECT(player->status1 & STATUS1_PARALYSIS);
        EXPECT_EQ((u32)player->volatiles.tauntTimer, 0);
    }
}


SINGLE_BATTLE_TEST("pranks Resolute heals one eighth max HP when crossing the one-third threshold")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SONIC_BOOM) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_SONIC_BOOM) == 20);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(120); HP(41); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_SONIC_BOOM); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_RESOLUTE);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SONIC_BOOM); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, damage: 20);
        HP_BAR(player, damage: -15);
    }
}

#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("Resolute cleanses harmful volatiles at one third HP but preserves nonvolatile status")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_TAUNT) == EFFECT_TAUNT);
        ASSUME(GetMoveEffect(MOVE_SONIC_BOOM) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_SONIC_BOOM) == 20);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(120); HP(41); Speed(1); Status1(STATUS1_PARALYSIS); Moves(MOVE_TACKLE, MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_TAUNT, MOVE_SONIC_BOOM); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_RESOLUTE);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAUNT); MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_PARALYSIS, FALSE)); }
        TURN { MOVE(opponent, MOVE_SONIC_BOOM); MOVE(player, MOVE_GROWL, WITH_RNG(RNG_PARALYSIS, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, opponent);
        HP_BAR(player, damage: 20);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        STATUS_ICON(player, paralysis: TRUE);
    } THEN {
        EXPECT(player->status1 & STATUS1_PARALYSIS);
        EXPECT_EQ((u32)player->volatiles.tauntTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Resilient heals at the new one third HP threshold")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SONIC_BOOM) == EFFECT_FIXED_HP_DAMAGE);
        ASSUME(GetMoveFixedHPDamage(MOVE_SONIC_BOOM) == 20);
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(120); HP(41); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_SONIC_BOOM); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_RESILIENT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SONIC_BOOM); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, damage: 20);
        HP_BAR(player, damage: -15);
    }
}

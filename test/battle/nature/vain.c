#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Vain permanently loses its battle stat boost after a successful stat drop")
{
    GIVEN {
        ASSUME_STAT_CHANGE(MOVE_GROWL, attack: -1);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); Attack(105); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_VAIN);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponent);
        ABILITY_POPUP(player);
    } THEN {
        EXPECT(GetBattlerPartyState(B_BATTLER_0)->vainBroken);
        EXPECT_EQ(player->attack, 100);
    }
}

SINGLE_BATTLE_TEST("pranks Vain does not break when a stat drop is prevented")
{
    GIVEN {
        ASSUME_STAT_CHANGE(MOVE_GROWL, attack: -1);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); Ability(ABILITY_CLEAR_BODY); Attack(105); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
        SetTestNature(&PLAYER_PARTY[0], NATURE_VAIN);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GROWL); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!GetBattlerPartyState(B_BATTLER_0)->vainBroken);
    }
}

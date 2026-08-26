#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_CONFUSE_RAY) == EFFECT_CONFUSE);
}

SINGLE_BATTLE_TEST("pranks Level-Headed prevents Confusion")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CONFUSE_RAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_LEVEL_HEADED;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); }
    } SCENE {
        ABILITY_POPUP(opponent);
    } THEN {
        u32 confusionTurns = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.confusionTurns;
        EXPECT_EQ(confusionTurns, 0);
    }
}

SINGLE_BATTLE_TEST("pranks Level-Headed prevents flinching")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        u32 nature = NATURE_LEVEL_HEADED;
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_FAKE_OUT); MOVE(opponent, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, player);
        ABILITY_POPUP(opponent);
        NOT MESSAGE("The opposing Wobbuffet flinched and couldn't move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, opponent);
    }
}

DOUBLE_BATTLE_TEST("pranks Level-Headed ignores Follow Me redirection")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_POUND); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        u32 nature = NATURE_LEVEL_HEADED;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_FOLLOW_ME);
            MOVE(playerLeft, MOVE_POUND, target: opponentRight);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOLLOW_ME, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, playerLeft);
        HP_BAR(opponentRight);
        NOT HP_BAR(opponentLeft);
    }
}

DOUBLE_BATTLE_TEST("pranks Level-Headed ignores ability redirection")
{
    GIVEN {
        WITH_CONFIG(B_REDIRECT_ABILITY_IMMUNITY, GEN_5);
        ASSUME(GetMoveType(MOVE_SPARK) == TYPE_ELECTRIC);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SPARK); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_RAICHU) { Ability(ABILITY_LIGHTNING_ROD); }
        OPPONENT(SPECIES_WOBBUFFET);
        u32 nature = NATURE_LEVEL_HEADED;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SPARK, target: opponentRight); }
    } SCENE {
        HP_BAR(opponentRight);
        NONE_OF {
            HP_BAR(opponentLeft);
            ABILITY_POPUP(opponentLeft, ABILITY_LIGHTNING_ROD);
        }
    }
}

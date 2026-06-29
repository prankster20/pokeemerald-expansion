#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("Flirty has a 1/8 chance to infatuate when attacking")
{
    PASSES_RANDOMLY(1, 8, RNG_NATURE_FLIRTY);
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_FLIRTY;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player);
    } THEN {
        u32 infatuation = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.infatuation;
        EXPECT(infatuation != 0);
    }
}

SINGLE_BATTLE_TEST("Flirty is immune to Infatuation")
{
    PASSES_RANDOMLY(1, 8, RNG_NATURE_FLIRTY);
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_FLIRTY;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        // The 1/8 roll still "happens" against an immune target - it just
        // shows the immunity popup instead of inflicting infatuation.
        ABILITY_POPUP(opponent);
    } THEN {
        u32 infatuation = gBattleMons[B_POSITION_OPPONENT_LEFT].volatiles.infatuation;
        EXPECT_EQ(infatuation, 0);
    }
}

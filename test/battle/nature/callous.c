#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(B_USE_FROSTBITE == TRUE);
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("pranks Callous has a 1/8 chance to inflict Frostbite when attacking")
{
    PASSES_RANDOMLY(1, 8, RNG_NATURE_CALLOUS);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_OLD_CALLOUS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player);
        FREEZE_OR_FROSTBURN_STATUS(opponent, TRUE);
    }
}

SINGLE_BATTLE_TEST("pranks Callous is immune to Frostbite")
{
    PASSES_RANDOMLY(1, 8, RNG_NATURE_CALLOUS);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_OLD_CALLOUS;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        SetMonData(&OPPONENT_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        // The 1/8 roll still "happens" against an immune target - it just
        // shows the immunity popup instead of inflicting Frostbite.
        ABILITY_POPUP(opponent);
        NOT FREEZE_OR_FROSTBURN_STATUS(opponent, TRUE);
    }
}

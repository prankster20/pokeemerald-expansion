#include "global.h"
#include "test/battle.h"

// Note: this only covers the end-of-turn heal. "Refuses to learn moves with
// recoil or ones that lower own stats" is checked by DoesBoxMonNatureRefuseMove
// (src/pokemon.c) at move-learn time, not in battle - that needs a different
// kind of test (not SINGLE_BATTLE_TEST), so it isn't covered here.

SINGLE_BATTLE_TEST("Hedonistic recovers 1/16 max HP at the end of each turn")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); MaxHP(160); HP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_HEDONISTIC;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, hp: 110); // 100 + (160 / 16)
    }
}

SINGLE_BATTLE_TEST("Hedonistic does not heal past max HP")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); MaxHP(160); HP(155); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_HEDONISTIC;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, hp: 160);
    }
}

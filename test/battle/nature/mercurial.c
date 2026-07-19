#include "global.h"
#include "test/battle.h"

// Capricious (renamed from Mercurial) rerolls the active nature after each
// battle. We can't guarantee it lands on a different value, but we CAN
// verify:
//   1. The MON_DATA_MERCURIAL_NATURE flag persists after the battle.
//   2. The new active nature is not Capricious again.
//      itself (the reroll blacklists it).

SINGLE_BATTLE_TEST("pranks Capricious keeps its reroll flag after battle ends")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        u32 nature = NATURE_ADAMANT;
        bool8 isMercurial = TRUE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        SetMonData(&PLAYER_PARTY[0], MON_DATA_MERCURIAL_NATURE, &isMercurial);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    } THEN {
        // The Mercurial/Capricious flag should still be set after battle.
        u32 flag = GetMonData(&PLAYER_PARTY[0], MON_DATA_MERCURIAL_NATURE);
        EXPECT(flag);
    }
}

SINGLE_BATTLE_TEST("pranks Capricious never rerolls back into Capricious")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        u32 nature = NATURE_ADAMANT;
        bool8 isMercurial = TRUE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        SetMonData(&PLAYER_PARTY[0], MON_DATA_MERCURIAL_NATURE, &isMercurial);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    } THEN {
        u32 newNature = GetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE);
        // The active nature must never be the Mercurial/Capricious nature ID itself.
        EXPECT(newNature != NATURE_CAPRICIOUS);
    }
}

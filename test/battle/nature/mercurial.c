#include "global.h"
#include "test/battle.h"

// Mercurial's reroll happens in the post-battle cleanup in battle_main.c
// (RerollMercurialNature), which runs after the simulated battle considers
// itself over - this is checking state that lives outside the usual
// SCENE-event flow, so it's the part of this batch I'm least sure will
// work as written. If THEN doesn't see the post-cleanup state, this is
// the one to look at first.

SINGLE_BATTLE_TEST("Mercurial's active Nature changes once the battle ends")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); HP(1); }
        u32 nature = NATURE_ADAMANT; // starting active nature, distinct from Mercurial itself
        bool8 isMercurial = TRUE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
        SetMonData(&PLAYER_PARTY[0], MON_DATA_MERCURIAL_NATURE, &isMercurial);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
    } THEN {
        u32 newNature = GetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE);
        // The active nature should have rerolled to something else...
        EXPECT(newNature != NATURE_ADAMANT);
        // ...but never to Mercurial itself (that's the "is this mon
        // Mercurial" flag, not a real active nature).
        EXPECT(newNature != NATURE_MERCURIAL);
        // The Mercurial flag itself should still be set - "stays forever".
        EXPECT(GetMonData(&PLAYER_PARTY[0], MON_DATA_MERCURIAL_NATURE));
    }
}

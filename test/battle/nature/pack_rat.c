#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("pranks Pack Rat enters battle with one Hoard Counter")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_MIENFOO);
        OPPONENT(SPECIES_WOBBUFFET);
        SetMonData(&PLAYER_PARTY[1], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_PACK_RAT});
    } WHEN {
        TURN { SWITCH(player, 1); }
    } THEN {
        EXPECT(player->volatiles.stockpileCounter == 1);
    }
}

#include "global.h"
#include "pokemon.h"
#include "test/test.h"

TEST("pranks Prodigious permits level-based evolution one level early")
{
    struct Pokemon mon;
    bool32 canStopEvo = FALSE;
    u32 nature = NATURE_PRODIGIOUS;

    CreateMon(&mon, SPECIES_BULBASAUR, 15, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStopEvo, CHECK_EVO), SPECIES_IVYSAUR);
}

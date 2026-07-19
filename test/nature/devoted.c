#include "global.h"
#include "pokemon.h"
#include "test/test.h"

static void ClearPlayerParty(void)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);
}

static void CreatePartyMon(u32 slot, enum Species species, u32 personality)
{
    u32 nature = NATURE_DEVOTED;

    CreateMon(&gParties[B_TRAINER_PLAYER][slot], species, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_PERSONALITY, &personality);
    SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HIDDEN_NATURE, &nature);
}

TEST("pranks Devoted returns the intended boost at every personality-distance boundary")
{
    u32 difference;
    u32 expectedBoost;
    s32 expectedSlot;
    s32 slot;

    PARAMETRIZE { difference = 0;  expectedBoost = 10; expectedSlot = 1; }
    PARAMETRIZE { difference = 9;  expectedBoost = 10; expectedSlot = 1; }
    PARAMETRIZE { difference = 10; expectedBoost = 5;  expectedSlot = 1; }
    PARAMETRIZE { difference = 33; expectedBoost = 5;  expectedSlot = 1; }
    PARAMETRIZE { difference = 34; expectedBoost = 0;  expectedSlot = 1; }
    PARAMETRIZE { difference = 50; expectedBoost = 0;  expectedSlot = 1; }

    ClearPlayerParty();
    CreatePartyMon(0, SPECIES_MIENFOO, 100);
    CreatePartyMon(1, SPECIES_WYNAUT, 100 + difference);

    slot = -1;
    EXPECT_EQ(GetDevotedBondData(&gParties[B_TRAINER_PLAYER][0], &slot), expectedBoost);
    EXPECT_EQ(slot, expectedSlot);
}

TEST("pranks Devoted chooses the closest personality match")
{
    s32 slot = -1;

    ClearPlayerParty();
    CreatePartyMon(0, SPECIES_MIENFOO, 100);
    CreatePartyMon(1, SPECIES_WYNAUT, 130);
    CreatePartyMon(2, SPECIES_RALTS, 107);

    EXPECT_EQ(GetDevotedBondData(&gParties[B_TRAINER_PLAYER][0], &slot), 10);
    EXPECT_EQ(slot, 2);
}

TEST("pranks Devoted returns no bond when alone")
{
    s32 slot = -1;

    ClearPlayerParty();
    CreatePartyMon(0, SPECIES_MIENFOO, 100);

    EXPECT_EQ(GetDevotedBondData(&gParties[B_TRAINER_PLAYER][0], &slot), 0);
    EXPECT_EQ(slot, -1);
}

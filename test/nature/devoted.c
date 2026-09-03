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
    CreateMon(&gParties[B_TRAINER_PLAYER][slot], species, 50, personality, OTID_STRUCT_PLAYER_ID);
}

static u32 MakePartyMonDevoted(u32 slot)
{
    u32 nature = NATURE_DEVOTED;

    SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HIDDEN_NATURE, &nature);
    return GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_PERSONALITY);
}

static u32 MakePersonalityAtDistance(u32 personality, u32 difference)
{
    if (personality % 100 >= difference)
        return personality - difference;
    else
        return personality + difference;
}

TEST("pranks Devoted returns the intended boost at every personality-distance boundary")
{
    u32 difference;
    u32 expectedBoost;
    s32 expectedSlot;
    s32 slot;
    u32 devotedPersonality;

    PARAMETRIZE { difference = 0;  expectedBoost = 6; expectedSlot = 1; }
    PARAMETRIZE { difference = 9;  expectedBoost = 6; expectedSlot = 1; }
    PARAMETRIZE { difference = 10; expectedBoost = 3; expectedSlot = 1; }
    PARAMETRIZE { difference = 33; expectedBoost = 3; expectedSlot = 1; }
    PARAMETRIZE { difference = 34; expectedBoost = 0;  expectedSlot = 1; }
    PARAMETRIZE { difference = 50; expectedBoost = 0;  expectedSlot = 1; }

    ClearPlayerParty();
    CreatePartyMon(0, SPECIES_MIENFOO, 100);
    devotedPersonality = MakePartyMonDevoted(0);
    CreatePartyMon(1, SPECIES_WYNAUT, MakePersonalityAtDistance(devotedPersonality, difference));

    slot = -1;
    EXPECT_EQ(GetDevotedBondData(&gParties[B_TRAINER_PLAYER][0], &slot), expectedBoost);
    EXPECT_EQ(slot, expectedSlot);
}

TEST("pranks Devoted chooses the closest personality match")
{
    s32 slot = -1;
    u32 devotedPersonality;

    ClearPlayerParty();
    CreatePartyMon(0, SPECIES_MIENFOO, 100);
    devotedPersonality = MakePartyMonDevoted(0);
    CreatePartyMon(1, SPECIES_WYNAUT, MakePersonalityAtDistance(devotedPersonality, 30));
    CreatePartyMon(2, SPECIES_RALTS, MakePersonalityAtDistance(devotedPersonality, 7));

    EXPECT_EQ(GetDevotedBondData(&gParties[B_TRAINER_PLAYER][0], &slot), 6);
    EXPECT_EQ(slot, 2);
}

TEST("pranks Devoted returns no bond when alone")
{
    s32 slot = -1;

    ClearPlayerParty();
    CreatePartyMon(0, SPECIES_MIENFOO, 100);
    MakePartyMonDevoted(0);

    EXPECT_EQ(GetDevotedBondData(&gParties[B_TRAINER_PLAYER][0], &slot), 0);
    EXPECT_EQ(slot, -1);
}

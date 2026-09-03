#include "global.h"
#include "item.h"
#include "pokemon.h"
#include "test/test.h"

TEST("pranks Minting Innocent sets Friendship to zero")
{
    struct Pokemon mon;
    u32 friendship = 255;

    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_FRIENDSHIP, &friendship);
    ApplyMintedNature(&mon, NATURE_INNOCENT);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), NATURE_INNOCENT);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_FRIENDSHIP), 0);
}

TEST("pranks Innocent evolves into a Nature based on Friendship")
{
    u32 friendship;
    u32 expectedNature;

    PARAMETRIZE { friendship = 0; expectedNature = NATURE_CYNICAL; }
    PARAMETRIZE { friendship = 99; expectedNature = NATURE_CYNICAL; }
    PARAMETRIZE { friendship = 100; expectedNature = NATURE_REALISTIC; }
    PARAMETRIZE { friendship = 199; expectedNature = NATURE_REALISTIC; }
    PARAMETRIZE { friendship = 200; expectedNature = NATURE_IDEALISTIC; }
    PARAMETRIZE { friendship = MAX_FRIENDSHIP; expectedNature = NATURE_IDEALISTIC; }

    EXPECT_EQ(GetInnocentEvolutionNatureFromFriendship(friendship), expectedNature);
}

TEST("pranks Reserved Natures cannot be offered by Mints or random Nature changes")
{
    u32 nature;

    PARAMETRIZE { nature = NATURE_CYNICAL; }
    PARAMETRIZE { nature = NATURE_REALISTIC; }
    PARAMETRIZE { nature = NATURE_IDEALISTIC; }
    EXPECT(IsNatureExcludedFromRandomAcquisition(nature));
}

TEST("pranks Ordinary custom Natures remain directly mintable")
{
    EXPECT(!IsNatureExcludedFromRandomAcquisition(NATURE_INNOCENT));
    EXPECT(!IsNatureExcludedFromRandomAcquisition(NATURE_AFFECTIONATE));
    EXPECT(!IsNatureExcludedFromRandomAcquisition(NATURE_CAPRICIOUS));
    EXPECT(!IsNatureExcludedFromRandomAcquisition(NATURE_STALWART));
}

TEST("pranks Nostalgic and Level-Headed occupy their stable IDs")
{
    EXPECT_EQ(NATURE_NOSTALGIC, 46);
    EXPECT_EQ(NATURE_STALWART, 77);
}

TEST("pranks Capricious minting enables persistent rerolls and chooses a different active Nature")
{
    struct Pokemon mon;
    u32 nature = NATURE_ADAMANT;

    SET_RNG(RNG_MINT, NATURE_HARDY);
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);
    ApplyMintedNature(&mon, NATURE_CAPRICIOUS);

    EXPECT(GetMonData(&mon, MON_DATA_MERCURIAL_NATURE));
    EXPECT_NE(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), NATURE_CAPRICIOUS);
}

TEST("pranks Observant stacking follows the two-through-four-roll curve")
{
    u32 count, roll, expectedRerolls;

    PARAMETRIZE { count = 0; roll = 0; expectedRerolls = 0; }
    PARAMETRIZE { count = 1; roll = 0; expectedRerolls = 1; }
    PARAMETRIZE { count = 2; roll = 0; expectedRerolls = 2; }
    PARAMETRIZE { count = 2; roll = 2; expectedRerolls = 1; }
    PARAMETRIZE { count = 3; roll = 3; expectedRerolls = 2; }
    PARAMETRIZE { count = 3; roll = 4; expectedRerolls = 1; }
    PARAMETRIZE { count = 4; roll = 0; expectedRerolls = 3; }
    PARAMETRIZE { count = 4; roll = 1; expectedRerolls = 2; }
    PARAMETRIZE { count = 5; roll = 2; expectedRerolls = 3; }
    PARAMETRIZE { count = 5; roll = 3; expectedRerolls = 2; }
    PARAMETRIZE { count = 6; roll = 4; expectedRerolls = 3; }

    EXPECT_EQ(GetObservantRerolls(count, roll), expectedRerolls);
}

TEST("pranks Shiny Odds options map to their displayed denominators")
{
    u32 option, expectedThreshold;

    PARAMETRIZE { option = OPTIONS_SHINY_ODDS_4096; expectedThreshold = 16; }
    PARAMETRIZE { option = OPTIONS_SHINY_ODDS_1024; expectedThreshold = 64; }
    PARAMETRIZE { option = OPTIONS_SHINY_ODDS_256;  expectedThreshold = 256; }
    PARAMETRIZE { option = OPTIONS_SHINY_ODDS_64;   expectedThreshold = 1024; }

    gSaveBlock2Ptr->optionsShinyOdds = option;
    EXPECT_EQ(GetConfiguredShinyOdds(), expectedThreshold);
}

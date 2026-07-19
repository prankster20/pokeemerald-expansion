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
    EXPECT(!IsNatureExcludedFromRandomAcquisition(NATURE_LEVEL_HEADED));
}

TEST("pranks Nostalgic and Level-Headed occupy their stable IDs")
{
    EXPECT_EQ(NATURE_NOSTALGIC, 46);
    EXPECT_EQ(NATURE_LEVEL_HEADED, 77);
}

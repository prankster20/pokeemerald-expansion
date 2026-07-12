#include "global.h"
#include "item.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Minting Innocent sets Friendship to zero")
{
    struct Pokemon mon;
    u32 friendship = 255;

    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_FRIENDSHIP, &friendship);
    ApplyMintedNature(&mon, NATURE_INNOCENT);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), NATURE_INNOCENT);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_FRIENDSHIP), 0);
}

TEST("Innocent evolves into a Nature based on Friendship")
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

TEST("Reserved Natures cannot be offered by Mints or random Nature changes")
{
    u32 nature;

    PARAMETRIZE { nature = NATURE_CYNICAL; }
    PARAMETRIZE { nature = NATURE_REALISTIC; }
    PARAMETRIZE { nature = NATURE_IDEALISTIC; }
    PARAMETRIZE { nature = NATURE_LEVEL_HEADED; }

    EXPECT(IsNatureExcludedFromRandomAcquisition(nature));
}

TEST("Nostalgic and reserve Level-Headed occupy their stable IDs")
{
    EXPECT_EQ(NATURE_NOSTALGIC, 46);
    EXPECT_EQ(NATURE_LEVEL_HEADED, 103);
}

TEST("Innocent Affectionate and Capricious remain directly mintable")
{
    EXPECT(!IsNatureExcludedFromRandomAcquisition(NATURE_INNOCENT));
    EXPECT(!IsNatureExcludedFromRandomAcquisition(NATURE_AFFECTIONATE));
    EXPECT(!IsNatureExcludedFromRandomAcquisition(NATURE_CAPRICIOUS));
}

TEST("Custom five-percent Nature stat modifiers use their intended stats")
{
    EXPECT_EQ(ModifyStatByNature(NATURE_VAIN, 100, STAT_ATK, 0), 105);
    EXPECT_EQ(ModifyStatByNature(NATURE_VAIN, 100, STAT_SPEED, 0), 105);
    EXPECT_EQ(ModifyStatByNature(NATURE_BITTER, 100, STAT_SPDEF, 0), 105);
    EXPECT_EQ(ModifyStatByNature(NATURE_BITTER, 100, STAT_DEF, 0), 100);
    EXPECT_EQ(ModifyStatByNature(NATURE_NOBLE, 100, STAT_DEF, 0), 105);
    EXPECT_EQ(ModifyStatByNature(NATURE_NOBLE, 100, STAT_SPDEF, 0), 100);
}

TEST("Finicky boosts only Speed by twenty percent")
{
    EXPECT_EQ(ModifyStatByNature(NATURE_FINICKY, 100, STAT_SPEED, 0), 120);
    EXPECT_EQ(ModifyStatByNature(NATURE_FINICKY, 100, STAT_ATK, 0), 100);
}

// TEST("Materialistic gains one percent per four unique Bag items")
// {
//     ClearBag();
//     AddBagItem(ITEM_POTION, 1);
//     AddBagItem(ITEM_ANTIDOTE, 1);
//     AddBagItem(ITEM_REPEL, 1);
//     EXPECT_EQ(GetMaterialisticBoostPercent(), 0);

//     AddBagItem(ITEM_ESCAPE_ROPE, 1);
//     EXPECT_EQ(GetMaterialisticBoostPercent(), 1);
// }

/* Eccentric and Tactical tests are pending their implementations.
   Left here as stubs to fill in once those natures are wired up.

TEST("Eccentric exchanges its Poke Ball with a different Ball from the Bag")
{
    struct Pokemon mon;
    u32 nature = NATURE_ECCENTRIC;
    u32 ball = BALL_GREAT;

    ClearBag();
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);
    SetMonData(&mon, MON_DATA_POKEBALL, &ball);
    AddBagItem(ITEM_ULTRA_BALL, 1);

    EXPECT(TrySwapEccentricPokeBall(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_POKEBALL), BALL_ULTRA);
    EXPECT(CheckBagHasItem(ITEM_GREAT_BALL, 1));
    EXPECT(!CheckBagHasItem(ITEM_ULTRA_BALL, 1));
}

TEST("Tactical grants one extra max PP only to move slot 4")
{
    struct Pokemon mon;
    u32 nature = NATURE_TACTICAL;

    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);
    SetMonMoveSlot(&mon, MOVE_TACKLE, 2);
    SetMonMoveSlot(&mon, MOVE_POUND, 3);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_PP3), GetMovePP(MOVE_TACKLE));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_PP4), GetMovePP(MOVE_POUND) + 1);
    EXPECT_EQ(CalculatePPWithBonusForMon(&mon, MOVE_POUND, 0, 3), GetMovePP(MOVE_POUND) + 1);
}
*/

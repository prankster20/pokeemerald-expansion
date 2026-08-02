#include "global.h"
#include "pokemon.h"
#include "test/test.h"

static void CreateBatchTwoMon(struct Pokemon *mon, enum Species species, u32 level, u32 nature)
{
    CreateMon(mon, species, level, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

TEST("pranks Benevolent boosts overworld healing for every party Pokemon")
{
    struct Pokemon target;

    CreateBatchTwoMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 50, NATURE_BENEVOLENT);
    CreateBatchTwoMon(&target, SPECIES_MIENFOO, 50, NATURE_HARDY);

    EXPECT_EQ(ApplyHealingNatureBoostsOverworld(&target, 100), 120);
}

TEST("pranks Delicate boosts its overworld healing by thirty percent")
{
    struct Pokemon target;

    CreateBatchTwoMon(&target, SPECIES_MIENFOO, 50, NATURE_DELICATE);
    EXPECT_EQ(ApplyHealingNatureBoostsOverworld(&target, 100), 130);
}

TEST("pranks Benevolent and Delicate overworld healing stack multiplicatively")
{
    struct Pokemon target;

    CreateBatchTwoMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 50, NATURE_BENEVOLENT);
    CreateBatchTwoMon(&target, SPECIES_MIENFOO, 50, NATURE_DELICATE);

    EXPECT_EQ(ApplyHealingNatureBoostsOverworld(&target, 100), 156);
}

TEST("pranks Nostalgic locks exactly its first two move slots")
{
    struct Pokemon mon;

    CreateBatchTwoMon(&mon, SPECIES_WOBBUFFET, 50, NATURE_NOSTALGIC);
    EXPECT(IsBoxMonMoveSlotLockedByNature(&mon.box, 0));
    EXPECT(IsBoxMonMoveSlotLockedByNature(&mon.box, 1));
    EXPECT(!IsBoxMonMoveSlotLockedByNature(&mon.box, 2));
    EXPECT(!IsBoxMonMoveSlotLockedByNature(&mon.box, 3));
}

TEST("pranks Loyal applies its earned boost only to Attack and Special Attack")
{
    struct Pokemon baseline;
    struct Pokemon loyal;
    u32 metLevel = 20;

    CreateBatchTwoMon(&baseline, SPECIES_MEW, 50, NATURE_HARDY);
    CreateBatchTwoMon(&loyal, SPECIES_MEW, 50, NATURE_LOYAL);
    SetMonData(&baseline, MON_DATA_MET_LEVEL, &metLevel);
    SetMonData(&loyal, MON_DATA_MET_LEVEL, &metLevel);
    CalculateMonStats(&baseline);
    CalculateMonStats(&loyal);

    EXPECT_EQ(GetLoyalBoostPercent(50, metLevel), 10);
    EXPECT_EQ(GetMonData(&loyal, MON_DATA_ATK), GetMonData(&baseline, MON_DATA_ATK) * 110 / 100);
    EXPECT_EQ(GetMonData(&loyal, MON_DATA_SPATK), GetMonData(&baseline, MON_DATA_SPATK) * 110 / 100);
    EXPECT_EQ(GetMonData(&loyal, MON_DATA_DEF), GetMonData(&baseline, MON_DATA_DEF));
    EXPECT_EQ(GetMonData(&loyal, MON_DATA_SPDEF), GetMonData(&baseline, MON_DATA_SPDEF));
    EXPECT_EQ(GetMonData(&loyal, MON_DATA_SPEED), GetMonData(&baseline, MON_DATA_SPEED));
}

TEST("pranks Observant stacking counts only non-Egg party Pokemon")
{
    CreateBatchTwoMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 50, NATURE_OBSERVANT);
    CreateBatchTwoMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_MIENFOO, 50, NATURE_OBSERVANT);
    CreateBatchTwoMon(&gParties[B_TRAINER_PLAYER][2], SPECIES_RALTS, 50, NATURE_HARDY);

    EXPECT_EQ(TestCountPlayerPartyMonsWithNature(NATURE_OBSERVANT), 2);
    EXPECT_EQ(GetObservantRerolls(TestCountPlayerPartyMonsWithNature(NATURE_OBSERVANT), 0), 2);
}

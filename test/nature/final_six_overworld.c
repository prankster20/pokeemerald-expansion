#include "global.h"
#include "pokemon.h"
#include "random.h"
#include "test/test.h"

static void ClearFinalSixTestParty(void)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);
}

static struct Pokemon *CreateFinalSixTestMon(u32 slot, enum Species species, u32 nature)
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];

    CreateMon(mon, species, 20, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    CalculateMonStats(mon);
    return mon;
}

TEST("pranks Adorable increases positive friendship gains by fifty percent")
{
    struct Pokemon *adorable;
    struct Pokemon *ordinary;
    u32 friendship = 0;

    ClearFinalSixTestParty();
    adorable = CreateFinalSixTestMon(0, SPECIES_RALTS, NATURE_ADORABLE);
    ordinary = CreateFinalSixTestMon(1, SPECIES_RALTS, NATURE_HARDY);
    SetMonData(adorable, MON_DATA_FRIENDSHIP, &friendship);
    SetMonData(ordinary, MON_DATA_FRIENDSHIP, &friendship);

    AdjustFriendship(adorable, FRIENDSHIP_EVENT_GROW_LEVEL);
    AdjustFriendship(ordinary, FRIENDSHIP_EVENT_GROW_LEVEL);

    EXPECT_EQ(GetMonData(adorable, MON_DATA_FRIENDSHIP), 8);
    EXPECT_EQ(GetMonData(ordinary, MON_DATA_FRIENDSHIP), 5);
}

TEST("pranks Pugnacious sparring damages neither participant below one HP and rewards both")
{
    struct Pokemon *pugnacious;
    struct Pokemon *partner;
    u32 oneHp = 1;
    u32 pugnaciousExp;
    u32 partnerExp;

    ClearFinalSixTestParty();
    pugnacious = CreateFinalSixTestMon(0, SPECIES_MIENFOO, NATURE_PUGNACIOUS);
    partner = CreateFinalSixTestMon(1, SPECIES_RALTS, NATURE_HARDY);
    SetMonData(pugnacious, MON_DATA_HP, &oneHp);
    SetMonData(partner, MON_DATA_HP, &oneHp);
    pugnaciousExp = GetMonData(pugnacious, MON_DATA_EXP);
    partnerExp = GetMonData(partner, MON_DATA_EXP);
    SET_RNG(RNG_NATURE_PUGNACIOUS_SPAR, 99);
    SET_RNG(RNG_NATURE_PUGNACIOUS_PARTNER, 0);
    SET_RNG(RNG_NATURE_PUGNACIOUS_EV, STAT_ATK);

    TryPugnaciousPartySparring();

    EXPECT_EQ(GetMonData(pugnacious, MON_DATA_HP), 1);
    EXPECT_EQ(GetMonData(partner, MON_DATA_HP), 1);
    EXPECT_GT(GetMonData(pugnacious, MON_DATA_EXP), pugnaciousExp);
    EXPECT_GT(GetMonData(partner, MON_DATA_EXP), partnerExp);
    EXPECT_EQ(GetMonData(pugnacious, MON_DATA_ATK_EV), 1);
    EXPECT_EQ(GetMonData(partner, MON_DATA_ATK_EV), 1);
}

TEST("pranks Wayfaring restores party HP and PP without reviving fainted Pokémon")
{
    struct Pokemon *wayfaring;
    struct Pokemon *ally;
    struct Pokemon *fainted;
    u32 hp;
    u32 pp;

    ClearFinalSixTestParty();
    wayfaring = CreateFinalSixTestMon(0, SPECIES_MIENFOO, NATURE_WAYFARING);
    ally = CreateFinalSixTestMon(1, SPECIES_RALTS, NATURE_HARDY);
    fainted = CreateFinalSixTestMon(2, SPECIES_WYNAUT, NATURE_HARDY);
    hp = GetMonData(ally, MON_DATA_MAX_HP) / 2;
    pp = 0;
    SetMonData(ally, MON_DATA_HP, &hp);
    SetMonData(ally, MON_DATA_PP1, &pp);
    SetMonData(fainted, MON_DATA_HP, &pp);

    ApplyWayfaringStepRecovery();

    EXPECT_GT(GetMonData(ally, MON_DATA_HP), hp);
    EXPECT_EQ(GetMonData(ally, MON_DATA_PP1), 1);
    EXPECT_EQ(GetMonData(fainted, MON_DATA_HP), 0);
    EXPECT_EQ(GetNature(wayfaring), NATURE_WAYFARING);
}

#include "global.h"
#include "item.h"
#include "party_menu.h"
#include "pokemon.h"
#include "shop.h"
#include "test/test.h"

static void CreateNatureMon(struct Pokemon *mon, enum Species species, u32 level, u32 nature)
{
    CreateMon(mon, species, level, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    CalculateMonStats(mon);
}

TEST("pranks Frivolous charges two thirds of the extra item's current shop price")
{
    u32 price = GetItemPrice(ITEM_POKE_BALL);

    EXPECT_EQ(TestGetFrivolousDiscountPrice(ITEM_POKE_BALL), (price * 2) / 3);
}

TEST("pranks Innocent friendship is reset while other natures are left alone")
{
    struct Pokemon innocent;
    struct Pokemon hardy;
    u32 friendship = 150;

    CreateNatureMon(&innocent, SPECIES_RALTS, 5, NATURE_INNOCENT);
    CreateNatureMon(&hardy, SPECIES_RALTS, 5, NATURE_HARDY);
    SetMonData(&innocent, MON_DATA_FRIENDSHIP, &friendship);
    SetMonData(&hardy, MON_DATA_FRIENDSHIP, &friendship);

    ApplyInnocentFriendshipRule(&innocent);
    ApplyInnocentFriendshipRule(&hardy);

    EXPECT_EQ(GetMonData(&innocent, MON_DATA_FRIENDSHIP), 0);
    EXPECT_EQ(GetMonData(&hardy, MON_DATA_FRIENDSHIP), 150);
}

TEST("pranks Pompous always doubles weight regardless of nickname")
{
    struct Pokemon mon;
    u8 oneChar[] = _("A");
    u8 sixChars[] = _("ABCDEF");
    u8 sevenChars[] = _("ABCDEFG");
    u8 tenChars[] = _("ABCDEFGHIJ");

    CreateNatureMon(&mon, SPECIES_WOBBUFFET, 50, NATURE_POMPOUS);
    SetMonData(&mon, MON_DATA_NICKNAME, oneChar);
    EXPECT_EQ(GetPompousWeightPercent(&mon), 100);
    SetMonData(&mon, MON_DATA_NICKNAME, sixChars);
    EXPECT_EQ(GetPompousWeightPercent(&mon), 100);
    SetMonData(&mon, MON_DATA_NICKNAME, sevenChars);
    EXPECT_EQ(GetPompousWeightPercent(&mon), 100);
    SetMonData(&mon, MON_DATA_NICKNAME, tenChars);
    EXPECT_EQ(GetPompousWeightPercent(&mon), 100);
}

TEST("pranks Supportive ignores empty slots eggs and the recipient itself")
{
    struct Pokemon party[PARTY_SIZE] = {0};

    CreateNatureMon(&party[0], SPECIES_WOBBUFFET, 50, NATURE_SUPPORTIVE);
    EXPECT_EQ(GetSupportiveBoostPercent(party, PARTY_SIZE, 0), 0);

    CreateNatureMon(&party[1], SPECIES_WYNAUT, 50, NATURE_SUPPORTIVE);
    EXPECT_EQ(GetSupportiveBoostPercent(party, PARTY_SIZE, 0), 5);
    EXPECT_EQ(GetSupportiveBoostPercent(party, PARTY_SIZE, 1), 5);
}

TEST("pranks Proud leaves all stats unchanged when highest and lowest are tied")
{
    struct Pokemon baseline;
    struct Pokemon proud;

    CreateNatureMon(&baseline, SPECIES_MEW, 50, NATURE_HARDY);
    CreateNatureMon(&proud, SPECIES_MEW, 50, NATURE_PROUD);

    EXPECT_EQ(GetMonData(&proud, MON_DATA_ATK), GetMonData(&baseline, MON_DATA_ATK));
    EXPECT_EQ(GetMonData(&proud, MON_DATA_DEF), GetMonData(&baseline, MON_DATA_DEF));
    EXPECT_EQ(GetMonData(&proud, MON_DATA_SPATK), GetMonData(&baseline, MON_DATA_SPATK));
    EXPECT_EQ(GetMonData(&proud, MON_DATA_SPDEF), GetMonData(&baseline, MON_DATA_SPDEF));
    EXPECT_EQ(GetMonData(&proud, MON_DATA_SPEED), GetMonData(&baseline, MON_DATA_SPEED));
}

TEST("pranks Devoted's closest bonded partner receives the same boost tier")
{
    struct Pokemon *devoted = &gParties[B_TRAINER_PLAYER][0];
    s32 bondSlot = -1;
    u32 nature = NATURE_DEVOTED;
    u32 devotedPersonality;

    for (u32 i = 0; i < PARTY_SIZE; i++)
        ZeroMonData(&gParties[B_TRAINER_PLAYER][i]);

    CreateMon(devoted, SPECIES_RALTS, 20, 100, OTID_STRUCT_PLAYER_ID);
    SetMonData(devoted, MON_DATA_HIDDEN_NATURE, &nature);
    devotedPersonality = GetMonData(devoted, MON_DATA_PERSONALITY);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_KIRLIA, 20, devotedPersonality + 9, OTID_STRUCT_PLAYER_ID);

    EXPECT_EQ(GetDevotedBondData(devoted, &bondSlot), 6);
    EXPECT_EQ(bondSlot, 1);
}

TEST("pranks Youthful is inactive before its evolution level and active at it")
{
    struct Pokemon mon;

    CreateNatureMon(&mon, SPECIES_TORCHIC, 15, NATURE_YOUTHFUL);
    EXPECT(!IsYouthfulNatureActive(&mon));
    CreateNatureMon(&mon, SPECIES_TORCHIC, 16, NATURE_YOUTHFUL);
    EXPECT(IsYouthfulNatureActive(&mon));
}

TEST("pranks Charitable alone receives the Care Package party action")
{
    EXPECT(TestNatureHasCarePackageAction(NATURE_OLD_CHARITABLE));
    EXPECT(!TestNatureHasCarePackageAction(NATURE_OLD_WAYFARING));
    EXPECT(!TestNatureHasCarePackageAction(NATURE_HARDY));
}

TEST("pranks Wayfaring alone receives the Head Home party action")
{
    EXPECT(TestNatureHasHeadHomeAction(NATURE_OLD_WAYFARING));
    EXPECT(!TestNatureHasHeadHomeAction(NATURE_OLD_CHARITABLE));
    EXPECT(!TestNatureHasHeadHomeAction(NATURE_HARDY));
}

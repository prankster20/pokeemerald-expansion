#include "global.h"
#include "event_data.h"
#include "pokemon.h"
#include "test/battle.h"

static void CreateNatureMon(struct Pokemon *mon, enum Species species, u32 level, u32 nature)
{
    u32 friendship = 0;

    CreateMon(mon, species, level, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
    CalculateMonStats(mon);
}

WILD_BATTLE_TEST("pranks Ambitious receives Badge Boosts when modern configurations disable them", s16 damage)
{
    u32 nature;
    PARAMETRIZE(nature = NATURE_HARDY);
    PARAMETRIZE(nature = NATURE_AMBITIOUS);

    GIVEN {
        WITH_CONFIG(B_BADGE_BOOST, GEN_LATEST);
        FlagSet(B_FLAG_BADGE_BOOST_ATTACK);
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
    }
}

WILD_BATTLE_TEST("pranks Ambitious receives the Special Attack Badge Boost", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_HARDY; }
    PARAMETRIZE { nature = NATURE_AMBITIOUS; }
    GIVEN {
        WITH_CONFIG(B_BADGE_BOOST, GEN_LATEST);
        FlagSet(B_FLAG_BADGE_BOOST_SPATK);
        PLAYER(SPECIES_MIENFOO) { SpAttack(100); Defense(100); MaxHP(999); HP(999); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); SpDefense(100); MaxHP(999); HP(999); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        HP_BAR(player);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
    }
}

WILD_BATTLE_TEST("pranks Ambitious receives the Defense Badge Boost", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_HARDY; }
    PARAMETRIZE { nature = NATURE_AMBITIOUS; }
    GIVEN {
        WITH_CONFIG(B_BADGE_BOOST, GEN_LATEST);
        FlagSet(B_FLAG_BADGE_BOOST_DEFENSE);
        PLAYER(SPECIES_MIENFOO) { Defense(100); MaxHP(999); HP(999); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); }
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.1), results[0].damage);
    }
}

TEST("pranks Communal counts shared traits once each and caps its boost at eight percent")
{
    struct Pokemon party[2] = {0};

    CreateNatureMon(&party[0], SPECIES_BULBASAUR, 50, NATURE_COMMUNAL);
    CreateNatureMon(&party[1], SPECIES_BULBASAUR, 50, NATURE_COMMUNAL);
    for (u32 i = 0; i < ARRAY_COUNT(party); i++)
    {
        SetMonMoveSlot(&party[i], MOVE_TACKLE, 0);
        SetMonMoveSlot(&party[i], MOVE_GROWL, 1);
        SetMonMoveSlot(&party[i], MOVE_VINE_WHIP, 2);
        SetMonMoveSlot(&party[i], MOVE_POISON_POWDER, 3);
    }

    EXPECT_EQ(GetCommunalBoostPercent(party, ARRAY_COUNT(party), 0), 8);
    EXPECT_EQ(GetCommunalBoostPercent(NULL, ARRAY_COUNT(party), 0), 0);
}

TEST("pranks Eclectic grants five percent per move of each category to the matching stat")
{
    struct Pokemon baseline;
    struct Pokemon eclectic;

    CreateNatureMon(&baseline, SPECIES_WOBBUFFET, 50, NATURE_HARDY);
    CreateNatureMon(&eclectic, SPECIES_WOBBUFFET, 50, NATURE_ECLECTIC);
    SetMonMoveSlot(&baseline, MOVE_TACKLE, 0);
    SetMonMoveSlot(&baseline, MOVE_WATER_GUN, 1);
    SetMonMoveSlot(&baseline, MOVE_CELEBRATE, 2);
    SetMonMoveSlot(&baseline, MOVE_NONE, 3);
    SetMonMoveSlot(&eclectic, MOVE_TACKLE, 0);
    SetMonMoveSlot(&eclectic, MOVE_WATER_GUN, 1);
    SetMonMoveSlot(&eclectic, MOVE_CELEBRATE, 2);
    SetMonMoveSlot(&eclectic, MOVE_NONE, 3);
    CalculateMonStats(&baseline);
    CalculateMonStats(&eclectic);

    EXPECT_EQ(GetMonData(&eclectic, MON_DATA_ATK), GetMonData(&baseline, MON_DATA_ATK) * 105 / 100);
    EXPECT_EQ(GetMonData(&eclectic, MON_DATA_SPATK), GetMonData(&baseline, MON_DATA_SPATK) * 105 / 100);
    EXPECT_EQ(GetMonData(&eclectic, MON_DATA_SPEED), GetMonData(&baseline, MON_DATA_SPEED) * 105 / 100);
    EXPECT_EQ(GetMonData(&eclectic, MON_DATA_DEF), GetMonData(&baseline, MON_DATA_DEF));
}

TEST("pranks Impressionable copies the preceding party member's stat ranking")
{
    u32 value;

    CreateNatureMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 50, NATURE_HARDY);
    CreateNatureMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_WYNAUT, 50, NATURE_OLD_IMPRESSIONABLE);
    value = 500; SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK, &value);
    value = 400; SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF, &value);
    value = 300; SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK, &value);
    value = 200; SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPDEF, &value);
    value = 100; SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED, &value);

    EXPECT_EQ(GetImpressionableStatBoostPercent(&gParties[B_TRAINER_PLAYER][1], STAT_ATK), 10);
    EXPECT_EQ(GetImpressionableStatBoostPercent(&gParties[B_TRAINER_PLAYER][1], STAT_DEF), 5);
    EXPECT_EQ(GetImpressionableStatBoostPercent(&gParties[B_TRAINER_PLAYER][1], STAT_SPATK), 0);
    EXPECT_EQ(GetImpressionableStatBoostPercent(&gParties[B_TRAINER_PLAYER][1], STAT_SPDEF), -5);
    EXPECT_EQ(GetImpressionableStatBoostPercent(&gParties[B_TRAINER_PLAYER][1], STAT_SPEED), -10);
}

TEST("pranks Loyal gains one percent per three levels since met and caps at fifteen percent")
{
    EXPECT_EQ(GetLoyalBoostPercent(10, 10), 0);
    EXPECT_EQ(GetLoyalBoostPercent(12, 10), 0);
    EXPECT_EQ(GetLoyalBoostPercent(13, 10), 1);
    EXPECT_EQ(GetLoyalBoostPercent(40, 10), 10);
    EXPECT_EQ(GetLoyalBoostPercent(100, 1), 15);
}

TEST("pranks Pompous maps nickname length to the documented weight offsets")
{
    struct Pokemon mon;
    u8 shortName[] = _("A");
    u8 longName[] = _("ABCDEFG");

    CreateNatureMon(&mon, SPECIES_WOBBUFFET, 50, NATURE_POMPOUS);
    SetMonData(&mon, MON_DATA_NICKNAME, shortName);
    EXPECT_EQ(GetPompousWeightPercent(&mon), 100);
    SetMonData(&mon, MON_DATA_NICKNAME, longName);
    EXPECT_EQ(GetPompousWeightPercent(&mon), 100);
}

TEST("pranks Proud boosts the highest stat and lowers the lowest stat by ten percent")
{
    struct Pokemon baseline;
    struct Pokemon proud;

    CreateNatureMon(&baseline, SPECIES_WOBBUFFET, 50, NATURE_HARDY);
    CreateNatureMon(&proud, SPECIES_WOBBUFFET, 50, NATURE_PROUD);

    EXPECT_EQ(GetMonData(&proud, MON_DATA_DEF), GetMonData(&baseline, MON_DATA_DEF) * 110 / 100);
    EXPECT_EQ(GetMonData(&proud, MON_DATA_ATK), GetMonData(&baseline, MON_DATA_ATK) * 90 / 100);
}

TEST("pranks Supportive starts at five percent and adds one percent per extra teammate")
{
    struct Pokemon party[4] = {0};

    CreateNatureMon(&party[0], SPECIES_WOBBUFFET, 50, NATURE_HARDY);
    CreateNatureMon(&party[1], SPECIES_WYNAUT, 50, NATURE_SUPPORTIVE);
    EXPECT_EQ(GetSupportiveBoostPercent(party, ARRAY_COUNT(party), 0), 5);
    CreateNatureMon(&party[2], SPECIES_ABRA, 50, NATURE_SUPPORTIVE);
    CreateNatureMon(&party[3], SPECIES_RALTS, 50, NATURE_SUPPORTIVE);
    EXPECT_EQ(GetSupportiveBoostPercent(party, ARRAY_COUNT(party), 0), 7);
    EXPECT_EQ(GetSupportiveBoostPercent(party, ARRAY_COUNT(party), 1), 6);
}

TEST("pranks Youthful activates at the evolution level and boosts both offenses")
{
    struct Pokemon baseline;
    struct Pokemon youthful;

    CreateNatureMon(&baseline, SPECIES_TORCHIC, 16, NATURE_HARDY);
    CreateNatureMon(&youthful, SPECIES_TORCHIC, 16, NATURE_YOUTHFUL);

    EXPECT(IsYouthfulNatureActive(&youthful));
    EXPECT_EQ(GetMonData(&youthful, MON_DATA_ATK), GetMonData(&baseline, MON_DATA_ATK) * 120 / 100);
    EXPECT_EQ(GetMonData(&youthful, MON_DATA_SPATK), GetMonData(&baseline, MON_DATA_SPATK) * 120 / 100);
    EXPECT_EQ(GetMonData(&youthful, MON_DATA_SPEED), GetMonData(&baseline, MON_DATA_SPEED));
    EXPECT_EQ(GetMonData(&youthful, MON_DATA_DEF), GetMonData(&baseline, MON_DATA_DEF));
}

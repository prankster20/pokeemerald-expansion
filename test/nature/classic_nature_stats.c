#include "global.h"
#include "pokemon.h"
#include "test/test.h"

TEST("pranks Classic Natures apply fifteen-percent boosts and penalties to the intended stats")
{
    u32 nature;
    enum Stat raisedStat;
    enum Stat loweredStat;
    enum Stat unchangedStat;

    PARAMETRIZE { nature = NATURE_LONELY;  raisedStat = STAT_ATK;   loweredStat = STAT_DEF;   unchangedStat = STAT_SPEED; }
    PARAMETRIZE { nature = NATURE_BRAVE;   raisedStat = STAT_ATK;   loweredStat = STAT_SPEED; unchangedStat = STAT_DEF; }
    PARAMETRIZE { nature = NATURE_ADAMANT; raisedStat = STAT_ATK;   loweredStat = STAT_SPATK; unchangedStat = STAT_SPDEF; }
    PARAMETRIZE { nature = NATURE_NAUGHTY; raisedStat = STAT_ATK;   loweredStat = STAT_SPDEF; unchangedStat = STAT_DEF; }
    PARAMETRIZE { nature = NATURE_BOLD;    raisedStat = STAT_DEF;   loweredStat = STAT_ATK;   unchangedStat = STAT_SPEED; }
    PARAMETRIZE { nature = NATURE_RELAXED; raisedStat = STAT_DEF;   loweredStat = STAT_SPEED; unchangedStat = STAT_SPDEF; }
    PARAMETRIZE { nature = NATURE_IMPISH;  raisedStat = STAT_DEF;   loweredStat = STAT_SPATK; unchangedStat = STAT_ATK; }
    PARAMETRIZE { nature = NATURE_LAX;     raisedStat = STAT_DEF;   loweredStat = STAT_SPDEF; unchangedStat = STAT_SPEED; }
    PARAMETRIZE { nature = NATURE_TIMID;   raisedStat = STAT_SPEED; loweredStat = STAT_ATK;   unchangedStat = STAT_DEF; }
    PARAMETRIZE { nature = NATURE_HASTY;   raisedStat = STAT_SPEED; loweredStat = STAT_DEF;   unchangedStat = STAT_SPATK; }
    PARAMETRIZE { nature = NATURE_JOLLY;   raisedStat = STAT_SPEED; loweredStat = STAT_SPATK; unchangedStat = STAT_SPDEF; }
    PARAMETRIZE { nature = NATURE_NAIVE;   raisedStat = STAT_SPEED; loweredStat = STAT_SPDEF; unchangedStat = STAT_ATK; }
    PARAMETRIZE { nature = NATURE_MODEST;  raisedStat = STAT_SPATK; loweredStat = STAT_ATK;   unchangedStat = STAT_DEF; }
    PARAMETRIZE { nature = NATURE_MILD;    raisedStat = STAT_SPATK; loweredStat = STAT_DEF;   unchangedStat = STAT_SPEED; }
    PARAMETRIZE { nature = NATURE_QUIET;   raisedStat = STAT_SPATK; loweredStat = STAT_SPEED; unchangedStat = STAT_SPDEF; }
    PARAMETRIZE { nature = NATURE_RASH;    raisedStat = STAT_SPATK; loweredStat = STAT_SPDEF; unchangedStat = STAT_ATK; }
    PARAMETRIZE { nature = NATURE_CALM;    raisedStat = STAT_SPDEF; loweredStat = STAT_ATK;   unchangedStat = STAT_DEF; }
    PARAMETRIZE { nature = NATURE_GENTLE;  raisedStat = STAT_SPDEF; loweredStat = STAT_DEF;   unchangedStat = STAT_SPEED; }
    PARAMETRIZE { nature = NATURE_SASSY;   raisedStat = STAT_SPDEF; loweredStat = STAT_SPEED; unchangedStat = STAT_ATK; }
    PARAMETRIZE { nature = NATURE_CAREFUL; raisedStat = STAT_SPDEF; loweredStat = STAT_SPATK; unchangedStat = STAT_DEF; }

    EXPECT_EQ(ModifyStatByNature(nature, 200, raisedStat, 0), 230);
    EXPECT_EQ(ModifyStatByNature(nature, 200, loweredStat, 0), 170);
    EXPECT_EQ(ModifyStatByNature(nature, 200, unchangedStat, 0), 200);
    EXPECT_EQ(ModifyStatByNature(nature, 200, STAT_HP, 0), 200);
}

TEST("pranks Serious and Bashful remain neutral for every non-HP stat")
{
    u32 nature;
    enum Stat stat;

    PARAMETRIZE { nature = NATURE_SERIOUS; stat = STAT_ATK; }
    PARAMETRIZE { nature = NATURE_SERIOUS; stat = STAT_DEF; }
    PARAMETRIZE { nature = NATURE_SERIOUS; stat = STAT_SPEED; }
    PARAMETRIZE { nature = NATURE_SERIOUS; stat = STAT_SPATK; }
    PARAMETRIZE { nature = NATURE_SERIOUS; stat = STAT_SPDEF; }
    PARAMETRIZE { nature = NATURE_BASHFUL; stat = STAT_ATK; }
    PARAMETRIZE { nature = NATURE_BASHFUL; stat = STAT_DEF; }
    PARAMETRIZE { nature = NATURE_BASHFUL; stat = STAT_SPEED; }
    PARAMETRIZE { nature = NATURE_BASHFUL; stat = STAT_SPATK; }
    PARAMETRIZE { nature = NATURE_BASHFUL; stat = STAT_SPDEF; }

    EXPECT_EQ(ModifyStatByNature(nature, 200, stat, 0), 200);
}

TEST("pranks Quirky apportions exactly twenty-five percent among its five stats")
{
    enum Stat stat;
    u32 expected;
    const u32 personality = 12345;

    PARAMETRIZE { stat = STAT_ATK;   expected = 204; }
    PARAMETRIZE { stat = STAT_DEF;   expected = 208; }
    PARAMETRIZE { stat = STAT_SPEED; expected = 216; }
    PARAMETRIZE { stat = STAT_SPATK; expected = 210; }
    PARAMETRIZE { stat = STAT_SPDEF; expected = 212; }

    EXPECT_EQ(ModifyStatByNature(NATURE_QUIRKY, 200, stat, personality), expected);
    EXPECT_EQ(ModifyStatByNature(NATURE_QUIRKY, 200, STAT_HP, personality), 200);
}

TEST("pranks Quirky caps every individual stat boost at eight percent")
{
    u32 total = 0;
    const u32 personality = 1111; // Displayed code 01111 strongly favors Attack.

    for (enum Stat stat = STAT_ATK; stat <= STAT_SPDEF; stat++)
    {
        u32 boost = GetQuirkyStatBoostPercent(personality, stat);

        EXPECT_LE(boost, 8);
        total += boost;
    }

    EXPECT_EQ(total, 25);
}

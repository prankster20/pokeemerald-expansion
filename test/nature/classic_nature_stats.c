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

TEST("pranks Quirky gives its five stats personality-derived boosts from one through five percent")
{
    enum Stat stat;
    u32 expected;
    const u32 personality = (0 << 0) | (1 << 3) | (2 << 6) | (3 << 9) | (4 << 12);

    PARAMETRIZE { stat = STAT_ATK;   expected = 202; }
    PARAMETRIZE { stat = STAT_DEF;   expected = 204; }
    PARAMETRIZE { stat = STAT_SPEED; expected = 206; }
    PARAMETRIZE { stat = STAT_SPATK; expected = 208; }
    PARAMETRIZE { stat = STAT_SPDEF; expected = 210; }

    EXPECT_EQ(ModifyStatByNature(NATURE_QUIRKY, 200, stat, personality), expected);
    EXPECT_EQ(ModifyStatByNature(NATURE_QUIRKY, 200, STAT_HP, personality), 200);
}

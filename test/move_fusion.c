#include "global.h"
#include "move_fusion.h"
#include "test/test.h"

TEST("Move fusion rejects using the same move as main and helper")
{
    EXPECT(!AreMovesFusionCompatible(MOVE_POUND, MOVE_POUND));
}

TEST("Unconfigured moves default to main-only")
{
    EXPECT(CanMoveBeFusionMain(MOVE_THUNDERBOLT));
    EXPECT(!CanMoveBeFusionHelper(MOVE_THUNDERBOLT));
}

TEST("Acid Spray plus Acid applies its BP and PP costs")
{
    struct ResolvedMoveFusion fusion;

    EXPECT(ResolveMoveFusion(MOVE_ACID_SPRAY, MOVE_ACID, &fusion));
    EXPECT_EQ(fusion.power, 20);
    EXPECT_EQ(fusion.pp, 15);
    EXPECT_EQ(fusion.superEffectiveType, TYPE_STEEL);
}

TEST("Power-Up Punch plus Double Hit becomes a half-power two-hit recipe")
{
    struct ResolvedMoveFusion fusion;

    EXPECT(ResolveMoveFusion(MOVE_POWER_UP_PUNCH, MOVE_DOUBLE_HIT, &fusion));
    EXPECT_EQ(fusion.power, 20);
    EXPECT_EQ(fusion.strikeCount, 2);
}

TEST("Bullet Seed cannot take the Double Hit helper")
{
    EXPECT(!AreMovesFusionCompatible(MOVE_BULLET_SEED, MOVE_DOUBLE_HIT));
}

TEST("Bullet Seed plus Assurance resolves to fifteen base power")
{
    struct ResolvedMoveFusion fusion;

    EXPECT(ResolveMoveFusion(MOVE_BULLET_SEED, MOVE_ASSURANCE, &fusion));
    EXPECT_EQ(fusion.power, 15);
    EXPECT_EQ(fusion.effect, EFFECT_ASSURANCE);
}

TEST("U-Turn helper caps PP at five")
{
    struct ResolvedMoveFusion fusion;

    EXPECT(ResolveMoveFusion(MOVE_TACKLE, MOVE_U_TURN, &fusion));
    EXPECT_EQ(fusion.pp, 5);
    EXPECT_EQ(fusion.effect, EFFECT_HIT_ESCAPE);
}

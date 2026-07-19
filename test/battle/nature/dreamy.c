#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

SINGLE_BATTLE_TEST("pranks Dreamy can act even while asleep")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Status1(STATUS1_SLEEP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        u32 nature = NATURE_DREAMY;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("Mienfoo is fast asleep.");
        MESSAGE("Mienfoo used Tackle!");
        HP_BAR(opponent);
    } THEN {
        // Sleep itself isn't cured by Dreamy - just doesn't block the move.
        EXPECT(gBattleMons[B_POSITION_PLAYER_LEFT].status1 & STATUS1_SLEEP);
    }
}

TEST("pranks Dreamy lowers calculated Speed to 0.9x")
{
    struct Pokemon mon;
    u32 nature = NATURE_DREAMY;
    u32 friendship = 0;

    CreateMon(&mon, SPECIES_MIENFOO, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_FRIENDSHIP, &friendship);
    CalculateMonStats(&mon);
    u32 neutralSpeed = GetMonData(&mon, MON_DATA_SPEED);

    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);
    CalculateMonStats(&mon);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPEED), neutralSpeed * 90 / 100);
}

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

SINGLE_BATTLE_TEST("pranks Dreamy lowers Speed by 10%")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_CELEBRATE); Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); Speed(95); }
        u32 nature = NATURE_DREAMY;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        // Without Dreamy's -10%, Mienfoo (100 Speed) would move first. With
        // it, Mienfoo's effective Speed (90) is below Wobbuffet's (95), so
        // Wobbuffet should move first instead.
        MESSAGE("The opposing Wobbuffet used Celebrate!");
        MESSAGE("Mienfoo used Celebrate!");
    }
}

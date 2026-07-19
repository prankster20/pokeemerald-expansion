#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveCategory(MOVE_TACKLE) == DAMAGE_CATEGORY_PHYSICAL);
}

WILD_BATTLE_TEST("pranks Noble always spares a wild Pokemon from fainting, once per battle")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Attack(999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2); Defense(1); }
        u32 nature = NATURE_NOBLE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT MESSAGE("Wobbuffet fainted!");
        HP_BAR(opponent, hp: 1);
        ABILITY_POPUP(player);
    }
}

SINGLE_BATTLE_TEST("pranks Noble's mercy does not apply against trainer battles")
{
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Attack(999); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2); Defense(1); }
        u32 nature = NATURE_NOBLE;
        SetMonData(&PLAYER_PARTY[0], MON_DATA_HIDDEN_NATURE, &nature);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, hp: 0);
        MESSAGE("The opposing Wobbuffet fainted!");
    }
}

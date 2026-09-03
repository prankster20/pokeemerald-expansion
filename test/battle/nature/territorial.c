#include "global.h"
#include "test/battle.h"

static void SetTestNature(struct Pokemon *mon, u32 nature)
{
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

SINGLE_BATTLE_TEST("pranks Territorial damages a foe that switches out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        SetTestNature(&OPPONENT_PARTY[0], NATURE_OLD_TERRITORIAL);
    } WHEN {
        TURN { SWITCH(player, 1); }
        TURN { SWITCH(player, 0); }
    } SCENE {
        HP_BAR(player, damage: 10);
    } THEN {
        EXPECT_EQ(player->hp, 150);
    }
}

SINGLE_BATTLE_TEST("pranks Territorial does not damage its ally switching out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(160); HP(160); }
        OPPONENT(SPECIES_WYNAUT);
        SetTestNature(&OPPONENT_PARTY[0], NATURE_OLD_TERRITORIAL);
    } WHEN {
        TURN { SWITCH(opponent, 1); }
    } THEN {
        EXPECT_EQ(GetMonData(&OPPONENT_PARTY[0], MON_DATA_HP), 160);
    }
}

SINGLE_BATTLE_TEST("pranks Territorial deals ten percent more damage to a foe that entered after it", s16 damage)
{
    u32 nature;
    PARAMETRIZE { nature = NATURE_HARDY; }
    PARAMETRIZE { nature = NATURE_TERRITORIAL; }
    GIVEN {
        PLAYER(SPECIES_MIENFOO) { Moves(MOVE_TACKLE); Attack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); MaxHP(999); HP(999); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); MaxHP(999); HP(999); }
        SetTestNature(&PLAYER_PARTY[0], nature);
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.1), results[1].damage);
    }
}

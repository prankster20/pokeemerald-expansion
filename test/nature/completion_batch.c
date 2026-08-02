#include "global.h"
#include "egg_hatch.h"
#include "move_relearner.h"
#include "pokemon.h"
#include "test/test.h"

static void CreateTestNatureMon(struct Pokemon *mon, enum Species species, u32 level, u32 nature)
{
    CreateMon(mon, species, level, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
}

static bool32 MoveListContains(const u16 *moves, u32 count, enum Move move)
{
    for (u32 i = 0; i < count; i++)
        if (moves[i] == move)
            return TRUE;
    return FALSE;
}

TEST("pranks Fastidious refuses every dirty held item and accepts clean items")
{
    struct Pokemon mon;
    enum Item item;
    bool32 refused;

    CreateTestNatureMon(&mon, SPECIES_WOBBUFFET, 50, NATURE_FASTIDIOUS);
    PARAMETRIZE { item = ITEM_FLAME_ORB;    refused = TRUE; }
    PARAMETRIZE { item = ITEM_TOXIC_ORB;    refused = TRUE; }
    PARAMETRIZE { item = ITEM_BLACK_SLUDGE; refused = TRUE; }
    PARAMETRIZE { item = ITEM_LEFTOVERS;    refused = FALSE; }
    PARAMETRIZE { item = ITEM_NONE;         refused = FALSE; }

    EXPECT_EQ(DoesBoxMonNatureRefuseHeldItem(&mon.box, item), refused);
}

TEST("pranks Fastidious cleans every non-Egg party member after battle")
{
    u32 status = STATUS1_BURN;

    CreateTestNatureMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 50, NATURE_FASTIDIOUS);
    CreateTestNatureMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_MIENFOO, 50, NATURE_HARDY);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS, &status);
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_STATUS, &status);

    TryFastidiousCleanPartyStatusAfterBattle();

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS), STATUS1_NONE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_STATUS), STATUS1_NONE);
}

TEST("pranks Energetic makes Eggs lose two cycles per hatch step")
{
    CreateTestNatureMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WOBBUFFET, 50, NATURE_ENERGETIC);
    EXPECT_EQ(GetEggCyclesToSubtract(), 2);

    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HIDDEN_NATURE, &(u32){NATURE_HARDY});
    EXPECT_EQ(GetEggCyclesToSubtract(), 1);
}

TEST("pranks Shortsighted refuses future-turn and two-turn moves")
{
    struct Pokemon mon;
    enum Move move;
    bool32 refused;

    CreateTestNatureMon(&mon, SPECIES_WOBBUFFET, 50, NATURE_SHORTSIGHTED);
    PARAMETRIZE { move = MOVE_FUTURE_SIGHT; refused = TRUE; }
    PARAMETRIZE { move = MOVE_WISH;         refused = TRUE; }
    PARAMETRIZE { move = MOVE_FLY;          refused = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE;       refused = FALSE; }

    EXPECT_EQ(DoesBoxMonNatureRefuseMove(&mon.box, move), refused);
}

TEST("pranks Pack Rat can always relearn Stockpile")
{
    struct Pokemon mon;
    u16 moves[MAX_RELEARNER_MOVES] = {0};
    u32 count;

    CreateTestNatureMon(&mon, SPECIES_MIENFOO, 10, NATURE_PACK_RAT);
    count = TestGetRelearnerLevelUpMoves(&mon.box, moves);

    EXPECT(MoveListContains(moves, count, MOVE_STOCKPILE));
}

TEST("pranks Prodigious can relearn a level-up move one level early")
{
    struct Pokemon mon;
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(SPECIES_BULBASAUR);
    u16 moves[MAX_RELEARNER_MOVES] = {0};
    enum Move nextMove = MOVE_NONE;
    u32 nextLevel = 0;
    u32 count;

    for (u32 i = 0; i < MAX_LEVEL_UP_MOVES && learnset[i].move != LEVEL_UP_MOVE_END; i++)
    {
        if (learnset[i].level > 1)
        {
            nextMove = learnset[i].move;
            nextLevel = learnset[i].level;
            break;
        }
    }

    ASSUME(nextMove != MOVE_NONE);
    CreateTestNatureMon(&mon, SPECIES_BULBASAUR, nextLevel - 1, NATURE_PRODIGIOUS);
    count = TestGetRelearnerLevelUpMoves(&mon.box, moves);

    EXPECT(MoveListContains(moves, count, nextMove));
}

TEST("pranks Scholarly can relearn its base species Egg Moves")
{
    struct Pokemon mon;
    u16 moves[MAX_RELEARNER_MOVES] = {0};
    const u16 *eggMoves = GetSpeciesEggMoves(SPECIES_BULBASAUR);
    u32 count;

    CreateTestNatureMon(&mon, SPECIES_VENUSAUR, 50, NATURE_SCHOLARLY);
    count = TestGetRelearnerLevelUpMoves(&mon.box, moves);

    ASSUME(eggMoves[0] != MOVE_UNAVAILABLE);
    EXPECT(MoveListContains(moves, count, eggMoves[0]));
}

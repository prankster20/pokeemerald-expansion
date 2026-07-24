#include "global.h"
#include "random.h"
#include "test/test.h"
#include "wild_encounter.h"

TEST("pranks Worldly mirrors aggregated species rarity rather than individual slots")
{
    struct WildPokemon mons[12];
    u32 weights[12];
    u32 rareSpeciesCount = 0;

    // Common species: 8 * 10 = 80 normal weight.
    // Rare species:   4 *  5 = 20 normal weight.
    // Worldly mirrors those aggregate weights to 20:80.
    for (u32 i = 0; i < ARRAY_COUNT(mons); i++)
    {
        mons[i].minLevel = 5;
        mons[i].maxLevel = 5;
        mons[i].species = i < 8 ? SPECIES_WURMPLE : SPECIES_RALTS;
        weights[i] = i < 8 ? 10 : 5;
    }

    SeedRng(1);
    for (u32 i = 0; i < 10000; i++)
    {
        u32 slot = TestChooseWorldlyWildMonIndex(mons, 0, ARRAY_COUNT(mons), weights);
        if (mons[slot].species == SPECIES_RALTS)
            rareSpeciesCount++;
    }

    EXPECT_GE(rareSpeciesCount, 7800);
    EXPECT_LE(rareSpeciesCount, 8200);
}

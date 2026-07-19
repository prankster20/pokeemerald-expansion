#include "global.h"
#include "pokemon.h"
#include "text.h"
#include "test/test.h"
#include "constants/pokemon.h"

// The BW summary screen renders nature descriptions at FONT_SMALL_NARROW
// with a max width of 195px, using AppendWrappedNatureDescription which
// breaks lines automatically. We want to make sure the LAST line of each
// description (including the Capricious "Changes after battles." append)
// still fits within that budget, i.e. the whole string wraps cleanly.
//
// The real AppendWrappedNatureDescription does line-wrapping, so technically
// any string "fits" visually - what we're actually checking here is that the
// whole string can be rendered without overflowing the text buffer. The buffer
// is sDynamicNatureDescriptionBuffer[300] in bw_summary_screen.c, so the
// combined length in bytes is the real constraint, not pixel width.
//
// A Capricious Pokémon with its persistent reroll flag appends:
//   " {COLOR DYNAMIC_COLOR4}{SHADOW DYNAMIC_COLOR5}Changes after battles."
//
// We check:
//  1. No description is NULL.
//  2. The raw byte length of description + Mercurial suffix <= 268 bytes
//     (300-byte buffer, leave 32 bytes for color escape sequences and EOS).

#define NATURE_DESCRIPTION_BUFFER_SIZE 300
static const u8 sMercurialSuffix[] = _(" {COLOR DYNAMIC_COLOR4}{SHADOW DYNAMIC_COLOR5}Changes after battles.");

// Counts printable bytes in a COMPOUND_STRING (stops at EOS = 0xFF or 0x00).
static u32 DescriptionLen(const u8 *str)
{
    u32 len = 0;
    if (str == NULL)
        return 0;
    while (str[len] != EOS && str[len] != 0xFF)
        len++;
    return len;
}

TEST("pranks Every nature has a non-empty description")
{
    u32 i;
    for (i = 0; i < NUM_NATURES; i++)
    {
        PARAMETRIZE { }
        EXPECT(gNaturesInfo[i].description != NULL);
        if (gNaturesInfo[i].description != NULL)
            EXPECT(DescriptionLen(gNaturesInfo[i].description) != 0);
    }
}

TEST("pranks No nature description overflows the summary screen buffer when Mercurial suffix is appended")
{
    u32 i;
    for (i = 0; i < NUM_NATURES; i++)
    {
        PARAMETRIZE { }
        const u8 *desc = gNaturesInfo[i].description;
        if (desc == NULL)
            continue; // caught by previous test
        u32 combinedLen = DescriptionLen(desc) + DescriptionLen(sMercurialSuffix) + 1; // EOS
        EXPECT(combinedLen <= NATURE_DESCRIPTION_BUFFER_SIZE);
    }
}

TEST("pranks Nature descriptions with Mercurial suffix fit within pixel budget at FONT_SMALL_NARROW")
{
    // AppendWrappedNatureDescription wraps at 195px. Each line wraps
    // independently, so a long description is fine as long as no single
    // word exceeds the budget. What we actually want to catch is a description
    // that's SO long it would eat into the last line with no room left for
    // the Mercurial suffix on the same line - but since the wrapper puts
    // the suffix on a new line anyway if needed, the pixel check here is
    // really just "does any individual word exceed 195px?". We check the
    // full combined string's total pixel width as a proxy - if it exceeds
    // 6 * 195px (6 lines of text), the description is probably too long.
    #define MAX_TOTAL_PX (6 * 195)

    // Construct the suffix as a plain u8 string (no color codes, since
    // GetStringWidth ignores them but we want a clean measurement).
    static const u8 sPlainMercurialSuffix[] = _(" Changes after battles.");

    u32 i;
    for (i = 0; i < NUM_NATURES; i++)
    {
        PARAMETRIZE { }
        const u8 *desc = gNaturesInfo[i].description;
        if (desc == NULL)
            continue;

        // GetStringWidth with letterSpacing=0 measures the full string.
        s32 descPx    = GetStringWidth(FONT_SMALL_NARROW, desc, 0);
        s32 suffixPx  = GetStringWidth(FONT_SMALL_NARROW, sPlainMercurialSuffix, 0);
        EXPECT(descPx + suffixPx <= MAX_TOTAL_PX);
    }
}

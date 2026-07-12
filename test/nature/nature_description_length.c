#include "global.h"
#include "pokemon.h"
#include "text.h"
#include "test/test.h"
#include "constants/pokemon.h"

// The BW summary screen renders nature descriptions at FONT_SMALL_NARROW
// with a max width of 195px, using AppendWrappedNatureDescription which
// breaks lines automatically. We want to make sure the LAST line of each
// description (including the Mercurial "Changes after every battle." append)
// still fits within that budget, i.e. the whole string wraps cleanly.
//
// The real AppendWrappedNatureDescription does line-wrapping, so technically
// any string "fits" visually - what we're actually checking here is that the
// whole string can be rendered without overflowing the text buffer. The buffer
// is sDynamicNatureDescriptionBuffer[300] in bw_summary_screen.c, so the
// combined length in bytes is the real constraint, not pixel width.
//
// Mercurial appends (with color codes stripped out, since those are zero-width):
//   " Changes after every battle."
// = 29 printable characters + 1 space = 30 chars.
//
// We check:
//  1. No description is NULL.
//  2. The raw byte length of description + Mercurial suffix <= 268 bytes
//     (300-byte buffer, leave 32 bytes for color escape sequences and EOS).

#define MERCURIAL_SUFFIX " Changes after every battle."
#define MERCURIAL_SUFFIX_LEN 30   // strlen of the above, not counting EOS

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

TEST("Every nature has a non-NULL description")
{
    u32 i;
    for (i = 0; i < NUM_NATURES; i++)
    {
        PARAMETRIZE { }
        EXPECT(gNaturesInfo[i].description != NULL);
    }
}

TEST("No nature description overflows the summary screen buffer when Mercurial suffix is appended")
{
    // The dynamic buffer is 300 bytes. Color escapes for the Mercurial suffix
    // add ~20 bytes of non-printable control codes on top of the 30 printable
    // chars. Leave 50 bytes of headroom for color codes + other dynamic content
    // (e.g. Devoted's nickname insertion). So effective limit on raw description:
    // 300 - 30 (Mercurial printable) - 20 (color codes) - 50 (headroom) = 200 bytes.
    //
    // In practice all descriptions should be well under this; if one hits it,
    // the author needs to shorten the description text.
    #define MAX_DESCRIPTION_BYTES 200

    u32 i;
    for (i = 0; i < NUM_NATURES; i++)
    {
        PARAMETRIZE { }
        const u8 *desc = gNaturesInfo[i].description;
        if (desc == NULL)
            continue; // caught by previous test
        u32 len = DescriptionLen(desc);
        // Print which nature failed if this trips - the PARAMETRIZE index maps
        // directly to the nature ID so you can cross-reference pokemon_constants.h.
        EXPECT(len <= MAX_DESCRIPTION_BYTES);
    }
}

TEST("Nature descriptions with Mercurial suffix fit within pixel budget at FONT_SMALL_NARROW")
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
    static const u8 sMercurialSuffix[] = _(" Changes after every battle.");

    u32 i;
    for (i = 0; i < NUM_NATURES; i++)
    {
        PARAMETRIZE { }
        const u8 *desc = gNaturesInfo[i].description;
        if (desc == NULL)
            continue;

        // GetStringWidth with letterSpacing=0 measures the full string.
        s32 descPx    = GetStringWidth(FONT_SMALL_NARROW, desc, 0);
        s32 suffixPx  = GetStringWidth(FONT_SMALL_NARROW, sMercurialSuffix, 0);
        EXPECT(descPx + suffixPx <= MAX_TOTAL_PX);
    }
}

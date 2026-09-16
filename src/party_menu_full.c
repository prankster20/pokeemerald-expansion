#include "global.h"
#include "bg.h"
#include "palette.h"
#include "party_menu_full.h"
#include "sprite.h"

// The experimental full-grid C party menu is currently disabled.  Keep its
// source available for a future rebuild, but do not compile its resources or
// implementation merely because the build system automatically compiles every
// .c file under src/.  The regular party_menu.c implementation remains active.
#if 0

// Native-C port of Stracker's scrambled-rs full party-screen resources and
// layout (rust/src/party_screen.rs on the scrambled-rs branch):
// https://github.com/Strackeror/scrambled-emerald/tree/scrambled-rs/rust
// UI control remains in party_menu.c so every expansion party action,
// including field moves and project-specific actions, remains available.
static const u32 sFullPartyTiles[] = INCBIN_U32("graphics/party_menu_full/tiles.4bpp");
static const u16 sFullPartyPalettes[] = INCBIN_U16("graphics/party_menu_full/tiles.gbapal");
static const u16 sFullPartyScrollMap[] = INCBIN_U16("graphics/party_menu_full/bg.bin");
static const u16 sFullPartyCardMap[] = INCBIN_U16("graphics/party_menu_full/mon_bg.bin");
static const u32 sFullPartyStatusIcons[] = INCBIN_U32("graphics/party_menu_full/status_icons.4bpp");
static const u16 sFullPartyStatusPalette[] = INCBIN_U16("graphics/party_menu_full/status_icons.gbapal");

static const u8 sCardPositions[PARTY_SIZE][2] =
{
    { 1,  1}, {11,  1}, {21,  1},
    { 1, 10}, {11, 10}, {21, 10},
};

static const u8 sFullPartyTextColors[] = {0, 4, 7};

void FullPartyMenu_LoadTiles(u8 fixedBg, u8 scrollBg)
{
    LoadBgTiles(fixedBg, sFullPartyTiles, sizeof(sFullPartyTiles), 0);
    LoadBgTiles(scrollBg, sFullPartyTiles, sizeof(sFullPartyTiles), 0);
}

void FullPartyMenu_LoadPalettes(void)
{
    u32 i;

    for (i = 0; i < PARTY_SIZE; i++)
        LoadPalette(sFullPartyPalettes, BG_PLTT_ID(i), PLTT_SIZE_4BPP);
    LoadPalette(sFullPartyPalettes, BG_PLTT_ID(FULL_PARTY_SCROLL_PALETTE), PLTT_SIZE_4BPP);
    LoadPalette(sFullPartyPalettes, BG_PLTT_ID(FULL_PARTY_CARD_PALETTE_BASE), sizeof(sFullPartyPalettes));
}

void FullPartyMenu_CopyScrollTilemap(u16 *dest)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sFullPartyScrollMap); i++)
        dest[i] = (sFullPartyScrollMap[i] & 0x0FFF) | (FULL_PARTY_SCROLL_PALETTE << 12);
}

void FullPartyMenu_GetCardTilePosition(u8 slot, u8 *left, u8 *top)
{
    if (slot >= PARTY_SIZE)
        slot = 0;
    *left = sCardPositions[slot][0];
    *top = sCardPositions[slot][1];
}

void FullPartyMenu_DrawCard(u16 *dest, u8 slot, enum FullPartyCardStyle style)
{
    u32 row, column;
    u8 left, top;

    FullPartyMenu_GetCardTilePosition(slot, &left, &top);
    for (row = 0; row < FULL_PARTY_CARD_HEIGHT; row++)
    {
        for (column = 0; column < FULL_PARTY_CARD_WIDTH; column++)
        {
            u16 tile = sFullPartyCardMap[row * FULL_PARTY_CARD_WIDTH + column];
            dest[(top + row) * 32 + left + column]
                = (tile & 0x0FFF) | ((FULL_PARTY_CARD_PALETTE_BASE + style) << 12);
        }
    }
}

void FullPartyMenu_ClearCard(u16 *dest, u8 slot)
{
    u32 row, column;
    u8 left, top;

    FullPartyMenu_GetCardTilePosition(slot, &left, &top);
    for (row = 0; row < FULL_PARTY_CARD_HEIGHT; row++)
    {
        for (column = 0; column < FULL_PARTY_CARD_WIDTH; column++)
            dest[(top + row) * 32 + left + column] = 0;
    }
}

void FullPartyMenu_SetCardStyle(u16 *dest, u8 slot, enum FullPartyCardStyle style)
{
    u32 row, column;
    u8 left, top;

    FullPartyMenu_GetCardTilePosition(slot, &left, &top);
    for (row = 0; row < FULL_PARTY_CARD_HEIGHT; row++)
    {
        for (column = 0; column < FULL_PARTY_CARD_WIDTH; column++)
        {
            u16 *tile = &dest[(top + row) * 32 + left + column];
            *tile = (*tile & 0x0FFF) | ((FULL_PARTY_CARD_PALETTE_BASE + style) << 12);
        }
    }
}

const u8 *FullPartyMenu_GetTextColors(void)
{
    return sFullPartyTextColors;
}

void FullPartyMenu_LoadStatusIcons(u16 tileTag, u16 paletteTag)
{
    const struct SpriteSheet sheet =
    {
        .data = sFullPartyStatusIcons,
        .size = sizeof(sFullPartyStatusIcons),
        .tag = tileTag,
    };
    const struct SpritePalette palette =
    {
        .data = sFullPartyStatusPalette,
        .tag = paletteTag,
    };

    LoadSpriteSheet(&sheet);
    LoadSpritePalette(&palette);
}

#endif // Disabled experimental full-grid party menu

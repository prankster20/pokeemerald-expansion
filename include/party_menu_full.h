#ifndef GUARD_PARTY_MENU_FULL_H
#define GUARD_PARTY_MENU_FULL_H

#include "global.h"

enum FullPartyCardStyle
{
    FULL_PARTY_CARD_FOCUSED,
    FULL_PARTY_CARD_UNFOCUSED,
    FULL_PARTY_CARD_SWITCH_FOCUSED,
    FULL_PARTY_CARD_SWITCH_UNFOCUSED,
    FULL_PARTY_CARD_FAINTED_FOCUSED,
    FULL_PARTY_CARD_FAINTED_UNFOCUSED,
};

#define FULL_PARTY_CARD_WIDTH  9
#define FULL_PARTY_CARD_HEIGHT 8
#define FULL_PARTY_TILE_COUNT  20
#define FULL_PARTY_SCROLL_PALETTE 6
#define FULL_PARTY_CARD_PALETTE_BASE 7

void FullPartyMenu_LoadTiles(u8 fixedBg, u8 scrollBg);
void FullPartyMenu_LoadPalettes(void);
void FullPartyMenu_CopyScrollTilemap(u16 *dest);
void FullPartyMenu_DrawCard(u16 *dest, u8 slot, enum FullPartyCardStyle style);
void FullPartyMenu_ClearCard(u16 *dest, u8 slot);
void FullPartyMenu_SetCardStyle(u16 *dest, u8 slot, enum FullPartyCardStyle style);
void FullPartyMenu_GetCardTilePosition(u8 slot, u8 *left, u8 *top);
const u8 *FullPartyMenu_GetTextColors(void);
void FullPartyMenu_LoadStatusIcons(u16 tileTag, u16 paletteTag);

#endif // GUARD_PARTY_MENU_FULL_H

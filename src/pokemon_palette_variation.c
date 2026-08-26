#include "global.h"
#include "constants/rgb.h"
#include "palette.h"
#include "pokemon_palette_variation.h"
#include "sprite.h"

#if P_PERSONALITY_PALETTE_VARIATION
static s32 ClampColorComponent(s32 component)
{
    if (component < 0)
        return 0;
    if (component > 31)
        return 31;
    return component;
}

static u32 MixPersonality(u32 personality)
{
    // This mixer prevents neighboring personality values from producing nearly
    // identical colors. It does not modify or store anything in the Pokémon.
    personality ^= personality >> 16;
    personality *= 0x7FEB352D;
    personality ^= personality >> 15;
    personality *= 0x846CA68B;
    return personality ^ (personality >> 16);
}

static u16 VaryColor(u16 color, s32 hue, s32 brightness)
{
    s32 red, green, blue;
    s32 oldRed, oldGreen, oldBlue;

    color &= 0x7FFF;
    if (color == RGB_BLACK)
        return color; // Preserve outlines and fully transparent/black entries.

    oldRed = red = color & 31;
    oldGreen = green = (color >> 5) & 31;
    oldBlue = blue = (color >> 10) & 31;

    // A cheap cyclic RGB rotation suitable for GBA hardware. The strength is
    // deliberately small, so species remain immediately recognizable.
    red   += (hue * (oldGreen - oldBlue)) / 16;
    green += (hue * (oldBlue - oldRed)) / 16;
    blue  += (hue * (oldRed - oldGreen)) / 16;

    red = ClampColorComponent(red + brightness);
    green = ClampColorComponent(green + brightness);
    blue = ClampColorComponent(blue + brightness);
    return RGB(red, green, blue);
}

static void VaryPaletteBuffer(u16 *palette, u16 paletteOffset, s32 hue, s32 brightness)
{
    u32 i;

    // Entry zero is transparent for 4bpp sprites and must never be changed.
    for (i = 1; i < 16; i++)
        palette[paletteOffset + i] = VaryColor(palette[paletteOffset + i], hue, brightness);
}
#endif

void ApplyPersonalityPaletteVariation(u16 paletteOffset, u32 personality)
{
#if P_PERSONALITY_PALETTE_VARIATION
    u32 mixed;
    s32 hue;
    s32 brightness;

    if (paletteOffset + 16 > PLTT_BUFFER_SIZE)
        return;

    mixed = MixPersonality(personality);
    hue = (s32)(mixed % (P_PERSONALITY_HUE_STRENGTH * 2 + 1)) - P_PERSONALITY_HUE_STRENGTH;
    brightness = (s32)((mixed >> 8) % (P_PERSONALITY_BRIGHTNESS_RANGE * 2 + 1)) - P_PERSONALITY_BRIGHTNESS_RANGE;

    VaryPaletteBuffer(gPlttBufferUnfaded, paletteOffset, hue, brightness);
    VaryPaletteBuffer(gPlttBufferFaded, paletteOffset, hue, brightness);
#else
    (void)paletteOffset;
    (void)personality;
#endif
}

void ApplyPersonalityPaletteVariationByTag(u16 paletteTag, u32 personality)
{
#if P_PERSONALITY_PALETTE_VARIATION
    u32 paletteIndex = IndexOfSpritePaletteTag(paletteTag);

    if (paletteIndex != 0xFF)
        ApplyPersonalityPaletteVariation(OBJ_PLTT_ID(paletteIndex), personality);
#else
    (void)paletteTag;
    (void)personality;
#endif
}

#ifndef GUARD_CONFIG_PERSONALITY_COLORS_H
#define GUARD_CONFIG_PERSONALITY_COLORS_H

// Hue is measured around a 1536-unit color wheel. 256 units is 60 degrees.
// Ordinary personality colors range from -15 to +15 degrees.
#define PERSONALITY_COLOR_HUE_RANGE                64
#define PERSONALITY_COLOR_BRIGHTNESS_RANGE          1

#define PERSONALITY_CODE_MODULUS                100000

#define VIBRANT_COLOR_HUE_MIN                     128 // 30 degrees
#define VIBRANT_COLOR_HUE_MAX                    1408 // 330 degrees
#define VIBRANT_COLOR_BRIGHTNESS_MIN               -3
#define VIBRANT_COLOR_BRIGHTNESS_MAX                3

// Keep outlines, deep shadows, whites, greys, and other nearly-neutral
// details from being brightened or darkened. Hue shifting still applies to
// every pixel; it naturally has no visible effect on true neutral colors.
// Value is 0-31; saturation is 0-255.
#define PERSONALITY_COLOR_MIN_VALUE                 4
#define PERSONALITY_COLOR_MIN_SATURATION           32

#endif // GUARD_CONFIG_PERSONALITY_COLORS_H

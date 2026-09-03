#include "global.h"
#include "encounter_sign.h"
#include "event_data.h"
#include "main.h"
#include "menu.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "script.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "window.h"
#include "constants/songs.h"
#include "constants/species.h"
#include "constants/field_specials.h"

// Encounter-sign layout. These are intentionally kept together so the grid
// can be rearranged without touching the drawing logic below.
#define GRID_COLUMNS           3
#define GRID_ROWS              2
#define ENTRIES_PER_PAGE       (GRID_COLUMNS * GRID_ROWS)
#define GRID_COLUMN_WIDTH      74
#define GRID_FIRST_CENTER_X    38
#define GRID_ROW_HEIGHT        58
#define GRID_FIRST_ICON_Y      50
#define GRID_FIRST_NAME_Y      60
#define GRID_FIRST_RATE_Y      70
#define PAGE_LABEL_Y           18
#define ICON_COUNT             ENTRIES_PER_PAGE

struct EncounterSignEntry
{
    enum Species species;
    u8 rate;
};

struct EncounterSignRow
{
    const u8 *label;
    u8 count;
    struct EncounterSignEntry entries[ENTRIES_PER_PAGE];
};

struct EncounterSign
{
    const u8 *title;
    u8 rowCount;
    const struct EncounterSignRow *rows;
};

static const struct EncounterSignRow sRoute101Rows[] =
{
    {
        .label = COMPOUND_STRING("Day"),
        .count = 2,
        .entries =
        {
            { SPECIES_FLETCHLING, 50 },
            { SPECIES_BLIPBUG, 50 },
        },
    },
    {
        .label = COMPOUND_STRING("Night"),
        .count = 2,
        .entries =
        {
            { SPECIES_STARLY, 50 },
            { SPECIES_NYMBLE, 50 },
        },
    },
};

static const struct EncounterSign sEncounterSigns[ENCOUNTER_SIGN_COUNT] =
{
    [ENCOUNTER_SIGN_ROUTE_101] =
    {
        .title = COMPOUND_STRING("Route 101"),
        .rowCount = ARRAY_COUNT(sRoute101Rows),
        .rows = sRoute101Rows,
    },
};

static const struct WindowTemplate sEncounterSignWindow =
{
    .bg = 0,
    .tilemapLeft = 1,
    .tilemapTop = 1,
    .width = 28,
    .height = 18,
    .paletteNum = 15,
    .baseBlock = 100,
};

static EWRAM_DATA u8 sEncounterSignWindowId = 0;
static EWRAM_DATA u8 sEncounterSignSpriteIds[ICON_COUNT] = {0};

static void Task_EncounterSignInput(u8 taskId);
static void DrawEncounterSignPage(u8 taskId);
static void DestroyEncounterSignIcons(void);
static void CloseEncounterSign(u8 taskId);

static void PrintCentered(u8 fontId, const u8 *text, u16 center, u8 y, u16 maxWidth)
{
    u16 width = GetStringWidth(fontId, text, 0);
    u16 x = (width < maxWidth) ? center - width / 2 : center - maxWidth / 2;

    AddTextPrinterParameterized(sEncounterSignWindowId, fontId, text, x, y, TEXT_SKIP_DRAW, NULL);
}

static void DestroyEncounterSignIcons(void)
{
    u32 i;

    for (i = 0; i < ICON_COUNT; i++)
    {
        if (sEncounterSignSpriteIds[i] != MAX_SPRITES)
        {
            FreeAndDestroyMonIconSprite(&gSprites[sEncounterSignSpriteIds[i]]);
            sEncounterSignSpriteIds[i] = MAX_SPRITES;
        }
    }
}

static void DrawEncounterSignPage(u8 taskId)
{
    const struct EncounterSign *sign = &sEncounterSigns[gTasks[taskId].data[1]];
    u8 page = gTasks[taskId].data[0];
    u8 pageCount = sign->rowCount;
    const struct EncounterSignRow *encounterPage = &sign->rows[page];
    u8 entryId;
    u8 text[16];

    DestroyEncounterSignIcons();
    FillWindowPixelBuffer(sEncounterSignWindowId, PIXEL_FILL(1));
    PrintCentered(FONT_NORMAL, sign->title, 112, 0, 176);

    if (pageCount > 1)
    {
        ConvertIntToDecimalStringN(text, page + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
        StringAppend(text, COMPOUND_STRING("/"));
        ConvertIntToDecimalStringN(text + StringLength(text), pageCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        AddTextPrinterParameterized(sEncounterSignWindowId, FONT_SMALL, text, 198, 4, TEXT_SKIP_DRAW, NULL);
        AddTextPrinterParameterized(sEncounterSignWindowId, FONT_SMALL, COMPOUND_STRING("{LEFT_ARROW}"), 4, 4, TEXT_SKIP_DRAW, NULL);
        AddTextPrinterParameterized(sEncounterSignWindowId, FONT_SMALL, COMPOUND_STRING("{RIGHT_ARROW}"), 212, 4, TEXT_SKIP_DRAW, NULL);
    }

    PrintCentered(FONT_SMALL, encounterPage->label, 112, PAGE_LABEL_Y, 176);
    for (entryId = 0; entryId < encounterPage->count; entryId++)
    {
        const struct EncounterSignEntry *entry = &encounterPage->entries[entryId];
        u8 column = entryId % GRID_COLUMNS;
        u8 row = entryId / GRID_COLUMNS;
        u16 center = GRID_FIRST_CENTER_X + column * GRID_COLUMN_WIDTH;

        sEncounterSignSpriteIds[entryId] = CreateMonIconNoPersonality(
            entry->species,
            SpriteCB_MonIcon,
            center + 8,
            GRID_FIRST_ICON_Y + row * GRID_ROW_HEIGHT,
            0);
        if (sEncounterSignSpriteIds[entryId] != MAX_SPRITES)
            gSprites[sEncounterSignSpriteIds[entryId]].oam.priority = 0;
        PrintCentered(FONT_SMALL_NARROWER,
                      gSpeciesInfo[entry->species].speciesName,
                      center,
                      GRID_FIRST_NAME_Y + row * GRID_ROW_HEIGHT,
                      GRID_COLUMN_WIDTH - 4);

        ConvertIntToDecimalStringN(text, entry->rate, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringAppend(text, COMPOUND_STRING("%"));
        PrintCentered(FONT_SMALL,
                      text,
                      center,
                      GRID_FIRST_RATE_Y + row * GRID_ROW_HEIGHT,
                      GRID_COLUMN_WIDTH - 4);
    }

    PutWindowTilemap(sEncounterSignWindowId);
    CopyWindowToVram(sEncounterSignWindowId, COPYWIN_FULL);
}

static void CloseEncounterSign(u8 taskId)
{
    DestroyEncounterSignIcons();
    FreeMonIconPalettes();
    ClearStdWindowAndFrameToTransparent(sEncounterSignWindowId, TRUE);
    RemoveWindow(sEncounterSignWindowId);
    sEncounterSignWindowId = WINDOW_NONE;
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_EncounterSignInput(u8 taskId)
{
    const struct EncounterSign *sign = &sEncounterSigns[gTasks[taskId].data[1]];
    u8 pageCount = sign->rowCount;

    if (JOY_NEW(A_BUTTON | B_BUTTON))
    {
        PlaySE(SE_SELECT);
        CloseEncounterSign(taskId);
    }
    else if (pageCount > 1 && JOY_NEW(DPAD_LEFT))
    {
        PlaySE(SE_SELECT);
        if (gTasks[taskId].data[0] == 0)
            gTasks[taskId].data[0] = pageCount - 1;
        else
            gTasks[taskId].data[0]--;
        DrawEncounterSignPage(taskId);
    }
    else if (pageCount > 1 && JOY_NEW(DPAD_RIGHT))
    {
        PlaySE(SE_SELECT);
        gTasks[taskId].data[0]++;
        if (gTasks[taskId].data[0] >= pageCount)
            gTasks[taskId].data[0] = 0;
        DrawEncounterSignPage(taskId);
    }
}

void ShowEncounterSign(void)
{
    u8 taskId;
    u32 i;
    u16 signId = gSpecialVar_0x8004;

    if (signId >= ENCOUNTER_SIGN_COUNT)
    {
        ScriptContext_Enable();
        return;
    }

    for (i = 0; i < ICON_COUNT; i++)
        sEncounterSignSpriteIds[i] = MAX_SPRITES;

    sEncounterSignWindowId = AddWindow(&sEncounterSignWindow);
    if (sEncounterSignWindowId == WINDOW_NONE)
    {
        ScriptContext_Enable();
        return;
    }

    LoadMonIconPalettes();
    LoadMessageBoxAndBorderGfx();
    SetStandardWindowBorderStyle(sEncounterSignWindowId, FALSE);
    taskId = CreateTask(Task_EncounterSignInput, 80);
    gTasks[taskId].data[0] = 0;
    gTasks[taskId].data[1] = signId;
    DrawEncounterSignPage(taskId);
}

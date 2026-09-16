#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "caps.h"
#include "pokemon.h"
#include "constants/global.h"
#include "constants/difficulty.h"


u32 GetCurrentProgressionLevelCap(void)
{
    static const u32 sLevelCapFlagMap[][2] =
    {
        {FLAG_BADGE01_GET, 15},
        {FLAG_BADGE02_GET, 19},
        {FLAG_BADGE03_GET, 24},
        {FLAG_BADGE04_GET, 29},
        {FLAG_BADGE05_GET, 31},
        {FLAG_BADGE06_GET, 33},
        {FLAG_BADGE07_GET, 42},
        {FLAG_BADGE08_GET, 46},
        {FLAG_IS_CHAMPION, 58},
    };

    u32 i;

    if (B_LEVEL_CAP_TYPE == LEVEL_CAP_FLAG_LIST)
    {
        for (i = 0; i < ARRAY_COUNT(sLevelCapFlagMap); i++)
        {
            if (!FlagGet(sLevelCapFlagMap[i][0]))
                return sLevelCapFlagMap[i][1];
        }
    }
    else if (B_LEVEL_CAP_TYPE == LEVEL_CAP_VARIABLE)
    {
        u32 levelCap = VarGet(B_LEVEL_CAP_VARIABLE);
        return levelCap == 0 ? MAX_LEVEL : levelCap;
    }

    return MAX_LEVEL;
}

u32 GetCurrentLevelCap(void)
{
    if (gSaveBlock2Ptr != NULL && gSaveBlock2Ptr->optionsDifficulty == DIFFICULTY_EASY)
        return MAX_LEVEL;

    return GetCurrentProgressionLevelCap();
}

u32 GetSoftLevelCapExpValue(u32 level, u32 expValue)
{
    static const u32 sExpScalingDown[5] = { 4, 8, 16, 32, 64 };
    static const u32 sExpScalingUp[5]   = { 16, 8, 4, 2, 1 };

    u32 levelDifference;
    u32 currentLevelCap;

    if (B_EXP_CAP_TYPE == EXP_CAP_NONE
     || gSaveBlock2Ptr->optionsDifficulty == DIFFICULTY_EASY)
        return expValue;

    currentLevelCap = GetCurrentLevelCap();

    if (level < currentLevelCap)
    {
        if (B_LEVEL_CAP_EXP_UP)
        {
            levelDifference = currentLevelCap - level;
            if (levelDifference > ARRAY_COUNT(sExpScalingUp) - 1)
                return expValue + (expValue / sExpScalingUp[ARRAY_COUNT(sExpScalingUp) - 1]);
            else
                return expValue + (expValue / sExpScalingUp[levelDifference]);
        }
        else
        {
            return expValue;
        }
    }
    else if (gSaveBlock2Ptr->optionsDifficulty != DIFFICULTY_EASY)
    {
        return 0;
    }
    else if (B_EXP_CAP_TYPE == EXP_CAP_SOFT)
    {
        levelDifference = level - currentLevelCap;
        if (levelDifference > ARRAY_COUNT(sExpScalingDown) - 1)
            return expValue / sExpScalingDown[ARRAY_COUNT(sExpScalingDown) - 1];
        else
            return expValue / sExpScalingDown[levelDifference];
    }
    else
    {
       return expValue;
    }
}

u32 GetCurrentEVCap(void)
{
    static const u16 sEvCapFlagMap[][2] = {
        // The pre-Gym allowance is 64 EVs. Each badge adds another 64,
        // with the last two stages clamped to the engine's 510-EV limit.
        {FLAG_BADGE01_GET,  64},
        {FLAG_BADGE02_GET, 128},
        {FLAG_BADGE03_GET, 192},
        {FLAG_BADGE04_GET, 256},
        {FLAG_BADGE05_GET, 320},
        {FLAG_BADGE06_GET, 384},
        {FLAG_BADGE07_GET, 448},
        {FLAG_BADGE08_GET, MAX_TOTAL_EVS},
    };

    if (B_EV_CAP_TYPE == EV_CAP_FLAG_LIST)
    {
        for (u32 evCap = 0; evCap < ARRAY_COUNT(sEvCapFlagMap); evCap++)
        {
            if (!FlagGet(sEvCapFlagMap[evCap][0]))
                return sEvCapFlagMap[evCap][1];
        }
    }
    else if (B_EV_CAP_TYPE == EV_CAP_VARIABLE)
    {
        return VarGet(B_EV_CAP_VARIABLE);
    }
    else if (B_EV_CAP_TYPE == EV_CAP_NO_GAIN)
    {
        return 0;
    }

    return MAX_TOTAL_EVS;
}

u32 GetCurrentPerStatEVCap(void)
{
    static const u16 sPerStatEvCapFlagMap[][2] = {
        {FLAG_BADGE01_GET,  32},
        {FLAG_BADGE02_GET,  64},
        {FLAG_BADGE03_GET,  96},
        {FLAG_BADGE04_GET, 128},
        {FLAG_BADGE05_GET, 160},
        {FLAG_BADGE06_GET, 192},
        {FLAG_BADGE07_GET, 224},
        {FLAG_BADGE08_GET, MAX_PER_STAT_EVS},
    };

    if (B_EV_CAP_TYPE == EV_CAP_NO_GAIN)
        return 0;
    if (B_EV_CAP_TYPE == EV_CAP_FLAG_LIST)
    {
        for (u32 i = 0; i < ARRAY_COUNT(sPerStatEvCapFlagMap); i++)
            if (!FlagGet(sPerStatEvCapFlagMap[i][0]))
                return sPerStatEvCapFlagMap[i][1];
    }

    return MAX_PER_STAT_EVS;
}

bool32 IsAcceleratedTrainingActive(void)
{
    // The badge check is a failsafe for old/debug saves that may set the badge
    // without running Roxanne's normal post-battle script.
    return FlagGet(FLAG_EXPEDITED_TRAINING) && !FlagGet(FLAG_BADGE01_GET);
}

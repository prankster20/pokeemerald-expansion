#include "global.h"
#include "battle.h"
#include "event_data.h"
#include "move.h"
#include "move_fusion.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/flags.h"

#include "data/move_fusion.h"

u32 GetMoveFusionRoles(enum Move move)
{
    if (move <= MOVE_NONE || move >= MOVES_COUNT)
        return MOVE_FUSION_ROLE_DISABLED;
    if (gMoveFusionInfo[move].roles == MOVE_FUSION_ROLE_DEFAULT)
        return MOVE_FUSION_ROLE_MAIN;
    return gMoveFusionInfo[move].roles;
}

bool32 CanMoveBeFusionMain(enum Move move)
{
    return GetMoveFusionRoles(move) & MOVE_FUSION_ROLE_MAIN;
}

bool32 CanMoveBeFusionHelper(enum Move move)
{
    return GetMoveFusionRoles(move) & MOVE_FUSION_ROLE_HELPER;
}

bool32 IsMoveFusionUnlocked(void)
{
    // Reuse story progression instead of allocating move-fusion save flags.
    return FlagGet(FLAG_DEFEATED_RIVAL_ROUTE103);
}

bool32 BoxMonHasMoveOrHelper(struct BoxPokemon *mon, enum Move move)
{
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
    {
        if (GetBoxMonData(mon, MON_DATA_MOVE1 + slot) == move
         || GetBoxMonMoveHelper(mon, slot) == move)
            return TRUE;
    }
    return FALSE;
}

static bool32 MeetsHelperRequirements(enum Move mainMove, u32 requirements)
{
    enum DamageCategory category = GetMoveCategory(mainMove);

    if ((requirements & MOVE_FUSION_REQUIRES_DAMAGING) && category == DAMAGE_CATEGORY_STATUS)
        return FALSE;
    if ((requirements & MOVE_FUSION_REQUIRES_STATUS) && category != DAMAGE_CATEGORY_STATUS)
        return FALSE;
    if ((requirements & MOVE_FUSION_REQUIRES_SINGLE_STRIKE) && GetMoveStrikeCount(mainMove) > 1)
        return FALSE;
    if ((requirements & MOVE_FUSION_REQUIRES_SINGLE_TARGET) && GetMoveTarget(mainMove) != TARGET_SELECTED)
        return FALSE;
    return TRUE;
}

bool32 AreMovesFusionCompatible(enum Move mainMove, enum Move helperMove)
{
    if (mainMove == helperMove)
        return FALSE;
    if (!CanMoveBeFusionMain(mainMove) || !CanMoveBeFusionHelper(helperMove))
        return FALSE;
    return MeetsHelperRequirements(mainMove, gMoveFusionInfo[helperMove].helper.requirements);
}

bool32 ResolveMoveFusion(enum Move mainMove, enum Move helperMove, struct ResolvedMoveFusion *resolved)
{
    const struct MoveFusionHelperInfo *helper;
    s32 power;
    s32 pp;

    if (resolved == NULL || !AreMovesFusionCompatible(mainMove, helperMove))
        return FALSE;

    helper = &gMoveFusionInfo[helperMove].helper;
    power = GetMovePower(mainMove);
    power = power * (helper->powerPercent == 0 ? 100 : helper->powerPercent) / 100;
    power += helper->powerDelta;
    pp = GetMovePP(mainMove) + helper->ppDelta;

    if (GetMoveCategory(mainMove) == DAMAGE_CATEGORY_STATUS)
        power = 0;
    else
        power = min(511, max(1, power));

    pp = min(255, max(1, pp));
    if (helper->ppCap != 0)
        pp = min(pp, helper->ppCap);

    *resolved = (struct ResolvedMoveFusion) {
        .mainMove = mainMove,
        .helperMove = helperMove,
        .power = power,
        .pp = pp,
        .bitingMove = helper->bitingMove,
        .pulseMove = helper->pulseMove,
        .makesContact = helper->makesContact,
        .infiltrates = helper->infiltrates,
        .overridesType = helper->overridesType || helper->useHelperType,
        .type = helper->useHelperType ? GetMoveType(helperMove) : helper->type,
        .criticalHitStage = helper->criticalHitStage,
        .strikeCount = helper->strikeCount,
        .superEffectiveType = helper->superEffectiveType,
        .effect = helper->effect,
        .argument = helper->argument,
        .additionalEffects = helper->additionalEffects,
        .numAdditionalEffects = helper->numAdditionalEffects,
    };
    return TRUE;
}

static bool32 IsLevelUpHelperAvailable(struct BoxPokemon *mon, enum Move helperMove)
{
    enum Species species = GetBoxMonData(mon, MON_DATA_SPECIES);
    u32 level = GetLevelFromBoxMonExp(mon);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);

    for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
    {
        if (learnset[i].level > level)
            break;
        if (learnset[i].move == helperMove)
            return TRUE;
    }
    return FALSE;
}

u32 GetAvailableFusionHelpers(struct BoxPokemon *mon, enum Move mainMove, enum Move *helpers, u32 capacity)
{
    u32 count = 0;

    for (enum Move move = MOVE_POUND; move < MOVES_COUNT; move++)
    {
        if (!AreMovesFusionCompatible(mainMove, move))
            continue;
        if (BoxMonHasMoveOrHelper(mon, move))
            continue;
        if (!IsLevelUpHelperAvailable(mon, move)
         && !CanLearnTeachableMove(GetBoxMonData(mon, MON_DATA_SPECIES), move))
            continue;
        if (count < capacity)
            helpers[count] = move;
        count++;
    }
    return min(count, capacity);
}

u8 *GetFusedMoveName(enum Move mainMove, enum Move helperMove, u8 *dst)
{
    const u8 *mainHalf = gMoveFusionInfo[mainMove].mainHalfName;
    const u8 *helperHalf = gMoveFusionInfo[helperMove].helperHalfName;

    if (mainHalf != NULL && helperHalf != NULL)
    {
        StringCopy(dst, mainHalf);
        StringAppend(dst, COMPOUND_STRING(" "));
        StringAppend(dst, helperHalf);
    }
    else
    {
        StringCopy(dst, GetMoveName(mainMove));
        StringAppend(dst, COMPOUND_STRING(" + "));
        StringAppend(dst, GetMoveName(helperMove));
    }
    return dst;
}

u8 *GetFusedMoveDescription(enum Move mainMove, const struct ResolvedMoveFusion *resolved, u8 *dst)
{
    const u8 *helperDescription = gMoveFusionInfo[resolved->helperMove].helper.description;

    StringCopy(dst, GetMoveDescription(mainMove));
    if (helperDescription != NULL)
    {
        StringAppend(dst, COMPOUND_STRING("\n"));
        StringAppend(dst, helperDescription);
    }
    return dst;
}

const u8 *GetMoveFusionEffectDescription(const struct ResolvedMoveFusion *resolved)
{
    if (resolved->numAdditionalEffects != 0)
    {
        switch (resolved->additionalEffects[0].moveEffect)
        {
        case MOVE_EFFECT_FLINCH:     return COMPOUND_STRING("May make the target flinch.");
        case MOVE_EFFECT_STAT_MINUS: return COMPOUND_STRING("May lower the target's stats.");
        default:                     return COMPOUND_STRING("Adds an additional move effect.");
        }
    }
    if (resolved->effect == EFFECT_RECOIL) return COMPOUND_STRING("The user takes recoil damage.");
    if (resolved->effect == EFFECT_ASSURANCE) return COMPOUND_STRING("Power doubles if the target was hurt.");
    if (resolved->effect == EFFECT_ABSORB) return COMPOUND_STRING("Restores HP based on damage dealt.");
    if (resolved->strikeCount != 0) return COMPOUND_STRING("Strikes the target multiple times.");
    if (resolved->effect == EFFECT_HIT_ESCAPE) return COMPOUND_STRING("The user switches out after attacking.");
    return COMPOUND_STRING("Adds a helper effect.");
}

bool32 ResolveBattlerMoveFusion(enum BattlerId battler, enum Move move, struct ResolvedMoveFusion *resolved)
{
    u32 slot = gBattleStruct->chosenMovePositions[battler];
    struct Pokemon *mon;
    enum Move helper;

    if (slot >= MAX_MON_MOVES || gBattleMons[battler].moves[slot] != move)
        return FALSE;
    mon = GetBattlerMon(battler);
    helper = GetMonMoveHelper(mon, slot);
    return ResolveMoveFusion(move, helper, resolved);
}

bool32 BattlerMoveInfiltrates(enum BattlerId battler, enum Move move)
{
    struct ResolvedMoveFusion fusion;

    if (MoveInfiltrates(move))
        return TRUE;
    return ResolveBattlerMoveFusion(battler, move, &fusion) && fusion.infiltrates;
}

u32 GetBattlerMoveCriticalHitStage(enum BattlerId battler, enum Move move)
{
    struct ResolvedMoveFusion fusion;
    u32 stage = GetMoveCriticalHitStage(move);

    if (ResolveBattlerMoveFusion(battler, move, &fusion))
        stage += fusion.criticalHitStage;
    return stage;
}

static bool32 AdditionalEffectsMatch(const struct AdditionalEffect *left, const struct AdditionalEffect *right)
{
    return left->moveEffect == right->moveEffect
        && left->self == right->self
        && left->onlyIfTargetRaisedStats == right->onlyIfTargetRaisedStats
        && left->onChargeTurnOnly == right->onChargeTurnOnly
        && left->preAttackEffect == right->preAttackEffect
        && left->onSide == right->onSide
        && left->stats == right->stats
        && left->attack == right->attack
        && left->defense == right->defense
        && left->spAtk == right->spAtk
        && left->spDef == right->spDef
        && left->speed == right->speed
        && left->accuracy == right->accuracy
        && left->evasion == right->evasion;
}

static bool32 HelperEffectMatchesMain(enum Move move, const struct AdditionalEffect *helperEffect)
{
    for (u32 i = 0; i < GetMoveAdditionalEffectCount(move); i++)
    {
        if (AdditionalEffectsMatch(GetMoveAdditionalEffectById(move, i), helperEffect))
            return TRUE;
    }
    return FALSE;
}

bool32 GetMoveFusionCombinedEffectChance(enum Move mainMove, const struct AdditionalEffect *helperEffect, u32 *chance)
{
    for (u32 i = 0; i < GetMoveAdditionalEffectCount(mainMove); i++)
    {
        const struct AdditionalEffect *mainEffect = GetMoveAdditionalEffectById(mainMove, i);

        if (!AdditionalEffectsMatch(mainEffect, helperEffect))
            continue;
        if (mainEffect->chance == 0 || helperEffect->chance == 0)
            *chance = 100;
        else
            *chance = min(100, mainEffect->chance + helperEffect->chance);
        return TRUE;
    }
    return FALSE;
}

u32 GetBattlerMoveAdditionalEffectCount(enum BattlerId battler, enum Move move)
{
    struct ResolvedMoveFusion fusion;
    u32 count = GetMoveAdditionalEffectCount(move);

    if (!ResolveBattlerMoveFusion(battler, move, &fusion))
        return count;
    for (u32 i = 0; i < fusion.numAdditionalEffects; i++)
    {
        if (!HelperEffectMatchesMain(move, &fusion.additionalEffects[i]))
            count++;
    }
    return count;
}

const struct AdditionalEffect *GetBattlerMoveAdditionalEffectById(enum BattlerId battler, enum Move move, u32 effect, struct AdditionalEffect *buffer)
{
    struct ResolvedMoveFusion fusion;
    u32 mainCount = GetMoveAdditionalEffectCount(move);

    if (effect < mainCount)
    {
        *buffer = *GetMoveAdditionalEffectById(move, effect);
        if (ResolveBattlerMoveFusion(battler, move, &fusion))
        {
            for (u32 i = 0; i < fusion.numAdditionalEffects; i++)
            {
                const struct AdditionalEffect *helperEffect = &fusion.additionalEffects[i];
                if (!AdditionalEffectsMatch(buffer, helperEffect))
                    continue;
                if (buffer->chance == 0 || helperEffect->chance == 0)
                    buffer->chance = 0;
                else
                    buffer->chance = min(100, buffer->chance + helperEffect->chance);
            }
        }
        return buffer;
    }

    if (!ResolveBattlerMoveFusion(battler, move, &fusion))
        return NULL;
    effect -= mainCount;
    for (u32 i = 0; i < fusion.numAdditionalEffects; i++)
    {
        if (HelperEffectMatchesMain(move, &fusion.additionalEffects[i]))
            continue;
        if (effect-- == 0)
        {
            *buffer = fusion.additionalEffects[i];
            return buffer;
        }
    }
    return NULL;
}

bool32 BattlerMoveIsAffectedBySheerForce(enum BattlerId battler, enum Move move)
{
    struct AdditionalEffect effectBuffer;
    u32 count = GetBattlerMoveAdditionalEffectCount(battler, move);

    for (u32 i = 0; i < count; i++)
    {
        const struct AdditionalEffect *effect = GetBattlerMoveAdditionalEffectById(battler, move, i, &effectBuffer);
        if ((effect->chance > 0) != effect->sheerForceOverride)
            return TRUE;
    }
    return FALSE;
}

bool32 IsBattlerSheerForceAffected(enum BattlerId battler, enum Move move, enum Ability ability)
{
    return ability == ABILITY_SHEER_FORCE && BattlerMoveIsAffectedBySheerForce(battler, move);
}

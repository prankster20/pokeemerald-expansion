#ifndef GUARD_MOVE_FUSION_H
#define GUARD_MOVE_FUSION_H

#include "global.h"
#include "constants/moves.h"
#include "move.h"

struct BoxPokemon;

enum MoveFusionRole
{
    // Zero-initialized/unlisted moves retain the convenient main-only default.
    MOVE_FUSION_ROLE_DEFAULT  = 0,
    MOVE_FUSION_ROLE_DISABLED = 1 << 0,
    MOVE_FUSION_ROLE_MAIN     = 1 << 1,
    MOVE_FUSION_ROLE_HELPER   = 1 << 2,
    MOVE_FUSION_ROLE_BOTH   = MOVE_FUSION_ROLE_MAIN | MOVE_FUSION_ROLE_HELPER,
};

enum MoveFusionRequirement
{
    MOVE_FUSION_REQUIRES_NONE          = 0,
    MOVE_FUSION_REQUIRES_DAMAGING      = 1 << 0,
    MOVE_FUSION_REQUIRES_STATUS        = 1 << 1,
    MOVE_FUSION_REQUIRES_SINGLE_TARGET = 1 << 2,
    MOVE_FUSION_REQUIRES_SINGLE_STRIKE = 1 << 3,
};

enum MoveFusionFlagOverride
{
    MOVE_FUSION_FLAG_INHERIT,
    MOVE_FUSION_FLAG_ADD,
    MOVE_FUSION_FLAG_REMOVE,
};

struct MoveFusionHelperInfo
{
    s16 powerDelta;
    u16 powerPercent; // Zero defaults to 100%.
    s8 ppDelta;
    u8 ppCap; // Zero means no cap.
    u8 requirements;
    // These are ROM metadata, never saved. They deliberately use the same
    // names as MoveInfo so helper authoring looks like moves_info.h.
    u8 bitingMove; // enum MoveFusionFlagOverride
    u8 pulseMove;  // enum MoveFusionFlagOverride
    u8 makesContact; // enum MoveFusionFlagOverride
    bool8 infiltrates;
    bool8 overridesType;
    bool8 useHelperType;
    enum Type type;
    u8 criticalHitStage; // Added to, rather than replacing, the main's stage.
    u8 strikeCount; // Zero leaves the main move unchanged.
    u8 superEffectiveType;
    enum BattleMoveEffects effect;
    union MoveEffectArgument argument;
    const struct AdditionalEffect *additionalEffects;
    u8 numAdditionalEffects;
    const u8 *description;
};

struct MoveFusionInfo
{
    u8 roles;
    const u8 *mainHalfName;
    const u8 *helperHalfName;
    struct MoveFusionHelperInfo helper;
};

struct ResolvedMoveFusion
{
    enum Move mainMove;
    enum Move helperMove;
    u16 power;
    u8 pp;
    u8 bitingMove; // enum MoveFusionFlagOverride
    u8 pulseMove;  // enum MoveFusionFlagOverride
    u8 makesContact; // enum MoveFusionFlagOverride
    bool8 infiltrates;
    bool8 overridesType;
    enum Type type;
    u8 criticalHitStage;
    u8 strikeCount;
    u8 superEffectiveType;
    enum BattleMoveEffects effect;
    union MoveEffectArgument argument;
    const struct AdditionalEffect *additionalEffects;
    u8 numAdditionalEffects;
};

extern const struct MoveFusionInfo gMoveFusionInfo[MOVES_COUNT];

u32 GetMoveFusionRoles(enum Move move);
bool32 CanMoveBeFusionMain(enum Move move);
bool32 CanMoveBeFusionHelper(enum Move move);
bool32 AreMovesFusionCompatible(enum Move mainMove, enum Move helperMove);
bool32 IsMoveFusionUnlocked(void);
bool32 BoxMonHasMoveOrHelper(struct BoxPokemon *mon, enum Move move);
bool32 ResolveMoveFusion(enum Move mainMove, enum Move helperMove, struct ResolvedMoveFusion *resolved);
u32 GetAvailableFusionHelpers(struct BoxPokemon *mon, enum Move mainMove, enum Move *helpers, u32 capacity);
const u8 *GetMoveFusionEffectDescription(const struct ResolvedMoveFusion *resolved);
u8 *GetFusedMoveName(enum Move mainMove, enum Move helperMove, u8 *dst);
u8 *GetFusedMoveDescription(enum Move mainMove, const struct ResolvedMoveFusion *resolved, u8 *dst);
bool32 ResolveBattlerMoveFusion(enum BattlerId battler, enum Move move, struct ResolvedMoveFusion *resolved);
bool32 BattlerMoveInfiltrates(enum BattlerId battler, enum Move move);
u32 GetBattlerMoveCriticalHitStage(enum BattlerId battler, enum Move move);
u32 GetBattlerMoveAdditionalEffectCount(enum BattlerId battler, enum Move move);
const struct AdditionalEffect *GetBattlerMoveAdditionalEffectById(enum BattlerId battler, enum Move move, u32 effect, struct AdditionalEffect *buffer);
bool32 GetMoveFusionCombinedEffectChance(enum Move mainMove, const struct AdditionalEffect *helperEffect, u32 *chance);
bool32 BattlerMoveIsAffectedBySheerForce(enum BattlerId battler, enum Move move);
bool32 IsBattlerSheerForceAffected(enum BattlerId battler, enum Move move, enum Ability ability);

#endif // GUARD_MOVE_FUSION_H

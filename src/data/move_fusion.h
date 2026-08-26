// This is the main authoring file for move-fusion behavior.
// Unlisted ordinary moves default to main-only. Listed moves choose a role and
// may define a helper contribution. Use DISABLED to prohibit both roles.

const struct MoveFusionInfo gMoveFusionInfo[MOVES_COUNT] =
{
    // BUG
    [MOVE_U_TURN] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .mainHalfName = COMPOUND_STRING("Turning"),
        .helperHalfName = COMPOUND_STRING("Turn"),
        .helper = {
            .ppCap = 5,
            .effect = EFFECT_HIT_ESCAPE,
            .description = COMPOUND_STRING("Switches the user out after attacking. PP cannot exceed 5."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    // DARK
    [MOVE_ASSURANCE] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .powerDelta = -10,
            .effect = EFFECT_ASSURANCE,
            .description = COMPOUND_STRING("Loses 10 power, but doubles in power if the target was already hurt this turn."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_BITE] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .ppDelta = -5,
            .additionalEffects = ADDITIONAL_EFFECTS({
                .moveEffect = MOVE_EFFECT_FLINCH,
                .chance = 20,
            }),
            .bitingMove = TRUE,
            .description = COMPOUND_STRING("Has a 20% flinch chance and becomes a biting move, but loses 5 PP."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_POWER_TRIP] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .ppCap = 10,
            .effect = EFFECT_STORED_POWER,
            .argument = { .powerPerStatBoost = 5 },
            .description = COMPOUND_STRING("Gains 5 power for each raised stat stage. PP cannot exceed 10."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    // DRAGON
    [MOVE_FICKLE_BEAM] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .powerDelta = -20,
            .effect = EFFECT_FICKLE_BEAM,
            .argument = { .randomPowerMultiplier = { .chance = 30, .multiplier = 2 } },
            .description = COMPOUND_STRING("Has a 30% chance to attack at double power, but loses 20 power."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    // ELECTRIC
    
    // FIGHTING
    [MOVE_ROCK_SMASH] =
    {
        .roles = MOVE_FUSION_ROLE_HELPER,
        .helper = {
            .additionalEffects = ADDITIONAL_EFFECTS({
                .moveEffect = MOVE_EFFECT_STAT_MINUS,
                .defense = 1,
                .chance = 35,
            }),
            .description = COMPOUND_STRING("35% to drop the foe's Defense."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    // GRASS
    [MOVE_ABSORB] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .effect = EFFECT_ABSORB,
            .argument = { .absorbPercentage = 10 },
            .description = COMPOUND_STRING("Heals 10% of damage dealt."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_BRANCH_POKE] =
    {
        .roles = MOVE_FUSION_ROLE_HELPER,
        .helper = {
            .powerDelta = -10,
            .infiltrates = TRUE,
            .description = COMPOUND_STRING("-10 BP but infiltrates while attacking."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_MEGA_DRAIN] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .effect = EFFECT_ABSORB,
            .powerDelta = -20,
            .argument = { .absorbPercentage = 50 },
            .description = COMPOUND_STRING("Heals 50% of damage dealt, but -20 BP."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_GIGA_DRAIN] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .effect = EFFECT_ABSORB,
            .powerDelta = -10,
            .argument = { .absorbPercentage = 33 },
            .description = COMPOUND_STRING("Heals 33% of damage dealt, but -10 BP."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_LEAFAGE] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .makesContact = FALSE,
            .description = COMPOUND_STRING("Removes Contact."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_VINE_WHIP] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .powerPercent = 50,
            .ppDelta = -5,
            .strikeCount = 2,
            .description = COMPOUND_STRING("Hits twice at half power per hit, but loses 5 PP."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING | MOVE_FUSION_REQUIRES_SINGLE_TARGET | MOVE_FUSION_REQUIRES_SINGLE_STRIKE,
        },
    },
    // NORMAL
    [MOVE_DOUBLE_HIT] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .powerPercent = 50,
            .ppDelta = -5,
            .strikeCount = 2,
            .description = COMPOUND_STRING("Hits twice at half power per hit, but loses 5 PP."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING | MOVE_FUSION_REQUIRES_SINGLE_TARGET | MOVE_FUSION_REQUIRES_SINGLE_STRIKE,
        },
    },
    [MOVE_POUND] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .mainHalfName = COMPOUND_STRING("Pounding"),
        .helperHalfName = COMPOUND_STRING("Pound"),
        .helper = {
            .additionalEffects = ADDITIONAL_EFFECTS({
                .moveEffect = MOVE_EFFECT_FLINCH,
                .chance = 10,
            }),
            .description = COMPOUND_STRING("10% to make the target flinch."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_SCRATCH] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .mainHalfName = COMPOUND_STRING("Scratching"),
        .helperHalfName = COMPOUND_STRING("Scratch"),
        .helper = {
            .additionalEffects = ADDITIONAL_EFFECTS({
                .moveEffect = MOVE_EFFECT_STAT_MINUS,
                .defense = 1,
                .chance = 10,
            }),
            .description = COMPOUND_STRING("10% to drop the foe's Defense."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_TACKLE] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .mainHalfName = COMPOUND_STRING("Tackling"),
        .helperHalfName = COMPOUND_STRING("Tackle"),
        .helper = {
            .powerDelta = 5,
            .effect = EFFECT_RECOIL,
            .argument = { .recoilPercentage = 1 },
            .description = COMPOUND_STRING("Adds 5 power, but the user takes 1% recoil damage."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    // POISON
    [MOVE_ACID] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .helper = {
            .powerDelta = -20,
            .ppDelta = -5,
            .superEffectiveType = TYPE_STEEL,
            .description = COMPOUND_STRING("Super-effective on Steel types, but -20 BP and -5 PP."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    // WATER
    [MOVE_SNIPE_SHOT] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .mainHalfName = COMPOUND_STRING("Snipe"),
        .helperHalfName = COMPOUND_STRING("Snipe"),
        .helper = {
            .powerDelta = -20,
            .pulseMove = TRUE,
            .criticalHitStage = 1,
            .description = COMPOUND_STRING("+MegaLauncher and adds +1 crit ratio, but -20 BP."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
    [MOVE_WATER_GUN] =
    {
        .roles = MOVE_FUSION_ROLE_BOTH,
        .mainHalfName = COMPOUND_STRING("Watergun"),
        .helperHalfName = COMPOUND_STRING("Watergun"),
        .helper = {
            .powerDelta = -10,
            .pulseMove = TRUE,
            .description = COMPOUND_STRING("+MegaLauncher, but -10 BP."),
            .requirements = MOVE_FUSION_REQUIRES_DAMAGING,
        },
    },
};

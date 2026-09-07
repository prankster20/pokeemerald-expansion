#include "global.h"
#include "pokemon.h"
#include "item.h"
#include "shop.h"
#include "test/test.h"

// Frivolous: 10% chance to secretly buy an extra item when purchasing.
// This is a shop-transaction hook — the random trigger can't be tested
// deterministically, so we only test the item eligibility logic.
//
// Frugal: 5% discount at Marts. Refuses priceless items and items costing > 5000.

// ===== FRUGAL =====

TEST("pranks Frugal accepts prices from 1 through 5000 and refuses zero or higher prices")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRUGAL;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    for (enum Item item = ITEM_NONE; item < ITEMS_COUNT; item++)
    {
        u32 price = GetItemPrice(item);
        bool32 expectedRefusal = item != ITEM_NONE && (price == 0 || price > 5000);

        EXPECT_EQ(DoesBoxMonNatureRefuseHeldItem(&mon.box, item), expectedRefusal);
    }
}

TEST("pranks Non-Frugal nature can hold any item regardless of price")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_DOCILE;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    for (enum Item item = ITEM_NONE; item < ITEMS_COUNT; item++)
        EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, item));
}

TEST("pranks Frugal does not refuse ITEM_NONE")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRUGAL;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, ITEM_NONE));
}

TEST("pranks approved held item price tiers match the Second Nature economy")
{
    EXPECT_EQ(GetItemPrice(ITEM_WEAKNESS_POLICY), 1000);
    EXPECT_EQ(GetItemPrice(ITEM_BRIGHT_POWDER), 2000);
    EXPECT_EQ(GetItemPrice(ITEM_NORMAL_GEM), 4000);
    EXPECT_EQ(GetItemPrice(ITEM_WIDE_LENS), 5000);
    EXPECT_EQ(GetItemPrice(ITEM_EXPERT_BELT), 8000);
    EXPECT_EQ(GetItemPrice(ITEM_CHOICE_BAND), 10000);
    EXPECT_EQ(GetItemPrice(ITEM_EVIOLITE), 15000);
}

TEST("pranks Scrounger junk is inexpensive and uses weaker premium hold effects")
{
    EXPECT_EQ(GetItemPrice(ITEM_RUBBER_BAND), 500);
    EXPECT_EQ(GetItemHoldEffect(ITEM_RUBBER_BAND), HOLD_EFFECT_CHOICE_BAND);
    EXPECT_EQ(GetItemHoldEffectParam(ITEM_RUBBER_BAND), 20);

    EXPECT_EQ(GetItemHoldEffect(ITEM_READING_GLASSES), HOLD_EFFECT_CHOICE_SPECS);
    EXPECT_EQ(GetItemHoldEffectParam(ITEM_READING_GLASSES), 20);
    EXPECT_EQ(GetItemHoldEffect(ITEM_ROLLER_SKATES), HOLD_EFFECT_CHOICE_SCARF);
    EXPECT_EQ(GetItemHoldEffectParam(ITEM_ROLLER_SKATES), 20);
    EXPECT_EQ(GetItemHoldEffect(ITEM_CHEWED_ROOT), HOLD_EFFECT_LEFTOVERS);
    EXPECT_EQ(GetItemHoldEffectParam(ITEM_CHEWED_ROOT), 32);
    EXPECT_EQ(GetItemHoldEffect(ITEM_GLASS_MARBLE), HOLD_EFFECT_LIFE_ORB);
    EXPECT_EQ(GetItemHoldEffectParam(ITEM_GLASS_MARBLE), 10);
    EXPECT_EQ(GetItemHoldEffect(ITEM_DENTED_HELMET), HOLD_EFFECT_ROCKY_HELMET);
    EXPECT_EQ(GetItemHoldEffectParam(ITEM_DENTED_HELMET), 12);
    EXPECT_EQ(GetItemHoldEffect(ITEM_TRAINING_PEBBLE), HOLD_EFFECT_EVIOLITE);
    EXPECT_EQ(GetItemHoldEffectParam(ITEM_TRAINING_PEBBLE), 25);
    EXPECT_EQ(GetItemHoldEffect(ITEM_ODD_BELT), HOLD_EFFECT_EXPERT_BELT);
    EXPECT_EQ(GetItemHoldEffectParam(ITEM_ODD_BELT), 10);
}

TEST("pranks Frugal accepts all Scrounger junk")
{
    static const enum Item sJunkItems[] =
    {
        ITEM_RUBBER_BAND,
        ITEM_READING_GLASSES,
        ITEM_ROLLER_SKATES,
        ITEM_CHEWED_ROOT,
        ITEM_GLASS_MARBLE,
        ITEM_DENTED_HELMET,
        ITEM_TRAINING_PEBBLE,
        ITEM_ODD_BELT,
    };
    struct Pokemon mon;
    u32 nature = NATURE_FRUGAL;

    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    for (u32 i = 0; i < ARRAY_COUNT(sJunkItems); i++)
        EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, sJunkItems[i]));
}

TEST("pranks Frugal gets a 5% discount at Marts")
{
    EXPECT_EQ(TestGetFrugalDiscountPrice(ITEM_POKE_BALL), (GetItemPrice(ITEM_POKE_BALL) * 19) / 20);
    EXPECT_EQ(TestGetFrugalDiscountPrice(ITEM_POTION), (GetItemPrice(ITEM_POTION) * 19) / 20);
    EXPECT_EQ(TestGetFrugalDiscountPrice(ITEM_MAX_REPEL), (GetItemPrice(ITEM_MAX_REPEL) * 19) / 20);
}

// ===== FRIVOLOUS =====
// The 10% extra-purchase trigger is random and happens at shop checkout.
// We can't test that in a unit test. What we CAN verify is that Frivolous
// doesn't accidentally block or modify the move-learning or item-refusal
// checks (i.e., it's purely a shop-trigger effect).

TEST("pranks Frivolous does not refuse any held item")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRIVOLOUS;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    // Frivolous only affects shopping, not held items.
    EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, ITEM_CHOICE_BAND));
    EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, ITEM_ORAN_BERRY));
}

TEST("pranks Frivolous does not refuse any learnable move")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRIVOLOUS;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    // Frivolous has no move restrictions.
    EXPECT(!DoesBoxMonNatureRefuseMove(&mon.box, MOVE_TACKLE));
    EXPECT(!DoesBoxMonNatureRefuseMove(&mon.box, MOVE_RECOVER));
}

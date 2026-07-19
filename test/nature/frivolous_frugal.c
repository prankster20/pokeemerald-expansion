#include "global.h"
#include "pokemon.h"
#include "item.h"
#include "test/test.h"

// Frivolous: 10% chance to secretly buy an extra item when purchasing.
// This is a shop-transaction hook — the random trigger can't be tested
// deterministically, so we only test the item eligibility logic.
//
// Frugal: 5% discount at Marts. Refuses held items costing > 5000.

// ===== FRUGAL =====

TEST("pranks Frugal refuses exactly the held items whose price is above 5000")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRUGAL;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    for (enum Item item = ITEM_NONE; item < ITEMS_COUNT; item++)
        EXPECT_EQ(DoesBoxMonNatureRefuseHeldItem(&mon.box, item), item != ITEM_NONE && GetItemPrice(item) > 5000);
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

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

TEST("Frugal refuses to hold items costing above 5000")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRUGAL;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    // Choice Band costs > 5000 — should be refused.
    EXPECT(DoesBoxMonNatureRefuseHeldItem(&mon.box, ITEM_CHOICE_BAND));
}

TEST("Frugal can hold items costing 5000 or less")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRUGAL;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    // Oran Berry costs less than 5000 — should be allowed.
    EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, ITEM_ORAN_BERRY));
}

TEST("Non-Frugal nature can hold any item regardless of price")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_DOCILE;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, ITEM_CHOICE_BAND));
}

TEST("Frugal does not refuse ITEM_NONE")
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

TEST("Frivolous does not refuse any held item")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRIVOLOUS;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    // Frivolous only affects shopping, not held items.
    EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, ITEM_CHOICE_BAND));
    EXPECT(!DoesBoxMonNatureRefuseHeldItem(&mon.box, ITEM_ORAN_BERRY));
}

TEST("Frivolous does not refuse any learnable move")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_WOBBUFFET, 50, 0, OTID_STRUCT_PLAYER_ID);
    u32 nature = NATURE_FRIVOLOUS;
    SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &nature);

    // Frivolous has no move restrictions.
    EXPECT(!DoesBoxMonNatureRefuseMove(&mon.box, MOVE_TACKLE));
    EXPECT(!DoesBoxMonNatureRefuseMove(&mon.box, MOVE_RECOVER));
}

/* Galerians cheats: trusted mod plugins for mods/packages/galerians.cheats.
 *
 * Codes for Galerians (USA). All three discs (SLUS-00986/01098/01099) boot the
 * same executable, so one set of addresses covers the whole game. Sources: the
 * DuckStation cheat database entry for SLUS-00986 and the CodeBreaker and
 * Action Replay pages on Almar's Guides. Memory cheats are re-applied on every
 * guest VBlank the way a cheat cartridge does. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mod_plugins.h"

#define GA_PKG "galerians.cheats"

/* Game state */
#define GA_HP_ADDR          0x801C2F9Cu
#define GA_AP_ADDR          0x801AF96Cu
#define GA_AP_AUX_ADDR      0x801AF96Eu
#define GA_NALCON_ADDR      0x801AF978u
#define GA_RED_ADDR         0x801AF97Cu
#define GA_DFELON_ADDR      0x801AF980u
#define GA_PSY_LEVEL_ADDR   0x801AF994u
#define GA_KILLS_ADDR       0x801AF99Cu
#define GA_DFELON_FLAG_ADDR 0x801AF9AEu
#define GA_ITEM_IDX_ADDR    0x801AFA0Cu   /* + 4 * slot, value 0x20 * (slot + 1) */
#define GA_ITEM_ID_ADDR     0x801AFAACu   /* + 2 * slot */
#define GA_ITEM_SLOTS_ADDR  0x801AFAFEu
#define GA_MEDICINE_ADDR    0x801AFB00u   /* + 2 * slot */
#define GA_TIMER_ADDR       0x801AFDECu
#define GA_HUD_ADDR         0x801AFDF2u
#define GA_ROOM_A_ADDR      0x801912DAu
#define GA_ROOM_B_ADDR      0x801912DCu
#define GA_ROOM_C_ADDR      0x801912DEu
#define GA_FACING_ADDR      0x801C2F8Cu
#define GA_ACTION_ADDR      0x801C2FA4u
#define GA_CHARGE_ADDR      0x801C2FA6u
#define GA_KNOCKDOWN_ADDR   0x801C2FD4u
#define GA_MOVIES_ADDR      0x801FD004u

/* The game's own pad words, in PlayStation button-bit order. GA_PAD_HELD is
 * what the codes use as their "joker"; GA_PAD_HIGH/GA_PAD_NEW are the bytes the
 * quick-turn code compares (Down held, Square pressed). */
#define GA_PAD_HELD         0x801C2F94u
#define GA_PAD_HIGH         0x801AC8D9u
#define GA_PAD_NEW          0x801AC8DCu
#define GA_BTN_L2_L1        0x0005u
#define GA_BTN_R1           0x0008u

static int ga_opt_int(const char* feature, const char* option, int fallback) {
    char buf[32];
    if (!psx_mod_option_value(GA_PKG, feature, option, buf, sizeof(buf)))
        return fallback;
    return (int)strtol(buf, NULL, 0);
}

static int ga_opt_hex(const char* feature, const char* option, int fallback) {
    char buf[32];
    if (!psx_mod_option_value(GA_PKG, feature, option, buf, sizeof(buf)))
        return fallback;
    return (int)strtol(buf, NULL, 16);
}

/* Set GA_CHEAT_LOG=<file path> to append a line whenever a code patch is
 * applied. Off by default. */
static void ga_log(const char* what, uint32_t addr, uint32_t from, uint32_t to) {
    const char* path = getenv("GA_CHEAT_LOG");
    FILE* f;
    if (!path) return;
    f = fopen(path, "a");
    if (!f) return;
    fprintf(f, "[galerians-cheat] %s @%08X %08X -> %08X\n", what,
            (unsigned)addr, (unsigned)from, (unsigned)to);
    fclose(f);
}

/* ---- Memory cheats ---------------------------------------------------- */

#define GA_HALF_CHEAT(fn, addr, value)                                       \
    static void fn(void) {                                                   \
        if (!psx_mod_game_started()) return;                                 \
        psx_mod_write_half((addr), (value));                                 \
    }

GA_HALF_CHEAT(ga_infinite_hp, GA_HP_ADDR, 0x00C8)
GA_HALF_CHEAT(ga_empty_ap, GA_AP_ADDR, 0x0000)
GA_HALF_CHEAT(ga_infinite_nalcon, GA_NALCON_ADDR, 0x00C8)
GA_HALF_CHEAT(ga_infinite_red, GA_RED_ADDR, 0x00C8)
GA_HALF_CHEAT(ga_infinite_dfelon, GA_DFELON_ADDR, 0x00C8)
GA_HALF_CHEAT(ga_have_dfelon, GA_DFELON_FLAG_ADDR, 0xFFFF)
GA_HALF_CHEAT(ga_timer_zero, GA_TIMER_ADDR, 0x0000)
GA_HALF_CHEAT(ga_max_item_slots, GA_ITEM_SLOTS_ADDR, 0x0028)

static void ga_psychic_max(void) {
    if (!psx_mod_game_started()) return;
    psx_mod_write_byte(GA_PSY_LEVEL_ADDR, 0x03);
}

static void ga_max_charge(void) {
    if (!psx_mod_game_started()) return;
    psx_mod_write_byte(GA_CHARGE_ADDR, 0xC8);
}

static void ga_all_movies(void) {
    if (!psx_mod_game_started()) return;
    psx_mod_write_word(GA_MOVIES_ADDR, 0xFFFFFFFFu);
}

static void ga_no_knockdown(void) {
    uint16_t state;
    if (!psx_mod_game_started()) return;
    state = psx_mod_read_half(GA_KNOCKDOWN_ADDR);
    if (state == 0x000E || state == 0x000C)
        psx_mod_write_half(GA_KNOCKDOWN_ADDR, 0x0000);
}

static void ga_quick_turn(void) {
    if (!psx_mod_game_started()) return;
    if (psx_mod_read_byte(GA_PAD_HIGH) == 0x40 &&
        psx_mod_read_byte(GA_PAD_NEW) == 0x80)
        psx_mod_write_word(GA_FACING_ADDR,
                           psx_mod_read_word(GA_FACING_ADDR) + 0x3E8u);
}

static void ga_shorting_on_demand(void) {
    if (!psx_mod_game_started()) return;
    if (psx_mod_read_half(GA_PAD_HELD) != GA_BTN_L2_L1) return;
    psx_mod_write_half(GA_AP_ADDR, 0x0064);
    psx_mod_write_half(GA_AP_AUX_ADDR, 0x0164);
}

static void ga_hud(void) {
    char mode[16];
    uint16_t value = 0x0100;
    if (!psx_mod_game_started()) return;
    if (psx_mod_option_value(GA_PKG, "hud", "mode", mode, sizeof(mode)) &&
        strcmp(mode, "no-meters") == 0)
        value = 0x0800;
    psx_mod_write_half(GA_HUD_ADDR, value);
}

static void ga_instant_charge(void) {
    static const uint16_t kCharge[] = { 0x00C8, 0x0190, 0x0258 };
    int level;
    if (!psx_mod_game_started()) return;
    if (psx_mod_read_half(GA_PAD_HELD) != GA_BTN_R1) return;
    level = ga_opt_int("instant-charge", "level", 3);
    if (level < 1 || level > 3) level = 3;
    psx_mod_write_half(GA_CHARGE_ADDR, kCharge[level - 1]);
}

static void ga_kills(void) {
    if (!psx_mod_game_started()) return;
    psx_mod_write_half(GA_KILLS_ADDR,
                       (uint16_t)ga_opt_int("kills", "count", 0));
}

static void ga_item_slot(void) {
    int slot, item;
    if (!psx_mod_game_started()) return;
    slot = ga_opt_int("item-slot", "slot", 1);
    item = ga_opt_hex("item-slot", "item", 0x02);
    if (slot < 1 || slot > 40 || item < 0 || item > 0x27) return;
    psx_mod_write_half(GA_ITEM_IDX_ADDR + 4u * (uint32_t)(slot - 1),
                       (uint16_t)(0x20 * slot));
    psx_mod_write_half(GA_ITEM_ID_ADDR + 2u * (uint32_t)(slot - 1),
                       (uint16_t)item);
}

static void ga_medicine_slot(void) {
    int slot, medicine;
    if (!psx_mod_game_started()) return;
    slot = ga_opt_int("medicine-slot", "slot", 1);
    medicine = ga_opt_hex("medicine-slot", "medicine", 0x03);
    if (slot < 1 || slot > 12 || medicine < 0 || medicine > 6) return;
    psx_mod_write_half(GA_MEDICINE_ADDR + 2u * (uint32_t)(slot - 1),
                       (uint16_t)medicine);
}

static void ga_action(void) {
    if (!psx_mod_game_started()) return;
    psx_mod_write_half(GA_ACTION_ADDR,
                       (uint16_t)ga_opt_int("action", "value", 0));
}

static void ga_room(void) {
    if (!psx_mod_game_started()) return;
    psx_mod_write_half(GA_ROOM_A_ADDR, (uint16_t)ga_opt_int("room", "a", 0));
    psx_mod_write_half(GA_ROOM_B_ADDR, (uint16_t)ga_opt_int("room", "b", 0));
    psx_mod_write_half(GA_ROOM_C_ADDR, (uint16_t)ga_opt_int("room", "c", 0));
}

/* ---- Code patches ----------------------------------------------------- */

/* Each replaces one instruction, checked against the boot executable. They go
 * through psx_mod_write_code_word so the recompiled function picks the change
 * up, and are only applied while the original instruction is still there. */
static void ga_patch_code(const char* what, uint32_t pc, uint32_t orig,
                          uint32_t patch) {
    if (!psx_mod_game_started()) return;
    if (psx_mod_read_word(pc) != orig) return;
    psx_mod_write_code_word(pc, patch);
    ga_log(what, pc, orig, patch);
}

static void ga_walk_through_walls(void) {
    ga_patch_code("walk-through-walls", 0x801212A4u, 0x16220011u, 0x10000011u);
}

static void ga_invulnerable(void) {
    ga_patch_code("invulnerable", 0x801486F0u, 0x10600005u, 0x10000005u);
    ga_patch_code("invulnerable", 0x801489C4u, 0x14400003u, 0x10000003u);
}

static void ga_new_game_psychic_3(void) {
    ga_patch_code("new-game-psychic-3", 0x801267A0u, 0x24020001u, 0x24020003u);
}

static void ga_shorting_no_drawbacks(void) {
    ga_patch_code("shorting-no-drawbacks", 0x80121D38u, 0x1452005Eu, 0x2400005Eu);
    ga_patch_code("shorting-no-drawbacks", 0x80121EA8u, 0x24020002u, 0x24020000u);
    ga_patch_code("shorting-no-drawbacks", 0x80148D1Cu, 0x104000D8u, 0x240000D8u);
    ga_patch_code("shorting-no-drawbacks", 0x80149438u, 0x00A32821u, 0x24050000u);
}

static void ga_infinite_ppec_alt(void) {
    ga_patch_code("infinite-ppec-alt", 0x801462FCu, 0xAC830628u, 0x24000628u);
}

static void ga_no_ap_alt(void) {
    ga_patch_code("no-ap-alt", 0x8014943Cu, 0xA4E50664u, 0x24000664u);
}

/* This one lives in code the game loads from disc after boot (0x801F86B8 is
 * past the end of the boot executable), so match on the branch's upper half
 * and re-apply whenever that code is loaded again. */
static void ga_doctors_scared(void) {
    uint32_t word;
    if (!psx_mod_game_started()) return;
    word = psx_mod_read_word(0x801F86B8u);
    if ((word >> 16) != 0x1622u) return;
    psx_mod_write_code_word(0x801F86B8u, (word & 0xFFFFu) | 0x24000000u);
    ga_log("doctors-scared", 0x801F86B8u, word, (word & 0xFFFFu) | 0x24000000u);
}

PSX_MOD_CONSTRUCTOR(galerians_register_cheats) {
    static const struct { const char* id; PSXModVBlankCallback fn; } kCheats[] = {
        { "galerians.cheat.infinite-hp", ga_infinite_hp },
        { "galerians.cheat.empty-ap", ga_empty_ap },
        { "galerians.cheat.infinite-nalcon", ga_infinite_nalcon },
        { "galerians.cheat.infinite-red", ga_infinite_red },
        { "galerians.cheat.infinite-dfelon", ga_infinite_dfelon },
        { "galerians.cheat.psychic-max", ga_psychic_max },
        { "galerians.cheat.have-dfelon", ga_have_dfelon },
        { "galerians.cheat.timer-zero", ga_timer_zero },
        { "galerians.cheat.max-item-slots", ga_max_item_slots },
        { "galerians.cheat.max-charge", ga_max_charge },
        { "galerians.cheat.all-movies", ga_all_movies },
        { "galerians.cheat.no-knockdown", ga_no_knockdown },
        { "galerians.cheat.quick-turn", ga_quick_turn },
        { "galerians.cheat.shorting-on-demand", ga_shorting_on_demand },
        { "galerians.cheat.hud", ga_hud },
        { "galerians.cheat.instant-charge", ga_instant_charge },
        { "galerians.cheat.kills", ga_kills },
        { "galerians.cheat.item-slot", ga_item_slot },
        { "galerians.cheat.medicine-slot", ga_medicine_slot },
        { "galerians.cheat.action", ga_action },
        { "galerians.cheat.room", ga_room },
        { "galerians.cheat.walk-through-walls", ga_walk_through_walls },
        { "galerians.cheat.invulnerable", ga_invulnerable },
        { "galerians.cheat.doctors-scared", ga_doctors_scared },
        { "galerians.cheat.new-game-psychic-3", ga_new_game_psychic_3 },
        { "galerians.cheat.shorting-no-drawbacks", ga_shorting_no_drawbacks },
        { "galerians.cheat.infinite-ppec-alt", ga_infinite_ppec_alt },
        { "galerians.cheat.no-ap-alt", ga_no_ap_alt },
    };
    for (unsigned i = 0; i < sizeof(kCheats) / sizeof(kCheats[0]); i++)
        psx_mod_register_vblank_plugin(kCheats[i].id, kCheats[i].fn);
}

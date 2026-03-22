// Upgrade entries: { id, name, desc, symbol, price, owned, sold, col, level, maxLevel, category }
{"rapid",     "RAPID FIRE",    "Increases fire rate",           "⚡", 120, false,false, {255,220, 60,255}, 1,3, "weapon"},
{"trishot",   "TRI-SHOT",      "Fire 3 bullets at once",        "◈", 200, false,false, { 80,200,255,255}, 1,1, "weapon"},
{"scatter",   "SCATTER",       "Bullets fan outward",           "✦", 180, false,false, {255,140, 60,255}, 1,1, "weapon"},
{"piercing",  "PIERCING",      "Bullets pierce enemies",        "→", 220, false,false, {160,255,180,255}, 1,1, "weapon"},
{"explosive", "EXPLOSIVE",     "Bullets explode on impact",     "💥",250, false,false, {255, 80, 60,255}, 1,1, "weapon"},
{"cooldown",  "COOL CORE",     "Reduces heat buildup",          "❄", 150, false,false, { 80,220,255,255}, 1,3, "weapon"},
{"chiflow",   "CHI FLOW",      "Builds chi gauge faster",       "☯", 160, false,false, {200, 80,255,255}, 1,3, "special"},
{"chiburst",  "CHI BURST",     "Chi boosts bullet damage",      "✸", 200, false,false, {255,160,220,255}, 1,1, "special"},
{"crit",      "CRITICAL HIT",  "Chance for double damage",      "★", 180, false,false, {255,210, 50,255}, 1,1, "special"},
{"goldmag",   "GOLD MAGNET",   "Attract golden enemy drops",    "◉", 140, false,false, {255,200, 40,255}, 1,1, "special"},
{"vampire",   "VAMPIRIC",      "Restore life every 10 kills",   "♥", 300, false,false, {255, 80,160,255}, 1,1, "special"},
{"bigbang",   "SHRINE NOVA",   "Every 50 kills: destroy all",   "✺", 500, false,false, {220, 80,255,255}, 1,1, "special"},
{"overclock", "OVERCLOCK",     "Heat vents faster over time",   "⚙", 170, false,false, {255,140, 40,255}, 1,3, "engine"},
{"warpcore",  "WARP CORE",     "Enables short-range warp",      "⬡", 280, false,false, { 80,160,255,255}, 1,1, "engine"},
{"afterburn", "AFTERBURN",     "Boost speed when overheated",   "🔥",220, false,false, {255,100, 40,255}, 1,1, "engine"},

// ── INTERCEPTOR SKILLS (ship 0 only) ──
{"sk0_dash",     "AFTERBURNER",  "Shift: dash in move direction", "▶▶",200, false,false, {  0,220,255,255}, 1,1, "skill_0"},
{"sk0_ricochet", "RICOCHET",     "Bullets bounce 3x on walls",   "↩", 280, false,false, {  0,210,185,255}, 1,1, "skill_0"},
{"sk0_momentum", "MOMENTUM",     "Speed amplifies bullet damage", "⚡",320, false,false, {  0,255,150,255}, 1,1, "skill_0"},

// ── BRAWLER SKILLS (ship 1 only) ──
{"sk1_regen",    "REGEN",        "Slowly regain HP in combat",   "♻", 220, false,false, {255, 80,120,255}, 1,1, "skill_1"},
{"sk1_shockwave","SHOCKWAVE",    "Ramming enemies triggers AoE", "◎", 300, false,false, {255,140, 60,255}, 1,1, "skill_1"},
{"sk1_charge",   "BERSERKER",    "Each kill: +5% damage (max 3x)","▲",350, false,false, {255,200, 50,255}, 1,1, "skill_1"},

// ── PHANTOM SKILLS (ship 2 only) ──
{"sk2_cloak",    "CLOAK",        "Taking damage: 2s invisibility","◌", 240, false,false, {180, 80,255,255}, 1,1, "skill_2"},
{"sk2_sniper",   "SNIPER",       "First bullet each burst: 4x dmg","◎",300, false,false, {255, 40,160,255}, 1,1, "skill_2"},
{"sk2_phase",    "PHASE STRIKE", "Bullets pierce up to 3 enemies","→", 320, false,false, {  0,140,200,255}, 1,1, "skill_2"},

// ── TITAN SKILLS (ship 3 only) ──
{"sk3_barrier",  "VOID BARRIER", "Block 1 hit every 8s auto",   "⬡", 250, false,false, {255,245,120,255}, 1,1, "skill_3"},
{"sk3_gravity",  "GRAVITY WELL", "Enemies pulled toward you",   "◉", 300, false,false, { 60, 30,120,255}, 1,1, "skill_3"},
{"sk3_overload", "OVERLOAD",     "Kills charge +2% dmg (max 2x)","▲",350, false,false, {255,200, 50,255}, 1,1, "skill_3"},

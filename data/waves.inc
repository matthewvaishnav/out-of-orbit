// Wave definitions: { killTarget, {enemyTypes...}, hasBoss, bossIdx, themeName, bgScale, spawnTicks }
// 28 waves total (indices 0–27)
{15,{E_BASIC},                          false,0,"Drifting Debris",   1.0f,90},  // 0
{18,{E_BASIC,E_FAST},                   false,0,"Neon Wastes",       1.0f,80},  // 1
{20,{E_BASIC,E_FAST,E_TANK},            false,0,"Plasma Fields",     1.1f,80},  // 2
{22,{E_FAST,E_TANK},                    true, 0,"Abyssal Trench",    1.1f,75},  // 3
{24,{E_BASIC,E_CURSED},                 false,0,"Cursed Expanse",    1.1f,75},  // 4
{25,{E_FAST,E_CURSED,E_BOMB},           false,0,"Volatile Rim",      1.2f,70},  // 5
{26,{E_TANK,E_CURSED,E_BOMB},           true, 1,"Void Rift",         1.2f,70},  // 6
{28,{E_BASIC,E_FAST,E_BOMB},            false,0,"Shrapnel Storm",    1.2f,65},  // 7
{28,{E_GOLDEN,E_FAST,E_BASIC},          false,0,"Golden Tide",       1.2f,65},  // 8
{30,{E_CRYSTAL,E_FAST,E_TANK},          true, 2,"Resonance Core",    1.3f,65},  // 9
{30,{E_CRYSTAL,E_CURSED,E_BOMB},        false,0,"Crystal Plague",    1.3f,60},  // 10
{32,{E_VOID,E_BASIC,E_FAST},            false,0,"Void Emergence",    1.3f,60},  // 11
{32,{E_VOID,E_TANK,E_BOMB},             true, 3,"Dark Convergence",  1.3f,60},  // 12
{34,{E_VOID,E_CURSED,E_CRYSTAL},        false,0,"Entropy Wave",      1.4f,55},  // 13
{35,{E_VOID,E_BOMB,E_GOLDEN},           false,0,"Gilded Oblivion",   1.4f,55},  // 14
{36,{E_CURSED,E_VOID,E_CRYSTAL},        true, 4,"Cursed Vortex",     1.4f,55},  // 15
{36,{E_TANK,E_VOID,E_BOMB},             false,0,"Iron Tempest",      1.4f,50},  // 16
{38,{E_FAST,E_VOID,E_CURSED},           false,0,"Shadow Rush",       1.5f,50},  // 17
{38,{E_CRYSTAL,E_VOID,E_GOLDEN},        true, 5,"Prismatic Storm",   1.5f,50},  // 18
{40,{E_VOID,E_CURSED,E_CRYSTAL,E_BOMB}, false,0,"Void Singularity",  1.5f,45},  // 19
{40,{E_VOID,E_CURSED,E_BOMB,E_FAST},    true, 6,"Chaos Awakening",   1.6f,45},  // 20
{42,{E_VOID,E_CRYSTAL,E_TANK,E_CURSED}, false,0,"Fractured Abyss",   1.6f,45},  // 21
{42,{E_VOID,E_BOMB,E_CURSED,E_GOLDEN},  false,0,"Gilded Apocalypse", 1.6f,40},  // 22
{44,{E_VOID,E_FAST,E_CRYSTAL,E_BOMB},   true, 7,"Resonant Doom",     1.7f,40},  // 23
{44,{E_VOID,E_CURSED,E_TANK,E_BOMB},    false,0,"Iron Oblivion",     1.7f,40},  // 24
{46,{E_VOID,E_CRYSTAL,E_CURSED,E_FAST}, false,0,"Crystal Requiem",   1.7f,38},  // 25
{46,{E_VOID,E_BOMB,E_TANK,E_CURSED},    true, 8,"Final Storm",       1.8f,35},  // 26
{50,{E_VOID,E_CURSED,E_CRYSTAL,E_BOMB,E_FAST},true,9,"Void Collapse", 1.8f,30}, // 27

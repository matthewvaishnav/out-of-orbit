// EnemyDef: { hp, radius, score, speed, col, golden, cursed, bomb, chain, sym }
// Order must match EnemyType enum: E_BASIC,E_FAST,E_TANK,E_GOLDEN,E_CURSED,E_BOMB,E_CRYSTAL,E_VOID
// EnemyDef: { hp, radius, score, speed, col, golden, cursed, bomb, chain, sym }
// Order must match EnemyType enum: E_BASIC,E_FAST,E_TANK,E_GOLDEN,E_CURSED,E_BOMB,E_CRYSTAL,E_VOID
{ 2, 22.f,  10, 80.f,  {180, 80,220,255}, false,false,false,false, "▲" }, // E_BASIC
{ 1, 18.f,  15, 140.f, { 80,220,255,255}, false,false,false,false, "◆" }, // E_FAST
{ 6, 34.f,  25, 50.f,  {220,120, 30,255}, false,false,false,false, "■" }, // E_TANK
{ 2, 22.f,  50, 90.f,  {255,210, 50,255}, true, false,false,false, "★" }, // E_GOLDEN
{ 3, 24.f,  20, 85.f,  {180, 30,220,255}, false,true, false,false, "✦" }, // E_CURSED
{ 2, 20.f,  18, 75.f,  {255, 80, 60,255}, false,false,true, false, "●" }, // E_BOMB
{ 4, 26.f,  30, 60.f,  { 80,255,210,255}, false,false,false,true,  "❖" }, // E_CRYSTAL
{ 5, 28.f,  40, 95.f,  { 60, 40,180,255}, false,true, false,false, "◈" }, // E_VOID

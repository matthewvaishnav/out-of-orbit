static float gPlayerHealthMultiplier = 1.0f;
float GetPlayerHealthMultiplier(){ return gPlayerHealthMultiplier; }

// game.cpp — Game initialization, shop setup, wave planets, ship skills
#include "void_blaster.h"

void SpawnWavePlanets(Game& g, int wave){
    g.planets.clear();
    // Planet screen position = planet.pos - camera*parallax
    // To place planet at screen fraction (x,y) regardless of camera:
    //   planet.pos = x*SCREEN_W + camera.x*parallax,  y*SCREEN_H + camera.y*parallax
    float camX = g.cameraPos.x, camY = g.cameraPos.y;
    struct PS{ float x,y,r,par; Color col,ring; bool ring2; int pat; };
    // Helper macro — pushes every entry from a PS array into g.planets
    #define PUSH_PLANETS(s) for(auto& p:(s)){Planet pl;pl.pos={p.x*SCREEN_W+camX*p.par,p.y*SCREEN_H+camY*p.par};pl.radius=p.r;pl.parallax=p.par;pl.ringAngle=RandF(.3f,.8f);pl.col=p.col;pl.ringCol=p.ring;pl.hasRing=p.ring2;pl.patternType=p.pat;pl.phase=RandF(0,2*PI);g.planets.push_back(pl);}

    switch(std::min(wave,19)){
    case 0:{ // OCEAN WORLD — deep blues + teals, lots of ringed gas giants, 11 bodies
        // Dominant: enormous ocean world + two massive ringed gas giants
        // Mid: 3 medium ocean worlds with continental patterns
        // Far: 5 small icy moons
        PS s[]={
            {0.50f,0.18f,76,.008f,{8,45,120,255}, {50,220,255,255},true ,0}, // enormous ocean world
            {0.14f,0.30f,52,.014f,{12,60,150,255},{40,210,250,255},true ,0}, // big ringed gas giant left
            {0.80f,0.26f,48,.016f,{10,52,135,255},{45,215,245,255},true ,2}, // big ringed gas giant right
            {0.35f,0.70f,36,.022f,{14,70,160,255},{35,200,240,255},true ,1}, // medium ocean world
            {0.70f,0.65f,30,.026f,{9,48,130,255}, {30,195,235,255},false,0}, // medium continental
            {0.18f,0.78f,22,.034f,{16,78,170,255},{55,225,248,255},false,2}, // medium coastal
            {0.88f,0.72f,18,.040f,{7,40,110,255}, {25,185,230,255},false,1}, // small
            {0.55f,0.85f,15,.046f,{11,58,140,255},{38,205,242,255},false,0}, // small
            {0.04f,0.50f,13,.050f,{6,35,100,255}, {20,178,225,255},false,2}, // small
            {0.92f,0.44f,10,.058f,{8,42,115,255}, {28,190,232,255},false,1}, // tiny icy moon
            {0.42f,0.92f,8, .064f,{5,30,90,255},  {15,168,218,255},false,0}, // tiny moon
        }; PUSH_PLANETS(s); break;}

    case 1:{ // TOXIC SWAMP — acid greens + sickly yellows, 11 bodies
        // Dominant: two enormous toxic gas giants flanking a massive swamp world
        // Mid: 3 acid-pool worlds with gas-giant pattern
        // Far: 5 tiny irradiated moons
        PS s[]={
            {0.18f,0.16f,68,.009f,{35,95,15,255}, {150,255,55,255},true ,2}, // enormous toxic gas left
            {0.78f,0.18f,60,.011f,{50,112,10,255},{170,255,48,255},true ,0}, // enormous gas right
            {0.50f,0.65f,46,.018f,{42,105,12,255},{160,252,50,255},true ,1}, // massive swamp world
            {0.28f,0.55f,32,.026f,{38,92,16,255}, {140,248,46,255},false,2}, // acid pool world
            {0.72f,0.60f,28,.030f,{45,108,11,255},{158,254,52,255},false,0}, // acid pool right
            {0.08f,0.72f,22,.036f,{40,100,14,255},{148,250,48,255},false,1}, // medium left
            {0.88f,0.65f,18,.040f,{32,88,17,255}, {130,244,44,255},false,2}, // medium right
            {0.45f,0.85f,14,.048f,{36,94,15,255}, {138,246,46,255},false,0}, // small
            {0.62f,0.88f,12,.054f,{28,82,13,255}, {125,240,42,255},false,1}, // small
            {0.20f,0.90f,10,.060f,{25,78,11,255}, {118,236,40,255},false,2}, // tiny
            {0.95f,0.40f,8, .066f,{22,72,10,255}, {112,230,38,255},false,0}, // tiny
        }; PUSH_PLANETS(s); break;}

    case 2:{ // EMBER FIELDS — volcanic reds, smouldering orange giants, 11 bodies
        // Dominant: one colossal lava-world + one massive ringed inferno giant
        // Mid: 3 volcanically active worlds (rocky pattern)
        // Far: 5 smouldering cinder moons
        PS s[]={
            {0.22f,0.16f,80,.007f,{175,22,4,255}, {255,120,20,255},true ,1}, // colossal lava world
            {0.70f,0.20f,58,.012f,{200,40,6,255}, {255,140,28,255},true ,0}, // big ringed inferno giant
            {0.45f,0.68f,44,.018f,{155,18,4,255}, {240,100,16,255},true ,2}, // large volcanic
            {0.88f,0.55f,30,.028f,{185,28,5,255}, {255,115,22,255},false,1}, // mid rocky inferno
            {0.08f,0.58f,34,.024f,{165,20,4,255}, {248,108,18,255},false,0}, // mid volcanic left
            {0.60f,0.40f,22,.036f,{190,32,6,255}, {255,125,24,255},false,2}, // mid
            {0.28f,0.82f,16,.044f,{170,24,5,255}, {245,110,20,255},false,1}, // small moon
            {0.78f,0.78f,14,.048f,{160,18,4,255}, {238,105,18,255},false,0}, // small moon
            {0.50f,0.90f,12,.054f,{150,15,3,255}, {230,98,16,255},false,2},  // tiny moon bottom
            {0.14f,0.90f,9, .060f,{140,12,3,255}, {220,90,14,255},false,1},  // tiny moon
            {0.92f,0.38f,8, .065f,{130,10,2,255}, {210,85,12,255},false,0},  // tiny cinder
        }; PUSH_PLANETS(s); break;}

    case 3:{ // CRYSTAL CAVERNS — cool teal/mint, many dense crystalline bodies, 11 bodies
        // Dominant: two enormous teal gas giants with bright mint rings
        // Mid: 4 compact crystal-textured worlds at varied distances
        // Far: 5 tiny ice-crystal moons with mint rims
        PS s[]={
            {0.22f,0.18f,70,.009f,{6,105,118,255},{25,245,225,255},true ,2}, // enormous teal gas giant
            {0.76f,0.20f,60,.011f,{8,118,130,255},{30,250,230,255},true ,0}, // enormous teal right
            {0.50f,0.60f,44,.018f,{5,92,104,255}, {18,238,218,255},true ,1}, // large crystal world
            {0.20f,0.68f,32,.026f,{7,108,122,255},{22,242,222,255},false,2}, // medium crystal
            {0.80f,0.65f,28,.030f,{9,115,128,255},{28,246,226,255},false,0}, // medium crystal
            {0.40f,0.82f,22,.036f,{4,88,100,255}, {15,235,215,255},false,1}, // small
            {0.65f,0.80f,18,.042f,{6,98,112,255}, {20,240,220,255},false,2}, // small
            {0.10f,0.50f,14,.048f,{8,112,125,255},{24,244,224,255},false,0}, // small
            {0.90f,0.48f,12,.054f,{5,88,102,255}, {16,236,216,255},false,1}, // tiny
            {0.55f,0.90f,10,.060f,{7,100,114,255},{20,240,220,255},false,2}, // tiny
            {0.04f,0.35f,8, .068f,{4,82,95,255},  {12,230,210,255},false,0}, // tiny far moon
        }; PUSH_PLANETS(s); break;}

    case 4:{ // RESONANCE CORE — gold/amber, massive resonant worlds, 11 bodies
        // Dominant: one spectacular gold super-giant with wide amber rings
        // Mid: 3 large amber worlds with rings — one per pattern type
        // Far: 6 gold moons of decreasing size
        PS s[]={
            {0.50f,0.14f,84,.007f,{185,128,12,255},{255,225,62,255},true ,0}, // spectacular gold super-giant
            {0.14f,0.30f,50,.014f,{170,112,10,255},{250,218,58,255},true ,1}, // large amber with ring
            {0.82f,0.26f,44,.018f,{178,120,11,255},{252,222,60,255},true ,2}, // large amber with ring
            {0.30f,0.65f,36,.023f,{165,105,9,255}, {248,215,56,255},true ,0}, // medium amber
            {0.68f,0.62f,30,.028f,{180,125,12,255},{254,220,62,255},false,1}, // medium
            {0.10f,0.72f,24,.034f,{162,102,8,255}, {246,212,54,255},false,2}, // medium
            {0.88f,0.70f,18,.040f,{175,115,10,255},{252,218,58,255},false,0}, // small
            {0.48f,0.82f,16,.046f,{168,108,9,255}, {248,215,56,255},false,1}, // small
            {0.25f,0.88f,12,.052f,{158,98,8,255},  {242,210,52,255},false,2}, // tiny
            {0.75f,0.86f,10,.058f,{155,95,7,255},  {240,208,50,255},false,0}, // tiny
            {0.55f,0.92f,8, .064f,{150,90,6,255},  {235,204,48,255},false,1}, // tiny far moon
        }; PUSH_PLANETS(s); break;}

    case 5:{ // SPORE WASTES — murky olive, toxic yellow, 11 bodies
        // Dominant: one vast spore-cloud world + one huge fog-shrouded gas giant
        // Mid: dense cluster of olive moons with ring systems
        // Far: scattered tiny spore moons
        PS s[]={
            {0.50f,0.16f,72,.009f,{62,82,6,255},  {185,205,20,255},true ,0}, // vast spore world
            {0.16f,0.32f,54,.014f,{55,74,8,255},  {170,192,17,255},true ,1}, // huge fog giant left
            {0.80f,0.26f,44,.018f,{70,92,5,255},  {192,210,22,255},true ,2}, // large right
            {0.35f,0.68f,34,.024f,{58,78,7,255},  {178,198,19,255},true ,0}, // medium with ring
            {0.70f,0.62f,28,.030f,{65,86,6,255},  {188,208,21,255},false,1}, // medium
            {0.12f,0.75f,22,.036f,{52,70,8,255},  {165,185,16,255},false,2}, // medium left
            {0.88f,0.70f,18,.040f,{68,88,5,255},  {190,212,23,255},false,0}, // small
            {0.48f,0.85f,14,.048f,{56,76,7,255},  {175,195,18,255},false,1}, // small
            {0.25f,0.88f,12,.054f,{50,68,9,255},  {162,182,15,255},false,2}, // small
            {0.78f,0.88f,10,.060f,{48,65,6,255},  {158,178,14,255},false,0}, // tiny
            {0.06f,0.92f,8, .066f,{44,60,5,255},  {152,172,13,255},false,2}, // tiny
        }; PUSH_PLANETS(s); break;}

    case 6:{ // STELLAR FORGE — fiery oranges, dense forgeworld cluster, 11 bodies
        // Dominant: one blazing forge-giant + one orange lava-ringed world
        // Mid: 4 fiery worlds — mix of gas/rocky/continental
        // Far: 5 cinder-moons
        PS s[]={
            {0.22f,0.16f,72,.008f,{198,72,9,255}, {255,198,50,255},true ,0}, // blazing forge-giant
            {0.76f,0.20f,60,.012f,{215,88,12,255},{255,210,55,255},true ,1}, // orange lava-ringed
            {0.50f,0.65f,44,.018f,{182,62,8,255}, {248,186,45,255},true ,2}, // large fiery world
            {0.18f,0.70f,32,.026f,{200,78,10,255},{255,200,52,255},false,0}, // medium rocky
            {0.80f,0.68f,28,.030f,{205,82,11,255},{255,202,53,255},false,1}, // medium
            {0.40f,0.82f,22,.036f,{188,68,9,255}, {250,190,48,255},false,2}, // medium
            {0.65f,0.78f,18,.042f,{195,74,10,255},{252,194,50,255},false,0}, // small
            {0.10f,0.55f,14,.048f,{180,60,7,255}, {244,182,44,255},false,1}, // small
            {0.88f,0.52f,12,.054f,{175,58,7,255}, {242,180,43,255},false,2}, // small
            {0.48f,0.90f,10,.060f,{170,55,6,255}, {240,178,42,255},false,0}, // tiny cinder
            {0.30f,0.92f,8, .066f,{165,52,6,255}, {238,175,41,255},false,1}, // tiny cinder
        }; PUSH_PLANETS(s); break;}

    case 7:{ // OBSIDIAN WASTES — cold charcoal, steel-blue highlights, 11 bodies
        // Dominant: one colossal charcoal gas giant with cyan rings + 2 large steel worlds
        // Mid: 4 dark worlds with cold blue-grey tones
        // Far: 4 razor-edged moons
        PS s[]={
            {0.50f,0.16f,78,.008f,{20,20,32,255}, {95,178,238,255},true ,2}, // colossal charcoal gas giant
            {0.14f,0.28f,54,.013f,{26,26,40,255}, {88,168,228,255},true ,0}, // large steel world left
            {0.82f,0.24f,48,.016f,{24,24,38,255}, {92,172,232,255},true ,1}, // large steel world right
            {0.32f,0.62f,38,.022f,{22,22,35,255}, {84,162,222,255},false,2}, // medium cold world
            {0.70f,0.60f,32,.027f,{28,28,42,255}, {96,175,235,255},false,0}, // medium
            {0.10f,0.72f,24,.034f,{18,18,30,255}, {78,158,218,255},false,1}, // medium
            {0.88f,0.68f,18,.040f,{25,25,40,255}, {90,170,230,255},false,2}, // small
            {0.48f,0.82f,14,.046f,{16,16,28,255}, {72,152,212,255},false,0}, // small
            {0.25f,0.88f,12,.052f,{20,20,33,255}, {80,160,222,255},false,1}, // small
            {0.75f,0.86f,10,.058f,{15,15,26,255}, {68,148,208,255},false,2}, // tiny
            {0.06f,0.55f,8, .066f,{12,12,22,255}, {62,142,202,255},false,0}, // tiny
        }; PUSH_PLANETS(s); break;}

    case 8:{ // CRIMSON NEBULA — deep reds + magenta, 11 bodies
        // Dominant: twin giant continental worlds flanking a ringed gas giant
        // Mid: 3 smouldering medium worlds with rings
        // Far: 5 tiny blood-red moons drifting at high parallax
        PS s[]={
            {0.50f,0.17f,74,.008f,{110,8,28,255}, {255,80,160,255},true ,2}, // massive ring giant, screen-centre top
            {0.12f,0.22f,52,.014f,{130,18,45,255},{255,90,150,255},true ,0}, // large continental, left
            {0.82f,0.24f,46,.016f,{150,25,50,255},{240,70,140,255},true ,0}, // large continental, right
            {0.30f,0.62f,38,.022f,{105,14,38,255},{220,60,120,255},true ,1}, // mid rocky, lower-left
            {0.68f,0.58f,32,.026f,{140,20,46,255},{255,100,170,255},true ,2}, // mid gas, lower-right
            {0.50f,0.80f,24,.032f,{90,10,32,255}, {200,55,110,255},false,1}, // mid, bottom
            {0.15f,0.55f,16,.040f,{120,16,42,255},{185,50,100,255},false,0}, // small, left mid
            {0.85f,0.50f,18,.038f,{100,12,36,255},{170,45,95,255},false,2},  // small, right mid
            {0.38f,0.88f,11,.052f,{80,10,30,255}, {155,40,85,255},false,1},  // tiny moon, bottom
            {0.70f,0.85f,9, .058f,{75,8,28,255},  {145,35,80,255},false,0},  // tiny moon, bottom-right
            {0.06f,0.78f,8, .062f,{70,7,25,255},  {135,30,75,255},false,2},  // tiny moon, far-left
        }; PUSH_PLANETS(s); break;}

    case 9:{ // ABYSSAL TRENCH — near-black ocean, bioluminescent teal, 11 bodies
        // Dominant: one leviathan-class dark world with teal ring + two large abyss giants
        // Mid: dense cluster of bio-lit worlds at varied depths
        // Far: 5 pitch-black moons with faint teal rims
        PS s[]={
            {0.50f,0.14f,82,.007f,{1,8,30,255},   {0,210,190,255},true ,0}, // leviathan abyss world
            {0.16f,0.28f,54,.013f,{2,12,38,255},  {5,200,182,255},true ,1}, // big dark giant left
            {0.82f,0.22f,50,.015f,{3,15,45,255},  {8,215,195,255},true ,2}, // big dark giant right
            {0.38f,0.62f,40,.020f,{2,10,34,255},  {4,195,178,255},true ,0}, // large deep world
            {0.72f,0.68f,32,.026f,{3,14,42,255},  {6,205,186,255},false,1}, // medium
            {0.10f,0.65f,28,.030f,{1,9,32,255},   {3,190,174,255},true ,2}, // medium with ring
            {0.58f,0.82f,20,.038f,{2,11,36,255},  {5,198,180,255},false,0}, // small
            {0.26f,0.85f,16,.044f,{1,8,28,255},   {2,188,170,255},false,2}, // small
            {0.88f,0.78f,13,.050f,{2,12,40,255},  {6,202,184,255},false,1}, // small
            {0.48f,0.92f,10,.056f,{1,7,26,255},   {1,182,166,255},false,0}, // tiny
            {0.04f,0.82f,8, .064f,{1,6,22,255},   {0,176,160,255},false,2}, // tiny far moon
        }; PUSH_PLANETS(s); break;}

    case 10:{ // SHARD TEMPEST — electric gold/white, fractured asteroid belt, 11 bodies
        // Dominant: one enormous golden ringed mega-world + 2 large crystal asteroid-bodies
        // Mid: many small dense worlds — the belt feel
        // Far: 5 tiny glinting shards
        PS s[]={
            {0.50f,0.16f,76,.008f,{228,196,22,255},{255,248,105,255},true ,0}, // enormous golden ring mega-world
            {0.18f,0.35f,34,.024f,{215,182,18,255},{255,240,92,255},false,2}, // large crystal shard left
            {0.80f,0.30f,30,.028f,{222,188,20,255},{255,244,98,255},false,1}, // large crystal shard right
            {0.35f,0.55f,24,.034f,{208,175,16,255},{252,236,88,255},false,0}, // medium
            {0.65f,0.58f,20,.038f,{218,184,19,255},{255,242,95,255},false,2}, // medium
            {0.12f,0.65f,16,.044f,{212,178,17,255},{254,238,90,255},false,1}, // small
            {0.85f,0.62f,14,.048f,{205,172,15,255},{250,234,86,255},false,0}, // small
            {0.50f,0.80f,12,.054f,{210,176,16,255},{253,237,88,255},false,2}, // small
            {0.28f,0.86f,10,.060f,{200,168,14,255},{248,232,84,255},false,1}, // tiny shard
            {0.72f,0.84f,9, .064f,{198,165,13,255},{246,230,82,255},false,0}, // tiny shard
            {0.06f,0.82f,8, .070f,{195,162,12,255},{244,228,80,255},false,2}, // tiny shard
        }; PUSH_PLANETS(s); break;}

    case 11:{ // ASHEN CITADEL — smouldering dark embers, 11 bodies
        // Dominant: one enormous ashen citadel-world with amber rings + 2 large smouldering worlds
        // Mid: 4 ember-lit worlds
        // Far: 4 dark ash moons
        PS s[]={
            {0.50f,0.14f,80,.007f,{52,26,7,255},  {215,128,36,255},true ,0}, // enormous ashen citadel-world
            {0.16f,0.28f,54,.013f,{62,32,9,255},  {208,122,34,255},true ,2}, // large smouldering left
            {0.82f,0.22f,48,.016f,{58,29,8,255},  {212,125,35,255},true ,1}, // large smouldering right
            {0.30f,0.65f,36,.022f,{55,28,8,255},  {205,118,32,255},true ,0}, // medium with ring
            {0.70f,0.60f,30,.028f,{60,30,9,255},  {210,122,34,255},false,2}, // medium
            {0.10f,0.72f,22,.036f,{48,24,7,255},  {198,112,30,255},false,1}, // medium
            {0.88f,0.68f,18,.040f,{65,33,10,255}, {215,128,36,255},false,0}, // small
            {0.48f,0.82f,14,.046f,{50,25,7,255},  {200,115,31,255},false,2}, // small
            {0.25f,0.88f,12,.052f,{45,22,6,255},  {192,108,28,255},false,1}, // small
            {0.75f,0.86f,10,.058f,{42,21,6,255},  {188,105,27,255},false,0}, // tiny
            {0.55f,0.92f,8, .064f,{38,18,5,255},  {182,100,25,255},false,2}, // tiny
        }; PUSH_PLANETS(s); break;}

    case 12:{ // VOID RIFT — deep purples + ultraviolet, 11 bodies with heavy ring density
        // Dominant: one colossal void-giant with triple rings + two large ultraviolet worlds
        // Mid: 4 violet worlds all with rings — distinctive zone feel
        // Far: 4 tiny obsidian moons
        PS s[]={
            {0.50f,0.15f,78,.008f,{45,6,85,255},  {185,85,255,255},true ,2}, // colossal void-giant
            {0.14f,0.28f,56,.013f,{65,10,108,255},{195,100,255,255},true ,0}, // large UV world left
            {0.82f,0.22f,50,.016f,{55,8,95,255},  {188,92,252,255},true ,1}, // large UV world right
            {0.32f,0.65f,38,.022f,{58,9,100,255}, {182,88,248,255},true ,2}, // medium with ring
            {0.70f,0.60f,34,.025f,{62,10,106,255},{190,95,252,255},true ,0}, // medium with ring
            {0.15f,0.72f,24,.034f,{48,7,88,255},  {175,80,242,255},true ,1}, // small with ring
            {0.88f,0.68f,20,.038f,{52,8,92,255},  {180,84,245,255},false,2}, // small
            {0.50f,0.82f,16,.044f,{44,6,82,255},  {170,78,238,255},false,0}, // small
            {0.28f,0.90f,12,.052f,{40,5,75,255},  {162,72,230,255},false,1}, // tiny
            {0.72f,0.88f,10,.058f,{38,5,72,255},  {158,68,225,255},false,2}, // tiny
            {0.04f,0.55f,8, .065f,{35,4,68,255},  {152,64,220,255},false,0}, // tiny far moon
        }; PUSH_PLANETS(s); break;}

    case 13:{ // QUANTUM DEPTHS — deep magenta/pink, exotic ringed worlds, 11 bodies
        // Dominant: one impossible-pink super-world with wide magenta rings + 2 large exotic ringed worlds
        // Mid: 4 deep quantum worlds
        // Far: 4 tiny pink moons
        PS s[]={
            {0.50f,0.14f,80,.008f,{115,4,68,255}, {255,82,182,255},true ,1}, // impossible-pink super-world
            {0.14f,0.28f,52,.013f,{130,6,78,255}, {255,98,198,255},true ,0}, // large ringed exotic left
            {0.82f,0.22f,46,.017f,{122,5,73,255}, {252,90,192,255},true ,2}, // large ringed exotic right
            {0.30f,0.62f,36,.023f,{118,4,70,255}, {248,86,188,255},true ,1}, // medium with ring
            {0.70f,0.58f,32,.027f,{125,5,74,255}, {254,92,195,255},false,0}, // medium
            {0.12f,0.75f,24,.034f,{112,4,66,255}, {244,82,185,255},false,2}, // medium
            {0.88f,0.70f,18,.040f,{108,3,64,255}, {240,78,180,255},false,1}, // small
            {0.48f,0.82f,14,.046f,{115,4,68,255}, {248,86,188,255},false,0}, // small
            {0.25f,0.88f,12,.052f,{105,3,62,255}, {235,72,175,255},false,2}, // tiny
            {0.75f,0.86f,10,.058f,{102,3,60,255}, {232,70,172,255},false,1}, // tiny
            {0.55f,0.92f,8, .064f,{98,2,58,255},  {228,68,170,255},false,0}, // tiny
        }; PUSH_PLANETS(s); break;}

    case 14:{ // PHANTOM GATE — near-black void, ghostly indigo/lavender, 11 bodies
        // Dominant: one vast near-black world with shimmering lavender ring
        // Mid: 3 large ghostly indigo worlds with rings + 2 without
        // Far: 5 barely-visible obsidian moons with faint lavender rims
        PS s[]={
            {0.50f,0.14f,80,.007f,{8,2,20,255},   {155,100,235,255},true ,1}, // vast phantom world
            {0.15f,0.30f,56,.012f,{12,4,26,255},  {145,90,220,255},true ,0}, // large ghostly left
            {0.82f,0.24f,48,.016f,{10,3,22,255},  {148,94,225,255},true ,2}, // large ghostly right
            {0.32f,0.62f,38,.022f,{14,5,30,255},  {140,85,215,255},true ,0}, // medium with ring
            {0.70f,0.68f,34,.025f,{9,3,20,255},   {142,88,218,255},true ,1}, // medium with ring
            {0.12f,0.75f,22,.034f,{16,6,34,255},  {135,80,208,255},false,2}, // medium no ring
            {0.88f,0.65f,18,.040f,{6,2,18,255},   {130,75,200,255},false,0}, // small
            {0.48f,0.82f,14,.046f,{11,4,24,255},  {138,82,212,255},false,1}, // small
            {0.25f,0.88f,12,.052f,{7,2,16,255},   {125,70,195,255},false,2}, // small
            {0.75f,0.85f,10,.058f,{9,3,21,255},   {128,72,198,255},false,0}, // tiny
            {0.06f,0.45f,8, .068f,{5,1,14,255},   {120,65,188,255},false,1}, // tiny far moon
        }; PUSH_PLANETS(s); break;}

    case 15:{ // SOLAR INFERNO — extreme orange/red, blinding bright, 11 bodies
        // Dominant: one enormous solar world + one blazing ringed giant
        // Mid: 4 inferno worlds with different patterns
        // Far: 5 scorched cinder moons
        PS s[]={
            {0.28f,0.14f,76,.008f,{240,68,6,255}, {255,185,32,255},true ,0}, // enormous solar world
            {0.72f,0.18f,62,.011f,{255,88,10,255},{255,205,42,255},true ,1}, // blazing ringed giant
            {0.50f,0.68f,46,.018f,{215,50,4,255}, {252,170,28,255},true ,2}, // large inferno
            {0.15f,0.62f,34,.024f,{230,62,6,255}, {255,180,30,255},false,0}, // medium rocky
            {0.82f,0.60f,28,.030f,{245,80,9,255}, {255,198,40,255},false,1}, // medium
            {0.40f,0.80f,22,.036f,{220,55,5,255}, {250,172,28,255},false,2}, // medium
            {0.65f,0.78f,18,.042f,{235,65,7,255}, {255,182,32,255},false,0}, // small
            {0.10f,0.80f,14,.048f,{210,45,4,255}, {248,165,26,255},false,1}, // small
            {0.88f,0.75f,12,.054f,{205,42,3,255}, {245,162,25,255},false,2}, // small
            {0.50f,0.90f,10,.060f,{200,40,3,255}, {242,160,24,255},false,0}, // tiny scorched
            {0.25f,0.90f,8, .066f,{195,38,2,255}, {238,155,22,255},false,1}, // tiny cinder
        }; PUSH_PLANETS(s); break;}

    case 16:{ // CORONA BREACH — white-gold eruption, 11 massive plasma worlds
        // Dominant: one titan-class white-gold world + 2 enormous plasma worlds
        // Mid: 4 bright eruption worlds
        // Far: 4 golden plasma moons
        PS s[]={
            {0.50f,0.12f,86,.007f,{255,235,150,255},{255,250,205,255},true ,0}, // titan-class white-gold
            {0.16f,0.26f,60,.012f,{248,218,118,255},{255,246,195,255},true ,1}, // enormous plasma left
            {0.82f,0.22f,54,.015f,{250,225,128,255},{255,248,198,255},true ,2}, // enormous plasma right
            {0.32f,0.62f,40,.020f,{245,212,112,255},{252,242,185,255},true ,0}, // large eruption world
            {0.70f,0.58f,34,.025f,{248,218,120,255},{255,245,190,255},false,1}, // large
            {0.10f,0.72f,26,.032f,{242,208,108,255},{250,240,180,255},false,2}, // medium
            {0.88f,0.68f,22,.036f,{245,214,115,255},{253,243,186,255},false,0}, // medium
            {0.50f,0.82f,18,.042f,{240,205,105,255},{248,238,175,255},false,1}, // small
            {0.28f,0.88f,14,.048f,{235,200,100,255},{245,235,170,255},false,2}, // small
            {0.75f,0.86f,12,.054f,{238,202,102,255},{246,236,172,255},false,0}, // tiny
            {0.55f,0.92f,10,.060f,{232,198,98,255}, {242,232,168,255},false,1}, // tiny
        }; PUSH_PLANETS(s); break;}

    case 17:{ // EVENT HORIZON — pure void black, violent magenta rims, 11 bodies
        // Dominant: one colossal black hole world (largest in game) + 2 massive black worlds
        // Mid: 4 pure-void worlds with blazing rims
        // Far: 4 near-invisible moons
        PS s[]={
            {0.50f,0.12f,90,.006f,{2,0,6,255},    {228,45,255,255},true ,2}, // colossal black hole world
            {0.12f,0.30f,58,.012f,{4,0,9,255},    {215,38,248,255},true ,1}, // massive black left
            {0.85f,0.24f,52,.015f,{3,0,7,255},    {220,42,252,255},true ,0}, // massive black right
            {0.28f,0.62f,40,.020f,{5,1,11,255},   {210,34,245,255},true ,2}, // large void world
            {0.72f,0.58f,34,.025f,{3,0,8,255},    {218,40,250,255},false,1}, // large
            {0.08f,0.68f,24,.034f,{4,0,9,255},    {205,30,240,255},false,0}, // medium
            {0.90f,0.65f,20,.038f,{2,0,6,255},    {212,36,246,255},false,2}, // medium
            {0.50f,0.82f,14,.046f,{3,0,7,255},    {200,28,235,255},false,1}, // small
            {0.30f,0.88f,12,.052f,{2,0,5,255},    {195,24,230,255},false,0}, // small
            {0.72f,0.86f,10,.058f,{3,0,6,255},    {190,22,228,255},false,2}, // tiny
            {0.15f,0.92f,8, .065f,{1,0,4,255},    {185,20,225,255},false,1}, // tiny
        }; PUSH_PLANETS(s); break;}

    case 18:{ // THE VOID — pure void darkness, ghostly lavender wisps, 11 bodies
        // Dominant: one abyssal void-world (near-invisible, vast) + 2 large void worlds with purple rings
        // Mid: 4 ethereal void worlds
        // Far: 4 near-invisible tiny moons
        PS s[]={
            {0.50f,0.12f,88,.006f,{3,0,10,255},   {165,85,245,255},true ,2}, // abyssal void-world
            {0.14f,0.28f,60,.011f,{6,1,16,255},   {158,78,238,255},true ,1}, // large void with ring
            {0.82f,0.22f,52,.015f,{5,1,14,255},   {162,82,242,255},true ,0}, // large void with ring
            {0.30f,0.65f,40,.021f,{7,2,18,255},   {155,75,235,255},true ,2}, // medium with ring
            {0.70f,0.60f,34,.026f,{4,1,12,255},   {148,70,228,255},false,1}, // medium
            {0.10f,0.72f,26,.032f,{8,2,20,255},   {158,78,238,255},false,0}, // medium
            {0.88f,0.68f,20,.038f,{5,1,14,255},   {152,72,232,255},false,2}, // small
            {0.48f,0.82f,14,.046f,{3,0,10,255},   {145,68,225,255},false,1}, // small
            {0.25f,0.88f,12,.052f,{4,1,12,255},   {142,65,222,255},false,0}, // small
            {0.75f,0.86f,10,.058f,{2,0,8,255},    {138,62,218,255},false,2}, // tiny near-invisible
            {0.06f,0.50f,8, .066f,{2,0,7,255},    {132,58,212,255},false,1}, // tiny
        }; PUSH_PLANETS(s); break;}

    default:{ // VOID SINGULARITY — absolute darkness, blinding white-gold rims, 11 bodies
        // Dominant: one god-class world (largest possible) + 2 colossal void-giants
        // Mid: 4 impossibly dark worlds with burning gold rims
        // Far: 4 near-invisible moons
        PS s[]={
            {0.50f,0.10f,96,.005f,{0,0,2,255},    {255,235,165,255},true ,2}, // god-class world
            {0.12f,0.26f,64,.011f,{1,0,3,255},    {252,228,158,255},true ,0}, // colossal void-giant left
            {0.86f,0.22f,56,.014f,{1,0,3,255},    {250,225,155,255},true ,1}, // colossal void-giant right
            {0.30f,0.62f,44,.019f,{2,0,5,255},    {245,220,150,255},true ,0}, // large void world
            {0.70f,0.56f,38,.023f,{1,0,4,255},    {248,222,152,255},false,2}, // large
            {0.08f,0.70f,28,.030f,{2,0,5,255},    {242,218,148,255},false,1}, // medium
            {0.90f,0.66f,22,.036f,{1,0,4,255},    {240,215,145,255},false,0}, // medium
            {0.48f,0.82f,16,.044f,{2,0,5,255},    {236,210,140,255},false,2}, // small
            {0.28f,0.88f,14,.050f,{1,0,3,255},    {232,206,136,255},false,1}, // small
            {0.72f,0.86f,12,.056f,{1,0,3,255},    {228,202,132,255},false,0}, // tiny
            {0.55f,0.92f,9, .064f,{1,0,2,255},    {222,196,128,255},false,2}, // tiny
        }; PUSH_PLANETS(s); break;}
    }
    #undef PUSH_PLANETS
}

void InitGame(Game& g){
    int hs=g.highScore; int ss=g.selectedShip; int tier=g.shipUpgradeTier; int rc=g.runCount; int bw=g.bestWave; g={}; g.highScore=hs; g.selectedShip=ss; g.shipUpgradeTier=tier; g.runCount=rc; g.bestWave=bw;
    g.state=PLAYING; g.combo=1; g.comboMax=200;
    // SHIP-SPECIFIC STARTING LIVES:
    // INTERCEPTOR (0): 3 lives (balanced)
    // BRAWLER (1): 4 lives (extra health tank)
    // PHANTOM (2): 2 lives (fragile speedster)
    // TITAN (3): 3 lives + shield (balanced fortress)
    if(g.selectedShip == 1) g.lives=4;      // Brawler: 4 lives
    else if(g.selectedShip == 2) g.lives=2; // Phantom: 2 lives
    else g.lives=3;                          // Interceptor & Titan: 3 lives
    if(g.selectedShip == 3) g.shieldHp=1;   // Titan: starts with shield
    g.wave=0; g.waveTarget=WAVES[0].target; g.spawnInterval=WAVES[0].spawnTicks;
    g.playerPos={WORLD_W/2.f,WORLD_H/2.f};
    g.cameraPos={g.playerPos.x-SCREEN_W/2.f,g.playerPos.y-SCREEN_H/2.f};
    g.prayer=PRAYER_FIRE;
    // ── Skill runtime state reset ──
    g.cloakActive=false; g.cloakTimer=0.f;
    g.regenTimer=0.f;
    g.barrierCooldown=0.f;
    g.overloadMult=1.f; g.overloadKills=0;
    g.sniperReady=true;
    g.momentumDmgMult=1.f;
    g.ghostOrbActive=false; g.ghostOrbAngle=0.f; g.ghostOrbTimer=0.f; g.spiritChainKills=0;
    // ── LEVELING SYSTEM — milestone-gated ship tier ──────────────────────────
    // Tier ONLY increases when BOTH conditions are met:
    //   1. Player has completed enough total runs for this tier bracket
    //   2. Player has reached the required wave depth at least once (bestWave check)
    // This means grinding early waves doesn't help — you have to get DEEP.
    // Tier milestones: tier 1 requires wave 5+, tier 2 requires wave 8+, etc.
    // Each bracket requires MORE runs as tier increases (exponential grind).
    static const int tierWaveReq[10]  = { 5, 8, 12, 15, 18, 20, 22, 24, 26, 27 };
    static const int tierRunsReq[10]  = { 5, 10, 16, 24, 35, 50, 70, 95, 130, 180 };
    g.runCount++;
    if(g.shipUpgradeTier < 10){
        int needed = g.shipUpgradeTier; // index into requirements (tier 0->1 uses slot 0)
        bool waveOk = (g.bestWave >= tierWaveReq[needed]);
        bool runsOk = (g.runCount >= tierRunsReq[needed]);
        if(waveOk && runsOk) g.shipUpgradeTier++;
    }

    // Stars in vaporwave tints
    Color tints[]={VP_WHITE,VP_PINK,VP_CYAN,VP_LAVENDER,VP_PURPLE};
    auto AddStars=[&](int n,int layer,float bMin,float bMax,float rMin,float rMax){
        for(int i=0;i<n;i++){Star s;s.pos={RandF(0,(float)WORLD_W),RandF(0,(float)WORLD_H)};s.brightness=RandF(bMin,bMax);s.radius=RandF(rMin,rMax);s.twinkleSpeed=RandF(.6f,4);s.twinkleOffset=RandF(0,2*PI);s.layer=layer;s.tint=tints[RandI(0,4)];g.stars.push_back(s);}
    };
    AddStars(160,0,.1f,.35f,.4f,.6f); AddStars(90,1,.25f,.65f,.8f,1.2f); AddStars(45,2,.5f,1,.12f,2.2f);

    // Nebulas in pink/purple/cyan
    Color nc[]={VP_DARKPURP,VP_PINK,VP_CYAN,VP_PURPLE,VP_DEEPBLUE};
    for(int i=0;i<NEBULA_COUNT;i++){Nebula n;n.pos={RandF(0,(float)WORLD_W),RandF(0,(float)WORLD_H)};n.radius=RandF(55,140);n.alpha=RandF(.05f,.15f);n.driftOffset=RandF(0,2*PI);n.col=nc[RandI(0,4)];g.nebulas.push_back(n);}

    // Easter eggs — cute per-wave scene decorations (no collision)
    SpawnEasterEggs(0, g.playerPos);

    g.shootStarTimer=0;
    g.shopCredits=0; g.gameWon=false;
    g.idleShootTimer=0; g.nearMissFlash=0; g.progressJuice=0; g.lastKillFlash=0;
    g.chiGauge=0; g.chiRingTimer=0; g.heatVentTimer=0;
    SpawnWavePlanets(g, 0);
    // Only show transition animation on major world changes (every 4 waves)
    if((g.wave % 4 == 0) || g.wave == 0){
        g.waveTransTimer=3.5f;  // 1s pause + 2.5s animation (0.4s fade in + 1.7s hold + 0.4s fade out)
    } else {
        g.waveTransTimer=0;  // No animation for non-world transitions
    }
    RefreshUpgradeCache(g);
}

// ══════════════════════════════════════════════════════════════
//  MAIN
// ══════════════════════════════════════════════════════════════
// ══════════════════════════════════════════════════════════════
//  BORDER PANELS  (drawn in screen-space outside the game rect)
std::vector<Skill> CreateShipSkills(ShipType ship) {
    std::vector<Skill> skills;
    if (ship == SHIP_INTERCEPTOR) {
        skills = {{SKILL_INTERCEPT_DASH, "Dash Matrix", "Speed +25%", false, 0.f, VP_CYAN},
                  {SKILL_INTERCEPT_BURST, "Rapid Pulse", "Fire Rate +40%", false, 0.f, VP_HOTPINK},
                  {SKILL_INTERCEPT_RICOCHET, "Ricochet", "Bullets bounce 3x", false, 0.f, VP_TEAL},
                  {SKILL_INTERCEPT_MOMENTUM, "Momentum", "Dmg scales with speed", false, 0.f, VP_MINT}};
    } else if (ship == SHIP_BRAWLER) {
        skills = {{SKILL_BRAWLER_PLATING, "Plating", "HP +40%", false, 0.f, VP_CORAL},
                  {SKILL_BRAWLER_SHOCKWAVE, "Shockwave", "Collision AoE", false, 0.f, VP_ORANGE},
                  {SKILL_BRAWLER_REGEN, "Regen", "HP/sec in combat", false, 0.f, VP_RED},
                  {SKILL_BRAWLER_CHARGE, "Charge", "8x damage", false, 0.f, VP_GOLD}};
    } else if (ship == SHIP_PHANTOM) {
        skills = {{SKILL_PHANTOM_CLOAK, "Cloak", "Invisible 2s", false, 0.f, VP_PURPLE},
                  {SKILL_PHANTOM_PRECISION, "Sniper", "Crit +30%", false, 0.f, VP_HOTPINK},
                  {SKILL_PHANTOM_SNAPSHOT, "Power Shot", "Charge 5x", false, 0.f, VP_LAVENDER},
                  {SKILL_PHANTOM_PHASE, "Phase", "Pierce 1 enemy", false, 0.f, VP_DEEPCYAN}};
    } else if (ship == SHIP_TITAN) {
        skills = {{SKILL_TITAN_BARRAGE, "Barrage", "5 bullets spread", false, 0.f, VP_YELLOW},
                  {SKILL_TITAN_GRAVITY_WELL, "Gravity", "Pull enemies", false, 0.f, VP_INDIGO},
                  {SKILL_TITAN_EXPLOSIVE, "Explosive", "AoE on hit", false, 0.f, VP_ORANGE},
                  {SKILL_TITAN_OVERLOAD, "Overload", "Stats scale +2% per kill", false, 0.f, VP_GOLD}};
    }
    return skills;
}

void ApplySkillsToWeapon(const std::vector<Skill>& skills, PlayerWeaponState& w, ShipType s) {
    w.fireRate = 8.f; w.bulletDamage = 15.f; w.bulletSpeed = 200.f; w.spreadAngle = 0.f;
    w.bulletCount = 1; w.critChance = 0.05f; w.piercingRounds = false; w.explosiveRounds = false;
    w.ricochetRounds = false; w.damageMultiplier = 1.0f; w.fireRateMultiplier = 1.0f;
    
    for (const auto& sk : skills) {
        if (!sk.unlocked) continue;
        float lv = sk.level / 3.f;
        if (s == SHIP_INTERCEPTOR) {
            if (sk.id == SKILL_INTERCEPT_BURST) { w.fireRateMultiplier += 0.4f * lv; w.damageMultiplier *= (1.0f - 0.2f * lv); }
            if (sk.id == SKILL_INTERCEPT_RICOCHET) w.ricochetRounds = true;
        } else if (s == SHIP_BRAWLER) {
            if (sk.id == SKILL_BRAWLER_PLATING) gPlayerHealthMultiplier = 1.0f + (0.4f * lv);
        } else if (s == SHIP_PHANTOM) {
            if (sk.id == SKILL_PHANTOM_PRECISION) w.critChance += 0.3f * lv;
            if (sk.id == SKILL_PHANTOM_PHASE) w.piercingRounds = true;
        } else if (s == SHIP_TITAN) {
            if (sk.id == SKILL_TITAN_BARRAGE) { w.bulletCount = 1 + (int)(4 * lv); w.spreadAngle = 0.4f; w.bulletDamage *= 0.7f; }
            if (sk.id == SKILL_TITAN_EXPLOSIVE) w.explosiveRounds = true;
            if (sk.id == SKILL_TITAN_OVERLOAD) w.damageMultiplier += lv;
        }
    }
}

int CalculateBulletDamage(const Bullet& b, const PlayerWeaponState& w) {
    int dmg = (int)(b.damage * w.damageMultiplier);
    if ((rand() % 100) / 100.0f < w.critChance) dmg = (int)(dmg * b.critMulti);
    return std::max(1, dmg);
}


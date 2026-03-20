// data.cpp — external game data (waves, enemies, bosses, upgrades)
// Edit the .inc files to balance the game without recompiling logic.
#include "void_blaster.h"

// ── Wave definitions ────────────────────────────────────────────
// Format: { killTarget, {enemyTypes...}, hasBoss, bossIdx, themeName, bgScale, spawnTicks }
// Edit data/waves.inc to adjust difficulty, spawn rates, and wave themes.
std::vector<WaveDef> WAVES = {
#include "data/waves.inc"
};

// ── Enemy definitions ───────────────────────────────────────────
// Format: { hp, radius, score, speed, color, golden, cursed, bomb, chain, symbol }
// Indexed by EnemyType enum order.
EnemyDef ENEMY_DEFS[] = {
#include "data/enemies.inc"
};

// ── Boss definitions ────────────────────────────────────────────
// Format: { hp, radius, score, speed, color, name, symbol }
BossDef BOSS_DEFS[] = {
#include "data/bosses.inc"
};

// ── Upgrade tree ────────────────────────────────────────────────
// Each entry is one purchasable upgrade level.
// Format: { id, displayName, desc, symbol, price, owned, sold, color, level, maxLevel, category }
std::vector<Upgrade> ALL_UPGRADES = {
#include "data/upgrades.inc"
};

// ── Data helpers ─────────────────────────────────────────────────
std::string UpgradeBaseId(const std::string& id){
    auto p=id.rfind('_');
    if(p!=std::string::npos&&p>0){
        std::string s=id.substr(p+1);
        if(s=="2"||s=="3"||s=="4"||s=="5") return id.substr(0,p);
    }
    return id;
}
int OwnedLevel(const Game& g,const std::string& baseId){
    int lv=0;
    for(auto& u:g.upgrades) if(UpgradeBaseId(u.id)==baseId) lv=std::max(lv,u.level);
    return lv;
}

Color LerpC(Color a,Color b,float t){
    return {(unsigned char)(a.r+(b.r-a.r)*t),
            (unsigned char)(a.g+(b.g-a.g)*t),
            (unsigned char)(a.b+(b.b-a.b)*t),
            (unsigned char)(a.a+(b.a-a.a)*t)};
}

PrayerType EnemyWeakness(EnemyType t){
    switch(t){
        case E_BASIC:   return PRAYER_FIRE;
        case E_FAST:    return PRAYER_ICE;
        case E_TANK:    return PRAYER_SPIRIT;
        case E_GOLDEN:  return PRAYER_ICE;
        case E_CURSED:  return PRAYER_FIRE;
        case E_BOMB:    return PRAYER_ICE;
        case E_CRYSTAL: return PRAYER_SPIRIT;
        case E_VOID:    return PRAYER_FIRE;
        default:        return PRAYER_FIRE;
    }
}

Color PrayerColor(PrayerType p){
    if(p==PRAYER_FIRE) return VP_ORANGE;
    if(p==PRAYER_ICE)  return VP_CYAN;
    return VP_PURPLE;
}

const char* PrayerName(PrayerType p){
    if(p==PRAYER_FIRE) return "SOLAR FLARE";
    if(p==PRAYER_ICE)  return "VOID LANCE";
    return "SOUL SWARM";
}

Rectangle GameRect(){
    float ww=(float)GetScreenWidth(), wh=(float)GetScreenHeight();
    float scale=std::min(ww/SCREEN_W, wh/SCREEN_H);
    float gw=SCREEN_W*scale, gh=SCREEN_H*scale;
    return {(ww-gw)*.5f,(wh-gh)*.5f,gw,gh};
}

// Easter egg global data
std::vector<EasterEgg> easterEggs;

// 16 spread spots around the player — wider and more varied spacing
// Spots are in world-space offsets from playerPos
// Arranged to avoid clustering: 4 cardinal-close, 4 diagonal-mid, 4 far-cardinal, 4 far-diagonal
const float EGG_SPOTS[][2]={
    {-280,-100}, // 0  left-near
    { 200,-120}, // 1  right-near
    {-160, 130}, // 2  lower-left-near
    { 240,  90}, // 3  lower-right-near
    {   0, 170}, // 4  bottom-centre
    {-310,  40}, // 5  far-left
    { 280, -30}, // 6  far-right
    {-100,-200}, // 7  upper-left
    { 130, 210}, // 8  lower-right-far
    { -50,-180}, // 9  upper-centre-left
    {  60,-220}, // 10 upper-centre-right
    { 330, 150}, // 11 far-right-lower
    {-340,-150}, // 12 far-left-upper
    {  80, 280}, // 13 far-lower-right
    {-180, 260}, // 14 far-lower-left
    { 250,-200}, // 15 far-upper-right
};

void SpawnEasterEggs(int wave, Vector2 playerPos){
    easterEggs.clear();
    struct Placement { int sub; int spotIdx; };
    // Each wave now uses all 5 slots (no {-1,0} gaps for waves that matter)
    // subType selects the creature variant (0-4 per wave's DrawEasterEggs case)
    // spotIdx uses the expanded 16-spot grid for much better spread
    static const Placement placements[28][6]={
        // Wave 0: OCEAN WORLD — whale+turtle+jellyfish+plankton+seahorse+coral golem
        {{0, 0},{1, 3},{2, 6},{3,10},{5, 2},{6,13}},
        // Wave 1: TOXIC SWAMP — tadpoles+swamp+geyser+golem+flytrap+sludge worm
        {{0, 1},{1, 5},{2, 8},{3,11},{5,14},{6, 4}},
        // Wave 2: EMBER FIELDS — golem+geyser+pool+sprites+salamander+manta
        {{0, 2},{1, 6},{2, 9},{3, 4},{5,12},{6, 7}},
        // Wave 3: CRYSTAL CAVERNS — stalactites+sprites+ice sprite+ice sprite
        {{0, 7},{0,15},{1, 0},{2, 3},{6, 8},{6,11}},
        // Wave 4: RESONANCE CORE — tuning forks+scarab+djinn
        {{0, 4},{0,11},{1, 2},{2, 5},{5, 9},{6,14}},
        // Wave 5: SPORE WASTES — mycelium brains+bloom pod+spore sprite
        {{0, 0},{1, 6},{2,10},{3,13},{5,15},{6, 3}},
        // Wave 6: STELLAR FORGE — golems+clockwork dragon+furnace imp
        {{0, 1},{1, 7},{2,12},{3, 4},{5, 9},{6, 0}},
        // Wave 7: OBSIDIAN WASTES — sentinels+glacial serpent+spider
        {{0, 2},{1, 8},{2,11},{3, 5},{5,14},{6,10}},
        // Wave 8: CRIMSON NEBULA — nebula wisps+plasma bat
        {{0, 3},{1, 7},{2,10},{3, 6},{5,13},{6, 1}},
        // Wave 9: ABYSSAL TRENCH — isopods+anglerfish+squid
        {{0, 0},{1, 4},{2, 9},{3,15},{5,12},{6, 6}},
        // Wave 10: SHARD TEMPEST — meteors+crystal golem+prism phoenix
        {{0, 5},{1, 1},{2, 8},{3,11},{5, 3},{6,14}},
        // Wave 11: ASHEN CITADEL — ash soldiers+cinder drake+bone chariot
        {{0, 6},{1, 2},{2,13},{3, 7},{5,10},{6, 4}},
        // Wave 12: VOID RIFT — shadow moths+void kraken+rift whale
        {{0, 9},{1, 4},{2,14},{3, 0},{5, 6},{6,11}},
        // Wave 13: QUANTUM DEPTHS — doppelgangers+probability cube+glitch entity
        {{0, 5},{1,10},{2,15},{3, 2},{5, 7},{6,12}},
        // Wave 14: PHANTOM GATE — kitsune foxes+spirit lantern+moon rabbit
        {{0, 1},{1, 8},{2,12},{3, 4},{5,11},{6, 6}},
        // Wave 15: SOLAR INFERNO — phoenixes+sun serpent+corona knight
        {{0, 0},{1, 6},{2,13},{3, 9},{5,15},{6, 3}},
        // Wave 16: CORONA BREACH — plasma arches+plasma jellyfish+corona whale
        {{0, 7},{1,11},{2, 3},{3,14},{5, 5},{6, 0}},
        // Wave 17: EVENT HORIZON — accretion disks+time fragment+gravity saint
        {{0, 2},{1, 9},{2,12},{3, 6},{5, 0},{6,15}},
        // Wave 18: THE VOID — void spheres+void leviathan+void bloom
        {{0,10},{1, 4},{2,15},{3, 1},{5, 8},{6,13}},
        // Wave 19: VOID SINGULARITY — great eyes+the architect+cosmic egg
        {{0, 3},{1,11},{2, 7},{3,14},{5, 5},{6, 9}},
        // Wave 20: chaos — full spread
        {{0, 0},{1, 6},{2,10},{3,13},{5, 7},{6, 2}},
        // Wave 21
        {{0, 1},{1, 8},{2,12},{3, 4},{5,15},{6, 9}},
        // Wave 22
        {{0, 2},{1, 9},{2,11},{3, 5},{5,14},{6, 0}},
        // Wave 23
        {{0, 3},{1, 7},{2,13},{3, 6},{5,10},{6, 1}},
        // Wave 24
        {{0, 4},{1, 0},{2,15},{3, 9},{5,11},{6, 8}},
        // Wave 25
        {{0, 5},{1,12},{2, 8},{3, 3},{5, 6},{6,14}},
        // Wave 26
        {{0, 6},{1,14},{2, 1},{3,10},{5, 7},{6,12}},
        // Wave 27: absolute void — barely visible at extreme range
        {{0,12},{1,15},{2, 7},{3,11},{5,13},{6, 4}},
    };
    int wi=std::min(wave,27);
    for(int i=0;i<6;i++){
        EasterEgg e;
        e.waveIdx=wave;
        e.subType=placements[wi][i].sub;
        int si=placements[wi][i].spotIdx;
        e.pos={playerPos.x+EGG_SPOTS[si][0],playerPos.y+EGG_SPOTS[si][1]};
        easterEggs.push_back(e);
    }
}

// ── UpgradeCache refresh ─────────────────────────────────────────
// Call after InitGame and after any shop purchase.
void RefreshUpgradeCache(Game& g){
    auto& c = g.ucache;
    c.rapid      = HasUpgrade(g,"rapid");
    c.afterburn  = HasUpgrade(g,"afterburn");
    c.warpcore   = HasUpgrade(g,"warpcore");
    c.explosive  = HasUpgrade(g,"explosive");
    c.piercing   = HasUpgrade(g,"piercing");
    c.trishot    = HasUpgrade(g,"trishot");
    c.scatter    = HasUpgrade(g,"scatter");
    c.ricochet   = HasUpgrade(g,"ricochet");
    c.chiflow    = HasUpgrade(g,"chiflow");
    c.chiburst   = HasUpgrade(g,"chiburst");
    c.cooldown   = HasUpgrade(g,"cooldown");
    c.crit       = HasUpgrade(g,"crit");
    c.magnet     = HasUpgrade(g,"magnet");
    c.vampire    = HasUpgrade(g,"vampire");
    c.bigbang    = HasUpgrade(g,"bigbang");
    c.goldmag    = HasUpgrade(g,"goldmag");
    c.shield     = HasUpgrade(g,"shield");
    c.overclock  = HasUpgrade(g,"overclock");
    c.cooldownLv = OwnedLevel(g,"cooldown");
    c.overclockLv= OwnedLevel(g,"overclock");
}

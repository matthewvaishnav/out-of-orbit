#pragma once
// ╔══════════════════════════════════════════════════════════════╗
// ║        N E O N   M I K O  ─  VAPORWAVE SHRINE EDiTiON       ║
// ║              Master Header — all shared types                ║
// ╚══════════════════════════════════════════════════════════════╝
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <deque>
#include <memory>

// ══════════════════════════════════════════════════════════════
//  SYNTHWAVE PALETTE
// ══════════════════════════════════════════════════════════════
#define VP_BLACK      CLITERAL(Color){  5,  1, 14,255}
#define VP_DEEPBLUE   CLITERAL(Color){ 10,  6, 42,255}
#define VP_DARKPURP   CLITERAL(Color){ 36, 10, 62,255}
#define VP_PINK       CLITERAL(Color){255,100,200,255}
#define VP_HOTPINK    CLITERAL(Color){255, 40,160,255}
#define VP_CYAN       CLITERAL(Color){  0,220,255,255}
#define VP_DEEPCYAN   CLITERAL(Color){  0,140,200,255}
#define VP_PURPLE     CLITERAL(Color){180, 90,255,255}
#define VP_DEEPPURP   CLITERAL(Color){ 90, 30,160,255}
#define VP_MINT       CLITERAL(Color){  0,255,150,255}
#define VP_YELLOW     CLITERAL(Color){255,245,120,255}
#define VP_GOLD       CLITERAL(Color){255,200, 50,255}
#define VP_ORANGE     CLITERAL(Color){255,130, 50,255}
#define VP_RED        CLITERAL(Color){255, 45, 90,255}
#define VP_DARKRED    CLITERAL(Color){160, 20, 50,255}
#define VP_LAVENDER   CLITERAL(Color){200,165,255,255}
#define VP_WHITE      CLITERAL(Color){255,245,255,255}
#define VP_OFFWHITE   CLITERAL(Color){220,210,235,255}
#define VP_GRAY       CLITERAL(Color){130,100,160,255}
#define VP_MIDGRAY    CLITERAL(Color){ 95, 70,130,255}
#define VP_DARKGRAY   CLITERAL(Color){ 22, 12, 42,255}
#define VP_TEAL       CLITERAL(Color){  0,210,185,255}
#define VP_CORAL      CLITERAL(Color){255, 85,120,255}
#define VP_INDIGO     CLITERAL(Color){ 60, 30,120,255}
#define UI_BG_DARK    CLITERAL(Color){  6,  3, 18,240}
#define UI_BG_MID     CLITERAL(Color){ 14,  8, 32,220}
#define UI_BORDER     CLITERAL(Color){ 80, 50,130,255}
#define GLASS_LIGHT   CLITERAL(Color){255,255,255, 12}
#define GLASS_MID     CLITERAL(Color){150,150,200, 12}

// ══════════════════════════════════════════════════════════════
//  MODERN UI COLOR SYSTEM
// ══════════════════════════════════════════════════════════════
constexpr Color SURFACE_0  = {  3,  2,  10,255};
constexpr Color SURFACE_1  = {  8,  5,  22,255};
constexpr Color SURFACE_2  = { 16, 10,  38,255};
constexpr Color SURFACE_3  = { 28, 18,  56,255};
constexpr Color ACCENT_CYAN   = {  0,240,255,255};
constexpr Color ACCENT_PINK   = {255, 60,180,255};
constexpr Color ACCENT_PURPLE = {180, 80,255,255};
constexpr Color ACCENT_GOLD   = {255,200, 60,255};
constexpr Color ACCENT_MINT   = { 60,255,160,255};
constexpr Color ACCENT_CORAL  = {255, 80,120,255};
constexpr Color TEXT_PRIMARY   = {255,250,255,255};
constexpr Color TEXT_SECONDARY = {180,160,200,255};
constexpr Color TEXT_MUTED     = {100, 85,130,255};
constexpr Color TEXT_SUCCESS   = {120,255,180,255};
constexpr Color TEXT_WARNING   = {255,200, 80,255};
constexpr Color TEXT_DANGER    = {255, 60,100,255};
constexpr Color GLASS_BG       = { 12,  8, 28,220};
constexpr Color GLASS_ACCENT   = { 60, 40,120, 60};
constexpr Color GLOW_CYAN      = {  0,220,255, 80};
constexpr Color GLOW_PINK      = {255, 60,180, 80};
constexpr Color GLOW_PURPLE    = {180, 80,255, 80};

// ══════════════════════════════════════════════════════════════
//  CONSTANTS
// ══════════════════════════════════════════════════════════════
constexpr int   SCREEN_W      = 640;
constexpr int   SCREEN_H      = 400;
constexpr int   SCALE         = 2;
constexpr int   WORLD_W       = 10000000;
constexpr int   WORLD_H       = 10000000;
constexpr int   NEBULA_COUNT  = 22;
constexpr float PLAYER_SPEED  = 118.f;
constexpr float PLAYER_RADIUS = 13.f;
constexpr float PLAYER_RADIUS_L2 = 18.f;

// Bullet pool sizes — pre-allocated, no heap fragmentation mid-frame
constexpr int BULLET_POOL_SIZE      = 1024;
constexpr int BOSS_BULLET_POOL_SIZE = 512;
constexpr int PARTICLE_POOL_SIZE    = 1024;
constexpr int PICKUP_POOL_SIZE      = 256;
constexpr int POPUP_POOL_SIZE       = 128;

// Spatial grid for collision optimization
constexpr int GRID_CELL_SIZE        = 128;
constexpr int GRID_COLS             = (SCREEN_W / GRID_CELL_SIZE) + 2;
constexpr int GRID_ROWS             = (SCREEN_H / GRID_CELL_SIZE) + 2;

// ══════════════════════════════════════════════════════════════
//  ENUMS
// ══════════════════════════════════════════════════════════════
enum GameState  { TITLE, PLAYING, SHOP, GAMEOVER, PAUSED };
enum EnemyType  { E_BASIC,E_FAST,E_TANK,E_GOLDEN,E_CURSED,E_BOMB,E_CRYSTAL,E_VOID };
enum PrayerType { PRAYER_FIRE=0, PRAYER_ICE=1, PRAYER_SPIRIT=2 };
enum ShipType   { SHIP_INTERCEPTOR=0, SHIP_BRAWLER=1, SHIP_PHANTOM=2, SHIP_TITAN=3 };

// ══════════════════════════════════════════════════════════════
//  ENTITY STRUCTS
// ══════════════════════════════════════════════════════════════
struct Bullet {
    Vector2 pos, vel, renderPos;
    int     life        = 0;
    bool    bounced     = false;
    bool    active      = false;
    Color   col         = {};
    float   glowR       = 4.f;
    int     damage      = 1;
    int     prayerType  = 0;   // 0=solar 1=lance 2=swarm 3=trance -1=other
    bool    homing      = false;
    float   homingStr   = 0.f;
    float   burstTimer  = 0.f;
    bool    piercing    = false;
    bool    explosive   = false;
    bool    ricochet    = false;
    float   critChance  = 0.f;
    float   critMulti   = 2.f;
    int     bounceCount = 0;
    int     shipType    = 0;   // 0=Interceptor 1=Brawler 2=Phantom 3=Titan
};

struct Particle {
    Vector2 pos, vel, prevPos, renderPos;
    float   life, maxLife, radius;
    Color   color;
    bool    active, isRing, isSpark;
    float   ringMaxR;
    float   stretch = 1.f;
    float   angle   = 0.f;
    float   angVel  = 0.f;
    bool    isShard = false;
};

struct Pickup   { Vector2 pos, renderPos; float vy,spin; int value,life; bool active; };
struct PopupText{ Vector2 pos, renderPos; std::string text; Color color; float life,maxLife; int fontSize; bool active; };

struct Enemy {
    Vector2  pos, vel, renderPos;
    EnemyType type;
    int      hp, maxHp, scoreVal, flashTimer;
    float    radius, angle, spinSpeed, glowPulse;
    bool     active, golden, cursed, bomb, chain;
    Color    col;
    float    shootTimer = 0.f;
    int      variant    = 0;
};

struct SpatialGrid {
    std::vector<int> cells[GRID_COLS][GRID_ROWS];
    void Clear() {
        for(int x=0; x<GRID_COLS; x++)
            for(int y=0; y<GRID_ROWS; y++)
                cells[x][y].clear();
    }
};

struct Boss {
    Vector2  pos;
    int      hp, maxHp, flashTimer;
    float    radius, angle, angle2;
    int      scoreVal;
    bool     active;
    Color    col;
    std::string name, sym;
    int      phase;
    float    shootTimer;
    int      shootPattern;
    float    dashTimer, dashCooldown;
    Vector2  dashVel;
    float    orbitAngle;
};

struct Upgrade {
    std::string id, name, desc, symbol;
    int    price;
    bool   owned, sold;
    Color  col;
    int    level    = 1;
    int    maxLevel = 5;
    std::string category;
};

struct Star      { Vector2 pos; float brightness,radius,twinkleSpeed,twinkleOffset; int layer; Color tint; };
struct Nebula    { Vector2 pos; float radius,alpha,driftOffset; Color col; };
struct TrailPt   { Vector2 pos; float life; };
struct Hazard    { Vector2 pos; float radius,angle,spinSpeed; int type; bool active; };
struct Planet    { Vector2 pos; float radius,parallax,ringAngle; Color col,ringCol; bool hasRing; int patternType; float phase; };
struct ShootStar { Vector2 pos,vel; float life,maxLife,len; Color col; bool active; };

enum SkillID {
    SKILL_INTERCEPT_DASH=0, SKILL_INTERCEPT_BURST, SKILL_INTERCEPT_RICOCHET, SKILL_INTERCEPT_MOMENTUM,
    SKILL_BRAWLER_PLATING, SKILL_BRAWLER_SHOCKWAVE, SKILL_BRAWLER_REGEN, SKILL_BRAWLER_CHARGE,
    SKILL_PHANTOM_CLOAK, SKILL_PHANTOM_PRECISION, SKILL_PHANTOM_SNAPSHOT, SKILL_PHANTOM_PHASE,
    SKILL_TITAN_BARRAGE, SKILL_TITAN_GRAVITY_WELL, SKILL_TITAN_EXPLOSIVE, SKILL_TITAN_OVERLOAD,
};

struct Skill { int id; std::string name,desc; bool unlocked; float level; Color color; };

struct PlayerWeaponState {
    float fireRate=8.f, bulletDamage=15.f, bulletSpeed=200.f, spreadAngle=0.f, critChance=0.05f;
    int   bulletCount=1;
    bool  piercingRounds=false, explosiveRounds=false, ricochetRounds=false;
    float damageMultiplier=1.0f, fireRateMultiplier=1.0f;
};

// ══════════════════════════════════════════════════════════════
//  DATA STRUCTS (defined in data.cpp)
// ══════════════════════════════════════════════════════════════
struct WaveDef  { int target; std::vector<EnemyType> types; bool hasBoss; int bossIdx; const char* theme; float bgScale; int spawnTicks; };
struct EnemyDef { int hp; float radius; int score; float speed; Color col; bool golden,cursed,bomb,chain; const char* sym; };
struct BossDef  { int hp; float radius; int score; float speed; Color col; const char* name; const char* sym; };

extern std::vector<WaveDef> WAVES;
extern EnemyDef ENEMY_DEFS[];
extern BossDef  BOSS_DEFS[];
extern std::vector<Upgrade> ALL_UPGRADES;

// ══════════════════════════════════════════════════════════════
//  EASTER EGG
// ══════════════════════════════════════════════════════════════
struct EasterEgg { int waveIdx, subType; Vector2 pos; };
extern std::vector<EasterEgg> easterEggs;
extern const float EGG_SPOTS[][2];
void SpawnEasterEggs(int wave, Vector2 playerPos);

// ══════════════════════════════════════════════════════════════
//  GAME STATE
// ══════════════════════════════════════════════════════════════
struct Game {
    GameState state = TITLE;
    int  score=0,highScore=0,lives=0,wave=0,combo=0;
    float comboTimer=0,comboMax=0,heat=0;
    int  shootCooldown=0,killCount=0,totalKills=0,totalCrits=0,biggestCrit=0;
    int  waveKills=0,waveTarget=0,spawnTimer=0,spawnInterval=0;
    bool bossPhase=false,bossSpawned=false;
    int  shieldHp=0,novaCount=0,shopCredits=0;
    float screenShake=0,invincTimer=0;
    int  critFlash=0;
    bool gameWon=false;
    float idleShootTimer=0,nearMissFlash=0,progressJuice=0,lastKillFlash=0;

    Vector2 playerPos={},playerVel={};
    float   playerAngle=0;
    bool    thrusterOn=false;
    float   dashCooldown=0,dashTimer=0;
    Vector2 dashVel={};
    std::deque<TrailPt> engineTrail;

    Vector2 cameraPos={};
    float   camShakeX=0,camShakeY=0;
    float   warpFlash=0,novaFlash=0,bossRumble=0,ambientPulse=0,waveTransTimer=0;

    int       selectedShip=0;
    int       shipUpgradeTier=0;
    int       runCount=0;
    int       bestWave=0;
    PrayerType prayer=PRAYER_FIRE;
    float prayerFxTimer=0;
    float tranceGauge=0,tranceTimer=0,tranceCooldown=0;
    bool  tranceActive=false;
    int   resonanceStreak=0;
    float resonanceTimer=0;
    float chiGauge=0,chiRingTimer=0,heatVentTimer=0;

    // ── Upgrade cache — precomputed booleans, refreshed on purchase ──
    struct UpgradeCache {
        bool rapid=false,afterburn=false,warpcore=false,explosive=false,piercing=false;
        bool trishot=false,scatter=false,ricochet=false,chiflow=false,chiburst=false;
        bool cooldown=false,crit=false,magnet=false,vampire=false,bigbang=false;
        bool goldmag=false,shield=false,overclock=false;
        int  cooldownLv=0, overclockLv=0;
    } ucache;

    // ── Bullet pools — fixed-size, active-flag, no mid-frame allocation ──
    std::array<Bullet, BULLET_POOL_SIZE>      bullets;
    std::array<Bullet, BOSS_BULLET_POOL_SIZE> bossBullets;
    std::array<Particle, PARTICLE_POOL_SIZE>  particles;
    std::array<Pickup, PICKUP_POOL_SIZE>      pickups;
    std::array<PopupText, POPUP_POOL_SIZE>    popups;

    std::vector<Enemy>     enemies;
    std::vector<Star>      stars;
    std::vector<Nebula>    nebulas;
    std::vector<Hazard>    hazards;
    std::vector<Upgrade>   upgrades;
    std::vector<Planet>    planets;
    std::vector<ShootStar> shootStars;
    float shootStarTimer=0;
    Boss boss={};
    std::vector<Upgrade> shopItems;

    SpatialGrid grid;

    // Rover indices for O(1) amortised pool insertion
    int bulletRover     = 0;
    int bossBulletRover = 0;
    int particleRover   = 0;
    int pickupRover     = 0;
    int popupRover      = 0;
};

// Pool helpers — circular rover, O(1) amortised
inline void BulletPoolPush(std::array<Bullet,BULLET_POOL_SIZE>& pool, int& rover, const Bullet& b){
    pool[rover] = b;
    pool[rover].active = true;
    rover = (rover + 1) % BULLET_POOL_SIZE;
}
inline void BossBulletPoolPush(std::array<Bullet,BOSS_BULLET_POOL_SIZE>& pool, int& rover, const Bullet& b){
    pool[rover] = b;
    pool[rover].active = true;
    rover = (rover + 1) % BOSS_BULLET_POOL_SIZE;
}
inline void ParticlePoolPush(std::array<Particle,PARTICLE_POOL_SIZE>& pool, int& rover, const Particle& p){
    pool[rover] = p;
    pool[rover].active = true;
    rover = (rover + 1) % PARTICLE_POOL_SIZE;
}
inline void PickupPoolPush(std::array<Pickup,PICKUP_POOL_SIZE>& pool, int& rover, const Pickup& p){
    pool[rover] = p;
    pool[rover].active = true;
    rover = (rover + 1) % PICKUP_POOL_SIZE;
}
inline void PopupPoolPush(std::array<PopupText,POPUP_POOL_SIZE>& pool, int& rover, const PopupText& p){
    pool[rover] = p;
    pool[rover].active = true;
    rover = (rover + 1) % POPUP_POOL_SIZE;
}

// ══════════════════════════════════════════════════════════════
//  UI STATE  (replaces global statics)
// ══════════════════════════════════════════════════════════════
struct KillEntry { std::string text; Color col; float life; };

struct UIState {
    // Fonts — loaded after InitWindow
    Font fontBig   = {};
    Font fontMono  = {};
    bool fontsLoaded = false;

    // Screen / window
    bool      fullscreen   = false;
    float     scanT        = 0.f;

    // Pause menu
    int       pauseOption  = 0;        // 0=Resume 1=Fullscreen 2=Back/Quit
    GameState prevState    = TITLE;
    float     hoverTime[3] = {};

    // Title / transition flags
    bool forceResetTitle = false;
    bool quitRequested   = false;
    float playerHealthMul = 1.0f;

    // Kill feed
    std::vector<KillEntry> killFeed;
    float killFeedTimer = 0.f;

    // Boss warning
    float       bossWarnTimer   = 0.f;
    std::string bossWarnText;
    bool        bossDeadHandled = false;
};

// ══════════════════════════════════════════════════════════════
//  INPUT STATE  (replaces scattered IsKeyDown calls)
// ══════════════════════════════════════════════════════════════
struct InputState {
    // Player actions
    bool fire        = false;  // mouse held
    bool firePressed = false;  // mouse clicked this frame
    bool dashPressed = false;
    bool up=false, down=false, left=false, right=false;
    bool rightClick  = false;  // prayer cycle
    bool tranceKey   = false;  // SPACE during play
    bool debugSkip   = false;  // Z key

    // Menu actions
    bool pausePressed   = false;  // ESC
    bool confirm        = false;  // ENTER / SPACE in menus
    bool menuUp         = false;
    bool menuDown       = false;
    bool shopAdvance    = false;
    bool kp[10]         = {};     // number keys 1-9 for shop/ship select
    bool quitToDesktop  = false;  // Q from title
    bool f11            = false;  // fullscreen toggle

    // Mouse
    Vector2 mouseScreen = {};    // screen space (scaled into game rect)
    Vector2 mouseWorld  = {};    // world space (camera offset)
    bool    mouseHeld   = false;
    bool    mouseClick  = false;
};

// ══════════════════════════════════════════════════════════════
//  HELPER FUNCTIONS (defined in helpers.cpp / inline here)
// ══════════════════════════════════════════════════════════════
inline float RandF(float a,float b){ return a+(float)rand()/RAND_MAX*(b-a); }
inline int   RandI(int a,int b)    { return a+rand()%(b-a+1); }
inline std::string IStr(int v){ std::ostringstream s; s<<v; return s.str(); }
inline std::string FmtScore(int v){
    std::string s=IStr(v); int n=s.size(),ins=(n-1)/3;
    for(int i=0;i<ins;i++) s.insert(n-3*(i+1)-i,",");
    return s;
}
inline bool HasUpgrade(const Game& g,const std::string& id){
    for(auto& u:g.upgrades) if(u.id==id) return true;
    // also match base-id prefix
    for(auto& u:g.upgrades){
        auto p=u.id.rfind('_');
        if(p!=std::string::npos){
            auto s=u.id.substr(p+1);
            if((s=="2"||s=="3"||s=="4"||s=="5")&&u.id.substr(0,p)==id) return true;
        }
    }
    return false;
}
inline Vector2 W2S(const Game& g,Vector2 w){ return{w.x-g.cameraPos.x+g.camShakeX,w.y-g.cameraPos.y+g.camShakeY}; }
inline bool OnScreen(Vector2 sp,float m=60){ return sp.x>-m&&sp.x<SCREEN_W+m&&sp.y>-m&&sp.y<SCREEN_H+m; }

std::string UpgradeBaseId(const std::string& id);
int OwnedLevel(const Game& g,const std::string& baseId);
void RefreshUpgradeCache(Game& g);

Color LerpC(Color a,Color b,float t);
Color PrayerColor(PrayerType p);
const char* PrayerName(PrayerType p);
PrayerType EnemyWeakness(EnemyType t);

Rectangle GameRect();

// ══════════════════════════════════════════════════════════════
//  INPUT POLLING
// ══════════════════════════════════════════════════════════════
InputState PollInput(const UIState& ui, const Game& g, Rectangle gameRect);

// ══════════════════════════════════════════════════════════════
//  VFX / SPAWN HELPERS
// ══════════════════════════════════════════════════════════════
void SpawnParticles(Game& g,Vector2 pos,Color col,int n=8,float spd=70,bool sparks=false);
void SpawnRing(Game& g,Vector2 pos,Color col,float maxR,float life=.55f);
void SpawnExplosion(Game& g,Vector2 pos,Color col,float radius,int tier=1);
void SpawnPopup(Game& g,Vector2 pos,const std::string& t,Color c,int sz=12);
void SpawnPickup(Game& g,Vector2 pos,int val);
void AddEngineTrail(Game& g);
void TriggerAberration(float dur);
void SpawnSectorRing(Vector2 pos,Color col,float maxR=240,float life=.6f);
void UpdateSectorRings(float dt);

// VFX ambient system updates
void UpdateHyper(float t,float dt,Game& g);
void UpdateLightning(float dt);
void UpdateDust(float dt,Game& g);
void UpdateShootingStars(Game& g,float dt);

// ══════════════════════════════════════════════════════════════
//  DRAW PRIMITIVES (defined in vfx.cpp)
// ══════════════════════════════════════════════════════════════
void DrawNeonLine(Vector2 a,Vector2 b,Color c,float thick=1,float alpha=1);
void DrawNeonCircle(Vector2 c,float r,Color col,float alpha=1);
void DrawNeonFill(Vector2 sp,float r,Color c,float fillA=.12f,float outA=.9f);
void DrawNeonPoly(Vector2* pts,int n,Color c,float thick=1.f,float alpha=1.f);
void DrawGlowBar(int x,int y,int w,int h,float pct,Color col,Color bg);
void DrawScanlines();
void DrawVignette();
void DrawModernCorners(int x,int y,int w,int h,Color c,float t,float intensity=1.f,bool animated=true);
void DrawGlassPanel(int x,int y,int w,int h,Color accent,float t,float alpha=1.f,bool animated=true);
void DrawCard(int x,int y,int w,int h,Color accent,bool hovered=false,float t=0.f,float alpha=1.f);
void DrawModernBar(int x,int y,int w,int h,float pct,Color col,Color bgCol,float t=0.f);
void DrawTextModern(const char* text,int x,int y,int size,Color c,bool centered=false);
void DrawTextGlow(const char* text,int x,int y,int size,Color c,float strength=1.f);
void DrawTextGlowC(const char* text,int y,int size,Color c,float strength=1.f);
void DrawSectionHeader(const char* text,int x,int y,int size,Color c,int w=0);
bool DrawPillButton(const char* text,int x,int y,int w,int h,Color accent,bool selected,float t);
void DrawAmbientParticles(float t,int count=30);
void DrawModernGrid(float t,float alpha=1.f);
void DrawPanel(int x,int y,int w,int h,Color accent,float a=1.f);
void DrawBox(int x,int y,int w,int h,Color border,Color fill);

// Text helpers using default raylib font
void T(const std::string& s,int x,int y,int sz,Color c);
void CT(const std::string& s,int y,int sz,Color c);
void TGlow(const std::string& s,int x,int y,int sz,Color c,float gStrength=1.f);
void CTGlow(const std::string& s,int y,int sz,Color c,float gs=1.f);
// Text helpers using custom fonts
void TF(const std::string& s,int x,int y,int sz,Color c,Font f);
void CTF(const std::string& s,int y,int sz,Color c,Font f);
void TGlowF(const std::string& s,int x,int y,int sz,Color c,float gs,Font f);
void CTGlowF(const std::string& s,int y,int sz,Color c,float gs,Font f);

// ══════════════════════════════════════════════════════════════
//  WORLD RENDERING (defined in world.cpp)
// ══════════════════════════════════════════════════════════════
void DrawEasterEggs(Game& g,float t,float revealAlpha=1.f);
void DrawPlanets(Game& g,float t);
void DrawShootingStars(Game& g);
void DrawAurora(float t,int wave);
void DrawHyper();
void DrawLightning();
void DrawDust();
void DrawAberration(float dt);
void DrawSectorRings(Game& g);
void DrawNebulas(Game& g,float t);
void DrawStars(Game& g,float t);
void DrawWorldGrid(Game& g,float t);
void DrawHazards(Game& g,float t);
void DrawVaporwaveSun(float t,int wave);
void DrawFloatingShapes(float t,int wave);

// ══════════════════════════════════════════════════════════════
//  ENEMY SYSTEM (defined in enemies.cpp)
// ══════════════════════════════════════════════════════════════
void SpawnEnemy(Game& g);
void SpawnBoss(Game& g,int idx);
void HitEnemy(Game& g,Enemy& e,int dmg=1);
void KillEnemy(Game& g,Enemy& e);
void HitBoss(Game& g,int dmg=1);
void PlayerHit(Game& g);
void OpenShop(Game& g);
void DrawEnemy(Enemy& e,Vector2 sp,float t);
void DrawBoss(Boss& b,Vector2 sp,float t);
void PushKillFeed(UIState& ui,const std::string& s,Color c);

// ══════════════════════════════════════════════════════════════
//  SHOOTING (defined in shoot.cpp)
// ══════════════════════════════════════════════════════════════
void Shoot(Game& g,Vector2 mw);

// ══════════════════════════════════════════════════════════════
//  DRAW — bullets / ships / player / pickups (defined in draw.cpp)
// ══════════════════════════════════════════════════════════════
void DrawShipSilhouette(int shipType,Vector2 sc,float a,float t,Color trailCol,bool thrusterOn,bool dashOn,int upgradeTier=0);
void DrawPlayer(Game& g,Vector2 sc,float t);
void DrawBullet(Bullet& b,Vector2 sp,float t);
void DrawPickup(Pickup& pk,Vector2 sp,float t);

// ══════════════════════════════════════════════════════════════
//  HUD (defined in hud.cpp)
// ══════════════════════════════════════════════════════════════
void DrawHUD(Game& g,float t);
void DrawNav(Game& g,float t,float dashCooldown);
void DrawRadar(Game& g,float t);
void DrawBorderPanels(Game& g,UIState& ui,float t,float dt);

// ══════════════════════════════════════════════════════════════
//  MENUS (defined in menus.cpp)
// ══════════════════════════════════════════════════════════════
void DrawTitle(Game& g,UIState& ui,float t,Vector2 msc,bool clicked);
void DrawShop(Game& g,float t);
void DrawGameOver(Game& g,float t);
void DrawPause(Game& g,UIState& ui,float t,Vector2 msc,bool clicked);
static void DrawUpgradeSprite(const Upgrade& u,int cx,int cy,int sz,float t,float alpha);

// ══════════════════════════════════════════════════════════════
//  GAME LOGIC (defined in game.cpp)
// ══════════════════════════════════════════════════════════════
void InitGame(Game& g);
void SpawnWavePlanets(Game& g,int wave);
std::vector<Skill> CreateShipSkills(ShipType ship);
void ApplySkillsToWeapon(const std::vector<Skill>& skills,PlayerWeaponState& w,ShipType s);
int  CalculateBulletDamage(const Bullet& b,const PlayerWeaponState& w);
float GetPlayerHealthMultiplier();
void Enemies_SetUI(UIState* ui);

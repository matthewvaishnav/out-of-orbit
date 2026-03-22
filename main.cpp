// main.cpp — game loop, input, physics, AI, and screen-level draw functions
// Extracted from the original void_blaster.cpp monolith and adapted for split build.
#include "void_blaster.h"
#include <cstring>
#include <cstring>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

// ══════════════════════════════════════════════════════════════
//  GLOBAL STATE
// ══════════════════════════════════════════════════════════════
bool        gFullscreen      = false;
bool        gQuitRequested   = false;
GameState   gPrevState       = TITLE;
int         gPauseOption     = 0;
float       gHoverTime[3]    = {0,0,0};
bool        gForceResetTitle = false;
Font        gFontBig         = {};
Font        gFontMono        = {};
bool        gFontsLoaded     = false;
float       gScanT           = 0.f;
UIState     gUI;

void Enemies_SetUI(UIState*){} // stub — we use gUI directly

// ══════════════════════════════════════════════════════════════
//  SPAWN HELPERS
// ══════════════════════════════════════════════════════════════
void SpawnEnemy(Game& g){
    WaveDef& wd=WAVES[std::min(g.wave,(int)WAVES.size()-1)];
    bool gm=g.ucache.goldmag;
    EnemyType pool[32]; int poolSz=0;
    for(auto t:wd.types){int w=(t==E_GOLDEN&&gm)?4:2;for(int i=0;i<w;i++)if(poolSz<32)pool[poolSz++]=t;}
    EnemyType type=pool[RandI(0,poolSz-1)];
    EnemyDef& def=ENEMY_DEFS[type];
    float ang=RandF(0,2*PI);
    Vector2 pos={g.playerPos.x+cosf(ang)*420,g.playerPos.y+sinf(ang)*420};
    float toA=atan2f(g.playerPos.y-pos.y,g.playerPos.x-pos.x);
    float waveSpeedMult=1.0f+g.wave*0.02f;
    float spd=def.speed*waveSpeedMult;
    Enemy e{}; e.pos=pos; e.vel={cosf(toA+RandF(-.3f,.3f))*spd,sinf(toA+RandF(-.3f,.3f))*spd};
    e.type=type;
    if(g.wave>=4){float rnd=RandF(0,1);if(rnd<0.08f)e.golden=true;else if(rnd<0.15f)e.cursed=true;else if(rnd<0.22f)e.bomb=true;}
    e.hp=e.maxHp=def.hp; e.radius=def.radius; e.scoreVal=def.score;
    e.angle=RandF(0,2*PI); e.spinSpeed=RandF(-.1f,.1f); e.active=true;
    e.golden=e.golden||def.golden; e.cursed=e.cursed||def.cursed; e.bomb=e.bomb||def.bomb; e.chain=def.chain;
    e.col=def.col; e.glowPulse=RandF(0,2*PI); e.variant=RandI(0,2);
    g.enemies.push_back(e);
}
void SpawnBoss(Game& g,int idx){
    BossDef& bd=BOSS_DEFS[std::min(idx,3)];
    float ang=RandF(0,2*PI);
    g.boss.pos={g.playerPos.x+cosf(ang)*500,g.playerPos.y+sinf(ang)*500};
    g.boss.hp=g.boss.maxHp=bd.hp; g.boss.radius=bd.radius; g.boss.scoreVal=bd.score;
    g.boss.active=true; g.boss.col=bd.col; g.boss.name=bd.name; g.boss.sym=bd.sym;
    g.boss.angle=g.boss.angle2=0; g.boss.phase=0;
}

// ══════════════════════════════════════════════════════════════
//  SHOOT
// ══════════════════════════════════════════════════════════════
void Shoot(Game& g,Vector2 mw){
    if(g.heatVentTimer>0) return;
    if(g.shootCooldown>0) return;
    bool rapid=g.ucache.rapid;

    int baseCooldown;
    if(g.tranceActive)             baseCooldown=4;
    else if(g.prayer==PRAYER_FIRE) baseCooldown=rapid?9:18;
    else if(g.prayer==PRAYER_ICE)  baseCooldown=rapid?5:11;
    else                           baseCooldown=rapid?7:14;

    switch(g.selectedShip){
        case 0: baseCooldown=std::max(3,baseCooldown-3); break;
        case 1: baseCooldown=(int)(baseCooldown*1.55f);  break;
        case 2: baseCooldown=baseCooldown+1;             break;
        case 3: baseCooldown=(int)(baseCooldown*1.80f);  break;
    }

    bool tier1=g.chiGauge>=40.f, tier2=g.chiGauge>=70.f, tier3=g.chiGauge>=88.f;
    if(!g.tranceActive&&tier3&&g.ucache.chiflow) baseCooldown=std::max(2,(int)(baseCooldown*0.62f));
    else if(!g.tranceActive&&tier2) baseCooldown=std::max(3,(int)(baseCooldown*0.75f));
    else if(!g.tranceActive&&tier1&&g.ucache.chiflow) baseCooldown=std::max(3,(int)(baseCooldown*0.82f));
    g.shootCooldown=baseCooldown;

    if(!g.tranceActive){
        float heatPerShot=6.5f;
        if(g.ucache.cooldown) heatPerShot*=(0.60f-(g.ucache.cooldownLv-1)*0.08f);
        if(g.prayer==PRAYER_FIRE) heatPerShot*=1.30f;
        if(g.prayer==PRAYER_ICE)  heatPerShot*=0.80f;
        if(g.selectedShip==3) heatPerShot*=1.40f;
        if(g.selectedShip==0) heatPerShot*=0.78f;
        g.chiGauge=std::min(100.f,g.chiGauge+heatPerShot);
        if(g.chiGauge>=100.f){
            g.chiGauge=100.f;
            float ventDur=1.8f;
            if(g.ucache.cooldown) ventDur*=std::max(0.35f,1.f-g.ucache.cooldownLv*0.12f);
            g.heatVentTimer=ventDur; g.chiRingTimer=ventDur;
            SpawnRing(g,g.playerPos,{255,80,0,255},180,.50f);
            SpawnParticles(g,g.playerPos,{255,120,0,255},6,90,true);
            SpawnPopup(g,{g.playerPos.x,g.playerPos.y-60},"OVERHEATED!",{255,80,0,255},14);
            TriggerAberration(.25f); g.screenShake=std::min(g.screenShake+.12f,.3f);
        }
    }

    float dx=mw.x-g.playerPos.x, dy=mw.y-g.playerPos.y;
    float d=sqrtf(dx*dx+dy*dy); if(d<.01f)return;
    float nx=dx/d, ny=dy/d;
    float px2=-ny, py2=nx;

    bool expl=g.ucache.explosive, pier=g.ucache.piercing;
    bool tri=g.ucache.trishot,   sct=g.ucache.scatter;

    auto MkBullet=[&](float ox, float oy, float ao, float spd, int life,
                      Color col, float glowR, int ptype, int dmg, bool hom=false) -> Bullet {
        float ca=cosf(ao),sa=sinf(ao);
        float bx=ca*nx-sa*ny, by=sa*nx+ca*ny;
        Bullet b{};
        b.pos   = {g.playerPos.x + nx*18 + ox, g.playerPos.y + ny*18 + oy};
        b.vel   = {bx*spd, by*spd};
        b.life  = life; b.burstTimer=(float)life;
        b.active=true;  b.col=col;   b.glowR=glowR;
        b.prayerType=ptype; b.damage=dmg;
        b.shipType=g.selectedShip;
        b.explosive=expl; b.piercing=pier;
        b.homing=hom;     b.homingStr=hom?3.2f:0.f;
        // ── Skill modifications ──
        // Interceptor MOMENTUM: speed converts to bonus damage
        if(g.ucache.sk_momentum && g.selectedShip==0)
            b.damage = std::max(1, (int)(b.damage * g.momentumDmgMult));
        // Interceptor RICOCHET: bullets bounce
        if(g.ucache.sk_ricochet && g.selectedShip==0)
            b.ricochet=true;
        // Phantom PHASE: pierce up to 3 enemies
        if(g.ucache.sk_phase && g.selectedShip==2)
            b.piercing=true;
        // Phantom SNIPER: first bullet of each burst hits for 4x
        if(g.ucache.sk_sniper && g.selectedShip==2 && g.sniperReady){
            b.damage*=4; b.glowR*=2.f; g.sniperReady=false;
        }
        // Brawler/Titan OVERLOAD/CHARGE: damage multiplier from kill streak
        if((g.ucache.sk_charge && g.selectedShip==1)||(g.ucache.sk_overload && g.selectedShip==3))
            b.damage = std::max(1, (int)(b.damage * g.overloadMult));
        return b;
    };

    if(g.tranceActive){
        int idx=(int)(GetTime()*6)%3;
        Color bcol=(idx==0)?VP_PINK:(idx==1)?VP_PURPLE:VP_CYAN;
        float angs[7]; int nAngs=0;
        if(tri){angs[nAngs++]=-.44f;angs[nAngs++]=-.22f;angs[nAngs++]=0;angs[nAngs++]=.22f;angs[nAngs++]=.44f;}
        else   {angs[nAngs++]=-.22f;angs[nAngs++]=0;angs[nAngs++]=.22f;}
        if(sct){angs[nAngs++]=RandF(-.5f,.5f);angs[nAngs++]=RandF(-.5f,.5f);}
        int dmg=(g.selectedShip==3)?2:1;
        for(int ai=0;ai<nAngs;ai++){
            auto b=MkBullet(0,0,angs[ai],320,95,bcol,9,3,dmg);
            BulletPoolPush(g.bullets,g.bulletRover,b);
        }
        SpawnParticles(g,{g.playerPos.x+nx*18,g.playerPos.y+ny*18},VP_PINK,4,90,true);
        return;
    }

    if(expl){
        auto b=MkBullet(0,0,0,310,95,VP_RED,8,-1,(g.selectedShip==3)?2:1);
        BulletPoolPush(g.bullets,g.bulletRover,b);
        SpawnParticles(g,b.pos,VP_RED,4,90,true); return;
    }
    if(pier){
        auto b=MkBullet(0,0,0,310,95,VP_MINT,5,-1,(g.selectedShip==3)?2:1);
        b.piercing=true;
        BulletPoolPush(g.bullets,g.bulletRover,b);
        SpawnParticles(g,b.pos,VP_MINT,4,90,true); return;
    }

    // ── Prayer defines identity; ship modifies it ───────────────
    // FIRE  : slow heavy slugs — burn stacks on hit, chain ignition on kill
    // ICE   : fast precise darts — freeze on hit, shatter bonus + ice shards on kill
    // SPIRIT: weak solo but chains to 3 enemies (+50% dmg/hop), ghost orb after 8 chain kills

    float baseSpd, baseGlow;
    int   baseLife, baseDmg, basePtype;
    Color baseCol;
    bool  baseHom   = false;
    int   chainHops = 0;

    switch(g.prayer){
        case PRAYER_FIRE:
            baseSpd   = 145.f;  // slow — threat is the burn, not the bullet
            baseLife  = 110;
            baseDmg   = 6;
            baseCol   = VP_ORANGE;
            baseGlow  = 12.f;
            basePtype = 0;
            break;
        case PRAYER_ICE:
            baseSpd   = 540.f;  // fastest — precision weapon
            baseLife  = 55;
            baseDmg   = 3;      // low base; shatter hit does 2x
            baseCol   = VP_CYAN;
            baseGlow  = 5.f;
            basePtype = 1;
            break;
        case PRAYER_SPIRIT:
        default:
            baseSpd   = 260.f;
            baseLife  = 85;
            baseDmg   = 1;      // intentionally weak solo
            baseCol   = VP_PURPLE;
            baseGlow  = 6.f;
            basePtype = 2;
            baseHom   = true;
            chainHops = 3;      // jumps to up to 3 enemies
            break;
    }

    // Ship modifiers on top of prayer
    float spdMult  = 1.f;
    float dmgMult  = 1.f;
    int   extraShots = 0;

    switch(g.selectedShip){
        case 0: spdMult = 1.15f; baseCooldown=std::max(3,baseCooldown-3); break;
        case 1: dmgMult = 1.6f;  baseCooldown=(int)(baseCooldown*1.55f);  break;
        case 2: spdMult = 1.2f;  baseCooldown=baseCooldown+1;             break;
        case 3: dmgMult = 1.8f;  extraShots=1; baseCooldown=(int)(baseCooldown*1.80f); break;
    }

    float finalSpd = baseSpd * spdMult;
    int   finalDmg = std::max(1,(int)(baseDmg * dmgMult));

    // Total shots: base 1, +1 if trishot, +extraShots for Titan
    int totalShots = 1 + (tri ? 1 : 0) + extraShots;

    static int iBarrel = 0;
    for(int si=0; si<totalShots; si++){
        float ao = 0.f;
        if(totalShots==2) ao = (si==0 ? -0.10f : 0.10f);
        if(totalShots==3) ao = (si==0 ? -0.14f : si==1 ? 0.14f : 0.f);

        // Interceptor and Brawler alternate barrels for visual spread
        float bSide = 0.f;
        if(g.selectedShip==0 || g.selectedShip==1){
            bSide = ((iBarrel+si) % 2 == 0) ? 1.f : -1.f;
        } else if(g.selectedShip==2){
            // Phantom fires from wing tips
            bSide = (si % 2 == 0) ? 1.f : -1.f;
        }
        float barrelW = (g.selectedShip==2) ? 14.f : 6.f;
        float ox = px2 * barrelW * bSide;
        float oy = py2 * barrelW * bSide;

        auto b = MkBullet(ox, oy, ao, finalSpd, baseLife,
                          baseCol, baseGlow, basePtype, finalDmg, baseHom);
        b.chainJumps   = chainHops;
        b.chainDmgMult = 1.f;
        BulletPoolPush(g.bullets, g.bulletRover, b);
    }
    iBarrel++;

    // Scatter shots — inaccurate, reduced damage
    if(sct){
        int nSct = (g.selectedShip==3) ? 3 : 2;
        for(int si=0; si<nSct; si++){
            auto bs = MkBullet(px2*8.f*(si%2==0?1.f:-1.f), py2*8.f*(si%2==0?1.f:-1.f),
                               RandF(-0.42f,0.42f), finalSpd*0.72f, baseLife-12,
                               baseCol, baseGlow*0.6f, basePtype, 1);
            BulletPoolPush(g.bullets, g.bulletRover, bs);
        }
    }

    // Muzzle flash — heavier for heavier ships
    int partCount = (g.selectedShip==3) ? 10 : (g.selectedShip==1) ? 7 : 3;
    SpawnParticles(g, {g.playerPos.x+nx*18, g.playerPos.y+ny*18},
                   baseCol, partCount, finalSpd*0.35f, true);
    if(g.selectedShip==3){
        SpawnRing(g,{g.playerPos.x+nx*20,g.playerPos.y+ny*20},baseCol,28,.25f);
        g.screenShake=std::min(g.screenShake+0.06f,0.40f);
    }
    if(g.selectedShip==1)
        g.screenShake=std::min(g.screenShake+0.03f,0.30f);
}

// ══════════════════════════════════════════════════════════════
//  HIT / KILL LOGIC
// ══════════════════════════════════════════════════════════════
void KillEnemy(Game& g,Enemy& e);
void HitEnemy(Game& g,Enemy& e,int dmg){
    bool crit=(RandF(0,1)<(g.ucache.crit?.3f:.1f));
    PrayerType weakness=EnemyWeakness(e.type);
    bool weak=(weakness==g.prayer);
    bool resist=(g.prayer!=weakness&&!g.tranceActive);
    int d=dmg;
    if(resist)    d=std::max(1,(int)(dmg*.5f));
    else if(weak) d=dmg*3;
    if(crit)      d=(int)(d*2.5f);
    if(g.chiGauge>=88.f&&!g.tranceActive)           d=(int)(d*1.35f);
    else if(g.chiGauge>=70.f&&!g.tranceActive)      d=(int)(d*1.15f);
    if(g.chiGauge>=70.f&&g.ucache.chiburst)         d=(int)(d*1.25f);

    // ── PRAYER ON-HIT EFFECTS ────────────────────────────────────
    if(g.prayer==PRAYER_FIRE){
        // Add burn stack (max 5). Each stack = 3 dmg/sec for 2 seconds.
        if(e.burnStacks<5) e.burnStacks++;
        e.burnTimer=2.0f;
        SpawnParticles(g,e.pos,VP_ORANGE,2,55.f,true);
        if(e.burnStacks==5)
            SpawnPopup(g,{e.pos.x,e.pos.y-28},"MAX BURN!",VP_ORANGE,11);
    }
    if(g.prayer==PRAYER_ICE){
        if(e.frozenTimer>0.f && e.frozenAmp){
            // Already frozen — SHATTER: 2x damage + ice shard spray
            d*=2;
            e.frozenAmp=false;
            SpawnRing(g,e.pos,VP_CYAN,42.f,0.35f);
            SpawnPopup(g,{e.pos.x,e.pos.y-28},"SHATTER x2!",VP_CYAN,12);
            for(int i=0;i<5;i++){
                float a2=RandF(0.f,2.f*PI);
                Bullet shard{};
                shard.pos={e.pos.x,e.pos.y};
                shard.vel={cosf(a2)*180.f,sinf(a2)*180.f};
                shard.life=28; shard.active=true;
                shard.col=VP_CYAN; shard.glowR=3.f;
                shard.prayerType=1; shard.damage=1;
                shard.shipType=g.selectedShip;
                BulletPoolPush(g.bullets,g.bulletRover,shard);
            }
        } else {
            // First hit: freeze
            e.frozenTimer=1.4f;
            e.frozenMult=0.35f;
            e.frozenAmp=true;
            SpawnParticles(g,e.pos,VP_CYAN,4,45.f,false);
            SpawnPopup(g,{e.pos.x,e.pos.y-24},"FROZEN",VP_CYAN,10);
        }
    }
    if(g.prayer==PRAYER_SPIRIT){
        g.tranceGauge=std::min(100.f,g.tranceGauge+8.f);
    }

    // ── STANDARD HIT PROCESSING ──────────────────────────────────
    e.hp-=d; e.flashTimer=10;
    if(resist&&!crit){SpawnPopup(g,{e.pos.x,e.pos.y-24},"RESIST",VP_GRAY,11);SpawnParticles(g,e.pos,VP_DARKGRAY,3,35);}
    else if(weak){
        g.tranceGauge=std::min(100.f,g.tranceGauge+12.f); g.resonanceStreak++; g.resonanceTimer=1.1f;
        if(g.prayer==PRAYER_FIRE){SpawnRing(g,e.pos,VP_RED,55,.4f);SpawnParticles(g,e.pos,VP_ORANGE,8,90,false);SpawnParticles(g,e.pos,VP_RED,4,60,true);SpawnPopup(g,{e.pos.x,e.pos.y-30},"NOVA BLAST! x3",VP_ORANGE,14);}
        else if(g.prayer==PRAYER_ICE){SpawnRing(g,e.pos,VP_CYAN,60,.45f);SpawnRing(g,e.pos,VP_LAVENDER,40,.3f);SpawnParticles(g,e.pos,VP_CYAN,8,70,true);SpawnPopup(g,{e.pos.x,e.pos.y-30},"PIERCED! x3",VP_CYAN,14);}
        else{SpawnRing(g,e.pos,VP_PURPLE,65,.45f);SpawnParticles(g,e.pos,VP_PURPLE,6,80,true);SpawnParticles(g,e.pos,VP_LAVENDER,4,50);SpawnPopup(g,{e.pos.x,e.pos.y-30},"HAUNTED! x3",VP_PURPLE,14);}
        if(g.resonanceStreak>=3){SpawnPopup(g,{e.pos.x,e.pos.y-50},"RESONANCE!",VP_PINK,16);SpawnRing(g,e.pos,VP_PINK,85,.5f);g.tranceGauge=std::min(100.f,g.tranceGauge+10.f);}
        SpawnParticles(g,e.pos,PrayerColor(g.prayer),5,75,true);
    } else {SpawnParticles(g,e.pos,e.col,4,55);}
    if(crit){std::string ct=weak?"*KAMI CRIT*":"CRIT! x2.5";Color cc=weak?VP_PINK:VP_YELLOW;SpawnPopup(g,{e.pos.x,e.pos.y-24+(weak?-12:0)},ct,cc,15);g.totalCrits++;g.biggestCrit=std::max(g.biggestCrit,(int)(e.scoreVal*g.combo));g.critFlash=10;SpawnParticles(g,e.pos,VP_YELLOW,8,90,true);g.tranceGauge=std::min(100.f,g.tranceGauge+5.f);}
    Color dmgCol=resist?VP_GRAY:weak?PrayerColor(g.prayer):VP_WHITE;
    SpawnPopup(g,e.pos,"-"+IStr(d),dmgCol,10);
    if(e.hp<=0) KillEnemy(g,e);
}
void KillEnemy(Game& g,Enemy& e){
    e.active=false;

    // ── PRAYER ON-KILL EFFECTS ───────────────────────────────────
    // FIRE: burning kill ignites nearby enemies
    if(g.prayer==PRAYER_FIRE && e.burnStacks>0){
        for(auto& en:g.enemies){
            if(!en.active||&en==&e) continue;
            float dx=en.pos.x-e.pos.x, dy=en.pos.y-e.pos.y;
            if(dx*dx+dy*dy<130.f*130.f){
                en.burnStacks=std::min(5,en.burnStacks+2);
                en.burnTimer=2.0f;
                SpawnParticles(g,en.pos,VP_ORANGE,3,50.f,true);
            }
        }
        SpawnRing(g,e.pos,VP_ORANGE,80.f,0.4f);
    }
    // ICE: frozen kill shatters — 6 ice shards spray outward
    if(g.prayer==PRAYER_ICE && e.frozenTimer>0.f){
        for(int i=0;i<6;i++){
            float a2=(float)i/6.f*2.f*PI;
            Bullet shard{};
            shard.pos={e.pos.x,e.pos.y};
            shard.vel={cosf(a2)*160.f,sinf(a2)*160.f};
            shard.life=35; shard.active=true;
            shard.col=VP_CYAN; shard.glowR=3.5f;
            shard.prayerType=1; shard.damage=2;
            shard.shipType=g.selectedShip;
            BulletPoolPush(g.bullets,g.bulletRover,shard);
        }
        SpawnRing(g,e.pos,VP_CYAN,65.f,0.4f);
        SpawnPopup(g,{e.pos.x,e.pos.y-28},"ICE SHATTER!",VP_CYAN,11);
    }
    // SPIRIT: chain kill counter — 8 chain kills summons ghost orb
    if(g.prayer==PRAYER_SPIRIT){
        g.spiritChainKills++;
        g.tranceGauge=std::min(100.f,g.tranceGauge+4.f);
        if(g.spiritChainKills>=8 && !g.ghostOrbActive){
            g.ghostOrbActive=true;
            g.ghostOrbTimer=0.9f;
            g.spiritChainKills=0;
            SpawnPopup(g,{g.playerPos.x,g.playerPos.y-70},"GHOST ORB SUMMONED",VP_PURPLE,14);
            SpawnRing(g,g.playerPos,VP_PURPLE,60.f,0.5f);
        }
    }
    // Brawler CHARGE / Titan OVERLOAD: kill streak builds damage multiplier (caps at 3x)
    if((g.ucache.sk_charge&&g.selectedShip==1)||(g.ucache.sk_overload&&g.selectedShip==3)){
        g.overloadKills++;
        g.overloadMult=1.f+std::min(2.f, g.overloadKills*0.05f);
        if(g.overloadKills%10==0){
            char buf[32]; snprintf(buf,sizeof(buf),"CHARGE x%.1f",g.overloadMult);
            SpawnPopup(g,{g.playerPos.x,g.playerPos.y-65},buf,VP_GOLD,11);
        }
    }

    PushKillFeed(gUI,std::string(ENEMY_DEFS[e.type].sym)+" destroyed",e.col);
    g.waveKills++; g.totalKills++; g.killCount++;
    g.combo=std::min(20,g.combo+1); g.comboTimer=g.comboMax;
    int sg=(int)(std::max(0,e.scoreVal)*g.combo); g.score+=sg; if(g.score>g.highScore)g.highScore=g.score;
    SpawnPopup(g,{e.pos.x,e.pos.y-20},"+"+IStr(sg),e.col,13);
    int tier=(e.type==E_TANK||e.type==E_BOMB||e.type==E_CRYSTAL||e.type==E_VOID)?3:(e.type==E_GOLDEN||e.chain)?2:1;
    SpawnExplosion(g,e.pos,e.col,e.radius,tier);
    if(e.golden)SpawnExplosion(g,e.pos,VP_GOLD,e.radius*1.5f,3);
    SpawnPickup(g,e.pos,std::max(0,e.scoreVal));
    g.tranceGauge=std::min(100.f,g.tranceGauge+2.f);
    float heatRelease=3.5f+g.wave*0.15f;
    if(e.type==E_TANK||e.type==E_BOMB)        heatRelease*=2.2f;
    if(e.type==E_GOLDEN||e.type==E_CRYSTAL)   heatRelease*=2.8f;
    if(e.type==E_VOID)                        heatRelease*=4.0f;
    if(g.ucache.overclock) heatRelease*=(1.f+g.ucache.overclockLv*0.35f);
    g.chiGauge=std::max(0.f,g.chiGauge-heatRelease);
    if(g.heatVentTimer>0&&g.chiGauge<50.f){g.heatVentTimer=0;g.chiRingTimer=0;}
    if(g.waveKills>=g.waveTarget&&!g.bossPhase){SpawnPopup(g,{g.playerPos.x,g.playerPos.y-90},"WAVE CLEAR!",VP_MINT,18);SpawnRing(g,g.playerPos,VP_MINT,180,.7f);SpawnRing(g,g.playerPos,VP_CYAN,120,.5f);SpawnParticles(g,g.playerPos,VP_MINT,12,120,true);}
    if(g.combo==5)  SpawnPopup(g,{g.playerPos.x,g.playerPos.y-65},"LOCKED ON",VP_CYAN,14);
    if(g.combo==8)  SpawnPopup(g,{g.playerPos.x,g.playerPos.y-65},"SURGE!!",VP_PINK,16);
    if(g.combo==12) SpawnPopup(g,{g.playerPos.x,g.playerPos.y-65},"OVERDRIVE!",VP_YELLOW,17);
    if(g.combo==16) SpawnPopup(g,{g.playerPos.x,g.playerPos.y-65},"VOID RESONANCE!",VP_PURPLE,14);
    if(g.combo==20){SpawnPopup(g,{g.playerPos.x,g.playerPos.y-80},"★ VOID MASTERY ★",VP_GOLD,20);SpawnRing(g,g.playerPos,VP_GOLD,200,.8f);SpawnRing(g,g.playerPos,VP_YELLOW,140,.5f);SpawnParticles(g,g.playerPos,VP_GOLD,18,140,true);g.screenShake=std::min(g.screenShake+.2f,.5f);}
    g.progressJuice=0.18f; g.lastKillFlash=0.22f;
    if(e.golden){for(int ci=0;ci<10;ci++){float ca=RandF(0,2*PI),cr=RandF(30,110);SpawnPopup(g,{e.pos.x+cosf(ca)*cr*0.3f,e.pos.y+sinf(ca)*cr*0.3f},"*",VP_GOLD,8);}}
    if(g.ucache.vampire&&g.killCount%10==0&&g.lives<5){g.lives++;SpawnPopup(g,{g.playerPos.x,g.playerPos.y-70},"<+1 HEART>",VP_PINK,15);}
    if(g.ucache.bigbang){g.novaCount++;if(g.novaCount>=50){g.novaCount=0;for(auto& en:g.enemies)if(en.active)en.hp=0;SpawnPopup(g,{g.playerPos.x,g.playerPos.y-80},"*** SHRINE NOVA ***",VP_PINK,20);SpawnRing(g,g.playerPos,VP_PINK,600,1.2f);SpawnRing(g,g.playerPos,VP_PURPLE,400,.9f);g.novaFlash=.6f;}}
    if(e.bomb){SpawnPopup(g,{e.pos.x,e.pos.y-36},"** BLAZE BURST **",VP_RED,15);SpawnRing(g,e.pos,VP_RED,120);SpawnParticles(g,e.pos,VP_RED,20,130,true);g.screenShake=.45f;for(auto& en:g.enemies){if(&en==&e||!en.active)continue;float dx=en.pos.x-e.pos.x,dy=en.pos.y-e.pos.y;if(dx*dx+dy*dy<14400)HitEnemy(g,en,2);}}
    if(e.chain&&RandF(0,1)<.65f){SpawnPopup(g,{e.pos.x,e.pos.y-36},"CHAIN!",VP_CYAN,14);SpawnRing(g,e.pos,VP_CYAN,140,.4f);for(auto& en:g.enemies){if(&en==&e||!en.active)continue;float dx=en.pos.x-e.pos.x,dy=en.pos.y-e.pos.y;if(dx*dx+dy*dy<19600)en.hp=0;}}
    if(e.cursed){g.score=std::max(0,g.score+e.scoreVal);g.combo=1;g.comboTimer=0;SpawnPopup(g,{e.pos.x,e.pos.y-36},"CURSED!",VP_PURPLE,15);SpawnRing(g,e.pos,VP_PURPLE,80);}
    if(e.golden)SpawnPopup(g,{e.pos.x,e.pos.y-24},"* GOLDEN SPIRIT * +"+IStr(sg),VP_YELLOW,14);
}
void HitBoss(Game& g,int dmg){
    if(!g.boss.active)return;
    bool crit=(RandF(0,1)<(g.ucache.crit?.3f:.1f));
    int d=crit?dmg*3:dmg; g.boss.hp-=d; g.boss.flashTimer=10;
    g.combo=std::min(20,g.combo+1); g.comboTimer=g.comboMax;
    if(crit){SpawnPopup(g,{g.boss.pos.x,g.boss.pos.y-48},"CRIT! x3",VP_YELLOW,16);g.critFlash=10;g.tranceGauge=std::min(100.f,g.tranceGauge+8.f);}
    else SpawnPopup(g,{g.boss.pos.x,g.boss.pos.y-24},"-"+IStr(d),VP_WHITE,12);
    SpawnParticles(g,g.boss.pos,g.boss.col,7,85);
    if(g.boss.phase==1)g.screenShake=std::min(g.screenShake+0.04f,0.4f);
    if((float)g.boss.hp/g.boss.maxHp<.4f&&g.boss.phase==0){g.boss.phase=1;SpawnPopup(g,{g.boss.pos.x,g.boss.pos.y-70},"** ENRAGED **",VP_RED,18);SpawnRing(g,g.boss.pos,VP_RED,150);g.screenShake=.6f;}
    if(g.boss.hp<=0){
        int sc=g.boss.scoreVal*g.combo; g.score+=sc; if(g.score>g.highScore)g.highScore=g.score;
        PushKillFeed(gUI,g.boss.name+" SEALED",VP_YELLOW);
        SpawnPopup(g,{g.boss.pos.x,g.boss.pos.y-60},"SPIRIT SEALED! +"+IStr(sc),VP_YELLOW,20);
        SpawnParticles(g,g.boss.pos,g.boss.col,40,180,true);
        for(int i=0;i<4;i++)SpawnRing(g,g.boss.pos,g.boss.col,60+i*50.f,.6f+i*.1f);
        for(int i=0;i<8;i++)SpawnPickup(g,{g.boss.pos.x+RandF(-80,80),g.boss.pos.y+RandF(-80,80)},g.boss.scoreVal/8);
        g.boss.active=false; g.screenShake=.8f; g.novaFlash=.4f;
        g.tranceGauge=std::min(100.f,g.tranceGauge+30.f);
    }
}
void PlayerHit(Game& g){
    if(g.invincTimer>0||g.tranceActive) return;
    // Phantom CLOAK: if cloaked, absorb the hit invisibly
    if(g.cloakActive){ g.cloakActive=false; g.cloakTimer=0.f; return; }
    if(g.shieldHp>0){g.shieldHp--;SpawnPopup(g,{g.playerPos.x,g.playerPos.y-60},"OFUDA WARD!",VP_CYAN,18);SpawnRing(g,g.playerPos,VP_CYAN,55);g.invincTimer=1.5f;return;}
    g.lives--; g.combo=1; g.comboTimer=0; g.screenShake=.55f; g.invincTimer=2; TriggerAberration(.35f);
    g.tranceGauge=std::max(0.f,g.tranceGauge-20.f);
    // Brawler CHARGE / Titan OVERLOAD: reset kill streak on hit
    g.overloadMult=1.f; g.overloadKills=0;
    SpawnParticles(g,g.playerPos,VP_PINK,24,95,true); SpawnRing(g,g.playerPos,VP_RED,80);
}
void OpenShop(Game& g){
    g.state=SHOP; g.shopItems.clear();

    // Determine which skill category belongs to this ship
    std::string mySkillCat = "skill_" + std::to_string(g.selectedShip);

    // Mutual exclusion: Interceptor can't have both ricochet AND momentum
    // Phantom can't have both sniper AND phase
    // Brawler can't have both shockwave AND charge
    // Titan can't have both gravity AND overload
    // Each pair is Path A vs Path B — owning one locks out the other
    auto isLockedOut = [&](const std::string& id) -> bool {
        if(g.selectedShip==0){
            if(id=="sk0_ricochet" && HasUpgrade(g,"sk0_momentum")) return true;
            if(id=="sk0_momentum" && HasUpgrade(g,"sk0_ricochet")) return true;
        }
        if(g.selectedShip==1){
            if(id=="sk1_shockwave" && HasUpgrade(g,"sk1_charge")) return true;
            if(id=="sk1_charge"    && HasUpgrade(g,"sk1_shockwave")) return true;
        }
        if(g.selectedShip==2){
            if(id=="sk2_sniper" && HasUpgrade(g,"sk2_phase")) return true;
            if(id=="sk2_phase"  && HasUpgrade(g,"sk2_sniper")) return true;
        }
        if(g.selectedShip==3){
            if(id=="sk3_gravity"  && HasUpgrade(g,"sk3_overload")) return true;
            if(id=="sk3_overload" && HasUpgrade(g,"sk3_gravity"))  return true;
        }
        return false;
    };

    std::vector<Upgrade> newTrees, upgrades;
    for(auto& u : ALL_UPGRADES){
        // Skip skills that belong to a different ship
        if(u.category.substr(0,5)=="skill" && u.category!=mySkillCat) continue;
        // Skip mutually exclusive locked-out skills
        if(isLockedOut(u.id)) continue;

        std::string base = UpgradeBaseId(u.id);
        int owned = OwnedLevel(g, base);
        if(owned==0 && u.level==1)       newTrees.push_back(u);
        else if(owned>0 && u.level==owned+1) upgrades.push_back(u);
    }
    for(int i=(int)newTrees.size()-1;i>0;i--) std::swap(newTrees[i],newTrees[RandI(0,i)]);
    for(int i=(int)upgrades.size()-1;i>0;i--) std::swap(upgrades[i],upgrades[RandI(0,i)]);
    int total=0;
    for(auto& u:upgrades){if(total>=6)break;g.shopItems.push_back(u);total++;}
    for(auto& u:newTrees){if(total>=6)break;g.shopItems.push_back(u);total++;}
}


// ══════════════════════════════════════════════════════════════
//  SCREEN DRAW FUNCTIONS
// ══════════════════════════════════════════════════════════════

void DrawTitle(Game& g,UIState& /*ui*/,float t,Vector2 msc,bool clicked){
    // ══════════════════════════════════════════════════════════════
    //  CINEMATIC TITLE SCREEN  — full theatrical intro sequence
    //  Phases:
    //   0.00–0.55s : absolute void — one blinking cursor
    //   0.55–1.60s : STUDIO CARD  — "DIGITAL VOID SYSTEMS" corp logo fades in/out
    //   1.60–2.80s : HYPERSPACE   — white blast, radial starlines contract inward
    //   2.80–4.00s : WORLD RISE   — sun climbs from below, grid materialises, stars settle
    //   4.00–5.60s : LOGO SLAM    — letters crash in with shockwave rings + screen-shake
    //   5.60–6.30s : SUBTITLE     — typewriter reveal, cursor blinks
    //   6.30–7.00s : FRAME IN     — corner brackets and divider sweep in
    //   7.00–7.80s : CARDS SLIDE  — ship selection panels glide up
    //   7.80s+     : IDLE LOOP
    // ══════════════════════════════════════════════════════════════
    static float introT = 0.f;
    static bool  firstFrame = true;
    if(firstFrame || gForceResetTitle){ introT = 0.f; firstFrame = false; gForceResetTitle = false; }
    introT += GetFrameTime();

    // Skip on any input — jump straight to idle (past all animated phases)
    auto AnyKey=[](){ for(int k=32;k<350;k++) if(IsKeyPressed(k)) return true; return false; };
    if(introT < 7.80f && (clicked || AnyKey())) introT = 7.80f;

    auto Ease   = [](float x)->float{ return x<.5f?2*x*x:1-2*(1-x)*(1-x); };
    auto EaseO  = [](float x)->float{ float t=1-x; return 1-t*t*t; };
    auto Clamp01= [](float v)->float{ return v<0?0:v>1?1:v; };
    auto Prog   = [&](float start, float dur)->float{ return Clamp01((introT-start)/dur); };

    // ── Global progress values ──────────────────────────────────
    float bgFade    = EaseO(Prog(2.80f, 0.80f));
    float logoFade  = Prog(4.00f, 1.60f);
    float subFade   = Ease(Prog(5.60f, 0.60f));
    float frameFade = Ease(Prog(6.30f, 0.65f));
    float cardFade  = Ease(Prog(7.00f, 0.70f));

    // ── Per-letter slam state ───────────────────────────────────
    static float shakeX[12]      = {};
    static float shakeY[12]      = {};
    static float letterLanded[12]= {};
    static float ringAge[12]     = {};

    // Hyperspace streaks
    static struct HStreak{ float angle, baseLen; } hStreaks[80];
    static bool hInit = false;
    if(!hInit){
        hInit = true;
        for(int i=0;i<80;i++){
            hStreaks[i].angle   = (float)i / 80.f * 2.f * PI;
            hStreaks[i].baseLen = 40.f + (float)(rand()%120);
        }
    }

    // Title-screen star field
    static struct TStar{float x,y,b,tw,to,r;Color tint;int layer;} tstars[120];
    static bool tstarInit=false;
    if(!tstarInit){
        tstarInit=true;
        Color tt[]={VP_WHITE,VP_PINK,VP_CYAN,VP_LAVENDER};
        for(int i=0;i<120;i++){
            tstars[i]={(float)(rand()%SCREEN_W),(float)(rand()%SCREEN_H),
                0.2f+0.8f*(rand()%100/100.f), 0.5f+3.5f*(rand()%100/100.f),
                (rand()%628)/100.f, 0.4f+1.4f*(rand()%100/100.f),
                tt[rand()%4], rand()%3};
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // PHASE 1 — VOID  (0.00 – 0.55s)
    // ═══════════════════════════════════════════════════════════════
    DrawRectangle(0,0,SCREEN_W,SCREEN_H,VP_BLACK);

    if(introT < 0.55f){
        if((int)(introT * 4.f) % 2 == 0){
            float cx0=(float)SCREEN_W*.5f, cy0=(float)SCREEN_H*.5f;
            DrawRectangle((int)(cx0-4),(int)(cy0-6),8,12,ColorAlpha(VP_WHITE,.55f));
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // PHASE 2 — STUDIO CARD  (0.55 – 1.60s)
    // ═══════════════════════════════════════════════════════════════
    if(introT >= 0.55f && introT < 1.60f){
        float scp = (introT - 0.55f) / 1.05f;
        float scA = scp < 0.25f ? scp/0.25f : scp > 0.75f ? (1.f-(scp-0.75f)/0.25f) : 1.f;
        scA = Clamp01(scA);

        float scx = (float)SCREEN_W * .5f;
        float scy = (float)SCREEN_H * .5f;

        float dmSize = 20.f * scA;
        Color dmCol  = ColorAlpha({220,180,255,255}, scA * .90f);
        DrawRectanglePro({scx, scy - 48.f, dmSize, dmSize}, {dmSize*.5f, dmSize*.5f}, 45.f, dmCol);
        DrawRectanglePro({scx, scy - 48.f, dmSize*.6f, dmSize*.6f}, {dmSize*.3f, dmSize*.3f}, 45.f,
            ColorAlpha({255,255,255,255}, scA * .30f));

        const char* dvs = "DIGITAL VOID SYSTEMS";
        int dvw = MeasureText(dvs, 10);
        DrawText(dvs, (int)(scx - dvw*.5f), (int)(scy - 30.f), 10,
            ColorAlpha({200,160,255,255}, scA * .85f));

        float lw2 = 120.f * scA;
        DrawLineEx({scx - lw2*.5f, scy - 18.f},{scx + lw2*.5f, scy - 18.f},
            1.f, ColorAlpha({180,120,255,255}, scA * .50f));

        const char* pres = "PRESENTS";
        int prw = MeasureText(pres, 7);
        DrawText(pres, (int)(scx - prw*.5f), (int)(scy - 12.f), 7,
            ColorAlpha({160,120,220,255}, scA * .60f));
    }

    // ═══════════════════════════════════════════════════════════════
    // PHASE 3 — HYPERSPACE ARRIVAL  (1.60 – 2.80s)
    // ═══════════════════════════════════════════════════════════════
    if(introT >= 1.60f && introT < 2.80f){
        float hp  = (introT - 1.60f) / 1.20f;
        float scx = (float)SCREEN_W * .5f;
        float scy = (float)SCREEN_H * .5f;

        float flashA = hp < 0.12f ? (1.f - hp/0.12f) : 0.f;
        if(flashA > 0.f)
            DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_WHITE, flashA * .95f));

        float streakFade = hp < 0.08f ? hp/0.08f : hp < 0.75f ? 1.f : Clamp01(1.f-(hp-0.75f)/0.25f);
        float maxStretch = 1.f - hp;
        if(streakFade > 0.01f){
            for(int si=0;si<80;si++){
                float a2   = hStreaks[si].angle;
                float len2 = hStreaks[si].baseLen * maxStretch * (0.4f + 0.6f*maxStretch);
                if(len2 < 2.f) continue;
                float cf   = (float)si / 80.f;
                Color sc2  = cf < .33f ? LerpC(VP_WHITE, VP_CYAN, cf*3.f) :
                             cf < .66f ? LerpC(VP_CYAN,  VP_PINK, (cf-.33f)*3.f) :
                                         LerpC(VP_PINK,  VP_LAVENDER, (cf-.66f)*3.f);
                float x1   = scx + cosf(a2) * 8.f;
                float y1   = scy + sinf(a2) * 8.f;
                float x2   = scx + cosf(a2) * (8.f + len2);
                float y2   = scy + sinf(a2) * (8.f + len2);
                float alpha= streakFade * (0.25f + 0.45f * maxStretch);
                DrawLineEx({x1,y1},{x2,y2}, 1.2f + maxStretch*1.5f, ColorAlpha(sc2, alpha));
            }
        }

        if(hp > 0.05f && hp < 0.85f){
            float rp    = (hp - 0.05f) / 0.80f;
            float rSize = (1.f - rp) * 180.f + 20.f;
            float rAlpha= (rp < .5f ? rp*2.f : 2.f-rp*2.f) * 0.55f;
            DrawCircleLinesV({scx,scy}, rSize, ColorAlpha(VP_CYAN, rAlpha));
            DrawCircleLinesV({scx,scy}, rSize*.6f, ColorAlpha(VP_WHITE, rAlpha*.5f));
            DrawLineEx({scx-rSize*.25f,scy},{scx+rSize*.25f,scy},1.f,ColorAlpha(VP_CYAN,rAlpha*.6f));
            DrawLineEx({scx,scy-rSize*.25f},{scx,scy+rSize*.25f},1.f,ColorAlpha(VP_CYAN,rAlpha*.6f));
        }

        if(hp > 0.55f){
            float worldA = (hp - 0.55f) / 0.45f;
            DrawRectangle(0,0,SCREEN_W,SCREEN_H,
                ColorAlpha({0,0,20,255}, Clamp01(1.f - worldA) * 0.6f));
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // PHASE 4 + IDLE  — WORLD (2.80s+)
    // ═══════════════════════════════════════════════════════════════
    if(introT >= 2.30f){
        float gf = bgFade;

        float nbPulse = .45f+.35f*sinf(t*.22f);
        for(int i=0;i<3;i++){
            float nx=SCREEN_W*(0.18f+i*0.32f)+sinf(t*.04f+i*2.1f)*40.f;
            float ny=SCREEN_H*(0.30f+i*0.22f)+cosf(t*.035f+i*1.7f)*30.f;
            float nr=SCREEN_W*(0.28f+0.06f*sinf(t*.06f+i));
            Color nc=i==0?VP_DEEPPURP:i==1?VP_DARKPURP:VP_DEEPBLUE;
            DrawCircleV({nx,ny},nr,ColorAlpha(nc,0.04f*nbPulse*gf));
            DrawCircleV({nx,ny},nr*.5f,ColorAlpha(nc,0.025f*nbPulse*gf));
        }

        for(auto& s:tstars){
            float b=s.b*(.4f+.6f*sinf(t*s.tw+s.to))*gf;
            if(s.layer==2 && b>.5f)
                DrawCircleV({s.x,s.y},s.r+1.5f,ColorAlpha(s.tint,b*.06f));
            DrawCircleV({s.x,s.y},s.r,ColorAlpha(s.layer==2?VP_WHITE:s.tint,
                b*(s.layer==2?.70f:s.layer==1?.55f:.40f)));
        }

        // ── Vaporwave sun — rises from below ──
        {
            float worldRiseP = EaseO(Prog(2.80f, 1.20f));
            float sx  = (float)SCREEN_W * .5f;
            float restY  = (float)SCREEN_H * .58f;
            float startY = restY + 80.f;
            float sy     = startY + (restY - startY) * worldRiseP;
            float bodyR  = 48.f;
            float pulse  = .7f+.3f*sinf(t*.5f);

            Color sunCols[]={{140,20,80,255},{200,40,100,255},{240,80,140,255},{255,120,180,255},{255,160,200,255}};
            for(int ri=6;ri>=0;ri--){
                float gr=(bodyR+ri*16.f);
                DrawCircleV({sx,sy},gr,ColorAlpha(sunCols[0],(.012f+.004f*(7-ri))*pulse*gf));
            }
            for(int i=4;i>=0;i--){
                float r2=bodyR+i*8.f;
                DrawCircleSector({sx,sy},r2,180.f,360.f,24,ColorAlpha(sunCols[i],(.55f+i*.04f)*gf));
            }
            DrawCircleSector({sx,sy},bodyR*.4f,180.f,360.f,24,ColorAlpha({255,255,230,255},.2f*gf));

            for(int si2=0;si2<4;si2++){
                float stripeDelay = si2 * 0.12f;
                float stripeProg  = Clamp01(worldRiseP * 4.f - stripeDelay * 3.f);
                float sy2=sy+5.f+si2*10.f;
                float fy2=sy2-sy; float chord2=bodyR*bodyR-fy2*fy2;
                if(chord2<0) continue;
                float hw=sqrtf(chord2);
                int sw2=(int)(hw*2.f*stripeProg);
                DrawRectangle((int)(sx-hw),(int)sy2, sw2,(int)(2.f+si2*.4f),
                    ColorAlpha({0,0,0,255},.75f-.05f*si2));
                if(stripeProg < 1.f && stripeProg > 0.01f)
                    DrawLineEx({sx-hw+sw2,sy2},{sx-hw+sw2,sy2+2.f+si2*.4f},
                        2.f, ColorAlpha(VP_HOTPINK, (1.f-stripeProg)*0.5f));
            }
            DrawCircleLinesV({sx,sy},bodyR,ColorAlpha({255,160,210,255},.2f*gf));
            DrawRectangle(0,(int)(sy-1),SCREEN_W,2,ColorAlpha({200,50,120,255},.05f*gf));
        }

        // ── Perspective grid — materialises from horizon outward ──
        {
            float gridRise = EaseO(Prog(3.00f, 0.90f));
            float gAlpha   = (.025f+.008f*sinf(t*.6f)) * gf;
            float horizY   = (float)SCREEN_H * .58f;
            float scroll   = fmodf(t*15.f, 36.f);
            for(float gy=horizY; gy<SCREEN_H+36.f; gy+=36.f){
                float gy2=gy+scroll; if(gy2>=SCREEN_H) continue;
                float depth=(gy2-horizY)/(SCREEN_H-horizY);
                if(depth > gridRise) continue;
                DrawLineEx({0,gy2},{(float)SCREEN_W,gy2},1.f,ColorAlpha(VP_HOTPINK,gAlpha*depth*1.8f));
            }
            int vlines=12;
            for(int vi=0;vi<=vlines;vi++){
                float tx2=(float)vi/vlines;
                float bx=tx2*SCREEN_W;
                float topX=SCREEN_W*.5f+(bx-SCREEN_W*.5f)*.03f;
                float dist = fabsf(tx2 - .5f) * 2.f;
                if(dist > gridRise) continue;
                DrawLineEx({topX,horizY},{bx,(float)SCREEN_H},1.f,ColorAlpha(VP_HOTPINK,gAlpha*.7f));
            }
        }

        if(gf > .4f){
            float sw=fmodf(t*40.f,SCREEN_H);
            DrawRectangle(0,(int)sw,SCREEN_W,1,ColorAlpha(VP_WHITE,.01f*gf));
        }

        if(gf > .3f) DrawAmbientParticles(t*0.5f, 25);
    }

    // ── Outer frame (idle) ──
    if(frameFade > 0.f){
        DrawModernCorners(2,2,SCREEN_W-4,SCREEN_H-4,VP_PINK,t,frameFade*0.9f,true);
        float fp=(.45f+.15f*sinf(t*1.2f))*frameFade;
        DrawRectangleLinesEx({6.f,6.f,(float)SCREEN_W-12.f,(float)SCREEN_H-12.f},1.f,ColorAlpha(VP_PURPLE,fp*0.5f));
    }

    // ═══════════════════════════════════════════════════════════════
    // PHASE 5 — LOGO SLAM  (4.00 – 5.60s)
    // ═══════════════════════════════════════════════════════════════
    if(logoFade > 0.f){
        const char* TITLE = "OUT OF ORBIT";
        int tsz    = 56;                          // BIG font
        int totalW = MeasureText(TITLE, tsz);
        int tx     = (SCREEN_W - totalW) / 2;
        int ty     = 12;
        int n      = (int)strlen(TITLE);

        float titleCX = SCREEN_W * 0.5f;
        float titleCY = ty + tsz * 0.5f;

        // ── full-title wide neon halo behind everything ──────────────
        if(logoFade > 0.05f){
            float haloP = 0.55f + 0.45f * sinf(t * 1.6f);
            for(int gi = 10; gi >= 1; gi--){
                float gr = gi * 7.f;
                float ga = logoFade * haloP * (0.055f - gi * 0.004f);
                // colour shifts: outer=purple, inner=orange
                Color hc = gi > 6 ? VP_PURPLE : gi > 3 ? VP_HOTPINK : VP_ORANGE;
                DrawRectangle((int)(tx - gr), (int)(ty - gr * 0.4f),
                    totalW + (int)(gr * 2.f), tsz + (int)(gr * 0.8f),
                    ColorAlpha(hc, ga));
            }
            // bright centre stripe
            float sc = (0.12f + 0.08f * sinf(t * 2.4f)) * logoFade;
            DrawRectangle(tx - 8, ty + tsz/2 - 3, totalW + 16, 6,
                ColorAlpha(VP_WHITE, sc));
        }

        // ── energy rail lines above + below the title ────────────────
        if(logoFade > 0.3f){
            float railA = logoFade * 0.85f;
            float railPulse = 0.6f + 0.4f * sinf(t * 3.f);
            // top rail
            DrawLineEx({(float)tx - 4.f, (float)ty - 2.f},
                       {(float)(tx + totalW) + 4.f, (float)ty - 2.f},
                       2.f, ColorAlpha(VP_CYAN, railA * railPulse));
            DrawLineEx({(float)tx - 4.f, (float)ty - 2.f},
                       {(float)(tx + totalW) + 4.f, (float)ty - 2.f},
                       6.f, ColorAlpha(VP_CYAN, railA * 0.18f));
            // bottom rail
            DrawLineEx({(float)tx - 4.f, (float)(ty + tsz) + 2.f},
                       {(float)(tx + totalW) + 4.f, (float)(ty + tsz) + 2.f},
                       2.f, ColorAlpha(VP_HOTPINK, railA * railPulse));
            DrawLineEx({(float)tx - 4.f, (float)(ty + tsz) + 2.f},
                       {(float)(tx + totalW) + 4.f, (float)(ty + tsz) + 2.f},
                       6.f, ColorAlpha(VP_HOTPINK, railA * 0.18f));
            // corner sparks
            float cs[4][2] = {{(float)tx-4, (float)ty-2},{(float)(tx+totalW)+4,(float)ty-2},
                               {(float)tx-4,(float)(ty+tsz)+2},{(float)(tx+totalW)+4,(float)(ty+tsz)+2}};
            for(auto& c2 : cs){
                DrawCircleV({c2[0],c2[1]}, 3.5f, ColorAlpha(VP_WHITE, railA * railPulse));
                DrawNeonCircle({c2[0],c2[1]}, 6.f, VP_GOLD, railA * 0.6f);
            }
            // energy spark travelling along top rail
            float sparkPos = fmodf(t * 220.f, (float)totalW + 8.f);
            float spx = tx - 4.f + sparkPos;
            DrawCircleV({spx, (float)ty - 2.f}, 3.f, ColorAlpha(VP_WHITE, railA));
            DrawNeonCircle({spx, (float)ty - 2.f}, 8.f, VP_CYAN, railA * 0.7f);
            // second spark going the other way on bottom rail
            float sparkPos2 = fmodf(t * 160.f, (float)totalW + 8.f);
            float spx2 = tx + totalW + 4.f - sparkPos2;
            DrawCircleV({spx2, (float)(ty + tsz) + 2.f}, 3.f, ColorAlpha(VP_WHITE, railA));
            DrawNeonCircle({spx2, (float)(ty + tsz) + 2.f}, 8.f, VP_HOTPINK, railA * 0.7f);
        }

        float totalShakeX = 0.f, totalShakeY = 0.f;
        for(int i=0;i<n;i++){ totalShakeX+=shakeX[i]; totalShakeY+=shakeY[i]; }
        int gsx=(int)(totalShakeX*.25f), gsy=(int)(totalShakeY*.25f);

        int cx2=tx;
        for(int i=0;i<n;i++){
            char buf[2]={TITLE[i],0};
            if(buf[0]==' '){ cx2+=tsz/2+2; continue; }
            int cw2=MeasureText(buf,tsz)+1;

            float lDelay = 4.00f + i * 0.072f;
            float lRaw   = Clamp01((introT - lDelay) / 0.18f);
            float lFade  = lRaw*lRaw*(3.f-2.f*lRaw);

            if(lFade > 0.85f && letterLanded[i] < 0.5f){
                letterLanded[i] = 1.f;
                ringAge[i]      = 0.f;
                shakeX[i] = (i%2==0 ? 1.f : -1.f) * 6.f;
                shakeY[i] = -5.f;
            }
            if(letterLanded[i] > .5f){
                ringAge[i] += GetFrameTime();
                shakeX[i]  *= 0.72f;
                shakeY[i]  *= 0.72f;
            }

            float drop   = (1.f-lFade)*(1.f-lFade)*(-110.f);
            float bounce = lFade > 0.8f ? sinf((lFade-0.8f)*PI*2.5f)*(-6.f*lFade) : 0.f;
            float wobX   = (1.f-lFade)*(1.f-lFade)*sinf(i*1.5f)*6.f;
            float finalY = (float)(ty+(int)drop+(int)bounce);
            float finalX = (float)(cx2+(int)wobX+(int)shakeX[i]+gsx);
            float cx3    = finalX + cw2 * .5f;
            float cy3    = finalY + gsy + tsz * .5f;

            // ── colour: slow rainbow wave left→right across title ────
            float h = fmodf(i * 0.18f + t * 0.22f, 6.f);
            Color cc = h<1 ? LerpC(VP_CYAN,     VP_LAVENDER, h)
                     : h<2 ? LerpC(VP_LAVENDER,  VP_PURPLE,   h-1)
                     : h<3 ? LerpC(VP_PURPLE,    VP_HOTPINK,  h-2)
                     : h<4 ? LerpC(VP_HOTPINK,   VP_GOLD,     h-3)
                     : h<5 ? LerpC(VP_GOLD,      VP_ORANGE,   h-4)
                           : LerpC(VP_ORANGE,    VP_CYAN,     h-5);

            if(lFade > 0.f){

                // ── 1. deep 3-step extrude shadow ───────────────────
                for(int d=5;d>=1;d--)
                    DrawText(buf,(int)(finalX+d*1.8f),(int)(finalY+gsy+d*1.8f),tsz,
                        ColorAlpha({0,0,0,255}, lFade * (0.50f / d)));

                // ── 2. large outer glow bloom ────────────────────────
                float bp = 0.7f + 0.3f * sinf(t * 2.1f + i * 0.6f);
                for(int gi=8;gi>=1;gi--){
                    float gr = gi * 4.5f;
                    float ga = lFade * bp * (0.10f - gi * 0.009f);
                    if(ga <= 0.f) continue;
                    DrawRectangle((int)(finalX - gr*0.55f),(int)(finalY+gsy - gr*0.35f),
                        cw2 + (int)(gr*1.1f), tsz + (int)(gr*0.7f),
                        ColorAlpha(cc, ga));
                }

                // ── 3. chromatic aberration (visible split, 3px) ────
                DrawText(buf,(int)(finalX-3),(int)(finalY+gsy),tsz,
                    ColorAlpha({0,255,240,255}, lFade * 0.45f));
                DrawText(buf,(int)(finalX+3),(int)(finalY+gsy),tsz,
                    ColorAlpha({255,30,130,255}, lFade * 0.45f));

                // ── 4. dark backing shadow ───────────────────────────
                DrawText(buf,(int)(finalX+2),(int)(finalY+gsy+3),tsz,
                    ColorAlpha({0,0,0,255}, lFade * 0.80f));

                // ── 5. main coloured fill ────────────────────────────
                DrawText(buf,(int)finalX,(int)(finalY+gsy),tsz, ColorAlpha(cc, lFade));

                // ── 6. bright top highlight (upper ¼ of letter) ─────
                float hiA = lFade * (0.60f + 0.25f * sinf(t * 3.5f + i));
                DrawText(buf,(int)finalX,(int)(finalY+gsy)-1,tsz,
                    ColorAlpha(VP_WHITE, hiA * 0.38f));

                // ── 7. scanline shimmer stripe (2px, fast sweep) ────
                float scanY = fmodf(t * 55.f + i * 9.f, (float)tsz);
                DrawRectangle((int)finalX,(int)(finalY+gsy+(int)scanY),cw2,2,
                    ColorAlpha(VP_WHITE, lFade * 0.55f));

                // ── 8. flash-white slam burst ────────────────────────
                if(lFade < 0.25f)
                    DrawText(buf,(int)finalX,(int)(finalY+gsy),tsz,
                        ColorAlpha(VP_WHITE,(0.25f-lFade)*4.f*0.90f));

                // ── 9. impact rings on landing ───────────────────────
                if(ringAge[i] < 0.80f && letterLanded[i] > .5f){
                    float ra2    = ringAge[i] / 0.80f;
                    float rr1    = ra2 * (cw2 * 3.8f + 28.f);
                    float rAlpha = (1.f - ra2) * 0.90f;
                    DrawCircleLinesV({cx3,cy3}, rr1,        ColorAlpha(cc,      rAlpha));
                    DrawCircleLinesV({cx3,cy3}, rr1 * .65f, ColorAlpha(VP_WHITE, rAlpha * .60f));
                    DrawCircleLinesV({cx3,cy3}, rr1 * .32f, ColorAlpha(VP_WHITE, rAlpha * .40f));
                    // shockwave sparks at ring edge
                    for(int sp=0;sp<6;sp++){
                        float sa2 = sp*(2.f*PI/6.f) + ra2*PI;
                        DrawCircleV({cx3+cosf(sa2)*rr1, cy3+sinf(sa2)*rr1},
                            2.8f*(1.f-ra2), ColorAlpha(VP_WHITE, rAlpha*0.80f));
                    }
                }

                // ── 10. idle persistent glow after letters settle ────
                if(letterLanded[i] > .5f && logoFade > 0.5f){
                    float idleA = (0.28f + 0.18f*sinf(t*2.2f + i*0.55f)) * logoFade;
                    float idleR = cw2 * 0.72f + 4.f;
                    DrawNeonCircle({cx3,cy3}, idleR, cc, idleA);
                    DrawCircleLinesV({cx3,cy3}, idleR*1.4f,
                        ColorAlpha(cc, idleA * 0.40f));
                }

                // ── 11. floating energy sparks per letter ───────────
                if(letterLanded[i] > .5f && logoFade > 0.5f){
                    for(int sp=0;sp<3;sp++){
                        float spt = fmodf(t*0.9f + i*0.4f + sp*0.33f, 1.f);
                        float spA = sp*(2.f*PI/3.f) + t*1.5f + i*0.7f;
                        float spR = cw2 * 0.55f + 8.f;
                        float spx3= cx3 + cosf(spA)*spR*(0.5f+spt*0.5f);
                        float spy3= cy3 + sinf(spA)*spR*(0.5f+spt*0.5f);
                        float spSz= 1.8f*(1.f-spt);
                        if(spSz > 0.3f){
                            DrawCircleV({spx3,spy3}, spSz,
                                ColorAlpha(VP_WHITE, logoFade*(1.f-spt)*0.90f));
                            DrawNeonCircle({spx3,spy3}, spSz*2.5f, cc,
                                logoFade*(1.f-spt)*0.50f);
                        }
                    }
                }
            }
            cx2 += cw2;
        }

        // ── PHASE 6 — SUBTITLE typewriter (5.60–6.30s) ──
        if(subFade > 0.f){
            const char* sub    = "DEEP SPACE EDITION";
            int subLen         = (int)strlen(sub);
            float typeProgress = Clamp01((introT - 5.60f) / 0.55f);
            int   charsToShow  = (int)(typeProgress * subLen);
            bool  showCursor   = (charsToShow >= subLen) && ((int)(t*4.f)%2==0);

            char typeBuf[32]  = {};
            for(int ci=0;ci<charsToShow && ci<subLen;ci++) typeBuf[ci]=sub[ci];
            if(showCursor && charsToShow < 31) typeBuf[charsToShow] = '_';

            DrawTextGlowC(typeBuf, ty+tsz+8, 8, ColorAlpha(TEXT_SECONDARY, subFade), 0.8f);
        }

        // Divider
        if(subFade > 0.f){
            int dy = ty+tsz+28;
            float dw = Clamp01(subFade * 2.2f) * ((float)SCREEN_W - 100.f);
            DrawLineEx({50.f,(float)dy},{50.f+dw,(float)dy},1.5f,ColorAlpha(VP_ORANGE,.25f*subFade));
            if(subFade > .4f){
                DrawCircleV({(float)SCREEN_W/2,(float)dy},3.f,ColorAlpha(VP_GOLD,.7f*subFade));
                DrawCircleV({(float)SCREEN_W/2,(float)dy},1.5f,ColorAlpha(VP_WHITE,.9f*subFade));
            }
        }
    }

    // ═══════════════════════════════════════════════════════════════
    // PHASE 7 — SHIP SELECT CARDS  (7.00s+)
    // ═══════════════════════════════════════════════════════════════
    if(cardFade > 0.f){
        int logoBottom = 22 + 38 + 26;
        int hdrY = logoBottom + 2;

        {
            float hpulse = (0.75f + 0.25f*sinf(t*2.f)) * cardFade;
            const char* hdr = "SELECT YOUR SHIP";
            int hw = MeasureText(hdr, 11);
            DrawTextGlow(hdr, (SCREEN_W-hw)/2, hdrY, 11, ColorAlpha(TEXT_PRIMARY, hpulse), 0.9f);
            int ly = hdrY + 14;
            int lw = hw + 40;
            int lx = (SCREEN_W - lw) / 2;
            DrawRectangle(lx, ly, lw/2, 2, ColorAlpha(VP_ORANGE, hpulse * 0.8f));
            DrawRectangle(lx + lw/2, ly, (lw+1)/2, 2, ColorAlpha(VP_ORANGE, hpulse * 0.4f));
            DrawCircleV({(float)SCREEN_W/2, (float)ly+1}, 2.5f, ColorAlpha(VP_GOLD, hpulse));
        }

        struct ShipDef{
            const char* name; const char* tag; const char* tip;
            const char* lore;
            int spd,def,atk;
            Color col;
        };
        ShipDef ships[]={
            {"INTERCEPTOR","[1]","AGILE DUELIST",   "Speed-tuned plasma racer", 4,2,3, VP_CYAN   },
            {"BRAWLER",    "[2]","HEAVY STRIKER",   "Quad-cannon siege platform",2,4,5, VP_ORANGE },
            {"PHANTOM",    "[3]","GHOST RUNNER",    "Stealth wing - strike & fade",5,1,3, VP_PURPLE},
            {"TITAN",      "[4]","FORTRESS CLASS",  "Shielded capital warship",  1,5,4, VP_MINT   },
        };

        int ncards=4, pad=10, gap=6;
        int totalW2=SCREEN_W-pad*2-gap*(ncards-1);
        int cw4=totalW2/ncards;
        int cardTop=hdrY+18;
        int cardH=SCREEN_H-cardTop-16;

        static float cardHover[4]={0,0,0,0};
        float hdt=GetFrameTime();

        for(int i=0;i<ncards;i++){
            int cx4=pad+i*(cw4+gap);
            float cSlide=std::min(1.f,std::max(0.f,(cardFade-i*.09f)/.38f));
            if(cSlide<0.1f){ cardHover[i]=0; continue; }
            Rectangle hr={(float)cx4,(float)cardTop,(float)cw4,(float)cardH};
            bool hov=CheckCollisionPointRec(msc,hr);
            cardHover[i]=hov ? std::min(1.f,cardHover[i]+hdt*8.f)
                              : std::max(0.f,cardHover[i]-hdt*10.f);
            if(hov) g.selectedShip=i;
        }

        for(int i=0;i<ncards;i++){
            int cx4=pad+i*(cw4+gap);
            int cy4=cardTop;
            bool sel=(g.selectedShip==i);
            float hv=cardHover[i];
            Color sc4=ships[i].col;

            float cSlide=std::min(1.f,std::max(0.f,(cardFade-i*.09f)/.38f));
            float slideOff=(1.f-cSlide)*(1.f-cSlide)*60.f;
            cy4+=(int)slideOff;
            cy4 -= (int)(hv*4.f);

            float selPulse=.75f+.25f*sinf(t*3.0f+i);

            if((sel||hv>0.05f) && cSlide>0.3f){
                float ga=(sel?0.06f:0.03f)+hv*0.05f;
                for(int gr=3;gr>=1;gr--)
                    DrawRectangle(cx4-gr*3,cy4-gr*3,cw4+gr*6,cardH+gr*6,
                        ColorAlpha(sc4,ga*cSlide/(gr)));
            }

            DrawRectangle(cx4+4,cy4+4,cw4,cardH,ColorAlpha(VP_BLACK,.72f*cSlide));
            DrawRectangle(cx4,cy4,cw4,cardH,ColorAlpha(VP_BLACK,.94f*cSlide));
            float fillA=(sel?0.10f:0.03f)+hv*0.07f;
            DrawRectangle(cx4+1,cy4+1,cw4-2,cardH-2,ColorAlpha(sc4,fillA*cSlide));
            DrawRectangle(cx4+2,cy4+1,cw4-4,2,ColorAlpha(VP_WHITE,0.06f*cSlide));

            int barH=sel?4:2;
            DrawRectangle(cx4,cy4,cw4,barH,ColorAlpha(sc4,(sel?.90f:.35f+hv*.25f)*cSlide));

            float borderA=(sel?0.92f:0.28f+hv*0.30f)*cSlide*selPulse;
            DrawRectangleLines(cx4,cy4,cw4,cardH,ColorAlpha(sc4,borderA));
            if(sel||hv>0.3f)
                DrawRectangleLines(cx4+1,cy4+1,cw4-2,cardH-2,ColorAlpha(sc4,(sel?.22f:hv*.15f)*cSlide));

            if((sel||hv>0.2f) && cSlide>.4f){
                float np=(sel?(.82f+.18f*sinf(t*4.f)):hv*.8f)*cSlide;
                int arm2=8;
                auto CL2=[&](float x1,float y1,float x2,float y2){
                    DrawLineEx({x1,y1},{x2,y2},sel?2.f:1.4f,ColorAlpha(sc4,np));
                };
                float fx=(float)cx4,fy=(float)cy4,fw=(float)(cx4+cw4),fh=(float)(cy4+cardH);
                CL2(fx,fy,fx+arm2,fy); CL2(fx,fy,fx,fy+arm2);
                CL2(fw,fy,fw-arm2,fy); CL2(fw,fy,fw,fy+arm2);
                CL2(fx,fh,fx+arm2,fh); CL2(fx,fh,fx,fh-arm2);
                CL2(fw,fh,fw-arm2,fh); CL2(fw,fh,fw,fh-arm2);
                for(auto [nx2,ny2]:std::initializer_list<std::pair<float,float>>
                    {{fx,fy},{fw,fy},{fx,fh},{fw,fh}}){
                    DrawCircleV({nx2,ny2},2.8f,ColorAlpha(VP_GOLD,.85f*cSlide));
                }
            }

            DrawRectangle(cx4+4,cy4+5,16,10,ColorAlpha(sc4,.18f*cSlide));
            DrawRectangleLines(cx4+4,cy4+5,16,10,ColorAlpha(sc4,.50f*cSlide));
            DrawText(ships[i].tag,cx4+5,cy4+6,8,ColorAlpha(VP_WHITE,.82f*cSlide));

            int previewH=(int)(cardH*0.56f);
            Vector2 previewC={(float)(cx4+cw4/2),(float)(cy4+previewH/2+6)};

            if(cSlide>0.3f){
                float orbitR=18.f+hv*5.f;
                float orbitA=(sel?0.22f:0.08f+hv*0.10f)*cSlide;
                for(int ri=0;ri<24;ri++){
                    float ang=ri*(PI*2.f/24.f)+t*(sel?.35f:.08f);
                    float rx=previewC.x+cosf(ang)*orbitR;
                    float ry=previewC.y+sinf(ang)*orbitR*.38f;
                    DrawCircleV({rx,ry},ri%3==0?1.4f:.7f,ColorAlpha(sc4,orbitA*(ri%3==0?1.f:.5f)));
                }
                float haloR=22.f+(sel?3.f:0.f)+hv*4.f;
                DrawCircleV(previewC,haloR,ColorAlpha(sc4,(sel?.10f:.04f+hv*.05f)*cSlide));
                DrawCircleV(previewC,haloR*.6f,ColorAlpha(sc4,(sel?.06f:.02f)*cSlide));
            }

            if(cSlide>0.5f && (sel||hv>0.1f)){
                float partA=(sel?.65f:hv*.55f)*cSlide;
                int nParts=sel?8:4;
                for(int pi=0;pi<nParts;pi++){
                    float pang=pi*(2*PI/nParts)+t*(i==0?1.4f:i==1?.6f:i==2?2.2f:.4f);
                    float pr=10.f+6.f*sinf(t*2.3f+pi*1.1f);
                    float px2=previewC.x+cosf(pang)*pr;
                    float py2=previewC.y+sinf(pang)*pr*.5f;
                    if(i==0){
                        DrawLineEx({px2,py2},{px2-cosf(pang)*5.f,py2-sinf(pang)*2.5f},
                            1.f,ColorAlpha(sc4,partA*.9f));
                        DrawCircleV({px2,py2},.9f,ColorAlpha(VP_WHITE,partA*.7f));
                    } else if(i==1){
                        float es=sinf(t*4.f+pi)*.5f+.5f;
                        DrawCircleV({px2,py2},1.2f+es*.8f,ColorAlpha(sc4,partA*es));
                        DrawCircleV({px2,py2},.5f,ColorAlpha(VP_YELLOW,partA*.6f));
                    } else if(i==2){
                        float ws=sinf(t*3.f+pi*1.5f)*.5f+.5f;
                        for(int wi=0;wi<2;wi++)
                            DrawCircleV({px2+(float)wi,py2},1.f+ws,ColorAlpha(sc4,partA*(0.5f-wi*.2f)));
                    } else {
                        float arcStart=pang-0.25f, arcEnd=pang+0.25f;
                        DrawLineEx({previewC.x+cosf(arcStart)*pr, previewC.y+sinf(arcStart)*pr*.5f},
                                   {previewC.x+cosf(arcEnd)*pr,   previewC.y+sinf(arcEnd)*pr*.5f},
                                   1.2f,ColorAlpha(sc4,partA*.7f));
                    }
                }
            }

            float spin=sel?t*1.2f:t*0.10f;
            if(hv>0.05f||sel){
                for(int gs=1;gs>=1;gs--){
                    DrawShipSilhouette(i,{previewC.x,previewC.y+gs*.5f},
                        spin-PI*.5f,t,ColorAlpha(sc4,0.08f*hv*cSlide),true,false);
                }
            }
            DrawShipSilhouette(i,previewC,spin-PI*.5f,t,sc4,sel||hv>0.4f,false);

            if((sel||hv>0.2f) && cSlide>0.4f){
                float exA=(sel?0.55f:hv*.45f)*cSlide;
                for(int ei=0;ei<5;ei++){
                    float eo=ei/5.f;
                    float ew=3.f*(1.f-eo);
                    float ey=previewC.y+16.f+eo*14.f+sinf(t*8.f+ei)*1.5f;
                    float ex=previewC.x+sinf(t*6.f+ei*.7f)*2.f;
                    DrawRectangle((int)(ex-ew/2),(int)ey,(int)ew,2,
                        ColorAlpha(sc4,exA*(1.f-eo)));
                    if(ei<3)DrawCircleV({ex,ey+1.f},ew*.6f,
                        ColorAlpha(VP_WHITE,exA*.35f*(1.f-eo)));
                }
            }

            int divY=cy4+previewH+2;
            for(int dx2=0;dx2<cw4-12;dx2++){
                float dpct=(float)dx2/(cw4-12);
                float da=(dpct<.5f?dpct*2:2-dpct*2)*.35f*(sel?.9f:.4f)*cSlide;
                DrawPixel(cx4+6+dx2,divY,ColorAlpha(sc4,da));
            }

            int nameFS=sel?11:10;
            int nw4=MeasureText(ships[i].name,nameFS);
            int nameY=divY+5;
            if(sel||hv>0.3f){
                for(int gp=1;gp<=2;gp++)
                    DrawText(ships[i].name,cx4+(cw4-nw4)/2-gp,nameY,nameFS,
                        ColorAlpha(sc4,.08f*selPulse*cSlide));
            }
            DrawText(ships[i].name,cx4+(cw4-nw4)/2+1,nameY+1,nameFS,
                ColorAlpha(VP_BLACK,.55f*cSlide));
            DrawText(ships[i].name,cx4+(cw4-nw4)/2,nameY,nameFS,
                ColorAlpha(sc4,(sel?1.f:.60f+hv*.20f)*cSlide));

            int tw2=MeasureText(ships[i].tip,7);
            DrawText(ships[i].tip,cx4+(cw4-tw2)/2,nameY+13,7,
                ColorAlpha(VP_OFFWHITE,(sel?.58f:.28f+hv*.18f)*cSlide));
            if(sel||hv>0.4f){
                int lw2=MeasureText(ships[i].lore,6);
                DrawText(ships[i].lore,cx4+(cw4-lw2)/2,nameY+22,6,
                    ColorAlpha(VP_GRAY,(sel?.42f:hv*.35f)*cSlide));
            }

            int statY=nameY+36;
            DrawLine(cx4+6,statY-4,cx4+cw4-6,statY-4,ColorAlpha(sc4,.14f*cSlide));
            int stats[3]={ships[i].spd,ships[i].def,ships[i].atk};
            const char* labels[3]={"SPD","DEF","ATK"};
            int barMaxW=cw4-30;
            for(int si=0;si<3;si++){
                int rowY=statY+si*13;
                int lblY=rowY;
                int barY=rowY+8;
                DrawText(labels[si],cx4+5,lblY,7,ColorAlpha(VP_GRAY,(sel?.65f:.38f)*cSlide));
                DrawRectangle(cx4+22,barY,barMaxW,5,ColorAlpha(VP_BLACK,.55f*cSlide));
                DrawRectangle(cx4+22,barY,barMaxW,5,ColorAlpha(sc4,.08f*cSlide));
                int fw2=(int)(barMaxW*stats[si]/5.f);
                DrawRectangle(cx4+22,barY,fw2,5,ColorAlpha(sc4,(sel?.76f:.40f+hv*.18f)*cSlide));
                DrawRectangle(cx4+22,barY,fw2,2,ColorAlpha(VP_WHITE,(sel?.18f:.06f)*cSlide));
                for(int ti=1;ti<5;ti++){
                    int tx=cx4+22+ti*barMaxW/5;
                    DrawRectangle(tx,barY-1,1,7,ColorAlpha(VP_BLACK,.55f*cSlide));
                }
            }

            if(sel && cSlide>.3f){
                float sp4=(.72f+.28f*sinf(t*4.5f))*cSlide;
                int bY=cy4+cardH-14;
                DrawRectangle(cx4+1,bY,cw4-2,13,ColorAlpha(sc4,.28f));
                DrawRectangle(cx4+1,bY,cw4-2,1,ColorAlpha(sc4,.60f));
                int bw4=MeasureText(">> SELECTED <<",8);
                DrawText(">> SELECTED <<",cx4+(cw4-bw4)/2+1,bY+2,8,ColorAlpha(VP_BLACK,.55f));
                DrawText(">> SELECTED <<",cx4+(cw4-bw4)/2,  bY+2,8,ColorAlpha(VP_WHITE,sp4));
            } else if(hv>0.1f && cSlide>.3f){
                int bY=cy4+cardH-14;
                DrawRectangle(cx4+1,bY,cw4-2,13,ColorAlpha(sc4,hv*.15f));
                int bw4=MeasureText("CLICK TO SELECT",7);
                DrawText("CLICK TO SELECT",cx4+(cw4-bw4)/2,bY+2,7,
                    ColorAlpha(VP_WHITE,hv*.65f*cSlide));
            }
        }
    }

    // ── FOOTER ──
    if(cardFade > 0.f){
        int fy=SCREEN_H-16;
        DrawRectangle(14,fy-3,SCREEN_W-28,1,ColorAlpha(VP_PURPLE,.35f*cardFade));
        std::string hs="HI-SCORE  "+FmtScore(g.highScore);
        DrawText(hs.c_str(),22,fy,8,ColorAlpha(VP_GOLD,cardFade*.88f));
        if((int)(t*2)%2==0)
            CTGlow("ENTER TO START",fy,9,ColorAlpha(VP_WHITE,cardFade),.92f);
        {
            const char* qstr="[Q] QUIT";
            int qw=MeasureText(qstr,7);
            int qx=SCREEN_W-qw-26,qy=fy;
            DrawText(qstr,qx,qy,7,ColorAlpha(VP_GRAY,.45f*cardFade));
        }
        DrawText("DIGITAL VOID SYSTEMS",
            SCREEN_W-MeasureText("DIGITAL VOID SYSTEMS",7)-22,fy+1,
            7,ColorAlpha(VP_GRAY,.40f*cardFade));
    }

    // ── PRE-INTRO OVERLAY — black fades fully by hyperspace ──
    if(introT < 1.60f){
        float fl = Clamp01(introT / 1.60f);
        DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_BLACK, 1.f - fl * fl));
    }
}


// ─── Sprite placeholder: draws a thematic icon for each upgrade symbol ───

static void DrawUpgradeSprite(const Upgrade& u, int cx, int cy, int sz, float t, float alpha){
    Color c = u.col;
    float p = sinf(t*2.5f+cx*.04f)*.5f+.5f;
    float S = sz/32.f; // scale factor relative to 32px base
    float fmx=(float)(cx+sz/2), fmy=(float)(cy+sz/2);
    int mx=(int)fmx, my=(int)fmy;
    // outer glow halo
    DrawCircleV({fmx,fmy},sz*.65f,ColorAlpha(c,alpha*.08f+p*.04f));
    const std::string& sym=u.symbol;

    auto PX=[&](int x,int y,Color pc,float pa){ DrawRectangle((int)(fmx+x*S-S*.5f),(int)(fmy+y*S-S*.5f),(int)std::max(1.f,S),(int)std::max(1.f,S),ColorAlpha(pc,pa*alpha)); };

    if(sym=="SKR"){// Sakura: cherry blossom with layered petals, stamen, branch
        float rot=t*.4f;
        // branch
        DrawLineEx({fmx-7*S,fmy+10*S},{fmx-2*S,fmy+2*S},1.5f*S,ColorAlpha({80,40,20,255},alpha*.7f));
        DrawLineEx({fmx-2*S,fmy+2*S},{fmx+4*S,fmy-3*S},1.2f*S,ColorAlpha({80,40,20,255},alpha*.6f));
        // small leaf on branch
        DrawCircleV({fmx-5*S,fmy+7*S},2.2f*S,ColorAlpha({40,120,50,255},alpha*.5f));
        DrawCircleV({fmx-4.5f*S,fmy+6.5f*S},1.5f*S,ColorAlpha({60,160,70,255},alpha*.45f));
        // 5 petals with gradient layers
        for(int i=0;i<5;i++){
            float a=i*(2*PI/5)+rot;
            float r1=8.f*S, r2=5.f*S;
            float px=fmx+cosf(a)*r1, py=fmy+sinf(a)*r1;
            // outer petal shadow
            DrawCircleV({px+.5f*S,py+.5f*S},4.5f*S,ColorAlpha({120,30,80,255},alpha*.25f));
            // main petal body
            DrawCircleV({px,py},4.2f*S,ColorAlpha(c,alpha*.75f));
            // petal midtone
            DrawCircleV({px-cosf(a)*.8f*S,py-sinf(a)*.8f*S},3.2f*S,ColorAlpha({255,150,220,255},alpha*.55f));
            // petal highlight
            DrawCircleV({px-cosf(a)*1.5f*S,py-sinf(a)*1.5f*S},1.8f*S,ColorAlpha(VP_WHITE,alpha*.25f));
            // petal vein line
            DrawLineEx({fmx+cosf(a)*2*S,fmy+sinf(a)*2*S},{px,py},0.6f*S,ColorAlpha({200,80,150,255},alpha*.30f));
        }
        // center pistil cluster
        DrawCircleV({fmx,fmy},3.2f*S,ColorAlpha({180,60,100,255},alpha*.8f));
        DrawCircleV({fmx,fmy},2.2f*S,ColorAlpha(VP_GOLD,alpha*.85f));
        // stamen dots
        for(int i=0;i<6;i++){
            float a=i*(PI/3)+rot*1.5f;
            DrawCircleV({fmx+cosf(a)*2*S,fmy+sinf(a)*2*S},0.8f*S,ColorAlpha(VP_YELLOW,alpha*.7f));
            DrawCircleV({fmx+cosf(a)*2*S,fmy+sinf(a)*2*S},0.4f*S,ColorAlpha(VP_WHITE,alpha*.5f));
        }
        DrawCircleV({fmx,fmy},1.f*S,ColorAlpha(VP_WHITE,alpha*p*.6f));
        // falling petal particles
        for(int i=0;i<3;i++){
            float ft=fmodf(t*.6f+i*1.2f,3.f);
            float fx=fmx+6*S+sinf(t+i*2)*4*S, fy=fmy-8*S+ft*6*S;
            DrawCircleV({fx,fy},1.2f*S,ColorAlpha(c,alpha*(1.f-ft/3.f)*.4f));
        }
    } else if(sym=="SWF"){// Swift: ornate lightning bolt with crackling energy
        float flash=.6f+.4f*sinf(t*8.f);
        // energy field behind bolt
        DrawCircleV({fmx,fmy},10*S,ColorAlpha(c,alpha*.06f*flash));
        DrawCircleV({fmx,fmy},7*S,ColorAlpha(c,alpha*.10f*flash));
        // main bolt body (thick multi-segment)
        Vector2 pts[]={
            {fmx-1*S,fmy-11*S},{fmx+3*S,fmy-4*S},{fmx-1*S,fmy-3*S},
            {fmx+4*S,fmy+2*S},{fmx,fmy+1*S},{fmx+2*S,fmy+11*S}
        };
        // glow pass
        for(int i=0;i<5;i++) DrawLineEx(pts[i],pts[i+1],4.f*S,ColorAlpha(c,alpha*.12f*flash));
        // bright core
        for(int i=0;i<5;i++) DrawLineEx(pts[i],pts[i+1],2.2f*S,ColorAlpha(c,alpha*.85f));
        // white hot center
        for(int i=0;i<5;i++) DrawLineEx(pts[i],pts[i+1],1.f*S,ColorAlpha(VP_WHITE,alpha*.55f*flash));
        // crackling side branches
        for(int i=0;i<4;i++){
            float by=fmy-8*S+i*5*S;
            float bx=fmx+(i%2==0?-1:1)*2*S;
            float ex=bx+(i%2==0?-5:5)*S, ey=by+(i%2==0?-2:2)*S;
            DrawLineEx({bx,by},{ex,ey},1.f*S,ColorAlpha(c,alpha*.5f*flash));
            DrawLineEx({ex,ey},{ex+(i%2==0?-2:2)*S,ey+2*S},0.7f*S,ColorAlpha(c,alpha*.3f*flash));
        }
        // spark particles at tips
        for(int i=0;i<3;i++){
            float sa=t*6+i*2.1f;
            DrawCircleV({pts[0].x+cosf(sa)*2*S,pts[0].y+sinf(sa)*2*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.5f*flash));
            DrawCircleV({pts[5].x+cosf(sa+1)*2*S,pts[5].y+sinf(sa+1)*2*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.4f*flash));
        }
    } else if(sym=="PTL"){// Petal Storm: ornate explosion with shrapnel and fire rings
        float exP=sinf(t*3)*.5f+.5f;
        // outer shockwave rings
        for(int i=0;i<3;i++){
            float r=(9+i*3)*S+exP*2*S;
            DrawCircleLinesV({fmx,fmy},r,ColorAlpha(c,alpha*(.25f-.06f*i)*exP));
        }
        // fire tongues radiating out
        for(int i=0;i<12;i++){
            float a=i*(PI/6)+t*.6f;
            float r1=4*S, r2=(9+sinf(t*4+i)*2)*S;
            float w=1.8f*S-(i%3)*.3f*S;
            DrawLineEx({fmx+cosf(a)*r1,fmy+sinf(a)*r1},{fmx+cosf(a)*r2,fmy+sinf(a)*r2},w,
                ColorAlpha(i%3==0?VP_YELLOW:i%3==1?VP_ORANGE:c,alpha*(.7f+exP*.2f)));
            // tip ember
            DrawCircleV({fmx+cosf(a)*r2,fmy+sinf(a)*r2},0.9f*S,ColorAlpha(VP_YELLOW,alpha*.5f*exP));
        }
        // shrapnel fragments
        for(int i=0;i<6;i++){
            float a=i*(PI/3)+t*.3f, r=7*S+sinf(t*2+i*1.5f)*2*S;
            float fx=fmx+cosf(a)*r, fy=fmy+sinf(a)*r;
            DrawRectanglePro({fx,fy,2.5f*S,1.5f*S},{1.25f*S,.75f*S},a*180/PI+t*40,ColorAlpha({180,60,30,255},alpha*.6f));
        }
        // center fireball layers
        DrawCircleV({fmx,fmy},5*S,ColorAlpha({180,30,10,255},alpha*.8f));
        DrawCircleV({fmx,fmy},3.5f*S,ColorAlpha(VP_ORANGE,alpha*.75f*exP));
        DrawCircleV({fmx,fmy},2.2f*S,ColorAlpha(VP_YELLOW,alpha*.8f));
        DrawCircleV({fmx,fmy},1.2f*S,ColorAlpha(VP_WHITE,alpha*.7f*exP));
        DrawNeonCircle({fmx,fmy},5.5f*S,c,alpha*.3f*exP);
    } else if(sym=="EYE"){// Miko Eye: detailed mystical eye with iris patterns and lashes
        float blink=sinf(t*2.5f);
        float eyeH=sz*.20f*(0.7f+0.3f*std::max(0.f,blink));
        // eye shadow
        DrawEllipse(mx,my+1,(int)(sz*.38f),(int)(eyeH+2),ColorAlpha(VP_BLACK,alpha*.4f));
        // outer eye shape (sclera)
        DrawEllipse(mx,my,(int)(sz*.36f),(int)(eyeH+1),ColorAlpha({240,230,245,255},alpha*.85f));
        // eyelid crease above
        DrawEllipseLines(mx,my-1,(int)(sz*.38f),(int)(eyeH+3),ColorAlpha(c,alpha*.4f));
        // iris with detailed rings
        float irisR=sz*.16f;
        DrawCircleV({fmx,fmy},irisR+1*S,ColorAlpha({60,20,80,255},alpha*.7f));
        DrawCircleV({fmx,fmy},irisR,ColorAlpha(c,alpha*.85f));
        // iris pattern lines
        for(int i=0;i<12;i++){
            float a=i*(PI/6)+t*.2f;
            DrawLineEx({fmx+cosf(a)*2*S,fmy+sinf(a)*2*S},{fmx+cosf(a)*irisR*.9f,fmy+sinf(a)*irisR*.9f},
                0.5f*S,ColorAlpha({200,120,255,255},alpha*.35f));
        }
        DrawCircleLinesV({fmx,fmy},irisR*.7f,ColorAlpha(c,alpha*.3f));
        // pupil
        DrawCircleV({fmx,fmy},sz*.08f,ColorAlpha(VP_BLACK,alpha*.9f));
        // pupil highlight
        DrawCircleV({fmx-1.5f*S,fmy-1.5f*S},1.5f*S,ColorAlpha(VP_WHITE,alpha*.75f));
        DrawCircleV({fmx+1*S,fmy+1*S},0.7f*S,ColorAlpha(VP_WHITE,alpha*.4f));
        // eyelashes
        for(int i=0;i<5;i++){
            float la=PI+i*(PI/6)-PI/12;
            float lx1=fmx+cosf(la)*sz*.34f, ly1=fmy+sinf(la)*eyeH*.85f;
            float lx2=lx1+cosf(la-0.3f)*3*S, ly2=ly1+sinf(la-0.3f)*3*S;
            DrawLineEx({lx1,ly1},{lx2,ly2},0.8f*S,ColorAlpha(c,alpha*.6f));
        }
        // mystical sparkle
        float sp=sinf(t*4)*.5f+.5f;
        DrawCircleV({fmx+4*S,fmy-4*S},1*S,ColorAlpha(VP_WHITE,alpha*.4f*sp));
        DrawNeonCircle({fmx,fmy},irisR+2*S,c,alpha*.15f*p);
    } else if(sym=="WRD"){// Ward: layered hexagonal shield with energy field and runes
        float shP=sinf(t*2)*.5f+.5f;
        // energy field background
        DrawCircleV({fmx,fmy},12*S,ColorAlpha(c,alpha*.05f*shP));
        // outer hex with thick edges
        for(int i=0;i<6;i++){
            float a=i*(PI/3)-PI/6, b=(i+1)*(PI/3)-PI/6;
            float r=11*S;
            DrawLineEx({fmx+cosf(a)*r,fmy+sinf(a)*r},{fmx+cosf(b)*r,fmy+sinf(b)*r},1.8f*S,ColorAlpha(c,alpha*.5f));
        }
        // mid hex
        for(int i=0;i<6;i++){
            float a=i*(PI/3)-PI/6, b=(i+1)*(PI/3)-PI/6;
            float r=8*S;
            DrawLineEx({fmx+cosf(a)*r,fmy+sinf(a)*r},{fmx+cosf(b)*r,fmy+sinf(b)*r},1.5f*S,ColorAlpha(c,alpha*.7f));
            // connecting struts between hexes
            float aOuter=a, rOuter=11*S;
            DrawLineEx({fmx+cosf(a)*r,fmy+sinf(a)*r},{fmx+cosf(aOuter)*rOuter,fmy+sinf(aOuter)*rOuter},
                0.7f*S,ColorAlpha(c,alpha*.35f));
        }
        // inner hex (bright core)
        for(int i=0;i<6;i++){
            float a=i*(PI/3)-PI/6, b=(i+1)*(PI/3)-PI/6;
            float r=4.5f*S;
            DrawLineEx({fmx+cosf(a)*r,fmy+sinf(a)*r},{fmx+cosf(b)*r,fmy+sinf(b)*r},1.2f*S,ColorAlpha(VP_WHITE,alpha*.4f*shP));
        }
        // rune marks on mid hex edges
        for(int i=0;i<6;i++){
            float a=i*(PI/3)-PI/6+PI/6;
            float rx=fmx+cosf(a)*8*S, ry=fmy+sinf(a)*8*S;
            DrawCircleV({rx,ry},1.2f*S,ColorAlpha(VP_WHITE,alpha*.5f*shP));
            DrawCircleV({rx,ry},0.6f*S,ColorAlpha(c,alpha*.8f));
        }
        // center ward eye
        DrawCircleV({fmx,fmy},3*S,ColorAlpha(c,alpha*.6f));
        DrawCircleV({fmx,fmy},2*S,ColorAlpha(VP_WHITE,alpha*.45f*shP));
        DrawCircleV({fmx,fmy},1*S,ColorAlpha(c,alpha*.9f));
        DrawNeonCircle({fmx,fmy},4*S,c,alpha*.25f*shP);
    } else if(sym=="LFB"){// Lifebloom: ornate crystal heart with veins and bloom particles
        float hb=sinf(t*3.5f)*.5f+.5f; // heartbeat pulse
        float beat=1.f+hb*.08f;
        // drop shadow
        float s=sz*.20f*beat;
        DrawCircleV({fmx-(s-.5f*S),fmy-(s*.2f)+1*S},s*1.05f,ColorAlpha(VP_BLACK,alpha*.25f));
        DrawCircleV({fmx+(s-.5f*S),fmy-(s*.2f)+1*S},s*1.05f,ColorAlpha(VP_BLACK,alpha*.25f));
        // heart body - left lobe
        DrawCircleV({fmx-s,fmy-s*.3f},s,ColorAlpha({180,20,40,255},alpha*.85f));
        DrawCircleV({fmx+s,fmy-s*.3f},s,ColorAlpha({180,20,40,255},alpha*.85f));
        DrawTriangle({fmx-s*2.1f,fmy+s*.1f},{fmx+s*2.1f,fmy+s*.1f},{fmx,fmy+s*2.5f},
            ColorAlpha({180,20,40,255},alpha*.85f));
        // lighter overlay
        DrawCircleV({fmx-s,fmy-s*.5f},s*.7f,ColorAlpha(c,alpha*.5f));
        DrawCircleV({fmx+s,fmy-s*.5f},s*.7f,ColorAlpha(c,alpha*.5f));
        // highlight shine
        DrawCircleV({fmx-s*.5f,fmy-s*.7f},s*.4f,ColorAlpha(VP_WHITE,alpha*.3f));
        // blood veins
        DrawLineEx({fmx,fmy-s*.3f},{fmx-s*1.2f,fmy+s*.8f},0.6f*S,ColorAlpha({120,10,25,255},alpha*.5f));
        DrawLineEx({fmx,fmy-s*.3f},{fmx+s*1.2f,fmy+s*.8f},0.6f*S,ColorAlpha({120,10,25,255},alpha*.5f));
        DrawLineEx({fmx,fmy-s*.3f},{fmx,fmy+s*1.8f},0.6f*S,ColorAlpha({120,10,25,255},alpha*.45f));
        // glow pulse
        DrawNeonCircle({fmx,fmy},s*2.5f,c,alpha*.15f*hb);
        // bloom particles rising
        for(int i=0;i<4;i++){
            float ft=fmodf(t*.8f+i*.7f,2.5f);
            float fx=fmx+sinf(t*1.5f+i*1.8f)*3*S;
            float fy2=fmy-s-ft*4*S;
            DrawCircleV({fx,fy2},1*S,ColorAlpha(c,alpha*(1.f-ft/2.5f)*.5f));
            DrawCircleV({fx,fy2},.5f*S,ColorAlpha(VP_WHITE,alpha*(1.f-ft/2.5f)*.35f));
        }
    } else if(sym=="ZEN"){// Zen: detailed yin-yang with flowing energy and meditation aura
        float zenR=sz*.30f;
        float rot=t*.3f;
        // meditation aura rings
        for(int i=0;i<3;i++){
            float ar=zenR+(3+i*2)*S+sinf(t*1.5f+i)*S;
            DrawCircleLinesV({fmx,fmy},ar,ColorAlpha(c,alpha*(.08f-.02f*i)));
        }
        // main circle body
        DrawCircleV({fmx,fmy},zenR,ColorAlpha({15,8,35,255},alpha*.85f));
        // yin half (top = colored)
        DrawCircleSector({fmx,fmy},zenR,(-90+rot*180/PI),(90+rot*180/PI),24,ColorAlpha(c,alpha*.7f));
        // yang half stays dark (already drawn)
        // inner S-curve dots
        float dy=zenR*.48f;
        DrawCircleV({fmx+cosf(rot+PI/2)*0,fmy-dy},zenR*.22f,ColorAlpha(c,alpha*.8f));
        DrawCircleV({fmx,fmy+dy},zenR*.22f,ColorAlpha({15,8,35,255},alpha*.75f));
        // pupils in dots
        DrawCircleV({fmx,fmy-dy},zenR*.09f,ColorAlpha({15,8,35,255},alpha*.7f));
        DrawCircleV({fmx,fmy+dy},zenR*.09f,ColorAlpha(c,alpha*.7f));
        // outer ring with notches
        DrawCircleLinesV({fmx,fmy},zenR,ColorAlpha(c,alpha*.75f));
        DrawCircleLinesV({fmx,fmy},zenR+1*S,ColorAlpha(c,alpha*.3f));
        for(int i=0;i<8;i++){
            float a=i*(PI/4)+rot;
            DrawLineEx({fmx+cosf(a)*zenR,fmy+sinf(a)*zenR},
                       {fmx+cosf(a)*(zenR+2*S),fmy+sinf(a)*(zenR+2*S)},0.8f*S,ColorAlpha(c,alpha*.5f));
        }
        // chi flow particles orbiting
        for(int i=0;i<4;i++){
            float a=t*1.2f+i*(PI/2);
            float or2=zenR+3*S;
            DrawCircleV({fmx+cosf(a)*or2,fmy+sinf(a)*or2},0.9f*S,ColorAlpha(VP_WHITE,alpha*.45f*p));
        }
    } else if(sym=="SRG"){// Surge: rocket/arrow with energy comet trail and sparks
        float srgP=sinf(t*5)*.5f+.5f;
        // energy trail
        for(int i=0;i<8;i++){
            float ft=(float)i/8.f;
            float tw=3.5f*S*(1.f-ft);
            float ty=fmy+3*S+i*2.5f*S;
            DrawRectangle((int)(fmx-tw/2),(int)ty,(int)tw,(int)(2*S),
                ColorAlpha(c,alpha*(.5f-.05f*i)*(1.f-ft)));
            // side sparks
            if(i%2==0){
                float sx=fmx+(i%4==0?-1:1)*(tw/2+1*S);
                DrawCircleV({sx,ty},0.7f*S,ColorAlpha(VP_WHITE,alpha*.35f*(1.f-ft)));
            }
        }
        // arrow body
        DrawTriangle({fmx,fmy-11*S},{fmx-5*S,fmy+1*S},{fmx+5*S,fmy+1*S},ColorAlpha(c,alpha*.85f));
        // arrow midtone
        DrawTriangle({fmx,fmy-8*S},{fmx-3*S,fmy},{fmx+3*S,fmy},ColorAlpha({200,150,255,255},alpha*.45f));
        // highlight edge
        DrawLineEx({fmx,fmy-11*S},{fmx-5*S,fmy+1*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.3f));
        DrawLineEx({fmx,fmy-11*S},{fmx+5*S,fmy+1*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.25f));
        // tail fins
        DrawTriangle({fmx-5*S,fmy+1*S},{fmx-8*S,fmy+5*S},{fmx-2*S,fmy+3*S},ColorAlpha(c,alpha*.65f));
        DrawTriangle({fmx+5*S,fmy+1*S},{fmx+8*S,fmy+5*S},{fmx+2*S,fmy+3*S},ColorAlpha(c,alpha*.65f));
        // tip glow
        DrawCircleV({fmx,fmy-11*S},2*S,ColorAlpha(VP_WHITE,alpha*.5f*srgP));
        DrawNeonCircle({fmx,fmy-11*S},3*S,c,alpha*.3f*srgP);
    } else if(sym=="CHI"){// Chi Strike: ornate starburst with kanji-inspired center
        float chiP=sinf(t*3)*.5f+.5f;
        // outer energy corona
        for(int i=0;i<16;i++){
            float a=i*(PI/8)+t*.5f;
            float r1=4*S, r2=(10+sinf(t*3+i)*2)*S;
            float w=i%2==0?1.5f*S:0.8f*S;
            Color rc=i%4==0?VP_WHITE:i%2==0?ColorAlpha(c,1.f):ColorAlpha({255,120,180,255},1.f);
            DrawLineEx({fmx+cosf(a)*r1,fmy+sinf(a)*r1},{fmx+cosf(a)*r2,fmy+sinf(a)*r2},w,
                ColorAlpha(rc,alpha*(.6f+chiP*.2f)));
        }
        // secondary shorter rays
        for(int i=0;i<8;i++){
            float a=i*(PI/4)+PI/8+t*.5f;
            float r1=3*S, r2=6*S;
            DrawLineEx({fmx+cosf(a)*r1,fmy+sinf(a)*r1},{fmx+cosf(a)*r2,fmy+sinf(a)*r2},
                0.6f*S,ColorAlpha(c,alpha*.4f));
        }
        // center kanji-style mark
        DrawCircleV({fmx,fmy},4.5f*S,ColorAlpha({40,10,50,255},alpha*.85f));
        DrawCircleLinesV({fmx,fmy},4.5f*S,ColorAlpha(c,alpha*.7f));
        // cross strokes inside
        DrawLineEx({fmx-2.5f*S,fmy},{fmx+2.5f*S,fmy},1.2f*S,ColorAlpha(c,alpha*.8f));
        DrawLineEx({fmx,fmy-2.5f*S},{fmx,fmy+2.5f*S},1.2f*S,ColorAlpha(c,alpha*.8f));
        DrawLineEx({fmx-1.5f*S,fmy-2*S},{fmx+1.5f*S,fmy+2*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.35f));
        // corner dots
        for(int i=0;i<4;i++){
            float a=i*(PI/2)+PI/4;
            DrawCircleV({fmx+cosf(a)*3*S,fmy+sinf(a)*3*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.55f*chiP));
        }
        DrawNeonCircle({fmx,fmy},5*S,c,alpha*.2f*chiP);
    } else if(sym=="GST"){// Ghost: spectral ship silhouette with speed lines and afterimage
        float gstP=sinf(t*4)*.5f+.5f;
        // afterimage copies trailing behind
        for(int gi=3;gi>=1;gi--){
            float goff=gi*3.5f*S;
            float ga=alpha*(.15f-.03f*gi);
            // ghost ship body
            DrawTriangle({fmx,fmy-6*S+goff},{fmx-4*S,fmy+4*S+goff},{fmx+4*S,fmy+4*S+goff},
                ColorAlpha(c,ga));
        }
        // speed lines
        for(int i=0;i<5;i++){
            float lx=fmx-8*S+i*4*S;
            float ly1=fmy+6*S, ly2=fmy+12*S;
            DrawLineEx({lx,ly1},{lx,ly2},0.7f*S,ColorAlpha(c,alpha*(.3f-.04f*i)));
        }
        // main ship body
        DrawTriangle({fmx,fmy-8*S},{fmx-5*S,fmy+4*S},{fmx+5*S,fmy+4*S},ColorAlpha(c,alpha*.85f));
        // cockpit canopy
        DrawTriangle({fmx,fmy-5*S},{fmx-2*S,fmy+1*S},{fmx+2*S,fmy+1*S},
            ColorAlpha({200,180,255,255},alpha*.5f));
        // wing tips
        DrawTriangle({fmx-5*S,fmy+4*S},{fmx-8*S,fmy+6*S},{fmx-3*S,fmy+2*S},ColorAlpha(c,alpha*.6f));
        DrawTriangle({fmx+5*S,fmy+4*S},{fmx+8*S,fmy+6*S},{fmx+3*S,fmy+2*S},ColorAlpha(c,alpha*.6f));
        // highlight edge
        DrawLineEx({fmx,fmy-8*S},{fmx-5*S,fmy+4*S},0.7f*S,ColorAlpha(VP_WHITE,alpha*.25f));
        // motion blur particles
        for(int i=0;i<3;i++){
            float ft=fmodf(t*2+i*.8f,1.5f);
            DrawCircleV({fmx+sinf(t*3+i*2)*3*S,fmy+3*S+ft*8*S},0.8f*S,ColorAlpha(c,alpha*(1-ft/1.5f)*.4f));
        }
        // engine glow
        DrawCircleV({fmx,fmy+4*S},2*S,ColorAlpha(VP_WHITE,alpha*.35f*gstP));
    } else if(sym=="BLK"){// Blink: elaborate warp portal with event horizon and energy arcs
        float blkP=sinf(t*3)*.5f+.5f;
        float rot=t*1.5f;
        // outer distortion ring
        DrawCircleLinesV({fmx,fmy},12*S,ColorAlpha(c,alpha*.25f));
        // swirling energy arcs
        for(int i=0;i<6;i++){
            float a=i*(PI/3)+rot;
            float r1=7*S, r2=11*S;
            float a2=a+0.4f+sinf(t*2+i)*.15f;
            DrawLineEx({fmx+cosf(a)*r1,fmy+sinf(a)*r1},{fmx+cosf(a2)*r2,fmy+sinf(a2)*r2},
                1.2f*S,ColorAlpha(c,alpha*(.5f+blkP*.2f)));
        }
        // mid ring with notch marks
        DrawCircleLinesV({fmx,fmy},8*S,ColorAlpha(c,alpha*.65f));
        for(int i=0;i<12;i++){
            float a=i*(PI/6)+rot*.5f;
            DrawLineEx({fmx+cosf(a)*7*S,fmy+sinf(a)*7*S},{fmx+cosf(a)*9*S,fmy+sinf(a)*9*S},
                0.6f*S,ColorAlpha(VP_WHITE,alpha*.3f));
        }
        // inner portal (dark center with blue edge)
        DrawCircleV({fmx,fmy},5.5f*S,ColorAlpha({3,1,12,255},alpha*.9f));
        DrawCircleLinesV({fmx,fmy},5.5f*S,ColorAlpha(c,alpha*.8f));
        // event horizon glow
        DrawCircleV({fmx,fmy},4*S,ColorAlpha(c,alpha*.12f*blkP));
        // central star
        for(int i=0;i<4;i++){
            float a=i*(PI/2)+t*2;
            DrawLineEx({fmx,fmy},{fmx+cosf(a)*3*S,fmy+sinf(a)*3*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.5f*blkP));
        }
        DrawCircleV({fmx,fmy},1.5f*S,ColorAlpha(VP_WHITE,alpha*.6f*blkP));
        // orbiting warp particles
        for(int i=0;i<4;i++){
            float a=rot*2+i*(PI/2);
            float or2=6.5f*S;
            DrawCircleV({fmx+cosf(a)*or2,fmy+sinf(a)*or2},1*S,ColorAlpha(c,alpha*.55f));
            DrawCircleV({fmx+cosf(a)*or2,fmy+sinf(a)*or2},.5f*S,ColorAlpha(VP_WHITE,alpha*.4f));
        }
    } else if(sym=="LCK"){// Luck: ornate four-leaf clover with golden aura and coins
        float lckP=sinf(t*2.5f)*.5f+.5f;
        // golden aura
        DrawCircleV({fmx,fmy},12*S,ColorAlpha(VP_GOLD,alpha*.04f*lckP));
        DrawCircleV({fmx,fmy},9*S,ColorAlpha(VP_GOLD,alpha*.06f*lckP));
        // four leaves with veins
        for(int i=0;i<4;i++){
            float a=i*(PI/2)+PI/4;
            float lx=fmx+cosf(a)*5*S, ly=fmy+sinf(a)*5*S;
            // leaf shadow
            DrawCircleV({lx+.5f*S,ly+.5f*S},4.5f*S,ColorAlpha({30,60,10,255},alpha*.25f));
            // leaf body
            DrawCircleV({lx,ly},4.2f*S,ColorAlpha(c,alpha*.75f));
            // leaf highlight
            DrawCircleV({lx-cosf(a)*1*S,ly-sinf(a)*1*S},2.5f*S,ColorAlpha({100,255,80,255},alpha*.35f));
            // leaf vein (center line)
            DrawLineEx({fmx,fmy},{lx+cosf(a)*2*S,ly+sinf(a)*2*S},0.7f*S,ColorAlpha({30,100,20,255},alpha*.45f));
            // side veins
            float va=a-0.4f, vb=a+0.4f;
            DrawLineEx({fmx+cosf(a)*3*S,fmy+sinf(a)*3*S},{lx+cosf(va)*2*S,ly+sinf(va)*2*S},
                0.4f*S,ColorAlpha({30,100,20,255},alpha*.3f));
            DrawLineEx({fmx+cosf(a)*3*S,fmy+sinf(a)*3*S},{lx+cosf(vb)*2*S,ly+sinf(vb)*2*S},
                0.4f*S,ColorAlpha({30,100,20,255},alpha*.3f));
        }
        // stem
        DrawLineEx({fmx,fmy},{fmx+1*S,fmy+9*S},1.2f*S,ColorAlpha({40,80,20,255},alpha*.6f));
        DrawLineEx({fmx+1*S,fmy+9*S},{fmx+3*S,fmy+10*S},0.8f*S,ColorAlpha({40,80,20,255},alpha*.45f));
        // center gem
        DrawCircleV({fmx,fmy},2.5f*S,ColorAlpha(VP_GOLD,alpha*.9f));
        DrawCircleV({fmx,fmy},1.5f*S,ColorAlpha(VP_YELLOW,alpha*.7f));
        DrawCircleV({fmx-.5f*S,fmy-.5f*S},.8f*S,ColorAlpha(VP_WHITE,alpha*.5f));
        // floating coin sparkles
        for(int i=0;i<3;i++){
            float ft=fmodf(t*.7f+i*1.1f,2.f);
            float fx=fmx+sinf(t+i*2.3f)*7*S;
            float fy=fmy-4*S-ft*4*S;
            float coinR=1*S;
            DrawCircleV({fx,fy},coinR,ColorAlpha(VP_GOLD,alpha*(1-ft/2)*.55f));
            DrawCircleV({fx-.3f*S,fy-.3f*S},coinR*.4f,ColorAlpha(VP_WHITE,alpha*(1-ft/2)*.3f));
        }
    } else if(sym=="SOL"){// Soul Draw: mystical vortex with soul wisps being pulled inward
        float solP=sinf(t*2)*.5f+.5f;
        // outer spiral arms
        for(int arm=0;arm<3;arm++){
            float baseA=arm*(2*PI/3)+t*.6f;
            for(int seg=0;seg<10;seg++){
                float ft=(float)seg/10.f;
                float a=baseA+ft*1.8f;
                float r=(11-ft*7)*S;
                float px=fmx+cosf(a)*r, py=fmy+sinf(a)*r;
                float dotR=(1.5f-ft*.8f)*S;
                DrawCircleV({px,py},dotR,ColorAlpha(c,alpha*(.6f-ft*.3f)));
                if(seg%3==0) DrawCircleV({px,py},dotR*.5f,ColorAlpha(VP_WHITE,alpha*.3f));
            }
        }
        // soul wisp particles being pulled in
        for(int i=0;i<5;i++){
            float wa=t*1.5f+i*(2*PI/5);
            float wr=(8-sinf(t*2+i)*3)*S;
            float wx=fmx+cosf(wa)*wr, wy=fmy+sinf(wa)*wr;
            // wisp body
            DrawCircleV({wx,wy},1.8f*S,ColorAlpha(c,alpha*.5f));
            DrawCircleV({wx,wy},1*S,ColorAlpha(VP_WHITE,alpha*.35f*solP));
            // wisp tail
            float ta=wa+0.5f;
            DrawLineEx({wx,wy},{wx+cosf(ta)*3*S,wy+sinf(ta)*3*S},0.6f*S,ColorAlpha(c,alpha*.25f));
        }
        // concentric pull rings
        for(int i=0;i<4;i++){
            float r=(3+i*2.5f)*S+sinf(t*3+i)*.5f*S;
            DrawCircleLinesV({fmx,fmy},r,ColorAlpha(c,alpha*(.5f-.1f*i)*solP));
        }
        // core
        DrawCircleV({fmx,fmy},3*S,ColorAlpha(c,alpha*.7f));
        DrawCircleV({fmx,fmy},2*S,ColorAlpha(VP_WHITE,alpha*.4f*solP));
        DrawCircleV({fmx,fmy},1*S,ColorAlpha(c,alpha*.9f));
        DrawNeonCircle({fmx,fmy},3.5f*S,c,alpha*.2f*solP);
    } else if(sym=="VDP"){// Void Pierce: ornate spear/lance with energy trail and runes
        float vdpP=sinf(t*3)*.5f+.5f;
        // energy trail behind lance
        for(int i=0;i<6;i++){
            float ft=(float)i/6.f;
            float tw=(3-ft*2.5f)*S;
            float tx=fmx-10*S+i*1.5f*S;
            DrawRectangle((int)(tx),(int)(fmy-tw/2),(int)(2*S),(int)tw,
                ColorAlpha(c,alpha*(.3f-.04f*i)));
        }
        // lance shaft
        DrawRectangle((int)(fmx-8*S),(int)(fmy-1.2f*S),(int)(18*S),(int)(2.4f*S),ColorAlpha(c,alpha*.8f));
        // shaft highlight
        DrawRectangle((int)(fmx-8*S),(int)(fmy-1.2f*S),(int)(18*S),(int)(1*S),ColorAlpha(VP_WHITE,alpha*.15f));
        // shaft rune marks
        for(int i=0;i<4;i++){
            float rx=fmx-5*S+i*4*S;
            DrawRectangle((int)(rx),(int)(fmy-0.5f*S),(int)(1.5f*S),(int)(1*S),ColorAlpha(VP_WHITE,alpha*.25f*vdpP));
        }
        // spearhead (elaborate arrowhead)
        DrawTriangle({fmx+10*S,fmy},{fmx+5*S,fmy-4*S},{fmx+5*S,fmy+4*S},ColorAlpha(c,alpha*.9f));
        // spearhead inner bright
        DrawTriangle({fmx+10*S,fmy},{fmx+6.5f*S,fmy-2*S},{fmx+6.5f*S,fmy+2*S},
            ColorAlpha(VP_WHITE,alpha*.25f));
        // spearhead edge glow
        DrawLineEx({fmx+10*S,fmy},{fmx+5*S,fmy-4*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.3f*vdpP));
        DrawLineEx({fmx+10*S,fmy},{fmx+5*S,fmy+4*S},0.8f*S,ColorAlpha(VP_WHITE,alpha*.25f));
        // barbs
        DrawTriangle({fmx+5*S,fmy-4*S},{fmx+3*S,fmy-6*S},{fmx+4*S,fmy-2*S},ColorAlpha(c,alpha*.65f));
        DrawTriangle({fmx+5*S,fmy+4*S},{fmx+3*S,fmy+6*S},{fmx+4*S,fmy+2*S},ColorAlpha(c,alpha*.65f));
        // pommel
        DrawCircleV({fmx-8*S,fmy},2*S,ColorAlpha(c,alpha*.7f));
        DrawCircleV({fmx-8*S,fmy},1*S,ColorAlpha(VP_WHITE,alpha*.4f*vdpP));
        // tip glow
        DrawCircleV({fmx+10*S,fmy},1.5f*S,ColorAlpha(VP_WHITE,alpha*.5f*vdpP));
        DrawNeonCircle({fmx+10*S,fmy},2.5f*S,c,alpha*.25f*vdpP);
    } else if(sym=="FLW"){// Flow: elegant wave with water droplets and chi current
        float flwP=sinf(t*2)*.5f+.5f;
        // flowing water background aura
        DrawCircleV({fmx,fmy},10*S,ColorAlpha(c,alpha*.04f));
        // main wave (thick sinusoidal with multiple layers)
        for(int layer=2;layer>=0;layer--){
            float yOff=layer*1.2f*S;
            float thick=(2.5f-layer*.6f)*S;
            float la=alpha*(.75f-.15f*layer);
            Color lc=layer==0?c:layer==1?ColorAlpha({100,255,200,255},1.f):ColorAlpha({50,200,150,255},1.f);
            Vector2 prev={fmx-12*S,fmy+sinf(t*3)*sz*.12f+yOff};
            for(int x2=1;x2<=16;x2++){
                float xf=(float)x2/16.f;
                float wx=fmx-12*S+x2*1.5f*S;
                float wy=fmy+sinf(xf*2.5f*PI+t*3+layer*.3f)*sz*.14f+yOff;
                DrawLineEx(prev,{wx,wy},thick,ColorAlpha(lc,la));
                prev={wx,wy};
            }
        }
        // wave crest highlights
        for(int i=0;i<3;i++){
            float xf=(i+.5f)/3.f;
            float wx=fmx-12*S+xf*24*S;
            float wy=fmy+sinf(xf*2.5f*PI+t*3)*sz*.14f;
            DrawCircleV({wx,wy-1*S},1*S,ColorAlpha(VP_WHITE,alpha*.3f*flwP));
        }
        // water droplets splashing
        for(int i=0;i<4;i++){
            float ft=fmodf(t*1.2f+i*.6f,2.f);
            float dx=fmx-6*S+i*4*S+sinf(t+i)*S;
            float dy=fmy-3*S-ft*3*S;
            DrawCircleV({dx,dy},(1.2f-ft*.4f)*S,ColorAlpha(c,alpha*(1-ft/2)*.5f));
            // droplet highlight
            DrawCircleV({dx-.3f*S,dy-.3f*S},.4f*S,ColorAlpha(VP_WHITE,alpha*(1-ft/2)*.35f));
        }
        // chi current sparkles along wave
        for(int i=0;i<5;i++){
            float xf=fmodf(t*.8f+i*.2f,1.f);
            float wx=fmx-12*S+xf*24*S;
            float wy=fmy+sinf(xf*2.5f*PI+t*3)*sz*.14f;
            DrawCircleV({wx,wy},.8f*S,ColorAlpha(VP_WHITE,alpha*.4f*(1-fabsf(xf-.5f)*2)));
        }
    } else if(sym=="NOV"){// Nova: elaborate supernova with expanding rings and debris
        float novP=sinf(t*2)*.5f+.5f;
        float expand=fmodf(t*.5f,2.f);
        // expanding shockwave rings
        for(int i=0;i<3;i++){
            float r=(4+expand*4+i*3)*S;
            float ra=alpha*(.35f-.1f*i)*(1-expand/2)*.8f;
            DrawCircleLinesV({fmx,fmy},r,ColorAlpha(c,ra));
            DrawCircleLinesV({fmx,fmy},r+.5f*S,ColorAlpha(VP_WHITE,ra*.3f));
        }
        // radiating light beams
        for(int i=0;i<12;i++){
            float a=i*(PI/6)+t*.2f;
            float r1=3*S, r2=(8+sinf(t*2+i*1.3f)*2)*S;
            Color rc=i%3==0?VP_GOLD:i%3==1?VP_PINK:c;
            DrawLineEx({fmx+cosf(a)*r1,fmy+sinf(a)*r1},{fmx+cosf(a)*r2,fmy+sinf(a)*r2},
                (i%3==0?1.5f:1.f)*S,ColorAlpha(rc,alpha*(.55f+novP*.15f)));
        }
        // debris fragments orbiting
        for(int i=0;i<6;i++){
            float da=t*.8f+i*(PI/3);
            float dr=(7+sinf(t+i)*2)*S;
            float dx=fmx+cosf(da)*dr, dy=fmy+sinf(da)*dr;
            DrawRectanglePro({dx,dy,2*S,1.5f*S},{1*S,.75f*S},da*180/PI+t*60,
                ColorAlpha({200,100,50,255},alpha*.5f));
        }
        // core supernova layers
        DrawCircleV({fmx,fmy},5*S,ColorAlpha(VP_PINK,alpha*.6f));
        DrawCircleV({fmx,fmy},3.5f*S,ColorAlpha(VP_GOLD,alpha*.7f*novP));
        DrawCircleV({fmx,fmy},2.2f*S,ColorAlpha(VP_YELLOW,alpha*.8f));
        DrawCircleV({fmx,fmy},1.2f*S,ColorAlpha(VP_WHITE,alpha*.85f*novP));
        DrawNeonCircle({fmx,fmy},5.5f*S,VP_GOLD,alpha*.25f*novP);
        // corona wisps
        for(int i=0;i<4;i++){
            float ca=t*1.5f+i*(PI/2);
            float cr=5*S;
            Vector2 cp={fmx+cosf(ca)*cr,fmy+sinf(ca)*cr};
            DrawCircleV(cp,1.5f*S,ColorAlpha(VP_GOLD,alpha*.35f*novP));
        }
    } else {// Fallback: ornate glowing rune circle
        float fbP=sinf(t*2.5f)*.5f+.5f;
        DrawCircleV({fmx,fmy},8*S,ColorAlpha(c,alpha*.08f));
        DrawCircleLinesV({fmx,fmy},7*S,ColorAlpha(c,alpha*.5f));
        DrawCircleLinesV({fmx,fmy},5*S,ColorAlpha(c,alpha*.35f));
        for(int i=0;i<6;i++){
            float a=i*(PI/3)+t*.5f;
            DrawLineEx({fmx+cosf(a)*5*S,fmy+sinf(a)*5*S},{fmx+cosf(a)*7*S,fmy+sinf(a)*7*S},
                0.8f*S,ColorAlpha(c,alpha*.5f));
        }
        DrawCircleV({fmx,fmy},3*S,ColorAlpha(c,alpha*.6f));
        DrawCircleV({fmx,fmy},1.5f*S,ColorAlpha(VP_WHITE,alpha*.5f*fbP));
    }
}


void DrawShop(Game& g,float t){
    // Category colors & labels
    struct CatInfo{ const char* id; const char* label; Color col; };
    CatInfo CATS[]={{"OFFENSE","⚔ OFFENSE",VP_RED},{"DEFENSE","🛡 DEFENSE",VP_LAVENDER},
                    {"CHI","◈ CHI",VP_CYAN},{"MOVE","⚡ MOVE",VP_YELLOW},
                    {"LUCK","★ LUCK",VP_GOLD},{"PRAYER","✦ PRAYER",VP_MINT}};

    // ── Animated background ──
    for(int x=0;x<SCREEN_W;x+=30) DrawLine(x,0,x,SCREEN_H,ColorAlpha(VP_DEEPPURP,.06f+.02f*sinf(t*.5f+x*.01f)));
    for(int y=0;y<SCREEN_H;y+=30) DrawLine(0,y,SCREEN_W,y,ColorAlpha(VP_DARKRED,.04f));
    for(int x=0;x<SCREEN_W;x+=60) for(int y=0;y<SCREEN_H;y+=60){
        float gp=sinf(t*2.2f+x*.04f+y*.03f)*.5f+.5f;
        if(gp>.72f) DrawCircleV({(float)x,(float)y},1.2f,ColorAlpha(VP_LAVENDER,gp*.12f));
    }

    // ── Outer frame ──
    DrawRectangle(10,3,SCREEN_W-20,SCREEN_H-6,ColorAlpha(VP_BLACK,.92f));
    DrawRectangleLines(10,3,SCREEN_W-20,SCREEN_H-6,ColorAlpha(VP_PINK,.65f));
    DrawRectangleLines(13,6,SCREEN_W-26,SCREEN_H-12,ColorAlpha(VP_PURPLE,.25f));
    float cpA=.75f+.20f*sinf(t*2.5f);
    for(int ci=0;ci<4;ci++){
        float cx2=(ci%2==0?10.f:(float)(SCREEN_W-10));
        float cy2=(ci<2?3.f:(float)(SCREEN_H-3));
        float sx2=(ci%2==0?1.f:-1.f)*12.f, sy2=(ci<2?1.f:-1.f)*12.f;
        DrawLineEx({cx2,cy2},{cx2+sx2,cy2},2.f,ColorAlpha(VP_PINK,cpA));
        DrawLineEx({cx2,cy2},{cx2,cy2+sy2},2.f,ColorAlpha(VP_PINK,cpA));
        DrawCircleV({cx2,cy2},3.f,ColorAlpha(VP_GOLD,.80f));
    }

    // ── Header ──
    DrawRectangle(13,6,SCREEN_W-26,40,ColorAlpha(VP_HOTPINK,.10f+.05f*sinf(t*2.f)));
    DrawRectangle(13,6,SCREEN_W-26,2,ColorAlpha(VP_PINK,.55f));
    float hpulse=.8f+.2f*sinf(t*3.f);
    float dx1=60.f,dy1=18.f;
    DrawLineV({dx1-5,dy1},{dx1,dy1-5},ColorAlpha(VP_HOTPINK,.70f*hpulse));
    DrawLineV({dx1,dy1-5},{dx1+5,dy1},ColorAlpha(VP_HOTPINK,.70f*hpulse));
    DrawLineV({dx1+5,dy1},{dx1,dy1+5},ColorAlpha(VP_HOTPINK,.70f*hpulse));
    DrawLineV({dx1,dy1+5},{dx1-5,dy1},ColorAlpha(VP_HOTPINK,.70f*hpulse));
    DrawCircleV({dx1,dy1},2.f,ColorAlpha(VP_GOLD,.85f));
    float dx2=(float)(SCREEN_W-60);
    DrawLineV({dx2-5,dy1},{dx2,dy1-5},ColorAlpha(VP_CYAN,.70f*hpulse));
    DrawLineV({dx2,dy1-5},{dx2+5,dy1},ColorAlpha(VP_CYAN,.70f*hpulse));
    DrawLineV({dx2+5,dy1},{dx2,dy1+5},ColorAlpha(VP_CYAN,.70f*hpulse));
    DrawLineV({dx2,dy1+5},{dx2-5,dy1},ColorAlpha(VP_CYAN,.70f*hpulse));
    DrawCircleV({dx2,dy1},2.f,ColorAlpha(VP_GOLD,.85f));
    CTGlow("UPGRADE  BAY",12,14,VP_WHITE,1.2f);

    // credits badge
    std::string credStr="CREDITS:  "+IStr(g.shopCredits);
    int credW=MeasureText(credStr.c_str(),9);
    int cbx=SCREEN_W/2-credW/2-10, cby=29;
    DrawRectangle(cbx,cby,credW+20,14,ColorAlpha(VP_BLACK,.88f));
    DrawRectangleLines(cbx,cby,credW+20,14,ColorAlpha(VP_GOLD,.80f));
    DrawRectangle(cbx,cby,credW+20,2,ColorAlpha(VP_GOLD,.45f));
    TGlow(credStr,SCREEN_W/2-credW/2,cby+3,9,VP_GOLD,1.f);

    // ── Item cards ──
    int cols=3, cw2=(SCREEN_W-72)/cols, ch2=120;
    for(int i=0;i<(int)g.shopItems.size()&&i<6;i++){
        auto& it=g.shopItems[i];
        int col=i%cols, row=i/cols;
        int cx=24+col*(cw2+4), cy=48+row*(ch2+5);

        bool cant=(g.shopCredits<it.price);
        bool isUpgrade=(it.level>1); // upgrading existing tree
        Color bc=it.sold?VP_MIDGRAY:cant?VP_DARKRED:it.col;
        Color tc=it.sold?VP_GRAY:cant?ColorAlpha(VP_GRAY,.60f):VP_WHITE;

        // find category color
        Color catCol=bc;
        for(auto& cat:CATS) if(it.category==cat.id){catCol=cat.col;break;}

        // card shadow + bg
        DrawRectangle(cx+2,cy+2,cw2,ch2,ColorAlpha(VP_BLACK,.65f));
        DrawRectangle(cx,cy,cw2,ch2,ColorAlpha(VP_BLACK,.90f));
        if(!it.sold){
            float cg=.06f+(cant?.0f:.03f*sinf(t*2.8f+i));
            DrawRectangle(cx,cy,cw2,ch2,ColorAlpha(bc,cg));
            // upgrade glow pulse (stronger for upgrade vs new)
            if(isUpgrade&&!cant) DrawRectangle(cx,cy,cw2,ch2,ColorAlpha(catCol,.04f+.03f*sinf(t*4+i)));
        }
        DrawRectangle(cx+1,cy+1,cw2-2,3,ColorAlpha(it.sold?VP_GRAY:bc,.14f));
        DrawRectangleLines(cx,cy,cw2,ch2,ColorAlpha(bc,it.sold?.22f:cant?.32f:.72f));
        DrawRectangleLines(cx+2,cy+2,cw2-4,ch2-4,ColorAlpha(bc,it.sold?.08f:.16f));
        // left accent stripe — category color
        DrawRectangle(cx,cy,2,ch2,ColorAlpha(catCol,it.sold?.18f:.60f));
        // upgrade indicator tag top-right
        if(isUpgrade&&!it.sold){
            int tagW=28;int tagX=cx+cw2-tagW-2;int tagY=cy+2;
            DrawRectangle(tagX,tagY,tagW,10,ColorAlpha(catCol,.25f));
            DrawRectangleLines(tagX,tagY,tagW,10,ColorAlpha(catCol,.55f));
            DrawText("UP",tagX+4,tagY+1,7,ColorAlpha(catCol,.90f));
        }

        // ── Sprite icon area ──
        int iconSz=32;
        int iconX=cx+cw2/2-iconSz/2, iconY=cy+4;
        DrawRectangle(iconX-2,iconY-2,iconSz+4,iconSz+4,ColorAlpha(catCol,.10f));
        DrawRectangleLines(iconX-2,iconY-2,iconSz+4,iconSz+4,ColorAlpha(catCol,it.sold?.15f:.30f));
        DrawUpgradeSprite(it,iconX,iconY,iconSz,t,it.sold?.35f:cant?.45f:1.f);

        // ── Name ──
        int nw2=MeasureText(it.name.c_str(),8);
        int nameX=cx+(cw2-nw2)/2;
        if(nameX<cx+2) nameX=cx+2;
        TGlow(it.name,nameX,cy+39,8,it.sold?VP_DARKGRAY:catCol,it.sold?.30f:.85f);

        // ── Level pip bar ──
        {
            int pipW=8,pipGap=3,nPips=it.maxLevel;
            int totalPW=nPips*(pipW+pipGap)-pipGap;
            int pipX=cx+(cw2-totalPW)/2, pipY=cy+51;
            for(int pi=0;pi<nPips;pi++){
                bool filled=(pi<it.level);
                Color pc=filled?catCol:VP_DARKGRAY;
                float pa=filled?(it.sold?.25f:.75f):.28f;
                DrawRectangle(pipX+pi*(pipW+pipGap),pipY,pipW,4,ColorAlpha(pc,pa));
                if(filled&&!it.sold)
                    DrawRectangle(pipX+pi*(pipW+pipGap),pipY,pipW,1,ColorAlpha(VP_WHITE,.30f));
            }
        }

        // ── Separator ──
        DrawLine(cx+6,cy+58,cx+cw2-6,cy+58,ColorAlpha(bc,it.sold?.14f:.28f));

        // ── Description (wrapped) ──
        std::string desc=it.desc;
        int dw3=MeasureText(desc.c_str(),6);
        if(dw3>cw2-10){
            size_t mid=desc.size()/2;
            while(mid>0&&desc[mid]!=' ')mid--;
            DrawText(desc.substr(0,mid).c_str(),cx+5,cy+61,6,ColorAlpha(VP_GRAY,.78f));
            DrawText(desc.substr(mid+1).c_str(),cx+5,cy+70,6,ColorAlpha(VP_GRAY,.78f));
        } else {
            DrawText(desc.c_str(),cx+(cw2-dw3)/2,cy+62,6,ColorAlpha(VP_GRAY,.78f));
        }

        // ── Category label ──
        {
            const char* catLabel="";
            for(auto& cat:CATS) if(it.category==cat.id){catLabel=cat.label;break;}
            int clW=MeasureText(catLabel,6);
            DrawText(catLabel,cx+(cw2-clW)/2,cy+80,6,ColorAlpha(catCol,it.sold?.20f:.40f));
        }

        // ── Price / EQUIPPED ──
        if(it.sold){
            int eby=cy+90;
            DrawRectangle(cx+6,eby,cw2-12,16,ColorAlpha(VP_MIDGRAY,.18f));
            DrawRectangleLines(cx+6,eby,cw2-12,16,ColorAlpha(VP_MIDGRAY,.38f));
            int ew=MeasureText("EQUIPPED",7);
            DrawText("EQUIPPED",cx+(cw2-ew)/2,eby+4,7,ColorAlpha(VP_GRAY,.60f));
        } else {
            std::string pt=IStr(it.price)+" CR";
            int pw=MeasureText(pt.c_str(),9);
            int bx3=cx+(cw2-pw-16)/2, by3=cy+90;
            Color btnCol=cant?VP_DARKRED:bc;
            DrawRectangle(bx3,by3,pw+16,18,ColorAlpha(VP_BLACK,.70f));
            DrawRectangle(bx3,by3,pw+16,18,ColorAlpha(btnCol,cant?.06f:(.10f+.05f*sinf(t*3+i))));
            if(!cant) DrawRectangle(bx3+1,by3+1,pw+14,2,ColorAlpha(VP_WHITE,.08f));
            DrawRectangleLines(bx3,by3,pw+16,18,ColorAlpha(btnCol,cant?.28f:.75f));
            DrawCircleV({(float)bx3,(float)by3},1.5f,ColorAlpha(btnCol,.55f));
            DrawCircleV({(float)(bx3+pw+16),(float)by3},1.5f,ColorAlpha(btnCol,.55f));
            TGlow(pt,bx3+(pw+16-pw)/2,by3+4,9,cant?ColorAlpha(VP_RED,.70f):VP_WHITE,cant?.40f:.95f);
        }

        // ── Index badge ──
        DrawText(("["+IStr(i+1)+"]").c_str(),cx+cw2-16,cy+3,6,ColorAlpha(VP_LAVENDER,.55f));
    }

    // ── Footer ──
    DrawLine(16,(int)(SCREEN_H-42),(int)(SCREEN_W-16),(int)(SCREEN_H-42),ColorAlpha(VP_DARKPURP,.40f));
    int fty=SCREEN_H-38;
    const char* hint="[1-6] buy   click a card";
    int hw=MeasureText(hint,7);
    DrawText(hint,(SCREEN_W-hw)/2,fty,7,ColorAlpha(VP_GRAY,.55f));

    // ── NEXT STAGE button ──
    int bw2=180,bh2=24,bx4=(SCREEN_W-bw2)/2,by4=SCREEN_H-28;
    float btnP=.75f+.25f*sinf(t*3.f);
    DrawRectangle(bx4+2,by4+2,bw2,bh2,ColorAlpha(VP_BLACK,.65f));
    DrawRectangle(bx4,by4,bw2,bh2,ColorAlpha(VP_BLACK,.88f));
    DrawRectangle(bx4,by4,bw2,bh2,ColorAlpha(VP_HOTPINK,.12f+.07f*sinf(t*3)));
    DrawRectangle(bx4+1,by4+1,bw2-2,3,ColorAlpha(VP_WHITE,.08f));
    DrawRectangleLines(bx4,by4,bw2,bh2,ColorAlpha(VP_HOTPINK,btnP*.85f));
    DrawRectangleLines(bx4+2,by4+2,bw2-4,bh2-4,ColorAlpha(VP_HOTPINK,btnP*.20f));
    DrawCircleV({(float)bx4,(float)by4},2.f,ColorAlpha(VP_GOLD,.80f));
    DrawCircleV({(float)(bx4+bw2),(float)by4},2.f,ColorAlpha(VP_GOLD,.80f));
    int tw2=MeasureText("[ NEXT STAGE ]",11);
    TGlow("[ NEXT STAGE ]",bx4+(bw2-tw2)/2,by4+6,11,VP_WHITE,1.f);
}


void DrawGameOver(Game& g,float t){
    Color ac=g.gameWon?VP_MINT:VP_RED;
    Color ac2=g.gameWon?VP_CYAN:VP_HOTPINK;
    float pulse=sinf(t*2.8f)*.5f+.5f;

    // Animated background — grid with shifting color
    for(int x=0;x<SCREEN_W;x+=28) DrawLine(x,0,x,SCREEN_H,ColorAlpha(ac,.04f+.02f*sinf(t*.4f+x*.008f)));
    for(int y=0;y<SCREEN_H;y+=28) DrawLine(0,y,SCREEN_W,y,ColorAlpha(ac2,.03f));
    // corner sparkles
    for(int ci=0;ci<4;ci++){
        float cx2=(ci%2==0?30.f:(float)(SCREEN_W-30));
        float cy2=(ci<2?30.f:(float)(SCREEN_H-30));
        float sp=sinf(t*3.f+ci*1.57f)*.5f+.5f;
        DrawCircleV({cx2,cy2},8.f*sp,ColorAlpha(ac,sp*.10f));
        DrawCircleV({cx2,cy2},3.f,ColorAlpha(ac,.35f+sp*.45f));
    }

    // Main panel with shadow
    DrawRectangle(52,25,SCREEN_W-104,SCREEN_H-50,ColorAlpha(VP_BLACK,.90f));
    // Outer border — double rule
    DrawRectangleLines(50,23,SCREEN_W-100,SCREEN_H-46,ColorAlpha(ac,.85f));
    DrawRectangleLines(53,26,SCREEN_W-106,SCREEN_H-52,ColorAlpha(ac2,.30f));
    // left accent stripe
    DrawRectangle(50,23,3,SCREEN_H-46,ColorAlpha(ac,pulse*.60f));
    // right accent stripe
    DrawRectangle(SCREEN_W-53,23,3,SCREEN_H-46,ColorAlpha(ac,.35f));
    // corner nail diamonds
    for(int ci=0;ci<4;ci++){
        float cx2=(ci%2==0?50.f:(float)(SCREEN_W-50));
        float cy2=(ci<2?23.f:(float)(SCREEN_H-23));
        DrawCircleV({cx2,cy2},4.5f,ColorAlpha(ac,.20f+pulse*.20f));
        DrawCircleV({cx2,cy2},2.5f,ColorAlpha(VP_GOLD,.80f));
    }

    // Header panel
    DrawRectangle(53,26,SCREEN_W-106,46,ColorAlpha(ac,.12f+.06f*sinf(t*2.5f)));
    DrawRectangle(53,26,SCREEN_W-106,2,ColorAlpha(ac,.90f));
    DrawRectangle(53,28,SCREEN_W-106,1,ColorAlpha(VP_WHITE,.06f));

    if(g.gameWon){
        CTGlow("VOID  VANQUISHED",38,18,VP_GOLD,1.4f);
        CTGlow("ALL SECTORS CLEARED",58,9,VP_MINT,.85f);
    } else {
        CTGlow("DESTROYED",38,18,ColorAlpha(VP_RED,.85f+.15f*sinf(t*4)),1.4f);
        CTGlow("YOUR SHIP WAS LOST IN THE VOID",58,9,VP_GRAY,.65f);
    }

    // Divider line with center diamond
    {
        int dy=82;
        DrawLine(70,dy,SCREEN_W/2-12,dy,ColorAlpha(ac,.32f));
        DrawLine(SCREEN_W/2+12,dy,SCREEN_W-70,dy,ColorAlpha(ac,.32f));
        DrawCircleV({(float)SCREEN_W/2,(float)dy},4.f,ColorAlpha(ac,.20f));
        DrawCircleV({(float)SCREEN_W/2,(float)dy},2.f,ColorAlpha(VP_GOLD,.90f));
    }

    // Score breakdown — with colored value bars
    int y=90;
    auto SR=[&](const char* label,const std::string& val,Color vc, float barPct=-1.f){
        // left label
        DrawText(label,76,y,9,ColorAlpha(VP_GRAY,.75f));
        // right value with glow
        int vw=MeasureText(val.c_str(),9);
        DrawText(val.c_str(),SCREEN_W-76-vw+1,y+1,9,ColorAlpha({0,0,0,255},.50f));
        DrawText(val.c_str(),SCREEN_W-76-vw,y,9,vc);
        // optional mini-bar
        if(barPct>=0.f&&barPct<=1.f){
            int bX=76, bY=y+12, bW=SCREEN_W-152, bH=2;
            DrawRectangle(bX,bY,bW,bH,ColorAlpha(VP_DARKGRAY,.40f));
            if(barPct>0.001f){
                DrawRectangle(bX,bY,(int)(bW*barPct),bH,ColorAlpha(vc,.70f));
                DrawRectangle(bX,bY,(int)(bW*barPct),1,ColorAlpha(VP_WHITE,.28f));
            }
            y+=16;
        } else {
            // dotted separator
            for(int dx=76;dx<SCREEN_W-76;dx+=6)
                DrawRectangle(dx,y+12,3,1,ColorAlpha(VP_DARKGRAY,.35f));
            y+=17;
        }
    };

    // Compute bar fractions
    float hsPct=(g.highScore>0)?(float)g.score/g.highScore:1.f; hsPct=hsPct>1.f?1.f:hsPct;
    float wavePct=(float)g.wave/std::max(1,(int)WAVES.size()-1);
    float killPct=g.totalKills>0?std::min(1.f,g.totalKills/200.f):0.f;
    float critPct=g.totalKills>0?std::min(1.f,(float)g.totalCrits/g.totalKills):0.f;

    SR("FINAL SCORE",   FmtScore(g.score),    VP_WHITE,    hsPct);
    SR("HIGH SCORE",    FmtScore(g.highScore), VP_GOLD,     1.0f);
    SR("STAGES CLEARED",IStr(g.wave)+"/"+IStr((int)WAVES.size()),VP_CYAN, wavePct);
    SR("ENEMIES KILLED", IStr(g.totalKills),   VP_MINT,     killPct);
    SR("CRITICAL HITS",  IStr(g.totalCrits),   VP_HOTPINK,  critPct);
    SR("BIGGEST CRIT",   FmtScore(g.biggestCrit),VP_YELLOW, -1.f);
    SR("UPGRADES",       IStr((int)g.upgrades.size())+"/"+IStr((int)ALL_UPGRADES.size()),VP_LAVENDER,-1.f);

    // Bottom divider
    {
        int dy=SCREEN_H-58;
        DrawLine(70,dy,SCREEN_W-70,dy,ColorAlpha(ac,.28f));
    }

    // CRT scan line sweep
    {
        float sw=fmodf(t*40.f,(float)SCREEN_H);
        DrawRectangle(50,(int)sw,SCREEN_W-100,1,ColorAlpha(ac,.04f));
        DrawRectangle(50,(int)sw+1,SCREEN_W-100,1,ColorAlpha(VP_WHITE,.02f));
    }

    // Blinking prompt
    if((int)(t*3)%2==0){
        const char* prompt=g.gameWon?"PRESS  ENTER  TO  PLAY  AGAIN":"PRESS  ENTER  OR  CLICK  TO  RETRY";
        int pw=MeasureText(prompt,8);
        DrawRectangle(SCREEN_W/2-pw/2-8,SCREEN_H-50,pw+16,14,ColorAlpha(VP_BLACK,.85f));
        DrawRectangle(SCREEN_W/2-pw/2-8,SCREEN_H-50,pw+16,1,ColorAlpha(ac,.30f));
        DrawRectangleLines(SCREEN_W/2-pw/2-8,SCREEN_H-50,pw+16,14,ColorAlpha(ac,.55f));
        CTGlow(prompt,SCREEN_H-47,8,VP_WHITE,1.f);
    }
}

// ══════════════════════════════════════════════════════════════
//  PAUSE SCREEN
// ══════════════════════════════════════════════════════════════

void DrawPause(Game& g,UIState& /*ui*/,float t,Vector2 msc,bool clicked){
    (void)g;
    float dt = GetFrameTime();

    // ── Dim overlay ──────────────────────────────────────────
    DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_BLACK,0.65f));

    float pulse = 0.75f + 0.25f * sinf(t * 2.2f);
    Color ac  = VP_CYAN;
    Color ac2 = VP_HOTPINK;

    bool fromTitle=(gPrevState==TITLE||gPrevState==SHOP);

    // ── Menu items definition ─────────────────────────────────
    struct MenuItem { const char* label; const char* hint; Color col; };
    MenuItem items[] = {
        { fromTitle ? "BACK"         : "RESUME",       nullptr,  VP_CYAN     },
        { gFullscreen ? "FULLSCREEN: ON" : "FULLSCREEN: OFF", "(F11)", VP_LAVENDER },
        { fromTitle ? "QUIT GAME"    : "QUIT TO TITLE", nullptr, VP_HOTPINK  },
    };
    const int nItems = 3;

    // ── Compact panel sizing ──────────────────────────────────
    const int itemH  = 26;   // tight row height
    const int hdrH   = 34;   // header block
    const int footH  = 16;   // footer hint
    const int padV   = 8;    // vertical inner padding
    int panH = hdrH + padV + nItems*itemH + padV + footH + 4;
    int panW = 220;
    int panX = (SCREEN_W-panW)/2;
    int panY = (SCREEN_H-panH)/2 - 8;

    // outer glow halos
    DrawRectangle(panX-6,panY-6,panW+12,panH+12,ColorAlpha(ac,0.03f));
    DrawRectangle(panX-3,panY-3,panW+6, panH+6, ColorAlpha(ac,0.06f));
    // body
    DrawRectangle(panX,panY,panW,panH,ColorAlpha(VP_BLACK,0.97f));
    // glass sheen
    DrawRectangle(panX+1,panY+1,panW-2,2,ColorAlpha(VP_WHITE,0.05f));
    // top/bottom accent bars
    DrawRectangle(panX,panY,panW,3,ColorAlpha(ac,pulse*0.90f));
    DrawRectangle(panX,panY+panH-3,panW,3,ColorAlpha(ac2,0.55f));
    // side bars
    DrawRectangle(panX,panY,2,panH,ColorAlpha(ac,0.75f));
    DrawRectangle(panX+panW-2,panY,2,panH,ColorAlpha(ac,0.50f));
    // border
    DrawRectangleLines(panX,panY,panW,panH,ColorAlpha(ac,pulse*0.85f));
    // corner nails
    for(int ci=0;ci<4;ci++){
        float cx2=(ci%2==0?(float)panX:(float)(panX+panW));
        float cy2=(ci<2?(float)panY:(float)(panY+panH));
        float sx2=(ci%2==0?1.f:-1.f)*7.f;
        float sy2=(ci<2?1.f:-1.f)*7.f;
        DrawLineEx({cx2,cy2},{cx2+sx2,cy2},2.f,ColorAlpha(ac,pulse*.80f));
        DrawLineEx({cx2,cy2},{cx2,cy2+sy2},2.f,ColorAlpha(ac,pulse*.80f));
        DrawCircleV({cx2,cy2},2.5f,ColorAlpha(VP_GOLD,0.85f));
    }

    // ── Header ────────────────────────────────────────────────
    const char* HDR = fromTitle ? "OPTIONS" : "PAUSED";
    int hdrW = MeasureText(HDR, 18);
    int hdrX = (SCREEN_W-hdrW)/2;
    // glow
    for(int g2=1;g2<=2;g2++)
        DrawText(HDR,hdrX-g2,panY+9,18,ColorAlpha(ac,0.08f*pulse));
    DrawText(HDR,hdrX+1,panY+10,18,ColorAlpha(VP_BLACK,0.55f));
    DrawText(HDR,hdrX,  panY+9, 18,ColorAlpha(ac,pulse));

    // thin divider under header
    int divY=panY+hdrH-2;
    DrawLine(panX+12,divY,panX+panW-12,divY,ColorAlpha(ac,0.25f));
    DrawCircleV({(float)SCREEN_W/2,(float)divY},2.f,ColorAlpha(VP_GOLD,0.80f));

    // ── Menu items ───────────────────────────────────────────
    int menuY = panY + hdrH + padV;

    // Hover detection
    for(int i=0;i<nItems;i++){
        Rectangle hr={(float)(panX+6),(float)(menuY+i*itemH),(float)(panW-12),(float)(itemH-2)};
        bool hovered=CheckCollisionPointRec(msc,hr);
        if(hovered){
            gPauseOption=i;
            gHoverTime[i]=std::min(1.f,gHoverTime[i]+dt*9.f);
        } else {
            gHoverTime[i]=std::max(0.f,gHoverTime[i]-dt*12.f);
        }
    }

    for(int i=0;i<nItems;i++){
        bool sel=(i==gPauseOption);
        float hv=gHoverTime[i];
        float rowY=(float)(menuY+i*itemH);
        Color ic=items[i].col;

        Rectangle selRect={(float)(panX+6),(float)rowY,(float)(panW-12),(float)(itemH-2)};

        if(sel){
            // fill
            DrawRectangleRec(selRect,ColorAlpha(ic,0.10f+0.05f*sinf(t*3.f)+hv*0.06f));
            // border passes
            for(int pass=0;pass<(int)(1+hv*2);pass++)
                DrawRectangleLinesEx(
                    {selRect.x-(float)pass,selRect.y-(float)pass,
                     selRect.width+(float)pass*2,selRect.height+(float)pass*2},
                    1.f,ColorAlpha(ic,(0.40f+hv*0.25f)*pulse/(pass+1)));
            // chevrons
            DrawText(">>",(int)(panX+8),(int)(rowY+4),8,ColorAlpha(ic,(0.7f+hv*0.3f)*pulse));
            DrawText("<<",(int)(panX+panW-22),(int)(rowY+4),8,ColorAlpha(ic,(0.7f+hv*0.3f)*pulse));
            // sparkle rays on hover
            if(hv>0.1f){
                float cx3=(float)SCREEN_W/2, cy3=rowY+itemH/2.f-1.f;
                for(int r2=0;r2<6;r2++){
                    float ang=r2*(PI/3.f)+t*1.8f;
                    DrawLineEx({cx3+cosf(ang)*3.f,cy3+sinf(ang)*3.f},
                               {cx3+cosf(ang)*(5.f+hv*8.f),cy3+sinf(ang)*(5.f+hv*8.f)},
                               1.f,ColorAlpha(ic,hv*0.5f*pulse));
                }
                DrawCircleV({cx3,cy3},1.8f+hv*1.5f,ColorAlpha(ic,hv*0.40f));
            }
        }

        // Label
        int lw=MeasureText(items[i].label,9);
        int lx=(SCREEN_W-lw)/2;
        float la=sel?(0.90f+hv*0.10f):(0.45f+hv*0.10f);
        if(sel){
            for(int g2=1;g2<=2;g2++)
                DrawText(items[i].label,lx-g2,(int)(rowY+5),9,ColorAlpha(ic,0.08f*pulse));
            DrawText(items[i].label,lx+1,(int)(rowY+6),9,ColorAlpha(VP_BLACK,0.50f));
            DrawText(items[i].label,lx,  (int)(rowY+5),9,ColorAlpha(ic,la*pulse));
        } else {
            DrawText(items[i].label,lx+1,(int)(rowY+6),9,ColorAlpha(VP_BLACK,0.35f));
            DrawText(items[i].label,lx,  (int)(rowY+5),9,ColorAlpha(ic,la));
        }
        // sub-hint inline (smaller, right of label)
        if(items[i].hint){
            int hw2=MeasureText(items[i].hint,6);
            DrawText(items[i].hint,lx+lw+4,(int)(rowY+7),6,
                ColorAlpha(VP_GRAY,sel?0.50f:0.22f));
        }

        // Click flash
        if(clicked&&CheckCollisionPointRec(msc,selRect))
            DrawRectangleRec(selRect,ColorAlpha(VP_WHITE,0.18f));
    }

    // ── Footer hint ───────────────────────────────────────────
    {
        const char* hint="UP/DN  ENTER  ESC";
        int hw2=MeasureText(hint,5);
        int fy=panY+panH-footH;
        DrawLine(panX+10,fy-2,panX+panW-10,fy-2,ColorAlpha(ac,0.15f));
        DrawText(hint,(SCREEN_W-hw2)/2,fy+2,5,ColorAlpha(VP_GRAY,0.42f));
    }
}


// ══════════════════════════════════════════════════════════════
//  PER-WAVE PLANET THEMES
// ══════════════════════════════════════════════════════════════
int main(){
    srand((unsigned)time(nullptr));
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_W * SCALE, SCREEN_H * SCALE, "OUT OF ORBIT");
    SetExitKey(KEY_NULL);   // disable ESC closing the window — handled by pause menu
    SetTargetFPS(60);
    gFullscreen = false;

    // Load crisp TTF fonts for UI panels
    // Noto Sans Bold for headers/big values, JetBrains Mono Bold for mono data
    const char* fontBigPath  = "/usr/share/fonts/noto/NotoSans-Bold.ttf";
    const char* fontMonoPath = "/usr/share/fonts/TTF/JetBrainsMonoNerdFontMono-Bold.ttf";
    gFontBig  = LoadFontEx(fontBigPath,  96, nullptr, 0);
    gFontMono = LoadFontEx(fontMonoPath, 64, nullptr, 0);
    // Additional fallback paths
    if(gFontBig.texture.id == 0) gFontBig = LoadFontEx("/usr/share/fonts/liberation/LiberationSans-Bold.ttf", 96, nullptr, 0);
    if(gFontMono.texture.id == 0) gFontMono = LoadFontEx("/usr/share/fonts/liberation/LiberationMono-Bold.ttf", 64, nullptr, 0);
    // Fallback: if loading failed, raylib returns default font
    gFontsLoaded = (gFontBig.texture.id > 0 && gFontMono.texture.id > 0);
    if(gFontsLoaded){
        SetTextureFilter(gFontBig.texture,  TEXTURE_FILTER_BILINEAR);
        SetTextureFilter(gFontMono.texture, TEXTURE_FILTER_BILINEAR);
    }
    RenderTexture2D rt=LoadRenderTexture(SCREEN_W,SCREEN_H);
    SetTextureFilter(rt.texture,TEXTURE_FILTER_POINT);
    // Bloom: half-res buffer for glow accumulation
    RenderTexture2D bloomRT=LoadRenderTexture(SCREEN_W/2,SCREEN_H/2);
    SetTextureFilter(bloomRT.texture,TEXTURE_FILTER_BILINEAR);

    Game g={}; g.highScore=0; g.state=TITLE; g.cameraPos={0,0};
    // Title stars
    Color tc[]={VP_WHITE,VP_PINK,VP_CYAN,VP_LAVENDER,VP_PURPLE};
    for(int i=0;i<180;i++){Star s;s.pos={RandF(0,(float)WORLD_W),RandF(0,(float)WORLD_H)};s.brightness=RandF(.15f,1);s.radius=RandF(.4f,2);s.twinkleSpeed=RandF(.6f,4);s.twinkleOffset=RandF(0,2*PI);s.layer=RandI(0,2);s.tint=tc[RandI(0,4)];g.stars.push_back(s);}
    Color nc[]={VP_DARKPURP,VP_PINK,VP_CYAN,VP_PURPLE,VP_DEEPBLUE};
    for(int i=0;i<12;i++){Nebula n;n.pos={RandF(0,(float)SCREEN_W*2),RandF(0,(float)SCREEN_H*2)};n.radius=RandF(60,130);n.alpha=RandF(.05f,.14f);n.driftOffset=RandF(0,2*PI);n.col=nc[RandI(0,4)];g.nebulas.push_back(n);}

    float bossWarnTimer=0; std::string bossWarnText; bool bossDeadHandled=false;

    while(!WindowShouldClose() && !gQuitRequested){
        float dt=GetFrameTime(); if(dt>.05f)dt=.05f;
        float t=GetTime();
        Vector2 rawM=GetMousePosition();
        // map mouse into game-texture space accounting for letterbox
        Rectangle gr2=GameRect();
        float gscale=gr2.width/SCREEN_W;
        Vector2 msc={(rawM.x-gr2.x)/gscale,(rawM.y-gr2.y)/gscale};
        Vector2 mw={0,0};
        bool clicked=IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool mheld  =IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        Vector2 sc={(float)SCREEN_W/2,(float)SCREEN_H/2};
        g.ambientPulse=fmodf(g.ambientPulse+dt*.8f,2*PI);

        // Hide OS cursor while playing, show it on all menus
        if(g.state==PLAYING) HideCursor(); else ShowCursor();

        // ── GLOBAL HOTKEYS ──────────────────────────────────────────
        // F11: toggle fullscreen from anywhere
        if(IsKeyPressed(KEY_F11)){
            gFullscreen = !gFullscreen;
            if(gFullscreen){
                int mon = GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(mon), GetMonitorHeight(mon));
                ToggleFullscreen();
            } else {
                ToggleFullscreen();
                SetWindowSize(1280, 800);
                SetWindowPosition(
                    (GetMonitorWidth(0)-1280)/2,
                    (GetMonitorHeight(0)-800)/2);
            }
        }
        // ESC: open options/pause from PLAYING, TITLE, SHOP — close if already paused
        if(IsKeyPressed(KEY_ESCAPE)){
            if(g.state==PLAYING||g.state==TITLE||g.state==SHOP){
                gPrevState=g.state; g.state=PAUSED; gPauseOption=0;
                for(int i=0;i<3;i++) gHoverTime[i]=0.f;
            }
            else if(g.state==PAUSED){ g.state=gPrevState; }
        }

        // ── PAUSED STATE UPDATE ──────────────────────────────────────
        if(g.state==PAUSED){
            bool fromTitleCtx2=(gPrevState==TITLE||gPrevState==SHOP);
            // Keyboard navigation
            if(IsKeyPressed(KEY_UP)||IsKeyPressed(KEY_W))   gPauseOption=(gPauseOption+2)%3;
            if(IsKeyPressed(KEY_DOWN)||IsKeyPressed(KEY_S)) gPauseOption=(gPauseOption+1)%3;

            // Panel geometry — must match DrawPause exactly
            const int itemH2=26, hdrH2=34, padV2=8, footH2=16;
            int panH2=hdrH2+padV2+3*itemH2+padV2+footH2+4;
            int panW2=220;
            int panX2=(SCREEN_W-panW2)/2, panY2=(SCREEN_H-panH2)/2-8;
            int menuY2=panY2+hdrH2+padV2;
            // Mouse hover → update selection
            for(int i=0;i<3;i++){
                Rectangle hr={(float)(panX2+6),(float)(menuY2+i*itemH2),(float)(panW2-12),(float)(itemH2-2)};
                if(CheckCollisionPointRec(msc,hr)) gPauseOption=i;
            }
            Rectangle selHit={(float)(panX2+6),(float)(menuY2+gPauseOption*itemH2),(float)(panW2-12),(float)(itemH2-2)};
            bool confirm=IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE)||
                         (clicked&&CheckCollisionPointRec(msc,selHit));
            if(confirm){
                if(gPauseOption==0){
                    // Resume / Back
                    g.state=gPrevState;
                }
                else if(gPauseOption==1){
                    // Toggle fullscreen
                    gFullscreen=!gFullscreen;
                    if(gFullscreen){
                        int mon=GetCurrentMonitor();
                        SetWindowSize(GetMonitorWidth(mon),GetMonitorHeight(mon));
                        ToggleFullscreen();
                    } else {
                        ToggleFullscreen();
                        SetWindowSize(1280,800);
                        SetWindowPosition((GetMonitorWidth(0)-1280)/2,(GetMonitorHeight(0)-800)/2);
                    }
                }
                else if(gPauseOption==2){
                    bool fromTitleCtx=(gPrevState==TITLE||gPrevState==SHOP);
                    if(fromTitleCtx){
                        gQuitRequested = true;
                    } else {
                        int hs=g.highScore; int ss=g.selectedShip; int tier=g.shipUpgradeTier; int rc=g.runCount; int bw=g.bestWave;
                        g={}; g.highScore=hs; g.selectedShip=ss; g.shipUpgradeTier=tier; g.runCount=rc; g.bestWave=bw;
                        g.state=TITLE;
                        gPrevState=TITLE;
                        Color tc[]={VP_WHITE,VP_PINK,VP_CYAN,VP_LAVENDER,VP_PURPLE};
                        for(int i=0;i<180;i++){Star s;s.pos={RandF(0,(float)WORLD_W),RandF(0,(float)WORLD_H)};s.brightness=RandF(.15f,1);s.radius=RandF(.4f,2);s.twinkleSpeed=RandF(.6f,4);s.twinkleOffset=RandF(0,2*PI);s.layer=RandI(0,2);s.tint=tc[RandI(0,4)];g.stars.push_back(s);}
                        Color nc[]={VP_DARKPURP,VP_PINK,VP_CYAN,VP_PURPLE,VP_DEEPBLUE};
                        for(int i2=0;i2<12;i2++){Nebula n;n.pos={RandF(0,(float)SCREEN_W*2),RandF(0,(float)SCREEN_H*2)};n.radius=RandF(60,130);n.alpha=RandF(.05f,.14f);n.driftOffset=RandF(0,2*PI);n.col=nc[RandI(0,4)];g.nebulas.push_back(n);}
                        gForceResetTitle=true;
                        for(int i=0;i<3;i++) gHoverTime[i]=0.f;
                        clicked=false; // consume click so TITLE handler doesn't re-fire it this frame
                    }
                }
            }
        }

        // ── STATE UPDATE ──
        if(g.state==TITLE){
            // Ship selection: number keys 1-4
            for(int i=0;i<4;i++) if(IsKeyPressed(KEY_ONE+i)) g.selectedShip=i;
            if(IsKeyPressed(KEY_ENTER)||IsKeyPressed(KEY_SPACE)){
                int ss=g.selectedShip; InitGame(g); g.selectedShip=ss;
            }
            // Q = quit to desktop from title
            if(IsKeyPressed(KEY_Q)) gQuitRequested=true;
            // Mouse: must be hovering over a card to click it
            if(clicked){
                int pad2=10, gap2=6, ncards2=4;
                int cw5=(SCREEN_W-pad2*2-gap2*(ncards2-1))/ncards2;
                int hdrY2=(22+38+26)+2+18; // cardTop
                int cardH2=SCREEN_H-hdrY2-16;
                for(int i=0;i<ncards2;i++){
                    int cx5=pad2+i*(cw5+gap2);
                    if(CheckCollisionPointRec(msc,{(float)cx5,(float)hdrY2,(float)cw5,(float)cardH2})){
                        if(i==g.selectedShip){
                            int ss=g.selectedShip; InitGame(g); g.selectedShip=ss;
                        } else {
                            g.selectedShip=i;
                        }
                        break;
                    }
                }
            }
        }
        else if(g.state==SHOP){
            for(int i=0;i<(int)g.shopItems.size();i++){
                if(IsKeyPressed(KEY_ONE+i)){auto& it=g.shopItems[i];if(!it.sold&&g.shopCredits>=it.price){
                    g.shopCredits-=it.price;it.sold=true;
                    Upgrade u=it;u.owned=true;
                    std::string base=UpgradeBaseId(u.id);
                    // Remove previous level of same tree before adding new one
                    g.upgrades.erase(std::remove_if(g.upgrades.begin(),g.upgrades.end(),
                        [&](const Upgrade& x){return UpgradeBaseId(x.id)==base;}),g.upgrades.end());
                    g.upgrades.push_back(u);
                    if(base=="shield") g.shieldHp=std::max(g.shieldHp,u.level);
                    RefreshUpgradeCache(g);
                }}
            }
            auto AdvWave=[&](){
                if(g.wave > g.bestWave) g.bestWave = g.wave;  // track deepest wave reached
                g.wave++; if(g.wave>=(int)WAVES.size()){g.gameWon=true;g.state=GAMEOVER;}
                else{g.state=PLAYING;g.waveTarget=WAVES[g.wave].target;g.waveKills=0;g.spawnInterval=WAVES[g.wave].spawnTicks;g.bossPhase=g.bossSpawned=false;g.boss.active=false;g.enemies.clear();for(auto& _b:g.bullets) _b={};for(auto& _b:g.bossBullets) _b={};for(auto& _b:g.bossBullets) _b={};g.spawnTimer=0;bossDeadHandled=false;bossWarnTimer=0;SpawnWavePlanets(g,g.wave);
                // Only show transition on world changes (every 4 waves)
                if((g.wave % 4 == 0) || g.wave == 0) g.waveTransTimer=3.5f; else g.waveTransTimer=0;
                SpawnEasterEggs(g.wave,g.playerPos);}
            };
            if(clicked){
                int cw2=(SCREEN_W-76)/3,ch2=112;
                for(int i=0;i<(int)g.shopItems.size();i++){
                    auto& it=g.shopItems[i];int col=i%3,row=i/3;int cx=38+col*(cw2+4),cy=56+row*(ch2+4);
                    if(CheckCollisionPointRec(msc,{(float)cx,(float)cy,(float)cw2,(float)ch2})&&!it.sold&&g.shopCredits>=it.price){
                        g.shopCredits-=it.price;it.sold=true;
                        Upgrade u=it;u.owned=true;
                        std::string base=UpgradeBaseId(u.id);
                        g.upgrades.erase(std::remove_if(g.upgrades.begin(),g.upgrades.end(),
                            [&](const Upgrade& x){return UpgradeBaseId(x.id)==base;}),g.upgrades.end());
                        g.upgrades.push_back(u);
                        if(base=="shield") g.shieldHp=std::max(g.shieldHp,u.level);
                        RefreshUpgradeCache(g);
                    }}
                int bw=168,bx2=(SCREEN_W-bw)/2,by2=SCREEN_H-38;
                if(CheckCollisionPointRec(msc,{(float)bx2,(float)by2,(float)bw,22}))AdvWave();
            }
            if(IsKeyPressed(KEY_ENTER))AdvWave();
            if(IsKeyPressed(KEY_SPACE))AdvWave();
        }
        else if(g.state==GAMEOVER){ if(clicked||IsKeyPressed(KEY_ENTER))g.state=TITLE; }
        else if(g.state==PLAYING){
            // DEBUG: Press Z to skip to next wave (testing purposes)
            if(IsKeyPressed(KEY_Z)){
                g.bossPhase=true; g.shopCredits=180+g.wave*45+(int)(g.score*.03f);
                SpawnPopup(g,{g.playerPos.x,g.playerPos.y-110},">> SKIP TO SHOP <<",VP_GOLD,16);
                OpenShop(g);
            }
            
            // ── PRAYER CYCLING (RIGHT CLICK) ──
            if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
                g.prayer=(PrayerType)((g.prayer+1)%3);
                g.prayerFxTimer=.3f;
                SpawnRing(g,g.playerPos,PrayerColor(g.prayer),40,.35f);
            }
            g.prayerFxTimer=std::max(0.f,g.prayerFxTimer-dt);

            // Resonance streak decay
            g.resonanceTimer=std::max(0.f,g.resonanceTimer-dt);
            if(g.resonanceTimer<=0) g.resonanceStreak=0;

            // ── TRANCE ACTIVATION (SPACE) ──
            if(IsKeyPressed(KEY_SPACE)&&g.tranceGauge>=100.f&&!g.tranceActive&&g.tranceCooldown<=0){
                g.tranceActive=true; g.tranceTimer=6.f; g.tranceGauge=0; SpawnSectorRing(g.playerPos,VP_HOTPINK,280,.55f); TriggerAberration(.2f);
                g.invincTimer=6.f;
                SpawnPopup(g,g.playerPos,"** TRANCE MODE **",VP_PINK,20);
                SpawnRing(g,g.playerPos,VP_PINK,220,1.f);
                SpawnRing(g,g.playerPos,VP_PURPLE,160,.8f);
                SpawnParticles(g,g.playerPos,VP_PINK,28,155,true);
                g.screenShake=.3f; g.novaFlash=.35f;
            }
            if(g.tranceActive){
                g.tranceTimer-=dt;
                if(g.tranceTimer<=0){
                    g.tranceActive=false; g.tranceCooldown=8.f; g.invincTimer=0;
                    SpawnPopup(g,g.playerPos,"TRANCE FADED",VP_LAVENDER,14);
                    SpawnRing(g,g.playerPos,VP_PURPLE,80,.5f);
                }
            }
            g.tranceCooldown=std::max(0.f,g.tranceCooldown-dt);
            // Trance gauge slowly drains when NOT being filled
            if(!g.tranceActive&&g.tranceCooldown<=0)
                g.tranceGauge=std::max(0.f,g.tranceGauge-.5f*dt);

            // ── PER-FRAME SKILL EFFECTS ──────────────────────────────
            // Brawler: REGEN — slowly recover 1 HP every 6s in combat
            if(g.ucache.sk_regen && g.selectedShip==1){
                g.regenTimer+=dt;
                if(g.regenTimer>=6.f){
                    g.regenTimer=0.f;
                    if(g.lives<6){ g.lives++; SpawnPopup(g,{g.playerPos.x,g.playerPos.y-55},"REGEN +1",VP_CORAL,11); }
                }
            }
            // Titan: VOID BARRIER — auto-shield recharges every 8s
            if(g.ucache.sk_barrier && g.selectedShip==3){
                if(g.shieldHp<=0){
                    g.barrierCooldown+=dt;
                    if(g.barrierCooldown>=8.f){
                        g.barrierCooldown=0.f; g.shieldHp=1;
                        SpawnPopup(g,{g.playerPos.x,g.playerPos.y-55},"BARRIER RECHARGED",VP_YELLOW,11);
                        SpawnRing(g,g.playerPos,VP_YELLOW,50.f,0.4f);
                    }
                } else { g.barrierCooldown=0.f; }
            }
            // Interceptor: MOMENTUM — speed boosts bullet damage
            if(g.ucache.sk_momentum && g.selectedShip==0){
                float pspd2=sqrtf(g.playerVel.x*g.playerVel.x+g.playerVel.y*g.playerVel.y);
                g.momentumDmgMult=1.f+std::min(1.5f, pspd2/PLAYER_SPEED);
            } else { g.momentumDmgMult=1.f; }
            // Phantom: CLOAK timer
            if(g.ucache.sk_cloak && g.selectedShip==2 && g.cloakActive){
                g.cloakTimer-=dt;
                if(g.cloakTimer<=0.f){ g.cloakActive=false; g.cloakTimer=0.f; }
            }
            // Titan: GRAVITY WELL — pull enemies toward player each frame
            if(g.ucache.sk_gravity && g.selectedShip==3){
                for(auto& en:g.enemies){
                    if(!en.active) continue;
                    float dx=g.playerPos.x-en.pos.x, dy=g.playerPos.y-en.pos.y;
                    float d=sqrtf(dx*dx+dy*dy);
                    if(d>20.f && d<320.f){
                        float pull=80.f*(1.f-d/320.f);
                        en.vel.x+=dx/d*pull*dt;
                        en.vel.y+=dy/d*pull*dt;
                    }
                }
            }

            // ── MOVEMENT ──
            float moveSpd=PLAYER_SPEED*(g.ucache.afterburn?1.6f:1);
            
            // SHIP-SPECIFIC MOVEMENT BONUSES:
            // INTERCEPTOR (0): +30% speed (agile)
            // BRAWLER (1): -10% speed, +1 extra life (tank)
            // PHANTOM (2): +50% speed (fast but fragile)
            // TITAN (3): -20% speed (slow but tanky)
            if(g.selectedShip == 0) moveSpd *= 1.45f;     // Interceptor: +45% — twitchy fast needle
            else if(g.selectedShip == 1) moveSpd *= 1.05f; // Brawler: +5% — heavy but not sluggish
            else if(g.selectedShip == 2) moveSpd *= 1.70f; // Phantom: +70% — scarily fast
            else if(g.selectedShip == 3) moveSpd *= 0.92f; // Titan: -8% — still noticeably weighty
            Vector2 inp={0,0};
            if(IsKeyDown(KEY_W)||IsKeyDown(KEY_UP))   inp.y-=1;
            if(IsKeyDown(KEY_S)||IsKeyDown(KEY_DOWN)) inp.y+=1;
            if(IsKeyDown(KEY_A)||IsKeyDown(KEY_LEFT)) inp.x-=1;
            if(IsKeyDown(KEY_D)||IsKeyDown(KEY_RIGHT))inp.x+=1;
            float ilen=sqrtf(inp.x*inp.x+inp.y*inp.y); if(ilen>.01f){inp.x/=ilen;inp.y/=ilen;}
            g.thrusterOn=(ilen>.01f);
            g.dashCooldown=std::max(0.f,g.dashCooldown-dt);
            g.dashTimer   =std::max(0.f,g.dashTimer-dt);
            if(IsKeyPressed(KEY_LEFT_SHIFT)&&g.ucache.warpcore&&g.dashCooldown<=0&&ilen>.01f){
                g.dashVel={inp.x*650,inp.y*650}; g.dashTimer=.13f; g.dashCooldown=1.5f;
                SpawnRing(g,g.playerPos,VP_CYAN,65); SpawnParticles(g,g.playerPos,VP_CYAN,12,130,true); g.warpFlash=.25f; TriggerAberration(.18f);
            }
            // Interceptor AFTERBURNER skill dash (faster cooldown, directional burst)
            if(IsKeyPressed(KEY_LEFT_SHIFT)&&g.ucache.sk_dash&&!g.ucache.warpcore&&g.dashCooldown<=0&&ilen>.01f){
                g.dashVel={inp.x*520,inp.y*520}; g.dashTimer=.10f; g.dashCooldown=0.9f;
                SpawnRing(g,g.playerPos,VP_CYAN,45); SpawnParticles(g,g.playerPos,VP_CYAN,8,100,true);
            }
            if(g.dashTimer>0){ g.playerVel=g.dashVel; }
            else if(g.dashVel.x!=0||g.dashVel.y!=0){ g.playerVel={0,0}; g.dashVel={0,0}; }
            else{
                g.playerVel.x+=inp.x*640*dt; g.playerVel.y+=inp.y*640*dt;
                g.playerVel.x-=g.playerVel.x*5.2f*dt; g.playerVel.y-=g.playerVel.y*5.2f*dt;
                float sp2=sqrtf(g.playerVel.x*g.playerVel.x+g.playerVel.y*g.playerVel.y);
                if(sp2>moveSpd){g.playerVel.x=g.playerVel.x/sp2*moveSpd;g.playerVel.y=g.playerVel.y/sp2*moveSpd;}
            }
            g.playerPos.x+=g.playerVel.x*dt; g.playerPos.y+=g.playerVel.y*dt;
            // No world boundary — player moves freely
            // Spawn trail while moving OR while coasting to a stop (velocity-based)
            // This gives a natural "fade out" as the ship decelerates rather than cutting instantly
            float pspd=sqrtf(g.playerVel.x*g.playerVel.x+g.playerVel.y*g.playerVel.y);
            if(g.thrusterOn||g.dashTimer>0||pspd>12.f) AddEngineTrail(g);
            // Slower decay (2.2/s = ~0.45s lifetime) so trail lingers naturally
            for(auto& tp:g.engineTrail) tp.life-=dt*2.2f;
            while(!g.engineTrail.empty()&&g.engineTrail.back().life<=0) g.engineTrail.pop_back();
            g.screenShake=std::max(0.f,g.screenShake-dt*2.8f);
            g.camShakeX=(g.screenShake>0)?RandF(-5,5)*g.screenShake:0;
            g.camShakeY=(g.screenShake>0)?RandF(-5,5)*g.screenShake:0;
            float tcx = g.playerPos.x - SCREEN_W/2.f;
            float tcy = g.playerPos.y - SCREEN_H/2.f;
            g.cameraPos.x+=(tcx-g.cameraPos.x)*11*dt; g.cameraPos.y+=(tcy-g.cameraPos.y)*11*dt;
            mw={msc.x+g.cameraPos.x, msc.y+g.cameraPos.y};
            g.playerAngle=atan2f(mw.y-g.playerPos.y,mw.x-g.playerPos.x);
            if(mheld)Shoot(g,mw);
            // idle weapon timer — resets whenever player is actively firing
            if(mheld) g.idleShootTimer=0;
            else {
                g.idleShootTimer+=dt;
                // Phantom sniper: first bullet of next burst gets 4x — reset when not firing
                if(g.ucache.sk_sniper && g.selectedShip==2) g.sniperReady=true;
            }
            g.shootCooldown=std::max(0,g.shootCooldown-1);
            g.heat=std::max(0.f,g.heat-1.80f); // cosmetic only — thruster glow
            // ── HEAT: drains when not firing (guns cool down passively) ──
            if(g.heatVentTimer > 0.f){
                // Forced vent: rapid cooldown, guns locked
                g.heatVentTimer = std::max(0.f, g.heatVentTimer - dt);
                g.chiRingTimer  = g.heatVentTimer; // keep HUD sync'd
                float ventRate  = 80.f * dt; // cools fast during vent
                g.chiGauge      = std::max(0.f, g.chiGauge - ventRate);
                // Visual: heat steam particles while venting
                if((int)(GetTime()*20)%3==0)
                    SpawnParticles(g,g.playerPos,{255,140,60,255},2,40,true);
            } else {
                // Normal passive cooling when not shooting
                float coolRate = mheld ? 0.f : 28.f * dt; // only cools when trigger released
                g.chiGauge = std::max(0.f, g.chiGauge - coolRate);
            }
            if(g.comboTimer>0){g.comboTimer-=85*dt;if(g.comboTimer<=0)g.combo=1;}
            g.invincTimer=std::max(0.f,g.invincTimer-dt);
            g.warpFlash=std::max(0.f,g.warpFlash-dt*4); g.novaFlash=std::max(0.f,g.novaFlash-dt*2); g.bossRumble=std::max(0.f,g.bossRumble-dt*2);
            g.nearMissFlash=std::max(0.f,g.nearMissFlash-dt*3.5f);
            g.progressJuice=std::max(0.f,g.progressJuice-dt*8.f);
            g.lastKillFlash=std::max(0.f,g.lastKillFlash-dt*12.f);
            UpdateHyper(t,dt,g); UpdateDust(dt,g); UpdateLightning(dt); UpdateSectorRings(dt);
            UpdatePrayerEffects(g,dt);
            for(auto& h:g.hazards)h.angle+=h.spinSpeed;
            UpdateShootingStars(g,dt);

            // ── SPAWN ──
            int waveIdx = std::min(g.wave,(int)WAVES.size()-1);
            WaveDef& wd=WAVES[waveIdx];
            g.spawnTimer++;
            if(!g.bossPhase&&g.spawnTimer>=g.spawnInterval&&g.waveKills<g.waveTarget){g.spawnTimer=0;SpawnEnemy(g);}
            bool ae=false; for(auto& e:g.enemies)if(e.active){ae=true;break;}
            if(!g.bossPhase&&!ae&&g.waveKills>=g.waveTarget){
                if(wd.hasBoss&&!g.bossSpawned){g.bossPhase=g.bossSpawned=true;bossWarnTimer=2.5f;bossWarnText="*** "+std::string(BOSS_DEFS[std::min(wd.bossIdx,3)].name)+" APPEARS ***";g.bossRumble=2.5f;}
                else if(!wd.hasBoss){g.bossPhase=true;g.shopCredits=130+g.wave*75+(int)(g.score*.035f);
                    SpawnPopup(g,{g.playerPos.x,g.playerPos.y-110},">> SHOP OPEN <<",VP_GOLD,16);
                    SpawnRing(g,g.playerPos,VP_GOLD,80,.5f);
                    OpenShop(g);}
            }
            if(bossWarnTimer>0){bossWarnTimer-=dt;if(bossWarnTimer<=0&&g.bossPhase&&!g.boss.active)SpawnBoss(g,wd.bossIdx);}
            if(g.bossPhase&&g.bossSpawned&&!g.boss.active&&bossWarnTimer<=0&&!bossDeadHandled){bossDeadHandled=true;g.shopCredits=220+g.wave*95+(int)(g.score*.04f);
                SpawnPopup(g,{g.playerPos.x,g.playerPos.y-110},">> SHOP OPEN <<",VP_GOLD,16);
                SpawnRing(g,g.playerPos,VP_GOLD,80,.5f);
                OpenShop(g);}

            // ── BULLETS ──
            bool pier=g.ucache.piercing,rico=g.ucache.ricochet,expl=g.ucache.explosive;
            for(auto& b:g.bullets){
                if(!b.active)continue;

                // ── SOUL SWARM homing: steer toward nearest enemy ──
                if(b.homing && b.prayerType==2){
                    float bestDist2=99999.f*99999.f; int bestIdx=-1;
                    for(int ei=0;ei<(int)g.enemies.size();ei++){
                        auto& e=g.enemies[ei]; if(!e.active)continue;
                        float ddx=e.pos.x-b.pos.x,ddy=e.pos.y-b.pos.y;
                        float dist2=ddx*ddx+ddy*ddy;
                        if(dist2<bestDist2){bestDist2=dist2;bestIdx=ei;}
                    }
                    if(bestIdx>=0){
                        auto& te=g.enemies[bestIdx];
                        float ddx=te.pos.x-b.pos.x,ddy=te.pos.y-b.pos.y;
                        float dd=sqrtf(ddx*ddx+ddy*ddy);
                        if(dd>1.f){
                            float tx=ddx/dd,ty=ddy/dd;
                            float spd=sqrtf(b.vel.x*b.vel.x+b.vel.y*b.vel.y);
                            float turnRate=b.homingStr*(1.f+0.015f*(160.f-b.life)); // sharpens as it ages
                            b.vel.x+=(tx*spd-b.vel.x)*turnRate*dt;
                            b.vel.y+=(ty*spd-b.vel.y)*turnRate*dt;
                            // re-normalise speed
                            float ns=sqrtf(b.vel.x*b.vel.x+b.vel.y*b.vel.y);
                            if(ns>1.f){b.vel.x=b.vel.x/ns*spd;b.vel.y=b.vel.y/ns*spd;}
                        }
                    }
                }

                // ── SOLAR FLARE burst countdown ──
                if(b.prayerType==0 && b.burstTimer>0.f){
                    b.burstTimer-=1.f;
                    if(b.burstTimer<=0.f){
                        // Time-out detonation: area blast
                        SpawnRing(g,b.pos,VP_ORANGE,90,.5f);
                        SpawnRing(g,b.pos,VP_RED,60,.35f);
                        SpawnParticles(g,b.pos,VP_ORANGE,12,100,true);
                        SpawnParticles(g,b.pos,VP_YELLOW,6,70,false);
                        g.screenShake=.15f;
                        for(auto& en:g.enemies){
                            if(!en.active)continue;
                            float ex2=en.pos.x-b.pos.x,ey2=en.pos.y-b.pos.y;
                            if(ex2*ex2+ey2*ey2<6400.f) HitEnemy(g,en,b.damage);
                        }
                        if(g.boss.active){float bx2=g.boss.pos.x-b.pos.x,by2=g.boss.pos.y-b.pos.y;if(bx2*bx2+by2*by2<8100.f)HitBoss(g,2);}
                        b.active=false; continue;
                    }
                }

                b.pos.x+=b.vel.x*dt; b.pos.y+=b.vel.y*dt; b.life--;
                if(b.life<=0){b.active=false;continue;}

                bool hit=false;
                bool isPiercing=(pier || b.prayerType==1); // Void Lance always pierces
                bool isSolarFlare=(b.prayerType==0);

                for(auto& e:g.enemies){
                    if(!e.active)continue;
                    float dx=b.pos.x-e.pos.x,dy=b.pos.y-e.pos.y;
                    float hitR = isSolarFlare ? e.radius+6.f : e.radius+3.f;
                    float hitR2 = hitR * hitR;
                    if(dx*dx+dy*dy<hitR2){
                        if(expl){
                            SpawnRing(g,b.pos,VP_RED,80);SpawnParticles(g,b.pos,VP_RED,14,100,true);
                            g.screenShake=.2f;
                            for(auto& en:g.enemies){if(!en.active)continue;float ex2=en.pos.x-b.pos.x,ey2=en.pos.y-b.pos.y;if(ex2*ex2+ey2*ey2<6400)HitEnemy(g,en,b.damage);}
                        } else if(isSolarFlare){
                            // Contact detonation
                            SpawnRing(g,b.pos,VP_ORANGE,80,.45f);
                            SpawnRing(g,b.pos,VP_RED,50,.30f);
                            SpawnParticles(g,b.pos,VP_ORANGE,10,95,true);
                            g.screenShake=.12f;
                            for(auto& en:g.enemies){if(!en.active)continue;float ex2=en.pos.x-b.pos.x,ey2=en.pos.y-b.pos.y;if(ex2*ex2+ey2*ey2<6400.f)HitEnemy(g,en,b.damage);}
                            if(g.boss.active){float bx2=g.boss.pos.x-b.pos.x,by2=g.boss.pos.y-b.pos.y;if(bx2*bx2+by2*by2<8100.f)HitBoss(g,2);}
                            b.active=false; b.burstTimer=0.f; hit=true;
                        } else {
                            int hitDmg=(int)(b.damage*b.chainDmgMult);
                            HitEnemy(g,e,hitDmg);

                            // ── SPIRIT CHAIN JUMP ────────────────────────
                            if(b.prayerType==2 && b.chainJumps>0 && e.active==false){
                                // Find nearest OTHER active enemy within 220px
                                Enemy* target=nullptr;
                                float  bestD=1e9f;
                                for(auto& en2:g.enemies){
                                    if(!en2.active||&en2==&e) continue;
                                    float dx2=en2.pos.x-e.pos.x,dy2=en2.pos.y-e.pos.y;
                                    float d2=dx2*dx2+dy2*dy2;
                                    if(d2<bestD && d2<220.f*220.f){bestD=d2;target=&en2;}
                                }
                                if(target){
                                    float cdx=target->pos.x-e.pos.x;
                                    float cdy=target->pos.y-e.pos.y;
                                    float cd=sqrtf(cdx*cdx+cdy*cdy);
                                    Bullet chain{};
                                    chain.pos={e.pos.x,e.pos.y};
                                    chain.vel={(cdx/cd)*300.f,(cdy/cd)*300.f};
                                    chain.life=60; chain.active=true;
                                    chain.col=VP_LAVENDER; chain.glowR=8.f;
                                    chain.prayerType=2; chain.damage=b.damage;
                                    chain.chainJumps=b.chainJumps-1;
                                    chain.chainDmgMult=b.chainDmgMult*1.5f;
                                    chain.shipType=g.selectedShip;
                                    chain.homing=true; chain.homingStr=3.5f;
                                    BulletPoolPush(g.bullets,g.bulletRover,chain);
                                    SpawnRing(g,e.pos,VP_PURPLE,28.f,0.25f);
                                }
                                b.active=false; hit=true;
                            }
                        }
                        if(!isPiercing && !isSolarFlare){b.active=false;hit=true;}
                        SpawnParticles(g,b.pos,VP_YELLOW,3,60,true);
                        if(hit)break;
                    }
                }
                if(b.active&&g.boss.active){
                    float dx=b.pos.x-g.boss.pos.x,dy=b.pos.y-g.boss.pos.y;
                    float bossHitR = g.boss.radius + 3.f;
                    if(dx*dx+dy*dy<bossHitR*bossHitR){
                        if(isSolarFlare){
                            SpawnRing(g,b.pos,VP_ORANGE,80,.45f);SpawnParticles(g,b.pos,VP_ORANGE,8,80,true);
                            HitBoss(g,2); b.active=false;
                        } else {
                            HitBoss(g,1); if(!isPiercing)b.active=false;
                        }
                        SpawnParticles(g,b.pos,VP_YELLOW,3,60,true);
                    }
                }
            }
            // (inactive bullets stay stale but are always fully re-initialised on reuse)

            // ── ENEMIES ──
            for(auto& e:g.enemies){
                if(!e.active)continue;
                float dx=g.playerPos.x-e.pos.x, dy=g.playerPos.y-e.pos.y;
                float dist=sqrtf(dx*dx+dy*dy);
                if(dist>.01f){float sp2=ENEMY_DEFS[e.type].speed*(1+g.wave*.07f);float steer=(e.type==E_FAST)?3.5f:1.8f;e.vel.x+=(dx/dist*sp2-e.vel.x)*steer*dt;e.vel.y+=(dy/dist*sp2-e.vel.y)*steer*dt;}
                e.pos.x+=e.vel.x*dt;e.pos.y+=e.vel.y*dt;e.angle+=e.spinSpeed;
                // no world wrap — enemies move freely
                float px=e.pos.x-g.playerPos.x,py=e.pos.y-g.playerPos.y;
                float pdist=sqrtf(px*px+py*py);
                float playerRadius = (g.wave == 2) ? PLAYER_RADIUS_L2 : PLAYER_RADIUS;  // Wider on level 2
                // near-miss sparks — enemy grazes within 22px without actually hitting
                if(pdist < e.radius+22.f && pdist > e.radius+playerRadius+0.5f && g.nearMissFlash<=0){
                    g.nearMissFlash = 0.35f;
                    SpawnParticles(g,g.playerPos,VP_YELLOW,5,70,true);
                }
                if(pdist<e.radius+playerRadius){
                    // Brawler SHOCKWAVE: ramming triggers AoE instead of taking damage
                    if(g.ucache.sk_shockwave && g.selectedShip==1){
                        for(auto& en2:g.enemies){
                            if(!en2.active||&en2==&e) continue;
                            float dx2=en2.pos.x-e.pos.x,dy2=en2.pos.y-e.pos.y;
                            if(dx2*dx2+dy2*dy2<120.f*120.f) HitEnemy(g,en2,4);
                        }
                        SpawnRing(g,e.pos,VP_ORANGE,100.f,0.5f);
                        SpawnParticles(g,e.pos,VP_ORANGE,10,110,true);
                        e.active=false;
                    } else {
                        e.active=false;
                        // Phantom CLOAK: taking damage triggers 2s invisibility
                        if(g.ucache.sk_cloak && g.selectedShip==2 && !g.cloakActive){
                            g.cloakActive=true; g.cloakTimer=2.f;
                            SpawnPopup(g,{g.playerPos.x,g.playerPos.y-55},"CLOAKED",VP_PURPLE,12);
                            SpawnRing(g,g.playerPos,VP_PURPLE,45.f,0.4f);
                        }
                        PlayerHit(g); if(g.lives<=0)g.state=GAMEOVER;
                    }
                }
                if(e.hp<=0)e.active=false;

                // ── ENEMY SHOOTING ──
                e.shootTimer-=dt;
                if(e.type==E_VOID && e.shootTimer<=0.f && pdist<420.f){
                    // VOIDLANCER: fires 3-way burst of void bolts at player
                    float shootInterval=1.8f-g.wave*0.04f;
                    shootInterval=std::max(0.7f,shootInterval);
                    e.shootTimer=shootInterval;
                    float baseA=atan2f(dy,dx); // toward player
                    float spreads[]={-0.20f,0.f,0.20f};
                    for(float sp3:spreads){
                        float fa=baseA+sp3;
                        float bspd=200.f+g.wave*5.f;
                        Bullet b{}; b.active=true; b.life=120;
                        b.pos={e.pos.x+cosf(fa)*e.radius,e.pos.y+sinf(fa)*e.radius};
                        b.vel={cosf(fa)*bspd,sinf(fa)*bspd};
                        b.col=VP_HOTPINK; b.glowR=5; b.damage=1; b.prayerType=-1;
                        BossBulletPoolPush(g.bossBullets, g.bossBulletRover, b); // reuse boss bullet vector for hostile shots
                    }
                }
                if(e.type==E_TANK && e.shootTimer<=0.f && pdist<380.f){
                    // DREADNAUGHT: fires single heavy slug forward
                    float shootInterval=3.2f-g.wave*0.06f;
                    shootInterval=std::max(1.4f,shootInterval);
                    e.shootTimer=shootInterval+(float)(rand()%100)/100.f; // stagger
                    float fa=atan2f(dy,dx);
                    Bullet b{}; b.active=true; b.life=140;
                    b.pos={e.pos.x+cosf(fa)*e.radius,e.pos.y+sinf(fa)*e.radius};
                    b.vel={cosf(fa)*(170.f+g.wave*4.f),sinf(fa)*(170.f+g.wave*4.f)};
                    b.col=VP_PURPLE; b.glowR=7; b.damage=2; b.prayerType=-1;
                    BossBulletPoolPush(g.bossBullets, g.bossBulletRover, b);
                }
            }
            g.enemies.erase(std::remove_if(g.enemies.begin(),g.enemies.end(),[](Enemy& e){return !e.active;}),g.enemies.end());

            // ── BOSS ──
            if(g.boss.active){
                float dx=g.playerPos.x-g.boss.pos.x, dy=g.playerPos.y-g.boss.pos.y, dist=sqrtf(dx*dx+dy*dy);
                float baseSpd=BOSS_DEFS[std::min(wd.bossIdx,3)].speed*(1+g.wave*.025f);
                float enraged=(g.boss.phase==1);
                float spd=baseSpd*(enraged?1.7f:1.f);

                // ── Movement: orbit + dash ──
                g.boss.dashCooldown=std::max(0.f,g.boss.dashCooldown-dt);
                if(g.boss.dashTimer>0){
                    // Active dash
                    g.boss.pos.x+=g.boss.dashVel.x*dt;
                    g.boss.pos.y+=g.boss.dashVel.y*dt;
                    g.boss.dashTimer-=dt;
                } else {
                    // Orbit player at preferred distance, varying with phase
                    float preferDist=enraged?140.f:200.f;
                    g.boss.orbitAngle+=dt*(enraged?1.6f:0.9f);
                    float targetX=g.playerPos.x+cosf(g.boss.orbitAngle)*preferDist;
                    float targetY=g.playerPos.y+sinf(g.boss.orbitAngle)*preferDist;
                    float tdx=targetX-g.boss.pos.x, tdy=targetY-g.boss.pos.y;
                    float tdist=sqrtf(tdx*tdx+tdy*tdy);
                    if(tdist>.5f){ g.boss.pos.x+=tdx/tdist*spd*dt; g.boss.pos.y+=tdy/tdist*spd*dt; }
                    // Trigger dash toward player periodically
                    if(g.boss.dashCooldown<=0){
                        float dashSpd=enraged?520.f:340.f;
                        float dashInterval=enraged?2.0f:3.5f;
                        g.boss.dashTimer=0.28f; g.boss.dashCooldown=dashInterval;
                        if(dist>.01f){ g.boss.dashVel={dx/dist*dashSpd, dy/dist*dashSpd}; }
                        SpawnRing(g,g.boss.pos,g.boss.col,40,.2f);
                        g.screenShake=std::max(g.screenShake,.12f);
                    }
                }
                // boss moves freely — no world clamping

                // ── Shooting patterns ──
                g.boss.shootTimer-=dt;
                float shootInterval=enraged?0.45f:0.85f;
                if(g.boss.shootTimer<=0){
                    g.boss.shootTimer=shootInterval;
                    g.boss.shootPattern=(g.boss.shootPattern+1)%5;
                    float ang=atan2f(dy,dx); // angle toward player — wait, dy/dx is already player-boss direction
                    // Recalc: dx/dy is player-boss
                    float toPlayer=atan2f(g.playerPos.y-g.boss.pos.y, g.playerPos.x-g.boss.pos.x);
                    float bspd=enraged?260.f:190.f;

                    auto SpawnBossBullet=[&](float a, float spd2, Color col){
                        Bullet b{}; b.pos=g.boss.pos; b.vel={cosf(a)*spd2, sinf(a)*spd2};
                        b.life=180; b.active=true; b.col=col; b.glowR=5;
                        BulletPoolPush(g.bullets, g.bulletRover, b); // reuse player bullets vector — but mark differently
                        // Actually push to enemies instead: we'll add a boss bullet vector
                    };

                    // Boss fires into enemy-side bullets — let's use a dedicated approach:
                    // We spawn them as particles that damage player on contact (via hazards)
                    // Actually simplest: push them into a local boss_bullets vector in Game
                    // For now let's just spawn them directly and check collision in player-hit code
                    // We'll add boss_bullets to game struct — instead, reuse Bullet but with negative life-flag
                    // SIMPLEST: spawn as Bullet with active=true into a new g.bossBullets vector

                    int pat=g.boss.shootPattern;
                    if(pat==0||pat==3){
                        // Aimed burst: 3 shots in tight spread at player
                        for(int i=-1;i<=1;i++){
                            float a2=toPlayer+i*0.18f;
                            Bullet b{}; b.pos=g.boss.pos; b.vel={cosf(a2)*bspd, sinf(a2)*bspd};
                            b.life=220; b.active=true; b.col=g.boss.col; b.glowR=6;
                            BossBulletPoolPush(g.bossBullets, g.bossBulletRover, b);
                        }
                    } else if(pat==1){
                        // Ring burst: 8 shots in circle
                        int n=enraged?12:8;
                        for(int i=0;i<n;i++){
                            float a2=toPlayer+i*(2*PI/n);
                            Bullet b{}; b.pos=g.boss.pos; b.vel={cosf(a2)*bspd*.8f, sinf(a2)*bspd*.8f};
                            b.life=200; b.active=true; b.col=VP_HOTPINK; b.glowR=5;
                            BossBulletPoolPush(g.bossBullets, g.bossBulletRover, b);
                        }
                        SpawnRing(g,g.boss.pos,g.boss.col,60,.3f);
                    } else if(pat==2){
                        // Spiral: 3 arms of 2 shots each
                        for(int arm=0;arm<3;arm++){
                            float a2=g.boss.angle+arm*(2*PI/3);
                            for(int s=0;s<2;s++){
                                float sr2=bspd*(0.7f+s*.35f);
                                Bullet b{}; b.pos=g.boss.pos; b.vel={cosf(a2)*sr2, sinf(a2)*sr2};
                                b.life=240; b.active=true; b.col=VP_PURPLE; b.glowR=5;
                                BossBulletPoolPush(g.bossBullets, g.bossBulletRover, b);
                            }
                        }
                    } else if(pat==4){
                        // Cross sweep: aimed + perpendicular
                        float perp=toPlayer+PI*.5f;
                        float s2=bspd*1.1f;
                        for(float a2:{toPlayer,perp,perp+PI}){
                            Bullet b{}; b.pos=g.boss.pos; b.vel={cosf(a2)*s2, sinf(a2)*s2};
                            b.life=200; b.active=true; b.col=VP_CYAN; b.glowR=6;
                            BossBulletPoolPush(g.bossBullets, g.bossBulletRover, b);
                        }
                    }
                    g.screenShake=std::max(g.screenShake,.06f);
                }

                // ── Enraged: extra random spray ──
                if(enraged){
                    static float sprayT=0; sprayT-=dt;
                    if(sprayT<=0){ sprayT=RandF(.15f,.3f);
                        float a2=RandF(0,2*PI);
                        Bullet b{}; b.pos=g.boss.pos; b.vel={cosf(a2)*220.f, sinf(a2)*220.f};
                        b.life=160; b.active=true; b.col=VP_RED; b.glowR=4;
                        BossBulletPoolPush(g.bossBullets, g.bossBulletRover, b);
                    }
                }

                g.boss.angle+=enraged?.08f:.03f;
                g.boss.angle2-=enraged?.13f:.05f;

                // Boss body contact (dist already uses toroidal delta)
                if(dist<g.boss.radius+PLAYER_RADIUS){ PlayerHit(g); if(g.lives<=0)g.state=GAMEOVER; }
            }

            // ── BOSS BULLETS update + player collision ──
            for(auto& b:g.bossBullets){
                if(!b.active) continue;
                b.pos.x+=b.vel.x*dt; b.pos.y+=b.vel.y*dt; b.life--;
                if(b.life<=0){b.active=false;continue;}
                float px=b.pos.x-g.playerPos.x, py=b.pos.y-g.playerPos.y;
                if(px*px+py*py<576.f){ b.active=false; PlayerHit(g); if(g.lives<=0)g.state=GAMEOVER; }
            }
            // (inactive boss bullets stay stale; always fully re-initialised on reuse)

            // ── PARTICLES ──
            for(auto& p:g.particles){p.prevPos=p.pos;p.pos.x+=p.vel.x*dt;p.pos.y+=p.vel.y*dt;p.vel.x*=.96f;p.vel.y*=.96f;p.life-=dt/p.maxLife*.85f;p.angle+=p.angVel*dt;}
            g.particles.erase(std::remove_if(g.particles.begin(),g.particles.end(),[](Particle& p){return p.life<=0;}),g.particles.end());

            // ── PICKUPS ──
            float magR=g.ucache.magnet?160:45;
            float softR=g.ucache.magnet?320:120; // soft attract radius — gold slowly drifts toward player
            for(auto& pk:g.pickups){
                if(!pk.active)continue; pk.pos.y+=pk.vy*dt; pk.spin+=dt*2; pk.life--;
                float dx=pk.pos.x-g.playerPos.x,dy=pk.pos.y-g.playerPos.y,dist=sqrtf(dx*dx+dy*dy);
                if(dist<magR){
                    // hard magnet snap
                    pk.pos.x+=(g.playerPos.x-pk.pos.x)*9*dt;
                    pk.pos.y+=(g.playerPos.y-pk.pos.y)*9*dt;
                } else if(dist<softR && dist>0.1f){
                    // soft drift: ramps from 0 at softR edge to ~1.8 speed at magR boundary
                    float t01 = 1.f - (dist-magR)/(softR-magR); // 0 far, 1 near
                    float strength = t01*t01 * 1.8f; // quadratic ease-in
                    float nx=dx/dist, ny=dy/dist;
                    pk.pos.x -= nx*strength*dt*60.f;
                    pk.pos.y -= ny*strength*dt*60.f;
                }
                if(dist<16){g.score+=pk.value;if(g.score>g.highScore)g.highScore=g.score;SpawnPopup(g,pk.pos,"+"+IStr(pk.value),VP_YELLOW,10);pk.active=false;}
                if(pk.life<=0)pk.active=false;
            }
            g.pickups.erase(std::remove_if(g.pickups.begin(),g.pickups.end(),[](Pickup& p){return !p.active;}),g.pickups.end());
            for(auto& pt:g.popups)pt.life-=dt;
            g.popups.erase(std::remove_if(g.popups.begin(),g.popups.end(),[](PopupText& p){return p.life<=0;}),g.popups.end());
        }

        // ══════ DRAW ══════
        BeginTextureMode(rt);
        // ── Per-wave sky base color — each level has a UNIQUE, saturated sky ──
        // These are deep, rich background colors that make every wave unmistakeable.
        {
            static const Color waveSky[28]={
                {18,  2, 28,255},  // 0  CRIMSON NEBULA    — deep blood-magenta
                {30,  5,  8,255},  // 1  EMBER FIELDS      — furnace black-red
                { 2, 10, 45,255},  // 2  OCEAN WORLD       — abyssal dark blue
                { 0,  5, 22,255},  // 3  ABYSSAL TRENCH    — crushing deep black-blue
                { 4, 22,  2,255},  // 4  TOXIC SWAMP       — poisonous black-green
                {12, 18,  1,255},  // 5  SPORE WASTES      — sickly olive dark
                {16,  0, 40,255},  // 6  VOID RIFT         — violent deep ultraviolet
                { 5,  1, 18,255},  // 7  PHANTOM GATE      — near-void indigo black
                { 0, 20, 22,255},  // 8  CRYSTAL CAVERNS   — dark teal ice
                {28, 18,  0,255},  // 9  RESONANCE CORE    — smoldering amber dark
                {35, 12,  0,255},  // 10 STELLAR FORGE     — deep forge orange-black
                {30, 24,  0,255},  // 11 SHARD TEMPEST     — hot gold dark
                {30,  0, 20,255},  // 12 QUANTUM DEPTHS    — deep magenta-void
                { 1,  0,  3,255},  // 13 EVENT HORIZON     — ABSOLUTE BLACK — near nothing
                {40,  4,  0,255},  // 14 SOLAR INFERNO     — scorched red-black
                {40, 32,  8,255},  // 15 CORONA BREACH     — white-gold burn haze
                { 6,  6, 14,255},  // 16 OBSIDIAN WASTES   — cold charcoal dark
                {14,  5,  1,255},  // 17 ASHEN CITADEL     — smoldering ash black
                { 2,  0,  8,255},  // 18 THE VOID          — deeper void
                { 0,  0,  1,255},  // 19 VOID SINGULARITY  — ABSOLUTE VOID
                {20,  0, 14,255},  // 20 CORONA BREACH+    — dark carnage
                {12,  0, 30,255},  // 21 OBSIDIAN WASTES+  — violet darkness
                { 3,  2,  8,255},  // 22 PHANTOM DEPTHS    — obsidian void
                {25, 10,  0,255},  // 23 ASHEN CITADEL+    — ember siege
                { 1,  0,  4,255},  // 24 THE VOID+         — deeper still
                { 0,  0,  0,255},  // 25 VOID SINGULARITY+ — total darkness
                {10,  0, 18,255},  // 26 DYING VIOLET      — last flicker
                { 0,  0,  0,255},  // 27 ABSOLUTE VOID     — nothing
            };
            int wi=(g.state==PLAYING||g.state==PAUSED)?std::min(g.wave,27):0;
            ClearBackground(waveSky[wi]);
        }
        DrawNebulas(g,t);
        DrawStars(g,t);
        DrawShootingStars(g);
        DrawAurora(t, g.wave);
        DrawDust();
        DrawHyper();
        DrawLightning();

        if(g.state==TITLE)                                               { DrawTitle(g,gUI,t,msc,clicked); }
        else if(g.state==SHOP)                                           { DrawShop(g,t); }
        else if(g.state==PAUSED&&(gPrevState==TITLE||gPrevState==SHOP))  { (gPrevState==SHOP?DrawShop(g,t):DrawTitle(g,gUI,t,msc,clicked)); DrawPause(g,gUI,t,msc,clicked); }
        else if(g.state==GAMEOVER) DrawGameOver(g,t);
        else if(g.state==PLAYING||g.state==PAUSED){
            // Easter eggs rendered in background — behind the grid tiles, with planets
            DrawEasterEggs(g,t, 1.0f);
            { static int _gf=0; _gf++; if(_gf%2==0) DrawWorldGrid(g,t); }
            // Strong per-wave color identity wash — makes every level feel DIFFERENT
            {
                static const Color waveWash[28]={
                    {180, 10, 80,255},  // 0  crimson — hot magenta bloom
                    {220, 40,  5,255},  // 1  ember — deep orange fire
                    {  5, 60,200,255},  // 2  ocean — saturated blue
                    {  0, 80,120,255},  // 3  abyssal — bioluminescent teal
                    { 50,180, 10,255},  // 4  toxic — acid green
                    { 90,120,  5,255},  // 5  spore — sickly olive
                    {100, 10,200,255},  // 6  void rift — deep violet
                    { 60, 20,140,255},  // 7  phantom — dark indigo
                    {  5,180,170,255},  // 8  crystal — teal ice
                    {200,140, 10,255},  // 9  resonance — amber gold
                    {220, 90,  5,255},  // 10 forge — orange forge
                    {240,200, 10,255},  // 11 shard — hot gold
                    {180,  5,100,255},  // 12 quantum — deep magenta
                    {100, 20,180,255},  // 13 event horizon — void violet (barely visible)
                    {220, 30,  0,255},  // 14 inferno — blazing red
                    {255,220, 80,255},  // 15 corona — white gold eruption
                    { 40, 80,140,255},  // 16 obsidian — cold steel
                    {140, 70, 10,255},  // 17 ashen — smoldering ash
                    { 80, 20,200,255},  // 18 the void — deep void violet
                    {180,100,255,255},  // 19 singularity — blinding void aurora
                    {200,  5,120,255},  // 20 carnage pink
                    {100, 30,220,255},  // 21 violet surge
                    { 20, 10, 55,255},  // 22 obsidian void
                    {160, 80,  5,255},  // 23 ember siege
                    { 40,  5,100,255},  // 24 deep void
                    { 10,  2, 40,255},  // 25 singularity+
                    { 60,  0,100,255},  // 26 dying violet
                    { 5,   0, 10,255},  // 27 absolute void
                };
                int wi=std::min(g.wave,27);
                DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(waveWash[wi],0.055f));
            }
            // Particles
            for(auto& p:g.particles){
                Vector2 sp=W2S(g,p.pos); if(!OnScreen(sp,20))continue;
                if(p.isRing){
                    float prog=1.f-p.life/p.maxLife;
                    float rr=p.ringMaxR*prog;
                    // Clean single ring — no multi-ring bloat
                    DrawCircleLinesV(sp,rr,   ColorAlpha(p.color,p.life*.90f));
                    DrawCircleLinesV(sp,rr+1, ColorAlpha(p.color,p.life*.20f));
                }
                else if(p.isShard){
                    float halfL=p.stretch*p.life*0.5f;
                    float halfW=p.radius*p.life*0.30f;
                    float ca=cosf(p.angle),sa=sinf(p.angle);
                    Vector2 A={sp.x+ca*halfL-sa*halfW, sp.y+sa*halfL+ca*halfW};
                    Vector2 B={sp.x-ca*halfL-sa*halfW, sp.y-sa*halfL+ca*halfW};
                    Vector2 C={sp.x-ca*halfL+sa*halfW, sp.y-sa*halfL-ca*halfW};
                    Vector2 D={sp.x+ca*halfL+sa*halfW, sp.y+sa*halfL-ca*halfW};
                    DrawTriangle(A,B,D,ColorAlpha(p.color,p.life*.80f));
                    DrawTriangle(B,C,D,ColorAlpha(p.color,p.life*.80f));
                }
                else if(p.isSpark){
                    // Sharp motion line — single pass
                    Vector2 sp2=W2S(g,p.prevPos);
                    DrawNeonLine(sp2,sp,p.color,1.4f,p.life*.85f);
                    if(p.life>0.55f)
                        DrawCircleV(sp,1.5f,ColorAlpha(VP_WHITE,p.life*.70f));
                }
                else {
                    float pr=p.radius*(p.life/p.maxLife);
                    DrawCircleV(sp,pr,      ColorAlpha(p.color,p.life*.75f));
                    DrawCircleV(sp,pr*.45f, ColorAlpha(VP_WHITE,p.life*.55f));
                }
            }
            for(auto& pk:g.pickups){if(!pk.active)continue;Vector2 sp=W2S(g,pk.pos);if(OnScreen(sp,30))DrawPickup(pk,sp,t);}
            for(auto& e:g.enemies){
                if(!e.active)continue;
                Vector2 sp=W2S(g,e.pos);
                if(!OnScreen(sp,50))continue;
                DrawEnemy(e,sp,t);
            }
            if(g.boss.active){Vector2 sp=W2S(g,g.boss.pos);DrawBoss(g.boss,sp,t);}
            for(auto& b:g.bullets){if(!b.active)continue;Vector2 sp=W2S(g,b.pos);if(OnScreen(sp,15))DrawBullet(b,sp,t);}
            // Boss bullets — draw as glowing hostile orbs
            for(auto& b:g.bossBullets){
                if(!b.active)continue;
                Vector2 sp=W2S(g,b.pos); if(!OnScreen(sp,20))continue;
                float pulse=.6f+.4f*sinf(t*12+b.pos.x*.02f);
                // Outer haze (1 ring only) + core
                DrawCircleV(sp,b.glowR+4,ColorAlpha(b.col,.12f));
                DrawCircleV(sp,b.glowR,ColorAlpha(b.col,.88f*pulse));
                // 4 spikes
                for(int ri=0;ri<4;ri++){
                    float ra=t*4.f+ri*(3.14159f*.5f)+b.pos.x*.01f;
                    Vector2 r1={sp.x+cosf(ra)*(b.glowR+1),sp.y+sinf(ra)*(b.glowR+1)};
                    Vector2 r2={sp.x+cosf(ra)*(b.glowR+5),sp.y+sinf(ra)*(b.glowR+5)};
                    DrawLineEx(r1,r2,0.9f,ColorAlpha(b.col,pulse*0.5f));
                }
                DrawCircleV(sp,b.glowR*.45f,ColorAlpha(VP_WHITE,.9f));
            }
            // Speed-reactive thrust wake
            {
                float tspd=sqrtf(g.playerVel.x*g.playerVel.x+g.playerVel.y*g.playerVel.y);
                if(tspd>25){
                    float tnx=g.playerVel.x/tspd, tny=g.playerVel.y/tspd;
                    Color tc2=g.tranceActive?VP_HOTPINK:(g.chiGauge>60?VP_ORANGE:VP_CYAN);
                    float intensity=std::min(1.f,(tspd-25)/150.f);
                    for(int ti=1;ti<=5;ti++){
                        float tt=(float)ti/5.f;
                        Vector2 twp={sc.x-tnx*ti*5.f,sc.y-tny*ti*5.f};
                        DrawCircleV(twp,(1.f-tt)*3.8f,ColorAlpha(tc2,(1.f-tt)*0.22f*intensity));
                    }
                }
            }
            DrawPlayer(g,sc,t);
            DrawGhostOrb(g,t);
            DrawSectorRings(g);
            // Crosshair (pink/cyan)
            Color xhc=g.tranceActive?VP_PINK:PrayerColor(g.prayer);
            {float xr=6+sinf(t*6)*.8f;DrawNeonCircle(msc,xr,xhc,.6f);
             DrawLine((int)msc.x-10,(int)msc.y,(int)msc.x-4,(int)msc.y,xhc);
             DrawLine((int)msc.x+4, (int)msc.y,(int)msc.x+10,(int)msc.y,xhc);
             DrawLine((int)msc.x,(int)msc.y-10,(int)msc.x,(int)msc.y-4,xhc);
             DrawLine((int)msc.x,(int)msc.y+4, (int)msc.x,(int)msc.y+10,xhc);}
            // ── Kill feed (top-right, scrolling) ──
            {
                int kfy=SCREEN_H-64;
                for(int ki=0;ki<(int)gUI.killFeed.size()&&ki<5;ki++){
                    auto& kfe=gUI.killFeed[ki];
                    if(kfe.life<=0)continue;
                    float kfRaw=std::min(1.f,kfe.life);
                    float kfa=kfRaw*std::min(1.f,kfe.life*2.f);
                    // slide in from right
                    float slideX=(1.f-std::min(1.f,kfe.life*6.f))*30.f;
                    int kfw=MeasureText(kfe.text.c_str(),8);
                    int kfx=(int)(SCREEN_W-kfw-16+slideX);
                    int kfh=12;
                    // bg + colored left accent stripe
                    DrawRectangle(kfx-5,kfy-1,kfw+10,kfh,ColorAlpha(VP_BLACK,kfa*0.72f));
                    DrawRectangle(kfx-5,kfy-1,2,kfh,ColorAlpha(kfe.col,kfa*0.90f));
                    // subtle right-edge glow
                    DrawRectangle(kfx+kfw+3,kfy-1,2,kfh,ColorAlpha(kfe.col,kfa*0.30f));
                    // top highlight
                    DrawRectangle(kfx-3,kfy-1,kfw+6,1,ColorAlpha(VP_WHITE,kfa*0.06f));
                    // border
                    DrawRectangleLines(kfx-5,kfy-1,kfw+10,kfh,ColorAlpha(kfe.col,kfa*0.35f));
                    // text shadow + text
                    DrawText(kfe.text.c_str(),kfx+1,kfy+1,8,ColorAlpha(VP_BLACK,kfa*0.60f));
                    DrawText(kfe.text.c_str(),kfx,  kfy,  8,ColorAlpha(kfe.col,kfa));
                    kfy-=15;
                }
            }
            // Popups — with scale pop-in, outer halo, stronger glow
            for(auto& pt:g.popups){
                Vector2 sp=W2S(g,pt.pos);
                float a=pt.life/pt.maxLife;
                float rise=(1.f-a)*58.f;
                // ease-out rise
                float scale=a<.18f?(a/.18f):(a>0.85f?(0.85f+(1.f-a)*.15f/.15f):1.f);
                scale=std::min(scale,1.f);
                int sz=(int)(pt.fontSize*scale);
                if(sz<4) sz=4;
                int tw2=MeasureText(pt.text.c_str(),sz);
                int tx=(int)sp.x-tw2/2;
                int ty=(int)(sp.y-rise);
                // outer halo (wider, soft)
                DrawText(pt.text.c_str(),tx+3,ty+3,sz,ColorAlpha(pt.color,a*.06f));
                DrawText(pt.text.c_str(),tx-3,ty-3,sz,ColorAlpha(pt.color,a*.06f));
                DrawText(pt.text.c_str(),tx+3,ty-3,sz,ColorAlpha(pt.color,a*.06f));
                DrawText(pt.text.c_str(),tx-3,ty+3,sz,ColorAlpha(pt.color,a*.06f));
                // mid glow passes
                DrawText(pt.text.c_str(),tx+2,ty+2,sz,ColorAlpha(VP_BLACK,a*.55f));
                DrawText(pt.text.c_str(),tx-1,ty,  sz,ColorAlpha(pt.color,a*.22f));
                DrawText(pt.text.c_str(),tx+1,ty,  sz,ColorAlpha(pt.color,a*.22f));
                DrawText(pt.text.c_str(),tx,  ty-1,sz,ColorAlpha(pt.color,a*.18f));
                DrawText(pt.text.c_str(),tx,  ty+1,sz,ColorAlpha(pt.color,a*.18f));
                // crisp top layer
                DrawText(pt.text.c_str(),tx,ty,sz,ColorAlpha(pt.color,a));
                // sparkle dot at peak alpha
                if(a>0.80f){
                    float sp2=a-.80f;
                    DrawCircleV({(float)(tx+tw2/2),(float)(ty-3)},2.5f*sp2,ColorAlpha(VP_WHITE,sp2*1.2f));
                }
            }
            // ── Boss warning — cinematic letterbox + alert ──
            if(bossWarnTimer>0){
                float a=std::min(1.f,bossWarnTimer);
                float flicker=((int)(t*4)%2==0)?.88f:1.f;

                // Black bars
                int barH=(int)(SCREEN_H*.14f);
                DrawRectangle(0,0,SCREEN_W,barH,ColorAlpha({0,0,0,255},.92f));
                DrawRectangle(0,SCREEN_H-barH,SCREEN_W,barH,ColorAlpha({0,0,0,255},.92f));

                // Scanline flash on bars
                for(int y2=0;y2<barH;y2+=3)
                    DrawRectangle(0,y2,SCREEN_W,1,ColorAlpha(VP_RED,.04f*a));

                // Center alert bar
                int alertY=SCREEN_H/2-18;
                DrawRectangle(0,alertY-2,SCREEN_W,42,ColorAlpha(VP_BLACK,.75f));
                DrawRectangle(0,alertY-2,SCREEN_W,2,ColorAlpha(VP_RED,a*.7f));
                DrawRectangle(0,alertY+40,SCREEN_W,2,ColorAlpha(VP_RED,a*.7f));

                // Alert text — red + white flash
                Color alertC=((int)(t*8)%2==0)?ColorAlpha(VP_RED,a*flicker):ColorAlpha(VP_WHITE,a*flicker);
                int tw=MeasureText("! WARNING !", 10);
                DrawText("! WARNING !",(SCREEN_W-tw)/2+1,alertY-1,10,ColorAlpha(VP_BLACK,a*.7f));
                DrawText("! WARNING !",(SCREEN_W-tw)/2,alertY,10,alertC);

                // Boss name — big centered
                int bnw=MeasureText(bossWarnText.c_str(),18);
                DrawText(bossWarnText.c_str(),(SCREEN_W-bnw)/2+2,alertY+14+2,18,ColorAlpha(VP_BLACK,a*.7f));
                // glow passes
                DrawText(bossWarnText.c_str(),(SCREEN_W-bnw)/2-1,alertY+14,18,ColorAlpha(VP_HOTPINK,a*.2f));
                DrawText(bossWarnText.c_str(),(SCREEN_W-bnw)/2+1,alertY+14,18,ColorAlpha(VP_HOTPINK,a*.2f));
                DrawText(bossWarnText.c_str(),(SCREEN_W-bnw)/2,alertY+14,18,ColorAlpha(VP_HOTPINK,a*flicker));

                // Red edge pulse
                float ep=.10f+.08f*sinf(t*12)*a;
                DrawRectangleLinesEx({0,0,(float)SCREEN_W,(float)SCREEN_H},4,ColorAlpha(VP_RED,ep*a));
                DrawRectangleLinesEx({4,4,(float)SCREEN_W-8,(float)SCREEN_H-8},2,ColorAlpha(VP_HOTPINK,ep*.5f*a));
            }
            // ── Flash overlays ──
            if(g.warpFlash>0){
                DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_CYAN,g.warpFlash*.14f));
            }
            if(g.novaFlash>0){
                DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_WHITE,g.novaFlash*.10f));
                for(int i=0;i<4;i++){
                    float nr=50+i*80.f+(1-g.novaFlash)*200;
                    DrawNeonCircle({(float)SCREEN_W/2,(float)SCREEN_H/2},nr,VP_PINK,g.novaFlash*.3f);
                }
            }
            if(g.bossRumble>0){
                float br=std::min(1.f,g.bossRumble);
                // Throbbing red border
                float rb=br*.12f+.04f*sinf(t*5)*br;
                DrawRectangleLinesEx({0,0,(float)SCREEN_W,(float)SCREEN_H},4,ColorAlpha(VP_RED,rb));
                DrawRectangleLinesEx({4,4,(float)SCREEN_W-8,(float)SCREEN_H-8},2,ColorAlpha(VP_HOTPINK,rb*.5f));
                // Vignette redness
                for(int i=0;i<5;i++)
                    DrawRectangleLinesEx({(float)i*2,(float)i*2,(float)SCREEN_W-i*4,(float)SCREEN_H-i*4},
                        2,ColorAlpha(VP_DARKRED,rb*.06f*(5-i)));
            }
            DrawHUD(g,t);
            DrawNav(g,t,g.dashCooldown);
            DrawRadar(g,t);

            // ── Per-wave ship victory animation — no overlay, no panel, just the ship ──
            // Each wave has a unique themed animation that fits the world.
            // The game world stays fully visible behind the ship.
            if(g.waveTransTimer>0){
                g.waveTransTimer-=dt;
                
                // Total duration: 3.5s
                // Phase 1 (3.5s → 2.5s): 1.0s PAUSE - no animation, just wait
                // Phase 2 (2.5s → 2.1s): 0.4s FADE IN - smooth entrance
                // Phase 3 (2.1s → 0.4s): 1.7s HOLD - full animation
                // Phase 4 (0.4s → 0.0s): 0.4s FADE OUT - smooth exit
                
                const float PAUSE_END = 2.5f;
                const float FADE_IN_END = 2.1f;
                const float FADE_OUT_START = 0.65f;   // longer fade-out window
                
                // During pause phase, don't render anything
                if(g.waveTransTimer > PAUSE_END){
                    // Still in pause - render nothing
                    DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_BLACK,0.0f));
                } else {
                    // Animation phase - calculate progress
                    float animTime = g.waveTransTimer;  // Time remaining in animation
                    
                    // inv: animation progress 0→1 mapped to HOLD phase only
                    // so animations fully complete (reach inv=1) before fade-out begins
                    const float HOLD_DURATION = FADE_IN_END - FADE_OUT_START; // 1.7s
                    float inv;
                    if(animTime >= FADE_IN_END)          inv = 0.f;
                    else if(animTime <= FADE_OUT_START)  inv = 1.f;
                    else inv = (FADE_IN_END - animTime) / HOLD_DURATION;
                    // keep 'a' for backwards-compat with any case that uses it
                    float a = 1.f - inv;
                    
                    Vector2 sc = W2S(g, g.playerPos);
                    int prevWave = std::max(0, g.wave - 1);
                    float shipA = g.playerAngle;

                    // Wave-specific accent color
                    static const Color waveCelebCol[28]={
                        {255,60,140,255},  // 0  crimson
                        {255,110,20,255},  // 1  ember
                        {30,180,255,255},  // 2  ocean
                        {0,210,190,255},   // 3  abyssal
                        {100,255,20,255},  // 4  toxic
                        {160,190,10,255},  // 5  spore
                        {160,40,255,255},  // 6  void rift
                        {120,60,220,255},  // 7  phantom
                        {20,230,210,255},  // 8  crystal
                        {255,210,30,255},  // 9  resonance
                        {255,160,30,255},  // 10 forge
                        {255,245,60,255},  // 11 shard
                        {255,60,200,255},  // 12 quantum
                        {180,30,255,255},  // 13 event horizon
                        {255,80,10,255},   // 14 inferno
                        {255,240,140,255}, // 15 corona
                        {60,160,220,255},  // 16 obsidian
                        {210,130,30,255},  // 17 ashen
                        {140,60,255,255},  // 18 void
                        {255,220,255,255}, // 19 singularity
                        {255,40,160,255},  // 20+
                        {140,60,255,255},{60,180,220,255},{220,140,30,255},
                        {100,20,220,255},{30,0,120,255},{120,0,200,255},{10,0,30,255},
                    };
                    Color acol = waveCelebCol[std::min(prevWave, 27)];

                    // SMOOTH FADE CALCULATION - cubic easing for extra smoothness
                    float fadeA = 1.f;
                    float screenDarken = 0.f;
                    
                    if(animTime > FADE_IN_END){
                        // FADE IN phase (2.5s → 2.1s)
                        float fadeProgress = (animTime - FADE_IN_END) / 0.4f;  // 1→0 during fade in
                        fadeProgress = 1.f - fadeProgress;  // 0→1 during fade in
                        // Cubic ease in for ultra smooth entrance
                        fadeA = fadeProgress * fadeProgress * fadeProgress;
                        screenDarken = (1.f - fadeA) * 0.25f;  // subtle cinematic dim during entrance
                    } else if(animTime > FADE_OUT_START){
                        // HOLD phase (2.1s → 0.4s) - full opacity
                        fadeA = 1.f;
                        screenDarken = 0.f;
                    } else {
                        // FADE OUT — animation elements fade via fadeA; world stays full brightness
                        float fadeProgress = animTime / FADE_OUT_START;  // 1→0
                        fadeA = fadeProgress * fadeProgress;
                        screenDarken = 0.f;  // no world darkening — prevents bright pop when anim ends
                    }

                    // SCREEN DARKENING for cinematic fade
                    DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_BLACK,screenDarken));

                    // ── UNIQUE ANIMATION PER WAVE ──────────────────────────────────────
                switch(prevWave % 28){

                case 0:{ // DIMENSIONAL FRACTURE — reality shatters into crystalline shards
                    int nShards = 24;
                    for(int i=0;i<nShards;i++){
                        float shardA = i*(2.f*3.14159f/nShards) + inv*0.8f;
                        float shardDist = (15.f + i*3.f)*inv;
                        float shardLen = 18.f + sinf(i*2.7f+inv*5.f)*10.f;
                        float x1 = sc.x + cosf(shardA)*shardDist;
                        float y1 = sc.y + sinf(shardA)*shardDist;
                        float x2 = x1 + cosf(shardA + 1.2f + sinf(i*1.3f))*shardLen;
                        float y2 = y1 + sinf(shardA + 1.2f + sinf(i*1.3f))*shardLen;
                        Color shardCol = (i%5==0)?VP_WHITE:(i%5==1)?VP_CYAN:(i%5==2)?VP_PINK:(i%5==3)?VP_PURPLE:VP_MINT;
                        DrawLineEx({x1,y1},{x2,y2},2.8f,ColorAlpha(shardCol,fadeA*(1.f-inv*0.6f)*0.9f));
                        // Crackling energy nodes
                        for(int j=0;j<4;j++){
                            float jitX = sinf(i*4.2f+j*2.f+inv*18.f)*4.f;
                            float jitY = cosf(i*3.8f+j*2.f+inv*18.f)*4.f;
                            DrawCircleV({x2+jitX,y2+jitY},1.6f,ColorAlpha(VP_WHITE,fadeA*0.75f*fabsf(sinf(inv*10.f+j))));
                        }
                        // Prismatic reflection trails
                        if(i%3==0){
                            DrawLineEx({x1,y1},{x2,y2},1.2f,ColorAlpha(VP_WHITE,fadeA*0.3f));
                        }
                    }
                    // Reality distortion waves
                    for(int i=0;i<3;i++){
                        float waveP = fmodf(inv*2.5f + i*0.33f, 1.f);
                        DrawCircleLinesV(sc,waveP*140.f,ColorAlpha(acol,fadeA*(1.f-waveP)*0.5f));
                    }
                    DrawShipSilhouette(g.selectedShip,sc,inv*1.5f+sinf(inv*8.f)*0.3f,t,acol,true,false,g.shipUpgradeTier);
                    break;}

                case 1:{ // PLASMA SERPENT — coiling energy dragon spirals around ship
                    int segments = 35;
                    float coilRadius = 28.f + sinf(inv*5.f)*12.f;
                    Vector2 prevPt = sc;
                    for(int i=0;i<segments;i++){
                        float p = (float)i/(float)segments;
                        float spiralA = p*10.f*3.14159f + inv*4.f;
                        float spiralR = coilRadius*(1.f-p*0.4f);
                        float x1 = sc.x + cosf(spiralA)*spiralR;
                        float y1 = sc.y + sinf(spiralA)*spiralR;
                        Vector2 currPt = {x1,y1};
                        float thickness = 4.5f*(1.f-p*p);
                        Color segCol = (i%4==0)?VP_WHITE:(i%4==1)?VP_CYAN:(i%4==2)?VP_PURPLE:VP_PINK;
                        DrawLineEx(prevPt,currPt,thickness,ColorAlpha(segCol,fadeA*0.85f));
                        // Glow effect
                        DrawLineEx(prevPt,currPt,thickness+3.f,ColorAlpha(segCol,fadeA*0.2f));
                        // Pulsing energy cores
                        if(i%4==0){
                            float coreSize = 3.f + sinf(inv*12.f+i)*1.5f;
                            DrawCircleV(currPt,coreSize,ColorAlpha(VP_WHITE,fadeA*0.95f));
                            DrawCircleV(currPt,coreSize+4.f,ColorAlpha(segCol,fadeA*0.3f));
                        }
                        // Plasma sparks
                        if(i%2==0 && inv > 0.3f){
                            float sparkA = spiralA + 1.57f;
                            float sparkDist = 6.f*sinf(inv*15.f+i);
                            DrawCircleV({x1+cosf(sparkA)*sparkDist,y1+sinf(sparkA)*sparkDist},1.2f,
                                ColorAlpha(VP_WHITE,fadeA*0.7f));
                        }
                        prevPt = currPt;
                    }
                    // Serpent head at end
                    float headA = segments/(float)segments*10.f*3.14159f + inv*4.f;
                    float headR = coilRadius*(1.f-(float)segments/(float)segments*0.4f);
                    Vector2 headPos = {sc.x + cosf(headA)*headR, sc.y + sinf(headA)*headR};
                    DrawCircleV(headPos,5.f,ColorAlpha(acol,fadeA*0.9f));
                    DrawCircleV(headPos,2.f,ColorAlpha(VP_WHITE,fadeA));
                    DrawShipSilhouette(g.selectedShip,sc,inv*2.5f*3.14159f,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 2:{ // CRYSTALLINE MATRIX — geometric lattice forms then explodes
                    if(inv < 0.55f){
                        // Growth phase - complex hexagonal crystallization
                        float growth = inv/0.55f;
                        int rings = 5;
                        for(int r=0;r<rings;r++){
                            float ringR = (r+1)*16.f*growth;
                            int hexSides = 6;
                            for(int i=0;i<hexSides;i++){
                                float a1 = i*(2.f*3.14159f/hexSides) + growth*0.3f;
                                float a2 = (i+1)*(2.f*3.14159f/hexSides) + growth*0.3f;
                                float x1 = sc.x + cosf(a1)*ringR;
                                float y1 = sc.y + sinf(a1)*ringR;
                                float x2 = sc.x + cosf(a2)*ringR;
                                float y2 = sc.y + sinf(a2)*ringR;
                                DrawLineEx({x1,y1},{x2,y2},2.2f,ColorAlpha(acol,fadeA*growth*0.8f));
                                // Connector lines to center
                                DrawLineEx(sc,{x1,y1},1.2f,ColorAlpha(acol,fadeA*growth*0.4f));
                                // Crystal nodes with glow
                                DrawCircleV({x1,y1},2.5f,ColorAlpha(VP_WHITE,fadeA*growth*0.95f));
                                DrawCircleV({x1,y1},5.f,ColorAlpha(acol,fadeA*growth*0.25f));
                            }
                            // Cross-connections between rings
                            if(r > 0){
                                float prevRingR = r*16.f*growth;
                                for(int i=0;i<hexSides;i++){
                                    float a1 = i*(2.f*3.14159f/hexSides) + growth*0.3f;
                                    DrawLineEx({sc.x+cosf(a1)*prevRingR,sc.y+sinf(a1)*prevRingR},
                                              {sc.x+cosf(a1)*ringR,sc.y+sinf(a1)*ringR},
                                              0.9f,ColorAlpha(acol,fadeA*growth*0.3f));
                                }
                            }
                        }
                    } else {
                        // Shatter phase - fragments explode with trails
                        float shatter = (inv-0.55f)/0.45f;
                        int nFragments = 30;
                        for(int i=0;i<nFragments;i++){
                            float fA = i*(2.f*3.14159f/nFragments) + sinf(i*2.1f)*0.5f;
                            float fDist = shatter*shatter*75.f + i*3.f;
                            float x = sc.x + cosf(fA)*fDist;
                            float y = sc.y + sinf(fA)*fDist;
                            // Fragment trail
                            int trailLen = 5;
                            for(int t=0;t<trailLen;t++){
                                float tp = (float)t/(float)trailLen;
                                float tx = x - cosf(fA)*tp*15.f;
                                float ty = y - sinf(fA)*tp*15.f;
                                DrawCircleV({tx,ty},1.8f*(1.f-tp),
                                    ColorAlpha(acol,fadeA*(1.f-shatter)*(1.f-tp)*0.6f));
                            }
                            // Spinning polygon fragments
                            int fragSides = 3 + (i%4);
                            float fragSize = 5.f + sinf(i*2.9f)*3.f;
                            float fragSpin = shatter*4.f + i*0.7f;
                            for(int j=0;j<fragSides;j++){
                                float fa1 = j*(2.f*3.14159f/fragSides) + fragSpin;
                                float fa2 = (j+1)*(2.f*3.14159f/fragSides) + fragSpin;
                                DrawLineEx({x+cosf(fa1)*fragSize,y+sinf(fa1)*fragSize},
                                          {x+cosf(fa2)*fragSize,y+sinf(fa2)*fragSize},
                                          1.8f,ColorAlpha(i%2==0?acol:VP_WHITE,fadeA*(1.f-shatter)*0.85f));
                            }
                        }
                    }
                    DrawShipSilhouette(g.selectedShip,sc,inv*3.14159f*1.5f,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 3:{ // CHROMATIC SPLIT — ship splits into RGB channels that dance then reconverge
                    float separation = sinf(inv*3.14159f)*28.f; // Peak separation at mid-animation
                    float phase = inv*2.f*3.14159f;
                    // Red channel - left spiral
                    float redX = sc.x - separation + cosf(phase)*8.f;
                    float redY = sc.y + sinf(phase*1.3f)*6.f;
                    DrawShipSilhouette(g.selectedShip,{redX,redY},sinf(phase)*0.2f,t,
                        ColorAlpha(VP_RED,fadeA*0.8f),false,false,g.shipUpgradeTier);
                    // Red trail particles
                    for(int i=0;i<8;i++){
                        float tp = (float)i/8.f;
                        if(tp < inv){
                            float tx = redX - cosf(phase)*tp*20.f;
                            float ty = redY - sinf(phase*1.3f)*tp*15.f;
                            DrawCircleV({tx,ty},2.f*(1.f-tp),ColorAlpha(VP_RED,fadeA*0.5f*(1.f-tp)));
                        }
                    }
                    // Green channel - center wave
                    float greenX = sc.x + cosf(phase*0.7f)*4.f;
                    float greenY = sc.y + separation*0.6f + sinf(phase*1.1f)*7.f;
                    DrawShipSilhouette(g.selectedShip,{greenX,greenY},cosf(phase)*0.2f,t,
                        ColorAlpha(VP_MINT,fadeA*0.8f),false,false,g.shipUpgradeTier);
                    for(int i=0;i<8;i++){
                        float tp = (float)i/8.f;
                        if(tp < inv){
                            float tx = greenX - cosf(phase*0.7f)*tp*20.f;
                            float ty = greenY - sinf(phase*1.1f)*tp*15.f;
                            DrawCircleV({tx,ty},2.f*(1.f-tp),ColorAlpha(VP_MINT,fadeA*0.5f*(1.f-tp)));
                        }
                    }
                    // Blue channel - right spiral
                    float blueX = sc.x + separation + cosf(phase*1.5f)*8.f;
                    float blueY = sc.y + sinf(phase*0.9f)*6.f;
                    DrawShipSilhouette(g.selectedShip,{blueX,blueY},-sinf(phase)*0.2f,t,
                        ColorAlpha(VP_CYAN,fadeA*0.8f),false,false,g.shipUpgradeTier);
                    for(int i=0;i<8;i++){
                        float tp = (float)i/8.f;
                        if(tp < inv){
                            float tx = blueX - cosf(phase*1.5f)*tp*20.f;
                            float ty = blueY - sinf(phase*0.9f)*tp*15.f;
                            DrawCircleV({tx,ty},2.f*(1.f-tp),ColorAlpha(VP_CYAN,fadeA*0.5f*(1.f-tp)));
                        }
                    }
                    // Scanline distortion
                    for(int i=0;i<35;i++){
                        float scanY = sc.y - 70.f + i*4.f + sinf(inv*6.f+i*0.3f)*12.f;
                        float scanX = sc.x - 50.f + sinf(inv*4.f+i*0.2f)*25.f;
                        DrawLineEx({scanX,scanY},{scanX+100.f-sinf(inv*4.f+i*0.2f)*50.f,scanY},1.2f,
                            ColorAlpha(i%3==0?VP_WHITE:i%3==1?VP_CYAN:VP_PINK,fadeA*0.15f*fabsf(sinf(i*0.6f+inv*4.f))));
                    }
                    // Convergence at end
                    if(inv > 0.75f){
                        float converge = (inv-0.75f)/0.25f;
                        DrawShipSilhouette(g.selectedShip,sc,0,t,ColorAlpha(acol,fadeA*converge),false,false,g.shipUpgradeTier);
                        // White flash at full convergence
                        if(converge > 0.8f){
                            DrawCircleV(sc,20.f*(converge-0.8f)*5.f,ColorAlpha(VP_WHITE,fadeA*(1.f-converge)*2.f));
                        }
                    }
                    break;}

                case 4:{ // SOLAR PROMINENCE — massive arcing plasma jets erupt from ship
                    int nArcs = 10;
                    for(int i=0;i<nArcs;i++){
                        float arcA = i*(2.f*3.14159f/nArcs) + inv*0.5f;
                        int bezierPts = 18;
                        float arcHeight = 65.f*inv + sinf(i*2.4f+inv*3.f)*20.f;
                        for(int j=0;j<bezierPts-1;j++){
                            float t1 = (float)j/(float)(bezierPts-1);
                            float t2 = (float)(j+1)/(float)(bezierPts-1);
                            // Parabolic arc with turbulence
                            float turb1 = sinf(t1*8.f+inv*10.f+i)*3.f;
                            float turb2 = sinf(t2*8.f+inv*10.f+i)*3.f;
                            float x1 = sc.x + cosf(arcA)*(t1*50.f) + turb1;
                            float y1 = sc.y + sinf(arcA)*(t1*50.f) - arcHeight*t1*(1.f-t1)*4.f;
                            float x2 = sc.x + cosf(arcA)*(t2*50.f) + turb2;
                            float y2 = sc.y + sinf(arcA)*(t2*50.f) - arcHeight*t2*(1.f-t2)*4.f;
                            float arcThick = 3.5f*(1.f-t1*t1);
                            Color arcCol = (t1<0.2f)?VP_WHITE:(t1<0.5f)?VP_YELLOW:(t1<0.75f)?VP_ORANGE:VP_RED;
                            DrawLineEx({x1,y1},{x2,y2},arcThick,ColorAlpha(arcCol,fadeA*0.85f));
                            // Glow layer
                            DrawLineEx({x1,y1},{x2,y2},arcThick+2.f,ColorAlpha(arcCol,fadeA*0.25f));
                        }
                        // Flare particles along arc
                        for(int k=0;k<5;k++){
                            float tipT = (float)k/5.f;
                            float turb = sinf(tipT*8.f+inv*10.f+i)*3.f;
                            float tipX = sc.x + cosf(arcA)*(tipT*50.f) + turb;
                            float tipY = sc.y + sinf(arcA)*(tipT*50.f) - arcHeight*tipT*(1.f-tipT)*4.f;
                            float flareSize = 2.5f + sinf(inv*20.f+k+i)*1.5f;
                            DrawCircleV({tipX,tipY},flareSize,ColorAlpha(VP_WHITE,fadeA*0.9f));
                            DrawCircleV({tipX,tipY},flareSize+4.f,ColorAlpha(VP_YELLOW,fadeA*0.35f));
                        }
                    }
                    // Pulsing core
                    float coreSize = 10.f+sinf(inv*25.f)*5.f;
                    DrawCircleV(sc,coreSize,ColorAlpha(VP_WHITE,fadeA*0.85f));
                    DrawCircleV(sc,coreSize+8.f,ColorAlpha(VP_YELLOW,fadeA*0.4f));
                    DrawCircleV(sc,coreSize+15.f,ColorAlpha(VP_ORANGE,fadeA*0.2f));
                    DrawShipSilhouette(g.selectedShip,sc,inv*3.14159f*2.5f,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 5:{ // QUANTUM FOAM — reality bubbles with probability clouds
                    int nBubbles = 50;
                    for(int i=0;i<nBubbles;i++){
                        float bubblePhase = fmodf(inv*2.f + i*0.02f, 1.f);
                        float bubbleR = bubblePhase*bubblePhase*40.f;
                        float bubbleA = i*3.1f + inv*5.f + sinf(i*1.7f)*0.6f;
                        float bx = sc.x + cosf(bubbleA)*(18.f + i*1.2f) + sinf(inv*12.f+i)*5.f;
                        float by = sc.y + sinf(bubbleA)*(18.f + i*1.2f) + cosf(inv*15.f+i)*5.f;
                        float bubbleSize = (2.5f + sinf(i*2.3f)*2.f)*(1.f-bubblePhase);
                        Color bubbleCol = (i%3==0)?VP_CYAN:(i%3==1)?VP_PURPLE:VP_PINK;
                        // Bubble shell
                        DrawCircleV({bx,by},bubbleSize,ColorAlpha(bubbleCol,fadeA*(1.f-bubblePhase)*0.5f));
                        DrawCircleV({bx,by},bubbleSize*0.7f,ColorAlpha(VP_WHITE,fadeA*(1.f-bubblePhase)*0.3f));
                        // Quantum particle at center
                        if(bubblePhase > 0.3f){
                            DrawCircleV({bx,by},1.2f,ColorAlpha(VP_WHITE,fadeA*0.85f));
                        }
                        // Probability waves
                        if(bubblePhase > 0.5f && i%4==0){
                            DrawCircleLinesV({bx,by},(bubblePhase-0.5f)*15.f,
                                ColorAlpha(bubbleCol,fadeA*(1.f-bubblePhase)*0.6f));
                        }
                    }
                    // Uncertainty principle - ship oscillates
                    float qX = sc.x + sinf(inv*30.f)*4.f;
                    float qY = sc.y + cosf(inv*35.f)*4.f;
                    float qA = sinf(inv*18.f)*0.25f;
                    // Ghost ships (superposition states)
                    for(int i=0;i<3;i++){
                        float ghostX = qX + cosf(i*2.f)*8.f;
                        float ghostY = qY + sinf(i*2.f)*8.f;
                        DrawShipSilhouette(g.selectedShip,{ghostX,ghostY},qA,t,
                            ColorAlpha(acol,fadeA*0.2f),false,false,g.shipUpgradeTier);
                    }
                    DrawShipSilhouette(g.selectedShip,{qX,qY},qA,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 6:{ // FRACTAL MANDALA — recursive sacred geometry emanates and rotates
                    int layers = 6;
                    float mandalaR = inv*65.f;
                    for(int l=0;l<layers;l++){
                        float layerR = mandalaR*(1.f - l*0.15f);
                        int petals = 12 - l*2;
                        float rotation = inv*2.f*3.14159f + l*0.8f - sinf(inv*3.f+l)*0.3f;
                        for(int p=0;p<petals;p++){
                            float pA = p*(2.f*3.14159f/petals) + rotation;
                            float pA2 = (p+0.5f)*(2.f*3.14159f/petals) + rotation;
                            // Petal outer point
                            float x1 = sc.x + cosf(pA)*layerR;
                            float y1 = sc.y + sinf(pA)*layerR;
                            // Petal inner curve control points
                            float x2 = sc.x + cosf(pA2)*layerR*0.5f;
                            float y2 = sc.y + sinf(pA2)*layerR*0.5f;
                            float x3 = sc.x + cosf(pA + 2.f*3.14159f/petals)*layerR;
                            float y3 = sc.y + sinf(pA + 2.f*3.14159f/petals)*layerR;
                            Color petalCol = (l%4==0)?VP_PINK:(l%4==1)?VP_PURPLE:(l%4==2)?VP_CYAN:VP_MINT;
                            // Petal arc
                            DrawLineEx({x1,y1},{x2,y2},2.2f,ColorAlpha(petalCol,fadeA*0.7f));
                            DrawLineEx({x2,y2},{x3,y3},2.2f,ColorAlpha(petalCol,fadeA*0.7f));
                            // Inner glow
                            DrawLineEx({x1,y1},{x2,y2},4.f,ColorAlpha(petalCol,fadeA*0.2f));
                            // Sacred nodes
                            DrawCircleV({x1,y1},2.8f,ColorAlpha(VP_WHITE,fadeA*0.85f));
                            DrawCircleV({x1,y1},5.f,ColorAlpha(petalCol,fadeA*0.3f));
                            // Connecting lines between layers
                            if(l > 0 && p%2==0){
                                float prevLayerR = mandalaR*(1.f - (l-1)*0.15f);
                                float px = sc.x + cosf(pA)*prevLayerR;
                                float py = sc.y + sinf(pA)*prevLayerR;
                                DrawLineEx({px,py},{x1,y1},0.9f,ColorAlpha(petalCol,fadeA*0.25f));
                            }
                        }
                        // Layer ring
                        if(l%2==0){
                            Color layerCol = (l%4==0)?VP_PINK:(l%4==1)?VP_PURPLE:(l%4==2)?VP_CYAN:VP_MINT;
                            DrawCircleLinesV(sc,layerR*0.75f,ColorAlpha(layerCol,fadeA*0.15f));
                        }
                    }
                    // Central bloom
                    float bloomSize = 8.f + sinf(inv*12.f)*3.f;
                    DrawCircleV(sc,bloomSize,ColorAlpha(VP_WHITE,fadeA*0.9f));
                    DrawCircleV(sc,bloomSize+6.f,ColorAlpha(acol,fadeA*0.5f));
                    DrawCircleV(sc,bloomSize+12.f,ColorAlpha(acol,fadeA*0.2f));
                    DrawShipSilhouette(g.selectedShip,sc,inv*3.14159f,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 7:{ // PHANTOM GATE — ship flickers in and out like a ghost, afterimage trails
                    float sx2 = sc.x + sinf(inv*5.f)*15.f;
                    float sy2 = sc.y + cosf(inv*4.3f)*8.f;
                    // Ghost afterimages
                    for(int i=1;i<=5;i++){
                        float gi = i * 0.1f;
                        float gx = sc.x + sinf((inv-gi)*5.f)*15.f;
                        float gy = sc.y + cosf((inv-gi)*4.3f)*8.f;
                        if(gi < inv)
                            DrawShipSilhouette(g.selectedShip,{gx,gy},shipA+gi,t,acol,false,false,0);
                    }
                    // Main ship flickers
                    float flicker = sinf(inv*40.f)*0.5f+0.5f;
                    if(flicker > 0.3f)
                        DrawShipSilhouette(g.selectedShip,{sx2,sy2},shipA,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 8:{ // CRYSTAL CAVERNS — ship slowly rotates surrounded by orbiting crystal shards
                    float spinA = inv * 2.f * 3.14159f;
                    int nShards = 6;
                    for(int i=0;i<nShards;i++){
                        float sa2 = i*(2.f*3.14159f/nShards) + spinA;
                        float sr = 28.f + 8.f*sinf(inv*4.f+i);
                        float shx = sc.x+cosf(sa2)*sr, shy=sc.y+sinf(sa2)*sr;
                        // Diamond shard
                        float sz = 3.5f + sinf(inv*6.f+i)*1.5f;
                        DrawLineEx({shx,shy-sz},{shx+sz*0.6f,shy},1.5f,ColorAlpha(acol,fadeA*0.8f));
                        DrawLineEx({shx+sz*0.6f,shy},{shx,shy+sz},1.5f,ColorAlpha(acol,fadeA*0.8f));
                        DrawLineEx({shx,shy+sz},{shx-sz*0.6f,shy},1.5f,ColorAlpha(acol,fadeA*0.8f));
                        DrawLineEx({shx-sz*0.6f,shy},{shx,shy-sz},1.5f,ColorAlpha(acol,fadeA*0.8f));
                        DrawCircleV({shx,shy},1.5f,ColorAlpha(VP_WHITE,fadeA*0.6f));
                    }
                    DrawShipSilhouette(g.selectedShip,sc,shipA+spinA*0.25f,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 9:{ // RESONANCE CORE — ship pulses with concentric gold rings like a tuning fork
                    for(int i=0;i<5;i++){
                        float rp = fmodf(inv*2.2f + i*0.2f, 1.f);
                        float rr = rp*65.f;
                        float ra = (1.f-rp)*(1.f-rp)*fadeA*0.55f;
                        DrawCircleLinesV(sc, rr, ColorAlpha(acol, ra));
                        DrawCircleLinesV(sc, rr+1.5f, ColorAlpha(VP_WHITE, ra*0.3f));
                    }
                    // ship pulses in scale (draw 3 overlaid at different sizes)
                    float pulse = 1.f + 0.12f*sinf(inv*18.f);
                    DrawCircleV(sc, 18.f*pulse, ColorAlpha(acol, 0.06f*fadeA));
                    DrawShipSilhouette(g.selectedShip,sc,shipA,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 10:{ // STELLAR FORGE — ship does a forward afterburner blast, orange/white exhaust plume
                    float thrust = inv*inv*60.f; // accelerates forward
                    float sx2 = sc.x + cosf(shipA)*thrust;
                    float sy2 = sc.y + sinf(shipA)*thrust;
                    // exhaust plume behind ship
                    int nPlume = 18;
                    for(int i=0;i<nPlume;i++){
                        float pp = (float)i/nPlume;
                        float px2 = sx2 - cosf(shipA)*(i*3.5f+2.f) + sinf(shipA)*sinf(i*0.8f+inv*15.f)*4.f;
                        float py2 = sy2 - sinf(shipA)*(i*3.5f+2.f) + cosf(shipA)*sinf(i*0.8f+inv*15.f)*4.f;
                        Color pc = i<5?VP_WHITE:i<9?VP_YELLOW:acol;
                        DrawCircleV({px2,py2}, (1.f-pp)*4.5f, ColorAlpha(pc,fadeA*(1.f-pp)*0.85f));
                    }
                    DrawShipSilhouette(g.selectedShip,{sx2,sy2},shipA,t,acol,true,false,g.shipUpgradeTier);
                    break;}

                case 11:{ // SHARD TEMPEST — rapid spin with sparks flying off like a grinder
                    float spin = inv * 8.f * 3.14159f; // very fast spin
                    int nSparks = 20;
                    for(int i=0;i<nSparks;i++){
                        float sp = fmodf(inv*4.f + i*0.05f, 1.f);
                        float sa2 = i*(2.f*3.14159f/nSparks) + spin*0.3f;
                        float sr = 12.f + sp*45.f;
                        float sx2 = sc.x+cosf(sa2)*sr, sy2=sc.y+sinf(sa2)*sr;
                        DrawLineEx({sc.x+cosf(sa2)*(sr-6.f),sc.y+sinf(sa2)*(sr-6.f)},{sx2,sy2},
                            1.2f,ColorAlpha(i%2==0?acol:VP_WHITE,(1.f-sp)*fadeA*0.9f));
                    }
                    DrawShipSilhouette(g.selectedShip,sc,spin,t,acol,true,true,g.shipUpgradeTier);
                    break;}

                case 12:{ // QUANTUM DEPTHS — ship phases through pink mirror copies of itself
                    // Draw 4 phase-offset ghosts in a tight orbit
                    for(int i=0;i<4;i++){
                        float pa = i*(2.f*3.14159f/4.f) + inv*3.14159f;
                        float pr = 18.f * sinf(inv*3.14159f); // orbit expands then contracts
                        float gx=sc.x+cosf(pa)*pr, gy=sc.y+sinf(pa)*pr;
                        float ga = 0.22f*(1.f-(float)i/4.f)*fadeA;
                        DrawShipSilhouette(g.selectedShip,{gx,gy},pa+3.14159f/2.f,t,acol,false,false,0);
                    }
                    DrawShipSilhouette(g.selectedShip,sc,shipA,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 13:{ // EVENT HORIZON — ship gets pulled toward a gravity well, barely escapes
                    // Phase 1 (inv 0-0.5): pulled toward center gravity well
                    // Phase 2 (inv 0.5-1.0): slingshots away at high speed
                    float gx2, gy2, fa2;
                    if(inv < 0.5f){
                        float p = inv/0.5f; // 0→1
                        gx2 = sc.x - cosf(shipA)*p*25.f;
                        gy2 = sc.y - sinf(shipA)*p*25.f;
                        fa2 = shipA + p*1.2f; // spirals in
                    } else {
                        float p = (inv-0.5f)/0.5f; // 0→1
                        gx2 = sc.x - cosf(shipA)*25.f + cosf(shipA)*p*p*70.f;
                        gy2 = sc.y - sinf(shipA)*25.f + sinf(shipA)*p*p*70.f;
                        fa2 = shipA + 1.2f - p*0.8f;
                    }
                    // Gravity well singularity dot
                    float gwR = 8.f*sinf(inv*3.14159f);
                    DrawCircleV(sc,gwR,ColorAlpha({10,0,20,255},fadeA*0.9f));
                    DrawCircleV(sc,gwR*0.5f,ColorAlpha(VP_WHITE,fadeA*0.4f));
                    DrawCircleLinesV(sc,gwR,ColorAlpha(acol,fadeA*0.7f));
                    // accretion ring
                    for(int i=0;i<3;i++){
                        float ar = gwR*(1.3f+i*0.4f);
                        DrawCircleLinesV(sc,ar,ColorAlpha(acol,fadeA*(0.3f-i*0.08f)));
                    }
                    DrawShipSilhouette(g.selectedShip,{gx2,gy2},fa2,t,acol,inv>0.5f,inv>0.6f,g.shipUpgradeTier);
                    break;}

                case 14:{ // SOLAR INFERNO — ship does a fiery victory roll, leaving a blazing ring
                    float rollA = inv * 6.f * 3.14159f;
                    float rx = cosf(rollA)*28.f * sinf(inv*3.14159f);
                    float ry = sinf(rollA)*16.f * sinf(inv*3.14159f);
                    float sx2 = sc.x + rx;
                    float sy2 = sc.y + ry;
                    // fire ring trace at the roll perimeter
                    float traceFade = fmodf(inv*3.f, 1.f);
                    DrawCircleLinesV(sc, 28.f*sinf(inv*3.14159f)*0.8f, ColorAlpha(acol, fadeA*0.4f));
                    // hot sparks radiating outward
                    for(int i=0;i<20;i++){
                        float sa2 = i*(2.f*3.14159f/20.f)+inv*2.f;
                        float sl = (0.3f+fmodf(inv*4.f+i*0.17f,0.7f))*40.f;
                        DrawLineEx({sc.x+cosf(sa2)*10.f,sc.y+sinf(sa2)*10.f},
                                   {sc.x+cosf(sa2)*sl,  sc.y+sinf(sa2)*sl},
                                   1.f,ColorAlpha(i%2==0?VP_WHITE:acol,fadeA*(1.f-sl/40.f)*0.7f));
                    }
                    DrawShipSilhouette(g.selectedShip,{sx2,sy2},rollA,t,acol,true,false,g.shipUpgradeTier);
                    break;}

                case 15:{ // CORONA BREACH — ship launches upward into an erupting sunburst corona
                    float rise = inv*inv*inv*80.f; // cubic ease — slow then rockets
                    float sx2 = sc.x;
                    float sy2 = sc.y - rise;
                    // Corona rays emanate from launch point
                    int nRays = 16;
                    float rayLen = rise*0.6f + 20.f*sinf(inv*10.f);
                    for(int i=0;i<nRays;i++){
                        float ra = i*(2.f*3.14159f/nRays)+inv;
                        float rl = rayLen*(0.5f+0.5f*sinf(i*1.4f+inv*5.f));
                        DrawLineEx({sc.x,sc.y},{sc.x+cosf(ra)*rl,sc.y+sinf(ra)*rl},
                            1.2f,ColorAlpha(i%3==0?VP_WHITE:acol,fadeA*(1.f-rl/rayLen)*0.55f+0.15f));
                    }
                    DrawShipSilhouette(g.selectedShip,{sx2,sy2},-3.14159f/2.f,t,acol,true,false,g.shipUpgradeTier);
                    break;}

                case 16:{ // OBSIDIAN WASTES — ship does a cold, precise 360° scan sweep
                    float scanA = inv * 2.f * 3.14159f;
                    // scan beam sweeping out from ship
                    float beamLen = 80.f;
                    DrawLineEx(sc,{sc.x+cosf(scanA)*beamLen,sc.y+sinf(scanA)*beamLen},
                        1.5f,ColorAlpha(acol,fadeA*0.55f));
                    DrawLineEx(sc,{sc.x+cosf(scanA)*beamLen*0.6f,sc.y+sinf(scanA)*beamLen*0.6f},
                        3.f,ColorAlpha(acol,fadeA*0.15f));
                    // scan arc trail
                    for(int i=0;i<12;i++){
                        float ta = scanA - i*0.12f;
                        float tl = beamLen * (1.f - i*0.06f);
                        DrawLineEx(sc,{sc.x+cosf(ta)*tl,sc.y+sinf(ta)*tl},
                            0.8f,ColorAlpha(acol,fadeA*(0.3f-i*0.025f)));
                    }
                    DrawShipSilhouette(g.selectedShip,sc,scanA,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 17:{ // ASHEN CITADEL — ship drifts in slow ash-fall, ember motes descend
                    float sx2 = sc.x + sinf(inv*1.8f)*12.f;
                    float sy2 = sc.y + sinf(inv*1.2f)*6.f;
                    // Falling ash embers
                    for(int i=0;i<16;i++){
                        float ep = fmodf(inv*1.2f + i*0.0625f, 1.f);
                        float ex2 = sc.x + (i*37.f+sinf(i*2.3f)*20.f-100.f)*0.4f;
                        float ey2 = sc.y - 60.f + ep*120.f;
                        float er = 1.f+sinf(i*1.4f)*0.6f;
                        Color ec = i%3==0?VP_WHITE:i%3==1?VP_YELLOW:acol;
                        DrawCircleV({ex2,ey2},er,ColorAlpha(ec,fadeA*(1.f-ep)*0.7f));
                    }
                    DrawShipSilhouette(g.selectedShip,{sx2,sy2},shipA+sinf(inv*2.f)*0.15f,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 18:{ // THE VOID — ship hovers perfectly still while void tendrils retract from it
                    int nTendrils = 8;
                    for(int i=0;i<nTendrils;i++){
                        float ta = i*(2.f*3.14159f/nTendrils);
                        float tlen = (1.f-inv)*60.f; // tendrils retract over time
                        float tx2 = sc.x+cosf(ta)*tlen, ty2=sc.y+sinf(ta)*tlen;
                        // tendril with taper
                        for(int j=0;j<8;j++){
                            float jp = (float)j/8.f;
                            float jx = sc.x + cosf(ta)*tlen*jp + sinf(ta+jp)*4.f*sinf(inv*8.f+j);
                            float jx2= sc.x + cosf(ta)*tlen*(jp+0.125f) + sinf(ta+(jp+0.125f))*4.f*sinf(inv*8.f+j+1);
                            float jy = sc.y + sinf(ta)*tlen*jp - cosf(ta+jp)*4.f*sinf(inv*8.f+j);
                            float jy2= sc.y + sinf(ta)*tlen*(jp+0.125f) - cosf(ta+(jp+0.125f))*4.f*sinf(inv*8.f+j+1);
                            DrawLineEx({jx,jy},{jx2,jy2},(1.f-jp)*2.5f,
                                ColorAlpha(acol,fadeA*(1.f-jp)*0.55f));
                        }
                    }
                    DrawShipSilhouette(g.selectedShip,sc,shipA,t,acol,false,false,g.shipUpgradeTier);
                    break;}

                case 19:{ // VOID SINGULARITY — everything collapses to a point then blindingly explodes
                    if(inv < 0.45f){
                        // Phase 1: ship spirals into center (collapsing)
                        float p = inv/0.45f;
                        float cr = (1.f-p)*40.f;
                        float ca2 = p * 4.f * 3.14159f;
                        float sx2 = sc.x + cosf(ca2)*cr;
                        float sy2 = sc.y + sinf(ca2)*cr;
                        // everything being sucked inward — ring compresses
                        DrawCircleLinesV(sc,cr,ColorAlpha(acol,p*0.6f));
                        DrawShipSilhouette(g.selectedShip,{sx2,sy2},ca2,t,acol,true,false,g.shipUpgradeTier);
                    } else {
                        // Phase 2: singularity flash + ship bursts outward in all directions
                        float p = (inv-0.45f)/0.55f;
                        // white flash
                        DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_WHITE,(1.f-p)*0.6f*fadeA));
                        // explosion ring
                        float er = p*90.f;
                        DrawCircleLinesV(sc,er,ColorAlpha(acol,fadeA*(1.f-p)*0.8f));
                        DrawCircleLinesV(sc,er*0.6f,ColorAlpha(VP_WHITE,fadeA*(1.f-p)*0.5f));
                        // ship flies straight out
                        float bx = sc.x + cosf(shipA)*p*55.f;
                        float by2 = sc.y + sinf(shipA)*p*55.f;
                        DrawShipSilhouette(g.selectedShip,{bx,by2},shipA,t,acol,true,true,g.shipUpgradeTier);
                    }
                    break;}

                default:{ // Waves 20-27 — escalating chaos: more particles, faster spin, brighter flash
                    int wmod = prevWave - 20;
                    float spin = inv*(4.f+wmod)*3.14159f;
                    float burst = inv*(30.f+wmod*6.f);
                    // burst ring
                    int nBurst = 12+wmod*2;
                    for(int i=0;i<nBurst;i++){
                        float ba = i*(2.f*3.14159f/nBurst)+spin*0.2f;
                        float br2 = burst + sinf(inv*10.f+i)*8.f;
                        DrawLineEx({sc.x+cosf(ba)*12.f,sc.y+sinf(ba)*12.f},
                                   {sc.x+cosf(ba)*br2,sc.y+sinf(ba)*br2},
                                   1.5f,ColorAlpha(i%2==0?acol:VP_WHITE,fadeA*(1.f-br2/(burst+8.f))*0.8f));
                        DrawCircleV({sc.x+cosf(ba)*br2,sc.y+sinf(ba)*br2},1.5f,
                            ColorAlpha(acol,fadeA*0.7f));
                    }
                    DrawShipSilhouette(g.selectedShip,sc,spin,t,acol,true,true,g.shipUpgradeTier);
                    break;}

                } // end switch
                } // end else (animation active phase)
            } // end if waveTransTimer>0
            DrawAberration(dt);
            DrawScanlines();
            DrawVignette();
            // Pause overlay — drawn on top of frozen game world
            if(g.state==PAUSED) DrawPause(g,gUI,t,msc,clicked);
        }
        EndTextureMode();

        // ── Bloom pass: downsample game RT → half-res, then blit back additively ──
        BeginTextureMode(bloomRT);
        ClearBackground({0,0,0,0});
        DrawTexturePro(rt.texture,
            {0,0,(float)SCREEN_W,-(float)SCREEN_H},
            {0,0,(float)SCREEN_W/2,(float)SCREEN_H/2},
            {0,0},0,WHITE);
        EndTextureMode();

        BeginDrawing();
        ClearBackground({6,1,14,255});
        Rectangle gr=GameRect();
        // 1) main frame
        DrawTexturePro(rt.texture,{0,0,(float)SCREEN_W,-(float)SCREEN_H},gr,{0,0},0,WHITE);
        // 2) subtle bloom — single centered upscale pass, very low alpha
        {
            BeginBlendMode(BLEND_ADDITIVE);
            DrawTexturePro(bloomRT.texture,
                {0,0,(float)SCREEN_W/2,-(float)SCREEN_H/2},
                gr,{0,0},0,ColorAlpha(WHITE,0.06f));
            EndBlendMode();
        }
        DrawBorderPanels(g,gUI,t,dt);
        EndDrawing();
    }
    UnloadRenderTexture(rt); UnloadRenderTexture(bloomRT); CloseWindow(); return 0;
}

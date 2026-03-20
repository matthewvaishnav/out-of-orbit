// vfx.cpp — Spawn helpers, draw primitives, UI draw utilities
#include "void_blaster.h"

// Forward-declare internal static font helpers used in TF/CTF
// (UIState holds fonts — pass via thread-local or separate accessor)
// For full font-based helpers, callers pass the Font directly.

// ══════════════════════════════════════════════════════════════
void SpawnParticles(Game& g,Vector2 pos,Color col,int n,float spd,bool sparks){
    if((int)g.particles.size()>=PARTICLE_LIMIT) return; // hard cap
    for(int i=0;i<n;i++){
        float a=RandF(0,2*PI),s=RandF(20,spd);
        Particle p{}; p.pos=p.prevPos=pos;
        p.vel={cosf(a)*s,sinf(a)*s};
        p.life=p.maxLife=RandF(.3f,.85f); p.radius=RandF(1.5f,4.5f);
        p.color=col; p.isSpark=sparks;
        g.particles.push_back(p);
    }
}
void SpawnRing(Game& g,Vector2 pos,Color col,float maxR,float life){
    Particle p{}; p.pos=pos; p.isRing=true; p.ringMaxR=maxR;
    p.life=p.maxLife=life; p.color=col; p.radius=2;
    g.particles.push_back(p);
}
// ── Rich type-specific explosion burst ──
void SpawnExplosion(Game& g, Vector2 pos, Color col, float radius, int tier){
    if((int)g.particles.size()>=PARTICLE_LIMIT) return; // hard cap
    // tier: 1=small enemy, 2=medium, 3=large/boss
    int nSpark  = 8  + tier*6;
    int nDebris = 4  + tier*4;
    int nRings  = 1  + tier;
    float spd   = 80 + tier*55.f;

    // bright sparks
    for(int i=0;i<nSpark;i++){
        float a=RandF(0,2*PI), s=RandF(spd*.4f,spd);
        Particle p{}; p.pos=p.prevPos=pos;
        p.vel={cosf(a)*s,sinf(a)*s};
        p.life=p.maxLife=RandF(.25f,.7f+tier*.2f);
        p.radius=RandF(1.2f,2.5f+tier*.5f);
        p.color=col; p.isSpark=true;
        g.particles.push_back(p);
    }
    // white-hot core sparks
    for(int i=0;i<4+tier*2;i++){
        float a=RandF(0,2*PI), s=RandF(spd*.7f,spd*1.3f);
        Particle p{}; p.pos=p.prevPos=pos;
        p.vel={cosf(a)*s,sinf(a)*s};
        p.life=p.maxLife=RandF(.12f,.30f);
        p.radius=RandF(.8f,1.8f);
        p.color=VP_WHITE; p.isSpark=true;
        g.particles.push_back(p);
    }
    // spinning debris shards
    for(int i=0;i<nDebris;i++){
        float a=RandF(0,2*PI), s=RandF(20,spd*.6f);
        Particle p{}; p.pos=p.prevPos=pos;
        p.vel={cosf(a)*s,sinf(a)*s};
        p.life=p.maxLife=RandF(.4f,.9f+tier*.15f);
        p.radius=RandF(2.f,4.f+tier);
        p.color=LerpC(col,VP_WHITE,RandF(.1f,.5f));
        p.isShard=true;
        p.angle=RandF(0,2*PI);
        p.angVel=RandF(-8.f,8.f);
        p.stretch=RandF(2.f,4.f+tier);
        g.particles.push_back(p);
    }
    // glow blobs
    for(int i=0;i<3+tier;i++){
        float a=RandF(0,2*PI), s=RandF(10,spd*.35f);
        Particle p{}; p.pos=p.prevPos=pos;
        p.vel={cosf(a)*s,sinf(a)*s};
        p.life=p.maxLife=RandF(.3f,.6f);
        p.radius=RandF(4.f,8.f+tier*2);
        p.color=col;
        g.particles.push_back(p);
    }
    // expanding rings
    for(int ri=0;ri<nRings;ri++){
        float rLife=.35f+ri*.15f;
        SpawnRing(g,pos,ri==0?VP_WHITE:col,radius*(1.5f+ri*.8f),rLife);
    }
}

void SpawnPopup(Game& g,Vector2 pos,const std::string& t,Color c,int sz){
    PopupText pt; pt.pos=pos; pt.text=t; pt.color=c;
    pt.life=pt.maxLife=1.3f; pt.fontSize=sz; g.popups.push_back(pt);
}
void SpawnPickup(Game& g,Vector2 pos,int val){
    Pickup pk{}; pk.pos=pos; pk.vy=-28; pk.value=val;
    pk.life=200; pk.active=true; pk.spin=RandF(0,2*PI);
    g.pickups.push_back(pk);
}
void AddEngineTrail(Game& g){
    float ca=cosf(g.playerAngle), sa=sinf(g.playerAngle);
    float pspd=sqrtf(g.playerVel.x*g.playerVel.x+g.playerVel.y*g.playerVel.y);
    // Intensity: 1.0 when thrusting/dashing, fades to 0.45 as ship coasts to stop
    float intensity = (g.thrusterOn||g.dashTimer>0) ? 1.f : std::max(0.45f, pspd/84.f);
    // Primary exhaust trail — behind engine(s)
    for(int i=0;i<2;i++){
        float spreadX = RandF(-3.5f, 3.5f);
        float spreadY = RandF(-3.5f, 3.5f);
        TrailPt tp;
        tp.pos = {g.playerPos.x - ca*13.f + spreadX,
                  g.playerPos.y - sa*13.f + spreadY};
        tp.life = intensity;
        g.engineTrail.push_front(tp);
    }
    // For dashing: extra bright core streak point right at center
    if(g.dashTimer>0){
        TrailPt tp2; tp2.pos=g.playerPos; tp2.life=1.f;
        g.engineTrail.push_front(tp2);
    }
    while((int)g.engineTrail.size()>64) g.engineTrail.pop_back();
}

// ══════════════════════════════════════════════════════════════
//  FORWARD DECLS
// ══════════════════════════════════════════════════════════════

// ══════════════════════════════════════════════════════════════
//  ENEMY / BOSS SPAWNING
// ══════════════════════════════════════════════════════════════
// ══════════════════════════════════════════════════════════════
//  HIT / KILL LOGIC
// ══════════════════════════════════════════════════════════════
// ══════════════════════════════════════════════════════════════
//  DRAW UTILITY  (upgraded)
// ══════════════════════════════════════════════════════════════

// Modern panel with glassmorphism + dynamic glow
void DrawPanel(int x,int y,int w,int h,Color accent,float a){
    DrawRectangle(x-2,y-2,w+4,h+4,ColorAlpha(accent,0.06f*a));
    DrawRectangle(x-1,y-1,w+2,h+2,ColorAlpha(accent,0.11f*a));
    DrawRectangle(x,y,w,h,ColorAlpha(UI_BG_DARK,a));
    DrawRectangle(x,y,w,(h>5?2:1),ColorAlpha(VP_WHITE,0.05f*a));
    DrawRectangleLines(x,y,w,h,ColorAlpha(accent,.88f*a));
    Color tc=ColorAlpha(accent,.60f*a);
    DrawRectangle(x,y,5,1,tc);      DrawRectangle(x,y,1,5,tc);
    DrawRectangle(x+w-5,y,5,1,tc);  DrawRectangle(x+w-1,y,1,5,tc);
    DrawRectangle(x,y+h-1,5,1,tc);  DrawRectangle(x,y+h-5,1,5,tc);
    DrawRectangle(x+w-5,y+h-1,5,1,tc); DrawRectangle(x+w-1,y+h-5,1,5,tc);
}
void DrawBox(int x,int y,int w,int h,Color border,Color fill){
    // gradient fill: slightly lighter at top
    DrawRectangle(x,y,w,h,fill);
    DrawRectangle(x+1,y+1,w-2,h/3,ColorAlpha(VP_WHITE,.03f));
    // inner bevel: highlight top+left, shadow bottom+right
    DrawRectangle(x+1,y+1,w-2,1,ColorAlpha(VP_WHITE,.08f));
    DrawRectangle(x+1,y+1,1,h-2,ColorAlpha(VP_WHITE,.05f));
    DrawRectangle(x+1,y+h-2,w-2,1,ColorAlpha(VP_BLACK,.12f));
    DrawRectangle(x+w-2,y+1,1,h-2,ColorAlpha(VP_BLACK,.08f));
    // outer border with glow
    DrawRectangleLinesEx({(float)x-1,(float)y-1,(float)(w+2),(float)(h+2)},1,ColorAlpha(border,.12f));
    DrawRectangleLines(x,y,w,h,border);
    DrawRectangleLines(x+1,y+1,w-2,h-2,ColorAlpha(border,.18f));
    // rounded corner circles instead of square dots
    float cr=2.2f;
    DrawCircleV({(float)x+cr,(float)y+cr},cr,ColorAlpha(border,.7f));
    DrawCircleV({(float)(x+w)-cr,(float)y+cr},cr,ColorAlpha(border,.7f));
    DrawCircleV({(float)x+cr,(float)(y+h)-cr},cr,ColorAlpha(border,.7f));
    DrawCircleV({(float)(x+w)-cr,(float)(y+h)-cr},cr,ColorAlpha(border,.7f));
    DrawCircleV({(float)x+cr,(float)y+cr},1.f,ColorAlpha(VP_WHITE,.35f));
    DrawCircleV({(float)(x+w)-cr,(float)y+cr},1.f,ColorAlpha(VP_WHITE,.35f));
    DrawCircleV({(float)x+cr,(float)(y+h)-cr},1.f,ColorAlpha(VP_WHITE,.35f));
    DrawCircleV({(float)(x+w)-cr,(float)(y+h)-cr},1.f,ColorAlpha(VP_WHITE,.35f));
}

// Text with drop-shadow for readability
void T(const std::string& s,int x,int y,int sz,Color c){
    DrawText(s.c_str(),x+1,y+1,sz,ColorAlpha({0,0,0,255},.55f));
    DrawText(s.c_str(),x,y,sz,c);
}
void CT(const std::string& s,int y,int sz,Color c){
    int w=MeasureText(s.c_str(),sz);
    DrawText(s.c_str(),(SCREEN_W-w)/2+1,y+1,sz,ColorAlpha({0,0,0,255},.55f));
    DrawText(s.c_str(),(SCREEN_W-w)/2,y,sz,c);
}
// Glowing text: enhanced with multiple glow layers
void TGlow(const std::string& s,int x,int y,int sz,Color c,float gStrength){
    // far outer glow
    DrawText(s.c_str(),x+3,y+3,sz,ColorAlpha(c,0.04f*gStrength));
    // mid glow
    DrawText(s.c_str(),x+2,y+2,sz,ColorAlpha(c,0.08f*gStrength));
    // near glow
    DrawText(s.c_str(),x-1,y,sz,ColorAlpha(c,0.16f*gStrength));
    DrawText(s.c_str(),x+1,y,sz,ColorAlpha(c,0.16f*gStrength));
    DrawText(s.c_str(),x,y-1,sz,ColorAlpha(c,0.14f*gStrength));
    DrawText(s.c_str(),x,y+1,sz,ColorAlpha(c,0.14f*gStrength));
    // shadow
    DrawText(s.c_str(),x+1,y+1,sz,ColorAlpha({0,0,0,255},.75f));
    // crisp top
    DrawText(s.c_str(),x,y,sz,c);
}
void CTGlow(const std::string& s,int y,int sz,Color c,float gs){
    int w=MeasureText(s.c_str(),sz); TGlow(s,(SCREEN_W-w)/2,y,sz,c,gs);
}

// Font-aware text helpers using loaded TTF fonts
void TF(const std::string& s,int x,int y,int sz,Color c,Font f){
    float sp=sz*0.04f;
    DrawTextEx(f,s.c_str(),{(float)(x+1),(float)(y+1)},(float)sz,sp,ColorAlpha({0,0,0,255},.55f));
    DrawTextEx(f,s.c_str(),{(float)x,(float)y},(float)sz,sp,c);
}
void CTF(const std::string& s,int y,int sz,Color c,Font f){
    float sp=sz*0.04f;
    Vector2 m=MeasureTextEx(f,s.c_str(),(float)sz,sp);
    float x=(SCREEN_W-m.x)*.5f;
    DrawTextEx(f,s.c_str(),{x+1,(float)(y+1)},(float)sz,sp,ColorAlpha({0,0,0,255},.55f));
    DrawTextEx(f,s.c_str(),{x,(float)y},(float)sz,sp,c);
}
void TGlowF(const std::string& s,int x,int y,int sz,Color c,float gs,Font f){
    float sp=sz*0.04f;
    auto D=[&](float ox,float oy,Color cc){ DrawTextEx(f,s.c_str(),{(float)x+ox,(float)y+oy},(float)sz,sp,cc); };
    D(3,3,ColorAlpha(c,.04f*gs)); D(2,2,ColorAlpha(c,.08f*gs));
    D(-1,0,ColorAlpha(c,.16f*gs)); D(1,0,ColorAlpha(c,.16f*gs));
    D(0,-1,ColorAlpha(c,.14f*gs)); D(0,1,ColorAlpha(c,.14f*gs));
    D(1,1,ColorAlpha({0,0,0,255},.75f)); D(0,0,c);
}
void CTGlowF(const std::string& s,int y,int sz,Color c,float gs,Font f){
    float sp=sz*0.04f;
    Vector2 m=MeasureTextEx(f,s.c_str(),(float)sz,sp);
    TGlowF(s,(int)((SCREEN_W-m.x)*.5f),y,sz,c,gs,f);
}

void DrawNeonLine(Vector2 a,Vector2 b,Color c,float thick,float alpha){
    // 5-layer bloom for maximum neon richness
    DrawLineEx(a,b,thick+10,ColorAlpha(c,alpha*.02f));
    DrawLineEx(a,b,thick+6, ColorAlpha(c,alpha*.06f));
    DrawLineEx(a,b,thick+4, ColorAlpha(c,alpha*.12f));
    DrawLineEx(a,b,thick+2, ColorAlpha(c,alpha*.22f));
    DrawLineEx(a,b,thick,   ColorAlpha(c,alpha));
}
void DrawNeonCircle(Vector2 c,float r,Color col,float alpha){
    // 5-layer bloom for vivid neon circles
    DrawCircleLinesV(c,r+5,ColorAlpha(col,alpha*.03f));
    DrawCircleLinesV(c,r+3,ColorAlpha(col,alpha*.07f));
    DrawCircleLinesV(c,r+2,ColorAlpha(col,alpha*.16f));
    DrawCircleLinesV(c,r+1,ColorAlpha(col,alpha*.30f));
    DrawCircleLinesV(c,r,  ColorAlpha(col,alpha*.95f));
}
// Filled neon shape: glowing outline + semi-transparent fill
void DrawNeonFill(Vector2 sp,float r,Color c,float fillA,float outA){
    DrawCircleV(sp,r,ColorAlpha(c,fillA));
    DrawCircleV(sp,r+3,ColorAlpha(c,fillA*.3f));
    DrawNeonCircle(sp,r,c,outA);
}
// Draw a thick glowing polygon edge
void DrawNeonPoly(Vector2* pts,int n,Color c,float thick,float alpha){
    for(int i=0;i<n;i++) DrawNeonLine(pts[i],pts[(i+1)%n],c,thick,alpha);
}
// Horizontal bar (for HP / progress) with glow rim
void DrawGlowBar(int x,int y,int w,int h,float pct,Color col,Color bg){
    DrawRectangle(x,y,w,h,bg);
    DrawRectangleLines(x,y,w,h,ColorAlpha(col,.28f));
    int fw=(int)(w*pct);
    if(fw>0){
        DrawRectangle(x,y,fw,h,ColorAlpha(col,.60f));
        DrawRectangle(x,y,fw,1,ColorAlpha(VP_WHITE,.28f));
        if(fw>2) DrawRectangle(x,y,fw-1,h-1,ColorAlpha(col,.15f));
        if(pct<0.999f && pct>0.01f) DrawRectangle(x+fw-1,y,2,h,ColorAlpha(VP_WHITE,.55f));
    }
}
void DrawScanlines(){
    static float scanT = 0.f; scanT += 0.4f; if(scanT > SCREEN_H) scanT = 0.f;
    // Subtle horizontal lines
    for(int y=0;y<SCREEN_H;y+=2) DrawRectangle(0,y,SCREEN_W,1,ColorAlpha(VP_BLACK,.08f));
    for(int y=0;y<SCREEN_H;y+=6) DrawRectangle(0,y,SCREEN_W,1,ColorAlpha(VP_WHITE,.012f));
    // Enhanced CRT sweep with glow
    int sy=(int)scanT;
    DrawRectangle(0,sy,   SCREEN_W,3,ColorAlpha(VP_WHITE,.035f));
    DrawRectangle(0,sy+3, SCREEN_W,2,ColorAlpha(VP_WHITE,.015f));
}
void DrawVignette(){
    // Smooth corner vignette with gradual fade
    for(int i=0;i<20;i++){
        float t = (float)i/20.f;
        float alpha = (0.025f + t*0.035f) * (1.f - t*0.5f);
        if(alpha<=0) break;
        DrawRectangleLinesEx({(float)i,(float)i,(float)(SCREEN_W-i*2),(float)(SCREEN_H-i*2)},1,ColorAlpha(VP_BLACK,alpha));
    }
    // Modern color tone overlay
    DrawRectangleLinesEx({0,0,(float)SCREEN_W,(float)SCREEN_H},3,ColorAlpha(VP_DARKPURP,.08f));
    DrawRectangleLinesEx({2,2,(float)SCREEN_W-4,(float)SCREEN_H-4},2,ColorAlpha(VP_PURPLE,.04f));
}

// ══════════════════════════════════════════════════════════════
//  MODERN UI HELPERS (2025 refresh)
// ══════════════════════════════════════════════════════════════

// Animated corner brackets with configurable style
void DrawModernCorners(int x,int y,int w,int h,Color c,float t,float intensity,bool animated){
    float pulse=animated?(0.7f+0.3f*sinf(t*3.f)):1.f;
    float a=intensity*pulse;
    int arm=12;
    int thick=2;
    // Top-left
    DrawLineEx({(float)x,(float)y+arm},{(float)x,(float)y},thick,ColorAlpha(c,a));
    DrawLineEx({(float)x,(float)y},{(float)x+arm,(float)y},thick,ColorAlpha(c,a));
    // Top-right
    DrawLineEx({(float)(x+w),(float)y+arm},{(float)(x+w),(float)y},thick,ColorAlpha(c,a));
    DrawLineEx({(float)(x+w),(float)y},{(float)(x+w)-arm,(float)y},thick,ColorAlpha(c,a));
    // Bottom-left
    DrawLineEx({(float)x,(float)(y+h)-arm},{(float)x,(float)(y+h)},thick,ColorAlpha(c,a));
    DrawLineEx({(float)x,(float)(y+h)},{(float)x+arm,(float)(y+h)},thick,ColorAlpha(c,a));
    // Bottom-right
    DrawLineEx({(float)(x+w),(float)(y+h)-arm},{(float)(x+w),(float)(y+h)},thick,ColorAlpha(c,a));
    DrawLineEx({(float)(x+w),(float)(y+h)},{(float)(x+w)-arm,(float)(y+h)},thick,ColorAlpha(c,a));
    // Corner dots
    DrawCircleV({(float)x,(float)y},2.5f,ColorAlpha(VP_GOLD,a*0.9f));
    DrawCircleV({(float)(x+w),(float)y},2.5f,ColorAlpha(VP_GOLD,a*0.9f));
    DrawCircleV({(float)x,(float)(y+h)},2.5f,ColorAlpha(VP_GOLD,a*0.9f));
    DrawCircleV({(float)(x+w),(float)(y+h)},2.5f,ColorAlpha(VP_GOLD,a*0.9f));
}

// Modern glass panel with layered depth
void DrawGlassPanel(int x,int y,int w,int h,Color accent,float t,float alpha,bool animated){
    float glow=animated?(0.06f+0.04f*sinf(t*2.f)):0.08f;
    // Outer glow
    DrawRectangle(x-4,y-4,w+8,h+8,ColorAlpha(accent,glow*0.3f*alpha));
    DrawRectangle(x-2,y-2,w+4,h+4,ColorAlpha(accent,glow*0.6f*alpha));
    // Main background with depth
    DrawRectangle(x,y,w,h,ColorAlpha(SURFACE_0,0.95f*alpha));
    DrawRectangle(x+1,y+1,w-2,h/3,ColorAlpha(SURFACE_1,0.4f*alpha));
    DrawRectangle(x+1,y+1,w-2,2,ColorAlpha(VP_WHITE,0.08f*alpha));
    // Bottom shadow
    DrawRectangle(x+1,y+h-2,w-2,1,ColorAlpha(VP_BLACK,0.15f*alpha));
    // Accent bar at top
    DrawRectangle(x,y,w/2,2,ColorAlpha(accent,0.8f*alpha));
    DrawRectangle(x+w/2,y,(w+1)/2,2,ColorAlpha(accent,0.4f*alpha));
    // Border
    DrawRectangleLines(x,y,w,h,ColorAlpha(accent,0.5f*alpha));
    DrawRectangleLines(x+1,y+1,w-2,h-2,ColorAlpha(accent,0.15f*alpha));
    // Corners
    DrawModernCorners(x,y,w,h,accent,t,alpha*0.8f,animated);
}

// Clean modern card with subtle depth
void DrawCard(int x,int y,int w,int h,Color accent,bool hovered,float t,float alpha){
    float hGlow=hovered?(0.15f+0.08f*sinf(t*4.f)):0.f;
    // Shadow
    DrawRectangle(x+2,y+2,w,h,ColorAlpha(VP_BLACK,0.4f*alpha));
    // Background
    DrawRectangle(x,y,w,h,ColorAlpha(SURFACE_1,alpha));
    // Top sheen
    DrawRectangle(x+1,y+1,w-2,3,ColorAlpha(VP_WHITE,0.06f*alpha));
    // Hover glow
    if(hGlow>0.001f) DrawRectangle(x,y,w,h,ColorAlpha(accent,hGlow*alpha));
    // Border
    Color bc=hovered?accent:ColorAlpha(VP_WHITE,0.15f);
    DrawRectangleLines(x,y,w,h,ColorAlpha(bc,alpha));
    DrawRectangleLines(x+1,y+1,w-2,h-2,ColorAlpha(bc,0.1f*alpha));
}

// Modern progress bar with glow tip
void DrawModernBar(int x,int y,int w,int h,float pct,Color col,Color bgCol,float t){
    DrawRectangle(x,y,w,h,ColorAlpha(bgCol,0.5f));
    DrawRectangleLines(x,y,w,h,ColorAlpha(col,0.2f));
    int fw=(int)(w*pct);
    if(fw>0){
        // Bar fill with gradient
        DrawRectangle(x,y,fw,h,ColorAlpha(col,0.7f));
        DrawRectangle(x,y,fw,1,ColorAlpha(VP_WHITE,0.3f));
        DrawRectangle(x,y+h-1,fw,1,ColorAlpha(VP_BLACK,0.2f));
        // Glow tip
        if(pct<1.f){
            float tipPulse=0.6f+0.4f*sinf(t*8.f);
            DrawRectangle(x+fw-1,y,2,h,ColorAlpha(VP_WHITE,tipPulse));
        }
    }
    // Segment marks at 25/50/75%
    for(int i=1;i<4;i++){
        int mx=x+(int)(w*i/4.f);
        DrawRectangle(mx,y-1,1,h+2,ColorAlpha(VP_BLACK,0.5f));
    }
}

// Modern text with cleaner shadow
void DrawTextModern(const char* text,int x,int y,int size,Color c,bool centered){
    if(centered){ int w=MeasureText(text,size); x-=w/2; }
    DrawText(text,x+1,y+1,size,ColorAlpha(VP_BLACK,0.5f));
    DrawText(text,x,y,size,c);
}

// Text with subtle glow effect
void DrawTextGlow(const char* text,int x,int y,int size,Color c,float strength){
    for(int i=3;i>=1;i--){
        float a=0.03f*strength*(4-i);
        DrawText(text,x+i,y+i,size,ColorAlpha(c,a));
    }
    DrawText(text,x+1,y+1,size,ColorAlpha(VP_BLACK,0.5f));
    DrawText(text,x,y,size,c);
}

// Centered text glow
void DrawTextGlowC(const char* text,int y,int size,Color c,float strength){
    int w=MeasureText(text,size);
    int x=(SCREEN_W-w)/2;
    DrawTextGlow(text,x,y,size,c,strength);
}

// Section header with accent underline
void DrawSectionHeader(const char* text,int x,int y,int size,Color c,int w){
    int tw=MeasureText(text,size);
    if(w==0) w=tw+20;
    // Text
    DrawTextModern(text,x,y,size,TEXT_PRIMARY,false);
    // Underline
    int ly=y+size+2;
    DrawRectangle(x,ly,std::min(tw,w),2,ColorAlpha(c,0.8f));
    DrawRectangle(x+tw/2,ly-1,2,4,ColorAlpha(VP_GOLD,0.8f));
}

// Modern segmented control / pill button
bool DrawPillButton(const char* text,int x,int y,int w,int h,Color accent,bool selected,float t){
    float pulse=selected?(0.8f+0.2f*sinf(t*3.f)):1.f;
    // Shadow
    DrawRectangle(x+1,y+1,w,h,ColorAlpha(VP_BLACK,0.4f));
    // Background
    Color bg=selected?ColorAlpha(accent,0.25f):ColorAlpha(SURFACE_2,0.8f);
    DrawRectangle(x,y,w,h,bg);
    // Border
    Color bc=selected?accent:ColorAlpha(VP_WHITE,0.2f);
    DrawRectangleLines(x,y,w,h,ColorAlpha(bc,pulse));
    // Top highlight
    DrawRectangle(x+1,y+1,w-2,1,ColorAlpha(VP_WHITE,selected?0.1f:0.05f));
    // Text
    int tw=MeasureText(text,8);
    Color tc=selected?TEXT_PRIMARY:TEXT_SECONDARY;
    DrawTextModern(text,x+(w-tw)/2,y+(h-8)/2+1,8,tc,false);
    return false; // Return true if clicked (caller checks)
}

// Animated background particles
void DrawAmbientParticles(float t,int count){
    for(int i=0;i<count;i++){
        float px=(float)((i*17)%SCREEN_W);
        float py=(float)((i*13+i*37)%SCREEN_H);
        float life=0.5f+0.5f*sinf(t*0.5f+i*1.37f);
        float size=0.5f+life*1.5f;
        Color c=(i%3==0)?VP_CYAN:(i%3==1)?VP_PINK:VP_PURPLE;
        DrawCircleV({px,py},size,ColorAlpha(c,life*0.15f));
    }
}

// Modern grid background
void DrawModernGrid(float t,float alpha){
    // Subtle vertical lines
    for(int x=0;x<SCREEN_W;x+=40){
        float a=0.03f+0.02f*sinf(t*0.3f+x*0.02f);
        DrawLine(x,0,x,SCREEN_H,ColorAlpha(VP_PURPLE,a*alpha));
    }
    // Subtle horizontal lines
    for(int y=0;y<SCREEN_H;y+=40){
        float a=0.03f+0.01f*sinf(t*0.2f+y*0.01f);
        DrawLine(0,y,SCREEN_W,y,ColorAlpha(VP_DEEPPURP,a*alpha));
    }
}

// ══════════════════════════════════════════════════════════════
//  BACKGROUND  ─  CINEMATIC LAYERS

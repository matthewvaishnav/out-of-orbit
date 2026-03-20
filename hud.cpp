// hud.cpp — HUD, radar, nav, kill feed, and border panels
#include "void_blaster.h"

void DrawRadar(Game& g,float t){
    const int RX=SCREEN_W-104,RY=SCREEN_H-98,RW=96,RH=86;
    float cx=(float)(RX+RW/2), cy2=(float)(RY+RH/2);
    float maxR=RW*.49f;

    // ── Outer panel — clean single border with top accent ──
    DrawRectangle(RX-4,RY-18,RW+8,RH+22,ColorAlpha(VP_BLACK,.88f));
    DrawRectangleLines(RX-4,RY-18,RW+8,RH+22,ColorAlpha(VP_PURPLE,.62f));
    // top accent bar + inner highlight
    DrawRectangle(RX-4,RY-18,RW+8,2,ColorAlpha(VP_PURPLE,.65f));
    DrawRectangle(RX-4,RY-16,RW+8,1,ColorAlpha(VP_WHITE,.04f));
    // left side accent
    DrawRectangle(RX-4,RY-18,2,RH+22,ColorAlpha(VP_PURPLE,.40f));

    // header
    TGlow("RADAR",RX+2,RY-14,7,VP_PURPLE,1.0f);
    // sweep angle indicator tick (right side of header)
    float sweepDeg=fmodf(t*1.3f*180/PI,360);
    std::string degStr=IStr((int)sweepDeg)+(char)0xB0;
    DrawText(degStr.c_str(),RX+RW-MeasureText(degStr.c_str(),6)-2,RY-13,6,ColorAlpha(VP_MINT,.50f));

    // ── Scope background ──
    DrawRectangle(RX,RY,RW,RH,ColorAlpha({2,8,18,255},.95f));
    // scanline overlay on scope
    for(int sy2=RY;sy2<RY+RH;sy2+=3)
        DrawRectangle(RX,sy2,RW,1,ColorAlpha(VP_BLACK,.08f));

    float sweepA=fmodf(t*1.3f,2*PI);

    // ── 4 concentric range rings ──
    for(int ri=1;ri<=4;ri++){
        float rr=maxR*ri/4.f;
        DrawCircleLinesV({cx,cy2},rr,ColorAlpha(VP_PURPLE,ri==4?.22f:.10f));
    }
    // crosshair lines
    DrawLine((int)(cx-maxR),RY+RH/2,(int)(cx+maxR),RY+RH/2,ColorAlpha(VP_PURPLE,.18f));
    DrawLine(RX+RW/2,RY,RX+RW/2,RY+RH,ColorAlpha(VP_PURPLE,.18f));
    // diagonal guides
    DrawLine((int)(cx-maxR*.7f),(int)(cy2-maxR*.7f),(int)(cx+maxR*.7f),(int)(cy2+maxR*.7f),ColorAlpha(VP_PURPLE,.06f));
    DrawLine((int)(cx+maxR*.7f),(int)(cy2-maxR*.7f),(int)(cx-maxR*.7f),(int)(cy2+maxR*.7f),ColorAlpha(VP_PURPLE,.06f));

    // ── Sweep trail (10 fade steps) ──
    for(int i=0;i<18;i++){
        float sa=sweepA-i*(PI/28.f);
        float ex=(float)(cx+cosf(sa)*maxR);
        float ey=(float)(cy2+sinf(sa)*maxR);
        DrawLineEx({cx,cy2},{ex,ey},1.2f,ColorAlpha(VP_MINT,.040f*(18-i)));
    }
    // live sweep line
    DrawLineEx({cx,cy2},{cx+cosf(sweepA)*maxR,cy2+sinf(sweepA)*maxR},
               1.8f,ColorAlpha(VP_MINT,.62f));
    // sweep tip glow
    DrawCircleV({cx+cosf(sweepA)*maxR,cy2+sinf(sweepA)*maxR},2.5f,ColorAlpha(VP_MINT,.55f));

    // ── Entities ──
    // Player-centered radar view — show entities within VIEW_R world units
    const float VIEW_R = 900.f;
    auto wToR=[&](Vector2 wpos)->Vector2{
        return {cx+(wpos.x-g.playerPos.x)/VIEW_R*maxR,
                cy2+(wpos.y-g.playerPos.y)/VIEW_R*maxR};
    };
    // viewport indicator (small centered box showing screen coverage)
    float vpHalf_x = (SCREEN_W*0.5f)/VIEW_R*maxR;
    float vpHalf_y = (SCREEN_H*0.5f)/VIEW_R*maxR;
    DrawRectangleLines((int)(cx-vpHalf_x),(int)(cy2-vpHalf_y),
        (int)(vpHalf_x*2),(int)(vpHalf_y*2),ColorAlpha(VP_CYAN,.22f));

    // enemy blips — color-coded by type, glow on recent sweep
    for(auto& e:g.enemies)if(e.active){
        Vector2 ep2=wToR(e.pos);
        float ex2=ep2.x, ey2=ep2.y;
        if(fabsf(ex2-cx)>maxR||fabsf(ey2-cy2)>maxR)continue;
        // glow if sweep just passed over this angle
        float eA=atan2f(ey2-cy2,ex2-cx);
        float aDiff=fmodf(sweepA-eA+4*PI,2*PI);
        float blipGlow=aDiff<0.5f?(1.f-aDiff*2.f):.0f;
        if(blipGlow>0)
            DrawCircleV({ex2,ey2},4.f,ColorAlpha(e.col,blipGlow*.45f));
        DrawRectangle((int)ex2-1,(int)ey2-1,3,3,ColorAlpha(e.col,.75f+blipGlow*.22f));
    }
    if(g.boss.active){
        Vector2 bp3=wToR(g.boss.pos);
        float bx2=bp3.x, by2=bp3.y;
        if(fabsf(bx2-cx)<=maxR+8&&fabsf(by2-cy2)<=maxR+8){
        float bPulse=.6f+.4f*sinf(t*6);
        DrawCircleV({bx2,by2},5.5f,ColorAlpha(VP_HOTPINK,bPulse*.20f));
        DrawCircleV({bx2,by2},3.5f,ColorAlpha(VP_HOTPINK,bPulse*.85f));
        DrawCircleV({bx2,by2},1.8f,VP_WHITE);
        }
    }
    // player blip — always at center of radar
    float ppx=cx, ppy=cy2;
    DrawCircleV({ppx,ppy},4.5f,ColorAlpha(VP_CYAN,.22f));
    DrawCircleV({ppx,ppy},3.0f,VP_CYAN);
    DrawCircleV({ppx,ppy},1.5f,VP_WHITE);
    // player direction tick
    DrawLineEx({ppx,ppy},{ppx+cosf(g.playerAngle)*6,ppy+sinf(g.playerAngle)*6},
               1.2f,ColorAlpha(VP_MINT,.70f));

    // ── Scope border ──
    DrawRectangleLines(RX,RY,RW,RH,ColorAlpha(VP_PURPLE,.50f));
    DrawRectangle(RX,RY,RW,1,ColorAlpha(VP_WHITE,.04f));
}

void DrawNav(Game& g,float t,float dashCooldown){
    float spd=sqrtf(g.playerVel.x*g.playerVel.x+g.playerVel.y*g.playerVel.y);
    bool hasWarp=g.ucache.warpcore;
    bool hasAfterburn=g.ucache.afterburn;
    int panW=118, panH=hasWarp?66:54;
    int panX=4, panY=SCREEN_H-panH-4;
    ShipType st=(ShipType)g.selectedShip;
    Color navCol=(st==SHIP_INTERCEPTOR)?VP_CYAN:(st==SHIP_BRAWLER)?VP_ORANGE:(st==SHIP_PHANTOM)?VP_PURPLE:VP_MINT;

    // ── Panel shell — refined single border ──
    DrawRectangle(panX,panY,panW,panH,ColorAlpha(VP_BLACK,.90f));
    // left color accent bar
    DrawRectangle(panX,panY,2,panH,ColorAlpha(navCol,.60f));
    // top highlight line
    DrawRectangle(panX+2,panY,panW-2,1,ColorAlpha(navCol,.45f));
    DrawRectangle(panX+2,panY+1,panW-2,1,ColorAlpha(VP_WHITE,.05f));
    // outer border
    DrawRectangleLines(panX,panY,panW,panH,ColorAlpha(navCol,.52f));

    // ── Compass rose (right side) ──
    int ccx=panX+panW-16, ccy=panY+18;
    float cr=12.f;
    // outer ring
    DrawCircleV({(float)ccx,(float)ccy},cr+1.5f,ColorAlpha(navCol,.08f));
    DrawCircleV({(float)ccx,(float)ccy},cr,ColorAlpha(VP_BLACK,.80f));
    DrawNeonCircle({(float)ccx,(float)ccy},cr,navCol,.45f);
    // inner ring
    DrawCircleLinesV({(float)ccx,(float)ccy},cr*.6f,ColorAlpha(navCol,.18f));
    // 8-tick cardinal marks
    for(int d=0;d<8;d++){
        float da=d*(PI*.25f);
        float r0=d%2==0?cr*.65f:cr*.78f;
        DrawLineEx({(float)(ccx+cosf(da)*r0),(float)(ccy+sinf(da)*r0)},
                   {(float)(ccx+cosf(da)*cr),(float)(ccy+sinf(da)*cr)},
                   d%2==0?1.4f:.8f,ColorAlpha(navCol,d%2==0?.70f:.35f));
    }
    // heading needle
    float na=g.playerAngle;
    DrawLineEx({(float)ccx,(float)ccy},
        {(float)(ccx+cosf(na)*cr*.88f),(float)(ccy+sinf(na)*cr*.88f)},
        2.0f,navCol);
    // aft tick (opposite direction, dimmer)
    DrawLineEx({(float)ccx,(float)ccy},
        {(float)(ccx+cosf(na+PI)*cr*.5f),(float)(ccy+sinf(na+PI)*cr*.5f)},
        1.0f,ColorAlpha(navCol,.32f));
    DrawCircleV({(float)ccx,(float)ccy},2.5f,VP_WHITE);

    // ── Text data ──
    int tx=panX+5, ty=panY+4;
    TGlow("NAV",tx,ty,7,navCol,.90f);
    // heading degrees (top right of text column)
    int headDeg=(int)((g.playerAngle*180.f/PI)+360)%360;
    std::string hdgStr=IStr(headDeg)+(char)0xB0;
    DrawText(hdgStr.c_str(),panX+panW-28-MeasureText(hdgStr.c_str(),6),ty+1,6,ColorAlpha(navCol,.55f));
    ty+=11;
    DrawLine(panX+2,ty,panX+panW-2,ty,ColorAlpha(navCol,.20f)); ty+=3;

    // coords
    std::string xs="X:"+IStr((int)g.playerPos.x);
    std::string ys="Y:"+IStr((int)g.playerPos.y);
    T(xs,tx,ty,7,VP_MINT); ty+=9;
    T(ys,tx,ty,7,VP_MINT); ty+=9;

    // speed with mini bar
    float maxSpd=PLAYER_SPEED*(hasAfterburn?1.6f:1.f);
    bool atMax=(spd>=maxSpd*.96f&&spd>10.f);
    float spdPct=std::min(1.f,spd/maxSpd);
    // speed bar (narrow, right of text)
    int bx=panX+50, bw2=panW-bx+panX-6;
    DrawRectangle(bx,ty,bw2,7,ColorAlpha(VP_BLACK,.65f));
    DrawRectangle(bx,ty,(int)(bw2*spdPct),7,ColorAlpha(atMax?VP_GOLD:navCol,atMax?.85f:.55f));
    DrawRectangle(bx,ty,bw2,1,ColorAlpha(VP_WHITE,.06f));
    DrawRectangleLines(bx,ty,bw2,7,ColorAlpha(navCol,.30f));
    if(atMax && hasAfterburn){
        float ab=.6f+.4f*sinf(t*7.f);
        TGlow("BURN",tx,ty,7,ColorAlpha(VP_GOLD,ab),1.1f);
    } else if(atMax){
        TGlow("MAX",tx,ty,7,VP_MINT,.90f);
    } else {
        T(("V:"+IStr((int)spd)).c_str(),tx,ty,7,VP_YELLOW);
    }
    ty+=9;

    // blink dash bar
    if(hasWarp){
        float pct=1.f-std::min(1.f,dashCooldown/1.5f);
        Color bc=pct>=1?VP_CYAN:VP_MIDGRAY;
        DrawLine(panX+2,ty,panX+panW-2,ty,ColorAlpha(navCol,.18f)); ty+=4;
        DrawRectangle(panX+3,ty,panW-6,7,ColorAlpha(VP_BLACK,.65f));
        DrawRectangle(panX+3,ty,(int)((panW-6)*pct),7,ColorAlpha(bc,pct>=1?.80f:.42f));
        DrawRectangle(panX+3,ty,panW-6,1,ColorAlpha(VP_WHITE,.05f));
        DrawRectangleLines(panX+3,ty,panW-6,7,ColorAlpha(bc,.35f));
        if(pct>=1){
            float bp=.7f+.3f*sinf(t*5);
            int bw3=MeasureText("BLINK RDY",7);
            TGlow("BLINK RDY",panX+(panW-bw3)/2,ty,7,ColorAlpha(VP_CYAN,bp),1.f);
        }
    }
}
void DrawHUD(Game& g,float t){
    ShipType st=(ShipType)g.selectedShip;
    Color tCol=(st==SHIP_INTERCEPTOR)?ACCENT_CYAN:(st==SHIP_BRAWLER)?ACCENT_PINK:(st==SHIP_PHANTOM)?ACCENT_PURPLE:ACCENT_MINT;
    Color pCol=PrayerColor(g.prayer);
    float pulse=sinf(t*2.5f)*.5f+.5f;

    // ── MODERN SCREEN FRAME — clean animated corners ──
    DrawModernCorners(0,0,SCREEN_W,SCREEN_H,tCol,t,0.7f,true);
    // Subtle vignette
    for(int vi=0;vi<6;vi++){
        float va=.015f*(6-vi);
        DrawRectangle(0,vi,SCREEN_W,1,ColorAlpha(VP_BLACK,va));
        DrawRectangle(0,SCREEN_H-1-vi,SCREEN_W,1,ColorAlpha(VP_BLACK,va));
        DrawRectangle(vi,0,1,SCREEN_H,ColorAlpha(VP_BLACK,va*.6f));
        DrawRectangle(SCREEN_W-1-vi,0,1,SCREEN_H,ColorAlpha(VP_BLACK,va*.6f));
    }

    // Side-center tick marks (targeting HUD style)
    {
        float tp=.25f+.08f*sinf(t*1.2f);
        int tk=4;
        DrawLineEx({(float)(SCREEN_W/2-tk),1},{(float)(SCREEN_W/2+tk),1},1,ColorAlpha(tCol,tp));
        DrawLineEx({(float)(SCREEN_W/2-tk),(float)(SCREEN_H-2)},{(float)(SCREEN_W/2+tk),(float)(SCREEN_H-2)},1,ColorAlpha(tCol,tp));
        DrawLineEx({1,(float)(SCREEN_H/2-tk)},{1,(float)(SCREEN_H/2+tk)},1,ColorAlpha(tCol,tp));
        DrawLineEx({(float)(SCREEN_W-2),(float)(SCREEN_H/2-tk)},{(float)(SCREEN_W-2),(float)(SCREEN_H/2+tk)},1,ColorAlpha(tCol,tp));
    }

    // Modern panel border helper — cleaner glass style
    auto PanelBorder=[&](int px,int py,int pw,int ph,Color pc,bool critical=false){
        // Glass background
        DrawRectangle(px,py,pw,ph,ColorAlpha(SURFACE_0,0.85f));
        DrawRectangle(px+1,py+1,pw-2,ph/3,ColorAlpha(pc,.03f));
        // Inner bevel
        DrawRectangle(px+1,py+1,pw-2,1,ColorAlpha(VP_WHITE,.05f));
        DrawRectangle(px+1,py+1,1,ph-2,ColorAlpha(VP_WHITE,.03f));
        DrawRectangle(px+1,py+ph-2,pw-2,1,ColorAlpha(VP_BLACK,.12f));
        // Top accent
        DrawRectangle(px,py,pw/2,2,ColorAlpha(pc,.6f));
        DrawRectangle(px+pw/2,py,(pw+1)/2,2,ColorAlpha(pc,.3f));
        DrawRectangle(px,py+ph-1,pw,1,ColorAlpha(pc,.1f));
        // Border
        DrawRectangleLines(px,py,pw,ph,ColorAlpha(pc,.45f));
        // Pulsing glow if critical
        if(critical){
            float cp=.12f+.08f*sinf(t*6);
            DrawRectangleLinesEx({(float)px-2,(float)py-2,(float)(pw+4),(float)(ph+4)},1,ColorAlpha(TEXT_DANGER,cp));
        }
    };

    // ══════════════════════════════════════════════════════════
    // TOP LEFT: HULL — modern lives display
    // ══════════════════════════════════════════════════════════
    {
        int px=8, py=6, pw=96, ph=26;
        bool critical=g.lives<=1;
        PanelBorder(px,py,pw,ph,critical?TEXT_DANGER:tCol,critical);
        // Label
        DrawTextModern("HULL",px+5,py+3,6,ColorAlpha(TEXT_SECONDARY,0.7f),false);
        // Life diamonds — modern geometric style
        int hx=px+8, hy=py+13;
        for(int i=0;i<5;i++){
            bool alive=(i<g.lives);
            Color lc=alive?(critical?TEXT_DANGER:g.lives==2?TEXT_WARNING:ACCENT_CORAL):VP_DARKGRAY;
            float la=alive?(0.8f+0.2f*sinf(t*2.5f+i*0.5f)):0.12f;
            float cx2=(float)(hx+i*16+6), cy2=(float)(hy+5);
            if(alive){
                // Diamond shape for modern look
                Vector2 pts[4]={{cx2,cy2-4},{cx2+4,cy2},{cx2,cy2+4},{cx2-4,cy2}};
                DrawTriangleFan(pts,4,ColorAlpha(lc,la));
                DrawLineV(pts[0],pts[1],ColorAlpha(VP_WHITE,la*0.3f));
                DrawLineV(pts[1],pts[2],ColorAlpha(VP_BLACK,la*0.3f));
                DrawLineV(pts[2],pts[3],ColorAlpha(VP_BLACK,la*0.3f));
                DrawLineV(pts[3],pts[0],ColorAlpha(VP_WHITE,la*0.3f));
                if(critical) DrawCircleV({cx2,cy2},7.f,ColorAlpha(TEXT_DANGER,.1f+.08f*sinf(t*5)));
            } else {
                // Empty outline
                Vector2 pts[4]={{cx2,cy2-3},{cx2+3,cy2},{cx2,cy2+3},{cx2-3,cy2}};
                DrawLineV(pts[0],pts[1],ColorAlpha(lc,.08f));
                DrawLineV(pts[1],pts[2],ColorAlpha(lc,.08f));
                DrawLineV(pts[2],pts[3],ColorAlpha(lc,.08f));
                DrawLineV(pts[3],pts[0],ColorAlpha(lc,.08f));
            }
        }
        // Shield bar (if active)
        if(g.shieldHp>0){
            float sA=0.6f+0.4f*sinf(t*3.f);
            DrawModernBar(px+5,py+20,pw-10,3,g.shieldHp/100.f,ACCENT_CYAN,VP_DARKGRAY,t);
        }
    }

    // ══════════════════════════════════════════════════════════
    // TOP CENTER: WAVE + SCORE — modern clean tag
    // ══════════════════════════════════════════════════════════
    {
        Color wc=(g.wave<20)?Color{(unsigned char)(100+g.wave*5),(unsigned char)45,(unsigned char)(180-g.wave*3),255}:VP_WHITE;
        std::string wt="W"+IStr(g.wave+1);
        std::string sc=FmtScore(g.score);
        int ww=MeasureText(wt.c_str(),7), sw;
        if(false){
            sw=(int)MeasureTextEx(GetFontDefault(),sc.c_str(),13,13*.04f).x;
        } else sw=MeasureText(sc.c_str(),13);
        int pw=std::max(sw,ww)+20, px=(SCREEN_W-pw)/2, py=6, ph=28;
        PanelBorder(px,py,pw,ph,wc,false);
        // Wave label
        DrawTextModern(wt.c_str(),px+pw-ww-5,py+3,7,ColorAlpha(TEXT_SECONDARY,0.55f),false);
        // Score with modern styling
        if(false){
            float sp=13*.04f;
            Vector2 sm=MeasureTextEx(GetFontDefault(),sc.c_str(),13,sp);
            DrawTextEx(GetFontDefault(),sc.c_str(),{(float)(px+pw/2)-(int)sm.x*.5f+1,(float)(py+10)},13,sp,ColorAlpha(VP_BLACK,.25f));
            DrawTextEx(GetFontDefault(),sc.c_str(),{(float)(px+pw/2)-(int)sm.x*.5f,(float)(py+9)},13,sp,ColorAlpha(ACCENT_GOLD,.9f));
        } else {
            DrawText(sc.c_str(),(int)((px+pw/2)-(int)sw*.5f)+1,py+10,13,ColorAlpha(VP_BLACK,.25f));
            DrawText(sc.c_str(),(int)((px+pw/2)-(int)sw*.5f),py+9,13,ColorAlpha(ACCENT_GOLD,.9f));
        }
    }

    // ══════════════════════════════════════════════════════════
    // TOP RIGHT: PRAYER — modern slim chip
    // ══════════════════════════════════════════════════════════
    {
        int pw=90, ph=26, px=SCREEN_W-pw-8, py=6;
        DrawGlassPanel(px,py,pw,ph,pCol,t,1.f,true);
        // Label
        DrawTextModern("PRAYER",px+5,py+3,6,ColorAlpha(TEXT_SECONDARY,0.5f),false);
        // Prayer name
        const char* prName=PrayerName(g.prayer);
        int prW=MeasureText(prName,8);
        DrawTextGlow(prName,(int)((px+pw/2)-(int)prW*.5f),py+12,8,ColorAlpha(pCol,.85f),0.6f);
    }

    // ── ZONE NAME — subtle pill just below top panels ──────────────────────
    {
        const char* zoneName=WAVES[std::min(g.wave,(int)WAVES.size()-1)].theme;
        int znW=MeasureText(zoneName,7);
        // Pill background
        int zpx=SCREEN_W/2-znW/2-8, zpy=36, zpw=znW+16, zph=12;
        DrawRectangle(zpx,zpy,zpw,zph,ColorAlpha(SURFACE_1,0.75f));
        DrawRectangle(zpx,zpy,zpw,1,ColorAlpha(tCol,0.40f));
        DrawRectangleLines(zpx,zpy,zpw,zph,ColorAlpha(tCol,0.20f));
        // Zone text — subtle, secondary
        DrawText(zoneName,SCREEN_W/2-znW/2+1,zpy+2,7,ColorAlpha(SURFACE_0,0.20f));
        DrawText(zoneName,SCREEN_W/2-znW/2,zpy+1,7,ColorAlpha(TEXT_SECONDARY,0.55f));
    }

    // ── COMBO — compact right-edge pill, only when active ──
    if(g.combo>1){
        std::string ct="x"+IStr(g.combo);
        int cw=MeasureText(ct.c_str(),11);
        Color cc=g.combo>=12?VP_PURPLE:g.combo>=8?VP_HOTPINK:g.combo>=4?VP_YELLOW:VP_GRAY;
        int cpx=SCREEN_W-cw-18, cpy=SCREEN_H/2-22;
        float comboFlash=g.combo>=8?(.08f+.04f*sinf(t*5.f)):0.f;
        DrawRectangle(cpx-4,cpy-2,cw+14,18,ColorAlpha(VP_BLACK,.72f));
        DrawRectangle(cpx-3,cpy-1,cw+12,8,ColorAlpha(cc,.04f+comboFlash));
        DrawRectangle(cpx-4,cpy-2,2,18,ColorAlpha(cc,.60f));
        DrawRectangleLines(cpx-4,cpy-2,cw+14,18,ColorAlpha(cc,.28f));
        DrawText(">",cpx-1,cpy+2,8,ColorAlpha(cc,.40f));
        DrawText(ct.c_str(),cpx+5,cpy+1,11,ColorAlpha(cc,.95f));
        // thin decay bar with gradient
        float ctp=(g.comboTimer/g.comboMax);
        int decayW=(int)((cw+14)*ctp);
        DrawRectangle(cpx-4,cpy+16,cw+14,2,ColorAlpha(VP_DARKGRAY,.25f));
        if(decayW>0){
            DrawRectangle(cpx-4,cpy+16,decayW,2,ColorAlpha(cc,ctp*.65f));
            DrawRectangle(cpx-4,cpy+16,decayW,1,ColorAlpha(VP_WHITE,ctp*.12f));
        }
    }

    // ══════════════════════════════════════════════════════════
    // BOSS HP BAR — slim, full width, bottom
    // ══════════════════════════════════════════════════════════
    if(g.boss.active){
        float bHpPct=(g.boss.maxHp>0)?(float)g.boss.hp/g.boss.maxHp:0.f;
        Color bc=g.boss.col;
        float bPulse=.65f+.35f*sinf(t*3.5f);
        float dangerPulse=(bHpPct<0.25f)?(.55f+.45f*sinf(t*8.f)):1.f;
        Color bFill=bHpPct<0.25f?VP_RED:(bHpPct<0.55f?VP_ORANGE:bc);
        int bbX=4,bbY=SCREEN_H-28,bbW=SCREEN_W-8,bbH=6;
        // name chip
        const char* bname=g.boss.name.c_str();
        int bnW=MeasureText(bname,8);
        int nchX=SCREEN_W/2-bnW/2-6;
        DrawRectangle(nchX,bbY-14,bnW+12,12,ColorAlpha(VP_BLACK,.78f));
        DrawRectangle(nchX,bbY-14,bnW+12,1,ColorAlpha(bc,bPulse*.30f));
        DrawRectangleLines(nchX,bbY-14,bnW+12,12,ColorAlpha(bc,bPulse*.40f));
        DrawText(bname,nchX+6+1,bbY-12+1,8,ColorAlpha(VP_BLACK,.40f));
        DrawText(bname,nchX+6,bbY-12,8,ColorAlpha(bc,bPulse*.90f));
        // HP percentage text
        {
            std::string hpPctStr=IStr((int)(bHpPct*100))+"%";
            int hpPctW=MeasureText(hpPctStr.c_str(),6);
            DrawText(hpPctStr.c_str(),bbX+bbW-hpPctW-2,bbY-10,6,ColorAlpha(bFill,dangerPulse*.55f));
        }
        // bar
        DrawRectangle(bbX,bbY,bbW,bbH,ColorAlpha(VP_BLACK,.70f));
        int bfW=(int)(bbW*bHpPct);
        if(bfW>0){
            DrawRectangle(bbX,bbY,bfW,bbH,ColorAlpha(bFill,dangerPulse*.80f));
            DrawRectangle(bbX,bbY,bfW,2,ColorAlpha(VP_WHITE,dangerPulse*.18f));
            DrawRectangle(bbX,bbY+bbH-1,bfW,1,ColorAlpha(VP_BLACK,dangerPulse*.20f));
            if(bfW<bbW-2) DrawRectangle(bbX+bfW-1,bbY,2,bbH,ColorAlpha(VP_WHITE,dangerPulse*.65f));
        }
        DrawRectangleLines(bbX,bbY,bbW,bbH,ColorAlpha(bc,bPulse*.32f));
        // quarter tick marks
        for(int qi=1;qi<=3;qi++){
            int qx=bbX+(int)(bbW*qi/4.f);
            DrawRectangle(qx,bbY-1,1,bbH+2,ColorAlpha(VP_BLACK,.55f));
            DrawRectangle(qx+1,bbY,1,bbH,ColorAlpha(bc,bPulse*.12f));
        }
    }

    // ══════════════════════════════════════════════════════════
    // BOTTOM: wave progress + kills + chi
    // ══════════════════════════════════════════════════════════
    {
        int barY=SCREEN_H-5, barH=4;
        float prog=(g.waveTarget>0)?(float)g.waveKills/g.waveTarget:0.f;
        Color progCol=prog>0.85f?VP_MINT:prog>0.5f?VP_CYAN:prog>0.25f?VP_DEEPCYAN:VP_DEEPPURP;

        DrawRectangle(0,barY-16,SCREEN_W,barH+17,ColorAlpha(VP_BLACK,.75f));
        DrawRectangle(0,barY-17,SCREEN_W,1,ColorAlpha(VP_PURPLE,.30f));
        DrawRectangle(0,barY-16,SCREEN_W,1,ColorAlpha(VP_PURPLE,.10f));
        DrawRectangle(0,barY,SCREEN_W,barH,ColorAlpha(VP_DARKGRAY,.35f));

        int fillW=(int)(SCREEN_W*prog);
        if(fillW>0){
            DrawRectangle(0,barY,fillW,barH,ColorAlpha(progCol,.78f));
            DrawRectangle(0,barY,fillW,1,ColorAlpha(VP_WHITE,.18f));
            DrawRectangle(0,barY+barH-1,fillW,1,ColorAlpha(VP_BLACK,.15f));
            if(prog<0.999f){
                int ledgeW=2+(int)(g.progressJuice*12.f);
                DrawRectangle(fillW-1,barY,ledgeW,barH,ColorAlpha(VP_WHITE,.60f+g.lastKillFlash*.25f));
                DrawRectangle(fillW,barY-1,1,barH+2,ColorAlpha(progCol,.45f));
            }
        }
        // segment ticks at 25% intervals
        for(int si=1;si<=3;si++){
            int sx=SCREEN_W*si/4;
            DrawRectangle(sx,barY,1,barH,ColorAlpha(VP_BLACK,.30f));
        }

        // kills left
        std::string kstr=IStr(g.waveKills)+" / "+IStr(g.waveTarget);
        DrawText(kstr.c_str(),5+1,barY-13+1,8,ColorAlpha(VP_BLACK,.25f));
        DrawText(kstr.c_str(),5,barY-13,8,ColorAlpha(progCol,.80f));

        // zone name — centered, top of bar area
        {
            const char* zoneName=WAVES[std::min(g.wave,(int)WAVES.size()-1)].theme;
            int znW=MeasureText(zoneName,7);
            DrawText(zoneName,SCREEN_W/2-znW/2,barY-13,7,ColorAlpha(TEXT_SECONDARY,.45f));
        }

        // FLUX mini bar right — styled with ship theme color
        {
            float chiPct=g.chiGauge/100.f;
            bool t1=g.chiGauge>=40.f,t2=g.chiGauge>=70.f,t3=g.chiGauge>=88.f;
            bool venting=g.heatVentTimer>0;
            // color: ship-tinted when cool, escalates toward danger orange/red when hot
            Color chc = venting ? ACCENT_CORAL :
                        t3 ? Color{255,60,60,255} :
                        t2 ? Color{255,150,40,255} :
                        t1 ? TEXT_WARNING :
                        tCol; // ship color when cool
            float cp=venting?(0.55f+0.45f*sinf(t*22)):(0.75f+0.20f*sinf(t*3));
            int hbW=32, hbX=SCREEN_W-hbW-5, hbY=barY-13;
            // label
            int flW=MeasureText("FLUX",6);
            DrawText("FLUX",hbX-flW-3,hbY,6,ColorAlpha(chc,cp*.75f));
            // track
            DrawRectangle(hbX,hbY,hbW,6,ColorAlpha(SURFACE_3,0.60f));
            DrawRectangle(hbX,hbY,(int)(hbW*chiPct),6,ColorAlpha(chc,cp*.80f));
            if(chiPct>0.001f)
                DrawRectangle(hbX,hbY,(int)(hbW*chiPct),1,ColorAlpha(VP_WHITE,0.22f));
            DrawRectangleLines(hbX,hbY,hbW,6,ColorAlpha(chc,0.30f));
        }
    }

    // ── Upgrade chips — tiny, bottom center ──
    if(!g.upgrades.empty()){
        int cellW=16,cellH=9,gap2=2;
        int uw=(int)g.upgrades.size()*(cellW+gap2)-gap2;
        int sx2=(SCREEN_W-uw)/2, barY2=SCREEN_H-32, xx=sx2;
        for(int ui=0;ui<(int)g.upgrades.size();ui++){
            auto& u=g.upgrades[ui];
            int sw4=MeasureText(u.symbol.c_str(),6);
            DrawRectangle(xx,barY2,cellW,cellH,ColorAlpha(VP_BLACK,.82f));
            DrawRectangleLines(xx,barY2,cellW,cellH,ColorAlpha(u.col,.45f));
            DrawText(u.symbol.c_str(),xx+(cellW-sw4)/2,barY2+1,6,ColorAlpha(u.col,.80f));
            xx+=cellW+gap2;
        }
    }

    // ── Crit flash ──
    if(g.critFlash>0){DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_YELLOW,.04f));g.critFlash--;}

    // ── Near-miss flash — single border ring ──
    if(g.nearMissFlash>0){
        float nma=g.nearMissFlash*.55f;
        DrawRectangleLinesEx({0,0,(float)SCREEN_W,(float)SCREEN_H},3,ColorAlpha(VP_GOLD,nma));
    }

    // ── Hull critical vignette (1 life) ──
    if(g.lives==1){
        float dp=.08f+.06f*sinf(t*5.f);
        DrawRectangleLinesEx({0,0,(float)SCREEN_W,(float)SCREEN_H},5,ColorAlpha(VP_RED,dp));
    }

    // ── Trance active — screen edge glow + timer ──
    if(g.tranceActive){
        float ta=sinf(t*7)*.5f+.5f;
        DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_HOTPINK,ta*.025f));
        DrawRectangleLinesEx({0,0,(float)SCREEN_W,(float)SCREEN_H},2,ColorAlpha(VP_HOTPINK,.18f+ta*.14f));
        std::string tts=IStr((int)g.tranceTimer+1)+"s";
        int ttw=MeasureText(tts.c_str(),8);
        DrawText(tts.c_str(),SCREEN_W/2-ttw/2,SCREEN_H-48,8,ColorAlpha(VP_HOTPINK,.65f+ta*.22f));
    }

    // ── Prayer switch flash — single ring, no giant text ──
    if(g.prayerFxTimer>0){
        float pa=g.prayerFxTimer;
        DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(pCol,pa*.07f));
        DrawNeonCircle({(float)SCREEN_W/2,(float)SCREEN_H/2},60+pa*140,pCol,pa*.40f);
        const char* prLabel=(g.prayer==PRAYER_FIRE?"SOLAR FLARE":g.prayer==PRAYER_ICE?"VOID LANCE":"SOUL SWARM");
        int plW=MeasureText(prLabel,11);
        DrawText(prLabel,SCREEN_W/2-plW/2,SCREEN_H/2+28,11,ColorAlpha(pCol,std::min(1.f,pa*3.f)));
    }
}
// KillEntry and killFeed now live in UIState (see void_blaster.h)
void PushKillFeed(UIState& ui, const std::string& s, Color c){
    KillEntry k; k.text=s; k.col=c; k.life=3.5f;
    ui.killFeed.insert(ui.killFeed.begin(),k);
    if((int)ui.killFeed.size()>6) ui.killFeed.pop_back();
}

void DrawBorderPanels(Game& g, UIState& ui, float t, float dt){
    Rectangle gr=GameRect();
    float ww=(float)GetScreenWidth(), wh=(float)GetScreenHeight();
    float lw=gr.x, rw=ww-gr.x-gr.width;
    float th=gr.y, bh=wh-gr.y-gr.height;

    // ── Background fills ──
    Color bgPanel={8,4,22,255};
    if(lw>0) DrawRectangle(0,0,(int)lw,(int)wh,bgPanel);
    if(rw>0) DrawRectangle((int)(gr.x+gr.width),0,(int)rw,(int)wh,bgPanel);
    if(th>0) DrawRectangle(0,0,(int)ww,(int)th,bgPanel);
    if(bh>0) DrawRectangle(0,(int)(gr.y+gr.height),(int)ww,(int)bh,bgPanel);

    // Game screen border — neon glow with corner brackets
    float eg=.5f+.15f*sinf(t*1.6f);
    DrawRectangleLinesEx({gr.x-2,gr.y-2,gr.width+4,gr.height+4},2,ColorAlpha(VP_HOTPINK,eg*.85f));
    DrawRectangleLinesEx({gr.x-4,gr.y-4,gr.width+8,gr.height+8},1,ColorAlpha(VP_PURPLE,.28f));
    for(int i=1;i<=3;i++)
        DrawRectangleLinesEx({gr.x-2.f-i*2,gr.y-2.f-i*2,gr.width+4.f+i*4,gr.height+4.f+i*4},
                             1,ColorAlpha(VP_HOTPINK,.04f*(4-i)));
    // Corner bracket flourishes
    {
        float bp=.7f+.3f*sinf(t*2.2f);
        float arm=14.f;
        float x0=gr.x-3, y0=gr.y-3;
        float x1=gr.x+gr.width+2, y1=gr.y+gr.height+2;
        auto CB=[&](float cx,float cy,float dx,float dy){
            DrawLineEx({cx,cy},{cx+dx*arm,cy},2.f,ColorAlpha(VP_CYAN,bp*.90f));
            DrawLineEx({cx,cy},{cx,cy+dy*arm},2.f,ColorAlpha(VP_CYAN,bp*.90f));
            DrawCircleV({cx,cy},2.5f,ColorAlpha(VP_GOLD,bp));
            DrawCircleV({cx,cy},1.2f,ColorAlpha(VP_WHITE,bp*.8f));
        };
        CB(x0,y0, 1, 1); CB(x1,y0,-1, 1);
        CB(x0,y1, 1,-1); CB(x1,y1,-1,-1);
    }

    // ── Planet color theme per wave ──
    Color waveColors[]={VP_RED,VP_RED,VP_CYAN,VP_CYAN,VP_MINT,VP_MINT,VP_PURPLE,VP_PURPLE,VP_GOLD,VP_GOLD,VP_LAVENDER,VP_LAVENDER,VP_HOTPINK,VP_HOTPINK,VP_ORANGE,VP_ORANGE,VP_CYAN,VP_CYAN,VP_GOLD,VP_GOLD};
    Color waveColors2[]={VP_HOTPINK,VP_HOTPINK,VP_DEEPCYAN,VP_DEEPCYAN,VP_TEAL,VP_TEAL,VP_LAVENDER,VP_LAVENDER,VP_ORANGE,VP_ORANGE,VP_CYAN,VP_CYAN,VP_PINK,VP_PINK,VP_YELLOW,VP_YELLOW,VP_MINT,VP_MINT,VP_PURPLE,VP_PURPLE};
    Color waveCol  = (g.wave < 20) ? waveColors[g.wave]  : VP_WHITE;
    Color waveCol2 = (g.wave < 20) ? waveColors2[g.wave] : VP_CYAN;

    // ══════════════════════════════════════════════════════
    // LEFT PANEL: CHI METER — vertical neon bar showing heat/chi energy
    // ══════════════════════════════════════════════════════
    if(lw >= 40){
        float camPct  = (g.state == PLAYING) ? sinf(g.cameraPos.y * 0.00035f) : 0.f;
        float camPctX = (g.state == PLAYING) ? sinf(g.cameraPos.x * 0.00035f) : 0.f;

        // Background nebula wash
        for(int i=0;i<5;i++){
            float nx = lw * 0.5f + sinf(t*0.12f + i*1.4f + camPctX*3.f) * lw * 0.35f;
            float ny = i * (wh/5.f) + cosf(t*0.09f + i*2.1f + camPct*4.f) * wh * 0.08f;
            ny = fmodf(ny + wh, wh);
            Color nc = (i%2==0) ? waveCol : waveCol2;
            DrawCircleV({nx, ny}, lw * 0.6f, ColorAlpha(nc, 0.04f + 0.02f*sinf(t*0.4f+i)));
        }
        // Background stars
        for(int i=0;i<40;i++){
            float seed = i * 137.508f;
            float sx = fmodf(seed * 0.618f * lw, lw);
            float sy = fmodf(seed * 0.382f * wh + camPct * wh * 2.f, wh);
            float br = 0.3f + 0.7f * sinf(seed + t * (0.5f + fmodf(seed, 2.f)));
            float r2 = 0.5f + fmodf(seed * 0.273f, 1.5f);
            Color sc2 = (i%5==0)?VP_CYAN:(i%5==1)?VP_PINK:(i%5==2)?VP_LAVENDER:(i%5==3)?VP_YELLOW:VP_WHITE;
            DrawCircleV({sx, sy}, r2, ColorAlpha(sc2, br * 0.55f));
        }

        // ── CHI METER — vertical bar, center of left panel ──
        if(g.state == PLAYING || g.state == PAUSED){
            float chiPct  = g.chiGauge / 100.f;
            // heat tier thresholds
            bool tier1 = g.chiGauge >= 40.f;  // warm — slight fire rate boost
            bool tier2 = g.chiGauge >= 70.f;  // hot  — damage bonus, faster shots
            bool tier3 = g.chiGauge >= 88.f;  // critical — overheating imminent
            bool burst  = g.heatVentTimer > 0.f; // venting / locked out

            float cx  = lw * 0.5f;
            float barH = wh * 0.50f;
            float barW = std::max(10.f, lw * 0.32f);
            float barX = cx - barW * 0.5f;
            float barY = wh * 0.5f - barH * 0.5f;

            // FLUX label — ship-tinted when cool, danger-red when critical
            Color chiCol = burst ? ACCENT_CORAL : tier3 ? Color{255,60,60,255} : tier2 ? Color{255,150,40,255} : tier1 ? TEXT_WARNING : waveCol;
            float chiPulse = burst ? (0.5f+0.5f*sinf(t*22.f)) : tier3 ? (0.65f+0.35f*sinf(t*9.f)) : 0.85f;
            int clW = MeasureText("FLUX", 10);
            DrawText("FLUX", (int)(cx - clW*0.5f), (int)(barY - 18.f), 10, ColorAlpha(chiCol, chiPulse));

            // Status label
            const char* bonusLbl = burst ? "VENTING" : tier3 ? "CRITICAL" : tier2 ? "+POWER" : tier1 ? "CHARGING" : "";
            if(bonusLbl[0]){
                int blW = MeasureText(bonusLbl, 8);
                DrawText(bonusLbl, (int)(cx-blW*0.5f), (int)(barY - 28.f), 8, ColorAlpha(chiCol, chiPulse*0.90f));
            }

            // Outer track
            DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, ColorAlpha(VP_BLACK, 0.70f));
            DrawRectangleLines((int)barX, (int)barY, (int)barW, (int)barH, ColorAlpha(chiCol, 0.50f));
            DrawRectangle((int)(barX+1), (int)(barY+1), (int)(barW-2), 1, ColorAlpha(VP_WHITE, 0.10f));

            // Tier threshold markers at 40%, 70%, 85%
            float tiers[3]={0.40f,0.70f,0.85f};
            Color tierCols[3]={VP_YELLOW,VP_ORANGE,VP_HOTPINK};
            for(int ti=0;ti<3;ti++){
                float ty = barY + barH * (1.f - tiers[ti]);
                DrawRectangle((int)(barX-4), (int)ty, (int)(barW+8), 1, ColorAlpha(tierCols[ti], 0.55f));
                DrawRectangle((int)(barX-4), (int)ty, 4, 3, ColorAlpha(tierCols[ti], 0.80f));
                DrawRectangle((int)(barX+barW), (int)ty, 4, 3, ColorAlpha(tierCols[ti], 0.80f));
            }

            // Fill — color shifts through tiers
            float fillH = barH * chiPct;
            float fillY = barY + barH - fillH;
            if(fillH > 1.f){
                Color chiLo = tier3 ? Color{255,50,0,255}   : tier2 ? Color{255,140,0,255} : tier1 ? VP_YELLOW : waveCol;
                Color chiHi = tier3 ? Color{255,200,100,255} : tier2 ? Color{255,80,0,255}  : tier1 ? Color{255,180,0,255} : waveCol2;
                for(int py=(int)fillY; py<(int)(fillY+fillH); py++){
                    float frac = (py - fillY) / fillH;
                    Color lc   = LerpC(chiHi, chiLo, frac);
                    float flk  = burst ? (0.70f+0.30f*sinf(t*20.f+py*0.12f))
                               : 0.55f+0.15f*sinf(t*5.f+py*0.07f);
                    DrawRectangle((int)(barX+1), py, (int)(barW-2), 1, ColorAlpha(lc, flk));
                }
                // Leading edge bright line
                DrawRectangle((int)(barX+1),(int)fillY,(int)(barW-2),2,ColorAlpha(chiHi,0.92f));
                // Glow
                float gz = tier3 ? 0.20f+0.10f*sinf(t*6.f) : 0.08f;
                DrawRectangle((int)(barX-3),(int)fillY,(int)(barW+6),(int)fillH,ColorAlpha(chiLo,gz*0.35f));
            }

            // 10 segment ticks
            for(int seg=1;seg<10;seg++){
                float ty = barY + barH*(1.f-seg*0.1f);
                DrawRectangle((int)barX,(int)ty,(int)barW,1,ColorAlpha(VP_WHITE,chiPct>=seg*0.1f?0.30f:0.10f));
            }

            // Burst flash ring
            if(burst){
                float fa=0.30f+0.30f*sinf(t*20.f);
                DrawRectangleLines((int)(barX-3),(int)barY,(int)(barW+6),(int)barH,ColorAlpha(VP_CYAN,fa*0.90f));
            }

            // Percentage readout
            char chiPctBuf[8]; snprintf(chiPctBuf,sizeof(chiPctBuf),"%d%%",(int)(chiPct*100.f));
            int cpW = MeasureText(chiPctBuf,9);
            DrawText(chiPctBuf,(int)(cx-cpW*0.5f),(int)(barY+barH+5.f),9,ColorAlpha(chiCol,0.70f));

            // Ship tier badge
            {
                std::string tierStr = "TIER "+IStr(g.shipUpgradeTier);
                int trW = MeasureText(tierStr.c_str(), 9);
                Color tierCol = (g.shipUpgradeTier>=7)?VP_GOLD:(g.shipUpgradeTier>=4)?VP_HOTPINK:VP_CYAN;
                DrawText(tierStr.c_str(),(int)(cx-trW*0.5f),(int)(barY+barH+18.f),9,ColorAlpha(tierCol,0.80f));
                static const int tierWaveReq[10] = { 5,8,12,15,18,20,22,24,26,27 };
                static const int tierRunsReq[10]  = { 5,10,16,24,35,50,70,95,130,180 };
                std::string nxtStr;
                if(g.shipUpgradeTier>=10){ nxtStr="MAX TIER"; }
                else {
                    int ni=g.shipUpgradeTier;
                    bool wOk=(g.bestWave>=tierWaveReq[ni]), rOk=(g.runCount>=tierRunsReq[ni]);
                    if(!wOk) nxtStr="W"+IStr(tierWaveReq[ni])+"+ needed";
                    else if(!rOk) nxtStr=IStr(tierRunsReq[ni]-g.runCount)+" runs left";
                    else nxtStr="LEVELING UP!";
                }
                int nxW=MeasureText(nxtStr.c_str(),7);
                DrawText(nxtStr.c_str(),(int)(cx-nxW*0.5f),(int)(barY+barH+30.f),7,ColorAlpha(VP_GRAY,0.50f));
                std::string bwStr="BEST W"+IStr(g.bestWave);
                int bwW=MeasureText(bwStr.c_str(),7);
                DrawText(bwStr.c_str(),(int)(cx-bwW*0.5f),(int)(barY+barH+40.f),7,ColorAlpha(VP_MINT,0.40f));
            }
        }

        // Score display
        if(g.state == PLAYING){
            int slw2=MeasureText("SCORE",9);
            DrawText("SCORE",(int)(lw/2-slw2/2),(int)(wh-52),9,ColorAlpha(VP_MIDGRAY,0.60f));
            std::string sc2 = FmtScore(g.score);
            float scoreGlow = 0.88f + 0.12f * sinf(t * 2.2f);
            int sw3 = MeasureText(sc2.c_str(), 12);
            DrawText(sc2.c_str(),(int)(lw/2-sw3/2+1),(int)(wh-40),12,ColorAlpha(VP_BLACK,0.55f));
            DrawText(sc2.c_str(),(int)(lw/2-sw3/2),(int)(wh-41),12,ColorAlpha(VP_PINK, scoreGlow));
            DrawLineEx({lw*0.15f,(float)(wh-56)},{lw*0.85f,(float)(wh-56)},0.8f,ColorAlpha(VP_PURPLE,0.28f));
        }

        // Vertical neon rail
        float rAlpha = .15f + .06f*sinf(t*2.f);
        DrawLineEx({gr.x-1.f,gr.y},{gr.x-1.f,gr.y+gr.height},1.5f,ColorAlpha(VP_HOTPINK,rAlpha));
    }

    // ══════════════════════════════════════════════════════
    // RIGHT PANEL: THREAT GAUGE — enemy density + kill streak meter
    // ══════════════════════════════════════════════════════
    if(rw >= 40){
        float rpx = gr.x + gr.width;
        float camPct = (g.state == PLAYING) ? sinf(g.cameraPos.y * 0.00035f) : 0.f;

        // Background nebula wash
        for(int i=0;i<5;i++){
            float seed = i * 113.7f + 500.f;
            float nx = rpx + rw * (0.2f + 0.6f * sinf(seed + t*0.11f));
            float ny = i * (wh/5.f) + sinf(t*0.08f + i*1.8f + camPct*3.f) * wh * 0.08f;
            ny = fmodf(ny + wh, wh);
            Color nc = (i%2==0) ? waveCol2 : waveCol;
            DrawCircleV({nx, ny}, rw * 0.55f, ColorAlpha(nc, 0.04f + 0.02f*sinf(t*0.35f+i*1.3f)));
        }
        // Stars
        for(int i=0;i<40;i++){
            float seed = i * 197.1f + 1000.f;
            float sx = rpx + fmodf(seed * 0.5f * rw, rw);
            float sy = fmodf(seed * 0.29f * wh + (1.f-camPct) * wh * 1.8f, wh);
            float br = 0.3f + 0.7f * sinf(seed * 0.8f + t * (0.4f + fmodf(seed * 0.1f, 1.8f)));
            float r2 = 0.5f + fmodf(seed * 0.31f, 1.4f);
            Color sc2 = (i%4==0)?VP_CYAN:(i%4==1)?VP_PURPLE:(i%4==2)?VP_MINT:VP_LAVENDER;
            DrawCircleV({sx, sy}, r2, ColorAlpha(sc2, br * 0.55f));
        }

        // ── THREAT GAUGE — vertical bar, center of right panel ──
        if(g.state == PLAYING || g.state == PAUSED){
            int enemyCount = 0;
            if(g.state == PLAYING) for(auto& e : g.enemies) if(e.active) enemyCount++;
            bool bossActive = g.boss.active;

            // Threat = how many enemies alive relative to max expected on this wave
            float maxExpected = std::max(15.f, (float)g.waveTarget * 0.40f);
            float threatPct   = std::min(1.f, enemyCount / maxExpected);
            if(bossActive) threatPct = std::min(1.f, threatPct + 0.35f + 0.15f*sinf(t*4.f));

            float cx     = rpx + rw * 0.5f;
            float barH   = wh * 0.52f;
            float barW   = std::max(8.f, rw * 0.28f);
            float barX   = cx - barW * 0.5f;
            float barY   = wh * 0.5f - barH * 0.5f;

            bool threatHigh = threatPct > 0.70f;
            bool threatCrit = threatPct > 0.90f || bossActive;
            Color threatLo  = threatPct > 0.70f ? VP_RED    : threatPct > 0.40f ? VP_ORANGE : VP_MINT;
            Color threatHi  = threatPct > 0.70f ? VP_HOTPINK: threatPct > 0.40f ? VP_YELLOW : VP_TEAL;
            Color threatCol = threatCrit ? VP_RED : threatHigh ? VP_ORANGE : VP_MINT;

            // THREAT label
            const char* threatLabel = bossActive ? "!! BOSS !!" : threatCrit ? "CRITICAL" : threatHigh ? "DANGER" : "THREAT";
            int tlW = MeasureText(threatLabel, 10);
            float tlPulse = threatCrit ? (0.7f + 0.3f*sinf(t*8.f)) : 0.75f;
            DrawText(threatLabel, (int)(cx - tlW*0.5f), (int)(barY - 18.f), 10, ColorAlpha(threatCol, tlPulse));

            // Outer track
            DrawRectangle((int)barX, (int)barY, (int)barW, (int)barH, ColorAlpha(VP_BLACK, 0.65f));
            DrawRectangleLines((int)barX, (int)barY, (int)barW, (int)barH, ColorAlpha(threatCol, 0.40f));
            DrawRectangle((int)(barX+1), (int)(barY+1), (int)(barW-2), 1, ColorAlpha(VP_WHITE, 0.08f));

            // Fill (grows upward from bottom)
            float fillH = barH * threatPct;
            float fillY = barY + barH - fillH;
            if(fillH > 1.f){
                for(int py=(int)fillY; py<(int)(fillY+fillH); py++){
                    float frac = (py - fillY) / fillH;
                    Color lc   = LerpC(threatHi, threatLo, frac);
                    float flk  = 0.50f + 0.18f*sinf(t*4.f + py*0.07f + threatPct*3.f);
                    if(threatCrit) flk = 0.55f + 0.35f*sinf(t*9.f + py*0.09f);
                    DrawRectangle((int)(barX+1), py, (int)(barW-2), 1, ColorAlpha(lc, flk));
                }
                // Leading edge
                DrawRectangle((int)(barX+1), (int)fillY, (int)(barW-2), 2, ColorAlpha(threatHi, 0.88f + 0.12f*sinf(t*6.f)));
                // Glow haze
                float gz = threatCrit ? 0.20f + 0.10f*sinf(t*5.f) : 0.08f;
                DrawRectangle((int)(barX-3), (int)fillY, (int)(barW+6), (int)fillH, ColorAlpha(threatLo, gz*0.35f));
                DrawRectangle((int)(barX-6), (int)fillY, (int)(barW+12), (int)fillH, ColorAlpha(threatLo, gz*0.12f));
            }

            // Segment ticks
            for(int seg=1; seg<10; seg++){
                float ty = barY + barH * (1.f - seg*0.1f);
                float ta = (threatPct >= seg*0.1f) ? 0.30f : 0.10f;
                DrawRectangle((int)barX, (int)ty, (int)barW, 1, ColorAlpha(VP_WHITE, ta));
            }

            // Boss warning rings around bar
            if(bossActive){
                float bp = 0.45f + 0.45f*sinf(t*5.f);
                DrawRectangleLines((int)(barX-3), (int)barY, (int)(barW+6), (int)barH, ColorAlpha(VP_RED, bp*0.75f));
                DrawRectangleLines((int)(barX-5), (int)barY, (int)(barW+10), (int)barH, ColorAlpha(VP_HOTPINK, bp*0.35f));
            }
            if(threatCrit && !bossActive){
                float fa = 0.35f + 0.35f*sinf(t*7.f);
                DrawRectangleLines((int)(barX-1), (int)barY, (int)(barW+2), (int)barH, ColorAlpha(VP_RED, fa*0.60f));
            }

            // Enemy count readout below bar
            char ecBuf[12]; snprintf(ecBuf, sizeof(ecBuf), "%d ON FIELD", enemyCount);
            int ecW = MeasureText(ecBuf, 8);
            DrawText(ecBuf, (int)(cx - ecW*0.5f), (int)(barY + barH + 5.f), 8,
                ColorAlpha(threatCrit ? VP_RED : VP_GRAY, 0.65f));

            // Kill streak badge above label
            {
                int streak = g.combo;
                Color skCol = streak>=12?VP_PURPLE:streak>=8?VP_HOTPINK:streak>=4?VP_YELLOW:VP_GRAY;
                std::string skStr = streak>1 ? ("x"+IStr(streak)+" STREAK") : "STREAK";
                int skW = MeasureText(skStr.c_str(), 9);
                float skA = streak>1 ? (0.75f + 0.20f*sinf(t*4.f)) : 0.30f;
                DrawText(skStr.c_str(), (int)(cx - skW*0.5f), (int)(barY - 30.f), 9, ColorAlpha(skCol, skA));
                float waveProg = g.waveTarget>0 ? (float)g.waveKills/g.waveTarget : 0.f;
                std::string wpStr = IStr(g.waveKills)+"/"+IStr(g.waveTarget)+" KILLS";
                int wpW = MeasureText(wpStr.c_str(), 8);
                DrawText(wpStr.c_str(), (int)(cx - wpW*0.5f), (int)(barY - 42.f), 8,
                    ColorAlpha(VP_GRAY, 0.50f));
            }
        }

        // Hull pips (lives)
        if(g.state == PLAYING){
            float pipY = wh - 22.f;
            for(int i=0;i<5;i++){
                float px = rpx + rw/2.f - 5*7.f/2.f + i*7.f;
                bool alive = (i < g.lives);
                Color pc = alive ? (g.lives==1 ? ColorAlpha(VP_RED, 0.7f+0.3f*sinf(t*6)) : VP_CORAL) : Color{25,12,40,255};
                DrawRectangle((int)(px), (int)pipY, 5, 8, ColorAlpha(pc, alive ? 0.55f : 0.1f));
                DrawRectangleLines((int)(px), (int)pipY, 5, 8, ColorAlpha(pc, alive ? 0.8f : 0.2f));
            }
        }

        // Right neon edge rail
        float rAlpha2 = .15f + .06f*sinf(t*2.3f);
        DrawLineEx({rpx+1.f,gr.y},{rpx+1.f,gr.y+gr.height},1.5f,ColorAlpha(VP_CYAN,rAlpha2));
    }

    // ── TOP BAR: animated chevrons + wave-colored scan line ──
    if(th >= 14){
        float midx = ww*.5f, midy = th*.5f;
        DrawRectangle(0,0,(int)ww,(int)th,ColorAlpha({6,2,18,255},.88f));
        DrawLine(0,(int)(th-1),(int)ww,(int)(th-1),ColorAlpha(VP_HOTPINK,.40f));
        DrawLine(0,(int)(th-3),(int)ww,(int)(th-3),ColorAlpha(VP_PURPLE,.22f));

        // Wave color scan line
        float scanX = fmodf(t * 80.f, ww * 2.f);
        if(scanX < ww) DrawLineEx({scanX, 0}, {scanX, th}, 1.5f, ColorAlpha(waveCol, 0.25f));

        // VOID BLASTER title
        float hcyc = fmodf(t*.4f,3.f);
        Color hc2 = hcyc<1?LerpC(VP_CYAN,VP_PURPLE,hcyc):hcyc<2?LerpC(VP_PURPLE,VP_HOTPINK,hcyc-1):LerpC(VP_HOTPINK,VP_CYAN,hcyc-2);
        int tsz=(int)(th*.62f); if(tsz<8)tsz=8;
        const char* ttl="VOID  BLASTER";
        if(false){
            Vector2 m = MeasureTextEx(ui.fontBig, ttl, (float)tsz, tsz*0.04f);
            TF(ttl, midx, midy - m.y*.5f, tsz, ColorAlpha(hc2,.12f), ui.fontBig);
            TF(ttl, midx+1, midy - m.y*.5f+1, tsz, ColorAlpha({0,0,0,255},.5f), ui.fontBig);
            TF(ttl, midx, midy - m.y*.5f, tsz, hc2, ui.fontBig);
        } else {
            int tw=MeasureText(ttl,tsz);
            DrawText(ttl,(int)(midx-tw*.5f)+1,(int)(midy-tsz*.5f)+1,tsz,ColorAlpha({0,0,0,255},.5f));
            DrawText(ttl,(int)(midx-tw*.5f),(int)(midy-tsz*.5f),tsz,hc2);
        }

        // Chevrons with wave-color tint
        for(int side=0;side<2;side++){
            float startX = side==0 ? 10 : ww-10;
            float endX   = side==0 ? gr.x-10 : gr.x+gr.width+10;
            float dir    = side==0 ? 1.f : -1.f;
            float span   = fabsf(endX - startX);
            int nc = (int)(span / 16) + 1;
            for(int i=0;i<nc;i++){
                float ox = startX + dir*i*16;
                float ph = fmodf(i*.3f - t*.6f*dir, 1.f);
                Color dc = ph<.5f?LerpC(VP_DEEPPURP,waveCol,ph*2):LerpC(waveCol,VP_PURPLE,(ph-.5f)*2);
                DrawLineEx({ox, midy-4},{ox+dir*5,midy},1.2f,ColorAlpha(dc,.38f));
                DrawLineEx({ox+dir*5,midy},{ox,midy+4},1.2f,ColorAlpha(dc,.38f));
            }
        }
    }

    // ── BOTTOM BAR: Waveform visualizer that reacts to combat ──
    if(bh >= 12){
        float barTop = gr.y + gr.height;
        float midy2  = barTop + bh*.5f;
        DrawRectangle(0,(int)barTop,(int)ww,(int)bh,ColorAlpha({6,2,18,255},.88f));
        DrawLine(0,(int)barTop,(int)ww,(int)barTop,ColorAlpha(VP_PURPLE,.30f));

        // Audio waveform bars that react to combat intensity
        int enemyCount2 = 0;
        if(g.state == PLAYING) for(auto& e : g.enemies) if(e.active) enemyCount2++;
        float intensity = (g.state == PLAYING) ? std::min(1.f, enemyCount2 / 15.f + (g.boss.active ? 0.5f : 0.f)) : 0.3f;
        float heatVis = (g.state == PLAYING) ? (g.chiGauge / 100.f) : 0.f;

        int numBars = (int)(ww / 4);
        for(int i=0;i<numBars;i++){
            float fx = i * 4.f;
            // Multi-frequency waves
            float h1 = sinf(i * 0.18f + t * 3.5f) * 0.5f + 0.5f;
            float h2 = sinf(i * 0.09f + t * 2.1f + 1.f) * 0.5f + 0.5f;
            float h3 = sinf(i * 0.32f - t * 4.8f + 2.f) * 0.3f + 0.3f;
            float barH2 = (h1 * 0.5f + h2 * 0.3f + h3 * 0.2f) * (0.3f + intensity * 0.7f) * bh * 0.85f;

            // Color cycles through wave colors
            float cpct = fmodf(i / (float)numBars + t * 0.12f, 1.f);
            Color bc2 = cpct < 0.5f ? LerpC(waveCol, waveCol2, cpct * 2.f) : LerpC(waveCol2, waveCol, (cpct - 0.5f) * 2.f);

            // Heat-based brightness
            float alpha2 = 0.25f + 0.45f * (h1 + heatVis * 0.3f);
            DrawLineEx({fx, midy2 - barH2*0.5f}, {fx, midy2 + barH2*0.5f}, 2.5f, ColorAlpha(bc2, alpha2));

            // Beat flash on every 8th bar when high intensity
            if(i % 8 == 0 && intensity > 0.5f){
                float beatT = fmodf(t * 4.f + i * 0.2f, 1.f);
                if(beatT < 0.15f) DrawLineEx({fx, barTop}, {fx, barTop + bh}, 1.5f, ColorAlpha(VP_WHITE, (1.f - beatT / 0.15f) * 0.3f));
            }
        }

        // Boss heartbeat pulse overlay
        if(g.state == PLAYING && g.boss.active){
            float bph = fabsf(sinf(t * 3.f));
            DrawRectangle(0, (int)barTop, (int)ww, (int)bh, ColorAlpha(VP_RED, bph * 0.08f));
        }
    }

    // ── Kill feed lifetime update ──
    ui.killFeedTimer+=dt;
    for(auto& k:ui.killFeed) k.life-=dt;
    ui.killFeed.erase(std::remove_if(ui.killFeed.begin(),ui.killFeed.end(),[](KillEntry& k){return k.life<=0;}),ui.killFeed.end());
}




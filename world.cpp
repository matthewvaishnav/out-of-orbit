// world.cpp — World rendering: planets, stars, nebulas, aurora, grid, ambient VFX systems
#include "void_blaster.h"

void DrawEasterEggs(Game& g, float t, float revealAlpha){
    for(auto& e : easterEggs){
        const float parallax = 0.15f;
        float px = e.pos.x - g.cameraPos.x * parallax;
        float py = e.pos.y - g.cameraPos.y * parallax;
        px = fmodf(px + (float)SCREEN_W * 40.f, (float)SCREEN_W);
        py = fmodf(py + (float)SCREEN_H * 40.f, (float)SCREEN_H);
        Vector2 sp = { px, py };
        if(!OnScreen(sp, 120)) continue;

        // Per-instance variation: subType used as seed for size/phase variation
        float seed  = e.subType * 1.618f;
        float spY   = sp.y;   // NO global bob — each creature owns its motion
        float ra    = revealAlpha * 0.80f;
        float sc    = 0.50f + 0.18f * revealAlpha + seed * 0.08f; // size varies per instance
        auto  S     = [&](float v){ return v * sc; };
        // Phase offset so multiple instances of same creature are out of sync
        float ph    = seed * 3.7f;

        int w = e.waveIdx % 28;

        switch(w){
        case 0:{ // OCEAN WORLD — 4 aquatic variations
            float wBob = sinf(t*0.35f+ph)*S(2.f);
            ra *= 0.82f; // ocean creatures are subtle background presences
            
            if(e.subType == 0){ // space whale — original majestic design, scaled up
                float wsc = sc * 1.25f; // whales are big but not overwhelming
                auto  W   = [&](float v){ return v * wsc; };
                float wBob = sinf(t*0.52f+ph)*W(2.f);
                float wTail = sinf(t*0.7f+ph)*0.25f;
                // shadow/outline layer
                DrawEllipse((int)(sp.x+W(1.f)),(int)(spY+W(1.f)+wBob),(int)W(29.f),(int)W(12.f),
                    ColorAlpha({3,25,45,255},ra*0.5f));
                // main body with defined shape
                DrawEllipse((int)sp.x,(int)(spY+wBob),(int)W(28.f),(int)W(11.f),
                    ColorAlpha({8,55,95,255},ra*0.92f));
                // body outline for definition
                DrawEllipseLines((int)sp.x,(int)(spY+wBob),(int)W(28.f),(int)W(11.f),
                    ColorAlpha({5,40,75,255},ra*0.7f));
                // lighter underbelly with gradient
                DrawEllipse((int)(sp.x-W(2.f)),(int)(spY-W(1.f)+wBob),(int)W(26.f),(int)W(9.f),
                    ColorAlpha({12,70,115,255},ra*0.70f));
                // back ridge detail
                for(int i = 0; i < 8; i++){
                    float rx = sp.x - W(22.f) + i*W(6.f);
                    DrawCircleV({rx,spY-W(7.f+sinf(i*0.8f)*2.f)+wBob},W(1.5f),
                        ColorAlpha({10,60,100,255},ra*0.65f));
                }
                // bioluminescent belly stripes with defined pattern
                DrawEllipse((int)sp.x,(int)(spY+W(4.f)+wBob),(int)W(22.f),(int)W(4.f),
                    ColorAlpha(VP_DEEPCYAN,ra*0.22f));
                DrawEllipse((int)sp.x,(int)(spY+W(5.f)+wBob),(int)W(20.f),(int)W(3.f),
                    ColorAlpha(VP_CYAN,ra*0.30f));
                // biolum spots along belly
                for(int i = 0; i < 10; i++){
                    float bx = sp.x - W(18.f) + i*W(4.f);
                    float bPulse = 0.4f + 0.6f*sinf(t*2.f+i*0.6f+ph);
                    DrawCircleV({bx,spY+W(6.f)+wBob},W(1.2f),ColorAlpha(VP_DEEPCYAN,ra*bPulse*0.7f));
                    DrawNeonCircle({bx,spY+W(6.f)+wBob},W(1.5f),VP_CYAN,ra*bPulse*0.4f);
                }
                DrawNeonCircle({sp.x,spY+W(4.f)+wBob},W(24.f),VP_CYAN,ra*0.08f);
                // dorsal fin with defined structure
                DrawTriangle({sp.x+W(4.f),spY-W(9.f)+wBob},{sp.x-W(4.f),spY-W(20.f)+wBob},
                    {sp.x+W(14.f),spY-W(8.f)+wBob},ColorAlpha({8,55,90,255},ra*0.82f));
                DrawLineEx({sp.x+W(4.f),spY-W(9.f)+wBob},{sp.x-W(4.f),spY-W(20.f)+wBob},
                    W(0.8f),ColorAlpha({5,35,65,255},ra*0.8f));
                DrawLineEx({sp.x-W(4.f),spY-W(20.f)+wBob},{sp.x+W(14.f),spY-W(8.f)+wBob},
                    W(0.8f),ColorAlpha({5,35,65,255},ra*0.8f));
                // fin rays
                for(int i = 0; i < 3; i++){
                    DrawLineEx({sp.x+W(1.f+i*3.f),spY-W(9.f)+wBob},{sp.x+W(i*3.f),spY-W(15.f)+wBob},
                        W(0.5f),ColorAlpha({10,65,105,255},ra*0.5f));
                }
                // tail fluke with detailed structure
                float tx = sp.x+W(26.f), ty = spY+wBob;
                // tail stock
                DrawEllipse((int)(tx-W(4.f)),(int)ty,(int)W(5.f),(int)W(4.f),
                    ColorAlpha({8,52,88,255},ra*0.85f));
                // fluke outline
                DrawTriangle({tx+W(1.f),ty+W(2.f)},{tx+W(11.f),ty-W(8.f)+wTail*W(12.f)},
                    {tx+W(11.f),ty+W(8.f)+wTail*W(12.f)},ColorAlpha({5,35,65,255},ra*0.9f));
                DrawTriangle({tx,ty+W(2.f)},{tx+W(10.f),ty-W(8.f)+wTail*W(12.f)},
                    {tx+W(10.f),ty+W(8.f)+wTail*W(12.f)},ColorAlpha({8,50,85,255},ra*0.85f));
                // fluke edge definition
                DrawLineEx({tx,ty+W(2.f)},{tx+W(10.f),ty-W(8.f)+wTail*W(12.f)},
                    W(0.8f),ColorAlpha({5,35,65,255},ra*0.8f));
                DrawLineEx({tx,ty+W(2.f)},{tx+W(10.f),ty+W(8.f)+wTail*W(12.f)},
                    W(0.8f),ColorAlpha({5,35,65,255},ra*0.8f));
                // pectoral fins with defined edges
                DrawTriangle({sp.x-W(5.f),spY+W(5.f)+wBob},{sp.x-W(18.f),spY+W(18.f)+wBob},
                    {sp.x+W(6.f),spY+W(8.f)+wBob},ColorAlpha({6,45,80,255},ra*0.75f));
                DrawLineEx({sp.x-W(5.f),spY+W(5.f)+wBob},{sp.x-W(18.f),spY+W(18.f)+wBob},
                    W(0.7f),ColorAlpha({4,30,60,255},ra*0.8f));
                DrawLineEx({sp.x-W(18.f),spY+W(18.f)+wBob},{sp.x+W(6.f),spY+W(8.f)+wBob},
                    W(0.7f),ColorAlpha({4,30,60,255},ra*0.8f));
                // blunt head with more definition
                DrawEllipse((int)(sp.x-W(27.f)),(int)(spY+W(1.f)+wBob),(int)W(10.f),(int)W(8.f),
                    ColorAlpha({6,45,80,255},ra*0.90f));
                DrawEllipse((int)(sp.x-W(26.f)),(int)(spY+W(1.f)+wBob),(int)W(9.f),(int)W(7.f),
                    ColorAlpha({8,50,90,255},ra*0.88f));
                DrawEllipseLines((int)(sp.x-W(26.f)),(int)(spY+W(1.f)+wBob),(int)W(9.f),(int)W(7.f),
                    ColorAlpha({5,35,65,255},ra*0.7f));
                // mouth line
                DrawLineEx({sp.x-W(30.f),spY+W(3.f)+wBob},{sp.x-W(22.f),spY+W(4.f)+wBob},
                    W(0.8f),ColorAlpha({4,30,55,255},ra*0.7f));
                // gentle eye with detailed structure
                DrawCircleV({sp.x-W(18.f),spY-W(3.f)+wBob},W(4.5f),ColorAlpha({3,25,50,255},ra*0.98f)); // eye socket
                DrawCircleV({sp.x-W(18.f),spY-W(3.f)+wBob},W(4.f),ColorAlpha({4,35,60,255},ra*0.96f));
                DrawCircleV({sp.x-W(18.f),spY-W(3.f)+wBob},W(2.8f),ColorAlpha({90,155,200,255},ra*0.6f)); // iris
                DrawCircleV({sp.x-W(17.5f),spY-W(3.5f)+wBob},W(2.f),ColorAlpha({60,120,160,255},ra*0.7f));
                DrawCircleV({sp.x-W(18.f),spY-W(3.f)+wBob},W(1.5f),ColorAlpha(VP_BLACK,ra)); // pupil
                // eye highlight
                DrawCircleV({sp.x-W(17.f),spY-W(4.f)+wBob},W(0.8f),ColorAlpha(VP_WHITE,ra*0.8f));
                DrawNeonCircle({sp.x-W(18.f),spY-W(3.f)+wBob},W(4.f),VP_TEAL,ra*0.35f);
                // blowholes with defined structure
                DrawEllipse((int)(sp.x-W(10.f)),(int)(spY-W(9.f)+wBob),(int)W(2.f),(int)W(1.5f),
                    ColorAlpha({5,30,55,255},ra*0.8f));
                DrawEllipse((int)(sp.x-W(6.f)),(int)(spY-W(9.f)+wBob),(int)W(2.f),(int)W(1.5f),
                    ColorAlpha({5,30,55,255},ra*0.8f));
                // plasma exhale
                float ba = fmodf(t*0.6f+ph,2.8f);
                if(ba < 2.f){
                    float bAlpha = ba < 0.4f ? ba/0.4f : ba > 1.6f ? (2.f-ba)/0.4f : 0.85f;
                    for(int i = 0; i < 3; i++){
                        float bi = (float)i/3.f;
                        DrawCircleV({sp.x-W(10.f),spY-W(10.f)+wBob-ba*W(5.f)-bi*W(3.f)},
                            W(2.5f-ba*0.7f-bi*0.8f),ColorAlpha(VP_CYAN,ra*bAlpha*(1.f-bi)*0.4f));
                    }
                    DrawNeonCircle({sp.x-W(10.f),spY-W(10.f)+wBob-ba*W(5.f)},
                        W(3.f-ba*0.7f),VP_CYAN,ra*bAlpha*0.6f);
                }
            }
            else if(e.subType == 1){ // sea turtle — ancient guardian drifting on a slow arc
                float tsc = sc * 0.52f; auto T = [&](float v){ return v * tsc; };
                float glideX = sinf(t*0.14f+ph)*T(6.f) + sinf(t*0.09f+ph*1.7f)*T(3.f);
                float glideY = cosf(t*0.18f+ph)*T(4.f);
                float tX = sp.x + glideX;
                float tY = spY + glideY;
                float fSwim = sinf(t*0.85f+ph); // flipper stroke
                float headNod = sinf(t*0.45f+ph)*T(1.2f);
                // ── front flippers (behind shell in draw order) ──
                float fFront = fSwim * T(5.f);
                DrawTriangle({tX-T(4.f), tY},
                             {tX-T(4.f), tY - T(16.f) + fFront},
                             {tX+T(8.f), tY - T(10.f)},
                             ColorAlpha({8,55,44,255}, ra*0.72f));
                DrawTriangle({tX-T(4.f), tY},
                             {tX-T(4.f), tY + T(16.f) - fFront},
                             {tX+T(8.f), tY + T(10.f)},
                             ColorAlpha({8,55,44,255}, ra*0.72f));
                // ── rear flippers ──
                float fRear = sinf(t*0.85f+ph+1.1f) * T(3.5f);
                DrawTriangle({tX+T(8.f), tY},
                             {tX+T(14.f), tY - T(12.f) + fRear},
                             {tX+T(18.f), tY - T(4.f)},
                             ColorAlpha({7,48,38,255}, ra*0.65f));
                DrawTriangle({tX+T(8.f), tY},
                             {tX+T(14.f), tY + T(12.f) - fRear},
                             {tX+T(18.f), tY + T(4.f)},
                             ColorAlpha({7,48,38,255}, ra*0.65f));
                // ── shell ──
                DrawEllipse((int)tX,(int)tY,(int)T(20.f),(int)T(15.f),
                    ColorAlpha({8,54,42,255},ra*0.92f));
                // dome highlight
                DrawEllipse((int)(tX-T(4.f)),(int)(tY-T(3.f)),(int)T(14.f),(int)T(9.f),
                    ColorAlpha({14,82,64,255},ra*0.52f));
                // scute pattern — 3 lines: one spine, two diagonals
                DrawLineEx({tX-T(10.f),tY},{tX+T(10.f),tY}, T(0.7f),ColorAlpha({4,28,20,255},ra*0.45f));
                DrawLineEx({tX,tY-T(10.f)},{tX+T(8.f),tY+T(8.f)}, T(0.6f),ColorAlpha({4,28,20,255},ra*0.38f));
                DrawLineEx({tX,tY-T(10.f)},{tX-T(8.f),tY+T(8.f)}, T(0.6f),ColorAlpha({4,28,20,255},ra*0.38f));
                // shell outline
                DrawEllipseLines((int)tX,(int)tY,(int)T(20.f),(int)T(15.f),
                    ColorAlpha({5,35,26,255},ra*0.70f));
                // bioluminescent glow edge — slow pulse
                float glowPulse = 0.4f+0.6f*sinf(t*1.3f+ph);
                DrawNeonCircle({tX,tY},T(21.f),VP_TEAL,ra*glowPulse*0.18f);
                // ── neck ──
                DrawEllipse((int)(tX-T(18.f)),(int)tY,(int)T(5.f),(int)T(3.5f),
                    ColorAlpha({10,62,48,255},ra*0.82f));
                // ── head ──
                DrawCircleV({tX-T(24.f),tY+headNod},T(7.f),ColorAlpha({10,62,48,255},ra*0.88f));
                DrawCircleV({tX-T(24.f),tY+headNod},T(5.5f),ColorAlpha({14,80,60,255},ra*0.72f));
                // eye
                DrawCircleV({tX-T(21.f),tY-T(3.f)+headNod},T(2.2f),ColorAlpha({2,12,10,255},ra*0.98f));
                DrawCircleV({tX-T(21.f),tY-T(3.f)+headNod},T(1.3f),ColorAlpha({28,145,105,255},ra*0.85f));
                DrawCircleV({tX-T(21.f),tY-T(3.f)+headNod},T(0.6f),ColorAlpha(VP_BLACK,ra));
                DrawCircleV({tX-T(20.5f),tY-T(3.6f)+headNod},T(0.45f),ColorAlpha(VP_WHITE,ra*0.85f));
            }
            else if(e.subType == 2){ // jellyfish — translucent bell with flowing tentacles
                float jsc = sc * 0.52f; auto J = [&](float v){ return v * jsc; };
                float jPulse = 0.5f + 0.5f*sinf(t*1.5f+ph);
                // Gentle drift — jellyfish rises slightly on each bell contraction
                float driftX = sinf(t*0.13f+ph)*J(5.f) + cosf(t*0.19f+ph*2.f)*J(2.f);
                float driftY = cosf(t*0.11f+ph)*J(4.f) + jPulse*J(0.4f);
                float jX = sp.x + driftX;
                float jY = spY + driftY;
                // bell contracts on pulse
                float bellW = J(14.f - jPulse*1.8f);
                float bellH = J(11.f - jPulse*1.2f);
                // shadow/glow underneath tentacles
                DrawEllipse((int)jX,(int)(jY+bellH+J(6.f)),(int)(bellW*0.75f),(int)J(3.f),
                    ColorAlpha({0,200,255,255},ra*0.06f));
                // outer halo glow
                DrawCircleV({jX,jY},bellW*1.18f,ColorAlpha({5,80,125,40},ra*0.18f));
                DrawNeonCircle({jX,jY},bellW,VP_CYAN,ra*jPulse*0.32f);
                // bell body — translucent stacked layers for depth
                DrawEllipse((int)jX,(int)jY,(int)bellW,(int)bellH,
                    ColorAlpha({18,88,128,68},ra*0.58f));
                DrawEllipse((int)jX,(int)(jY-J(1.5f)),(int)(bellW*0.82f),(int)(bellH*0.72f),
                    ColorAlpha({25,108,155,78},ra*0.48f));
                DrawEllipse((int)jX,(int)(jY-J(3.f)),(int)(bellW*0.60f),(int)(bellH*0.48f),
                    ColorAlpha({38,138,182,90},ra*0.38f));
                // bell apex highlight — bright luminous top
                DrawCircleV({jX,jY-bellH*0.78f},J(3.2f),ColorAlpha({110,210,245,255},ra*0.42f));
                DrawCircleV({jX,jY-bellH*0.78f},J(1.5f),ColorAlpha(VP_WHITE,ra*0.58f));
                // bell outline for definition
                DrawEllipseLines((int)jX,(int)jY,(int)bellW,(int)bellH,
                    ColorAlpha({30,150,195,255},ra*jPulse*0.38f));
                // radial ribbing lines
                for(int i = 0; i < 8; i++){
                    float rib = i*(2.f*PI/8.f);
                    DrawLineEx({jX,jY},
                        {jX+cosf(rib)*bellW*0.88f, jY+sinf(rib)*bellH*0.88f},
                        J(0.4f),ColorAlpha({42,165,205,255},ra*0.17f));
                }
                // inner radial organ structures (gonads) — 4 lobes
                for(int i = 0; i < 4; i++){
                    float oa = i*(PI/2.f) + t*0.04f + ph;
                    float ox = jX + cosf(oa)*J(5.f);
                    float oy = jY + sinf(oa)*J(3.2f);
                    DrawEllipse((int)ox,(int)oy,(int)J(3.0f),(int)J(2.0f),
                        ColorAlpha({185,95,190,255},ra*0.38f));
                    DrawEllipseLines((int)ox,(int)oy,(int)J(3.0f),(int)J(2.0f),
                        ColorAlpha({220,130,220,255},ra*0.28f));
                }
                // bioluminescent rim dots on bell edge
                for(int i = 0; i < 8; i++){
                    float rib = i*(2.f*PI/8.f);
                    float bPulse2 = 0.4f+0.6f*sinf(t*2.2f+i*0.5f+ph);
                    DrawCircleV({jX+cosf(rib)*bellW*0.88f,jY+sinf(rib)*bellH*0.88f},J(1.6f),
                        ColorAlpha(VP_DEEPCYAN,ra*bPulse2*0.75f));
                    DrawNeonCircle({jX+cosf(rib)*bellW*0.88f,jY+sinf(rib)*bellH*0.88f},J(2.4f),
                        VP_CYAN,ra*bPulse2*0.32f);
                }
                // oral arms — 4 thick frilly primary arms
                for(int i = 0; i < 4; i++){
                    float oa = (i-1.5f)*0.42f;
                    float oLen = J(20.f + sinf(t*0.8f+i*0.9f+ph)*6.f);
                    float ox2 = jX + sinf(oa)*J(5.f);
                    Vector2 prev = {ox2, jY+bellH};
                    for(int j = 1; j <= 8; j++){
                        float fj = (float)j/8.f;
                        float wag = sinf(t*1.2f+i*0.7f+j*0.5f+ph)*J(3.2f)*(1.f-fj*0.3f);
                        Vector2 curr = {prev.x+wag, prev.y+fj*oLen/8.f};
                        DrawLineEx(prev,curr,J(2.8f-fj*2.2f),ColorAlpha({32,118,162,180},ra*(1.f-fj*0.5f)*0.68f));
                        prev = curr;
                    }
                    DrawCircleV(prev,J(1.0f),ColorAlpha(VP_CYAN,ra*jPulse*0.38f));
                }
                // fine hair tentacles — thin sinuous trailing filaments
                for(int i = 0; i < 16; i++){
                    float ta = (i-8.f)/8.f * 0.75f;
                    float tLen = J(24.f + sinf(t*1.3f+i*0.5f+ph)*10.f);
                    Vector2 prev = {jX+sinf(ta)*bellW*0.72f, jY+bellH*0.92f};
                    for(int j = 1; j <= 8; j++){
                        float fj = (float)j/8.f;
                        Vector2 curr = {prev.x+sinf(t*2.2f+i*0.3f+j*0.4f+ph)*J(1.8f),
                                       prev.y+fj*tLen/8.f};
                        DrawLineEx(prev,curr,J(0.7f-fj*0.55f),ColorAlpha(VP_DEEPCYAN,ra*(1.f-fj*0.42f)*0.48f));
                        prev = curr;
                    }
                    DrawCircleV(prev,J(0.7f),ColorAlpha(VP_CYAN,ra*jPulse*0.38f));
                }
            }
            else if(e.subType == 5){ // SEAHORSE — upright drifter with iconic S-curve
                float hsc = sc * 0.52f; auto H = [&](float v){ return v * hsc; };
                float driftX = cosf(t*0.17f+ph)*H(3.f) + cosf(t*0.11f+ph*1.4f)*H(1.5f);
                float driftY = sinf(t*0.14f+ph)*H(4.f) + sinf(t*0.20f+ph*0.8f)*H(2.f);
                float sX = sp.x + driftX;
                float sY = spY + driftY;
                float sway = sinf(t*0.9f+ph)*H(1.2f);
                // ── body — tapered tube drawn as overlapping circles ──
                // Trunk: wide at top, narrows toward tail
                static const float bW[] = {5.5f,5.8f,5.6f,5.2f,4.6f,3.9f,3.1f,2.3f};
                static const float bY[] = { 0.f, 3.5f, 7.f,10.5f,14.f,17.5f,20.5f,23.f};
                for(int i = 0; i < 8; i++){
                    DrawCircleV({sX+sway, sY+H(bY[i])}, H(bW[i]),
                        ColorAlpha({10,75,108,255}, ra*0.85f));
                }
                // lighter belly highlight strip
                for(int i = 0; i < 7; i++){
                    DrawCircleV({sX+sway+H(1.5f), sY+H(bY[i]+1.5f)}, H(bW[i]*0.45f),
                        ColorAlpha({18,120,158,255}, ra*0.38f));
                }
                // ── curled tail ──
                for(int i = 0; i < 10; i++){
                    float ta = PI*0.5f + i*0.32f;
                    float tr = H(9.f - i*0.7f);
                    float tx = sX + sway - H(2.f) + cosf(ta)*tr;
                    float ty = sY + H(24.f) + i*H(2.2f);
                    DrawCircleV({tx,ty}, H(2.8f-i*0.22f), ColorAlpha({10,75,108,255},ra*0.82f));
                }
                // ── neck bend — small ellipse bridging trunk to head ──
                DrawEllipse((int)(sX+sway-H(2.f)),(int)(sY-H(4.f)),(int)H(4.f),(int)H(4.f),
                    ColorAlpha({12,88,122,255},ra*0.80f));
                // ── head — distinct rounded snout profile ──
                DrawCircleV({sX+sway-H(5.f),sY-H(10.f)},H(7.f),ColorAlpha({10,75,108,255},ra*0.88f));
                DrawCircleV({sX+sway-H(5.f),sY-H(10.f)},H(5.f),ColorAlpha({14,95,132,255},ra*0.72f));
                // snout — long tube pointing forward-right
                DrawLineEx({sX+sway-H(3.f),sY-H(14.f)},{sX+sway+H(11.f),sY-H(17.f)},
                    H(3.0f), ColorAlpha({10,75,108,255},ra*0.88f));
                DrawLineEx({sX+sway-H(3.f),sY-H(14.f)},{sX+sway+H(11.f),sY-H(17.f)},
                    H(1.2f), ColorAlpha({18,120,158,255},ra*0.55f));
                // coronet — 3 short spines on top of head
                for(int i = 0; i < 3; i++){
                    float ca2 = -PI*0.75f + i*0.28f;
                    DrawLineEx({sX+sway-H(7.f),sY-H(15.f)},
                        {sX+sway-H(7.f)+cosf(ca2)*H(5.f+i*1.f), sY-H(15.f)+sinf(ca2)*H(5.f+i*1.f)},
                        H(1.1f), ColorAlpha({18,148,192,255}, ra*0.60f));
                }
                // dorsal fin — 5 thin spines fanning off mid-trunk
                for(int i = 0; i < 5; i++){
                    float fa = -PI*0.5f - 0.3f + i*0.15f;
                    float flen = H(7.f + sinf(t*6.f+i*0.5f+ph)*1.8f);
                    DrawLineEx({sX+sway+H(5.f), sY+H(4.f)},
                        {sX+sway+H(5.f)+cosf(fa)*flen, sY+H(4.f)+sinf(fa)*flen},
                        H(0.85f), ColorAlpha({22,165,205,255}, ra*(0.55f-i*0.06f)));
                }
                // eye
                DrawCircleV({sX+sway-H(2.5f),sY-H(12.f)},H(2.0f),ColorAlpha({2,12,20,255},ra*0.98f));
                DrawCircleV({sX+sway-H(2.5f),sY-H(12.f)},H(1.2f),ColorAlpha({0,200,240,255},ra*0.85f));
                DrawCircleV({sX+sway-H(2.5f),sY-H(12.f)},H(0.55f),ColorAlpha(VP_BLACK,ra));
                DrawCircleV({sX+sway-H(2.f),sY-H(12.6f)},H(0.4f),ColorAlpha(VP_WHITE,ra*0.80f));
                // bioluminescent belly dots — slow drift down the body
                for(int i = 0; i < 4; i++){
                    float bage = fmodf(t*1.6f+i*0.28f+ph,1.f);
                    DrawCircleV({sX+sway, sY+H(bY[i*2])}, H(bW[i*2]*0.55f),
                        ColorAlpha({0,220,255,255}, ra*(1.f-bage)*0.45f));
                }
            }
            else if(e.subType == 6){ // SPACE MANTA RAY — vast flat ray, glides without effort
                float msc = sc * 0.58f; auto M = [&](float v){ return v * msc; };
                // Manta drifts on a gentle arc — weightless glide
                float driftX = cosf(t*0.11f+ph)*M(12.f) + cosf(t*0.07f+ph*1.6f)*M(5.f);
                float driftY = sinf(t*0.09f+ph)*M(6.f)  + sinf(t*0.15f+ph*0.6f)*M(3.f);
                float mX = sp.x + driftX;
                float mY = spY  + driftY;
                // Wing tips curl very slowly up and down, each side independent
                float wL = sinf(t*0.4f+ph)*M(5.f);       // left tip curl
                float wR = sinf(t*0.4f+ph+1.1f)*M(5.f);  // right tip, slightly offset
                float tail = sinf(t*0.55f+ph)*M(2.f);     // tail undulation
                // deep shadow beneath
                DrawEllipse((int)mX,(int)(mY+M(8.f)),(int)M(38.f),(int)M(7.f),
                    ColorAlpha({0,0,0,255},ra*0.20f));
                // outer aura glow
                DrawEllipse((int)mX,(int)mY,(int)M(40.f),(int)M(14.f),
                    ColorAlpha({0,55,115,28},ra*0.18f));
                // left wing — multi-layer for thickness and shading
                DrawTriangle(
                    {mX-M(2.f),   mY},
                    {mX-M(36.f),  mY+M(7.f)+wL},
                    {mX-M(20.f),  mY-M(6.5f)},
                    ColorAlpha({7,42,72,255},ra*0.88f));
                DrawTriangle(
                    {mX-M(2.f),   mY},
                    {mX-M(36.f),  mY+M(7.f)+wL},
                    {mX-M(16.f),  mY+M(2.5f)},
                    ColorAlpha({9,52,85,255},ra*0.72f));
                // left wing surface vein lines
                DrawLineEx({mX-M(8.f),mY-M(3.f)},{mX-M(30.f),mY+M(5.5f)+wL*0.7f},M(0.5f),
                    ColorAlpha({14,72,122,255},ra*0.38f));
                DrawLineEx({mX-M(5.f),mY+M(2.f)},{mX-M(26.f),mY+M(8.5f)+wL*0.62f},M(0.45f),
                    ColorAlpha({14,72,122,255},ra*0.30f));
                DrawLineEx({mX-M(11.f),mY+M(1.f)},{mX-M(22.f),mY-M(5.f)},M(0.4f),
                    ColorAlpha({16,82,132,255},ra*0.24f));
                // right wing
                DrawTriangle(
                    {mX+M(2.f),   mY},
                    {mX+M(36.f),  mY+M(7.f)+wR},
                    {mX+M(20.f),  mY-M(6.5f)},
                    ColorAlpha({7,42,72,255},ra*0.88f));
                DrawTriangle(
                    {mX+M(2.f),   mY},
                    {mX+M(36.f),  mY+M(7.f)+wR},
                    {mX+M(16.f),  mY+M(2.5f)},
                    ColorAlpha({9,52,85,255},ra*0.72f));
                DrawLineEx({mX+M(8.f),mY-M(3.f)},{mX+M(30.f),mY+M(5.5f)+wR*0.7f},M(0.5f),
                    ColorAlpha({14,72,122,255},ra*0.38f));
                DrawLineEx({mX+M(5.f),mY+M(2.f)},{mX+M(26.f),mY+M(8.5f)+wR*0.62f},M(0.45f),
                    ColorAlpha({14,72,122,255},ra*0.30f));
                DrawLineEx({mX+M(11.f),mY+M(1.f)},{mX+M(22.f),mY-M(5.f)},M(0.4f),
                    ColorAlpha({16,82,132,255},ra*0.24f));
                // wing leading-edge highlights
                DrawLineEx({mX-M(2.f),mY-M(1.f)},{mX-M(36.f),mY+M(6.5f)+wL},M(0.95f),
                    ColorAlpha({18,88,140,255},ra*0.58f));
                DrawLineEx({mX+M(2.f),mY-M(1.f)},{mX+M(36.f),mY+M(6.5f)+wR},M(0.95f),
                    ColorAlpha({18,88,140,255},ra*0.58f));
                // main body disc — solid center mass
                DrawEllipse((int)mX,(int)mY,(int)M(15.f),(int)M(10.f),
                    ColorAlpha({6,38,66,255},ra*0.92f));
                DrawEllipse((int)mX,(int)(mY+M(1.f)),(int)M(13.f),(int)M(8.f),
                    ColorAlpha({10,55,92,255},ra*0.82f));
                DrawEllipseLines((int)mX,(int)mY,(int)M(15.f),(int)M(10.f),
                    ColorAlpha({14,72,122,255},ra*0.38f));
                // underbelly — lighter paler center
                DrawEllipse((int)mX,(int)(mY+M(2.f)),(int)M(9.f),(int)M(5.f),
                    ColorAlpha({20,90,140,255},ra*0.32f));
                // gill slits on underbelly — 5 short curved lines
                for(int i = -2; i <= 2; i++){
                    float gx = mX + i*M(2.4f);
                    DrawLineEx({gx-M(0.9f),mY+M(3.5f)},{gx+M(0.9f),mY+M(5.5f)},M(0.75f),
                        ColorAlpha({5,35,62,255},ra*0.72f));
                }
                // dorsal spine ridge
                DrawLineEx({mX,mY-M(9.f)},{mX,mY+M(4.5f)},M(0.85f),
                    ColorAlpha({14,68,112,255},ra*0.68f));
                // cephalic horns — large prominent forward fins with outline
                for(int s = -1; s <= 1; s += 2){
                    DrawLineEx({mX+s*M(6.f),mY-M(9.f)},{mX+s*M(12.f),mY-M(18.f)},M(2.2f),
                        ColorAlpha({10,58,98,255},ra*0.84f));
                    DrawLineEx({mX+s*M(6.f),mY-M(9.f)},{mX+s*M(12.f),mY-M(18.f)},M(0.9f),
                        ColorAlpha({18,88,140,255},ra*0.55f));
                    DrawCircleV({mX+s*M(12.f),mY-M(18.f)},M(1.5f),
                        ColorAlpha({12,65,105,255},ra*0.78f));
                }
                // eyes — full socket/iris/pupil/highlight treatment
                float ePulse = 0.6f+0.4f*sinf(t*1.8f+ph);
                for(int s = -1; s <= 1; s += 2){
                    float ex = mX + s*M(5.f);
                    DrawCircleV({ex,mY-M(4.f)},M(2.8f),ColorAlpha({3,18,35,255},ra*0.98f)); // socket
                    DrawCircleV({ex,mY-M(4.f)},M(2.2f),ColorAlpha({5,30,55,255},ra*0.96f));
                    DrawCircleV({ex,mY-M(4.f)},M(1.5f),ColorAlpha({42,158,228,255},ra*ePulse*0.90f)); // iris
                    DrawCircleV({ex,mY-M(4.f)},M(0.7f),ColorAlpha(VP_BLACK,ra)); // pupil
                    DrawCircleV({ex-M(0.5f),mY-M(4.6f)},M(0.5f),ColorAlpha(VP_WHITE,ra*0.82f)); // highlight
                    DrawNeonCircle({ex,mY-M(4.f)},M(2.2f),VP_CYAN,ra*ePulse*0.28f);
                }
                // mouth line
                DrawLineEx({mX-M(7.f),mY+M(5.f)},{mX+M(7.f),mY+M(5.f)},M(0.85f),
                    ColorAlpha({4,28,52,255},ra*0.65f));
                // bioluminescent wing spots — two rows per wing
                for(int s = -1; s <= 1; s += 2){
                    for(int i = 1; i <= 5; i++){
                        float bx = mX + s*i*M(6.5f);
                        float by = mY + M(1.5f) + (s==-1?wL:wR)*0.4f*(float)i/5.f;
                        float bPulse2 = 0.4f+0.6f*sinf(t*2.f+s*i*0.4f+ph);
                        DrawCircleV({bx,by},M(1.3f),ColorAlpha({62,188,248,255},ra*bPulse2*0.68f));
                        DrawNeonCircle({bx,by},M(2.2f),VP_CYAN,ra*bPulse2*0.28f);
                    }
                }
                // bioluminescent spine row
                for(int i=-2;i<=2;i++){
                    float bPulse3 = 0.4f+0.6f*sinf(t*2.f+i*0.5f+ph);
                    DrawCircleV({mX+i*M(3.2f),mY},M(1.0f),ColorAlpha({62,188,248,255},ra*bPulse3*0.72f));
                }
                // tail — long graceful three-segment whip
                DrawLineEx({mX,mY+M(5.5f)},{mX+tail,mY+M(20.f)},M(2.0f),
                    ColorAlpha({8,48,80,255},ra*0.86f));
                DrawLineEx({mX+tail,mY+M(20.f)},{mX+tail*0.6f,mY+M(35.f)},M(1.1f),
                    ColorAlpha({8,48,80,255},ra*0.68f));
                DrawLineEx({mX+tail*0.6f,mY+M(35.f)},{mX+tail*0.3f,mY+M(46.f)},M(0.6f),
                    ColorAlpha({8,48,80,255},ra*0.45f));
            }
            else{ // bioluminescent plankton cloud — swarm of glowing micro-organisms
                float psc = sc * 0.48f; auto P = [&](float v){ return v * psc; };
                // cloud drifts on a gentle current
                float driftX = cosf(t*0.10f+ph)*P(4.f) + sinf(t*0.14f+ph*1.5f)*P(2.f);
                float driftY = sinf(t*0.11f+ph)*P(3.f);
                float cX = sp.x + driftX;
                float cY = spY + driftY;
                // outer ambient glow — the cloud lights up the surrounding water
                DrawCircleV({cX,cY},P(22.f),ColorAlpha(VP_DEEPCYAN,ra*0.06f));
                DrawNeonCircle({cX,cY},P(22.f),VP_CYAN,ra*0.10f);
                DrawCircleV({cX,cY},P(16.f),ColorAlpha(VP_DEEPCYAN,ra*0.10f));
                // inner core glow — denser concentration at centre
                DrawCircleV({cX,cY},P(9.f),ColorAlpha({0,200,240,255},ra*0.18f));
                DrawNeonCircle({cX,cY},P(10.f),VP_CYAN,ra*0.22f);
                // secondary sub-clusters orbiting the main swarm
                for(int c = 0; c < 4; c++){
                    float ca2 = t*0.25f + c*(2.f*PI/4.f) + ph;
                    float cr = P(10.f + sinf(t*0.5f+c*0.9f)*3.f);
                    float cx2 = cX + cosf(ca2)*cr;
                    float cy2 = cY + sinf(ca2)*cr*0.7f;
                    DrawCircleV({cx2,cy2},P(4.5f),ColorAlpha(VP_DEEPCYAN,ra*0.12f));
                    DrawNeonCircle({cx2,cy2},P(5.f),VP_CYAN,ra*(0.4f+0.6f*sinf(t*2.f+c+ph))*0.18f);
                }
                // individual plankton — 42 tiny lights at varied sizes and brightness
                for(int i = 0; i < 42; i++){
                    float ipx = cX + sinf(i*2.4f+t*0.6f+ph)*P(16.f);
                    float ipy = cY + cosf(i*1.8f+t*0.5f+ph)*P(13.f);
                    float iSize = P(0.45f + (i%4)*0.28f);
                    float iGlow = 0.5f + 0.5f*sinf(t*3.f+i*0.7f+ph);
                    float iAlpha = ra*iGlow*(0.65f + 0.35f*(i%3==0?1.f:0.f));
                    DrawCircleV({ipx,ipy},iSize,ColorAlpha(VP_CYAN,iAlpha*0.85f));
                    // larger organisms get a visible body shape — faint oval
                    if(i % 5 == 0){
                        DrawEllipse((int)ipx,(int)ipy,(int)(iSize*1.8f),(int)(iSize*1.2f),
                            ColorAlpha({0,180,220,255},ra*iGlow*0.22f));
                    }
                    // brightest ones also emit a tiny halo ring
                    if(i % 4 == 0){
                        DrawNeonCircle({ipx,ipy},iSize+P(0.6f),VP_DEEPCYAN,ra*iGlow*0.45f);
                    }
                }
                // slow spiral swirl trails — motion streaks showing the current
                for(int i = 0; i < 6; i++){
                    float sa = t*0.38f + i*(2.f*PI/6.f) + ph;
                    float sr = P(11.f + sinf(t*0.7f+i*1.1f)*4.f);
                    float sTrail = 0.35f + 0.65f*sinf(t*1.5f+i*0.8f+ph);
                    DrawCircleV({cX+cosf(sa)*sr, cY+sinf(sa)*sr*0.75f},P(0.7f),
                        ColorAlpha(VP_CYAN,ra*sTrail*0.30f));
                    DrawNeonCircle({cX+cosf(sa)*sr, cY+sinf(sa)*sr*0.75f},P(1.2f),
                        VP_DEEPCYAN,ra*sTrail*0.14f);
                }
                // expanding pulse rings — the swarm breathes
                for(int r = 0; r < 3; r++){
                    float rAge = fmodf(t*0.55f + r/3.f + ph*0.3f, 1.f);
                    DrawNeonCircle({cX,cY},P(6.f)+rAge*P(18.f),VP_CYAN,(1.f-rAge)*ra*0.20f);
                }
            }
            break;}

        case 1:{ // TOXIC SWAMP — radioactive tadpole swarm: dozens of glowing mutant frogspawn
            if(e.subType < 5){
            // Draw a cluster of 8 tadpoles orbiting a nucleus
            float swarmSpin = t * 0.3f + ph;
            for(int i = 0; i < 8; i++){
                float ta = swarmSpin + i*(2.f*PI/8.f);
                float tr = S(12.f + sinf(t*1.4f+i*0.7f+ph)*4.f);
                float tx = sp.x + cosf(ta)*tr;
                float ty = spY  + sinf(ta)*tr*0.65f;
                float tphase = fmodf(t*1.2f + i*0.4f + ph, 1.f);
                // body
                DrawEllipse((int)tx,(int)ty,(int)S(4.f),(int)S(3.f),
                    ColorAlpha({60,180,20,255},ra*0.82f));
                DrawNeonCircle({tx,ty},S(4.f),VP_MINT,ra*(0.25f+0.2f*sinf(t*3.f+i+ph)));
                // tail (wiggling)
                float tailA = ta + PI + sinf(t*3.f+i*0.6f+ph)*0.4f;
                DrawLineEx({tx,ty},{tx+cosf(tailA)*S(7.f),ty+sinf(tailA)*S(5.f)},
                    S(0.8f),ColorAlpha({80,200,30,255},ra*0.55f));
                // eye
                DrawCircleV({tx+cosf(ta+0.3f)*S(2.5f),ty+sinf(ta+0.3f)*S(2.f)},S(1.f),
                    ColorAlpha(VP_BLACK,ra*0.9f));
            }
            // central toxic nucleus (glowing green orb)
            float np = 0.6f+0.4f*sinf(t*2.5f+ph);
            DrawCircleV({sp.x,spY},S(7.f),ColorAlpha({20,100,5,255},ra*0.85f));
            DrawNeonCircle({sp.x,spY},S(7.f),VP_MINT,ra*np*0.55f);
            DrawNeonCircle({sp.x,spY},S(11.f),VP_YELLOW,ra*np*0.2f);
            DrawCircleV({sp.x,spY},S(3.f),ColorAlpha({120,230,40,255},ra*np*0.8f));
            }
            else if(e.subType == 5){ // VENUS FLYtrap — mutant acid-dripping carnivorous plant
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float snap = 0.5f + 0.5f*sinf(t * 0.7f + ph);
                float drip = fmodf(t * 1.4f + ph, 1.f);
                // stem — thick segmented stalk
                for(int i = 0; i < 5; i++){
                    float sy2 = spY + S(6.f) + i * S(5.f);
                    float sw2 = S(4.f - i*0.3f);
                    DrawCircleV({sp.x, sy2}, sw2, ColorAlpha({30,90,10,255}, ra*0.90f));
                    DrawLineEx({sp.x - sw2, sy2}, {sp.x + sw2, sy2}, S(0.8f), ColorAlpha({20,60,5,255}, ra*0.5f));
                }
                // leaves
                for(int side = -1; side <= 1; side += 2){
                    float lx = sp.x + side * S(12.f);
                    float ly = spY + S(15.f);
                    DrawEllipse((int)lx, (int)ly, (int)S(8.f), (int)S(4.f), ColorAlpha({25,80,8,255}, ra*0.80f));
                    DrawEllipseLines((int)lx, (int)ly, (int)S(8.f), (int)S(4.f), ColorAlpha({15,50,5,255}, ra*0.6f));
                }
                // trap head — two hinged jaws
                float jawOpen = snap * S(14.f);
                // bottom jaw
                DrawEllipse((int)sp.x, (int)(spY + jawOpen * 0.4f), (int)S(14.f), (int)S(6.f),
                    ColorAlpha({35,110,12,255}, ra*0.90f));
                // top jaw
                DrawEllipse((int)sp.x, (int)(spY - jawOpen * 0.4f), (int)S(14.f), (int)S(6.f),
                    ColorAlpha({35,110,12,255}, ra*0.90f));
                // inner mouth — acid glow
                DrawEllipse((int)sp.x, (int)spY, (int)S(13.f*snap), (int)S(5.f*snap),
                    ColorAlpha({120,255,40,255}, ra*0.70f));
                DrawNeonCircle({sp.x, spY}, S(8.f), {120,255,40,255}, ra*0.4f*snap);
                // teeth spines
                for(int i = 0; i < 6; i++){
                    float tx2 = sp.x - S(11.f) + i * S(4.4f);
                    DrawLineEx({tx2, spY + jawOpen*0.3f}, {tx2, spY + jawOpen*0.3f + S(4.f)},
                        S(1.2f), ColorAlpha({200,255,80,255}, ra*0.85f));
                    DrawLineEx({tx2, spY - jawOpen*0.3f}, {tx2, spY - jawOpen*0.3f - S(4.f)},
                        S(1.2f), ColorAlpha({200,255,80,255}, ra*0.85f));
                }
                // acid drips
                for(int i = 0; i < 3; i++){
                    float doff = fmodf(drip + i*0.33f, 1.f);
                    float dx2 = sp.x + (i-1)*S(6.f);
                    float dy2 = spY + S(6.f) + doff * S(18.f);
                    DrawCircleV({dx2, dy2}, S(2.f*(1.f-doff)+0.5f), ColorAlpha({150,255,50,255}, ra*(1.f-doff)*0.75f));
                    DrawNeonCircle({dx2, dy2}, S(3.f), {120,220,30,255}, ra*(1.f-doff)*0.35f);
                }
                // eye in centre
                DrawCircleV({sp.x, spY}, S(3.f), ColorAlpha({5,20,2,255}, ra*0.95f));
                DrawCircleV({sp.x, spY}, S(1.8f), ColorAlpha({200,255,60,255}, ra*pulse));
            }
            else if(e.subType == 6){ // SLUDGE WORM — segmented toxic worm burrowing through the ground
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float wormWave=sinf(t*1.5f+ph);
                float bubbleAge=fmodf(t*1.8f+ph,1.f);
                // toxic ground puddle
                DrawEllipse((int)sp.x,(int)(spY+S(10.f)),(int)S(22.f),(int)S(6.f),ColorAlpha({80,130,10,255},ra*0.25f));
                DrawNeonCircle({sp.x,spY+S(10.f)},S(22.f),{100,180,15,255},ra*0.12f);
                // 8 body segments
                for(int i=0;i<8;i++){
                    float fi=(float)i/8.f;
                    float sx2=sp.x+(i-3.5f)*S(5.5f)+wormWave*S(3.f)*sinf(i*0.6f+ph);
                    float sy2=spY+sinf(t*2.f+ph+i*0.45f)*S(5.f);
                    float sr=S(6.5f-fi*2.f);
                    // segment body
                    DrawCircleV({sx2,sy2},sr,ColorAlpha({40,80,8,255},ra*0.90f));
                    DrawCircleV({sx2,sy2},sr*0.75f,ColorAlpha({65,120,12,255},ra*0.82f));
                    // segment ring
                    DrawCircleLinesV({sx2,sy2},sr,ColorAlpha({90,160,15,255},ra*0.40f));
                    // toxic slime seam
                    if(i%2==0)
                        DrawCircleV({sx2,sy2+sr*0.8f},S(1.5f),ColorAlpha({150,240,30,255},ra*0.65f));
                    // bristles
                    for(int b=-1;b<=1;b+=2){
                        DrawLineEx({sx2,sy2},{sx2+b*S(1.5f),sy2+sr+S(3.f)},S(0.8f),ColorAlpha({75,130,12,255},ra*0.50f));
                    }
                }
                // head
                float hx=sp.x-S(20.f)+wormWave*S(3.f);
                float hy=spY+sinf(t*2.f+ph)*S(5.f);
                DrawCircleV({hx,hy},S(8.f),ColorAlpha({35,72,7,255},ra*0.92f));
                DrawCircleV({hx,hy},S(6.f),ColorAlpha({60,110,11,255},ra*0.85f));
                // mandibles
                for(int side=-1;side<=1;side+=2){
                    DrawLineEx({hx,hy+S(4.f)},{hx+side*S(10.f),hy+S(10.f)},S(2.5f),ColorAlpha({45,85,9,255},ra*0.82f));
                    DrawCircleV({hx+side*S(10.f),hy+S(10.f)},S(2.5f),ColorAlpha({150,240,30,255},ra*0.78f));
                    DrawNeonCircle({hx+side*S(10.f),hy+S(10.f)},S(4.f),{120,200,20,255},ra*pulse*0.40f);
                }
                // eyes — glowing toxic
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({hx+side*S(3.5f),hy-S(2.f)},S(2.5f),ColorAlpha({5,10,1,255},ra*0.95f));
                    DrawCircleV({hx+side*S(3.5f),hy-S(2.f)},S(1.4f),ColorAlpha({180,255,30,255},ra*0.90f));
                    DrawNeonCircle({hx+side*S(3.5f),hy-S(2.f)},S(3.5f),{140,220,20,255},ra*pulse*0.55f);
                }
                // toxic bubbles rising
                for(int i=0;i<5;i++){
                    float ba=fmodf(bubbleAge+i*0.2f,1.f);
                    DrawCircleV({sp.x+(i-2)*S(8.f),spY+S(8.f)-ba*S(25.f)},
                        S(2.f+ba),ColorAlpha({120,210,20,255},ra*(1.f-ba)*0.45f));
                    DrawCircleLinesV({sp.x+(i-2)*S(8.f),spY+S(8.f)-ba*S(25.f)},
                        S(2.f+ba),ColorAlpha({160,255,35,255},ra*(1.f-ba)*0.35f));
                }
            }
            break;}

        case 2:{ // EMBER FIELDS — 4 lava/fire variations
            float lPulse = 0.5f + 0.5f * sinf(t * 3.f + ph);
            
            if(e.subType == 0){ // lava golem — hulking magma creature
                // independent per-part animation
                float breathe  = sinf(t*0.55f+ph)*S(1.2f);
                float headNod  = sinf(t*0.38f+ph+0.8f)*S(0.9f);
                float lArmSway = sinf(t*0.55f+ph+1.2f)*S(2.0f);
                float rArmSway = sinf(t*0.55f+ph+1.2f+PI)*S(2.0f);
                float lLegShift= sinf(t*0.28f+ph)*S(0.8f);
                float rLegShift= sinf(t*0.28f+ph+PI)*S(0.8f);
                Color lC1={50,15,3,255};   // dark lava rock
                Color lC2={90,25,5,255};   // mid lava rock
                Color lC3={130,40,6,255};  // light lava rock
                Color lGlow={255,130,50,255}; // pure orange — explicit, no VP_ alias
                Color lHot ={255,220,80,255}; // hot yellow-white
                Color lCrack={180,65,8,255};  // crack dark orange
                // lava pool — fixed
                DrawEllipse((int)sp.x,(int)(spY+S(22.f)),(int)S(22.f),(int)S(8.f),ColorAlpha({60,10,2,255},ra*0.60f));
                DrawEllipse((int)sp.x,(int)(spY+S(22.f)),(int)S(18.f),(int)S(6.f),ColorAlpha({180,55,8,255},ra*0.70f));
                DrawEllipse((int)sp.x,(int)(spY+S(22.f)),(int)S(12.f),(int)S(4.f),ColorAlpha({240,130,20,255},ra*0.55f));
                // legs
                for(int side=-1;side<=1;side+=2){
                    float lx=sp.x+side*S(5.5f);
                    float ls=side==(-1)?lLegShift:rLegShift;
                    DrawRectangle((int)(lx-S(4.f)),(int)(spY+S(8.f)+ls),(int)S(8.f),(int)S(15.f),ColorAlpha(lC1,ra*0.95f));
                    DrawRectangle((int)(lx-S(3.f)),(int)(spY+S(9.f)+ls),(int)S(7.f),(int)S(14.f),ColorAlpha(lC2,ra*0.90f));
                    // lava crack down leg — two solid lines, no neon
                    DrawLineEx({lx,spY+S(10.f)+ls},{lx,spY+S(21.f)+ls},S(2.5f),ColorAlpha(lCrack,ra*0.85f));
                    DrawLineEx({lx,spY+S(10.f)+ls},{lx,spY+S(21.f)+ls},S(1.0f), ColorAlpha(lGlow, ra*lPulse*0.80f));
                    // knee — solid filled circle, orange
                    DrawCircleV({lx,spY+S(16.f)+ls},S(3.5f),ColorAlpha({100,28,4,255},ra*0.92f));
                    DrawCircleV({lx,spY+S(16.f)+ls},S(2.2f),ColorAlpha(lGlow,ra*lPulse*0.85f));
                }
                // torso
                DrawRectangle((int)(sp.x-S(15.f)),(int)(spY-S(7.f)+breathe),(int)S(30.f),(int)S(16.f),ColorAlpha(lC1,ra*0.95f));
                DrawRectangle((int)(sp.x-S(14.f)),(int)(spY-S(6.f)+breathe),(int)S(28.f),(int)S(14.f),ColorAlpha(lC2,ra*0.92f));
                DrawRectangle((int)(sp.x-S(12.f)),(int)(spY-S(4.f)+breathe),(int)S(24.f),(int)S(10.f),ColorAlpha(lC3,ra*0.65f));
                // diagonal fissures — solid lines only
                DrawLineEx({sp.x-S(10.f),spY-S(2.f)+breathe},{sp.x-S(1.f),spY+S(6.f)+breathe},S(3.f),ColorAlpha(lCrack,ra*0.90f));
                DrawLineEx({sp.x-S(10.f),spY-S(2.f)+breathe},{sp.x-S(1.f),spY+S(6.f)+breathe},S(1.5f),ColorAlpha(lGlow,ra*lPulse*0.90f));
                DrawLineEx({sp.x+S(2.f), spY-S(4.f)+breathe},{sp.x+S(10.f),spY+S(5.f)+breathe},S(3.f),ColorAlpha(lCrack,ra*0.90f));
                DrawLineEx({sp.x+S(2.f), spY-S(4.f)+breathe},{sp.x+S(10.f),spY+S(5.f)+breathe},S(1.5f),ColorAlpha(lGlow,ra*lPulse*0.90f));
                // chest heart — solid fills only
                DrawCircleV({sp.x,spY+S(1.f)+breathe},S(5.f), ColorAlpha({120,35,6,255},ra*0.90f));
                DrawCircleV({sp.x,spY+S(1.f)+breathe},S(3.5f),ColorAlpha(lGlow,          ra*lPulse*0.92f));
                DrawCircleV({sp.x,spY+S(1.f)+breathe},S(1.8f),ColorAlpha(lHot,           ra*lPulse));
                // arms
                for(int side=-1;side<=1;side+=2){
                    float ax=sp.x+side*S(19.f);
                    float aOff=side==(-1)?lArmSway:rArmSway;
                    DrawRectangle((int)(ax-S(4.f)),(int)(spY-S(5.f)+breathe+aOff),(int)S(8.f),(int)S(13.f),ColorAlpha(lC1,ra*0.92f));
                    DrawRectangle((int)(ax-S(3.f)),(int)(spY-S(4.f)+breathe+aOff),(int)S(7.f),(int)S(12.f),ColorAlpha(lC2,ra*0.87f));
                    // elbow — solid orange circle
                    DrawCircleV({ax,spY+S(8.f)+breathe+aOff},S(3.5f),ColorAlpha({100,28,4,255},ra*0.90f));
                    DrawCircleV({ax,spY+S(8.f)+breathe+aOff},S(2.0f),ColorAlpha(lGlow,          ra*lPulse*0.85f));
                    // fist
                    DrawRectangle((int)(ax-S(4.5f)),(int)(spY+S(8.f)+breathe+aOff),(int)S(9.f),(int)S(8.f),ColorAlpha(lC1,ra*0.95f));
                    DrawRectangle((int)(ax-S(3.5f)),(int)(spY+S(9.f)+breathe+aOff),(int)S(8.f),(int)S(7.f),ColorAlpha(lC2,ra*0.88f));
                    // knuckle cracks — lines not circles
                    DrawLineEx({ax-S(2.f),spY+S(9.f)+breathe+aOff},{ax-S(2.f),spY+S(15.f)+breathe+aOff},S(1.2f),ColorAlpha(lGlow,ra*lPulse*0.75f));
                    DrawLineEx({ax+S(1.f),spY+S(9.f)+breathe+aOff},{ax+S(1.f),spY+S(15.f)+breathe+aOff},S(1.2f),ColorAlpha(lGlow,ra*lPulse*0.75f));
                }
                // head
                float hY=spY-S(19.f)+headNod;
                DrawRectangle((int)(sp.x-S(11.f)),(int)hY,           (int)S(22.f),(int)S(13.f),ColorAlpha(lC1,ra*0.95f));
                DrawRectangle((int)(sp.x-S(10.f)),(int)(hY+S(1.f)),  (int)S(20.f),(int)S(12.f),ColorAlpha(lC2,ra*0.93f));
                DrawRectangle((int)(sp.x-S(9.f)), (int)(hY+S(2.f)),  (int)S(18.f),(int)S(9.f), ColorAlpha(lC3,ra*0.60f));
                // 3 solid horn spikes — rectangles tapering to a point
                for(int i=-1;i<=1;i++){
                    float hx2=sp.x+i*S(5.5f);
                    float hh=S(7.f+fabsf((float)i)*S(1.5f));
                    DrawRectangle((int)(hx2-S(2.5f)),(int)(hY-hh),(int)S(5.f),(int)hh,ColorAlpha(lC2,ra*0.90f));
                    DrawRectangle((int)(hx2-S(1.5f)),(int)(hY-hh),(int)S(3.f),(int)(hh*0.6f),ColorAlpha(lC3,ra*0.70f));
                    // horn tip — solid bright orange, no neon ring
                    DrawCircleV({hx2,hY-hh},S(2.2f),ColorAlpha(lGlow,ra*lPulse*0.92f));
                    DrawCircleV({hx2,hY-hh},S(1.0f),ColorAlpha(lHot, ra*lPulse));
                }
                // ember eyes — solid fills, no neon rings
                for(int side=-1;side<=1;side+=2){
                    float ex2=sp.x+side*S(5.f), ey2=hY+S(5.f);
                    DrawCircleV({ex2,ey2},S(3.8f),ColorAlpha({55,12,2,255},ra*0.95f));
                    DrawCircleV({ex2,ey2},S(2.8f),ColorAlpha(lGlow,       ra*lPulse*0.95f));
                    DrawCircleV({ex2,ey2},S(1.4f),ColorAlpha(lHot,        ra*lPulse));
                }
                // mouth slot — solid rect
                DrawRectangle((int)(sp.x-S(5.f)),(int)(hY+S(10.f)),(int)S(10.f),(int)S(2.f),ColorAlpha(lCrack,ra*0.88f));
                DrawRectangle((int)(sp.x-S(4.f)),(int)(hY+S(10.f)),(int)S(8.f), (int)S(1.f), ColorAlpha(lGlow, ra*lPulse*0.92f));
                // lava drip from fists
                for(int side=-1;side<=1;side+=2){
                    float ax=sp.x+side*S(19.f);
                    float aOff=side==(-1)?lArmSway:rArmSway;
                    float dAge=fmodf(t*0.7f+ph+side*0.5f,1.f);
                    DrawCircleV({ax,spY+S(16.f)+breathe+aOff+dAge*S(12.f)},
                        S(2.2f*(1.f-dAge)),ColorAlpha(lGlow,ra*(1.f-dAge)*0.85f));
                }
            }
            else if(e.subType == 1){ // fire geyser — erupting column of flame
                float erupt = fmodf(t*0.7f+ph, 3.f);
                float eAlpha = erupt<0.5f? erupt/0.5f : erupt>2.5f? (3.f-erupt)/0.5f : 1.f;
                // ground vent
                DrawEllipse((int)sp.x,(int)(spY+S(18.f)),(int)S(12.f),(int)S(5.f),
                    ColorAlpha({120,30,5,255},ra*0.75f));
                DrawNeonCircle({sp.x,spY+S(18.f)},S(14.f),VP_ORANGE,ra*lPulse*0.4f);
                // erupting flame column
                if(erupt < 2.5f){
                    for(int i = 0; i < 8; i++){
                        float fi = (float)i/8.f;
                        float fy = spY + S(18.f) - erupt*S(8.f) - fi*S(15.f);
                        float fr = S(8.f - fi*6.f);
                        Color fc = i<3?ColorAlpha(VP_WHITE,ra*eAlpha*(1.f-fi)*0.7f):
                                   i<6?ColorAlpha(VP_YELLOW,ra*eAlpha*(1.f-fi)*0.65f):
                                       ColorAlpha(VP_ORANGE,ra*eAlpha*(1.f-fi)*0.6f);
                        DrawCircleV({sp.x+sinf(t*2.f+i+ph)*S(2.f),fy},fr,fc);
                        DrawNeonCircle({sp.x+sinf(t*2.f+i+ph)*S(2.f),fy},fr+S(1.f),VP_YELLOW,ra*eAlpha*(1.f-fi)*0.4f);
                    }
                }
                // heat shimmer
                for(int i = 0; i < 5; i++){
                    float ha = fmodf(t*1.2f+i*0.3f+ph,1.f);
                    DrawCircleV({sp.x+sinf(t+i)*S(8.f),spY+S(18.f)-ha*S(25.f)},S(1.f),
                        ColorAlpha(VP_ORANGE,ra*(1.f-ha)*0.3f));
                }
            }
            else if(e.subType == 2){ // magma pool — bubbling lava surface
                // main pool surface
                DrawEllipse((int)sp.x,(int)spY,(int)S(24.f),(int)S(14.f),
                    ColorAlpha({140,35,5,255},ra*0.82f));
                DrawEllipse((int)sp.x,(int)spY,(int)S(20.f),(int)S(11.f),
                    ColorAlpha({180,50,10,255},ra*0.65f));
                DrawNeonCircle({sp.x,spY},S(26.f),VP_ORANGE,ra*lPulse*0.35f);
                // bubbles rising and popping
                for(int i = 0; i < 6; i++){
                    float ba = fmodf(t*0.8f+i*0.4f+ph,2.2f);
                    if(ba < 1.8f){
                        float bx = sp.x + S(-15.f+i*6.f);
                        float by = spY - ba*S(4.f);
                        float br = S(2.f+ba*2.f);
                        DrawCircleLinesV({bx,by},br,ColorAlpha(VP_YELLOW,ra*(1.f-ba/1.8f)*0.6f));
                    }
                }
                // surface crust breaking
                for(int i = 0; i < 5; i++){
                    float ca = i*(2.f*PI/5.f)+t*0.3f+ph;
                    float cr = S(14.f+sinf(t*1.5f+i+ph)*4.f);
                    DrawLineEx({sp.x,spY},{sp.x+cosf(ca)*cr,spY+sinf(ca)*cr*0.7f},
                        S(1.5f),ColorAlpha({90,20,2,255},ra*0.55f));
                }
            }
            else if(e.subType == 5){ // FIRE SALAMANDER — stocky lizard with ember belly markings
                float walk = sinf(t * 2.2f + ph);
                float legAnim2 = sinf(t * 4.f + ph);
                // shadow
                DrawEllipse((int)sp.x, (int)(spY + S(18.f)), (int)S(16.f), (int)S(4.f),
                    ColorAlpha(VP_BLACK, ra*0.25f));
                // tail — curved behind
                for(int i = 0; i < 7; i++){
                    float fi = (float)i/7.f;
                    float tx2 = sp.x - S(10.f) - i*S(4.f);
                    float ty2 = spY + S(8.f) + sinf(t*1.5f + ph + fi*PI)*S(4.f)*fi;
                    DrawCircleV({tx2, ty2}, S(5.f - fi*3.f), ColorAlpha({50,15,3,255}, ra*0.88f));
                    // flame stripe
                    if(i % 2 == 0)
                        DrawCircleV({tx2, ty2 - S(1.f)}, S(2.f - fi), ColorAlpha(VP_ORANGE, ra*0.65f*lPulse));
                }
                // legs
                for(int side = -1; side <= 1; side += 2){
                    float loff = side * legAnim2 * S(2.5f);
                    DrawLineEx({sp.x + side*S(8.f), spY + S(6.f)},
                        {sp.x + side*S(14.f), spY + S(18.f) + loff}, S(3.5f), ColorAlpha({45,12,2,255}, ra*0.88f));
                    DrawCircleV({sp.x + side*S(14.f), spY + S(18.f) + loff}, S(3.f), ColorAlpha({55,16,3,255}, ra*0.85f));
                    // rear legs
                    DrawLineEx({sp.x - side*S(4.f), spY + S(10.f)},
                        {sp.x - side*S(12.f), spY + S(18.f) - loff}, S(3.5f), ColorAlpha({45,12,2,255}, ra*0.88f));
                    DrawCircleV({sp.x - side*S(12.f), spY + S(18.f) - loff}, S(3.f), ColorAlpha({55,16,3,255}, ra*0.85f));
                }
                // body
                DrawEllipse((int)sp.x, (int)(spY + S(4.f)), (int)S(14.f), (int)S(9.f),
                    ColorAlpha({48,14,2,255}, ra*0.92f));
                DrawEllipse((int)sp.x, (int)(spY + S(4.f)), (int)S(12.f), (int)S(8.f),
                    ColorAlpha({80,22,4,255}, ra*0.85f));
                // ember belly spots
                for(int i = 0; i < 5; i++){
                    float sx2 = sp.x - S(8.f) + i*S(4.f);
                    float sy2 = spY + S(6.f);
                    DrawCircleV({sx2, sy2}, S(2.2f), ColorAlpha({160,40,8,255}, ra*0.80f));
                    DrawCircleV({sx2, sy2}, S(1.3f), ColorAlpha(VP_ORANGE, ra*lPulse*0.85f));
                    DrawCircleV({sx2, sy2}, S(0.6f), ColorAlpha(VP_YELLOW, ra*lPulse));
                    DrawNeonCircle({sx2, sy2}, S(3.f), VP_ORANGE, ra*lPulse*0.4f);
                }
                // head
                DrawEllipse((int)(sp.x + S(12.f) + walk), (int)(spY - S(1.f)), (int)S(9.f), (int)S(7.f),
                    ColorAlpha({50,15,3,255}, ra*0.92f));
                // eyes
                for(int side = -1; side <= 1; side += 2){
                    float ex2 = sp.x + S(12.f) + walk + side*S(4.f);
                    DrawCircleV({ex2, spY - S(3.f)}, S(2.5f), ColorAlpha({10,5,1,255}, ra*0.95f));
                    DrawCircleV({ex2, spY - S(3.f)}, S(1.5f), ColorAlpha(VP_ORANGE, ra*0.9f));
                    DrawCircleV({ex2, spY - S(3.f)}, S(0.7f), ColorAlpha(VP_WHITE, ra));
                }
            }
            else if(e.subType == 6){ // MAGMA MANTA — flat ray gliding over lava, trailing fire
                float glide6=sinf(t*0.8f+ph)*S(3.f);
                float wingFlap6=sinf(t*1.8f+ph);
                float trailAge=fmodf(t*2.f+ph,1.f);
                // lava glow beneath
                DrawEllipse((int)sp.x,(int)(spY+S(14.f)),(int)S(28.f),(int)S(7.f),ColorAlpha(VP_RED,ra*0.10f));
                DrawNeonCircle({sp.x,spY+S(14.f)},S(28.f),VP_ORANGE,ra*0.08f);
                // left wing
                DrawTriangle({sp.x,spY+glide6},{sp.x-S(32.f)+wingFlap6*S(4.f),spY-S(6.f)+glide6},{sp.x-S(8.f),spY+S(12.f)+glide6},ColorAlpha({55,14,2,255},ra*0.85f));
                DrawTriangleLines({sp.x,spY+glide6},{sp.x-S(32.f)+wingFlap6*S(4.f),spY-S(6.f)+glide6},{sp.x-S(8.f),spY+S(12.f)+glide6},ColorAlpha(VP_ORANGE,ra*0.30f));
                // right wing
                DrawTriangle({sp.x,spY+glide6},{sp.x+S(32.f)-wingFlap6*S(4.f),spY-S(6.f)+glide6},{sp.x+S(8.f),spY+S(12.f)+glide6},ColorAlpha({55,14,2,255},ra*0.85f));
                DrawTriangleLines({sp.x,spY+glide6},{sp.x+S(32.f)-wingFlap6*S(4.f),spY-S(6.f)+glide6},{sp.x+S(8.f),spY+S(12.f)+glide6},ColorAlpha(VP_ORANGE,ra*0.30f));
                // wing vein lines
                for(int side=-1;side<=1;side+=2)
                    for(int v=1;v<=3;v++){
                        float vt=(float)v/4.f;
                        DrawLineEx({sp.x,spY+glide6},{sp.x+side*S(32.f)*vt,spY-S(6.f)*vt+glide6},S(0.7f),ColorAlpha(VP_RED,ra*0.25f));
                    }
                // central disc body
                DrawEllipse((int)sp.x,(int)(spY+glide6),(int)S(10.f),(int)S(7.f),ColorAlpha({45,12,2,255},ra*0.92f));
                DrawEllipse((int)sp.x,(int)(spY+glide6),(int)S(8.f),(int)S(5.5f),ColorAlpha({80,22,4,255},ra*0.85f));
                // lava core
                DrawCircleV({sp.x,spY+glide6},S(4.f),ColorAlpha({160,40,8,255},ra*0.88f));
                DrawCircleV({sp.x,spY+glide6},S(2.5f),ColorAlpha(VP_ORANGE,ra*lPulse*0.85f));
                DrawCircleV({sp.x,spY+glide6},S(1.2f),ColorAlpha(VP_YELLOW,ra*lPulse));
                DrawNeonCircle({sp.x,spY+glide6},S(6.f),VP_ORANGE,ra*lPulse*0.55f);
                // long tail with fire tip
                for(int i=0;i<6;i++){
                    float fi=(float)i/6.f;
                    float tx2=sp.x+S(10.f)+i*S(5.f);
                    float ty2=spY+glide6+sinf(t*1.8f+ph+fi*PI)*S(4.f)*fi;
                    DrawCircleV({tx2,ty2},S(3.5f-fi*2.f),ColorAlpha({50,14,2,255},ra*0.85f));
                    if(i==5){
                        DrawCircleV({tx2,ty2},S(3.f),ColorAlpha(VP_ORANGE,ra*lPulse*0.80f));
                        DrawNeonCircle({tx2,ty2},S(5.f),VP_RED,ra*lPulse*0.50f);
                    }
                }
                // fire trail particles
                for(int i=0;i<5;i++){
                    float ta=fmodf(trailAge+i*0.2f,1.f);
                    float tx2=sp.x+S(10.f+ta*20.f)+(i-2)*S(3.f);
                    float ty2=spY+glide6-ta*S(12.f);
                    DrawCircleV({tx2,ty2},S(2.f+ta*2.f),ColorAlpha(ta<0.4f?VP_YELLOW:VP_ORANGE,ra*(1.f-ta)*0.55f));
                }
            }
            else{ // ember sprite — tiny flying fire spirit
                float sBob = sinf(t*2.f+ph)*S(4.f);
                float sFlutter = sinf(t*8.f+ph);
                // sprite body (small, bright)
                DrawCircleV({sp.x,spY+sBob},S(5.f),ColorAlpha(VP_ORANGE,ra*0.85f));
                DrawCircleV({sp.x,spY+sBob},S(3.f),ColorAlpha(VP_YELLOW,ra*lPulse*0.75f));
                DrawCircleV({sp.x,spY+sBob},S(1.5f),ColorAlpha(VP_WHITE,ra*lPulse));
                DrawNeonCircle({sp.x,spY+sBob},S(6.f),VP_YELLOW,ra*lPulse*0.6f);
                // flame wings (flickering)
                DrawTriangle({sp.x,spY+sBob-S(3.f)},{sp.x-S(10.f)+sFlutter*S(2.f),spY+sBob-S(7.f)},
                    {sp.x-S(2.f),spY+sBob+S(2.f)},ColorAlpha(VP_ORANGE,ra*0.55f));
                DrawTriangle({sp.x,spY+sBob-S(3.f)},{sp.x+S(10.f)-sFlutter*S(2.f),spY+sBob-S(7.f)},
                    {sp.x+S(2.f),spY+sBob+S(2.f)},ColorAlpha(VP_ORANGE,ra*0.55f));
                DrawNeonLine({sp.x,spY+sBob-S(3.f)},{sp.x-S(10.f)+sFlutter*S(2.f),spY+sBob-S(7.f)},
                    VP_YELLOW,S(0.8f),ra*lPulse*0.4f);
                DrawNeonLine({sp.x,spY+sBob-S(3.f)},{sp.x+S(10.f)-sFlutter*S(2.f),spY+sBob-S(7.f)},
                    VP_YELLOW,S(0.8f),ra*lPulse*0.4f);
                // sparkle trail
                for(int i = 1; i <= 4; i++){
                    DrawCircleV({sp.x,spY+sBob+S(3.f*i)},S(1.f-i*0.2f),
                        ColorAlpha(VP_YELLOW,ra*(1.f-i/5.f)*0.6f));
                }
            }
            break;}

        case 3:{ // CRYSTAL CAVERNS — ice stalactite with living crystal sprites beneath it
            if(e.subType < 5){
            // hanging stalactite from above
            float cPulse = 0.6f + 0.4f * sinf(t*1.8f+ph);
            DrawTriangle({sp.x-S(10.f),spY-S(20.f)},{sp.x+S(10.f),spY-S(20.f)},
                {sp.x+sinf(t*0.4f+ph)*S(2.f),spY+S(5.f)},
                ColorAlpha(VP_DEEPCYAN,ra*0.75f));
            DrawTriangle({sp.x-S(7.f),spY-S(20.f)},{sp.x+S(7.f),spY-S(20.f)},
                {sp.x+sinf(t*0.4f+ph)*S(1.5f),spY+S(3.f)},
                ColorAlpha(VP_CYAN,ra*0.40f));
            DrawNeonCircle({sp.x+sinf(t*0.4f+ph)*S(2.f),spY+S(5.f)},S(2.f),VP_WHITE,ra*cPulse*0.7f);
            // water drip from tip
            float dAge = fmodf(t*0.9f+ph,2.f);
            DrawCircleV({sp.x+sinf(t*0.4f+ph)*S(2.f),spY+S(5.f)+dAge*S(8.f)},S(1.2f-dAge*0.3f),
                ColorAlpha(VP_DEEPCYAN,ra*(1.f-dAge/2.f)*0.7f));
            // 3 crystal sprites hovering beneath (tiny pixie-like creatures)
            for(int i = 0; i < 3; i++){
                float sx2 = sp.x + S(-10.f + i*10.f) + sinf(t*1.5f+i*1.2f+ph)*S(3.f);
                float sy2 = spY + S(10.f+i*4.f) + cosf(t*1.8f+i*0.9f+ph)*S(2.f);
                // sprite body
                DrawCircleV({sx2,sy2},S(3.5f),ColorAlpha(VP_DEEPCYAN,ra*0.80f));
                DrawCircleV({sx2,sy2},S(2.f),  ColorAlpha(VP_WHITE,   ra*0.45f));
                DrawNeonCircle({sx2,sy2},S(3.5f),VP_CYAN,ra*cPulse*0.5f);
                // wings
                DrawTriangle({sx2,sy2-S(2.f)},{sx2-S(7.f),sy2-S(5.f)},{sx2-S(1.f),sy2+S(1.f)},
                    ColorAlpha(VP_CYAN,ra*0.30f));
                DrawTriangle({sx2,sy2-S(2.f)},{sx2+S(7.f),sy2-S(5.f)},{sx2+S(1.f),sy2+S(1.f)},
                    ColorAlpha(VP_CYAN,ra*0.30f));
                // eyes
                DrawCircleV({sx2-S(1.2f),sy2-S(1.f)},S(0.8f),ColorAlpha(VP_BLACK,ra*0.9f));
                DrawCircleV({sx2+S(1.2f),sy2-S(1.f)},S(0.8f),ColorAlpha(VP_BLACK,ra*0.9f));
            }
            }
            else if(e.subType == 5){ // FROST YETI — big fluffy snow guardian
                float ysc = sc * 1.05f; auto Y = [&](float v){ return v * ysc; };
                float bob     = sinf(t*0.5f+ph)*Y(1.5f);
                float armSway = sinf(t*0.65f+ph)*Y(3.f);
                float shiver  = sinf(t*9.f+ph)*Y(0.5f);
                float glowP   = 0.5f+0.5f*sinf(t*1.4f+ph);
                float breathT = fmodf(t*0.8f+ph, 2.5f);
                Color fw={210,235,255,255}; Color fm={230,245,255,255};
                Color fl={248,252,255,255}; Color ic={155,210,255,255};
                Color ih={220,245,255,255};
                // legs
                for(int s=-1;s<=1;s+=2){
                    float lx=sp.x+s*Y(8.f);
                    DrawEllipse((int)lx,(int)(spY+Y(24.f)),(int)Y(7.f),(int)Y(10.f),ColorAlpha(fw,ra*0.90f));
                    DrawEllipse((int)(lx-Y(1.f)),(int)(spY+Y(20.f)),(int)Y(5.f),(int)Y(6.f),ColorAlpha(fm,ra*0.52f));
                    DrawEllipse((int)lx,(int)(spY+Y(32.f)),(int)Y(8.f),(int)Y(5.f),ColorAlpha(fw,ra*0.88f));
                    for(int i=-1;i<=1;i++){
                        float tx2=lx+i*Y(3.f);
                        DrawLineEx({tx2,spY+Y(35.f)},{tx2+(float)i*Y(0.8f),spY+Y(40.f)},Y(2.2f),ColorAlpha(ic,ra*0.80f));
                        DrawCircleV({tx2+(float)i*Y(0.8f),spY+Y(40.f)},Y(1.2f),ColorAlpha(ih,ra*0.92f));
                    }
                }
                // body
                DrawCircleV({sp.x,spY+Y(8.f)+bob},Y(19.f),ColorAlpha(fw,ra*0.93f));
                DrawCircleV({sp.x-Y(3.f),spY+Y(5.f)+bob},Y(14.f),ColorAlpha(fm,ra*0.62f));
                DrawEllipse((int)(sp.x+Y(3.f)),(int)(spY+Y(11.f)+bob),(int)Y(9.f),(int)Y(7.f),ColorAlpha(fl,ra*0.30f));
                for(int i=0;i<10;i++){
                    float fa=i*(2.f*PI/10.f)+ph*0.3f;
                    DrawCircleV({sp.x+cosf(fa)*Y(18.f),spY+Y(8.f)+bob+sinf(fa)*Y(18.f)},Y(4.f),ColorAlpha(fm,ra*0.65f));
                }
                // arms
                for(int s=-1;s<=1;s+=2){
                    float ax=sp.x+s*Y(20.f), ay=spY+Y(2.f)+bob+armSway*(float)s;
                    DrawEllipse((int)ax,(int)ay,(int)Y(6.5f),(int)Y(11.f),ColorAlpha(fw,ra*0.90f));
                    DrawEllipse((int)(ax-s*Y(1.f)),(int)(ay-Y(2.f)),(int)Y(4.5f),(int)Y(7.f),ColorAlpha(fm,ra*0.50f));
                    DrawCircleV({ax+(float)s*Y(1.f),ay+Y(12.f)},Y(6.f),ColorAlpha(fw,ra*0.88f));
                    for(int i=0;i<3;i++){
                        float cx=ax+(float)s*Y(1.f)+(float)(i-1)*Y(3.f);
                        DrawLineEx({cx,ay+Y(17.f)},{cx+(float)s*Y(1.f),ay+Y(23.f)},Y(2.5f),ColorAlpha(ic,ra*0.82f));
                        DrawCircleV({cx+(float)s*Y(1.f),ay+Y(23.f)},Y(1.1f),ColorAlpha(ih,ra*0.92f));
                    }
                }
                // head
                float hx=sp.x+shiver, hy=spY-Y(12.f)+bob;
                DrawCircleV({hx,hy},Y(15.f),ColorAlpha(fw,ra*0.94f));
                DrawCircleV({hx-Y(2.f),hy-Y(2.f)},Y(11.f),ColorAlpha(fm,ra*0.65f));
                DrawEllipse((int)(hx-Y(3.f)),(int)(hy-Y(5.f)),(int)Y(7.f),(int)Y(5.f),ColorAlpha(fl,ra*0.35f));
                for(int i=0;i<6;i++){
                    float fa=i*(2.f*PI/6.f)-PI*0.5f+ph*0.2f;
                    DrawCircleV({hx+cosf(fa)*Y(13.5f),hy+sinf(fa)*Y(13.5f)},Y(4.5f),ColorAlpha(fm,ra*0.62f));
                }
                // 3 short rounded icicles on top
                for(int i=-1;i<=1;i++){
                    float ix=hx+(float)i*Y(5.f);
                    float ilen=Y(8.f-fabsf((float)i)*1.5f);
                    DrawLineEx({ix,hy-Y(13.f)},{ix+(float)i*Y(0.8f),hy-Y(13.f)-ilen},Y(4.f-fabsf((float)i)*0.5f),ColorAlpha(ic,ra*0.82f));
                    DrawEllipse((int)(ix+(float)i*Y(0.8f)),(int)(hy-Y(13.f)-ilen),(int)Y(3.f),(int)Y(2.f),ColorAlpha(ih,ra*0.90f));
                    DrawNeonCircle({ix+(float)i*Y(0.8f),hy-Y(13.f)-ilen},Y(3.5f),{180,225,255,255},ra*glowP*0.28f);
                }
                // eyes
                for(int s=-1;s<=1;s+=2){
                    float ex2=hx+s*Y(5.f), ey2=hy-Y(3.5f);
                    DrawCircleV({ex2,ey2},Y(4.5f),ColorAlpha({30,70,120,255},ra*0.95f));
                    DrawCircleV({ex2,ey2},Y(3.5f),ColorAlpha({15,100,190,255},ra*0.90f));
                    DrawCircleV({ex2,ey2},Y(2.2f),ColorAlpha({55,165,240,255},ra*0.82f));
                    DrawCircleV({ex2,ey2},Y(1.f),ColorAlpha(VP_BLACK,ra));
                    DrawCircleV({ex2-Y(0.8f),ey2-Y(1.f)},Y(0.7f),ColorAlpha(VP_WHITE,ra*0.88f));
                    DrawNeonCircle({ex2,ey2},Y(4.f),{120,200,255,255},ra*0.32f);
                }
                // nose
                DrawCircleV({hx,hy+Y(2.f)},Y(2.f),ColorAlpha({80,130,185,255},ra*0.88f));
                DrawCircleV({hx,hy+Y(2.f)},Y(1.f),ColorAlpha({120,175,220,255},ra*0.68f));
                // rosy cheeks
                for(int s=-1;s<=1;s+=2)
                    DrawEllipse((int)(hx+s*Y(6.f)),(int)(hy+Y(5.f)),(int)Y(3.5f),(int)Y(2.f),ColorAlpha({175,205,240,255},ra*0.42f));
                // happy mouth arc (no teeth)
                for(int i=0;i<5;i++){
                    float ma=(float)i/4.f;
                    DrawLineEx(
                        {hx+(-Y(4.f)+ma*Y(8.f)),        hy+Y(7.5f)+sinf(ma*PI)*Y(2.f)},
                        {hx+(-Y(4.f)+(ma+0.25f)*Y(8.f)),hy+Y(7.5f)+sinf((ma+0.25f)*PI)*Y(2.f)},
                        Y(1.4f),ColorAlpha({60,110,165,255},ra*0.78f));
                }
                // frosty breath wisps
                if(breathT < 2.0f){
                    float bf=breathT/2.0f;
                    for(int i=0;i<5;i++){
                        float bi=(float)i/4.f;
                        float bp=fmodf(bf+bi*0.22f,1.f);
                        float bx=hx+sinf(bp*3.2f+i*1.f)*Y(3.f);
                        float by=hy+Y(9.f)-bp*Y(16.f);
                        DrawCircleV({bx,by},Y(1.4f+bp*2.2f),ColorAlpha({222,240,255,255},ra*(1.f-bp)*0.36f));
                    }
                }
                DrawNeonCircle({hx,hy},Y(16.f),{175,220,255,255},ra*0.15f);
                DrawNeonCircle({sp.x,spY+Y(8.f)+bob},Y(21.f),{155,205,255,255},ra*0.08f);
            }
            else if(e.subType == 6){ // ICE SPRITE — tiny winged fairy of living ice crystals
                float sparkle6=fmodf(t*2.5f+ph,1.f);
                float wingFlap6=sinf(t*8.f+ph);
                float drift6=sinf(t*1.1f+ph)*S(4.f);
                // fairy dust trail
                for(int i=0;i<8;i++){
                    float da=fmodf(t*1.5f+i*0.125f+ph,1.f);
                    float dx2=sp.x+drift6+(i-4)*S(3.f)+cosf(t*2.f+i)*S(2.f);
                    float dy2=spY+S(12.f)+da*S(20.f);
                    DrawCircleV({dx2,dy2},S(1.5f*(1.f-da)),ColorAlpha({180,230,255,255},ra*(1.f-da)*0.55f));
                }
                // wings — gossamer crystal
                for(int side=-1;side<=1;side+=2){
                    float wo=side*(S(10.f)+wingFlap6*side*S(3.f));
                    DrawEllipse((int)(sp.x+drift6+wo),(int)(spY-S(3.f)),(int)S(10.f),(int)S(7.f),ColorAlpha({200,235,255,255},ra*0.18f));
                    DrawEllipseLines((int)(sp.x+drift6+wo),(int)(spY-S(3.f)),(int)S(10.f),(int)S(7.f),ColorAlpha({160,215,255,255},ra*0.55f));
                    // wing veins
                    for(int v=1;v<=2;v++){
                        float vt=(float)v/3.f;
                        DrawLineEx({sp.x+drift6,spY-S(3.f)},{sp.x+drift6+wo*vt,spY-S(3.f)+sinf(vt*PI)*S(5.f)},S(0.6f),ColorAlpha({180,220,255,255},ra*0.35f));
                    }
                    // crystal shard on wingtip
                    DrawRectanglePro({sp.x+drift6+wo,spY-S(8.f),S(3.f),S(6.f)},{S(1.5f),S(3.f)},side*25.f,ColorAlpha({220,240,255,255},ra*0.75f));
                    DrawNeonCircle({sp.x+drift6+wo,spY-S(8.f)},S(3.f),{180,225,255,255},ra*sparkle6*0.40f);
                }
                // body — slender ice figure
                DrawEllipse((int)(sp.x+drift6),(int)spY,(int)S(4.f),(int)S(8.f),ColorAlpha({170,215,255,255},ra*0.88f));
                DrawEllipse((int)(sp.x+drift6),(int)spY,(int)S(3.f),(int)S(6.f),ColorAlpha({210,238,255,255},ra*0.80f));
                // ice dress flares
                for(int i=0;i<5;i++){
                    float fa=ph+t*0.3f+(i-2)*0.35f;
                    DrawLineEx({sp.x+drift6,spY+S(7.f)},{sp.x+drift6+cosf(fa)*S(6.f),spY+S(7.f)+sinf(fa)*S(4.f)},S(1.2f),ColorAlpha({190,225,255,255},ra*0.50f));
                    DrawCircleV({sp.x+drift6+cosf(fa)*S(6.f),spY+S(7.f)+sinf(fa)*S(4.f)},S(1.f),ColorAlpha({230,245,255,255},ra*sparkle6*0.65f));
                }
                // head
                DrawCircleV({sp.x+drift6,spY-S(10.f)},S(5.5f),ColorAlpha({175,218,255,255},ra*0.90f));
                DrawCircleV({sp.x+drift6,spY-S(10.f)},S(4.f),ColorAlpha({215,240,255,255},ra*0.82f));
                // ice crown
                for(int i=0;i<5;i++){
                    float ca=(i-2)*0.28f-PI*.5f;
                    DrawRectanglePro({sp.x+drift6+(i-2)*S(2.5f),spY-S(15.f),S(2.f),S(4.f+i%2*2.f)},{S(1.f),S(4.f+i%2*2.f)},0.f,ColorAlpha({210,238,255,255},ra*0.80f));
                }
                // eyes
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+drift6+side*S(2.2f),spY-S(11.f)},S(1.5f),ColorAlpha({8,30,60,255},ra*0.95f));
                    DrawCircleV({sp.x+drift6+side*S(2.2f),spY-S(11.f)},S(0.8f),ColorAlpha({0,190,255,255},ra*0.90f));
                    DrawNeonCircle({sp.x+drift6+side*S(2.2f),spY-S(11.f)},S(2.5f),{180,225,255,255},ra*sparkle6*0.55f);
                }
                // sparkle bursts
                for(int i=0;i<4;i++){
                    float sa=fmodf(t*2.5f+i*0.25f+ph,1.f);
                    float sx2=sp.x+drift6+cosf(ph+i*(PI/2.f))*S(14.f);
                    float sy2=spY+sinf(ph+i*(PI/2.f))*S(10.f);
                    if(sa<0.3f){
                        DrawCircleV({sx2,sy2},S(1.5f),ColorAlpha(VP_WHITE,ra*(1.f-sa/0.3f)*0.80f));
                        DrawNeonCircle({sx2,sy2},S(4.f),{200,230,255,255},ra*(1.f-sa/0.3f)*0.45f);
                    }
                }
            }
            break;}

        case 4:{ // RESONANCE CORE — tuning fork being struck by a plasma hammer: harmonic rings
            float rPulse = 0.5f + 0.5f * sinf(t*2.4f+ph);
            if(e.subType < 5){
            // tuning fork — scaled down, same slow 2D drift as the whale
            float fsc = sc * 0.55f; // smaller than default
            auto  F   = [&](float v){ return v * fsc; };
            // whale-style drift: two summed sines, very slow
            float driftX = cosf(t*0.12f+ph)*F(8.f) + cosf(t*0.07f+ph*1.3f)*F(4.f);
            float driftY = sinf(t*0.09f+ph)*F(5.f) + sinf(t*0.13f+ph*0.7f)*F(2.5f);
            float fX = sp.x + driftX;
            float fY = spY  + driftY;
            float vib = sinf(t*6.f+ph)*F(1.2f);
            Color fC1={180,150,20,255};
            Color fC2={240,200,40,255};
            Color fC3={255,240,140,255};
            // resonance rings
            for(int i=0;i<3;i++){
                float rAge=fmodf(t*0.55f+i*0.35f+ph,1.f);
                DrawEllipse((int)fX,(int)fY,(int)(F(12.f+rAge*22.f)),(int)(F(6.f+rAge*11.f)),
                    ColorAlpha(fC2,ra*(1.f-rAge)*0.12f));
                DrawEllipseLines((int)fX,(int)fY,(int)(F(12.f+rAge*22.f)),(int)(F(6.f+rAge*11.f)),
                    ColorAlpha(fC2,ra*(1.f-rAge)*0.35f));
            }
            DrawRectangle((int)(fX-F(3.f)),(int)(fY+F(4.f)),(int)F(6.f),(int)F(16.f),ColorAlpha(fC1,ra*0.90f));
            DrawRectangle((int)(fX-F(2.f)),(int)(fY+F(5.f)),(int)F(4.f),(int)F(14.f),ColorAlpha(fC2,ra*0.80f));
            DrawRectangle((int)(fX-F(11.f)),(int)(fY+F(2.f)),(int)F(22.f),(int)F(5.f),ColorAlpha(fC1,ra*0.90f));
            DrawRectangle((int)(fX-F(10.f)),(int)(fY+F(3.f)),(int)F(20.f),(int)F(3.f),ColorAlpha(fC2,ra*0.80f));
            DrawRectangle((int)(fX-F(11.f)),(int)(fY-F(16.f)),(int)F(5.f),(int)F(19.f),ColorAlpha(fC1,ra*0.90f));
            DrawRectangle((int)(fX-F(10.f)),(int)(fY-F(15.f)),(int)F(4.f),(int)F(18.f),ColorAlpha(fC2,ra*0.82f));
            DrawRectangle((int)(fX+F(6.f)),(int)(fY-F(16.f)),(int)F(5.f),(int)F(19.f),ColorAlpha(fC1,ra*0.90f));
            DrawRectangle((int)(fX+F(7.f)),(int)(fY-F(15.f)),(int)F(4.f),(int)F(18.f),ColorAlpha(fC2,ra*0.82f));
            DrawCircleV({fX-F(8.f),fY-F(16.f)+vib},F(4.f),  ColorAlpha(fC1,ra*0.92f));
            DrawCircleV({fX-F(8.f),fY-F(16.f)+vib},F(2.8f), ColorAlpha(fC3,ra*rPulse*0.90f));
            DrawCircleV({fX+F(9.f),fY-F(16.f)-vib},F(4.f),  ColorAlpha(fC1,ra*0.92f));
            DrawCircleV({fX+F(9.f),fY-F(16.f)-vib},F(2.8f), ColorAlpha(fC3,ra*rPulse*0.90f));
            DrawLineEx({fX-F(2.f),fY+F(12.f)},{fX+F(2.f),fY+F(12.f)},F(1.f),ColorAlpha(fC1,ra*0.60f));
            DrawLineEx({fX-F(2.f),fY+F(15.f)},{fX+F(2.f),fY+F(15.f)},F(1.f),ColorAlpha(fC1,ra*0.60f));
            }
            else if(e.subType == 5){ // GOLDEN SCARAB — gem-encrusted beetle drifting in orbit
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float wingBeat = sinf(t * 3.5f + ph);
                float rot = t * 0.4f + ph;
                // elytra (wing cases) — spread open
                for(int side = -1; side <= 1; side += 2){
                    float wo = side * (S(8.f) + wingBeat*side*S(2.f));
                    DrawEllipse((int)(sp.x + wo), (int)(spY - S(2.f)),
                        (int)S(9.f), (int)S(14.f), ColorAlpha({185,140,8,255}, ra*0.88f));
                    DrawEllipse((int)(sp.x + wo), (int)(spY - S(2.f)),
                        (int)S(7.f), (int)S(12.f), ColorAlpha({230,190,30,255}, ra*0.80f));
                    // wing gem inlay
                    for(int gi = 0; gi < 3; gi++){
                        DrawCircleV({sp.x + wo + side*S(1.f), spY - S(6.f) + gi*S(5.f)},
                            S(1.8f), ColorAlpha({255,240,80,255}, ra*0.85f));
                        DrawNeonCircle({sp.x + wo + side*S(1.f), spY - S(6.f) + gi*S(5.f)},
                            S(2.5f), VP_GOLD, ra*0.5f*rPulse);
                    }
                }
                // flying wings — translucent
                for(int side = -1; side <= 1; side += 2){
                    float wext = side * (S(16.f) + wingBeat*side*S(4.f));
                    DrawEllipse((int)(sp.x + wext), (int)(spY - S(4.f)),
                        (int)S(10.f), (int)S(8.f), ColorAlpha({255,245,180,255}, ra*0.22f));
                    DrawEllipseLines((int)(sp.x + wext), (int)(spY - S(4.f)),
                        (int)S(10.f), (int)S(8.f), ColorAlpha({255,220,80,255}, ra*0.40f));
                }
                // body — armoured thorax + abdomen
                DrawEllipse((int)sp.x, (int)(spY + S(4.f)), (int)S(8.f), (int)S(12.f),
                    ColorAlpha({160,110,6,255}, ra*0.92f));
                DrawEllipse((int)sp.x, (int)(spY + S(4.f)), (int)S(6.f), (int)S(10.f),
                    ColorAlpha({220,175,20,255}, ra*0.85f));
                // abdomen segments
                for(int i = 0; i < 4; i++)
                    DrawLineEx({sp.x - S(6.f), spY + S(1.f) + i*S(3.f)},
                        {sp.x + S(6.f), spY + S(1.f) + i*S(3.f)},
                        S(0.7f), ColorAlpha({140,100,5,255}, ra*0.55f));
                // thorax
                DrawEllipse((int)sp.x, (int)(spY - S(7.f)), (int)S(7.f), (int)S(6.f),
                    ColorAlpha({175,125,8,255}, ra*0.90f));
                // head + horns
                DrawCircleV({sp.x, spY - S(14.f)}, S(6.f), ColorAlpha({200,155,12,255}, ra*0.90f));
                DrawLineEx({sp.x, spY - S(18.f)}, {sp.x, spY - S(26.f)}, S(2.5f),
                    ColorAlpha({220,180,20,255}, ra*0.85f));
                DrawCircleV({sp.x, spY - S(26.f)}, S(2.5f), ColorAlpha({255,220,40,255}, ra*0.90f));
                DrawNeonCircle({sp.x, spY - S(26.f)}, S(4.f), VP_GOLD, ra*rPulse*0.65f);
                // legs
                for(int i = 0; i < 3; i++){
                    for(int side = -1; side <= 1; side += 2){
                        float lx = sp.x + side*S(7.f);
                        float ly = spY - S(4.f) + i*S(4.f);
                        DrawLineEx({lx, ly}, {lx + side*S(8.f), ly + S(5.f)},
                            S(1.5f), ColorAlpha({160,110,6,255}, ra*0.82f));
                    }
                }
                DrawNeonCircle({sp.x, spY}, S(12.f), VP_GOLD, ra*0.15f*pulse);
            }
            else if(e.subType == 6){ // DESERT DJINN — swirling spirit of sand and amber light
                float djinnSpin=t*1.2f+ph;
                float sandAge=fmodf(t*2.f+ph,1.f);
                float formFade=0.5f+0.5f*sinf(t*0.8f+ph);
                // sand tornado body — expanding spiral rings
                for(int i=0;i<5;i++){
                    float ta=fmodf(sandAge+i*0.2f,1.f);
                    float tr=S(6.f+ta*24.f);
                    float tw=(1.f-ta)*S(3.f)+S(0.5f);
                    DrawEllipse((int)sp.x,(int)(spY+S(8.f)-ta*S(20.f)),(int)tr,(int)(tr*0.4f),ColorAlpha({200,150,40,255},ra*(1.f-ta)*0.18f));
                    DrawEllipseLines((int)sp.x,(int)(spY+S(8.f)-ta*S(20.f)),(int)tr,(int)(tr*0.4f),ColorAlpha({220,180,60,255},ra*(1.f-ta)*0.30f));
                }
                // djinn lower half — tornado funnel
                for(int i=0;i<8;i++){
                    float fi=(float)i/8.f;
                    float sa=djinnSpin+i*(2*PI/8.f);
                    float sr=S(10.f+fi*6.f);
                    float sy2=spY+S(8.f)+fi*S(16.f);
                    DrawCircleV({sp.x+cosf(sa)*sr,sy2},S(2.5f-fi),ColorAlpha({185,135,28,255},ra*(1.f-fi)*0.55f));
                }
                // upper body — humanoid form condensing from sand
                DrawEllipse((int)sp.x,(int)(spY+S(2.f)),(int)(S(10.f)*formFade),(int)(S(14.f)*formFade),ColorAlpha({210,165,45,255},ra*formFade*0.70f));
                // arms — swirling arc
                for(int side=-1;side<=1;side+=2){
                    for(int a=0;a<5;a++){
                        float aa=side*(0.5f+a*0.15f)+djinnSpin*0.2f;
                        float ar=S(12.f+a*2.f);
                        DrawCircleV({sp.x+cosf(aa)*ar,spY-S(2.f)+sinf(aa)*ar*0.5f},S(2.5f-a*0.3f),ColorAlpha({200,155,35,255},ra*formFade*(1.f-a*0.15f)*0.65f));
                    }
                }
                // head — condensed amber orb
                DrawCircleV({sp.x,spY-S(14.f)*formFade},S(8.f)*formFade,ColorAlpha({190,140,30,255},ra*formFade*0.85f));
                DrawCircleV({sp.x,spY-S(14.f)*formFade},S(6.f)*formFade,ColorAlpha({240,200,70,255},ra*formFade*0.78f));
                DrawNeonCircle({sp.x,spY-S(14.f)*formFade},S(10.f)*formFade,VP_GOLD,ra*formFade*0.55f);
                // eyes — glowing amber
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(3.f)*formFade,spY-S(15.f)*formFade},S(2.f)*formFade,ColorAlpha({5,3,0,255},ra*0.95f));
                    DrawCircleV({sp.x+side*S(3.f)*formFade,spY-S(15.f)*formFade},S(1.2f)*formFade,ColorAlpha({255,210,50,255},ra*formFade*0.90f));
                    DrawNeonCircle({sp.x+side*S(3.f)*formFade,spY-S(15.f)*formFade},S(3.f)*formFade,VP_GOLD,ra*formFade*0.60f);
                }
                // ambient sand motes
                for(int i=0;i<8;i++){
                    float ma=djinnSpin+i*(2*PI/8.f);
                    float mr=S(20.f+sinf(t+i)*5.f);
                    DrawCircleV({sp.x+cosf(ma)*mr,spY+sinf(ma)*mr*0.5f},S(1.2f),ColorAlpha({200,160,40,255},ra*0.40f));
                }
            }
            break;}

        case 5:{ // SPORE WASTES — mycelium brain: a floating fungal neural mass
            float mPulse = 0.55f + 0.45f * sinf(t*1.6f+ph);
            if(e.subType < 5){
            // outer membrane (irregular blob via overlapping circles)
            for(int i = 0; i < 6; i++){
                float ma = i*(2.f*PI/6.f) + t*0.08f + ph;
                float mr = S(10.f + sinf(t*0.9f+i+ph)*3.f);
                DrawCircleV({sp.x+cosf(ma)*S(6.f), spY+sinf(ma)*S(4.5f)}, mr,
                    ColorAlpha({160,50,200,255},ra*0.18f));
            }
            DrawEllipse((int)sp.x,(int)spY,(int)S(16.f),(int)S(12.f),
                ColorAlpha({140,40,180,255},ra*0.75f));
            // brain folds (curved lines on surface)
            for(int i = 0; i < 6; i++){
                float fa = i*(PI/6.f) + 0.2f + ph*0.3f;
                float fr = S(10.f + i*0.8f);
                DrawNeonLine({sp.x+cosf(fa)*fr*0.5f,spY+sinf(fa)*S(8.f)*0.5f},
                             {sp.x+cosf(fa+0.5f)*fr,spY+sinf(fa+0.5f)*S(8.f)},
                             VP_PURPLE, S(0.7f), ra*0.35f);
            }
            // spore vents (pulsing)
            for(int i = 0; i < 4; i++){
                float va = i*(PI/2.f) + ph;
                float vx = sp.x+cosf(va)*S(8.f), vy = spY+sinf(va)*S(6.f);
                float vAge = fmodf(t*0.7f+i*0.4f+ph,2.f);
                DrawNeonCircle({vx,vy},S(2.f),VP_LAVENDER,ra*mPulse*0.55f);
                // spore puff ascending
                if(vAge < 1.6f){
                    float vA = vAge<0.3f?vAge/0.3f:vAge>1.2f?(1.6f-vAge)/0.4f:0.8f;
                    DrawNeonCircle({vx,vy-vAge*S(6.f)},S(1.5f-vAge*0.6f),VP_PURPLE,ra*vA*0.45f);
                }
            }
            // glowing nucleus
            DrawCircleV({sp.x,spY},S(5.f),ColorAlpha({200,80,240,255},ra*mPulse*0.8f));
            DrawCircleV({sp.x,spY},S(2.5f),ColorAlpha(VP_WHITE,ra*mPulse*0.55f));
            DrawNeonCircle({sp.x,spY},S(5.f),VP_PURPLE,ra*mPulse*0.6f);
            }
            else if(e.subType == 5){ // BLOOM POD — giant spore pod venting spore clouds
                float ventAge = fmodf(t * 0.8f + ph, 1.f);
                float throb = 0.8f + 0.2f*sinf(t * 1.5f + ph);
                // drifting spore clouds
                for(int i = 0; i < 6; i++){
                    float sa = fmodf(t * 0.5f + i * (2*PI/6.f) + ph, 2*PI);
                    float sd = S(15.f + i * 3.f);
                    float sA2 = 0.5f + 0.5f*sinf(t * 0.8f + i + ph);
                    for(int j = 0; j < 4; j++){
                        float va = fmodf(t * 0.9f + i*0.7f + j*0.4f + ph, 1.f);
                        float vr = S(4.f + va*10.f);
                        DrawCircleV({sp.x + cosf(sa)*sd, spY + sinf(sa)*sd*0.7f},
                            vr, ColorAlpha({120,160,20,255}, ra*(1.f-va)*0.12f));
                    }
                    DrawCircleV({sp.x + cosf(sa)*sd, spY + sinf(sa)*sd*0.7f},
                        S(3.5f), ColorAlpha({160,210,30,255}, ra*sA2*0.55f));
                }
                // outer pod skin — lumpy
                for(int i = 0; i < 6; i++){
                    float ba = i*(2*PI/6.f) + ph;
                    DrawCircleV({sp.x + cosf(ba)*S(13.f)*throb, spY + sinf(ba)*S(10.f)*throb},
                        S(7.f), ColorAlpha({55,90,8,255}, ra*0.80f));
                }
                // pod body
                DrawEllipse((int)sp.x, (int)spY, (int)(S(14.f)*throb), (int)(S(11.f)*throb),
                    ColorAlpha({45,75,7,255}, ra*0.92f));
                DrawEllipse((int)sp.x, (int)spY, (int)(S(11.f)*throb), (int)(S(8.f)*throb),
                    ColorAlpha({70,115,10,255}, ra*0.82f));
                // vent pores — glowing vents on surface
                for(int i = 0; i < 4; i++){
                    float pa = PI*0.25f + i*(PI/3.5f);
                    float px2 = sp.x + cosf(pa)*S(9.f)*throb;
                    float py2 = spY + sinf(pa)*S(7.f)*throb;
                    float ve = fmodf(ventAge + i*0.25f, 1.f);
                    DrawCircleV({px2, py2}, S(2.5f), ColorAlpha({80,140,10,255}, ra*0.85f));
                    DrawCircleV({px2, py2}, S(1.5f), ColorAlpha({180,255,40,255}, ra*(1.f-ve)*0.80f));
                    DrawNeonCircle({px2, py2}, S(4.f), {140,220,20,255}, ra*(1.f-ve)*0.50f);
                }
                // spore jet plumes from top vents
                for(int j = 0; j < 5; j++){
                    float je = fmodf(t*1.1f + j*0.2f + ph, 1.f);
                    DrawCircleV({sp.x + (j-2)*S(3.f), spY - S(10.f) - je*S(18.f)},
                        S(2.f + je*2.f), ColorAlpha({160,215,25,255}, ra*(1.f-je)*0.45f));
                }
                // glowing nucleus
                DrawCircleV({sp.x, spY}, S(4.f), ColorAlpha({100,170,15,255}, ra*0.90f));
                DrawCircleV({sp.x, spY}, S(2.5f), ColorAlpha({200,255,50,255}, ra*mPulse));
                DrawNeonCircle({sp.x, spY}, S(5.f), {150,230,20,255}, ra*mPulse*0.55f);
            }
            else if(e.subType == 6){ // SPORE SPRITE — tiny sentient mushroom creature floating on spore jets
                float jetAge=fmodf(t*2.2f+ph,1.f);
                float capBob=sinf(t*0.35f+ph)*S(2.f); // gentle drift, not a bounce
                float glow6=0.6f+0.4f*sinf(t*2.f+ph);
                // spore jet propulsion — plumes downward
                for(int i=0;i<4;i++){
                    float ja=fmodf(jetAge+i*0.25f,1.f);
                    float jx=sp.x+(i-1.5f)*S(3.f);
                    float jy=spY+S(12.f)+ja*S(18.f);
                    DrawCircleV({jx,jy},S(2.f+ja*3.f),ColorAlpha({140,200,25,255},ra*(1.f-ja)*0.50f));
                    DrawNeonCircle({jx,jy},S(4.f),{110,175,18,255},ra*(1.f-ja)*0.30f);
                }
                // legs — two stubby stalks
                for(int side=-1;side<=1;side+=2){
                    DrawLineEx({sp.x+side*S(3.f),spY+S(10.f)+capBob},{sp.x+side*S(5.f),spY+S(16.f)+capBob},S(3.f),ColorAlpha({45,80,8,255},ra*0.88f));
                    DrawCircleV({sp.x+side*S(5.f),spY+S(16.f)+capBob},S(3.f),ColorAlpha({55,95,10,255},ra*0.82f));
                    // foot spots
                    DrawCircleV({sp.x+side*S(5.f),spY+S(16.f)+capBob},S(1.5f),ColorAlpha({160,220,25,255},ra*glow6*0.65f));
                }
                // stem body
                DrawEllipse((int)sp.x,(int)(spY+S(5.f)+capBob),(int)S(5.f),(int)S(7.f),ColorAlpha({42,76,7,255},ra*0.90f));
                DrawEllipse((int)sp.x,(int)(spY+S(5.f)+capBob),(int)S(4.f),(int)S(5.5f),ColorAlpha({68,118,11,255},ra*0.82f));
                // belly spots
                for(int i=0;i<3;i++){
                    float sx2=sp.x+(i-1)*S(3.f);
                    DrawCircleV({sx2,spY+S(6.f)+capBob},S(1.5f),ColorAlpha({160,220,25,255},ra*glow6*0.70f));
                    DrawNeonCircle({sx2,spY+S(6.f)+capBob},S(2.5f),{130,190,20,255},ra*glow6*0.35f);
                }
                // mushroom cap
                DrawEllipse((int)sp.x,(int)(spY-S(4.f)+capBob),(int)S(14.f),(int)S(8.f),ColorAlpha({38,68,6,255},ra*0.92f));
                DrawEllipse((int)sp.x,(int)(spY-S(4.f)+capBob),(int)S(12.f),(int)S(6.5f),ColorAlpha({62,108,10,255},ra*0.85f));
                // cap spots — bioluminescent
                for(int i=0;i<5;i++){
                    float sa=ph+i*(2*PI/5.f);
                    float sr=S(7.f);
                    float sx2=sp.x+cosf(sa)*sr;
                    float sy2=spY-S(4.f)+capBob+sinf(sa)*sr*0.5f;
                    DrawCircleV({sx2,sy2},S(2.f),ColorAlpha({170,235,28,255},ra*glow6*0.75f));
                    DrawNeonCircle({sx2,sy2},S(3.5f),{140,205,22,255},ra*glow6*0.40f);
                }
                // face on stem
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(2.f),spY+S(4.f)+capBob},S(1.8f),ColorAlpha({5,10,1,255},ra*0.95f));
                    DrawCircleV({sp.x+side*S(2.f),spY+S(4.f)+capBob},S(1.f),ColorAlpha({175,240,30,255},ra*glow6*0.90f));
                }
                // grin — arc of dots
                for(int i=0;i<5;i++){
                    float ga=(i-2)*0.2f+PI*.5f;
                    float gr=S(2.8f);
                    DrawCircleV({sp.x+cosf(ga)*gr,spY+S(7.f)+capBob+sinf(ga)*gr*0.5f},S(0.7f),ColorAlpha({175,240,30,255},ra*0.65f));
                }
                // floating spore puffs nearby
                for(int i=0;i<4;i++){
                    float pa=t*0.5f+ph+i*(PI/2.f);
                    float pr=S(18.f+sinf(t*0.7f+i)*4.f);
                    DrawCircleV({sp.x+cosf(pa)*pr,spY+sinf(pa)*pr*0.6f},S(2.f),ColorAlpha({120,185,18,255},ra*0.30f));
                }
            }
            break;}

        case 6:{ // STELLAR FORGE — forge golem: a titan built of molten metal with hammer-hands
            float fPulse= 0.6f+0.4f*sinf(t*3.5f+ph);
            if(e.subType < 5){
            float gBob  = sinf(t*0.5f+ph)*S(1.5f);
            float armL  = sinf(t*1.4f+ph)*0.4f;
            float armR  = -armL;
            // lava floor haze
            DrawEllipse((int)sp.x,(int)(spY+S(20.f)),(int)S(25.f),(int)S(5.f),
                ColorAlpha(VP_ORANGE,ra*0.12f));
            // legs (two pillars)
            DrawRectangle((int)(sp.x-S(9.f)),(int)(spY+S(8.f)+gBob),(int)S(6.f),(int)S(12.f),
                ColorAlpha({60,30,5,255},ra*0.88f));
            DrawRectangle((int)(sp.x+S(3.f)),(int)(spY+S(8.f)+gBob),(int)S(6.f),(int)S(12.f),
                ColorAlpha({60,30,5,255},ra*0.88f));
            // torso
            DrawRectangle((int)(sp.x-S(12.f)),(int)(spY-S(4.f)+gBob),(int)S(24.f),(int)S(13.f),
                ColorAlpha({40,20,4,255},ra*0.92f));
            // lava chest cracks
            DrawNeonLine({sp.x-S(4.f),spY+gBob},{sp.x+S(2.f),spY+S(8.f)+gBob},VP_ORANGE,S(1.5f),ra*fPulse*0.7f);
            DrawNeonLine({sp.x+S(4.f),spY+gBob},{sp.x-S(1.f),spY+S(9.f)+gBob},VP_YELLOW,S(1.f),ra*fPulse*0.55f);
            // left arm (swinging)
            float lax = sp.x-S(12.f)+cosf(armL-PI/2.f)*S(12.f);
            float lay = spY+gBob+sinf(armL-PI/2.f)*S(12.f);
            DrawLineEx({sp.x-S(12.f),spY+S(2.f)+gBob},{lax,lay},S(4.f),ColorAlpha({50,25,4,255},ra*0.85f));
            DrawCircleV({lax,lay},S(5.f),ColorAlpha(VP_ORANGE,ra*0.8f)); // hammer head
            DrawNeonCircle({lax,lay},S(5.f),VP_YELLOW,ra*fPulse*0.55f);
            // right arm
            float rax = sp.x+S(12.f)+cosf(armR+PI/2.f)*S(10.f);
            float ray = spY+gBob+sinf(armR+PI/2.f)*S(10.f);
            DrawLineEx({sp.x+S(12.f),spY+S(2.f)+gBob},{rax,ray},S(4.f),ColorAlpha({50,25,4,255},ra*0.85f));
            DrawCircleV({rax,ray},S(4.5f),ColorAlpha(VP_ORANGE,ra*0.75f));
            // head (angular, helmet-like)
            DrawRectangle((int)(sp.x-S(8.f)),(int)(spY-S(14.f)+gBob),(int)S(16.f),(int)S(10.f),
                ColorAlpha({35,18,3,255},ra*0.92f));
            // visor (glowing red slit)
            DrawRectangle((int)(sp.x-S(6.f)),(int)(spY-S(11.f)+gBob),(int)S(12.f),(int)S(3.f),
                ColorAlpha({200,50,5,255},ra*0.35f));
            DrawNeonLine({sp.x-S(6.f),spY-S(9.5f)+gBob},{sp.x+S(6.f),spY-S(9.5f)+gBob},
                VP_ORANGE,S(1.f),ra*fPulse*0.7f);
            }
            else if(e.subType == 5){ // CLOCKWORK DRAGON — mechanical dragon of spinning gears + steam
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float wingFan = 0.6f + 0.4f*sinf(t*1.4f + ph);
                float steamAge = fmodf(t*1.8f + ph, 1.f);
                float gearRot = t * 2.5f + ph;
                // tail — segmented gear-rings
                for(int i = 0; i < 6; i++){
                    float fi = (float)i/6.f;
                    float tx2 = sp.x - S(10.f) - i*S(5.f);
                    float ty2 = spY + S(4.f) + sinf(t*1.2f + ph + i*0.5f)*S(3.f)*fi;
                    DrawCircleV({tx2, ty2}, S(5.f - fi*2.f), ColorAlpha({25,25,32,255}, ra*0.90f));
                    DrawCircleLinesV({tx2, ty2}, S(5.f - fi*2.f), ColorAlpha({90,140,180,255}, ra*0.55f));
                    // gear teeth
                    for(int gt = 0; gt < 4; gt++){
                        float ga = gearRot * (1.f + fi) + gt*(PI/2.f);
                        DrawCircleV({tx2 + cosf(ga)*S(5.5f-fi*2.f), ty2 + sinf(ga)*S(5.5f-fi*2.f)},
                            S(1.f), ColorAlpha({80,130,170,255}, ra*0.65f));
                    }
                }
                // wings
                for(int side = -1; side <= 1; side += 2){
                    float wBase = sp.x + side*S(8.f);
                    for(int wi = 0; wi < 4; wi++){
                        float wa = side*(0.3f + wi*0.2f)*wingFan;
                        float wl = S(12.f + wi*3.f);
                        float wtx = wBase + cosf(wa + side*PI*.4f)*wl;
                        float wty = spY - S(4.f) + sinf(wa + side*PI*.4f)*wl;
                        DrawLineEx({wBase, spY - S(4.f)}, {wtx, wty},
                            S(2.f - wi*0.3f), ColorAlpha({30,30,40,255}, ra*0.88f));
                        DrawLineEx({wBase, spY - S(4.f)}, {wtx, wty},
                            S(1.f), ColorAlpha({80,130,170,255}, ra*0.45f));
                    }
                }
                // body — plated chassis
                DrawEllipse((int)sp.x, (int)spY, (int)S(14.f), (int)S(9.f), ColorAlpha({20,20,28,255}, ra*0.92f));
                DrawEllipse((int)sp.x, (int)spY, (int)S(12.f), (int)S(7.f), ColorAlpha({35,35,50,255}, ra*0.85f));
                // chest gear
                DrawCircleLinesV({sp.x, spY}, S(5.f), ColorAlpha({90,140,180,255}, ra*0.65f));
                for(int gt = 0; gt < 8; gt++){
                    float ga = gearRot + gt*(PI/4.f);
                    DrawCircleV({sp.x + cosf(ga)*S(5.f), spY + sinf(ga)*S(5.f)},
                        S(1.2f), ColorAlpha({80,130,170,255}, ra*0.70f));
                }
                DrawCircleV({sp.x, spY}, S(3.f), ColorAlpha({255,160,20,255}, ra*0.85f));
                DrawCircleV({sp.x, spY}, S(1.8f), ColorAlpha(VP_YELLOW, ra*fPulse));
                // head
                DrawEllipse((int)(sp.x + S(14.f)), (int)(spY - S(3.f)), (int)S(9.f), (int)S(7.f),
                    ColorAlpha({22,22,32,255}, ra*0.92f));
                // steam jets from nostrils
                for(int j = 0; j < 4; j++){
                    float je = fmodf(steamAge + j*0.25f, 1.f);
                    DrawCircleV({sp.x + S(22.f) + je*S(10.f), spY - S(4.f) + j*S(1.5f)},
                        S(1.5f + je*2.f), ColorAlpha({180,200,220,255}, ra*(1.f-je)*0.40f));
                }
                // eye — glowing
                DrawCircleV({sp.x + S(16.f), spY - S(5.f)}, S(2.5f), ColorAlpha({5,5,10,255}, ra*0.95f));
                DrawCircleV({sp.x + S(16.f), spY - S(5.f)}, S(1.5f), ColorAlpha({255,160,20,255}, ra*0.90f));
                DrawNeonCircle({sp.x + S(16.f), spY - S(5.f)}, S(3.f), VP_ORANGE, ra*0.5f*pulse);
                // legs
                for(int side = -1; side <= 1; side += 2){
                    DrawLineEx({sp.x + side*S(8.f), spY + S(7.f)},
                        {sp.x + side*S(12.f), spY + S(18.f)}, S(3.5f), ColorAlpha({25,25,34,255}, ra*0.88f));
                    DrawCircleV({sp.x + side*S(12.f), spY + S(18.f)}, S(4.f), ColorAlpha({30,30,44,255}, ra*0.85f));
                }
            }
            else if(e.subType == 6){ // FURNACE IMP — small imp of molten steel, riding a gear platform
                float impBob=sinf(t*0.38f+ph+1.4f)*S(2.f); // slow asymmetric float
                float gearSpin=t*3.f+ph;
                float sparkAge=fmodf(t*3.f+ph,1.f);
                // gear platform
                DrawCircleV({sp.x,spY+S(16.f)},S(12.f),ColorAlpha({22,22,30,255},ra*0.90f));
                DrawCircleLinesV({sp.x,spY+S(16.f)},S(12.f),ColorAlpha({80,130,170,255},ra*0.55f));
                // gear teeth
                for(int i=0;i<10;i++){
                    float ga=gearSpin+i*(2*PI/10.f);
                    DrawCircleV({sp.x+cosf(ga)*S(12.f),spY+S(16.f)+sinf(ga)*S(12.f)},S(2.f),ColorAlpha({70,120,160,255},ra*0.65f));
                }
                // gear hub
                DrawCircleV({sp.x,spY+S(16.f)},S(4.f),ColorAlpha({255,140,20,255},ra*0.85f));
                DrawCircleV({sp.x,spY+S(16.f)},S(2.5f),ColorAlpha(VP_YELLOW,ra*fPulse));
                DrawNeonCircle({sp.x,spY+S(16.f)},S(5.f),VP_ORANGE,ra*fPulse*0.55f);
                // imp legs
                for(int side=-1;side<=1;side+=2){
                    DrawLineEx({sp.x+side*S(4.f),spY+S(8.f)+impBob},{sp.x+side*S(7.f),spY+S(14.f)+impBob},S(3.f),ColorAlpha({30,10,2,255},ra*0.88f));
                    DrawCircleV({sp.x+side*S(7.f),spY+S(14.f)+impBob},S(2.5f),ColorAlpha({45,14,3,255},ra*0.82f));
                }
                // imp body
                DrawEllipse((int)sp.x,(int)(spY+S(1.f)+impBob),(int)S(7.f),(int)S(9.f),ColorAlpha({28,9,1,255},ra*0.92f));
                DrawEllipse((int)sp.x,(int)(spY+S(1.f)+impBob),(int)S(5.5f),(int)S(7.5f),ColorAlpha({50,16,2,255},ra*0.85f));
                // belly forge glow
                DrawCircleV({sp.x,spY+S(3.f)+impBob},S(3.5f),ColorAlpha({180,50,8,255},ra*0.82f));
                DrawCircleV({sp.x,spY+S(3.f)+impBob},S(2.f),ColorAlpha(VP_ORANGE,ra*fPulse*0.80f));
                DrawNeonCircle({sp.x,spY+S(3.f)+impBob},S(5.f),VP_ORANGE,ra*fPulse*0.40f);
                // imp arms — holding a tiny hammer
                DrawLineEx({sp.x-S(6.f),spY-S(1.f)+impBob},{sp.x-S(14.f),spY-S(6.f)+impBob},S(3.f),ColorAlpha({30,10,2,255},ra*0.85f));
                DrawRectangle((int)(sp.x-S(16.f)),(int)(spY-S(8.f)+impBob),(int)S(5.f),(int)S(8.f),ColorAlpha({70,120,160,255},ra*0.85f));
                DrawLineEx({sp.x+S(6.f),spY-S(1.f)+impBob},{sp.x+S(13.f),spY-S(3.f)+impBob},S(3.f),ColorAlpha({30,10,2,255},ra*0.85f));
                // head
                DrawEllipse((int)sp.x,(int)(spY-S(9.f)+impBob),(int)S(6.f),(int)S(7.f),ColorAlpha({30,10,2,255},ra*0.92f));
                DrawEllipse((int)sp.x,(int)(spY-S(9.f)+impBob),(int)S(5.f),(int)S(5.5f),ColorAlpha({52,16,2,255},ra*0.85f));
                // horns
                for(int side=-1;side<=1;side+=2)
                    DrawLineEx({sp.x+side*S(3.f),spY-S(14.f)+impBob},{sp.x+side*S(6.f),spY-S(20.f)+impBob},S(2.f),ColorAlpha({35,11,2,255},ra*0.80f));
                // eyes
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(2.5f),spY-S(10.f)+impBob},S(2.f),ColorAlpha({5,2,0,255},ra*0.95f));
                    DrawCircleV({sp.x+side*S(2.5f),spY-S(10.f)+impBob},S(1.2f),ColorAlpha(VP_ORANGE,ra*0.90f));
                    DrawNeonCircle({sp.x+side*S(2.5f),spY-S(10.f)+impBob},S(3.f),VP_RED,ra*fPulse*0.55f);
                }
                // forge sparks flying
                for(int i=0;i<5;i++){
                    float sa=fmodf(sparkAge+i*0.2f,1.f);
                    float sx2=sp.x+(i-2)*S(4.f)+cosf(t*3.f+i)*S(3.f);
                    float sy2=spY+S(16.f)-sa*S(28.f);
                    DrawCircleV({sx2,sy2},S(1.5f*(1.f-sa)),ColorAlpha(sa<0.5f?VP_YELLOW:VP_ORANGE,ra*(1.f-sa)*0.70f));
                }
            }
            break;}

        case 7:{ // OBSIDIAN WASTES — iron sentinel: a cold robotic guardian of obsidian
            if(e.subType < 5){
            float sBob   = sinf(t*0.4f+ph)*S(1.f);
            float eyeP   = 0.6f+0.4f*sinf(t*1.5f+ph);
            float scanX  = sinf(t*0.9f+ph); // scanning beam sweep -1..1
            float hydraulic = sinf(t*0.22f+ph); // slow arm hydraulic cycle
            float chestCore = 0.55f+0.45f*sinf(t*2.1f+ph);
            float jointGlow = 0.4f+0.6f*fabsf(sinf(t*0.8f+ph));
            // ground shadow
            DrawEllipse((int)sp.x,(int)(spY+S(23.f)),(int)S(18.f),(int)S(4.f),ColorAlpha({0,0,0,255},ra*0.25f));
            // legs — double-plated with knee joints
            for(int side=-1;side<=1;side+=2){
                float lx=sp.x+side*S(5.5f);
                // upper leg plate
                DrawRectangle((int)(lx-S(3.f)),(int)(spY+S(8.f)+sBob),(int)S(6.f),(int)S(8.f),ColorAlpha({22,22,35,255},ra*0.92f));
                DrawRectangleLines((int)(lx-S(3.f)),(int)(spY+S(8.f)+sBob),(int)S(6.f),(int)S(8.f),ColorAlpha({50,65,100,255},ra*0.45f));
                // knee joint disc
                DrawCircleV({lx,spY+S(16.f)+sBob},S(3.5f),ColorAlpha({30,30,50,255},ra*0.90f));
                DrawCircleLinesV({lx,spY+S(16.f)+sBob},S(3.5f),ColorAlpha({70,100,150,255},ra*0.55f));
                DrawCircleV({lx,spY+S(16.f)+sBob},S(1.8f),ColorAlpha({0,180,255,255},ra*jointGlow*0.65f));
                DrawNeonCircle({lx,spY+S(16.f)+sBob},S(3.5f),VP_DEEPCYAN,ra*jointGlow*0.35f);
                // lower leg / shin
                DrawRectangle((int)(lx-S(2.5f)),(int)(spY+S(19.f)+sBob),(int)S(5.f),(int)S(8.f),ColorAlpha({18,18,28,255},ra*0.90f));
                DrawRectangleLines((int)(lx-S(2.5f)),(int)(spY+S(19.f)+sBob),(int)S(5.f),(int)S(8.f),ColorAlpha({45,60,95,255},ra*0.40f));
                // boot / foot block
                DrawRectangle((int)(lx-S(4.f)),(int)(spY+S(27.f)+sBob),(int)S(8.f),(int)S(4.f),ColorAlpha({15,15,25,255},ra*0.94f));
                DrawRectangleLines((int)(lx-S(4.f)),(int)(spY+S(27.f)+sBob),(int)S(8.f),(int)S(4.f),ColorAlpha({55,75,120,255},ra*0.45f));
                // foot thruster nozzle — faint blue glow beneath
                DrawEllipse((int)lx,(int)(spY+S(31.f)+sBob),(int)S(3.f),(int)S(1.5f),ColorAlpha({0,140,200,255},ra*0.35f));
                DrawNeonCircle({lx,spY+S(31.f)+sBob},S(3.5f),VP_DEEPCYAN,ra*0.18f);
            }
            // torso — layered armour with panel seams
            DrawRectangle((int)(sp.x-S(13.f)),(int)(spY-S(2.f)+sBob),(int)S(26.f),(int)S(12.f),ColorAlpha({14,14,24,255},ra*0.96f));
            DrawRectangle((int)(sp.x-S(12.f)),(int)(spY-S(1.f)+sBob),(int)S(24.f),(int)S(10.f),ColorAlpha({20,20,33,255},ra*0.92f));
            DrawRectangleLines((int)(sp.x-S(12.f)),(int)(spY-S(1.f)+sBob),(int)S(24.f),(int)S(10.f),ColorAlpha({55,75,115,255},ra*0.55f));
            // torso panel seams
            DrawLineEx({sp.x,spY-S(1.f)+sBob},{sp.x,spY+S(9.f)+sBob},S(0.7f),ColorAlpha({40,55,85,255},ra*0.50f));
            DrawLineEx({sp.x-S(12.f),spY+S(4.f)+sBob},{sp.x+S(12.f),spY+S(4.f)+sBob},S(0.7f),ColorAlpha({40,55,85,255},ra*0.45f));
            // chest core reactor — pulsing cyan orb
            DrawCircleV({sp.x,spY+S(4.f)+sBob},S(5.f),ColorAlpha({5,30,55,255},ra*0.90f));
            DrawCircleV({sp.x,spY+S(4.f)+sBob},S(3.5f),ColorAlpha({0,180,255,255},ra*chestCore*0.85f));
            DrawCircleV({sp.x,spY+S(4.f)+sBob},S(1.8f),ColorAlpha(VP_WHITE,ra*chestCore*0.70f));
            DrawNeonCircle({sp.x,spY+S(4.f)+sBob},S(5.f),VP_DEEPCYAN,ra*chestCore*0.65f);
            // chest core ring detail
            DrawCircleLinesV({sp.x,spY+S(4.f)+sBob},S(5.f),ColorAlpha({0,200,255,255},ra*0.45f));
            // shoulder pauldrons
            for(int side=-1;side<=1;side+=2){
                DrawRectangle((int)(sp.x+side*S(12.f)-S(2.f)),(int)(spY-S(4.f)+sBob),(int)S(7.f),(int)S(6.f),ColorAlpha({25,25,42,255},ra*0.90f));
                DrawRectangleLines((int)(sp.x+side*S(12.f)-S(2.f)),(int)(spY-S(4.f)+sBob),(int)S(7.f),(int)S(6.f),ColorAlpha({60,80,125,255},ra*0.50f));
                // pauldron ridge
                DrawLineEx({sp.x+side*S(12.f)-S(2.f),spY-S(2.f)+sBob},{sp.x+side*S(17.f),spY-S(2.f)+sBob},S(1.f),ColorAlpha({45,65,100,255},ra*0.55f));
            }
            // arms — hydraulic pistons, slightly angled by cycle
            for(int side=-1;side<=1;side+=2){
                float armY = spY-S(1.f)+sBob + hydraulic*side*S(1.5f);
                float ax = sp.x+side*S(18.f);
                DrawRectangle((int)(ax-S(3.f)),(int)armY,(int)S(6.f),(int)S(10.f),ColorAlpha({18,18,30,255},ra*0.90f));
                DrawRectangleLines((int)(ax-S(3.f)),(int)armY,(int)S(6.f),(int)S(10.f),ColorAlpha({50,68,108,255},ra*0.48f));
                // piston groove lines
                DrawLineEx({ax-S(1.f),armY+S(2.f)},{ax-S(1.f),armY+S(8.f)},S(0.6f),ColorAlpha({35,50,80,255},ra*0.45f));
                DrawLineEx({ax+S(1.f),armY+S(2.f)},{ax+S(1.f),armY+S(8.f)},S(0.6f),ColorAlpha({35,50,80,255},ra*0.45f));
                // elbow joint
                DrawCircleV({ax,armY+S(10.f)},S(2.8f),ColorAlpha({28,28,45,255},ra*0.90f));
                DrawCircleLinesV({ax,armY+S(10.f)},S(2.8f),ColorAlpha({60,85,130,255},ra*0.50f));
                // forearm / fist block
                DrawRectangle((int)(ax-S(3.5f)),(int)(armY+S(10.f)),(int)S(7.f),(int)S(7.f),ColorAlpha({22,22,36,255},ra*0.88f));
                DrawRectangleLines((int)(ax-S(3.5f)),(int)(armY+S(10.f)),(int)S(7.f),(int)S(7.f),ColorAlpha({55,75,118,255},ra*0.50f));
                // knuckle lines
                for(int k=0;k<3;k++)
                    DrawLineEx({ax-S(2.f)+k*S(2.f),armY+S(11.f)},{ax-S(2.f)+k*S(2.f),armY+S(16.f)},S(0.5f),ColorAlpha({40,55,85,255},ra*0.50f));
            }
            // head — blocky with neck collar
            DrawRectangle((int)(sp.x-S(7.f)),(int)(spY-S(4.f)+sBob),(int)S(14.f),(int)S(3.f),ColorAlpha({22,22,38,255},ra*0.85f)); // neck
            DrawRectangle((int)(sp.x-S(10.f)),(int)(spY-S(16.f)+sBob),(int)S(20.f),(int)S(13.f),ColorAlpha({14,14,24,255},ra*0.96f));
            DrawRectangle((int)(sp.x-S(9.f)),(int)(spY-S(15.f)+sBob),(int)S(18.f),(int)S(12.f),ColorAlpha({18,18,30,255},ra*0.94f));
            DrawRectangleLines((int)(sp.x-S(9.f)),(int)(spY-S(15.f)+sBob),(int)S(18.f),(int)S(12.f),ColorAlpha({50,70,115,255},ra*0.55f));
            // brow ridge
            DrawRectangle((int)(sp.x-S(9.f)),(int)(spY-S(15.f)+sBob),(int)S(18.f),(int)S(3.f),ColorAlpha({22,22,38,255},ra*0.80f));
            // visor slit with scanning beam
            DrawRectangle((int)(sp.x-S(7.f)),(int)(spY-S(11.f)+sBob),(int)S(14.f),(int)S(4.f),ColorAlpha({5,25,55,255},ra*0.88f));
            DrawNeonLine({sp.x-S(7.f),spY-S(9.5f)+sBob},{sp.x+S(7.f),spY-S(9.5f)+sBob},VP_DEEPCYAN,S(0.9f),ra*eyeP*0.80f);
            // scanning dot moving across visor
            float scanDot = sp.x + scanX*S(5.f);
            DrawCircleV({scanDot,spY-S(9.5f)+sBob},S(1.8f),ColorAlpha(VP_WHITE,ra*eyeP*0.80f));
            DrawNeonCircle({scanDot,spY-S(9.5f)+sBob},S(3.f),{0,220,255,255},ra*eyeP*0.55f);
            // cheek vents
            for(int side=-1;side<=1;side+=2){
                for(int v=0;v<3;v++)
                    DrawLineEx({sp.x+side*S(6.f),spY-S(7.f)+v*S(2.f)+sBob},{sp.x+side*S(9.f),spY-S(7.f)+v*S(2.f)+sBob},S(0.7f),ColorAlpha({40,60,95,255},ra*0.50f));
            }
            // twin antennae
            for(int side=-1;side<=1;side+=2){
                float atx=sp.x+side*S(5.f);
                DrawLineEx({atx,spY-S(15.f)+sBob},{atx+sinf(t*0.8f+ph+side)*S(1.5f),spY-S(23.f)+sBob},S(0.8f),ColorAlpha({0,160,220,255},ra*0.60f));
                DrawNeonCircle({atx+sinf(t*0.8f+ph+side)*S(1.5f),spY-S(23.f)+sBob},S(1.6f),VP_CYAN,ra*eyeP*0.65f);
            }
            // radar sweep glow — faint cone rotating from chest
            {
                float radarA = t*1.4f+ph;
                DrawLineEx({sp.x,spY+S(4.f)+sBob},{sp.x+cosf(radarA)*S(18.f),spY+S(4.f)+sinf(radarA)*S(10.f)+sBob},S(0.6f),ColorAlpha({0,180,255,255},ra*0.20f));
                DrawLineEx({sp.x,spY+S(4.f)+sBob},{sp.x+cosf(radarA+0.3f)*S(16.f),spY+S(4.f)+sinf(radarA+0.3f)*S(9.f)+sBob},S(0.4f),ColorAlpha({0,180,255,255},ra*0.10f));
            }
            }
            else if(e.subType == 5){ // OBSIDIAN BASILISK — volcanic glass lizard, petrifying gaze
                float pulse   = 0.55f+0.45f*sinf(t*1.8f+ph);
                float breathe = sinf(t*0.45f+ph)*S(1.0f);
                float tailWave= sinf(t*0.9f+ph);
                float eyeGlow = 0.5f+0.5f*fabsf(sinf(t*2.5f+ph));
                // ground shadow
                DrawEllipse((int)sp.x,(int)(spY+S(24.f)),(int)S(24.f),(int)S(5.f),ColorAlpha(VP_BLACK,ra*0.22f));
                // tail — thick tapering segments, curves to one side
                int tSeg=10;
                for(int i=tSeg-1;i>=0;i--){
                    float fi=(float)i/tSeg;
                    float tx2=sp.x+S(12.f)+fi*S(22.f);
                    float ty2=spY+S(8.f)+breathe+sinf(tailWave+fi*2.f)*S(6.f)*fi;
                    float tr=S(6.f-fi*4.f);
                    DrawCircleV({tx2,ty2},tr,ColorAlpha({10,8,14,255},ra*0.92f));
                    DrawCircleLinesV({tx2,ty2},tr,ColorAlpha({55,50,70,255},ra*0.45f));
                    // dorsal ridge spike every 2 segments
                    if(i%2==0 && i>1){
                        float sa=-(PI*0.5f);
                        DrawLineEx({tx2,ty2},{tx2+cosf(sa)*tr*1.6f,ty2+sinf(sa)*tr*1.6f},
                            S(1.5f),ColorAlpha({8,6,12,255},ra*0.88f));
                        DrawCircleV({tx2+cosf(sa)*tr*1.6f,ty2+sinf(sa)*tr*1.6f},
                            S(0.9f),ColorAlpha({90,80,110,255},ra*0.60f));
                    }
                }
                // hind legs
                for(int side=-1;side<=1;side+=2){
                    float lx=sp.x+S(8.f)+side*S(8.f), ly=spY+S(12.f)+breathe;
                    DrawEllipse((int)lx,(int)ly,(int)S(5.f),(int)S(9.f),ColorAlpha({12,10,18,255},ra*0.88f));
                    DrawEllipseLines((int)lx,(int)ly,(int)S(5.f),(int)S(9.f),ColorAlpha({60,55,80,255},ra*0.40f));
                    // foot claws
                    for(int c=-1;c<=1;c++)
                        DrawLineEx({lx+c*S(2.f),ly+S(8.f)},{lx+c*S(3.f),ly+S(13.f)},S(1.8f),ColorAlpha({10,8,14,255},ra*0.88f));
                }
                // body — solid obsidian mass, glossy dark
                DrawEllipse((int)(sp.x-S(2.f)),(int)(spY+S(6.f)+breathe),(int)S(18.f),(int)S(11.f),ColorAlpha({8,6,12,255},ra*0.95f));
                DrawEllipse((int)(sp.x-S(2.f)),(int)(spY+S(6.f)+breathe),(int)S(16.f),(int)S(9.f),  ColorAlpha({18,14,26,255},ra*0.88f));
                DrawEllipseLines((int)(sp.x-S(2.f)),(int)(spY+S(6.f)+breathe),(int)S(18.f),(int)S(11.f),ColorAlpha({70,60,95,255},ra*0.45f));
                // glassy highlight on body
                DrawEllipse((int)(sp.x-S(8.f)),(int)(spY+S(3.f)+breathe),(int)S(7.f),(int)S(3.f),ColorAlpha({140,130,180,255},ra*0.18f));
                // fore legs
                for(int side=-1;side<=1;side+=2){
                    float lx=sp.x-S(12.f)+side*S(7.f), ly=spY+S(8.f)+breathe;
                    DrawEllipse((int)lx,(int)ly,(int)S(4.f),(int)S(7.f),ColorAlpha({11,9,16,255},ra*0.88f));
                    DrawEllipseLines((int)lx,(int)ly,(int)S(4.f),(int)S(7.f),ColorAlpha({55,50,75,255},ra*0.38f));
                    for(int c=-1;c<=1;c++)
                        DrawLineEx({lx+c*S(2.f),ly+S(6.f)},{lx+c*S(2.5f),ly+S(10.f)},S(1.5f),ColorAlpha({9,7,13,255},ra*0.88f));
                }
                // neck
                DrawEllipse((int)(sp.x-S(18.f)),(int)(spY+S(2.f)+breathe),(int)S(9.f),(int)S(7.f),ColorAlpha({10,8,14,255},ra*0.92f));
                // head — broad flat reptile skull
                DrawEllipse((int)(sp.x-S(26.f)),(int)(spY-S(2.f)+breathe),(int)S(11.f),(int)S(8.f),ColorAlpha({8,6,12,255},ra*0.95f));
                DrawEllipse((int)(sp.x-S(26.f)),(int)(spY-S(2.f)+breathe),(int)S(10.f),(int)S(7.f),ColorAlpha({16,13,22,255},ra*0.88f));
                DrawEllipseLines((int)(sp.x-S(26.f)),(int)(spY-S(2.f)+breathe),(int)S(11.f),(int)S(8.f),ColorAlpha({70,60,90,255},ra*0.45f));
                // brow ridge horns — two short obsidian spikes
                for(int side=-1;side<=1;side+=2){
                    DrawLineEx({sp.x-S(26.f)+side*S(4.f),spY-S(7.f)+breathe},
                        {sp.x-S(26.f)+side*S(5.f),spY-S(13.f)+breathe},
                        S(2.5f),ColorAlpha({12,10,18,255},ra*0.90f));
                    DrawCircleV({sp.x-S(26.f)+side*S(5.f),spY-S(13.f)+breathe},
                        S(1.2f),ColorAlpha({80,70,105,255},ra*0.65f));
                }
                // petrifying eyes — glowing amber with slit pupils
                for(int side=-1;side<=1;side+=2){
                    float ex2=sp.x-S(26.f)+side*S(4.f), ey2=spY-S(2.f)+breathe;
                    DrawCircleV({ex2,ey2},S(3.5f),ColorAlpha({5,3,8,255},ra*0.95f));
                    DrawCircleV({ex2,ey2},S(2.5f),ColorAlpha({180,90,10,255},ra*eyeGlow*0.90f));
                    DrawCircleV({ex2,ey2},S(1.0f),ColorAlpha(VP_YELLOW,ra*eyeGlow));
                    DrawNeonCircle({ex2,ey2},S(4.f),{200,100,0,255},ra*eyeGlow*0.60f);
                    // slit pupil
                    DrawLineEx({ex2,ey2-S(1.8f)},{ex2,ey2+S(1.8f)},S(1.0f),ColorAlpha(VP_BLACK,ra*0.95f));
                }
                // petrify glow aura — amber haze around eyes
                DrawCircleV({sp.x-S(26.f),spY-S(2.f)+breathe},S(8.f),ColorAlpha({160,80,5,255},ra*eyeGlow*0.07f));
                DrawNeonCircle({sp.x-S(26.f),spY-S(2.f)+breathe},S(9.f),{180,90,0,255},ra*eyeGlow*0.18f);
                // stone fragment halo — chunks of petrified matter orbiting slowly
                for(int i=0;i<5;i++){
                    float sa=t*0.3f+ph+i*(2*PI/5.f);
                    float sr2=S(20.f+sinf(t*0.6f+i+ph)*3.f);
                    DrawRectanglePro({sp.x+cosf(sa)*sr2,spY+sinf(sa)*sr2*0.6f,S(4.f),S(3.f)},{S(2.f),S(1.5f)},sa*RAD2DEG,ColorAlpha({55,50,65,255},ra*0.55f));
                    DrawNeonCircle({sp.x+cosf(sa)*sr2,spY+sinf(sa)*sr2*0.6f},S(2.f),{180,90,0,255},ra*eyeGlow*0.28f);
                }
            }
            else if(e.subType == 6){ // OBSIDIAN SPIDER — giant spider of volcanic glass, web of steel
                float legStep=sinf(t*2.5f+ph);
                float webAge=fmodf(t*0.8f+ph,1.f);
                // web strands radiating out
                for(int i=0;i<8;i++){
                    float wa=ph+i*(2*PI/8.f);
                    float wl=S(35.f);
                    DrawLineEx(sp,{sp.x+cosf(wa)*wl,spY+sinf(wa)*wl},S(0.6f),ColorAlpha({70,80,90,255},ra*0.30f));
                }
                // web rings
                for(int r=1;r<=3;r++){
                    float wr=S(r*10.f);
                    DrawCircleLinesV(sp,wr,ColorAlpha({65,75,88,255},ra*0.25f));
                }
                // 8 legs — 4 per side
                for(int i=0;i<4;i++){
                    for(int side=-1;side<=1;side+=2){
                        float la=side*(0.4f+i*0.22f)+PI*.5f;
                        float lOff=legStep*side*(i%2==0?1.f:-1.f)*S(2.f);
                        float knee={sp.x+cosf(la)*S(14.f)};
                        float kneeY=spY+sinf(la)*S(10.f);
                        float footX=sp.x+cosf(la)*S(26.f)+lOff;
                        float footY=spY+sinf(la)*S(18.f)+S(4.f);
                        DrawLineEx(sp,{knee,kneeY},S(2.5f),ColorAlpha({15,15,22,255},ra*0.90f));
                        DrawLineEx({knee,kneeY},{footX,footY},S(2.f),ColorAlpha({18,18,26,255},ra*0.85f));
                        // claw tips
                        DrawCircleV({footX,footY},S(2.f),ColorAlpha({8,8,14,255},ra*0.92f));
                        DrawCircleLinesV({footX,footY},S(2.f),ColorAlpha({80,90,105,255},ra*0.45f));
                    }
                }
                // abdomen
                DrawEllipse((int)(sp.x-S(6.f)),(int)(spY+S(5.f)),(int)S(12.f),(int)S(10.f),ColorAlpha({10,10,16,255},ra*0.92f));
                DrawEllipseLines((int)(sp.x-S(6.f)),(int)(spY+S(5.f)),(int)S(12.f),(int)S(10.f),ColorAlpha({70,80,95,255},ra*0.45f));
                // abdomen facets
                for(int i=0;i<4;i++){
                    float fa=ph+i*(PI/2.f);
                    DrawLineEx({sp.x-S(6.f),spY+S(5.f)},{sp.x-S(6.f)+cosf(fa)*S(10.f),spY+S(5.f)+sinf(fa)*S(8.f)},S(0.5f),ColorAlpha({50,58,70,255},ra*0.30f));
                }
                // cephalothorax
                DrawEllipse((int)sp.x,(int)(spY-S(2.f)),(int)S(9.f),(int)S(7.f),ColorAlpha({12,12,18,255},ra*0.92f));
                DrawEllipseLines((int)sp.x,(int)(spY-S(2.f)),(int)S(9.f),(int)S(7.f),ColorAlpha({65,75,90,255},ra*0.50f));
                // 8 eyes — two rows
                float eyePos[][2]={{-4,-5},{-1,-5},{2,-5},{4.5f,-4.f},{-3,-2},{0,-2},{3,-2},{4.5f,-1.f}};
                for(auto& ep:eyePos){
                    DrawCircleV({sp.x+S(ep[0]),spY+S(ep[1])},S(1.3f),ColorAlpha({3,3,6,255},ra*0.95f));
                    DrawCircleV({sp.x+S(ep[0]),spY+S(ep[1])},S(0.7f),ColorAlpha({80,90,105,255},ra*0.75f));
                }
                // chelicerae (fangs)
                for(int side=-1;side<=1;side+=2){
                    DrawLineEx({sp.x+side*S(3.f),spY+S(4.f)},{sp.x+side*S(5.f),spY+S(9.f)},S(2.f),ColorAlpha({14,14,20,255},ra*0.88f));
                    DrawCircleV({sp.x+side*S(5.f),spY+S(9.f)},S(1.8f),ColorAlpha({70,80,95,255},ra*0.60f));
                }
                DrawNeonCircle(sp,S(22.f),{55,65,80,255},ra*0.10f);
            }
            break;}

        case 8:{ // CRIMSON NEBULA — 4 variations based on subType
            float pulse = 0.6f + 0.4f * sinf(t * 1.4f + ph);
            
            if(e.subType == 0){ // nebula wisp — ethereal gas creature with tendril arms
                // outer aura rings (definition layers)
                for(int i = 0; i < 3; i++){
                    float age = fmodf(t * 0.5f + i * 0.4f + ph, 1.f);
                    DrawCircleLinesV({sp.x,spY}, S(22.f + i*6.f + age*4.f),
                        ColorAlpha(VP_RED, ra*(1.f-age)*0.25f));
                }
                // wispy body layers with defined edges
                for(int i = 0; i < 5; i++){
                    float age = fmodf(t * 0.4f + i * 0.3f + ph, 1.f);
                    float radius = S(12.f - i*1.8f + age*3.f);
                    DrawCircleV({sp.x,spY-S(2.f*i)}, radius,
                        ColorAlpha(i%2==0?VP_HOTPINK:VP_CORAL, ra*(1.f-age)*0.15f));
                    // outline each layer for definition
                    DrawCircleLinesV({sp.x,spY-S(2.f*i)}, radius,
                        ColorAlpha(VP_RED, ra*(1.f-age)*0.4f));
                }
                // face features (two glowing eyes)
                float eyeGlow = 0.7f + 0.3f*sinf(t*2.5f+ph);
                DrawCircleV({sp.x-S(3.f),spY-S(3.f)},S(2.2f),ColorAlpha({80,10,25,255},ra*0.9f));
                DrawCircleV({sp.x+S(3.f),spY-S(3.f)},S(2.2f),ColorAlpha({80,10,25,255},ra*0.9f));
                DrawCircleV({sp.x-S(3.f),spY-S(3.f)},S(1.5f),ColorAlpha(VP_WHITE,ra*eyeGlow));
                DrawCircleV({sp.x+S(3.f),spY-S(3.f)},S(1.5f),ColorAlpha(VP_WHITE,ra*eyeGlow));
                DrawNeonCircle({sp.x-S(3.f),spY-S(3.f)},S(2.5f),VP_HOTPINK,ra*eyeGlow*0.6f);
                DrawNeonCircle({sp.x+S(3.f),spY-S(3.f)},S(2.5f),VP_HOTPINK,ra*eyeGlow*0.6f);
                // flowing tendrils with segmented structure
                for(int i = 0; i < 6; i++){
                    float ta = t*0.8f + ph + i*(2.f*PI/6.f);
                    float tLen = S(18.f + sinf(t*1.2f+i+ph)*6.f);
                    Vector2 base = {sp.x,spY+S(5.f)};
                    Vector2 tip = {sp.x+cosf(ta)*tLen,spY+S(5.f)+sinf(ta)*tLen};
                    // draw tendril segments for definition
                    for(int j = 0; j < 5; j++){
                        float fj = (float)j/5.f;
                        Vector2 p1 = {base.x+(tip.x-base.x)*fj, base.y+(tip.y-base.y)*fj};
                        Vector2 p2 = {base.x+(tip.x-base.x)*(fj+0.2f), base.y+(tip.y-base.y)*(fj+0.2f)};
                        DrawLineEx(p1,p2,S(2.2f-fj*1.2f),ColorAlpha(VP_HOTPINK,ra*(0.45f-fj*0.1f)));
                        // outline for definition
                        DrawLineEx(p1,p2,S(2.8f-fj*1.2f),ColorAlpha(VP_RED,ra*(0.25f-fj*0.05f)));
                    }
                    // glowing tip orb with bright core
                    DrawCircleV(tip,S(2.8f),ColorAlpha(VP_RED,ra*0.7f));
                    DrawCircleV(tip,S(2.f),ColorAlpha(VP_HOTPINK,ra*pulse*0.8f));
                    DrawCircleV(tip,S(1.2f),ColorAlpha(VP_WHITE,ra*pulse));
                    DrawNeonCircle(tip,S(3.f),VP_RED,ra*pulse*0.5f);
                }
                // defined glowing core with multiple layers
                DrawCircleV({sp.x,spY},S(8.f),ColorAlpha({60,8,20,255},ra*0.85f));
                DrawCircleV({sp.x,spY},S(6.f),ColorAlpha(VP_RED,ra*0.8f));
                DrawCircleV({sp.x,spY},S(3.5f),ColorAlpha(VP_HOTPINK,ra*pulse*0.75f));
                DrawCircleV({sp.x,spY},S(2.f),ColorAlpha(VP_WHITE,ra*pulse*0.9f));
                // core outline
                DrawCircleLinesV({sp.x,spY},S(8.f),ColorAlpha(VP_RED,ra*0.6f));
                DrawNeonCircle({sp.x,spY},S(9.f),VP_HOTPINK,ra*pulse*0.55f);
            }
            else if(e.subType == 1){ // baby protostar — original design
                for(int i = 4; i >= 0; i--){
                    float age = fmodf(t * 0.3f + i * 0.22f + ph, 1.f);
                    DrawCircleV({sp.x,spY}, S(18.f + i*5.f + age*6.f),
                        ColorAlpha(i%2==0?VP_RED:VP_HOTPINK, ra*(1.f-age)*0.07f));
                }
                for(int i = 0; i < 7; i++){
                    float a = t*0.25f + ph + i*(2.f*PI/7.f);
                    DrawLineEx({sp.x+cosf(a)*S(22.f),spY+sinf(a)*S(16.f)},
                               {sp.x+cosf(a+0.4f)*S(9.f),spY+sinf(a+0.4f)*S(7.f)},
                               S(1.1f), ColorAlpha(VP_HOTPINK, ra*0.22f));
                }
                DrawCircleV({sp.x,spY}, S(8.f),  ColorAlpha({100,10,40,255},  ra*0.9f));
                DrawCircleV({sp.x,spY}, S(5.5f), ColorAlpha(VP_HOTPINK, ra*pulse*0.8f));
                DrawCircleV({sp.x,spY}, S(3.f),  ColorAlpha(VP_LAVENDER,ra*pulse*0.7f));
                DrawCircleV({sp.x,spY}, S(1.2f), ColorAlpha(VP_WHITE,   ra*pulse*0.95f));
                DrawNeonCircle({sp.x,spY}, S(7.f), VP_RED, ra*0.4f*pulse);
            }
            else if(e.subType == 2){ // gas tendril — serpentine ribbon of stellar gas
                float coil = t*0.6f + ph;
                Vector2 prev = {sp.x-S(25.f), spY};
                // 15-segment tendril weaving through space
                for(int i = 0; i < 15; i++){
                    float fi = (float)i/15.f;
                    float tx = sp.x - S(25.f) + fi*S(50.f);
                    float ty = spY + sinf(coil + i*0.5f)*S(8.f);
                    float tr = S(4.f - fi*2.5f);
                    Color tc = fi<0.3f?ColorAlpha(VP_RED,ra*0.7f):fi<0.7f?ColorAlpha(VP_HOTPINK,ra*0.65f):ColorAlpha(VP_LAVENDER,ra*0.55f);
                    DrawLineEx(prev,{tx,ty},tr*1.5f,ColorAlpha(VP_HOTPINK,ra*0.25f));
                    DrawCircleV({tx,ty},tr,tc);
                    DrawNeonCircle({tx,ty},tr+S(1.f),VP_RED,ra*pulse*(1.f-fi)*0.35f);
                    prev = {tx,ty};
                }
            }
            else if(e.subType == 5){ // PLASMA BAT — crimson bat of ionised gas, sonar pulse rings
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float flap = sinf(t * 4.5f + ph);
                float sonarAge = fmodf(t * 1.0f + ph, 1.f);
                // sonar pulse rings expanding outward
                for(int i = 0; i < 3; i++){
                    float ra2 = fmodf(sonarAge + i * 0.33f, 1.f);
                    DrawCircleLinesV({sp.x, spY}, S(8.f + ra2 * 40.f),
                        ColorAlpha(VP_RED, ra * (1.f - ra2) * 0.35f));
                }
                // wings — swept membrane panels
                for(int side = -1; side <= 1; side += 2){
                    float wFlex = flap * side;
                    // outer wing
                    Vector2 shoulder = {sp.x + side*S(4.f), spY - S(2.f)};
                    Vector2 wTip     = {sp.x + side*S(28.f + wFlex*4.f), spY - S(10.f + wFlex*6.f)};
                    Vector2 wBot     = {sp.x + side*S(20.f), spY + S(8.f)};
                    DrawTriangle(shoulder, wTip, wBot,
                        ColorAlpha({80,8,18,255}, ra*0.75f));
                    DrawTriangleLines(shoulder, wTip, wBot,
                        ColorAlpha(VP_RED, ra*0.45f));
                    // wing membrane veins
                    for(int v = 1; v <= 3; v++){
                        float vt = (float)v/4.f;
                        Vector2 vp = {shoulder.x + (wTip.x-shoulder.x)*vt,
                                      shoulder.y + (wTip.y-shoulder.y)*vt};
                        DrawLineEx(shoulder, vp, S(0.7f), ColorAlpha(VP_HOTPINK, ra*0.30f));
                    }
                    // thumb claw
                    DrawCircleV({sp.x + side*S(14.f + wFlex*2.f), spY - S(6.f)},
                        S(2.5f), ColorAlpha({60,5,12,255}, ra*0.88f));
                    DrawLineEx({sp.x + side*S(14.f + wFlex*2.f), spY - S(6.f)},
                        {sp.x + side*S(18.f + wFlex*2.f), spY - S(12.f)},
                        S(1.8f), ColorAlpha({100,10,22,255}, ra*0.80f));
                }
                // body
                DrawEllipse((int)sp.x, (int)spY, (int)S(7.f), (int)S(11.f),
                    ColorAlpha({70,5,15,255}, ra*0.92f));
                DrawEllipse((int)sp.x, (int)spY, (int)S(5.5f), (int)S(9.f),
                    ColorAlpha({120,10,25,255}, ra*0.82f));
                DrawNeonCircle({sp.x, spY}, S(7.f), VP_RED, ra*0.22f*pulse);
                // head — upside-down hanging
                DrawCircleV({sp.x, spY - S(12.f)}, S(7.f), ColorAlpha({65,5,14,255}, ra*0.92f));
                DrawCircleV({sp.x, spY - S(12.f)}, S(5.5f), ColorAlpha({110,8,20,255}, ra*0.85f));
                // ears
                for(int side = -1; side <= 1; side += 2){
                    DrawLineEx({sp.x + side*S(3.f), spY - S(17.f)},
                        {sp.x + side*S(7.f), spY - S(24.f)},
                        S(3.f), ColorAlpha({70,5,15,255}, ra*0.88f));
                    DrawCircleV({sp.x + side*S(7.f), spY - S(24.f)}, S(2.f),
                        ColorAlpha({120,10,25,255}, ra*0.80f));
                }
                // eyes — glowing scarlet
                for(int side = -1; side <= 1; side += 2){
                    DrawCircleV({sp.x + side*S(3.f), spY - S(13.f)}, S(2.8f),
                        ColorAlpha({5,0,1,255}, ra*0.95f));
                    DrawCircleV({sp.x + side*S(3.f), spY - S(13.f)}, S(1.6f),
                        ColorAlpha(VP_RED, ra*0.90f));
                    DrawNeonCircle({sp.x + side*S(3.f), spY - S(13.f)}, S(3.5f),
                        VP_HOTPINK, ra*pulse*0.55f);
                }
            }
            else if(e.subType == 6){ // NEBULA MANTA — stingray of ionised gas drifting through space
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float glide8=sinf(t*0.7f+ph)*S(4.f);
                float ripple8=sinf(t*1.5f+ph);
                float ionAge=fmodf(t*1.8f+ph,1.f);
                // ionised trail streamers
                for(int i=0;i<6;i++){
                    float ta=fmodf(ionAge+i*0.17f,1.f);
                    float tx2=sp.x+S(8.f)+ta*S(22.f)+(i-2.5f)*S(3.f);
                    float ty2=spY+glide8+ta*S(6.f);
                    DrawCircleV({tx2,ty2},S(2.f+ta*2.f),ColorAlpha(i%2==0?VP_RED:VP_HOTPINK,ra*(1.f-ta)*0.40f));
                }
                // wing left
                DrawTriangle({sp.x,spY+glide8},{sp.x-S(30.f)+ripple8*S(5.f),spY-S(8.f)+glide8},{sp.x-S(6.f),spY+S(10.f)+glide8},ColorAlpha({60,8,18,255},ra*0.82f));
                DrawTriangleLines({sp.x,spY+glide8},{sp.x-S(30.f)+ripple8*S(5.f),spY-S(8.f)+glide8},{sp.x-S(6.f),spY+S(10.f)+glide8},ColorAlpha(VP_RED,ra*0.35f));
                // wing right
                DrawTriangle({sp.x,spY+glide8},{sp.x+S(30.f)-ripple8*S(5.f),spY-S(8.f)+glide8},{sp.x+S(6.f),spY+S(10.f)+glide8},ColorAlpha({60,8,18,255},ra*0.82f));
                DrawTriangleLines({sp.x,spY+glide8},{sp.x+S(30.f)-ripple8*S(5.f),spY-S(8.f)+glide8},{sp.x+S(6.f),spY+S(10.f)+glide8},ColorAlpha(VP_RED,ra*0.35f));
                // wing veins
                for(int side=-1;side<=1;side+=2)
                    for(int v=1;v<=3;v++)
                        DrawLineEx({sp.x,spY+glide8},{sp.x+side*S(30.f)*(float)v/4.f,spY-S(8.f)*(float)v/4.f+glide8},S(0.6f),ColorAlpha(VP_HOTPINK,ra*0.22f));
                // central disc
                DrawEllipse((int)sp.x,(int)(spY+glide8),(int)S(9.f),(int)S(6.f),ColorAlpha({50,5,14,255},ra*0.92f));
                DrawEllipse((int)sp.x,(int)(spY+glide8),(int)S(7.f),(int)S(4.5f),ColorAlpha({90,10,24,255},ra*0.85f));
                // plasma nucleus
                DrawCircleV({sp.x,spY+glide8},S(3.f),ColorAlpha(VP_RED,ra*0.88f));
                DrawCircleV({sp.x,spY+glide8},S(1.8f),ColorAlpha(VP_HOTPINK,ra*pulse*0.85f));
                DrawNeonCircle({sp.x,spY+glide8},S(5.f),VP_RED,ra*pulse*0.55f);
                // tail barb
                for(int i=0;i<5;i++){
                    float fi=(float)i/5.f;
                    float tx2=sp.x+S(9.f)+i*S(5.5f);
                    float ty2=spY+glide8+sinf(t*1.8f+ph+fi*PI)*S(3.f)*fi;
                    DrawCircleV({tx2,ty2},S(3.5f-fi*2.f),ColorAlpha({52,6,14,255},ra*0.85f));
                }
                DrawCircleV({sp.x+S(36.f),spY+glide8},S(2.5f),ColorAlpha(VP_HOTPINK,ra*pulse*0.75f));
                DrawNeonCircle({sp.x+S(36.f),spY+glide8},S(4.f),VP_RED,ra*pulse*0.50f);
                // ambient nebula glow
                DrawEllipse((int)sp.x,(int)(spY+glide8),(int)S(32.f),(int)S(18.f),ColorAlpha(VP_RED,ra*0.04f));
            }
            else{ // ember mote — tiny glowing particle with orbit trail
                // orbital path
                for(int i = 0; i < 20; i++){
                    float oa = i*(2.f*PI/20.f);
                    DrawCircleV({sp.x+cosf(oa)*S(15.f),spY+sinf(oa)*S(12.f)},S(0.5f),
                        ColorAlpha(VP_CORAL,ra*0.15f));
                }
                // orbiting mote
                float oa = t*1.2f + ph;
                float mx = sp.x + cosf(oa)*S(15.f);
                float my = spY + sinf(oa)*S(12.f);
                DrawCircleV({mx,my},S(4.f),ColorAlpha(VP_ORANGE,ra*0.85f));
                DrawCircleV({mx,my},S(2.5f),ColorAlpha(VP_YELLOW,ra*pulse*0.7f));
                DrawCircleV({mx,my},S(1.f),ColorAlpha(VP_WHITE,ra*pulse));
                DrawNeonCircle({mx,my},S(5.f),VP_RED,ra*pulse*0.6f);
                // trailing embers
                for(int i = 1; i <= 5; i++){
                    float ta = oa - i*0.25f;
                    float tr = S(2.5f - i*0.4f);
                    DrawCircleV({sp.x+cosf(ta)*S(15.f),spY+sinf(ta)*S(12.f)},tr,
                        ColorAlpha(VP_ORANGE,ra*(1.f-i/6.f)*0.5f));
                }
            }
            break;}

        case 9:{ // ABYSSAL TRENCH — giant isopod: armoured deep-sea crustacean, photophore lit
            if(e.subType < 5){
            float iBob = sinf(t*0.45f+ph)*S(1.5f);
            float legAnim = sinf(t*1.8f+ph);
            // body (segmented armour plates — 5 segments)
            for(int i = 0; i < 5; i++){
                float sx2 = sp.x + S(-14.f + i*7.f);
                DrawRectangle((int)(sx2-S(3.f)),(int)(spY-S(4.f)+iBob),(int)S(7.f),(int)S(9.f),
                    ColorAlpha({10,50,70,255},ra*0.88f));
                DrawRectangle((int)(sx2-S(2.f)),(int)(spY-S(4.f)+iBob),(int)S(1.5f),(int)S(9.f),
                    ColorAlpha({15,70,95,255},ra*0.35f));
                // segment photophore (bioluminescent dot)
                DrawNeonCircle({sx2,spY+iBob},S(1.2f),VP_DEEPCYAN,ra*(0.4f+0.4f*sinf(t*2.f+i*0.8f+ph)));
            }
            // head shield (larger, rounded)
            DrawEllipse((int)(sp.x-S(17.f)),(int)(spY+iBob),(int)S(7.f),(int)S(5.f),
                ColorAlpha({12,60,80,255},ra*0.9f));
            // compound eyes (two rows of 3)
            for(int i = 0; i < 3; i++){
                DrawCircleV({sp.x-S(20.f+i*1.8f),spY-S(3.f)+iBob},S(1.3f),
                    ColorAlpha({180,240,0,255},ra*0.65f));
                DrawNeonCircle({sp.x-S(20.f+i*1.8f),spY-S(3.f)+iBob},S(1.3f),VP_MINT,ra*0.35f);
            }
            // antennae
            DrawLineEx({sp.x-S(22.f),spY-S(3.f)+iBob},
                       {sp.x-S(32.f)+sinf(t*1.2f+ph)*S(3.f),spY-S(10.f)+iBob},
                       S(0.8f),ColorAlpha(VP_DEEPCYAN,ra*0.55f));
            DrawLineEx({sp.x-S(22.f),spY-S(3.f)+iBob},
                       {sp.x-S(30.f)+sinf(t*1.5f+ph)*S(2.f),spY-S(6.f)+iBob},
                       S(0.8f),ColorAlpha(VP_DEEPCYAN,ra*0.45f));
            // legs (7 pairs, alternating animated)
            for(int i = 0; i < 7; i++){
                float lx = sp.x + S(-12.f + i*4.5f);
                float kick = sinf(t*1.8f + i*0.5f + ph) * S(2.f);
                DrawLineEx({lx,spY+S(5.f)+iBob},{lx+S(1.5f),spY+S(11.f)+kick+iBob},
                    S(0.9f),ColorAlpha({15,65,85,255},ra*0.75f));
                DrawLineEx({lx,spY+S(5.f)+iBob},{lx-S(1.f),spY+S(11.f)-kick+iBob},
                    S(0.9f),ColorAlpha({15,65,85,255},ra*0.60f));
            }
            }
            else if(e.subType == 5){ // ANGLERFISH — deep-sea anglerfish with bioluminescent lure
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float lureSway = sinf(t * 1.2f + ph) * S(6.f);
                float mouthOpen = 0.4f + 0.4f*sinf(t * 0.5f + ph);
                float lureGlow = 0.7f + 0.3f*sinf(t * 3.f + ph);
                // body — massive oval, dark
                DrawEllipse((int)sp.x, (int)(spY + S(4.f)), (int)S(22.f), (int)S(16.f),
                    ColorAlpha({2,8,20,255}, ra*0.95f));
                DrawEllipse((int)sp.x, (int)(spY + S(4.f)), (int)S(20.f), (int)S(14.f),
                    ColorAlpha({5,18,38,255}, ra*0.88f));
                // skin texture scales
                for(int i = 0; i < 8; i++){
                    float sa = i*(2*PI/8.f) + ph;
                    DrawCircleV({sp.x + cosf(sa)*S(14.f), spY + S(4.f) + sinf(sa)*S(10.f)},
                        S(2.5f), ColorAlpha({8,25,50,255}, ra*0.60f));
                }
                // tail fin
                DrawTriangle({sp.x + S(22.f), spY + S(4.f)},
                    {sp.x + S(32.f), spY - S(8.f)},
                    {sp.x + S(32.f), spY + S(16.f)},
                    ColorAlpha({3,12,28,255}, ra*0.88f));
                DrawTriangleLines({sp.x + S(22.f), spY + S(4.f)},
                    {sp.x + S(32.f), spY - S(8.f)},
                    {sp.x + S(32.f), spY + S(16.f)},
                    ColorAlpha({0,150,160,255}, ra*0.30f));
                // pectoral fins
                for(int side = -1; side <= 1; side += 2){
                    DrawEllipse((int)(sp.x + side*S(6.f)), (int)(spY + S(14.f)),
                        (int)S(8.f), (int)S(4.f), ColorAlpha({4,14,30,255}, ra*0.80f));
                }
                // mouth
                DrawEllipse((int)(sp.x - S(14.f)), (int)spY, (int)S(10.f),
                    (int)(S(8.f) * mouthOpen), ColorAlpha({1,5,12,255}, ra*0.95f));
                // teeth
                for(int i = 0; i < 5; i++){
                    float tx2 = sp.x - S(22.f) + i*S(4.f);
                    DrawLineEx({tx2, spY - S(8.f*mouthOpen)}, {tx2, spY - S(8.f*mouthOpen) - S(5.f)},
                        S(1.5f), ColorAlpha({180,200,215,255}, ra*0.80f));
                    DrawLineEx({tx2, spY + S(8.f*mouthOpen)}, {tx2, spY + S(8.f*mouthOpen) + S(5.f)},
                        S(1.5f), ColorAlpha({180,200,215,255}, ra*0.80f));
                }
                // eye — wide, vacant
                DrawCircleV({sp.x - S(6.f), spY - S(6.f)}, S(7.f), ColorAlpha({3,10,24,255}, ra*0.92f));
                DrawCircleV({sp.x - S(6.f), spY - S(6.f)}, S(5.f), ColorAlpha({1,4,10,255}, ra*0.88f));
                DrawCircleV({sp.x - S(6.f), spY - S(6.f)}, S(2.f), ColorAlpha({0,180,160,255}, ra*0.60f));
                DrawCircleLinesV({sp.x - S(6.f), spY - S(6.f)}, S(7.f),
                    ColorAlpha({0,140,150,255}, ra*0.35f));
                // lure stalk
                Vector2 lureTip = {sp.x - S(10.f) + lureSway, spY - S(28.f)};
                DrawLineEx({sp.x - S(2.f), spY - S(16.f)},
                    {sp.x - S(6.f) + lureSway*.5f, spY - S(24.f)},
                    S(1.5f), ColorAlpha({4,14,30,255}, ra*0.85f));
                DrawLineEx({sp.x - S(6.f) + lureSway*.5f, spY - S(24.f)}, lureTip,
                    S(1.2f), ColorAlpha({4,14,30,255}, ra*0.80f));
                // lure bulb
                DrawCircleV(lureTip, S(5.f), ColorAlpha({0,180,160,255}, ra*lureGlow*0.85f));
                DrawCircleV(lureTip, S(3.f), ColorAlpha({0,230,200,255}, ra*lureGlow*0.90f));
                DrawCircleV(lureTip, S(1.5f), ColorAlpha(VP_WHITE, ra*lureGlow));
                DrawNeonCircle(lureTip, S(8.f), {0,200,180,255}, ra*lureGlow*0.60f);
                // lure glow halo
                for(int i = 0; i < 3; i++){
                    float la = fmodf(t*1.8f + i*0.33f + ph, 1.f);
                    DrawCircleLinesV(lureTip, S(5.f + la*20.f),
                        ColorAlpha({0,200,180,255}, ra*(1.f-la)*0.22f));
                }
            }
            else if(e.subType == 6){ // BIOLUME SQUID — squid with pulsing chromatophore body patterns
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float ink9=sinf(t*1.2f+ph);
                float chromAge=fmodf(t*2.f+ph,1.f);
                float hover9=sinf(t*0.9f+ph)*S(3.f);
                // mantle body — tapered oval
                DrawEllipse((int)sp.x,(int)(spY+hover9),(int)S(9.f),(int)S(18.f),ColorAlpha({2,8,20,255},ra*0.92f));
                DrawEllipse((int)sp.x,(int)(spY+hover9),(int)S(7.f),(int)S(15.f),ColorAlpha({5,18,40,255},ra*0.85f));
                // chromatophore pattern — expanding colour dots
                for(int i=0;i<8;i++){
                    float pa=ph+i*(2*PI/8.f);
                    float pr=S(5.f);
                    float cAge2=fmodf(chromAge+i*0.125f,1.f);
                    Color cCol=i%3==0?ColorAlpha({0,200,180,255},ra*cAge2*0.70f):
                               i%3==1?ColorAlpha({180,20,240,255},ra*cAge2*0.65f):
                                       ColorAlpha({255,100,20,255},ra*cAge2*0.60f);
                    DrawCircleV({sp.x+cosf(pa)*pr,spY+hover9+sinf(pa)*pr},S(2.f+cAge2*1.5f),cCol);
                }
                // lateral fins
                for(int side=-1;side<=1;side+=2){
                    DrawEllipse((int)(sp.x+side*S(9.f)),(int)(spY+hover9-S(5.f)),(int)S(6.f),(int)S(10.f),ColorAlpha({3,12,28,255},ra*0.80f));
                    DrawEllipseLines((int)(sp.x+side*S(9.f)),(int)(spY+hover9-S(5.f)),(int)S(6.f),(int)S(10.f),ColorAlpha({0,150,140,255},ra*0.35f));
                }
                // 10 tentacles from base
                for(int i=0;i<10;i++){
                    float ta=(i-4.5f)*0.18f;
                    int nseg=7;
                    Vector2 prev={sp.x+ta*S(7.f),spY+hover9+S(16.f)};
                    for(int seg2=0;seg2<nseg;seg2++){
                        float sf=(float)seg2/nseg;
                        float wave=sinf(t*2.5f+ph+i*0.6f+sf*PI)*S(4.f+sf*3.f);
                        Vector2 cur={prev.x+wave*0.08f,prev.y+S(5.f+sf)};
                        DrawLineEx(prev,cur,S(1.8f-sf*1.2f),ColorAlpha({4,15,32,255},ra*(1.f-sf*0.5f)*0.88f));
                        // sucker dots
                        if(seg2%2==0)
                            DrawCircleV(cur,S(1.f-sf*0.5f),ColorAlpha({0,160,150,255},ra*0.55f));
                        prev=cur;
                    }
                }
                // head — wide with big eyes
                DrawEllipse((int)sp.x,(int)(spY+hover9-S(12.f)),(int)S(10.f),(int)S(8.f),ColorAlpha({3,10,22,255},ra*0.92f));
                // eyes
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(5.f),spY+hover9-S(14.f)},S(5.f),ColorAlpha({1,4,10,255},ra*0.92f));
                    DrawCircleV({sp.x+side*S(5.f),spY+hover9-S(14.f)},S(3.5f),ColorAlpha({0,180,165,255},ra*0.70f));
                    DrawCircleV({sp.x+side*S(5.f),spY+hover9-S(14.f)},S(2.f),ColorAlpha({0,230,210,255},ra*0.80f));
                    DrawCircleV({sp.x+side*S(5.f),spY+hover9-S(14.f)},S(1.f),ColorAlpha(VP_WHITE,ra*0.90f));
                    DrawNeonCircle({sp.x+side*S(5.f),spY+hover9-S(14.f)},S(6.f),{0,200,185,255},ra*pulse*0.40f);
                }
                // ink cloud puff
                float inkT=fmodf(t*0.7f+ph,1.f);
                if(inkT<0.4f){
                    float it=inkT/0.4f;
                    DrawCircleV({sp.x+ink9*S(5.f),spY+hover9+S(22.f)},S(3.f+it*10.f),ColorAlpha({1,4,10,255},ra*(1.f-it)*0.45f));
                }
            }
            break;}

        case 10:{ // SHARD TEMPEST — glass meteor: a spinning asteroid of crystalline gold shards
            float cPulse = 0.7f + 0.3f*sinf(t*2.f + ph);
            if(e.subType < 5){
            float sSpin = t*0.7f+ph;
            // outer debris cloud
            for(int i = 0; i < 16; i++){
                float da = sSpin*0.4f + i*(2.f*PI/16.f);
                float dr = S(20.f+sinf(t*1.2f+i+ph)*4.f);
                DrawCircleV({sp.x+cosf(da)*dr,spY+sinf(da)*dr*0.7f},S(0.8f),
                    ColorAlpha(i%2==0?VP_GOLD:VP_YELLOW,ra*0.35f));
            }
            // main crystal body (jagged polygon)
            int nPts = 8;
            for(int i = 0; i < nPts; i++){
                float a1 = sSpin + i*(2.f*PI/nPts);
                float a2 = sSpin + (i+1)*(2.f*PI/nPts);
                float r1 = S(8.f + sinf(i*2.1f)*4.f);
                float r2 = S(8.f + sinf((i+1)*2.1f)*4.f);
                DrawTriangle({sp.x,spY},
                    {sp.x+cosf(a1)*r1,spY+sinf(a1)*r1},
                    {sp.x+cosf(a2)*r2,spY+sinf(a2)*r2},
                    ColorAlpha(i%2==0?VP_GOLD:VP_ORANGE,ra*0.80f));
            }
            // inner glow
            DrawCircleV({sp.x,spY},S(5.f),ColorAlpha(VP_WHITE,ra*0.35f));
            DrawNeonCircle({sp.x,spY},S(7.f),VP_GOLD,ra*0.55f);
            // lightning sparks between points
            for(int i = 0; i < nPts; i+=2){
                float a1 = sSpin + i*(2.f*PI/nPts);
                float r1 = S(8.f + sinf(i*2.1f)*4.f);
                float a2 = sSpin + (i+4)*(2.f*PI/nPts);
                float r2 = S(8.f + sinf((i+4)*2.1f)*4.f);
                if(((int)(t*14.f+i)%6)<3)
                    DrawNeonLine({sp.x+cosf(a1)*r1,spY+sinf(a1)*r1},
                                 {sp.x+cosf(a2)*r2,spY+sinf(a2)*r2},
                                 VP_WHITE,S(0.7f),ra*0.6f);
            }
            }
            else if(e.subType == 5){ // CRYSTAL GOLEM — golem of interlocking golden crystal shards
                float cRot = t * 0.6f + ph;
                // orbiting shard constellation
                for(int i = 0; i < 6; i++){
                    float oa = cRot + i*(2*PI/6.f);
                    float or2 = S(26.f);
                    float ox2 = sp.x + cosf(oa)*or2;
                    float oy2 = spY + sinf(oa)*or2*0.6f;
                    DrawRectanglePro({ox2, oy2, S(6.f), S(3.f)}, {S(3.f), S(1.5f)},
                        oa*RAD2DEG + 45.f, ColorAlpha({220,185,20,255}, ra*0.75f));
                    DrawNeonCircle({ox2, oy2}, S(4.f), VP_GOLD, ra*cPulse*0.35f);
                }
                // legs
                for(int side = -1; side <= 1; side += 2){
                    DrawRectangle((int)(sp.x + side*S(5.f)), (int)(spY + S(12.f)),
                        (int)S(8.f), (int)S(14.f), ColorAlpha({180,140,10,255}, ra*0.88f));
                    DrawRectangleLines((int)(sp.x + side*S(5.f)), (int)(spY + S(12.f)),
                        (int)S(8.f), (int)S(14.f), ColorAlpha({240,200,30,255}, ra*0.55f));
                    // crystal toe shards
                    for(int t2 = 0; t2 < 3; t2++)
                        DrawRectanglePro({sp.x + side*S(9.f) + t2*S(2.5f), spY + S(26.f),
                            S(3.f), S(6.f)}, {S(1.5f), 0},
                            side*15.f + t2*10.f, ColorAlpha({230,190,25,255}, ra*0.82f));
                }
                // body
                DrawRectangle((int)(sp.x - S(14.f)), (int)(spY - S(6.f)),
                    (int)S(28.f), (int)S(18.f), ColorAlpha({185,145,12,255}, ra*0.90f));
                DrawRectangleLines((int)(sp.x - S(14.f)), (int)(spY - S(6.f)),
                    (int)S(28.f), (int)S(18.f), ColorAlpha({245,205,35,255}, ra*0.55f));
                // chest crystal — big hexagonal gem
                DrawCircleV({sp.x, spY + S(3.f)}, S(7.f), ColorAlpha({225,182,18,255}, ra*0.88f));
                DrawCircleV({sp.x, spY + S(3.f)}, S(5.f), ColorAlpha({255,230,60,255}, ra*0.80f));
                DrawCircleV({sp.x, spY + S(3.f)}, S(2.5f), ColorAlpha(VP_WHITE, ra*cPulse));
                DrawNeonCircle({sp.x, spY + S(3.f)}, S(8.f), VP_GOLD, ra*cPulse*0.70f);
                // arms with crystal fists
                for(int side = -1; side <= 1; side += 2){
                    DrawRectangle((int)(sp.x + side*S(14.f)), (int)(spY - S(4.f)),
                        (int)S(10.f), (int)S(14.f), ColorAlpha({185,145,12,255}, ra*0.88f));
                    DrawRectangle((int)(sp.x + side*S(16.f)), (int)(spY + S(8.f)),
                        (int)S(8.f), (int)S(8.f), ColorAlpha({200,162,16,255}, ra*0.85f));
                    DrawNeonCircle({sp.x + side*S(20.f), spY + S(12.f)}, S(5.f),
                        VP_GOLD, ra*cPulse*0.40f);
                }
                // head
                DrawRectangle((int)(sp.x - S(9.f)), (int)(spY - S(18.f)),
                    (int)S(18.f), (int)S(12.f), ColorAlpha({195,155,14,255}, ra*0.90f));
                // eye gems
                for(int side = -1; side <= 1; side += 2){
                    DrawCircleV({sp.x + side*S(4.f), spY - S(13.f)}, S(3.f),
                        ColorAlpha({240,200,25,255}, ra*0.90f));
                    DrawCircleV({sp.x + side*S(4.f), spY - S(13.f)}, S(1.8f),
                        ColorAlpha(VP_WHITE, ra*cPulse));
                    DrawNeonCircle({sp.x + side*S(4.f), spY - S(13.f)}, S(4.f),
                        VP_GOLD, ra*cPulse*0.60f);
                }
                // crown shards
                for(int i = 0; i < 5; i++){
                    float ca2 = -PI*.5f + (i-2)*0.25f;
                    DrawRectanglePro({sp.x + (i-2)*S(4.f), spY - S(18.f),
                        S(3.f), S(6.f + i%2*3.f)}, {S(1.5f), S(6.f + i%2*3.f)},
                        ca2*RAD2DEG, ColorAlpha({230,190,22,255}, ra*0.80f));
                }
            }
            else if(e.subType == 6){ // PRISM PHOENIX — a phoenix built entirely of fractured golden light
                float wingFan10=sinf(t*2.f+ph);
                float prismAge=fmodf(t*1.5f+ph,1.f);
                float shimmer10=0.7f+0.3f*sinf(t*4.f+ph);
                // prismatic light scatter — refracted colour shards drifting
                for(int i=0;i<8;i++){
                    float pa=prismAge*2*PI+i*(2*PI/8.f);
                    float pr=S(18.f+i*2.f);
                    Color pc=i%4==0?ColorAlpha({255,230,50,255},ra*shimmer10*0.45f):
                             i%4==1?ColorAlpha({255,160,20,255},ra*shimmer10*0.40f):
                             i%4==2?ColorAlpha({200,255,100,255},ra*shimmer10*0.35f):
                                     ColorAlpha({255,200,60,255},ra*shimmer10*0.38f);
                    DrawLineEx({sp.x+cosf(pa)*S(10.f),spY+sinf(pa)*S(8.f)},{sp.x+cosf(pa)*pr,spY+sinf(pa)*pr*0.7f},S(1.2f),pc);
                    DrawCircleV({sp.x+cosf(pa)*pr,spY+sinf(pa)*pr*0.7f},S(2.f),pc);
                }
                // wings — large crystalline fan
                for(int side=-1;side<=1;side+=2){
                    for(int wi=0;wi<5;wi++){
                        float wa=side*(0.25f+wi*0.18f)*wingFan10*1.2f;
                        float wl=S(18.f+wi*4.f);
                        float wax=sp.x+side*S(6.f);
                        DrawLineEx({wax,spY-S(2.f)},{wax+cosf(wa+side*PI*.35f)*wl,spY-S(2.f)+sinf(wa+side*PI*.35f)*wl},S(2.f-wi*0.2f),ColorAlpha({220,185,18,255},ra*0.80f));
                        DrawCircleV({wax+cosf(wa+side*PI*.35f)*wl,spY-S(2.f)+sinf(wa+side*PI*.35f)*wl},S(2.5f),ColorAlpha({255,235,80,255},ra*shimmer10*0.70f));
                        DrawNeonCircle({wax+cosf(wa+side*PI*.35f)*wl,spY-S(2.f)+sinf(wa+side*PI*.35f)*wl},S(4.f),VP_GOLD,ra*shimmer10*0.38f);
                    }
                    // wing membrane fill
                    DrawEllipse((int)(sp.x+side*S(14.f)),(int)(spY-S(6.f)),(int)S(12.f),(int)S(8.f),ColorAlpha({220,185,18,255},ra*0.14f));
                }
                // tail fan — 7 crystal feathers
                for(int i=0;i<7;i++){
                    float fa=(i-3)*0.18f+PI*.5f;
                    float fl=S(16.f+i%2*5.f);
                    DrawLineEx({sp.x,spY+S(10.f)},{sp.x+cosf(fa)*fl,spY+S(10.f)+sinf(fa)*fl},S(1.5f),ColorAlpha({230,192,22,255},ra*0.75f));
                    DrawCircleV({sp.x+cosf(fa)*fl,spY+S(10.f)+sinf(fa)*fl},S(2.f),ColorAlpha({255,238,80,255},ra*shimmer10*0.72f));
                }
                // body
                DrawEllipse((int)sp.x,(int)spY,(int)S(8.f),(int)S(11.f),ColorAlpha({195,155,12,255},ra*0.90f));
                DrawEllipse((int)sp.x,(int)spY,(int)S(6.f),(int)S(9.f),ColorAlpha({240,210,40,255},ra*0.82f));
                DrawNeonCircle({sp.x,spY},S(8.f),VP_GOLD,ra*cPulse*0.55f);
                // head + crest
                DrawCircleV({sp.x,spY-S(13.f)},S(7.f),ColorAlpha({200,158,15,255},ra*0.90f));
                DrawCircleV({sp.x,spY-S(13.f)},S(5.f),ColorAlpha({255,225,55,255},ra*0.82f));
                DrawNeonCircle({sp.x,spY-S(13.f)},S(8.f),VP_GOLD,ra*cPulse*0.60f);
                // crest spikes
                for(int i=0;i<5;i++){
                    float ca=(i-2)*0.25f-PI*.5f;
                    DrawRectanglePro({sp.x+(i-2)*S(3.5f),spY-S(20.f),S(2.5f),S(6.f+i%2*3.f)},{S(1.25f),S(6.f+i%2*3.f)},0.f,ColorAlpha({240,208,30,255},ra*0.80f));
                }
                // eye
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(3.5f),spY-S(14.f)},S(2.5f),ColorAlpha({15,10,1,255},ra*0.95f));
                    DrawCircleV({sp.x+side*S(3.5f),spY-S(14.f)},S(1.4f),ColorAlpha({255,220,40,255},ra*shimmer10*0.90f));
                    DrawNeonCircle({sp.x+side*S(3.5f),spY-S(14.f)},S(3.5f),VP_GOLD,ra*shimmer10*0.60f);
                }
            }
            break;}

        case 11:{ // ASHEN CITADEL — ash soldier: petrified warrior still standing at their post
            float lPulse = 0.5f+0.5f*sinf(t*1.5f+ph);
            if(e.subType < 5){
            float aBob = sinf(t*0.3f+ph)*S(0.5f); // barely moves — almost stone
            float ashDrift = fmodf(t*0.5f+ph, 1.f);
            // ash/embers drifting off body
            for(int i = 0; i < 10; i++){
                float ep = fmodf(t*0.6f + i*0.18f + ph, 2.5f);
                float ex2 = sp.x + S(-10.f+i*2.2f) + sinf(i*1.4f+t*0.4f)*S(3.f);
                float ey2 = spY + S(5.f) - ep*S(8.f);
                Color ec = i%3==0?VP_GOLD:i%3==1?VP_ORANGE:VP_OFFWHITE;
                DrawCircleV({ex2,ey2},S(0.7f+i*0.1f),
                    ColorAlpha(ec,ra*(1.f-ep/2.5f)*0.55f));
            }
            // legs
            DrawRectangle((int)(sp.x-S(6.f)),(int)(spY+S(10.f)+aBob),(int)S(4.f),(int)S(11.f),
                ColorAlpha({55,45,35,255},ra*0.88f));
            DrawRectangle((int)(sp.x+S(2.f)),(int)(spY+S(10.f)+aBob),(int)S(4.f),(int)S(11.f),
                ColorAlpha({55,45,35,255},ra*0.88f));
            // torso + breastplate
            DrawRectangle((int)(sp.x-S(8.f)),(int)(spY-S(3.f)+aBob),(int)S(16.f),(int)S(13.f),
                ColorAlpha({50,40,30,255},ra*0.90f));
            DrawRectangle((int)(sp.x-S(7.f)),(int)(spY-S(2.f)+aBob),(int)S(14.f),(int)S(10.f),
                ColorAlpha({70,55,40,255},ra*0.55f));
            // arms (holding spear)
            DrawRectangle((int)(sp.x-S(12.f)),(int)(spY-S(2.f)+aBob),(int)S(4.f),(int)S(10.f),
                ColorAlpha({52,42,32,255},ra*0.80f));
            DrawRectangle((int)(sp.x+S(8.f)),(int)(spY-S(2.f)+aBob),(int)S(4.f),(int)S(10.f),
                ColorAlpha({52,42,32,255},ra*0.80f));
            // spear
            DrawLineEx({sp.x+S(10.f),spY+S(8.f)+aBob},{sp.x+S(10.f),spY-S(25.f)+aBob},
                S(1.2f),ColorAlpha({80,65,45,255},ra*0.85f));
            DrawTriangle({sp.x+S(8.f),spY-S(20.f)+aBob},{sp.x+S(12.f),spY-S(20.f)+aBob},
                {sp.x+S(10.f),spY-S(27.f)+aBob},ColorAlpha(VP_GOLD,ra*0.7f));
            // head (helm with plume)
            DrawEllipse((int)sp.x,(int)(spY-S(9.f)+aBob),(int)S(7.f),(int)S(6.f),
                ColorAlpha({48,38,28,255},ra*0.92f));
            DrawRectangle((int)(sp.x-S(7.f)),(int)(spY-S(9.f)+aBob),(int)S(14.f),(int)S(2.f),
                ColorAlpha({65,50,35,255},ra*0.85f)); // visor
            // plume (smouldering)
            for(int i = 0; i < 4; i++){
                float pa = -PI/2.f + (i-1.5f)*0.25f;
                float pLen = S(5.f+sinf(t*2.f+i*0.5f+ph)*2.f);
                DrawNeonLine({sp.x,spY-S(14.f)+aBob},{sp.x+cosf(pa)*pLen,spY-S(14.f)+sinf(pa)*pLen+aBob},
                    VP_ORANGE,S(1.f),ra*0.45f);
            }
            }
            else if(e.subType == 5){ // CINDER DRAKE — small dragon woven from embers and ash
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float drakeFlap = sinf(t*3.5f + ph);
                float emberAge  = fmodf(t*2.f + ph, 1.f);
                // ember particle trail drifting upward
                for(int i = 0; i < 8; i++){
                    float ea = fmodf(t*1.5f + i*0.125f + ph, 1.f);
                    float ex2 = sp.x + (i-4)*S(3.f) + sinf(t*2.f + i)*S(2.f);
                    float ey2 = spY + S(14.f) - ea*S(30.f);
                    DrawCircleV({ex2, ey2}, S(1.8f*(1.f-ea)),
                        ColorAlpha(ea<0.4f?VP_ORANGE:ea<0.7f?VP_RED:Color{80,30,5,255}, ra*(1.f-ea)*0.65f));
                    if(ea < 0.3f)
                        DrawNeonCircle({ex2, ey2}, S(3.f), VP_ORANGE, ra*(1.f-ea)*0.30f);
                }
                // wings — ragged ash membrane
                for(int side = -1; side <= 1; side += 2){
                    float wFlex = drakeFlap * side * S(5.f);
                    Vector2 wShoulder = {sp.x + side*S(5.f), spY - S(3.f)};
                    Vector2 wTip      = {sp.x + side*S(20.f), spY - S(14.f) + wFlex};
                    Vector2 wBase     = {sp.x + side*S(14.f), spY + S(4.f)};
                    DrawTriangle(wShoulder, wTip, wBase,
                        ColorAlpha({42,18,5,255}, ra*0.82f));
                    DrawTriangleLines(wShoulder, wTip, wBase,
                        ColorAlpha(VP_ORANGE, ra*0.30f));
                    // bone spars in wings
                    for(int b = 1; b <= 2; b++){
                        float bt = (float)b/3.f;
                        Vector2 bp = {wShoulder.x + (wTip.x-wShoulder.x)*bt,
                                      wShoulder.y + (wTip.y-wShoulder.y)*bt};
                        DrawLineEx(wShoulder, bp, S(0.8f), ColorAlpha({60,25,7,255}, ra*0.45f));
                    }
                }
                // tail
                for(int i = 0; i < 5; i++){
                    float fi = (float)i/5.f;
                    float tx2 = sp.x - S(8.f) - i*S(4.5f);
                    float ty2 = spY + S(5.f) + sinf(t*1.8f + ph + fi*PI)*S(3.f)*fi;
                    DrawCircleV({tx2, ty2}, S(4.5f - fi*2.f), ColorAlpha({38,15,4,255}, ra*0.88f));
                    // tail tip flame
                    if(i == 4){
                        DrawCircleV({tx2, ty2}, S(3.f), ColorAlpha(VP_ORANGE, ra*0.75f*pulse));
                        DrawNeonCircle({tx2, ty2}, S(5.f), VP_RED, ra*pulse*0.45f);
                    }
                }
                // body
                DrawEllipse((int)sp.x, (int)spY, (int)S(9.f), (int)S(7.f), ColorAlpha({40,16,4,255}, ra*0.92f));
                DrawEllipse((int)sp.x, (int)spY, (int)S(7.5f), (int)S(5.5f), ColorAlpha({65,24,6,255}, ra*0.85f));
                // belly glow — ember seams
                for(int i = 0; i < 3; i++)
                    DrawLineEx({sp.x - S(5.f) + i*S(5.f), spY + S(3.f)},
                        {sp.x - S(3.f) + i*S(4.f), spY + S(6.f)},
                        S(1.f), ColorAlpha(VP_ORANGE, ra*pulse*0.60f));
                DrawNeonCircle({sp.x, spY + S(2.f)}, S(5.f), VP_RED, ra*pulse*0.28f);
                // neck + head
                DrawEllipse((int)(sp.x + S(9.f)), (int)(spY - S(4.f)), (int)S(5.f), (int)S(4.f),
                    ColorAlpha({42,16,4,255}, ra*0.90f));
                DrawEllipse((int)(sp.x + S(16.f)), (int)(spY - S(8.f)), (int)S(7.f), (int)S(6.f),
                    ColorAlpha({40,16,4,255}, ra*0.92f));
                // eye
                DrawCircleV({sp.x + S(19.f), spY - S(10.f)}, S(2.5f), ColorAlpha({5,2,0,255}, ra*0.95f));
                DrawCircleV({sp.x + S(19.f), spY - S(10.f)}, S(1.5f), ColorAlpha(VP_ORANGE, ra*0.90f));
                DrawNeonCircle({sp.x + S(19.f), spY - S(10.f)}, S(3.5f), VP_RED, ra*pulse*0.55f);
                // mouth — small flame
                for(int i = 0; i < 4; i++){
                    float fa2 = fmodf(t*3.f + i*0.25f + ph, 1.f);
                    DrawCircleV({sp.x + S(22.f + fa2*8.f), spY - S(8.f)},
                        S(1.5f + fa2*2.f), ColorAlpha(fa2<0.5f?VP_YELLOW:VP_ORANGE, ra*(1.f-fa2)*0.75f));
                }
                // horns
                for(int side = -1; side <= 1; side += 2){
                    DrawLineEx({sp.x + S(14.f) + side*S(3.f), spY - S(13.f)},
                        {sp.x + S(12.f) + side*S(7.f), spY - S(20.f)},
                        S(1.8f), ColorAlpha({52,20,5,255}, ra*0.80f));
                    DrawCircleV({sp.x + S(12.f) + side*S(7.f), spY - S(20.f)},
                        S(1.2f), ColorAlpha(VP_ORANGE, ra*0.7f));
                }
                // legs
                for(int side = -1; side <= 1; side += 2){
                    DrawLineEx({sp.x + side*S(5.f), spY + S(6.f)},
                        {sp.x + side*S(9.f), spY + S(14.f)}, S(3.f), ColorAlpha({38,15,4,255}, ra*0.85f));
                    DrawCircleV({sp.x + side*S(9.f), spY + S(14.f)}, S(3.f), ColorAlpha({45,18,4,255}, ra*0.82f));
                }
            }
            else if(e.subType == 6){ // BONE CHARIOT — skeletal knight on a charred undead horse
                float gallop=sinf(t*3.5f+ph);
                float cloakBill=sinf(t*1.8f+ph)*S(3.f);
                float emberA=fmodf(t*2.2f+ph,1.f);
                // shadow
                DrawEllipse((int)sp.x,(int)(spY+S(30.f)),(int)S(24.f),(int)S(5.f),ColorAlpha(VP_BLACK,ra*0.20f));
                // horse legs — four galloping
                float legPhase[]={0.f,PI,PI*.5f,PI*1.5f};
                float legX[]={-10.f,-6.f,4.f,8.f};
                for(int i=0;i<4;i++){
                    float lg=gallop*((i<2)?1.f:-1.f);
                    float lx=sp.x+S(legX[i]);
                    DrawLineEx({lx,spY+S(14.f)},{lx+lg*S(3.f),spY+S(22.f)},S(3.f),ColorAlpha({35,30,28,255},ra*0.88f));
                    DrawLineEx({lx+lg*S(3.f),spY+S(22.f)},{lx+lg*S(2.f),spY+S(30.f)},S(2.5f),ColorAlpha({30,26,24,255},ra*0.85f));
                    DrawCircleV({lx+lg*S(2.f),spY+S(30.f)},S(2.5f),ColorAlpha({25,22,20,255},ra*0.82f));
                }
                // horse body
                DrawEllipse((int)sp.x,(int)(spY+S(9.f)),(int)S(20.f),(int)S(10.f),ColorAlpha({32,28,26,255},ra*0.90f));
                DrawEllipse((int)sp.x,(int)(spY+S(9.f)),(int)S(18.f),(int)S(8.f),ColorAlpha({50,44,40,255},ra*0.82f));
                // ember ribs
                for(int i=0;i<5;i++){
                    float rx=sp.x-S(14.f)+i*S(7.f);
                    DrawLineEx({rx,spY+S(4.f)},{rx+S(2.f),spY+S(14.f)},S(1.f),ColorAlpha({90,25,4,255},ra*0.60f));
                    DrawNeonCircle({rx+S(1.f),spY+S(9.f)},S(2.f),VP_RED,ra*lPulse*0.25f);
                }
                // horse neck+head
                DrawRectangle((int)(sp.x-S(16.f)),(int)(spY-S(4.f)),(int)S(8.f),(int)S(14.f),ColorAlpha({34,30,28,255},ra*0.88f));
                DrawEllipse((int)(sp.x-S(20.f)),(int)(spY-S(8.f)),(int)S(8.f),(int)S(7.f),ColorAlpha({35,30,28,255},ra*0.90f));
                // horse jaw glow
                DrawNeonCircle({sp.x-S(22.f),spY-S(6.f)},S(6.f),VP_ORANGE,ra*lPulse*0.30f);
                // horse eyes
                DrawCircleV({sp.x-S(22.f),spY-S(10.f)},S(2.f),ColorAlpha({5,2,0,255},ra*0.95f));
                DrawCircleV({sp.x-S(22.f),spY-S(10.f)},S(1.2f),ColorAlpha(VP_ORANGE,ra*0.90f));
                // rider — skeletal knight torso
                DrawRectangle((int)(sp.x-S(6.f)),(int)(spY-S(12.f)),(int)S(12.f),(int)S(14.f),ColorAlpha({30,26,24,255},ra*0.90f));
                DrawRectangle((int)(sp.x-S(5.f)),(int)(spY-S(11.f)),(int)S(10.f),(int)S(12.f),ColorAlpha({50,44,40,255},ra*0.82f));
                // shoulder pauldrons
                for(int side=-1;side<=1;side+=2)
                    DrawEllipse((int)(sp.x+side*S(8.f)),(int)(spY-S(10.f)),(int)S(5.f),(int)S(4.f),ColorAlpha({42,36,34,255},ra*0.88f));
                // lance arm
                DrawLineEx({sp.x+S(5.f),spY-S(8.f)},{sp.x+S(20.f)+cloakBill,spY-S(14.f)},S(3.f),ColorAlpha({35,30,28,255},ra*0.85f));
                DrawLineEx({sp.x+S(20.f)+cloakBill,spY-S(14.f)},{sp.x+S(30.f)+cloakBill,spY-S(16.f)},S(1.5f),ColorAlpha({60,55,50,255},ra*0.80f));
                DrawRectanglePro({sp.x+S(30.f)+cloakBill,spY-S(16.f),S(2.f),S(8.f)},{S(1.f),0.f},15.f,ColorAlpha({70,65,55,255},ra*0.85f));
                // skull helmet
                DrawCircleV({sp.x,spY-S(20.f)},S(6.5f),ColorAlpha({42,38,34,255},ra*0.92f));
                DrawCircleV({sp.x,spY-S(20.f)},S(5.f),ColorAlpha({62,56,50,255},ra*0.85f));
                // skull eyes glowing
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(2.5f),spY-S(21.f)},S(2.f),ColorAlpha({5,2,0,255},ra*0.95f));
                    DrawCircleV({sp.x+side*S(2.5f),spY-S(21.f)},S(1.2f),ColorAlpha(VP_ORANGE,ra*lPulse*0.90f));
                    DrawNeonCircle({sp.x+side*S(2.5f),spY-S(21.f)},S(3.f),VP_RED,ra*lPulse*0.55f);
                }
                // cloak billowing
                for(int i=0;i<4;i++){
                    float ce=fmodf(t*1.2f+i*0.25f+ph,1.f);
                    DrawLineEx({sp.x+S(4.f),spY-S(12.f)},{sp.x+S(4.f)+(i-1.5f)*S(4.f)+cloakBill,spY+S(2.f)+ce*S(10.f)},S(2.f-i*0.3f),ColorAlpha({25,20,18,255},ra*(1.f-ce)*0.55f));
                }
                // ember trail from hooves
                for(int i=0;i<5;i++){
                    float ea2=fmodf(emberA+i*0.2f,1.f);
                    DrawCircleV({sp.x-S(14.f)-ea2*S(14.f)+(i-2)*S(3.f),spY+S(28.f)+gallop*S(2.f)},S(1.5f*(1.f-ea2)),ColorAlpha(ea2<0.4f?VP_ORANGE:VP_RED,ra*(1.f-ea2)*0.55f));
                }
            }
            break;}

        case 12:{ // VOID RIFT — shadow moth: a vast insect of pure darkness, wings shimmer with rifts
            if(e.subType < 5){
            float wFlap      = sinf(t*2.2f+ph);
            float wSpan      = S(28.f);
            float antennaPh  = sinf(t*1.8f+ph);
            float riftShimmer= 0.50f+0.35f*sinf(t*3.5f+ph);
            float dustAge    = fmodf(t*1.2f+ph, 1.f);
            float wH         = S(16.f) + wFlap*S(9.f);
            // — void dust drifting down from wings —
            for(int i=0;i<5;i++){
                float da=fmodf(dustAge+i*0.20f,1.f);
                DrawCircleV({sp.x+(i-2)*S(9.f)+sinf(ph+i)*S(2.f), spY+S(14.f)+da*S(22.f)},
                    S(2.f*(1.f-da)), ColorAlpha({160,40,240,255},ra*(1.f-da)*0.45f));
            }
            // — upper wings: dark violet fill with bright purple edge —
            Color wingFill  = {45,8,90,255};   // deep violet — readable on dark bg
            Color wingEdge  = {160,60,255,255}; // bright purple outline
            DrawTriangle({sp.x,spY},{sp.x-wSpan,spY-wH},{sp.x-S(6.f),spY+S(5.f)},
                ColorAlpha(wingFill,ra*0.92f));
            DrawTriangle({sp.x,spY},{sp.x+wSpan,spY-wH},{sp.x+S(6.f),spY+S(5.f)},
                ColorAlpha(wingFill,ra*0.92f));
            DrawTriangleLines({sp.x,spY},{sp.x-wSpan,spY-wH},{sp.x-S(6.f),spY+S(5.f)},
                ColorAlpha(wingEdge,ra*0.75f));
            DrawTriangleLines({sp.x,spY},{sp.x+wSpan,spY-wH},{sp.x+S(6.f),spY+S(5.f)},
                ColorAlpha(wingEdge,ra*0.75f));
            // — wing rift tears: glowing iridescent slashes —
            for(int side=-1;side<=1;side+=2){
                for(int v=1;v<=3;v++){
                    float vt=(float)v/4.f;
                    float vx=sp.x+side*(wSpan*vt);
                    float vy=spY-wH*vt+S(4.f);
                    DrawLineEx({sp.x+side*S(4.f),spY+S(2.f)},{vx,vy},
                        S(1.8f),ColorAlpha({130,40,220,255},ra*0.55f));
                }
            }
            // — large eyespot on each upper wing —
            for(int side=-1;side<=1;side+=2){
                float ex=sp.x+side*S(16.f), ey=spY-S(10.f)+wFlap*side*S(2.f);
                DrawCircleV({ex,ey},S(6.5f), ColorAlpha({5,0,12,255},   ra*0.92f));
                DrawCircleV({ex,ey},S(5.f),  ColorAlpha({90,15,160,255}, ra*0.85f));
                DrawCircleV({ex,ey},S(3.f),  ColorAlpha({200,60,255,255},ra*riftShimmer*0.90f));
                DrawCircleV({ex,ey},S(1.2f), ColorAlpha(VP_WHITE,        ra*riftShimmer));
                DrawNeonCircle({ex,ey},S(6.f),VP_PURPLE,ra*riftShimmer*0.65f);
            }
            // — lower wings —
            Color lwFill={35,6,70,255};
            DrawTriangle({sp.x,spY+S(2.f)},{sp.x-S(18.f),spY+S(10.f)-wFlap*S(3.f)},{sp.x-S(4.f),spY+S(14.f)},
                ColorAlpha(lwFill,ra*0.88f));
            DrawTriangle({sp.x,spY+S(2.f)},{sp.x+S(18.f),spY+S(10.f)-wFlap*S(3.f)},{sp.x+S(4.f),spY+S(14.f)},
                ColorAlpha(lwFill,ra*0.88f));
            DrawTriangleLines({sp.x,spY+S(2.f)},{sp.x-S(18.f),spY+S(10.f)-wFlap*S(3.f)},{sp.x-S(4.f),spY+S(14.f)},
                ColorAlpha(wingEdge,ra*0.50f));
            DrawTriangleLines({sp.x,spY+S(2.f)},{sp.x+S(18.f),spY+S(10.f)-wFlap*S(3.f)},{sp.x+S(4.f),spY+S(14.f)},
                ColorAlpha(wingEdge,ra*0.50f));
            // lower eyespots
            for(int side=-1;side<=1;side+=2){
                float lx=sp.x+side*S(10.f), ly=spY+S(8.f)-wFlap*side*S(1.5f);
                DrawCircleV({lx,ly},S(3.5f),ColorAlpha({5,0,10,255},        ra*0.90f));
                DrawCircleV({lx,ly},S(2.f), ColorAlpha({170,40,240,255},    ra*0.80f));
                DrawCircleV({lx,ly},S(0.8f),ColorAlpha(VP_WHITE,            ra*riftShimmer));
            }
            // — thorax body —
            DrawEllipse((int)sp.x,(int)spY,(int)S(7.f),(int)S(9.f),
                ColorAlpha({60,12,100,255},ra*0.96f));
            DrawEllipse((int)sp.x,(int)spY,(int)S(5.f),(int)S(7.f),
                ColorAlpha({90,20,150,255},ra*0.88f));
            DrawNeonCircle({sp.x,spY},S(7.f),VP_PURPLE,ra*0.40f);
            // — abdomen: visible tapered segments —
            for(int i=0;i<5;i++){
                float fi=(float)i/5.f;
                float ay=spY+S(7.f)+i*S(3.8f);
                float ar=S(4.5f-fi*2.8f);
                DrawCircleV({sp.x,ay},ar, ColorAlpha({55,10,95,255}, ra*0.90f));
                DrawCircleLinesV({sp.x,ay},ar,ColorAlpha({120,30,200,255},ra*0.55f));
            }
            // — compound eyes (large, glowing) —
            for(int side=-1;side<=1;side+=2){
                float ex2=sp.x+side*S(4.f), ey2=spY-S(7.f);
                DrawCircleV({ex2,ey2},S(3.5f),ColorAlpha({80,0,160,255},ra*0.95f));
                DrawCircleV({ex2,ey2},S(2.f), ColorAlpha({220,80,255,255},ra*riftShimmer));
                DrawNeonCircle({ex2,ey2},S(4.f),VP_PURPLE,ra*riftShimmer*0.70f);
            }
            // — antennae: thick enough to see —
            for(int side=-1;side<=1;side+=2){
                float ax1=sp.x+side*S(3.f), ay1=spY-S(9.f);
                float ax2=ax1+side*S(7.f)+antennaPh*side*S(3.f), ay2=ay1-S(10.f);
                float ax3=ax2+side*S(4.f), ay3=ay2-S(8.f);
                DrawLineEx({ax1,ay1},{ax2,ay2},S(2.0f),ColorAlpha({150,40,230,255},ra*0.80f));
                DrawLineEx({ax2,ay2},{ax3,ay3},S(1.5f),ColorAlpha({150,40,230,255},ra*0.65f));
                DrawNeonCircle({ax3,ay3},S(3.f),VP_PURPLE,ra*riftShimmer*0.85f);
                DrawCircleV({ax3,ay3},S(1.5f),ColorAlpha(VP_WHITE,ra*riftShimmer*0.80f));
            }
            }
            else if(e.subType == 5){ // VOID KRAKEN — many-tentacled void creature trailing rifts
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float kRot = t * 0.4f + ph;
                float riftAge = fmodf(t * 1.2f + ph, 1.f);
                // rift tears expanding from body
                for(int i = 0; i < 3; i++){
                    float ra2 = fmodf(riftAge + i*0.33f, 1.f);
                    DrawEllipse((int)sp.x, (int)spY, (int)(S(8.f + ra2*35.f)), (int)(S(4.f + ra2*18.f)),
                        ColorAlpha({45,6,85,255}, ra*(1.f-ra2)*0.18f));
                    DrawEllipseLines((int)sp.x, (int)spY, (int)(S(8.f + ra2*35.f)), (int)(S(4.f + ra2*18.f)),
                        ColorAlpha(VP_PURPLE, ra*(1.f-ra2)*0.30f));
                }
                // 8 writhing tentacles
                for(int i = 0; i < 8; i++){
                    float baseA = kRot + i*(2*PI/8.f);
                    float waveA = sinf(t*1.8f + ph + i*0.7f);
                    Vector2 base = {sp.x + cosf(baseA)*S(10.f), spY + sinf(baseA)*S(6.f)};
                    // 6 segments per tentacle
                    Vector2 prev = base;
                    for(int seg2 = 0; seg2 < 6; seg2++){
                        float sf = (float)seg2/6.f;
                        float sa = baseA + waveA*(sf*0.8f + 0.2f);
                        float sr = S(14.f + sf*8.f);
                        Vector2 cur = {prev.x + cosf(sa)*S(5.f + sf*2.f),
                                       prev.y + sinf(sa)*S(3.f + sf)};
                        DrawLineEx(prev, cur, S(3.5f - sf*2.5f),
                            ColorAlpha({50,8,90,255}, ra*(1.f - sf*0.4f)));
                        DrawLineEx(prev, cur, S(2.f - sf*1.5f),
                            ColorAlpha({120,40,200,255}, ra*(0.6f - sf*0.3f)));
                        // sucker dots
                        if(seg2 % 2 == 0)
                            DrawCircleV(cur, S(1.5f - sf*0.8f),
                                ColorAlpha({80,10,140,255}, ra*0.65f));
                        prev = cur;
                    }
                    // tentacle tip glow
                    DrawNeonCircle(prev, S(2.5f), VP_PURPLE, ra*pulse*0.50f);
                }
                // central mantle body
                DrawCircleV({sp.x, spY}, S(11.f), ColorAlpha({30,4,58,255}, ra*0.92f));
                DrawCircleV({sp.x, spY}, S(8.f),  ColorAlpha({55,8,100,255}, ra*0.85f));
                DrawCircleV({sp.x, spY}, S(5.f),  ColorAlpha({80,12,145,255}, ra*0.78f));
                DrawNeonCircle({sp.x, spY}, S(11.f), VP_PURPLE, ra*0.30f*pulse);
                // eyes — 3 clustered
                for(int i = 0; i < 3; i++){
                    float ea = (i-1)*0.5f;
                    DrawCircleV({sp.x + cosf(ea)*S(4.f), spY + sinf(ea)*S(2.5f)},
                        S(2.2f), ColorAlpha({5,0,10,255}, ra*0.95f));
                    DrawCircleV({sp.x + cosf(ea)*S(4.f), spY + sinf(ea)*S(2.5f)},
                        S(1.2f), ColorAlpha({200,80,255,255}, ra*0.90f));
                    DrawNeonCircle({sp.x + cosf(ea)*S(4.f), spY + sinf(ea)*S(2.5f)},
                        S(3.f), VP_PURPLE, ra*pulse*0.55f);
                }
            }
            else if(e.subType == 6){ // VOID SERPENT — colossal serpent woven from void-space itself
                float pulse   = 0.55f+0.45f*sinf(t*1.8f+ph);
                float coilSpd = t*0.35f+ph;
                float riftAge = fmodf(t*0.9f+ph,1.f);
                int seg=18;
                // rift tears left in the serpent's wake
                for(int i=0;i<4;i++){
                    float ta=fmodf(riftAge+i*0.25f,1.f);
                    float rx2=sp.x+S(10.f)+ta*S(20.f)+(i-1.5f)*S(10.f);
                    float ry2=spY+sinf(coilSpd*0.5f+i)*S(8.f);
                    DrawEllipse((int)rx2,(int)ry2,(int)(S(5.f+ta*10.f)),(int)(S(2.5f+ta*5.f)),
                        ColorAlpha({80,10,140,255},ra*(1.f-ta)*0.22f));
                    DrawEllipseLines((int)rx2,(int)ry2,(int)(S(5.f+ta*10.f)),(int)(S(2.5f+ta*5.f)),
                        ColorAlpha(VP_PURPLE,ra*(1.f-ta)*0.45f));
                }
                // body segments tail-to-head — coiling in slow spiral
                for(int i=seg-1;i>=0;i--){
                    float fi=(float)i/seg;
                    float sx2=sp.x+sinf(coilSpd+fi*2.8f)*S(22.f-fi*8.f);
                    float sy2=spY+cosf(coilSpd*0.6f+fi*2.f)*S(14.f);
                    float sr=S(9.f-fi*6.f);
                    // outer void segment — deep purple-black
                    DrawCircleV({sx2,sy2},sr,ColorAlpha({6,1,14,255},ra*0.95f));
                    DrawCircleV({sx2,sy2},sr*0.72f,ColorAlpha({30,4,60,255},ra*0.88f));
                    DrawCircleLinesV({sx2,sy2},sr,ColorAlpha({100,20,180,255},ra*0.45f));
                    // void-crack fissure glowing through each segment
                    if(i%3==0){
                        DrawNeonCircle({sx2,sy2},sr*0.5f,VP_PURPLE,ra*pulse*0.40f);
                        DrawCircleV({sx2,sy2},sr*0.28f,ColorAlpha({200,80,255,255},ra*pulse*0.65f));
                    }
                }
                // head — large blunt angular void-skull
                float hx=sp.x+sinf(coilSpd)*S(22.f);
                float hy=spY+cosf(coilSpd*0.6f)*S(14.f);
                DrawEllipse((int)hx,(int)hy,(int)S(13.f),(int)S(10.f),ColorAlpha({4,0,10,255},ra*0.96f));
                DrawEllipse((int)hx,(int)hy,(int)S(11.f),(int)S(8.f), ColorAlpha({18,2,38,255},ra*0.90f));
                DrawEllipseLines((int)hx,(int)hy,(int)S(13.f),(int)S(10.f),ColorAlpha({120,25,210,255},ra*0.50f));
                // crown of void-spines arcing back from head
                for(int i=0;i<5;i++){
                    float sa=(i-2)*0.28f-PI*0.5f;
                    float sl=S(8.f+i%2*5.f);
                    DrawLineEx({hx,hy},{hx+cosf(sa)*sl,hy+sinf(sa)*sl},
                        S(2.0f),ColorAlpha({8,1,18,255},ra*0.88f));
                    DrawCircleV({hx+cosf(sa)*sl,hy+sinf(sa)*sl},
                        S(1.8f),ColorAlpha({180,50,255,255},ra*pulse*0.70f));
                    DrawNeonCircle({hx+cosf(sa)*sl,hy+sinf(sa)*sl},S(3.f),VP_PURPLE,ra*pulse*0.40f);
                }
                // glowing void eyes — large, twin
                for(int side=-1;side<=1;side+=2){
                    float ex2=hx+side*S(5.f), ey2=hy-S(2.f);
                    DrawCircleV({ex2,ey2},S(4.f),  ColorAlpha({3,0,7,255},    ra*0.96f));
                    DrawCircleV({ex2,ey2},S(3.f),  ColorAlpha({150,20,255,255},ra*pulse*0.92f));
                    DrawCircleV({ex2,ey2},S(1.5f), ColorAlpha(VP_WHITE,        ra*pulse));
                    DrawNeonCircle({ex2,ey2},S(5.f),VP_PURPLE,ra*pulse*0.65f);
                }
                // forked tongue of pure void energy
                float tongueLen=S(8.f+sinf(t*4.f+ph)*2.f);
                float ta2=coilSpd;
                Vector2 tongBase={hx+cosf(ta2)*S(12.f),hy+sinf(ta2)*S(8.f)};
                Vector2 tongMid= {tongBase.x+cosf(ta2)*tongueLen*0.6f, tongBase.y+sinf(ta2)*tongueLen*0.6f};
                for(int fork=-1;fork<=1;fork+=2){
                    float fa=ta2+fork*0.35f;
                    DrawLineEx(tongMid,{tongMid.x+cosf(fa)*tongueLen*0.5f,tongMid.y+sinf(fa)*tongueLen*0.5f},
                        S(1.5f),ColorAlpha({200,60,255,255},ra*0.75f));
                }
                DrawLineEx(tongBase,tongMid,S(2.f),ColorAlpha({180,40,255,255},ra*0.70f));
            }
            break;}

        case 13:{ // QUANTUM DEPTHS — mirror doppelganger: a phase-shifted copy of your own ship
            if(e.subType < 5){
            float qFlick  = 0.65f+0.35f*sinf(t*5.f+ph);
            float qAngle  = t*0.4f+ph;
            float echoAge = fmodf(t*0.5f+ph, 1.f);
            float scanAge = fmodf(t*0.9f+ph, 1.f);
            // — outer phase aura glow —
            DrawCircleV({sp.x,spY},S(26.f),ColorAlpha({255,60,180,255},ra*0.08f));
            DrawNeonCircle({sp.x,spY},S(24.f),VP_HOTPINK,ra*qFlick*0.25f);
            // — orbiting quantum foam nodes —
            for(int i=0;i<8;i++){
                float fa=t*0.7f+i*(2.f*PI/8.f)+ph;
                float fr=S(18.f+sinf(t*1.5f+i+ph)*3.f);
                float nodeA=ra*(0.40f+0.25f*sinf(t*2.f+i+ph))*qFlick;
                DrawCircleV({sp.x+cosf(fa)*fr,spY+sinf(fa)*fr*0.75f},S(2.f),
                    ColorAlpha({255,80,200,255},nodeA));
                DrawNeonCircle({sp.x+cosf(fa)*fr,spY+sinf(fa)*fr*0.75f},S(3.f),VP_HOTPINK,nodeA*0.55f);
            }
            // — two ghost echo ships at offset angles —
            for(int echo=1;echo<=2;echo++){
                float eA=qAngle - echo*0.5f;
                float eAlpha=ra*qFlick*(0.28f-echo*0.08f);
                float eca2=cosf(eA),esa2=sinf(eA);
                Vector2 en2={sp.x+eca2*S(14.f),spY+esa2*S(14.f)};
                Vector2 et2={sp.x-eca2*S(14.f),spY-esa2*S(14.f)};
                Vector2 elW2={sp.x+eca2*S(-4.f)-esa2*S(10.f),spY+esa2*S(-4.f)+eca2*S(10.f)};
                Vector2 erW2={sp.x+eca2*S(-4.f)+esa2*S(10.f),spY+esa2*S(-4.f)-eca2*S(10.f)};
                DrawTriangle(en2,elW2,et2,ColorAlpha(VP_LAVENDER,eAlpha));
                DrawTriangle(en2,erW2,et2,ColorAlpha(VP_LAVENDER,eAlpha*0.8f));
            }
            // — main ghost ship — large, bright, readable —
            float ca=cosf(qAngle), sa=sinf(qAngle);
            Vector2 nose={sp.x+ca*S(16.f), spY+sa*S(16.f)};
            Vector2 tail={sp.x-ca*S(16.f), spY-sa*S(16.f)};
            Vector2 lWing={sp.x+ca*S(-4.f)-sa*S(13.f), spY+sa*S(-4.f)+ca*S(13.f)};
            Vector2 rWing={sp.x+ca*S(-4.f)+sa*S(13.f), spY+sa*S(-4.f)-ca*S(13.f)};
            // filled body — bright magenta so it reads clearly
            DrawTriangle(nose,lWing,tail,ColorAlpha({200,30,140,255},ra*qFlick*0.72f));
            DrawTriangle(nose,rWing,tail,ColorAlpha({200,30,140,255},ra*qFlick*0.60f));
            // bright hull edges
            DrawLineEx(nose,lWing,S(2.5f),ColorAlpha({255,100,210,255},ra*qFlick*0.90f));
            DrawLineEx(nose,rWing,S(2.5f),ColorAlpha({255,100,210,255},ra*qFlick*0.90f));
            DrawLineEx(lWing,tail,S(2.0f),ColorAlpha(VP_LAVENDER,      ra*qFlick*0.70f));
            DrawLineEx(rWing,tail,S(2.0f),ColorAlpha(VP_LAVENDER,      ra*qFlick*0.70f));
            // cockpit glow
            Vector2 cockpit={sp.x+ca*S(9.f),spY+sa*S(9.f)};
            DrawCircleV(cockpit,S(3.5f),ColorAlpha({255,180,240,255},ra*qFlick*0.88f));
            DrawCircleV(cockpit,S(1.8f),ColorAlpha(VP_WHITE,            ra*qFlick*0.80f));
            DrawNeonCircle(cockpit,S(5.f),VP_HOTPINK,                   ra*qFlick*0.60f);
            // engine tail glow
            DrawCircleV(tail,S(3.5f),ColorAlpha({220,60,180,255},ra*qFlick*0.80f));
            DrawCircleV(tail,S(1.8f),ColorAlpha(VP_WHITE,        ra*qFlick*0.70f));
            DrawNeonCircle(tail,S(7.f),VP_LAVENDER,               ra*qFlick*0.50f);
            // exhaust expanding rings
            for(int i=0;i<3;i++){
                float ea=fmodf(echoAge+i*0.33f,1.f);
                Vector2 ev={tail.x-ca*S(ea*20.f), tail.y-sa*S(ea*20.f)};
                DrawCircleLinesV(ev,S(3.f+ea*7.f),ColorAlpha(VP_HOTPINK,ra*(1.f-ea)*qFlick*0.40f));
            }
            // nose + wing tip sparks
            DrawCircleV(nose, S(3.f),ColorAlpha(VP_WHITE,   ra*qFlick*0.85f));
            DrawNeonCircle(nose,S(5.f),VP_HOTPINK,           ra*qFlick*0.65f);
            DrawNeonCircle(lWing,S(3.f),VP_HOTPINK,          ra*qFlick*0.55f);
            DrawNeonCircle(rWing,S(3.f),VP_HOTPINK,          ra*qFlick*0.55f);
            // horizontal scan lines (data corruption)
            for(int i=0;i<3;i++){
                float sa2=fmodf(scanAge+i*0.33f,1.f);
                float sy2=spY-S(16.f)+sa2*S(32.f);
                DrawLineEx({sp.x-S(16.f),sy2},{sp.x+S(16.f),sy2},
                    S(1.2f),ColorAlpha({255,120,220,255},ra*(1.f-sa2)*0.35f));
            }
            }
            else if(e.subType == 5){ // PROBABILITY CUBE — a Schrödinger box flickering between states
                float flip = sinf(t * 0.9f + ph);
                float flicker = 0.4f + 0.6f*fabsf(sinf(t * 4.5f + ph));
                float stateA = (flip > 0.f) ? 1.f : 0.f;
                float spinQ = t * 0.8f + ph;
                // outer probability cloud — nodes of superposition
                for(int i = 0; i < 12; i++){
                    float qa = spinQ + i*(2*PI/12.f);
                    float qr = S(22.f + sinf(t*1.5f + i*0.5f + ph)*4.f);
                    float qA = 0.3f + 0.7f*(i%2==0?stateA:(1.f-stateA));
                    DrawCircleV({sp.x + cosf(qa)*qr, spY + sinf(qa)*qr*0.6f},
                        S(1.8f), ColorAlpha(i%2==0?VP_PINK:VP_CYAN, ra*qA*0.55f));
                    DrawLineEx(sp, {sp.x + cosf(qa)*qr, spY + sinf(qa)*qr*0.6f},
                        S(0.5f), ColorAlpha(i%2==0?VP_PINK:VP_CYAN, ra*0.12f));
                }
                // spinning cube outline — wireframe
                float cs = S(12.f);
                float cs2 = S(7.f);
                // front face
                DrawRectangleLinesEx({sp.x - cs + cosf(spinQ)*S(2.f),
                    spY - cs + sinf(spinQ)*S(2.f), cs*2, cs*2},
                    S(1.5f), ColorAlpha(VP_PINK, ra*flicker*0.70f));
                // back face offset
                DrawRectangleLinesEx({sp.x - cs + cs2*.3f, spY - cs + cs2*.3f, cs*2, cs*2},
                    S(0.8f), ColorAlpha(VP_CYAN, ra*flicker*0.40f));
                // connecting edges (4 depth lines)
                for(int i = 0; i < 4; i++){
                    float ex = (i<2 ? -cs : cs) + sp.x;
                    float ey = (i%2==0 ? -cs : cs) + spY;
                    DrawLineEx({ex, ey}, {ex + cs2*.3f, ey + cs2*.3f},
                        S(0.9f), ColorAlpha(flip>0?VP_PINK:VP_CYAN, ra*flicker*0.45f));
                }
                // state A: alive — glowing orb inside
                DrawCircleV({sp.x, spY}, S(5.f) * stateA,
                    ColorAlpha(VP_PINK, ra*flicker*stateA*0.80f));
                DrawNeonCircle({sp.x, spY}, S(6.f)*stateA, VP_PINK, ra*stateA*0.55f);
                // state B: dead — X mark inside
                if(stateA < 0.5f){
                    float xc = S(6.f)*(1.f-stateA);
                    DrawLineEx({sp.x - xc, spY - xc}, {sp.x + xc, spY + xc},
                        S(2.f), ColorAlpha(VP_CYAN, ra*flicker*0.75f));
                    DrawLineEx({sp.x + xc, spY - xc}, {sp.x - xc, spY + xc},
                        S(2.f), ColorAlpha(VP_CYAN, ra*flicker*0.75f));
                }
                // label text: "?"
                DrawText("?", (int)(sp.x - S(3.f)), (int)(spY - S(5.f)), (int)S(18.f),
                    ColorAlpha(VP_WHITE, ra*flicker*0.55f));
            }
            else if(e.subType == 6){ // GLITCH ENTITY — corrupted data creature flickering in/out of reality
                float glitch13=fmodf(t*6.f+ph,1.f);
                float corruptAmp=sinf(t*7.2f+ph);
                float scanAge=fmodf(t*2.f+ph,1.f);
                // scan line distortion passing through
                DrawLineEx({sp.x-S(30.f),spY-S(8.f)+scanAge*S(30.f)},{sp.x+S(30.f),spY-S(8.f)+scanAge*S(30.f)},S(1.f),ColorAlpha(VP_CYAN,ra*0.25f));
                // corrupted bounding box — flickering
                if(glitch13>0.1f){
                    float bx=sp.x+corruptAmp*S(2.f);
                    DrawRectangleLinesEx({bx-S(14.f),spY-S(18.f),S(28.f),S(36.f)},S(1.2f),ColorAlpha(VP_CYAN,ra*0.45f));
                }
                // body — solid core that flickers between forms
                if(glitch13>0.3f){
                    DrawEllipse((int)(sp.x+corruptAmp*S(3.f)),(int)spY,(int)S(10.f),(int)S(14.f),ColorAlpha({5,30,40,255},ra*0.88f));
                    DrawEllipseLines((int)(sp.x+corruptAmp*S(3.f)),(int)spY,(int)S(10.f),(int)S(14.f),ColorAlpha(VP_CYAN,ra*0.55f));
                }
                if(glitch13<0.7f){
                    DrawRectangle((int)(sp.x-S(8.f)+corruptAmp*S(2.f)),(int)(spY-S(10.f)),(int)S(16.f),(int)S(20.f),ColorAlpha({4,25,35,255},ra*0.75f));
                    DrawRectangleLines((int)(sp.x-S(8.f)+corruptAmp*S(2.f)),(int)(spY-S(10.f)),(int)S(16.f),(int)S(20.f),ColorAlpha(VP_PINK,ra*0.50f));
                }
                // pixel artefacts scattered around
                for(int i=0;i<10;i++){
                    float px2=sp.x+(fmodf(ph*7.3f+i*19.4f,1.f)-0.5f)*S(40.f);
                    float py2=spY+(fmodf(ph*5.1f+i*13.7f,1.f)-0.5f)*S(40.f);
                    float pixA=fmodf(t*4.f+i*0.1f+ph,1.f);
                    if(pixA>0.6f)
                        DrawRectangle((int)px2,(int)py2,(int)S(3.f),(int)S(3.f),
                            ColorAlpha(i%3==0?VP_CYAN:i%3==1?VP_PINK:VP_WHITE,ra*(pixA-0.6f)/0.4f*0.65f));
                }
                // glitch arms — fragmented
                for(int side=-1;side<=1;side+=2){
                    float armX=sp.x+side*S(12.f)+corruptAmp*side*S(2.f);
                    if(glitch13>0.2f){
                        DrawRectangle((int)(armX-S(3.f)),(int)(spY-S(5.f)),(int)S(6.f),(int)S(12.f),ColorAlpha({5,28,38,255},ra*0.80f));
                        DrawRectangleLines((int)(armX-S(3.f)),(int)(spY-S(5.f)),(int)S(6.f),(int)S(12.f),ColorAlpha(VP_CYAN,ra*0.40f));
                    }
                }
                // head — pixelated with corruption noise
                DrawRectangle((int)(sp.x-S(8.f)),(int)(spY-S(22.f)),(int)S(16.f),(int)S(12.f),ColorAlpha({4,24,34,255},ra*0.90f));
                DrawRectangleLines((int)(sp.x-S(8.f)),(int)(spY-S(22.f)),(int)S(16.f),(int)S(12.f),ColorAlpha(VP_CYAN,ra*0.55f));
                // pixelated eyes
                for(int side=-1;side<=1;side+=2){
                    float ex2=sp.x+side*S(4.f)+corruptAmp*S(1.5f);
                    DrawRectangle((int)(ex2-S(2.f)),(int)(spY-S(20.f)),(int)S(4.f),(int)S(4.f),ColorAlpha({0,220,220,255},ra*0.90f));
                    if(glitch13>0.5f)
                        DrawNeonCircle({ex2,spY-S(18.f)},S(4.f),VP_CYAN,ra*0.50f);
                }
                // error code text drifting up
                float errT=fmodf(t*0.8f+ph,1.f);
                DrawText("ERR",(int)(sp.x-S(12.f)),(int)(spY-errT*S(30.f)),
                    (int)S(10.f),ColorAlpha(VP_CYAN,ra*(1.f-errT)*0.45f));
                // data streams
                for(int i=0;i<4;i++){
                    float da=fmodf(t*3.f+i*0.25f+ph,1.f);
                    DrawLineEx({sp.x+(i-1.5f)*S(6.f),spY-S(20.f)},{sp.x+(i-1.5f)*S(6.f),spY-S(20.f)-da*S(20.f)},S(1.f),ColorAlpha(VP_CYAN,ra*(1.f-da)*0.40f));
                }
            }
            break;}

        case 14:{ // PHANTOM GATE — kitsune spirit fox: a many-tailed spectral fox floating serenely
            if(e.subType < 5){
            float fFade = 0.45f + 0.35f * sinf(t*0.8f+ph);
            // Tails (the more tails, the more powerful — 3 here)
            int nTails = 3;
            for(int i = 0; i < nTails; i++){
                float tAngle = (i - nTails/2) * 0.35f;
                float tSway  = sinf(t*1.3f + i*0.6f + ph) * 0.25f;
                float tBase  = tAngle + tSway;
                float tx1 = sp.x + cosf(PI/2.f + tBase)*S(5.f);
                float ty1 = spY  + sinf(PI/2.f + tBase)*S(5.f);
                float tx2 = sp.x + cosf(PI/2.f + tBase)*S(22.f);
                float ty2 = spY  + sinf(PI/2.f + tBase)*S(22.f);
                DrawLineEx({tx1,ty1},{tx2,ty2},S(2.5f-i*0.4f),
                    ColorAlpha(VP_LAVENDER,ra*fFade*(0.6f-i*0.12f)));
                DrawNeonCircle({tx2,ty2},S(2.5f),VP_PURPLE,ra*fFade*0.4f);
            }
            // body (ethereal, barely visible)
            DrawEllipse((int)sp.x,(int)(spY+S(2.f)),(int)S(8.f),(int)S(6.f),
                ColorAlpha(VP_LAVENDER,ra*fFade*0.45f));
            // head
            DrawEllipse((int)sp.x,(int)(spY-S(5.f)),(int)S(7.f),(int)S(6.f),
                ColorAlpha(VP_LAVENDER,ra*fFade*0.55f));
            // ears
            DrawTriangle({sp.x-S(4.f),spY-S(8.f)},{sp.x-S(8.f),spY-S(15.f)},{sp.x,spY-S(8.f)},
                ColorAlpha(VP_LAVENDER,ra*fFade*0.5f));
            DrawTriangle({sp.x+S(4.f),spY-S(8.f)},{sp.x+S(8.f),spY-S(15.f)},{sp.x,spY-S(8.f)},
                ColorAlpha(VP_LAVENDER,ra*fFade*0.5f));
            // inner ear (pink)
            DrawTriangle({sp.x-S(3.f),spY-S(9.f)},{sp.x-S(6.f),spY-S(13.f)},{sp.x,spY-S(9.f)},
                ColorAlpha(VP_HOTPINK,ra*fFade*0.3f));
            // eyes (slanted, fox-like)
            DrawLineEx({sp.x-S(4.f),spY-S(6.f)},{sp.x-S(1.5f),spY-S(5.f)},S(1.5f),
                ColorAlpha(VP_PURPLE,ra*fFade*0.9f));
            DrawLineEx({sp.x+S(4.f),spY-S(6.f)},{sp.x+S(1.5f),spY-S(5.f)},S(1.5f),
                ColorAlpha(VP_PURPLE,ra*fFade*0.9f));
            DrawNeonCircle({sp.x-S(2.8f),spY-S(5.5f)},S(1.2f),VP_LAVENDER,ra*fFade*0.6f);
            DrawNeonCircle({sp.x+S(2.8f),spY-S(5.5f)},S(1.2f),VP_LAVENDER,ra*fFade*0.6f);
            // spirit flame (hovering above head)
            float flAge = fmodf(t*1.1f+ph,1.f);
            DrawNeonCircle({sp.x+sinf(t*2.f+ph)*S(3.f),spY-S(20.f)-flAge*S(4.f)},
                S(2.5f-flAge*1.5f),VP_PURPLE,ra*(1.f-flAge)*0.65f);
            }
            else if(e.subType == 5){ // SPIRIT LANTERN — paper lantern drifting with ghost inside
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float drift = sinf(t * 0.7f + ph) * S(3.f);
                float ghostSway = sinf(t * 1.5f + ph) * S(2.5f);
                float flameFlick = 0.7f + 0.3f*sinf(t * 8.f + ph);
                // hanging cord + tassel
                DrawLineEx({sp.x, spY - S(24.f)}, {sp.x + drift*.2f, spY - S(18.f)},
                    S(1.2f), ColorAlpha({140,90,180,255}, ra*0.60f));
                for(int i = 0; i < 4; i++){
                    float tx2 = sp.x + drift*.2f + (i-1.5f)*S(2.5f);
                    DrawLineEx({tx2, spY + S(14.f)}, {tx2 + sinf(t*2.f+i)*S(1.5f), spY + S(22.f)},
                        S(1.f), ColorAlpha({160,100,200,255}, ra*0.50f));
                }
                // lantern body — layered ellipses (rice paper look)
                DrawEllipse((int)(sp.x + drift), (int)spY, (int)S(12.f), (int)S(16.f),
                    ColorAlpha({200,170,220,255}, ra*0.25f));
                DrawEllipse((int)(sp.x + drift), (int)spY, (int)S(11.f), (int)S(15.f),
                    ColorAlpha({240,210,255,255}, ra*0.18f));
                DrawEllipseLines((int)(sp.x + drift), (int)spY, (int)S(12.f), (int)S(16.f),
                    ColorAlpha({180,130,220,255}, ra*0.65f));
                // lantern ribs
                for(int i = 0; i < 5; i++){
                    float ry = spY - S(12.f) + i*S(6.f);
                    float chordSq = S(12.f)*S(12.f) - (ry-spY)*(ry-spY);
                    if(chordSq < 0) continue;
                    float chord2 = sqrtf(chordSq);
                    DrawLineEx({sp.x + drift - chord2, ry}, {sp.x + drift + chord2, ry},
                        S(0.7f), ColorAlpha({180,130,220,255}, ra*0.45f));
                }
                // ghost shape inside lantern — floating face
                DrawCircleV({sp.x + drift + ghostSway, spY - S(3.f)}, S(5.5f),
                    ColorAlpha({220,190,255,255}, ra*0.55f));
                DrawCircleV({sp.x + drift + ghostSway, spY - S(3.f)}, S(4.f),
                    ColorAlpha({245,220,255,255}, ra*0.40f));
                // ghost tail wisps
                for(int i = 0; i < 3; i++){
                    float wt = fmodf(t*1.2f + i*0.33f + ph, 1.f);
                    DrawCircleV({sp.x + drift + ghostSway + (i-1)*S(3.f),
                        spY + S(4.f) + wt*S(10.f)},
                        S(2.5f - wt*2.f), ColorAlpha({210,180,248,255}, ra*(1.f-wt)*0.45f));
                }
                // ghost eyes
                for(int side = -1; side <= 1; side += 2){
                    DrawCircleV({sp.x + drift + ghostSway + side*S(2.f), spY - S(4.5f)},
                        S(1.5f), ColorAlpha({120,60,180,255}, ra*0.85f));
                    DrawNeonCircle({sp.x + drift + ghostSway + side*S(2.f), spY - S(4.5f)},
                        S(2.5f), VP_LAVENDER, ra*pulse*0.55f);
                }
                // inner flame
                DrawCircleV({sp.x + drift, spY + S(9.f)}, S(3.5f),
                    ColorAlpha({200,100,255,255}, ra*flameFlick*0.80f));
                DrawCircleV({sp.x + drift, spY + S(9.f)}, S(2.f),
                    ColorAlpha(VP_WHITE, ra*flameFlick*0.90f));
                DrawNeonCircle({sp.x + drift, spY + S(9.f)}, S(5.f),
                    VP_LAVENDER, ra*flameFlick*0.55f);
                // ambient glow from lantern
                DrawEllipse((int)(sp.x + drift), (int)spY, (int)S(18.f), (int)S(22.f),
                    ColorAlpha({180,100,240,255}, ra*0.06f));
            }
            else if(e.subType == 6){ // MOON RABBIT — lunar rabbit pounding mooncake mochi in a stone mortar
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float pound=fabsf(sinf(t*2.5f+ph));
                float dustAge=fmodf(t*2.f+ph,1.f);
                float earSway=sinf(t*1.2f+ph)*S(2.f);
                // mortar shadow
                DrawEllipse((int)sp.x,(int)(spY+S(28.f)),(int)S(16.f),(int)S(4.f),ColorAlpha(VP_BLACK,ra*0.18f));
                // mortar
                DrawEllipse((int)sp.x,(int)(spY+S(20.f)),(int)S(14.f),(int)S(6.f),ColorAlpha({50,35,70,255},ra*0.88f));
                DrawRectangle((int)(sp.x-S(12.f)),(int)(spY+S(14.f)),(int)S(24.f),(int)S(9.f),ColorAlpha({48,33,68,255},ra*0.90f));
                DrawRectangleLines((int)(sp.x-S(12.f)),(int)(spY+S(14.f)),(int)S(24.f),(int)S(9.f),ColorAlpha({100,75,140,255},ra*0.50f));
                DrawEllipse((int)sp.x,(int)(spY+S(14.f)),(int)S(12.f),(int)S(5.f),ColorAlpha({55,38,76,255},ra*0.88f));
                // mochi inside mortar
                DrawEllipse((int)sp.x,(int)(spY+S(16.f)),(int)S(8.f),(int)S(4.f),ColorAlpha({240,230,220,255},ra*0.85f));
                DrawNeonCircle({sp.x,spY+S(16.f)},S(6.f),VP_LAVENDER,ra*0.18f);
                // pestle
                float pestleY=spY-S(2.f)+pound*S(10.f);
                DrawRectangle((int)(sp.x-S(2.f)),(int)pestleY,(int)S(4.f),(int)S(14.f),ColorAlpha({60,45,82,255},ra*0.88f));
                DrawEllipse((int)sp.x,(int)(pestleY+S(14.f)),(int)S(4.5f),(int)S(3.f),ColorAlpha({70,52,95,255},ra*0.85f));
                // dust puffs when pounding
                if(pound>0.7f){
                    for(int i=0;i<4;i++){
                        float da=fmodf(dustAge+i*0.25f,1.f);
                        DrawCircleV({sp.x+(i-1.5f)*S(5.f),spY+S(14.f)-da*S(10.f)},S(2.f+da*3.f),ColorAlpha({240,230,220,255},ra*(1.f-da)*0.45f));
                    }
                }
                // bunny body
                DrawEllipse((int)sp.x,(int)(spY-S(2.f)),(int)S(10.f),(int)S(12.f),ColorAlpha({235,235,248,255},ra*0.90f));
                DrawEllipse((int)sp.x,(int)(spY-S(2.f)),(int)S(8.f),(int)S(10.f),ColorAlpha({248,248,255,255},ra*0.82f));
                // tail
                DrawCircleV({sp.x+S(9.f),spY+S(4.f)},S(3.5f),ColorAlpha({248,248,255,255},ra*0.80f));
                // arms holding pestle
                DrawLineEx({sp.x-S(4.f),spY-S(2.f)},{sp.x-S(2.f),spY+S(6.f)},S(3.f),ColorAlpha({230,230,245,255},ra*0.85f));
                DrawLineEx({sp.x+S(4.f),spY-S(2.f)},{sp.x+S(2.f),spY+S(6.f)},S(3.f),ColorAlpha({230,230,245,255},ra*0.85f));
                // head
                DrawCircleV({sp.x,spY-S(14.f)},S(9.f),ColorAlpha({238,238,252,255},ra*0.90f));
                DrawCircleV({sp.x,spY-S(14.f)},S(7.5f),ColorAlpha({252,252,255,255},ra*0.82f));
                // ears — long with inner pink
                for(int side=-1;side<=1;side+=2){
                    float ex2=sp.x+side*S(4.f)+earSway;
                    DrawEllipse((int)ex2,(int)(spY-S(26.f)),(int)S(3.f),(int)S(10.f),ColorAlpha({240,240,252,255},ra*0.90f));
                    DrawEllipse((int)ex2,(int)(spY-S(26.f)),(int)S(1.8f),(int)S(7.f),ColorAlpha({255,160,180,255},ra*0.65f));
                }
                // eyes — beady pink
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(3.5f),spY-S(15.f)},S(2.f),ColorAlpha({5,2,8,255},ra*0.95f));
                    DrawCircleV({sp.x+side*S(3.5f),spY-S(15.f)},S(1.2f),ColorAlpha({255,120,160,255},ra*0.90f));
                    DrawNeonCircle({sp.x+side*S(3.5f),spY-S(15.f)},S(3.f),{255,140,180,255},ra*pulse*0.45f);
                }
                // crescent moon charm floating above
                float moonBob2=sinf(t*0.8f+ph)*S(2.f);
                DrawCircleV({sp.x+S(12.f),spY-S(22.f)+moonBob2},S(5.f),ColorAlpha({255,240,180,255},ra*0.70f));
                DrawCircleV({sp.x+S(14.f),spY-S(23.f)+moonBob2},S(4.5f),ColorAlpha({35,30,50,255},ra*0.88f));
                DrawNeonCircle({sp.x+S(10.f),spY-S(22.f)+moonBob2},S(4.f),VP_LAVENDER,ra*0.35f);
            }
            break;}

        case 15:{ // SOLAR INFERNO — fire phoenix: a blazing bird reborn in corona plasma
            float pPulse= 0.6f+0.4f*sinf(t*3.5f+ph);
            if(e.subType < 5){
            float pBob  = sinf(t*0.28f+ph)*S(1.5f); // ghostly float
            float pFlap = sinf(t*2.8f+ph);
            // tail feathers (streaming fire)
            for(int i = 0; i < 6; i++){
                float ta = PI/2.f + (i-2.5f)*0.22f;
                float tLen = S(18.f+sinf(t*1.5f+i*0.6f+ph)*6.f);
                Color tc = i%3==0?ColorAlpha(VP_WHITE,ra*0.4f):
                           i%3==1?ColorAlpha(VP_YELLOW,ra*0.5f):ColorAlpha(VP_ORANGE,ra*0.55f);
                DrawLineEx({sp.x,spY+S(6.f)+pBob},{sp.x+cosf(ta)*tLen,spY+S(6.f)+sinf(ta)*tLen+pBob},
                    S(1.8f-i*0.15f),tc);
                DrawNeonCircle({sp.x+cosf(ta)*tLen,spY+S(6.f)+sinf(ta)*tLen+pBob},
                    S(1.5f),VP_ORANGE,ra*pPulse*0.4f);
            }
            // wings (spread, flapping up/down)
            float wY = pFlap*S(8.f);
            DrawTriangle({sp.x,spY+pBob},{sp.x-S(22.f),spY-S(4.f)+wY+pBob},{sp.x-S(6.f),spY+S(6.f)+pBob},
                ColorAlpha(VP_ORANGE,ra*0.80f));
            DrawTriangle({sp.x,spY+pBob},{sp.x+S(22.f),spY-S(4.f)+wY+pBob},{sp.x+S(6.f),spY+S(6.f)+pBob},
                ColorAlpha(VP_ORANGE,ra*0.80f));
            // wing highlight
            DrawNeonLine({sp.x-S(22.f),spY-S(4.f)+wY+pBob},{sp.x,spY+pBob},
                VP_YELLOW,S(0.8f),ra*pPulse*0.4f);
            DrawNeonLine({sp.x+S(22.f),spY-S(4.f)+wY+pBob},{sp.x,spY+pBob},
                VP_YELLOW,S(0.8f),ra*pPulse*0.4f);
            // body
            DrawEllipse((int)sp.x,(int)(spY+S(2.f)+pBob),(int)S(6.f),(int)S(9.f),
                ColorAlpha(VP_ORANGE,ra*0.90f));
            DrawEllipse((int)sp.x,(int)(spY+S(2.f)+pBob),(int)S(4.f),(int)S(7.f),
                ColorAlpha(VP_YELLOW,ra*0.50f));
            // head with crest
            DrawCircleV({sp.x,spY-S(6.f)+pBob},S(5.5f),ColorAlpha(VP_ORANGE,ra*0.88f));
            DrawCircleV({sp.x,spY-S(6.f)+pBob},S(3.5f),ColorAlpha(VP_YELLOW,ra*0.55f));
            DrawNeonCircle({sp.x,spY-S(6.f)+pBob},S(5.5f),VP_YELLOW,ra*pPulse*0.5f);
            // crest feathers
            for(int i = 0; i < 4; i++){
                float ca2 = -PI/2.f + (i-1.5f)*0.3f;
                DrawLineEx({sp.x,spY-S(9.f)+pBob},{sp.x+cosf(ca2)*S(8.f+i),spY-S(9.f)+sinf(ca2)*S(8.f+i)+pBob},
                    S(1.f),ColorAlpha(VP_YELLOW,ra*0.60f));
            }
            // eye (fiery amber)
            DrawCircleV({sp.x+S(2.f),spY-S(7.f)+pBob},S(2.f),ColorAlpha(VP_WHITE,ra*0.6f));
            DrawCircleV({sp.x+S(2.f),spY-S(7.f)+pBob},S(1.2f),ColorAlpha(VP_BLACK,ra));
            }
            else if(e.subType == 5){ // SUN SERPENT — coiling serpent of pure solar plasma
                float coilSpd2 = t * 0.65f + ph;
                float flareAge = fmodf(t * 2.f + ph, 1.f);
                int seg2 = 12;
                // solar corona halo
                DrawCircleV({sp.x, spY}, S(28.f), ColorAlpha({255,120,0,255}, ra*0.05f));
                // draw body segments
                for(int i = seg2-1; i >= 0; i--){
                    float fi = (float)i/seg2;
                    float sx2 = sp.x + sinf(coilSpd2 + i*0.6f)*S(22.f - fi*6.f);
                    float sy2 = spY + cosf(coilSpd2*0.7f + i*0.5f)*S(14.f);
                    float sr = S(8.f - fi*5.f);
                    // outer glow
                    DrawCircleV({sx2, sy2}, sr*1.8f, ColorAlpha({255,80,0,255}, ra*(1.f-fi)*0.08f));
                    // hot body
                    DrawCircleV({sx2, sy2}, sr, ColorAlpha({220,60,0,255}, ra*0.88f));
                    DrawCircleV({sx2, sy2}, sr*0.7f, ColorAlpha({255,140,20,255}, ra*0.80f));
                    DrawCircleV({sx2, sy2}, sr*0.4f, ColorAlpha({255,240,120,255}, ra*0.75f));
                    // scale ridges
                    if(i%2==0 && sr > S(3.f)){
                        DrawCircleLinesV({sx2, sy2}, sr,
                            ColorAlpha({255,180,40,255}, ra*(1.f-fi)*0.25f));
                    }
                    // plasma spines
                    if(i > 1 && i < seg2-1){
                        float spineA = coilSpd2 + i*0.6f - PI*.5f;
                        float spLen = sr * 1.5f;
                        DrawLineEx({sx2, sy2},
                            {sx2 + cosf(spineA)*spLen, sy2 + sinf(spineA)*spLen},
                            sr*0.3f, ColorAlpha({255,200,50,255}, ra*(1.f-fi)*0.55f));
                        DrawNeonCircle({sx2 + cosf(spineA)*spLen, sy2 + sinf(spineA)*spLen},
                            S(1.5f), VP_YELLOW, ra*(1.f-fi)*0.40f);
                    }
                }
                // head
                float hx = sp.x + sinf(coilSpd2)*S(22.f);
                float hy = spY + cosf(coilSpd2*0.7f)*S(14.f);
                DrawCircleV({hx, hy}, S(10.f), ColorAlpha({200,50,0,255}, ra*0.92f));
                DrawCircleV({hx, hy}, S(8.f),  ColorAlpha({255,120,15,255}, ra*0.85f));
                DrawCircleV({hx, hy}, S(5.f),  ColorAlpha({255,220,80,255}, ra*0.78f));
                DrawNeonCircle({hx, hy}, S(12.f), {255,100,0,255}, ra*0.40f*pPulse);
                // solar horns
                for(int side = -1; side <= 1; side += 2){
                    DrawLineEx({hx + side*S(6.f), hy - S(7.f)},
                        {hx + side*S(12.f), hy - S(16.f)},
                        S(3.f), ColorAlpha({255,160,20,255}, ra*0.80f));
                    DrawNeonCircle({hx + side*S(12.f), hy - S(16.f)}, S(4.f),
                        VP_YELLOW, ra*pPulse*0.60f);
                }
                // eyes
                for(int side = -1; side <= 1; side += 2){
                    DrawCircleV({hx + side*S(4.f), hy - S(2.f)}, S(3.f),
                        ColorAlpha({50,10,0,255}, ra*0.95f));
                    DrawCircleV({hx + side*S(4.f), hy - S(2.f)}, S(1.8f),
                        ColorAlpha({255,220,50,255}, ra*0.90f));
                    DrawNeonCircle({hx + side*S(4.f), hy - S(2.f)}, S(4.f),
                        VP_YELLOW, ra*pPulse*0.65f);
                }
                // flare bursts from body
                for(int i = 0; i < 3; i++){
                    float fa2 = fmodf(flareAge + i*0.33f, 1.f);
                    DrawCircleV({hx + cosf(coilSpd2+i*2.f)*S(8.f),
                        hy + sinf(coilSpd2+i*2.f)*S(5.f)},
                        S(2.f + fa2*8.f), ColorAlpha({255,160,20,255}, ra*(1.f-fa2)*0.45f));
                }
            }
            else if(e.subType == 6){ // CORONA KNIGHT — armoured warrior clad in living solar fire
                float armorPulse=0.7f+0.3f*sinf(t*2.f+ph);
                float cloakFlame=sinf(t*2.2f+ph)*S(3.f);
                float sparks15=fmodf(t*2.5f+ph,1.f);
                // ground corona glow
                DrawEllipse((int)sp.x,(int)(spY+S(32.f)),(int)S(22.f),(int)S(5.f),ColorAlpha({255,100,0,255},ra*0.12f));
                DrawNeonCircle({sp.x,spY+S(32.f)},S(22.f),{255,130,20,255},ra*0.08f);
                // legs
                for(int side=-1;side<=1;side+=2){
                    DrawRectangle((int)(sp.x+side*S(5.f)),(int)(spY+S(12.f)),(int)S(8.f),(int)S(14.f),ColorAlpha({120,45,4,255},ra*0.88f));
                    DrawRectangleLines((int)(sp.x+side*S(5.f)),(int)(spY+S(12.f)),(int)S(8.f),(int)S(14.f),ColorAlpha({255,130,20,255},ra*0.35f));
                    // greave fire trim
                    DrawNeonCircle({sp.x+side*S(9.f),spY+S(24.f)},S(5.f),VP_ORANGE,ra*armorPulse*0.30f);
                    // boots
                    DrawRectangle((int)(sp.x+side*S(5.f)),(int)(spY+S(26.f)),(int)S(10.f),(int)S(6.f),ColorAlpha({100,38,3,255},ra*0.88f));
                }
                // flame cloak — billowing behind
                for(int i=0;i<6;i++){
                    float fca=fmodf(t*1.5f+i*0.17f+ph,1.f);
                    float fcx=sp.x+(i-2.5f)*S(4.f)+cloakFlame;
                    float fcy=spY+S(4.f)+fca*S(22.f);
                    DrawCircleV({fcx,fcy},S(4.f+fca*4.f),ColorAlpha(fca<0.4f?VP_YELLOW:fca<0.7f?VP_ORANGE:VP_RED,ra*(1.f-fca)*0.45f));
                }
                // torso — full plate
                DrawRectangle((int)(sp.x-S(12.f)),(int)(spY-S(5.f)),(int)S(24.f),(int)S(16.f),ColorAlpha({115,42,3,255},ra*0.90f));
                DrawRectangleLines((int)(sp.x-S(12.f)),(int)(spY-S(5.f)),(int)S(24.f),(int)S(16.f),ColorAlpha({255,140,25,255},ra*0.40f));
                // chest sun emblem
                DrawCircleV({sp.x,spY+S(3.f)},S(6.f),ColorAlpha({140,50,4,255},ra*0.88f));
                DrawCircleV({sp.x,spY+S(3.f)},S(4.5f),ColorAlpha({255,160,25,255},ra*armorPulse*0.85f));
                DrawCircleV({sp.x,spY+S(3.f)},S(2.5f),ColorAlpha(VP_YELLOW,ra*armorPulse));
                DrawNeonCircle({sp.x,spY+S(3.f)},S(8.f),VP_ORANGE,ra*armorPulse*0.55f);
                // sun rays from emblem
                for(int i=0;i<8;i++){
                    float ra2=ph*0.5f+i*(2*PI/8.f);
                    DrawLineEx({sp.x+cosf(ra2)*S(5.f),spY+S(3.f)+sinf(ra2)*S(5.f)},{sp.x+cosf(ra2)*S(10.f),spY+S(3.f)+sinf(ra2)*S(10.f)},S(1.f),ColorAlpha(VP_YELLOW,ra*armorPulse*0.45f));
                }
                // arms + gauntlets
                for(int side=-1;side<=1;side+=2){
                    DrawRectangle((int)(sp.x+side*S(12.f)),(int)(spY-S(4.f)),(int)S(8.f),(int)S(14.f),ColorAlpha({115,42,3,255},ra*0.88f));
                    DrawRectangleLines((int)(sp.x+side*S(12.f)),(int)(spY-S(4.f)),(int)S(8.f),(int)S(14.f),ColorAlpha({255,130,20,255},ra*0.30f));
                    DrawCircleV({sp.x+side*S(16.f),spY+S(10.f)},S(5.f),ColorAlpha({130,48,4,255},ra*0.85f));
                    DrawNeonCircle({sp.x+side*S(16.f),spY+S(10.f)},S(6.f),VP_ORANGE,ra*armorPulse*0.40f);
                }
                // pauldrons — flame-shouldered
                for(int side=-1;side<=1;side+=2){
                    DrawEllipse((int)(sp.x+side*S(14.f)),(int)(spY-S(6.f)),(int)S(6.f),(int)S(5.f),ColorAlpha({125,46,4,255},ra*0.90f));
                    // shoulder flame jets
                    for(int j=0;j<3;j++){
                        float ja=fmodf(t*2.2f+j*0.33f+ph,1.f);
                        DrawCircleV({sp.x+side*S(14.f)+(j-1)*S(2.f),spY-S(10.f)-ja*S(12.f)},S(2.f+ja*2.f),ColorAlpha(ja<0.5f?VP_YELLOW:VP_ORANGE,ra*(1.f-ja)*0.55f));
                    }
                }
                // helmet
                DrawEllipse((int)sp.x,(int)(spY-S(16.f)),(int)S(10.f),(int)S(9.f),ColorAlpha({120,44,4,255},ra*0.92f));
                DrawEllipseLines((int)sp.x,(int)(spY-S(16.f)),(int)S(10.f),(int)S(9.f),ColorAlpha({255,130,20,255},ra*0.40f));
                // visor slit
                DrawLineEx({sp.x-S(6.f),spY-S(15.f)},{sp.x+S(6.f),spY-S(15.f)},S(2.f),ColorAlpha({5,2,0,255},ra*0.92f));
                DrawLineEx({sp.x-S(6.f),spY-S(15.f)},{sp.x+S(6.f),spY-S(15.f)},S(1.f),ColorAlpha({255,200,50,255},ra*armorPulse*0.85f));
                DrawNeonCircle({sp.x,spY-S(15.f)},S(7.f),VP_ORANGE,ra*armorPulse*0.40f);
                // crown of solar spikes
                for(int i=0;i<5;i++){
                    float ca=(i-2)*0.22f-PI*.5f;
                    DrawRectanglePro({sp.x+(i-2)*S(4.f),spY-S(25.f),S(3.f),S(7.f+i%2*3.f)},{S(1.5f),S(7.f+i%2*3.f)},0.f,ColorAlpha({255,160,25,255},ra*0.80f));
                    DrawNeonCircle({sp.x+(i-2)*S(4.f),spY-S(25.f)},S(4.f),VP_YELLOW,ra*armorPulse*0.40f);
                }
            }
            break;}

        case 16:{ // CORONA BREACH — solar prominence arch: a colossal plasma loop rising from a sun
            float cPulse = 0.55f + 0.45f*sinf(t*1.3f+ph);
            if(e.subType < 5){
            float cFlick = 0.5f + 0.5f*sinf(t*4.f+ph);
            // arch (parametric half-circle of plasma blobs)
            int nArch = 12;
            Vector2 archPrev = {sp.x-S(20.f), spY+S(5.f)};
            for(int i = 0; i <= nArch; i++){
                float ap = (float)i/nArch;
                float ax = sp.x + cosf(PI + ap*PI)*S(20.f);
                float ay = spY  + S(5.f) + sinf(PI + ap*PI)*S(25.f);
                float ar = S(3.5f + sinf(ap*PI)*4.f);
                Color ac = ap < 0.5f ? LerpC(VP_YELLOW, VP_WHITE, ap*2.f)
                                     : LerpC(VP_WHITE, VP_ORANGE, (ap-0.5f)*2.f);
                DrawLineEx(archPrev,{ax,ay},ar*1.5f,ColorAlpha(ac,ra*0.55f));
                DrawCircleV({ax,ay},ar,ColorAlpha(ac,ra*cPulse*0.7f));
                DrawNeonCircle({ax,ay},ar+S(2.f),VP_YELLOW,ra*cPulse*0.25f);
                archPrev = {ax,ay};
            }
            // base flare spray
            for(int i = 0; i < 8; i++){
                float fa = -PI/3.f + (float)i*(2.f*PI/3.f)/8.f;
                float fLen = S(10.f+sinf(t*3.f+i*0.7f+ph)*5.f);
                DrawNeonLine({sp.x+cosf(-PI/2.f)*S(20.f),spY+S(5.f)},
                             {sp.x+cosf(-PI/2.f)*S(20.f)+cosf(fa)*fLen,spY+S(5.f)+sinf(fa)*fLen},
                             VP_YELLOW,S(0.9f),ra*cFlick*0.5f);
            }
            }
            else if(e.subType == 5){ // PLASMA JELLYFISH — solar plasma jellyfish trailing streams
                float jellyBob = sinf(t * 0.32f + ph) * S(2.5f); // slow drift
                float tentPulse = sinf(t * 2.5f + ph);
                float streamAge = fmodf(t * 1.5f + ph, 1.f);
                // outer corona halo
                DrawCircleV({sp.x, spY + jellyBob}, S(24.f), ColorAlpha({255,235,150,255}, ra*0.05f));
                DrawCircleV({sp.x, spY + jellyBob}, S(18.f), ColorAlpha({255,215,100,255}, ra*0.07f));
                // bell — domed cap
                DrawEllipse((int)sp.x, (int)(spY + jellyBob - S(4.f)), (int)S(16.f), (int)S(12.f),
                    ColorAlpha({255,225,130,255}, ra*0.22f));
                DrawEllipse((int)sp.x, (int)(spY + jellyBob - S(4.f)), (int)S(14.f), (int)S(10.f),
                    ColorAlpha({255,245,180,255}, ra*0.18f));
                DrawEllipseLines((int)sp.x, (int)(spY + jellyBob - S(4.f)), (int)S(16.f), (int)S(12.f),
                    ColorAlpha({255,220,80,255}, ra*0.50f));
                // bell radial ribs
                for(int i = 0; i < 6; i++){
                    float ra2 = -PI + i*(PI/6.f);
                    DrawLineEx({sp.x, spY + jellyBob - S(4.f)},
                        {sp.x + cosf(ra2)*S(14.f), spY + jellyBob - S(4.f) + sinf(ra2)*S(10.f)},
                        S(0.6f), ColorAlpha({255,200,50,255}, ra*0.30f));
                }
                // oral arms — inner tentacles
                for(int i = 0; i < 5; i++){
                    float oa = (i-2)*0.25f;
                    DrawLineEx({sp.x + oa*S(8.f), spY + jellyBob + S(6.f)},
                        {sp.x + oa*S(14.f) + sinf(t*2.f+i+ph)*S(3.f),
                         spY + jellyBob + S(18.f)},
                        S(2.f - fabsf(i-2)*0.3f), ColorAlpha({255,210,80,255}, ra*0.65f));
                }
                // trailing streamers — long flowing tendrils
                for(int i = 0; i < 8; i++){
                    float ta = (i - 3.5f) * 0.18f;
                    int nseg = 8;
                    Vector2 prev = {sp.x + ta*S(10.f), spY + jellyBob + S(7.f)};
                    for(int seg2 = 0; seg2 < nseg; seg2++){
                        float sf = (float)seg2/nseg;
                        float wave = sinf(t*2.5f + ph + i*0.6f + sf*PI) * S(4.f + sf*4.f);
                        Vector2 cur = {prev.x + wave*(0.1f), prev.y + S(6.f + sf*2.f)};
                        DrawLineEx(prev, cur, S(1.5f - sf*1.f),
                            ColorAlpha({255,200,60,255}, ra*(1.f - sf*0.6f)*0.65f));
                        // glow nub at end
                        if(seg2 == nseg-1)
                            DrawNeonCircle(cur, S(2.f), VP_GOLD, ra*cPulse*0.35f);
                        prev = cur;
                    }
                }
                // nucleus — bright pulsing core
                DrawCircleV({sp.x, spY + jellyBob - S(4.f)}, S(5.f),
                    ColorAlpha({255,240,180,255}, ra*0.88f));
                DrawCircleV({sp.x, spY + jellyBob - S(4.f)}, S(3.f),
                    ColorAlpha(VP_WHITE, ra*cPulse));
                DrawNeonCircle({sp.x, spY + jellyBob - S(4.f)}, S(7.f),
                    VP_GOLD, ra*cPulse*0.65f);
                // stream rings from bell edge
                for(int i = 0; i < 3; i++){
                    float fa2 = fmodf(streamAge + i*0.33f, 1.f);
                    DrawCircleLinesV({sp.x, spY + jellyBob - S(4.f)},
                        S(14.f + fa2*20.f),
                        ColorAlpha({255,200,60,255}, ra*(1.f-fa2)*0.25f));
                }
            }
            else if(e.subType == 6){ // PLASMA TITAN — colossal humanoid born of living solar corona fire
                float breathe  = sinf(t*0.50f+ph)*S(1.2f);
                float lArmSway = sinf(t*0.50f+ph+1.1f)*S(2.5f);
                float rArmSway = sinf(t*0.50f+ph+1.1f+PI)*S(2.5f);
                float headNod  = sinf(t*0.35f+ph+0.6f)*S(0.8f);
                float corePulse= 0.55f+0.45f*sinf(t*2.2f+ph);
                float flareAge = fmodf(t*1.3f+ph,1.f);
                // outer corona aura — wide gold/orange haze
                DrawCircleV({sp.x,spY},S(36.f),ColorAlpha({255,210,40,255},ra*0.05f));
                DrawNeonCircle({sp.x,spY},S(34.f),VP_GOLD,ra*corePulse*0.18f);
                // solar flare arcs erupting from body
                for(int i=0;i<3;i++){
                    float fa=flareAge+i*0.33f;
                    float fp=fmodf(fa,1.f);
                    float fa2=ph+i*(2*PI/3.f)+t*0.5f;
                    float fr=S(18.f+fp*18.f);
                    DrawCircleV({sp.x+cosf(fa2)*fr,spY+sinf(fa2)*fr*0.7f},
                        S(3.f+fp*4.f),ColorAlpha({255,200,40,255},ra*(1.f-fp)*0.35f));
                    DrawNeonCircle({sp.x+cosf(fa2)*fr,spY+sinf(fa2)*fr*0.7f},
                        S(5.f+fp*5.f),VP_GOLD,ra*(1.f-fp)*0.22f);
                }
                // legs
                for(int side=-1;side<=1;side+=2){
                    float lx=sp.x+side*S(9.f);
                    // upper leg
                    DrawRectangle((int)(lx-S(5.f)),(int)(spY+S(10.f)),(int)S(10.f),(int)S(12.f),
                        ColorAlpha({200,130,5,255},ra*0.90f));
                    DrawRectangle((int)(lx-S(4.f)),(int)(spY+S(11.f)),(int)S(8.f),(int)S(10.f),
                        ColorAlpha({255,200,40,255},ra*0.78f));
                    // knee flare node
                    DrawCircleV({lx,spY+S(22.f)},S(4.f),ColorAlpha({255,160,10,255},ra*0.88f));
                    DrawNeonCircle({lx,spY+S(22.f)},S(5.f),VP_GOLD,ra*corePulse*0.55f);
                    // lower leg / boot
                    DrawRectangle((int)(lx-S(4.f)),(int)(spY+S(22.f)),(int)S(8.f),(int)S(10.f),
                        ColorAlpha({190,120,5,255},ra*0.88f));
                    DrawRectangle((int)(lx-S(5.f)),(int)(spY+S(30.f)),(int)S(10.f),(int)S(4.f),
                        ColorAlpha({210,150,8,255},ra*0.90f));
                }
                // torso — radiating heat
                DrawRectangle((int)(sp.x-S(14.f)),(int)(spY-S(6.f)+breathe),(int)S(28.f),(int)S(16.f),
                    ColorAlpha({210,140,6,255},ra*0.92f));
                DrawRectangle((int)(sp.x-S(12.f)),(int)(spY-S(4.f)+breathe),(int)S(24.f),(int)S(13.f),
                    ColorAlpha({255,200,35,255},ra*0.80f));
                // solar core in chest — white-hot centre
                DrawCircleV({sp.x,spY+S(3.f)+breathe},S(7.f), ColorAlpha({255,230,100,255},ra*0.90f));
                DrawCircleV({sp.x,spY+S(3.f)+breathe},S(5.f), ColorAlpha({255,255,200,255},ra*corePulse*0.92f));
                DrawCircleV({sp.x,spY+S(3.f)+breathe},S(2.8f),ColorAlpha(VP_WHITE,          ra*corePulse));
                DrawNeonCircle({sp.x,spY+S(3.f)+breathe},S(8.f),VP_GOLD,ra*corePulse*0.75f);
                // shoulder pauldrons — flaring plasma wings
                for(int side=-1;side<=1;side+=2){
                    float px=sp.x+side*S(16.f);
                    DrawRectangle((int)(px-S(3.f)),(int)(spY-S(8.f)+breathe),(int)S(8.f),(int)S(7.f),
                        ColorAlpha({220,160,8,255},ra*0.88f));
                    // plasma wing flare
                    DrawTriangle({px+side*S(3.f),spY-S(8.f)+breathe},
                        {px+side*S(14.f),spY-S(4.f)+breathe},
                        {px+side*S(6.f),spY+S(4.f)+breathe},
                        ColorAlpha({255,200,40,255},ra*corePulse*0.50f));
                }
                // arms
                for(int side=-1;side<=1;side+=2){
                    float ax=sp.x+side*S(20.f);
                    float aOff=side==(-1)?lArmSway:rArmSway;
                    DrawRectangle((int)(ax-S(4.f)),(int)(spY-S(4.f)+breathe+aOff),(int)S(8.f),(int)S(12.f),
                        ColorAlpha({200,130,5,255},ra*0.90f));
                    DrawRectangle((int)(ax-S(3.f)),(int)(spY-S(3.f)+breathe+aOff),(int)S(6.f),(int)S(11.f),
                        ColorAlpha({255,195,30,255},ra*0.80f));
                    // elbow flare
                    DrawNeonCircle({ax,spY+S(8.f)+breathe+aOff},S(4.f),VP_GOLD,ra*corePulse*0.50f);
                    // fist — corona-hot
                    DrawRectangle((int)(ax-S(5.f)),(int)(spY+S(8.f)+breathe+aOff),(int)S(10.f),(int)S(8.f),
                        ColorAlpha({210,145,6,255},ra*0.92f));
                    DrawNeonCircle({ax,spY+S(12.f)+breathe+aOff},S(6.f),VP_GOLD,ra*corePulse*0.60f);
                    DrawCircleV({ax,spY+S(12.f)+breathe+aOff},S(3.f),ColorAlpha(VP_WHITE,ra*corePulse*0.65f));
                }
                // head
                float hY=spY-S(20.f)+headNod;
                DrawRectangle((int)(sp.x-S(11.f)),(int)hY,(int)S(22.f),(int)S(14.f),
                    ColorAlpha({215,145,6,255},ra*0.92f));
                DrawRectangle((int)(sp.x-S(10.f)),(int)(hY+S(1.f)),(int)S(20.f),(int)S(12.f),
                    ColorAlpha({255,205,40,255},ra*0.82f));
                // crown of solar flare spikes
                for(int i=-2;i<=2;i++){
                    float ch=S(5.f+fabsf((float)i)*S(1.5f));
                    DrawRectangle((int)(sp.x+i*S(4.5f)-S(1.8f)),(int)(hY-ch),(int)S(3.5f),(int)ch,
                        ColorAlpha({240,180,10,255},ra*0.88f));
                    DrawCircleV({sp.x+i*S(4.5f),hY-ch},S(2.f),ColorAlpha(VP_WHITE,ra*corePulse*0.90f));
                    DrawNeonCircle({sp.x+i*S(4.5f),hY-ch},S(3.5f),VP_GOLD,ra*corePulse*0.60f);
                }
                // blazing eyes
                for(int side=-1;side<=1;side+=2){
                    float ex2=sp.x+side*S(5.f), ey2=hY+S(5.f);
                    DrawCircleV({ex2,ey2},S(3.5f),ColorAlpha({120,60,2,255}, ra*0.95f));
                    DrawCircleV({ex2,ey2},S(2.5f),ColorAlpha({255,240,180,255},ra*corePulse*0.92f));
                    DrawCircleV({ex2,ey2},S(1.2f),ColorAlpha(VP_WHITE,        ra*corePulse));
                    DrawNeonCircle({ex2,ey2},S(4.5f),VP_GOLD,ra*corePulse*0.70f);
                }
            }
            break;}

        case 17:{ // EVENT HORIZON — accretion disk with a central singularity, warped light
            if(e.subType < 5){
            float adSpin = t*0.55f+ph;
            float adPulse= 0.55f+0.45f*sinf(t*1.2f+ph);
            // outer accretion disk (hot gas rings)
            for(int i = 0; i < 5; i++){
                float ar = S(16.f+i*4.f);
                float aa = 0.08f+0.06f*sinf(t*0.8f+i*0.4f+ph);
                Color ac = i<2?ColorAlpha(VP_WHITE,ra*aa*1.2f):i<3?ColorAlpha(VP_YELLOW,ra*aa):
                           i<4?ColorAlpha(VP_ORANGE,ra*aa):ColorAlpha(VP_RED,ra*aa*0.7f);
                DrawEllipseLines((int)sp.x,(int)spY,(int)ar,(int)(ar*0.4f),ac);
            }
            // spinning infalling gas streams
            for(int i = 0; i < 3; i++){
                float ga = adSpin + i*(2.f*PI/3.f);
                for(int j = 0; j < 8; j++){
                    float jp = (float)j/8.f;
                    float gr = S(32.f - jp*20.f);
                    float gja = ga + jp * 1.8f; // spiral inward
                    Color gc = j<3?ColorAlpha(VP_WHITE,ra*0.3f*(1.f-jp)):
                               j<6?ColorAlpha(VP_ORANGE,ra*0.25f*(1.f-jp)):
                                   ColorAlpha(VP_RED,ra*0.2f*(1.f-jp));
                    DrawCircleV({sp.x+cosf(gja)*gr,spY+sinf(gja)*gr*0.4f},S(0.9f),gc);
                }
            }
            // central black singularity
            DrawCircleV({sp.x,spY},S(8.f),ColorAlpha({0,0,0,255},ra));
            DrawCircleV({sp.x,spY},S(6.f),ColorAlpha({2,0,4,255},ra));
            DrawNeonCircle({sp.x,spY},S(8.f),VP_RED,ra*adPulse*0.4f);
            DrawNeonCircle({sp.x,spY},S(10.f),VP_HOTPINK,ra*adPulse*0.2f);
            }
            else if(e.subType == 5){ // TIME FRAGMENT — a clock face torn apart by gravity
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float tearAge = fmodf(t * 0.6f + ph, 1.f);
                float gravWarp = sinf(t * 0.8f + ph);
                float handRot = t * 1.2f + ph;
                // gravitational warp rings
                for(int i = 0; i < 3; i++){
                    float wa = fmodf(tearAge + i*0.33f, 1.f);
                    DrawEllipse((int)sp.x, (int)spY, (int)(S(14.f + wa*30.f)), (int)(S(6.f + wa*12.f)),
                        ColorAlpha({228,45,255,255}, ra*(1.f-wa)*0.12f));
                    DrawEllipseLines((int)sp.x, (int)spY, (int)(S(14.f + wa*30.f)), (int)(S(6.f + wa*12.f)),
                        ColorAlpha({228,45,255,255}, ra*(1.f-wa)*0.22f));
                }
                // clock face — cracked circle
                DrawCircleV({sp.x, spY}, S(12.f), ColorAlpha({3,0,8,255}, ra*0.88f));
                DrawCircleLinesV({sp.x, spY}, S(12.f), ColorAlpha({228,45,255,255}, ra*0.65f));
                DrawCircleLinesV({sp.x, spY}, S(11.f), ColorAlpha({180,30,220,255}, ra*0.35f));
                // hour markers — torn and floating
                for(int i = 0; i < 12; i++){
                    float ma = i*(2*PI/12.f);
                    float drift = sinf(t*0.5f + ph + i*0.8f) * gravWarp * S(3.f);
                    float mr = S(9.5f) + drift;
                    float mx2 = sp.x + cosf(ma)*mr;
                    float my2 = spY  + sinf(ma)*mr;
                    DrawCircleV({mx2, my2}, i%3==0?S(1.8f):S(1.1f),
                        ColorAlpha({228,45,255,255}, ra*0.75f));
                }
                // hour hand — thick, warped
                float hha = handRot * 0.083f;
                DrawLineEx(sp, {sp.x + cosf(hha)*S(7.f) + gravWarp*S(1.f),
                    spY + sinf(hha)*S(7.f)},
                    S(2.5f), ColorAlpha({200,30,240,255}, ra*0.85f));
                DrawCircleV({sp.x + cosf(hha)*S(7.f), spY + sinf(hha)*S(7.f)},
                    S(1.5f), ColorAlpha({228,45,255,255}, ra*0.80f));
                // minute hand — thin, spinning
                DrawLineEx(sp, {sp.x + cosf(handRot)*S(10.f),
                    spY + sinf(handRot)*S(10.f) + gravWarp*S(2.f)},
                    S(1.2f), ColorAlpha({180,20,215,255}, ra*0.80f));
                // cracked glass lines
                for(int i = 0; i < 4; i++){
                    float ca2 = ph + i*(PI/2.f) + gravWarp*0.3f;
                    DrawLineEx(sp, {sp.x + cosf(ca2)*S(11.f + gravWarp*S(2.f)),
                        spY + sinf(ca2)*S(11.f)},
                        S(0.6f), ColorAlpha({228,45,255,255}, ra*0.30f));
                }
                // centre pivot
                DrawCircleV(sp, S(2.5f), ColorAlpha({228,45,255,255}, ra*0.90f));
                DrawNeonCircle(sp, S(4.f), {228,45,255,255}, ra*pulse*0.65f);
                // floating torn shards drifting outward
                for(int i = 0; i < 5; i++){
                    float sa2 = ph + i*(2*PI/5.f);
                    float sr = S(14.f + tearAge*12.f);
                    float sx2 = sp.x + cosf(sa2 + tearAge*0.5f)*sr;
                    float sy2 = spY  + sinf(sa2 + tearAge*0.5f)*sr;
                    DrawRectanglePro({sx2, sy2, S(4.f), S(3.f)}, {S(2.f), S(1.5f)},
                        (sa2 + tearAge*90.f)*RAD2DEG,
                        ColorAlpha({228,45,255,255}, ra*(1.f-tearAge)*0.55f));
                }
            }
            else if(e.subType == 6){ // GRAVITY SAINT — robed figure walking on the edge of a singularity
                float pulse = 0.5f+0.5f*sinf(t*2.f+ph);
                float halo17=t*0.5f+ph;
                float robeFlow=sinf(t*1.f+ph)*S(3.f);
                float lensAge=fmodf(t*0.7f+ph,1.f);
                // gravitational lens rings warping space
                for(int i=0;i<4;i++){
                    float la=fmodf(lensAge+i*0.25f,1.f);
                    float lr=S(12.f+la*35.f);
                    DrawEllipse((int)sp.x,(int)spY,(int)lr,(int)(lr*0.4f),ColorAlpha({200,20,255,255},ra*(1.f-la)*0.12f));
                    DrawEllipseLines((int)sp.x,(int)spY,(int)lr,(int)(lr*0.4f),ColorAlpha({228,45,255,255},ra*(1.f-la)*0.22f));
                }
                // robe — layered flowing panels
                for(int i=0;i<5;i++){
                    float rr=fmodf(t*1.f+i*0.2f+ph,1.f);
                    float rx=sp.x+(i-2)*S(4.f)+robeFlow*(float)(i-2)*0.2f;
                    float ry=spY+S(6.f)+rr*S(16.f);
                    DrawLineEx({rx,spY+S(6.f)},{rx+robeFlow*0.3f,ry},S(3.f-fabsf(i-2)*0.4f),ColorAlpha({18,2,38,255},ra*(1.f-rr)*0.70f));
                }
                DrawEllipse((int)sp.x,(int)(spY+S(6.f)),(int)S(12.f),(int)S(6.f),ColorAlpha({15,1,30,255},ra*0.82f));
                // body
                DrawEllipse((int)sp.x,(int)(spY-S(4.f)),(int)S(8.f),(int)S(10.f),ColorAlpha({12,0,26,255},ra*0.88f));
                DrawEllipse((int)sp.x,(int)(spY-S(4.f)),(int)S(6.f),(int)S(8.f),ColorAlpha({22,2,46,255},ra*0.82f));
                // outstretched arms — palm-forward
                for(int side=-1;side<=1;side+=2){
                    DrawLineEx({sp.x,spY-S(4.f)},{sp.x+side*S(16.f),spY-S(2.f)},S(2.5f),ColorAlpha({14,1,30,255},ra*0.85f));
                    DrawCircleV({sp.x+side*S(16.f),spY-S(2.f)},S(3.5f),ColorAlpha({18,2,36,255},ra*0.88f));
                    // palm singularity
                    DrawCircleV({sp.x+side*S(16.f),spY-S(2.f)},S(2.f),ColorAlpha({228,45,255,255},ra*pulse*0.85f));
                    DrawNeonCircle({sp.x+side*S(16.f),spY-S(2.f)},S(4.f),{228,45,255,255},ra*pulse*0.55f);
                    // mini gravity rings from palms
                    for(int j=0;j<2;j++){
                        float pa=fmodf(t*1.4f+j*0.5f+ph,1.f);
                        DrawCircleLinesV({sp.x+side*S(16.f),spY-S(2.f)},S(4.f+pa*14.f),ColorAlpha({228,45,255,255},ra*(1.f-pa)*0.30f));
                    }
                }
                // head
                DrawCircleV({sp.x,spY-S(16.f)},S(8.f),ColorAlpha({12,0,25,255},ra*0.90f));
                DrawCircleV({sp.x,spY-S(16.f)},S(6.5f),ColorAlpha({22,2,45,255},ra*0.82f));
                // halo — orbiting ring of light
                DrawEllipseLines((int)sp.x,(int)(spY-S(22.f)),(int)S(12.f),(int)S(4.f),ColorAlpha({228,45,255,255},ra*0.65f));
                DrawNeonCircle({sp.x,spY-S(22.f)},S(10.f),{228,45,255,255},ra*0.22f);
                // orbiting fragments of crushed matter
                for(int i=0;i<5;i++){
                    float oa=halo17+i*(2*PI/5.f);
                    float or2=S(18.f);
                    DrawRectanglePro({sp.x+cosf(oa)*or2,spY+sinf(oa)*or2*0.4f,S(4.f),S(3.f)},{S(2.f),S(1.5f)},oa*RAD2DEG,ColorAlpha({160,20,220,255},ra*0.60f));
                }
                // eyes — two voids of infinite depth
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(3.f),spY-S(17.f)},S(2.5f),ColorAlpha({0,0,0,255},ra*0.98f));
                    DrawCircleLinesV({sp.x+side*S(3.f),spY-S(17.f)},S(2.5f),ColorAlpha({228,45,255,255},ra*0.55f));
                    DrawNeonCircle({sp.x+side*S(3.f),spY-S(17.f)},S(4.f),{228,45,255,255},ra*pulse*0.50f);
                }
            }
            break;}

        case 18:{ // THE VOID — nothing-sphere: a sphere of perfect anti-matter, erasing what it touches
            if(e.subType < 5){
            float vPulse = 0.6f + 0.4f*sinf(t*0.7f+ph);
            float vRot   = t*0.18f + ph;
            // outer erasure zone (reality warping — inverted rings)
            for(int i = 0; i < 4; i++){
                float rAge = fmodf(t*0.5f+i*0.28f+ph,1.f);
                DrawCircleLinesV({sp.x,spY},S(25.f+rAge*15.f),
                    ColorAlpha({5,0,10,255},(1.f-rAge)*ra*0.45f));
            }
            // the sphere itself (perfectly black with subtle void shimmer at edge)
            DrawCircleV({sp.x,spY},S(14.f),ColorAlpha({0,0,0,255},ra));
            DrawCircleV({sp.x,spY},S(13.f),ColorAlpha({1,0,2,255},ra));
            DrawNeonCircle({sp.x,spY},S(14.f),VP_PURPLE,ra*vPulse*0.30f);
            DrawNeonCircle({sp.x,spY},S(16.f),VP_DEEPPURP,ra*vPulse*0.15f);
            // void tendrils reaching outward
            for(int i = 0; i < 6; i++){
                float ta = vRot + i*(2.f*PI/6.f);
                float tLen = S(8.f+sinf(t*1.5f+i+ph)*5.f);
                for(int j = 0; j < 5; j++){
                    float jp = (float)j/5.f;
                    float jLen = S(14.f) + tLen*jp;
                    float jAngle = ta + sinf(t*2.f+j*0.4f+i)*0.2f;
                    DrawCircleV({sp.x+cosf(jAngle)*jLen,spY+sinf(jAngle)*jLen},
                        S(1.5f-jp),ColorAlpha(VP_PURPLE,ra*(1.f-jp)*vPulse*0.4f));
                }
            }
            // stars being consumed at the edge
            for(int i = 0; i < 5; i++){
                float sa = t*0.3f + i*(2.f*PI/5.f) + ph;
                float sr = S(18.f+sinf(t*1.1f+i+ph)*3.f);
                DrawCircleV({sp.x+cosf(sa)*sr,spY+sinf(sa)*sr},S(0.8f),
                    ColorAlpha(VP_LAVENDER,ra*(0.3f+0.3f*sinf(t*2.f+i+ph))));
            }
            }
            else if(e.subType == 5){ // VOID LEVIATHAN — enormous serpentine void creature, barely visible
                float lRot = t * 0.25f + ph;
                float shimmer2 = 0.3f + 0.2f*sinf(t * 1.8f + ph);
                float eraseAge = fmodf(t * 1.0f + ph, 1.f);
                // reality-erasure effect — expanding rings of nothingness
                for(int i = 0; i < 3; i++){
                    float ea = fmodf(eraseAge + i*0.33f, 1.f);
                    DrawCircleLinesV({sp.x, spY}, S(10.f + ea*50.f),
                        ColorAlpha({165,85,245,255}, ra*(1.f-ea)*0.18f));
                }
                // vast body segments — barely visible
                int nseg = 10;
                for(int i = nseg-1; i >= 0; i--){
                    float fi = (float)i/nseg;
                    float sx2 = sp.x + sinf(lRot + i*0.7f)*S(30.f - fi*10.f);
                    float sy2 = spY + cosf(lRot*0.6f + i*0.55f)*S(18.f);
                    float sr = S(14.f - fi*9.f);
                    // anti-matter core — erases what it touches
                    DrawCircleV({sx2, sy2}, sr, ColorAlpha({1,0,4,255}, ra*0.90f));
                    DrawCircleV({sx2, sy2}, sr*0.7f, ColorAlpha({4,0,12,255}, ra*0.75f));
                    // faint outline with lavender ghost shimmer
                    DrawCircleLinesV({sx2, sy2}, sr,
                        ColorAlpha({165,85,245,255}, ra*shimmer2*(1.f-fi)*0.40f));
                    // void spines
                    if(i > 0 && i < nseg-1){
                        float spineA = lRot + i*0.7f - PI*.5f;
                        DrawLineEx({sx2, sy2},
                            {sx2 + cosf(spineA)*sr*1.6f, sy2 + sinf(spineA)*sr*1.6f},
                            sr*0.25f, ColorAlpha({130,65,200,255}, ra*(1.f-fi)*0.35f));
                    }
                }
                // massive head
                float hx = sp.x + sinf(lRot)*S(30.f);
                float hy = spY + cosf(lRot*0.6f)*S(18.f);
                DrawCircleV({hx, hy}, S(16.f), ColorAlpha({1,0,4,255}, ra*0.92f));
                DrawCircleV({hx, hy}, S(12.f), ColorAlpha({3,0,10,255}, ra*0.85f));
                DrawCircleLinesV({hx, hy}, S(16.f),
                    ColorAlpha({165,85,245,255}, ra*shimmer2*0.45f));
                // void maw
                DrawCircleV({hx + cosf(lRot)*S(14.f), hy + sinf(lRot*0.6f)*S(10.f)},
                    S(7.f), ColorAlpha({0,0,0,255}, ra*0.96f));
                DrawNeonCircle({hx + cosf(lRot)*S(14.f), hy + sinf(lRot*0.6f)*S(10.f)},
                    S(9.f), {165,85,245,255}, ra*shimmer2*0.50f);
                // ancient eyes — pair of voids
                for(int side = -1; side <= 1; side += 2){
                    float ex2 = hx + side*S(6.f);
                    float ey2 = hy - S(4.f);
                    DrawCircleV({ex2, ey2}, S(4.f), ColorAlpha({0,0,0,255}, ra*0.98f));
                    DrawCircleLinesV({ex2, ey2}, S(4.f),
                        ColorAlpha({165,85,245,255}, ra*shimmer2*0.60f));
                    DrawNeonCircle({ex2, ey2}, S(5.5f), {165,85,245,255}, ra*shimmer2*0.40f);
                }
            }
            else if(e.subType == 6){ // VOID BLOOM — an impossible flower that blossoms from nothingness
                float petalAge=fmodf(t*0.6f+ph,1.f);
                float bloomPulse=0.5f+0.5f*sinf(t*1.2f+ph);
                float spiralRot=t*0.4f+ph;
                // outer void aura
                DrawCircleV({sp.x,spY},S(28.f),ColorAlpha({165,85,245,255},ra*0.04f));
                // petals — 7 void petals on 3 layers
                for(int layer=0;layer<3;layer++){
                    int nPet=5+layer*2;
                    float pr=S(8.f+layer*7.f);
                    float pOff=layer*0.35f+spiralRot*(layer%2==0?1.f:-1.f)*0.15f;
                    for(int i=0;i<nPet;i++){
                        float pa=pOff+i*(2*PI/nPet);
                        float pPulse2=0.4f+0.6f*sinf(t*0.9f+ph+i*0.7f+layer);
                        Color pCol=layer==0?ColorAlpha({120,40,200,255},ra*pPulse2*0.65f):
                                   layer==1?ColorAlpha({80,5,160,255},ra*pPulse2*0.55f):
                                            ColorAlpha({165,85,245,255},ra*pPulse2*0.45f);
                        DrawEllipse((int)(sp.x+cosf(pa)*pr),(int)(spY+sinf(pa)*pr),(int)S(5.f+layer*2.f),(int)S(3.f+layer),pCol);
                        DrawEllipseLines((int)(sp.x+cosf(pa)*pr),(int)(spY+sinf(pa)*pr),(int)S(5.f+layer*2.f),(int)S(3.f+layer),ColorAlpha({165,85,245,255},ra*pPulse2*0.40f));
                    }
                }
                // stem — a line of nothingness
                for(int i=0;i<6;i++){
                    float sy2=spY+S(8.f)+i*S(4.f);
                    DrawCircleV({sp.x+sinf(t*0.5f+ph+i*0.3f)*S(1.5f),sy2},S(1.8f),ColorAlpha({35,5,65,255},ra*0.80f));
                }
                // void stamen — pulsing black orbs at centre
                for(int i=0;i<5;i++){
                    float sa=spiralRot+i*(2*PI/5.f);
                    float sr=S(4.f);
                    DrawCircleV({sp.x+cosf(sa)*sr,spY+sinf(sa)*sr},S(2.f),ColorAlpha({5,0,12,255},ra*0.92f));
                    DrawCircleV({sp.x+cosf(sa)*sr,spY+sinf(sa)*sr},S(1.f),ColorAlpha({165,85,245,255},ra*bloomPulse*0.75f));
                    DrawNeonCircle({sp.x+cosf(sa)*sr,spY+sinf(sa)*sr},S(3.f),{165,85,245,255},ra*bloomPulse*0.40f);
                }
                // expanding dissolution rings
                for(int i=0;i<3;i++){
                    float ra2=fmodf(petalAge+i*0.33f,1.f);
                    DrawCircleLinesV({sp.x,spY},S(6.f+ra2*26.f),ColorAlpha({165,85,245,255},ra*(1.f-ra2)*0.20f));
                }
                // core
                DrawCircleV({sp.x,spY},S(5.f),ColorAlpha({0,0,0,255},ra*0.95f));
                DrawCircleV({sp.x,spY},S(3.f),ColorAlpha({165,85,245,255},ra*bloomPulse*0.80f));
                DrawCircleV({sp.x,spY},S(1.5f),ColorAlpha(VP_WHITE,ra*bloomPulse*0.70f));
                DrawNeonCircle({sp.x,spY},S(6.f),{165,85,245,255},ra*bloomPulse*0.60f);
                // void dust motes
                for(int i=0;i<8;i++){
                    float da2=spiralRot+i*(2*PI/8.f);
                    float dr=S(18.f+sinf(t*0.7f+i+ph)*5.f);
                    DrawCircleV({sp.x+cosf(da2)*dr,spY+sinf(da2)*dr},S(1.2f),ColorAlpha({165,85,245,255},ra*0.30f));
                }
            }
            break;}

        case 19:{ // VOID SINGULARITY — the great eye: a universe-spanning eye of ancient intelligence
            if(e.subType < 5){
            float eyeOpen = 0.55f + 0.45f*sinf(t*0.22f+ph);
            float vPulse  = 0.6f  + 0.40f*sinf(t*1.8f+ph);
            // outer lids
            DrawEllipse((int)sp.x,(int)spY,(int)S(26.f),(int)(S(14.f)*eyeOpen+1),
                ColorAlpha(VP_LAVENDER,ra*0.38f));
            DrawEllipse((int)sp.x,(int)spY,(int)S(24.f),(int)(S(12.f)*eyeOpen),
                ColorAlpha({6,2,15,255},ra*0.94f));
            // iris (deep void purple, layered rings)
            DrawCircleV({sp.x,spY},S(11.f),ColorAlpha({35,8,65,255},ra*0.90f));
            for(int ii = 0; ii < 6; ii++){
                float ia = t*0.04f + ii*(2.f*PI/6.f);
                DrawLineEx({sp.x+cosf(ia)*S(7.f),spY+sinf(ia)*S(7.f)},
                           {sp.x+cosf(ia)*S(10.5f),spY+sinf(ia)*S(10.5f)},
                    S(0.7f),ColorAlpha(VP_PURPLE,ra*0.08f*vPulse));
            }
            DrawNeonCircle({sp.x,spY},S(11.f),VP_PURPLE,ra*0.50f*vPulse);
            DrawNeonCircle({sp.x,spY},S(8.5f),VP_HOTPINK,ra*0.25f);
            // pupil (vertical slit — absolute nothing)
            DrawCircleV({sp.x,spY},S(5.f),ColorAlpha(VP_BLACK,ra));
            DrawEllipse((int)sp.x,(int)spY,(int)S(1.4f),(int)S(5.f),ColorAlpha(VP_BLACK,ra));
            DrawNeonCircle({sp.x,spY},S(5.f),VP_HOTPINK,ra*0.35f*vPulse);
            // lashes (radiating)
            for(int li = 0; li < 14; li++){
                float la = li*(2.f*PI/14.f) + t*0.06f;
                float lLen = S(3.f+sinf(t*1.8f+li*0.5f)*1.5f);
                DrawLineEx({sp.x+cosf(la)*S(24.f)*eyeOpen,spY+sinf(la)*S(12.f)*eyeOpen*0.8f},
                           {sp.x+cosf(la)*(S(24.f)+lLen)*eyeOpen,spY+sinf(la)*(S(12.f)+lLen)*eyeOpen*0.8f},
                    S(0.9f),ColorAlpha(VP_HOTPINK,ra*eyeOpen*0.65f));
            }
            // orbiting void stars
            for(int si = 0; si < 5; si++){
                float sa = t*0.32f+si*(2.f*PI/5.f)+ph;
                float sr = S(30.f+sinf(t+si+ph)*4.f);
                DrawNeonCircle({sp.x+cosf(sa)*sr,spY+sinf(sa)*sr*0.5f},
                    S(1.6f),VP_PURPLE,ra*(0.3f+0.35f*sinf(t*2.f+si+ph)));
            }
            }
            else if(e.subType == 5){ // THE ARCHITECT — humanoid silhouette made of star-stuff
                float starDrift = t * 0.3f + ph;
                float stellarPulse = 0.7f + 0.3f*sinf(t * 1.4f + ph);
                float conAge = fmodf(t * 0.8f + ph, 1.f);
                // constellation lines between body stars
                // head centre → shoulders → elbows → hands; torso; hips → knees → feet
                auto SC = [&](float nx2, float ny2) -> Vector2 {
                    return {sp.x + nx2*sc, spY + ny2*sc};
                };
                struct StarNode { float x, y; float bright; };
                StarNode nodes[] = {
                    {0, -20}, // head
                    {-8,-14},{8,-14}, // shoulders
                    {-14,-6},{14,-6}, // elbows
                    {-18, 2},{18, 2}, // hands
                    {0,  -6}, // neck/chest
                    {0,   4}, // torso
                    {-6, 12},{6, 12}, // hips
                    {-8, 20},{8, 20}, // knees
                    {-8, 28},{8, 28}, // feet
                };
                int nNodes = 15;
                int edges[][2] = {
                    {0,2},{0,1},{1,3},{2,4},{3,5},{4,6}, // arms
                    {0,7},{7,8},{8,9},{8,10},{9,11},{10,12},{11,13},{12,14} // body+legs
                };
                // constellation connection lines
                for(auto& e2 : edges){
                    Vector2 a = SC(nodes[e2[0]].x, nodes[e2[0]].y);
                    Vector2 b = SC(nodes[e2[1]].x, nodes[e2[1]].y);
                    DrawLineEx(a, b, S(0.7f), ColorAlpha({255,235,165,255}, ra*0.30f));
                }
                // star nodes
                for(int i = 0; i < nNodes; i++){
                    Vector2 pos = SC(nodes[i].x, nodes[i].y);
                    float nPulse = 0.5f + 0.5f*sinf(starDrift + i*0.7f);
                    float nSize = (i == 0 || i == 7) ? S(3.5f) : S(2.f);
                    // outer glow
                    DrawCircleV(pos, nSize*2.5f, ColorAlpha({255,225,140,255}, ra*nPulse*0.12f));
                    DrawCircleV(pos, nSize, ColorAlpha({255,240,180,255}, ra*nPulse*0.85f));
                    DrawCircleV(pos, nSize*0.5f, ColorAlpha(VP_WHITE, ra*nPulse));
                    DrawNeonCircle(pos, nSize*1.6f, {255,215,80,255}, ra*nPulse*stellarPulse*0.40f);
                }
                // star-body inner fill glow at key joints
                for(int i : {0, 7, 8}){
                    Vector2 pos = SC(nodes[i].x, nodes[i].y);
                    DrawNeonCircle(pos, S(6.f + (i==0?4.f:0.f)),
                        {255,225,120,255}, ra*stellarPulse*0.20f);
                }
                // constellation background nebula — faint glow behind figure
                DrawEllipse((int)sp.x, (int)spY, (int)S(22.f), (int)S(32.f),
                    ColorAlpha({255,215,80,255}, ra*0.04f));
                // shooting star passing through
                float shotT = fmodf(t*1.5f + ph, 3.f);
                if(shotT < 1.f){
                    float sx2 = sp.x - S(30.f) + shotT*S(60.f);
                    float sy2 = spY - S(20.f) + shotT*S(10.f);
                    DrawLineEx({sx2 - S(12.f), sy2 - S(2.f)}, {sx2, sy2},
                        S(1.2f), ColorAlpha({255,240,180,255}, ra*(1.f-shotT)*0.55f));
                    DrawCircleV({sx2, sy2}, S(1.5f), ColorAlpha(VP_WHITE, ra*(1.f-shotT)*0.80f));
                }
            }
            else if(e.subType == 6){ // COSMIC EGG — an enormous egg of creation cracking open, universe inside
                float crackAge=fmodf(t*0.5f+ph,1.f);
                float crackOpen=0.3f+0.5f*fabsf(sinf(t*0.6f+ph));
                float cosmicPulse=0.6f+0.4f*sinf(t*1.5f+ph);
                float starSpin=t*0.3f+ph;
                // outer void aura
                DrawCircleV({sp.x,spY},S(30.f),ColorAlpha({255,225,100,255},ra*0.04f));
                // egg halves — splitting apart
                float gapY=crackOpen*S(8.f);
                // top half
                DrawEllipse((int)sp.x,(int)(spY-S(8.f)-gapY),(int)S(18.f),(int)S(14.f),ColorAlpha({10,5,2,255},ra*0.92f));
                DrawEllipseLines((int)sp.x,(int)(spY-S(8.f)-gapY),(int)S(18.f),(int)S(14.f),ColorAlpha({255,225,80,255},ra*0.55f));
                // bottom half
                DrawEllipse((int)sp.x,(int)(spY+S(8.f)+gapY),(int)S(18.f),(int)S(14.f),ColorAlpha({10,5,2,255},ra*0.92f));
                DrawEllipseLines((int)sp.x,(int)(spY+S(8.f)+gapY),(int)S(18.f),(int)S(14.f),ColorAlpha({255,225,80,255},ra*0.55f));
                // crack lines
                for(int i=0;i<5;i++){
                    float cx2=sp.x+(i-2)*S(5.f);
                    float cOff=crackOpen*S(2.f+i);
                    DrawLineEx({cx2-cOff,spY-S(4.f)},{cx2+cOff,spY+S(4.f)},S(0.8f),ColorAlpha({255,200,50,255},ra*0.65f));
                }
                // universe leaking out from gap
                // tiny galaxies
                for(int i=0;i<5;i++){
                    float ga=starSpin+i*(2*PI/5.f);
                    float gr=S(5.f+crackOpen*8.f)*(1.f+i*0.2f);
                    float gx=sp.x+cosf(ga)*gr;
                    float gy=spY+sinf(ga)*gr*0.4f;
                    DrawCircleV({gx,gy},S(2.f),ColorAlpha({255,240,150,255},ra*cosmicPulse*0.65f));
                    DrawNeonCircle({gx,gy},S(4.f),{255,230,100,255},ra*cosmicPulse*0.35f);
                }
                // star motes in gap
                for(int i=0;i<12;i++){
                    float sa=starSpin*1.8f+i*(2*PI/12.f);
                    float sr=S(2.f+i%3*2.f)*crackOpen;
                    float sx2=sp.x+cosf(sa)*sr;
                    float sy2=spY+sinf(sa)*sr*0.5f;
                    DrawCircleV({sx2,sy2},S(0.8f+i%3*0.3f),ColorAlpha({255,245,200,255},ra*cosmicPulse*0.75f));
                }
                // light blast from gap
                DrawEllipse((int)sp.x,(int)spY,(int)(S(14.f)*crackOpen),(int)(S(4.f)*crackOpen),ColorAlpha({255,250,220,255},ra*crackOpen*0.30f));
                DrawNeonCircle({sp.x,spY},S(8.f)*crackOpen,{255,240,150,255},ra*crackOpen*0.60f);
                // gold cracks glowing
                for(int i=0;i<6;i++){
                    float ca=ph+i*(2*PI/6.f);
                    float cr2=S(8.f+crackOpen*6.f);
                    DrawLineEx({sp.x,spY},{sp.x+cosf(ca)*cr2,spY+sinf(ca)*cr2*0.6f},S(0.7f),ColorAlpha({255,215,50,255},ra*crackOpen*0.50f));
                }
                // pattern on egg shell
                for(int r=1;r<=2;r++){
                    for(int i=0;i<6*r;i++){
                        float ma=ph+i*(2*PI/(6.f*r));
                        float mr=S(10.f+r*4.f);
                        DrawCircleV({sp.x+cosf(ma)*mr,spY-S(8.f)-gapY+sinf(ma)*mr*0.5f},S(1.f),ColorAlpha({255,215,50,255},ra*0.30f));
                    }
                }
            }
            break;}

        default:{ // Waves 20-27 — escalating chaos entities, each unique
            int chaos = w - 20;
            float cPulse = 0.55f + 0.45f*sinf(t*(1.5f+chaos*0.3f)+ph);
            float cSpin  = t*(0.4f+chaos*0.1f)+ph;
            static const Color chaosColors[8]={VP_HOTPINK,VP_PURPLE,VP_DEEPCYAN,VP_ORANGE,
                                               VP_RED,VP_LAVENDER,VP_GOLD,{5,0,10,255}};
            Color cc = chaosColors[chaos%8];

            if(chaos == 0){ // FRACTAL MIND — a tessellating mandala of interlocking geometric rings
                float fracSpin = t*0.6f+ph;
                float fracPulse = 0.5f+0.5f*sinf(t*1.8f+ph);
                // three nested rotating polygon rings
                for(int ring=0;ring<3;ring++){
                    int nSides=5+ring;
                    float rr=S(8.f+ring*7.f);
                    float rOff=fracSpin*(ring%2==0?1.f:-1.f)*(0.5f+ring*0.2f);
                    for(int i=0;i<nSides;i++){
                        float a1=rOff+i*(2*PI/nSides);
                        float a2=rOff+(i+1)*(2*PI/nSides);
                        DrawLineEx({sp.x+cosf(a1)*rr,spY+sinf(a1)*rr},
                            {sp.x+cosf(a2)*rr,spY+sinf(a2)*rr},
                            S(1.2f-ring*0.2f),ColorAlpha(cc,ra*(0.70f-ring*0.15f)));
                        // spoke to centre
                        DrawLineEx({sp.x,spY},{sp.x+cosf(a1)*rr,spY+sinf(a1)*rr},
                            S(0.5f),ColorAlpha(cc,ra*0.18f));
                        DrawNeonCircle({sp.x+cosf(a1)*rr,spY+sinf(a1)*rr},S(1.5f),cc,ra*fracPulse*0.45f);
                    }
                }
                // expanding burst rings
                for(int i=0;i<3;i++){
                    float rAge=fmodf(t*0.8f+i*0.35f+ph,1.f);
                    DrawCircleLinesV({sp.x,spY},S(10.f+rAge*28.f),ColorAlpha(cc,ra*(1.f-rAge)*0.40f));
                }
                DrawCircleV({sp.x,spY},S(5.f),ColorAlpha(cc,ra*0.85f));
                DrawCircleV({sp.x,spY},S(3.f),ColorAlpha(VP_WHITE,ra*fracPulse*0.70f));
                DrawNeonCircle({sp.x,spY},S(6.f),cc,ra*fracPulse*0.75f);
            }
            else if(chaos == 1){ // VOID HYDRA — multi-headed serpent of pure darkness
                float headSpin = t*0.4f+ph;
                float neckWave = sinf(t*1.8f+ph);
                for(int h=0;h<5;h++){
                    float ha=headSpin+h*(2*PI/5.f);
                    float nr=S(14.f+sinf(t*1.2f+h*0.7f+ph)*4.f);
                    float hx=sp.x+cosf(ha)*nr;
                    float hy=spY+sinf(ha)*nr;
                    // neck — undulating
                    int nseg=5;
                    Vector2 prev={sp.x+cosf(ha)*S(5.f),spY+sinf(ha)*S(5.f)};
                    for(int s=1;s<=nseg;s++){
                        float sf=(float)s/nseg;
                        float wave2=sinf(t*2.f+h*0.5f+s*0.4f+ph)*S(2.5f);
                        Vector2 cur={sp.x+cosf(ha)*S(5.f)+cosf(ha)*(nr-S(5.f))*sf + sinf(ha)*wave2,
                                     spY+sinf(ha)*S(5.f)+sinf(ha)*(nr-S(5.f))*sf + cosf(ha)*wave2};
                        DrawLineEx(prev,cur,S(3.f-sf*1.5f),ColorAlpha({20,2,40,255},ra*(1.f-sf*0.3f)));
                        prev=cur;
                    }
                    // head
                    DrawCircleV({hx,hy},S(5.5f),ColorAlpha({15,1,30,255},ra*0.90f));
                    DrawCircleV({hx,hy},S(4.f),ColorAlpha({30,3,60,255},ra*0.82f));
                    // eyes
                    for(int side=-1;side<=1;side+=2){
                        float ea=ha+PI*.5f;
                        DrawCircleV({hx+cosf(ea)*side*S(2.f),hy+sinf(ea)*side*S(2.f)},S(1.8f),ColorAlpha({5,0,10,255},ra*0.95f));
                        DrawCircleV({hx+cosf(ea)*side*S(2.f),hy+sinf(ea)*side*S(2.f)},S(1.f),ColorAlpha(cc,ra*cPulse*0.90f));
                        DrawNeonCircle({hx+cosf(ea)*side*S(2.f),hy+sinf(ea)*side*S(2.f)},S(2.5f),cc,ra*cPulse*0.55f);
                    }
                }
                DrawCircleV({sp.x,spY},S(8.f),ColorAlpha({12,1,25,255},ra*0.90f));
                DrawNeonCircle({sp.x,spY},S(9.f),cc,ra*cPulse*0.55f);
            }
            else if(chaos == 2){ // CRYSTAL TITAN — colossal humanoid of prismatic shards
                float shimmer3=0.6f+0.4f*sinf(t*2.4f+ph);
                float breathe=sinf(t*0.5f+ph)*S(1.f);
                // shadow
                DrawEllipse((int)sp.x,(int)(spY+S(32.f)),(int)S(22.f),(int)S(5.f),ColorAlpha(VP_BLACK,ra*0.22f));
                // legs — crystal pillars
                for(int side=-1;side<=1;side+=2){
                    float lx=sp.x+side*S(9.f);
                    DrawRectangle((int)(lx-S(4.f)),(int)(spY+S(10.f)),(int)S(8.f),(int)S(22.f),ColorAlpha({5,40,55,255},ra*0.88f));
                    DrawRectangleLines((int)(lx-S(4.f)),(int)(spY+S(10.f)),(int)S(8.f),(int)S(22.f),ColorAlpha(cc,ra*0.35f));
                    // crystal facets
                    DrawLineEx({lx,spY+S(10.f)},{lx,spY+S(32.f)},S(0.6f),ColorAlpha({120,220,240,255},ra*shimmer3*0.35f));
                    DrawNeonCircle({lx,spY+S(16.f)},S(2.f),cc,ra*shimmer3*0.40f);
                }
                // torso
                DrawRectangle((int)(sp.x-S(14.f)),(int)(spY-S(8.f)+breathe),(int)S(28.f),(int)S(18.f),ColorAlpha({4,35,50,255},ra*0.90f));
                DrawRectangleLines((int)(sp.x-S(14.f)),(int)(spY-S(8.f)+breathe),(int)S(28.f),(int)S(18.f),ColorAlpha(cc,ra*0.40f));
                // chest crystal
                DrawCircleV({sp.x,spY+breathe},S(6.f),ColorAlpha({5,45,65,255},ra*0.88f));
                DrawCircleV({sp.x,spY+breathe},S(4.f),ColorAlpha(cc,ra*shimmer3*0.75f));
                DrawCircleV({sp.x,spY+breathe},S(2.f),ColorAlpha(VP_WHITE,ra*shimmer3));
                DrawNeonCircle({sp.x,spY+breathe},S(7.f),cc,ra*shimmer3*0.65f);
                // arms — angled outward
                for(int side=-1;side<=1;side+=2){
                    float ax=sp.x+side*S(18.f);
                    DrawRectangle((int)(ax-S(3.f)),(int)(spY-S(6.f)+breathe),(int)S(6.f),(int)S(16.f),ColorAlpha({4,32,48,255},ra*0.88f));
                    DrawRectangleLines((int)(ax-S(3.f)),(int)(spY-S(6.f)+breathe),(int)S(6.f),(int)S(16.f),ColorAlpha(cc,ra*0.35f));
                    DrawNeonCircle({ax,spY+S(2.f)+breathe},S(2.5f),cc,ra*shimmer3*0.45f);
                    // hand crystal shard
                    DrawRectanglePro({ax,spY+S(10.f)+breathe,S(5.f),S(10.f)},{S(2.5f),S(0.f)},side*15.f,ColorAlpha({5,40,60,255},ra*0.82f));
                    DrawNeonCircle({ax,spY+S(14.f)+breathe},S(4.f),cc,ra*shimmer3*0.55f);
                }
                // head
                DrawRectangle((int)(sp.x-S(10.f)),(int)(spY-S(22.f)+breathe),(int)S(20.f),(int)S(15.f),ColorAlpha({3,28,42,255},ra*0.92f));
                DrawRectangleLines((int)(sp.x-S(10.f)),(int)(spY-S(22.f)+breathe),(int)S(20.f),(int)S(15.f),ColorAlpha(cc,ra*0.45f));
                // crown spikes
                for(int i=0;i<5;i++){
                    DrawRectanglePro({sp.x+(i-2)*S(4.f),spY-S(22.f)+breathe,S(3.f),S(6.f+i%2*3.f)},{S(1.5f),S(6.f+i%2*3.f)},0.f,ColorAlpha({5,42,62,255},ra*0.80f));
                    DrawNeonCircle({sp.x+(i-2)*S(4.f),spY-S(28.f)-i%2*S(3.f)+breathe},S(1.5f),cc,ra*shimmer3*0.55f);
                }
                // eyes
                for(int side=-1;side<=1;side+=2){
                    DrawCircleV({sp.x+side*S(4.f),spY-S(15.f)+breathe},S(3.f),ColorAlpha({2,10,18,255},ra*0.95f));
                    DrawCircleV({sp.x+side*S(4.f),spY-S(15.f)+breathe},S(1.8f),ColorAlpha(cc,ra*shimmer3*0.90f));
                    DrawNeonCircle({sp.x+side*S(4.f),spY-S(15.f)+breathe},S(4.f),cc,ra*shimmer3*0.60f);
                }
            }
            else if(chaos == 3){ // STORM WRAITH — crackling lightning-being, barely contained
                float bolt = fmodf(t*8.f+ph,1.f);
                float stormSpin=t*2.f+ph;
                float arc=sinf(t*12.f+ph);
                // plasma rings spinning
                for(int r=0;r<4;r++){
                    float rr=S(10.f+r*6.f);
                    float ra2=stormSpin*(r%2==0?1.f:-1.f);
                    DrawCircleLinesV({sp.x,spY},rr,ColorAlpha(cc,ra*(0.5f-r*0.08f)));
                    DrawNeonCircle({sp.x+cosf(ra2)*rr,spY+sinf(ra2)*rr},S(2.f),cc,ra*cPulse*0.60f);
                }
                // lightning bolts radiating out
                for(int i=0;i<8;i++){
                    float ba=stormSpin*0.3f+i*(2*PI/8.f);
                    float bl=S(20.f+sinf(t*6.f+i+ph)*8.f);
                    // zigzag bolt
                    Vector2 prev={sp.x+cosf(ba)*S(6.f),spY+sinf(ba)*S(6.f)};
                    for(int s=1;s<=4;s++){
                        float sf=(float)s/4.f;
                        float jitter=arc*(s%2==0?1.f:-1.f)*S(3.f);
                        Vector2 cur={sp.x+cosf(ba)*bl*sf+sinf(ba)*jitter,
                                     spY+sinf(ba)*bl*sf+cosf(ba)*jitter};
                        DrawLineEx(prev,cur,S(1.5f-sf*0.8f),ColorAlpha(cc,ra*(1.f-sf*0.4f)*((bolt>0.5f)?1.f:0.5f)));
                        prev=cur;
                    }
                    DrawNeonCircle(prev,S(2.f),cc,ra*cPulse*0.55f);
                }
                // crackling core
                DrawCircleV({sp.x,spY},S(7.f),ColorAlpha({8,2,2,255},ra*0.88f));
                DrawCircleV({sp.x,spY},S(5.f),ColorAlpha(cc,ra*cPulse*0.80f));
                DrawCircleV({sp.x,spY},S(2.5f),ColorAlpha(VP_WHITE,ra*cPulse));
                DrawNeonCircle({sp.x,spY},S(8.f),cc,ra*cPulse*0.75f);
            }
            else if(chaos == 4){ // LEVIATHAN EGG — a cracking shell leaking primordial chaos
                float crack4=0.4f+0.5f*fabsf(sinf(t*0.7f+ph));
                float gapY4=crack4*S(6.f);
                float innerGlow=0.6f+0.4f*sinf(t*2.2f+ph);
                // shell halves
                DrawEllipse((int)sp.x,(int)(spY-gapY4),(int)S(20.f),(int)S(16.f),ColorAlpha({30,10,5,255},ra*0.92f));
                DrawEllipseLines((int)sp.x,(int)(spY-gapY4),(int)S(20.f),(int)S(16.f),ColorAlpha(cc,ra*0.55f));
                DrawEllipse((int)sp.x,(int)(spY+gapY4),(int)S(20.f),(int)S(16.f),ColorAlpha({30,10,5,255},ra*0.92f));
                DrawEllipseLines((int)sp.x,(int)(spY+gapY4),(int)S(20.f),(int)S(16.f),ColorAlpha(cc,ra*0.55f));
                // shell surface scale texture
                for(int i=0;i<6;i++){
                    float sa2=ph+i*(2*PI/6.f);
                    DrawCircleV({sp.x+cosf(sa2)*S(14.f),spY-gapY4+sinf(sa2)*S(10.f)},S(2.f),ColorAlpha({55,15,8,255},ra*0.60f));
                }
                // inner chaos leaking from gap
                DrawEllipse((int)sp.x,(int)spY,(int)(S(16.f)*crack4),(int)(S(5.f)*crack4),ColorAlpha(cc,ra*innerGlow*0.50f));
                DrawNeonCircle({sp.x,spY},S(10.f)*crack4,cc,ra*innerGlow*0.60f);
                // chaos tentacles emerging
                for(int i=0;i<5;i++){
                    float ta=ph+i*(2*PI/5.f);
                    float tr=S(10.f)*crack4;
                    float tx2=sp.x+cosf(ta)*tr;
                    float ty2=spY+sinf(ta)*tr*0.4f;
                    DrawLineEx({sp.x,spY},{tx2,ty2},S(2.f),ColorAlpha(cc,ra*crack4*0.65f));
                    DrawNeonCircle({tx2,ty2},S(2.f),cc,ra*crack4*0.45f);
                }
                DrawCircleV({sp.x,spY},S(5.f)*crack4,ColorAlpha(VP_WHITE,ra*innerGlow*0.60f));
            }
            else if(chaos == 5){ // MIRROR SWARM — a thousand reflective shards orbiting a dark gravity
                float swarmRot=t*0.5f+ph;
                for(int ring=0;ring<3;ring++){
                    int n=6+ring*4;
                    float rr=S(10.f+ring*8.f);
                    float rOff=swarmRot*(ring%2==0?1.f:-1.f);
                    for(int i=0;i<n;i++){
                        float sa2=rOff+i*(2*PI/n);
                        float sx2=sp.x+cosf(sa2)*rr;
                        float sy2=spY+sinf(sa2)*rr;
                        float shardRot=(sa2+t*(ring+1)*0.8f)*RAD2DEG;
                        DrawRectanglePro({sx2,sy2,S(4.f+ring),S(2.f)},{S(2.f),S(1.f)},shardRot,ColorAlpha(cc,ra*0.65f));
                        DrawNeonCircle({sx2,sy2},S(1.2f),cc,ra*cPulse*0.40f);
                    }
                }
                // central gravity node
                DrawCircleV({sp.x,spY},S(7.f),ColorAlpha({5,2,15,255},ra*0.90f));
                DrawNeonCircle({sp.x,spY},S(8.f),cc,ra*cPulse*0.65f);
                // gravity rings contracting
                for(int i=0;i<3;i++){
                    float rAge=fmodf(t*0.6f+i*0.35f+ph,1.f);
                    DrawCircleLinesV({sp.x,spY},S(8.f+rAge*22.f),ColorAlpha(cc,ra*(1.f-rAge)*0.35f));
                }
                DrawCircleV({sp.x,spY},S(3.f),ColorAlpha(VP_WHITE,ra*cPulse*0.70f));
            }
            else if(chaos == 6){ // SOLAR DEVOURER — a black hole eating a star, accretion disk blazing
                float adSpin6=t*0.9f+ph;
                float flareAge=fmodf(t*1.4f+ph,1.f);
                // hot accretion disk — 4 rings
                for(int r=0;r<4;r++){
                    float rr=S(12.f+r*5.f);
                    Color rc=r==0?ColorAlpha(VP_WHITE,ra*0.35f):r==1?ColorAlpha(VP_YELLOW,ra*0.30f):r==2?ColorAlpha(VP_ORANGE,ra*0.25f):ColorAlpha(VP_RED,ra*0.18f);
                    DrawEllipseLines((int)sp.x,(int)spY,(int)rr,(int)(rr*0.35f),rc);
                    // hot nodes on ring
                    DrawNeonCircle({sp.x+cosf(adSpin6*(r+1))*rr,spY+sinf(adSpin6*(r+1))*rr*0.35f},S(2.f),cc,ra*cPulse*0.45f);
                }
                // infalling gas spirals
                for(int i=0;i<3;i++){
                    float ga=adSpin6+i*(2*PI/3.f);
                    for(int j=0;j<6;j++){
                        float jp=(float)j/6.f;
                        float gr=S(28.f-jp*20.f);
                        DrawCircleV({sp.x+cosf(ga+jp*1.5f)*gr,spY+sinf(ga+jp*1.5f)*gr*0.35f},S(1.f),ColorAlpha(cc,ra*0.30f*(1.f-jp)));
                    }
                }
                // solar flare eruption
                if(flareAge<0.6f){
                    float fa=flareAge/0.6f;
                    DrawLineEx({sp.x,spY},{sp.x+cosf(adSpin6*2.f)*S(30.f*fa),spY+sinf(adSpin6*2.f)*S(12.f*fa)},S(2.f),ColorAlpha(cc,ra*(1.f-fa)*0.55f));
                    DrawNeonCircle({sp.x+cosf(adSpin6*2.f)*S(30.f*fa),spY+sinf(adSpin6*2.f)*S(12.f*fa)},S(3.f+fa*4.f),cc,ra*(1.f-fa)*0.45f);
                }
                // event horizon — absolute black
                DrawCircleV({sp.x,spY},S(8.f),ColorAlpha({0,0,0,255},ra));
                DrawCircleV({sp.x,spY},S(6.f),ColorAlpha({2,0,2,255},ra));
                DrawNeonCircle({sp.x,spY},S(9.f),cc,ra*cPulse*0.55f);
                DrawCircleV({sp.x,spY},S(2.f),ColorAlpha({0,0,0,255},ra)); // centre always black
            }
            else{ // chaos==7: VOID OMEGA — the absolute end of everything
                float voidSpin=t*0.2f+ph;
                float collapse=0.5f+0.5f*sinf(t*0.4f+ph);
                float eyeOpen7=0.55f+0.45f*sinf(t*0.18f+ph);
                // reality erasure — dark rings expanding from nothing
                for(int i=0;i<5;i++){
                    float rAge=fmodf(t*0.3f+i*0.22f+ph,1.f);
                    DrawCircleLinesV({sp.x,spY},S(8.f+rAge*40.f),ColorAlpha({80,20,140,255},ra*(1.f-rAge)*0.22f));
                }
                // tendrils of void reaching outward
                for(int i=0;i<8;i++){
                    float ta=voidSpin+i*(2*PI/8.f);
                    float tLen=S(14.f+sinf(t*1.2f+i+ph)*6.f);
                    for(int s=0;s<5;s++){
                        float sf=(float)s/5.f;
                        DrawCircleV({sp.x+cosf(ta)*tLen*sf,spY+sinf(ta)*tLen*sf},
                            S(2.f-sf*1.2f),ColorAlpha(cc,ra*(1.f-sf)*cPulse*0.40f));
                    }
                    DrawNeonCircle({sp.x+cosf(ta)*tLen,spY+sinf(ta)*tLen},S(1.8f),cc,ra*cPulse*0.45f);
                }
                // the great eye of the void — slowly opening/closing
                DrawEllipse((int)sp.x,(int)spY,(int)S(20.f),(int)(S(12.f)*eyeOpen7+1),ColorAlpha({60,15,100,255},ra*0.35f));
                DrawEllipse((int)sp.x,(int)spY,(int)S(18.f),(int)(S(10.f)*eyeOpen7),ColorAlpha({3,0,8,255},ra*0.96f));
                DrawCircleV({sp.x,spY},S(9.f),ColorAlpha({20,4,40,255},ra*0.88f));
                DrawCircleV({sp.x,spY},S(6.f),ColorAlpha({60,10,110,255},ra*0.82f));
                // iris rings
                DrawNeonCircle({sp.x,spY},S(9.f),cc,ra*cPulse*0.50f);
                DrawNeonCircle({sp.x,spY},S(6.5f),VP_PURPLE,ra*0.30f);
                // vertical slit pupil — pure black
                DrawCircleV({sp.x,spY},S(4.f),ColorAlpha({0,0,0,255},ra));
                DrawEllipse((int)sp.x,(int)spY,(int)S(1.2f),(int)S(4.f),ColorAlpha({0,0,0,255},ra));
                DrawNeonCircle({sp.x,spY},S(4.5f),VP_HOTPINK,ra*cPulse*0.40f);
                // eyelashes
                for(int li=0;li<10;li++){
                    float la=li*(2*PI/10.f)+t*0.05f;
                    DrawLineEx({sp.x+cosf(la)*S(18.f)*eyeOpen7,spY+sinf(la)*S(10.f)*eyeOpen7},
                        {sp.x+cosf(la)*S(22.f)*eyeOpen7,spY+sinf(la)*S(12.5f)*eyeOpen7},
                        S(0.8f),ColorAlpha(cc,ra*eyeOpen7*0.55f));
                }
            }
            break;}

                }
 // end switch
    }
}
// ══════════════════════════════════════════════════════════════
void DrawVaporwaveSun(float t, int wave){
    float sx=SCREEN_W*0.5f, sy=80.f+sinf(t*.14f)*5.f;

    // ── Per-wave sun personality ──
    // Each wave gets a unique color palette, shape variant, and special effect
    struct SunTheme {
        Color layers[6];   // inner -> outer body gradient
        Color corona;      // outer glow color
        Color bloom;       // horizon bloom
        Color rim;         // rim light
        float size;        // body radius multiplier
        int   stripes;     // number of horizontal stripes (0 = no stripes)
        bool  fullCircle;  // false = classic lower-half only
        bool  pulsing;     // corona pulses
        bool  rotating;    // stripe colors rotate/cycle
    };

    SunTheme themes[20] = {
        // Wave 0: CRIMSON NEBULA — classic vaporwave magenta/red/orange half-sun
        {{{255,35,95,255},{255,75,55,255},{255,140,45,255},{255,200,70,255},{255,240,110,255},{255,252,170,255}},
         {255,60,120,255},{180,30,100,255},{255,200,80,255}, 1.0f, 9, false, false, false},
        // Wave 1: EMBER FIELDS — deep crimson to ember orange, wide low sun, dense stripes
        {{{140,10,20,255},{200,40,15,255},{240,90,20,255},{255,145,35,255},{255,200,70,255},{255,235,130,255}},
         {220,60,30,255},{120,15,10,255},{255,170,50,255}, 1.15f, 13, false, true, false},
        // Wave 2: OCEAN WORLD — deep blue to cyan, full circle
        {{{10,40,140,255},{15,80,200,255},{20,140,220,255},{30,190,240,255},{60,220,255,255},{180,240,255,255}},
         {0,160,255,255},{0,80,180,255},{60,220,255,255}, 1.1f, 7, true, true, false},
        // Wave 3: ABYSSAL TRENCH — near-black deep ocean, bioluminescent teal, full circle
        {{{2,10,35,255},{5,25,70,255},{8,50,110,255},{10,90,150,255},{15,150,180,255},{30,210,200,255}},
         {0,180,200,255},{0,30,80,255},{20,220,210,255}, 0.9f, 5, true, true, true},
        // Wave 4: TOXIC SWAMP — acid green to yellow, sickly rotating
        {{{30,100,10,255},{60,150,20,255},{100,200,30,255},{160,230,50,255},{210,255,80,255},{240,255,150,255}},
         {80,220,40,255},{40,120,10,255},{160,255,60,255}, 0.85f, 11, false, false, true},
        // Wave 5: SPORE WASTES — murky olive/yellow, pulsing with sickly bloom
        {{{55,80,5,255},{90,120,10,255},{130,160,15,255},{180,200,25,255},{220,235,55,255},{245,250,110,255}},
         {140,190,20,255},{60,90,5,255},{200,240,40,255}, 0.8f, 14, false, true, false},
        // Wave 6: VOID RIFT — deep purple to ultraviolet, full circle, rotating
        {{{50,5,100,255},{90,15,160,255},{130,30,210,255},{170,60,240,255},{200,100,255,255},{230,160,255,255}},
         {140,50,255,255},{60,10,120,255},{180,80,255,255}, 1.2f, 6, true, true, true},
        // Wave 7: PHANTOM GATE — near-black with ghostly indigo wisps, full circle, pulse
        {{{8,2,20,255},{20,5,45,255},{45,10,90,255},{80,20,140,255},{130,50,200,255},{190,110,255,255}},
         {160,70,255,255},{15,3,40,255},{200,120,255,255}, 1.05f, 4, true, true, true},
        // Wave 8: CRYSTAL CAVERNS — cool teal/mint, compact, fast-stripe
        {{{5,80,90,255},{10,130,140,255},{15,180,175,255},{25,220,200,255},{60,245,220,255},{160,255,240,255}},
         {20,220,210,255},{5,80,100,255},{60,240,220,255}, 0.9f, 10, false, false, false},
        // Wave 9: RESONANCE CORE — gold/white hot core, amber corona, intense pulse
        {{{180,100,10,255},{220,150,15,255},{250,200,30,255},{255,230,70,255},{255,248,140,255},{255,255,210,255}},
         {255,190,40,255},{160,90,5,255},{255,220,80,255}, 1.1f, 8, false, true, false},
        // Wave 10: STELLAR FORGE — fiery white-hot, orange corona, half-sun
        {{{220,80,10,255},{240,130,20,255},{255,180,40,255},{255,220,90,255},{255,245,160,255},{255,255,220,255}},
         {255,160,40,255},{200,80,10,255},{255,230,100,255}, 1.15f, 7, false, true, false},
        // Wave 11: SHARD TEMPEST — electric gold to white, full circle, fast rotation
        {{{160,120,5,255},{210,170,10,255},{245,210,25,255},{255,235,70,255},{255,250,140,255},{255,255,220,255}},
         {255,220,60,255},{140,100,5,255},{255,240,100,255}, 1.0f, 16, true, false, true},
        // Wave 12: QUANTUM DEPTHS — hot pink to deep magenta, full circle, pulsing
        {{{140,5,80,255},{200,15,110,255},{240,30,140,255},{255,60,170,255},{255,110,200,255},{255,180,230,255}},
         {255,80,180,255},{120,5,70,255},{255,140,210,255}, 1.1f, 7, true, true, false},
        // Wave 13: EVENT HORIZON — pitch black with only a violent magenta rim
        {{{5,0,8,255},{12,2,18,255},{22,4,35,255},{40,8,65,255},{80,15,120,255},{160,40,220,255}},
         {200,50,255,255},{8,2,15,255},{255,80,200,255}, 1.35f, 3, true, true, true},
        // Wave 14: SOLAR INFERNO — extreme orange/red, blazing, max brightness
        {{{200,30,0,255},{240,80,5,255},{255,140,15,255},{255,200,40,255},{255,240,100,255},{255,255,180,255}},
         {255,100,20,255},{180,20,0,255},{255,200,60,255}, 1.2f, 10, false, true, false},
        // Wave 15: CORONA BREACH — white core eruption, orange/gold plumes, rotating
        {{{255,200,100,255},{255,220,130,255},{255,235,160,255},{255,245,195,255},{255,252,220,255},{255,255,245,255}},
         {255,180,60,255},{220,140,20,255},{255,240,140,255}, 1.3f, 6, false, true, true},
        // Wave 16: OBSIDIAN WASTES — dark charcoal/grey, cold cyan rim, compact
        {{{18,18,28,255},{30,30,50,255},{45,50,75,255},{65,80,110,255},{90,120,160,255},{140,180,220,255}},
         {60,160,220,255},{10,10,22,255},{80,180,230,255}, 0.85f, 12, false, false, true},
        // Wave 17: ASHEN CITADEL — deep slate, smoldering ember glow, pulse
        {{{25,10,5,255},{55,22,8,255},{90,40,12,255},{130,70,18,255},{180,110,30,255},{230,170,70,255}},
         {200,120,40,255},{20,8,3,255},{240,160,50,255}, 0.95f, 9, false, true, false},
        // Wave 18: THE VOID — pure void, ghostly lavender wisps, full circle
        {{{3,0,8,255},{8,2,18,255},{18,5,38,255},{35,10,65,255},{70,20,110,255},{140,60,200,255}},
         {180,100,255,255},{5,1,12,255},{220,140,255,255}, 1.25f, 4, true, true, true},
        // Wave 19: VOID SINGULARITY — absolute black, blinding white-gold rim, max size
        {{{0,0,2,255},{4,1,8,255},{10,2,20,255},{25,5,45,255},{80,25,130,255},{200,120,255,255}},
         {255,220,180,255},{2,0,5,255},{255,240,200,255}, 1.5f, 3, true, true, true},
    };

    SunTheme& th = themes[std::min(wave, 19)];
    float bodyR = (22.f + 5*9.f) * th.size;
    float pulse = th.pulsing ? (0.85f + 0.15f * sinf(t * 1.8f)) : 1.0f;

    // Distant atmospheric horizon bloom
    for(int i=0;i<6;i++){
        float bh=28.f-i*3.f;
        DrawRectangle(0,(int)(sy+60+i*5),SCREEN_W,(int)bh,
            ColorAlpha(th.bloom,.018f-.002f*i));
    }

    // Deep outer corona rings
    float coronaR[]={bodyR+80,bodyR+55,bodyR+35,bodyR+18};
    float corA[]   ={.020f,.035f,.050f,.065f};
    for(int i=0;i<4;i++)
        DrawCircleV({sx,sy},coronaR[i]*pulse,ColorAlpha(th.corona,corA[i]*pulse));

    // Mid glow rings
    DrawCircleV({sx,sy},bodyR+28,ColorAlpha(th.corona,.09f*pulse));
    DrawCircleV({sx,sy},bodyR+12,ColorAlpha(th.layers[2],.12f*pulse));

    // Sun body
    float sizeR    = 22.f * th.size;

    if(th.stripes > 0){
        // UPPER HALF: always draw as gradient sectors (180°→360° = top in screen coords)
        for(int i=5;i>=0;i--){
            float r = sizeR + i * (9.f * th.size);
            DrawCircleSector({sx,sy},r,180.f,360.f,24,ColorAlpha(th.layers[i],.60f+i*.04f));
        }

        // LOWER HALF: scanline bands with TRUE GAPS (background shows through)
        // Classic retrowave look — sun-colored horizontal strips, background between them
        float slotH  = (bodyR * 0.94f) / (float)th.stripes;
        float bandFrac = 0.66f;   // 66% band, 34% gap

        for(int si = 0; si < th.stripes; si++){
            float bandTop = sy + si * slotH;
            float bandBot = bandTop + slotH * bandFrac;
            for(int py = (int)bandTop; py < (int)bandBot; py++){
                float fy = (float)py - sy;
                if(fy > bodyR) break;
                float chord2 = bodyR*bodyR - fy*fy;
                if(chord2 <= 0.f) break;
                float hw = sqrtf(chord2);
                // Gradient: interpolate layers by vertical position in lower half
                float pct  = fy / bodyR;              // 0 = center, 1 = bottom
                float lf   = pct * 5.f;
                int   li   = std::min((int)lf, 4);
                Color c    = LerpC(th.layers[li], th.layers[std::min(li+1,5)], lf - li);
                DrawRectangle((int)(sx - hw), py, (int)(hw * 2.f), 1,
                    ColorAlpha(c, 0.88f - si * 0.02f));
            }
        }

        // For full-circle suns also fill the upper half lower portion
        // (sectors already handle that above; nothing extra needed)
    } else {
        // No stripes — plain gradient circle or semicircle
        float startAng = th.fullCircle ? 0.f : 180.f;
        for(int i=5;i>=0;i--){
            float r = sizeR + i * (9.f * th.size);
            DrawCircleSector({sx,sy},r,startAng,360.f,24,ColorAlpha(th.layers[i],.55f+i*.04f));
        }
    }
    // Hot core
    DrawCircleV({sx,sy},sizeR,ColorAlpha({255,255,220,255},.22f));

    // Rotating corona tendrils for special suns (void, neutron)
    if(th.rotating){
        int tendrils = 8;
        for(int i=0;i<tendrils;i++){
            float a = t * 0.4f + i * (2*PI/tendrils);
            float r1 = bodyR * 0.85f, r2 = bodyR * (1.2f + 0.1f * sinf(t*2+i));
            Vector2 p1 = {sx + cosf(a)*r1, sy + sinf(a)*r1};
            Vector2 p2 = {sx + cosf(a)*r2, sy + sinf(a)*r2};
            DrawLineEx(p1, p2, 1.5f, ColorAlpha(th.corona, 0.20f + 0.12f*sinf(t*3+i)));
            DrawCircleV(p2, 2.f, ColorAlpha(th.corona, 0.35f));
        }
    }

    // Rim light
    DrawCircleLinesV({sx,sy},bodyR,ColorAlpha(th.rim,.22f));
    DrawCircleLinesV({sx,sy},bodyR+1,ColorAlpha(th.rim,.10f));

    // Soft horizon glow (very subtle, no hard lines)
    float hgY = sy + bodyR - 2;
    DrawRectangle(0,(int)hgY,SCREEN_W,2,ColorAlpha(th.bloom,.03f));
    DrawRectangle(0,(int)(hgY+2),SCREEN_W,1,ColorAlpha(th.layers[0],.02f));

    // Special: pulsing wave rings for ocean/ice suns
    if(th.pulsing && th.fullCircle){
        for(int i=0;i<3;i++){
            float pr = bodyR * (0.9f + 0.15f*i) + sinf(t*2+i)*8;
            DrawCircleLinesV({sx,sy}, pr, ColorAlpha(th.corona, 0.08f*pulse*(3-i)));
        }
    }
}

void DrawFloatingShapes(float t, int wave){
    // Per-wave themed wireframe geometry — each wave has its own shape set
    struct S{float x,y,sz,sp,ph,sz2; Color col; int sides;};

    // 20 wave-specific shape palettes
    static const S waveShapes[20][8]={
        // Wave 0: CRIMSON NEBULA — red/magenta diamonds & hexagons
        {{0.11f,0.20f,22,.22f,0.0f,10,{255,60,140,255},4},{0.88f,0.16f,18,-.16f,1.2f,8,{200,20,80,255},6},
         {0.74f,0.77f,28,.30f,2.5f,13,{255,100,60,255},4},{0.07f,0.80f,16,-.26f,4.1f,7,{220,40,100,255},3},
         {0.50f,0.08f,14,.18f,3.3f,6,{255,80,160,255},6},{0.30f,0.64f,12,-.38f,5.7f,5,{180,20,70,255},4},
         {0.62f,0.35f,10,.28f,7.1f,4,{255,50,120,255},3},{0.19f,0.50f,8,-.22f,2.8f,3,{200,30,90,255},6}},
        // Wave 1: EMBER FIELDS — orange/red triangles, jagged asymmetric
        {{0.15f,0.18f,26,.35f,0.5f,11,{255,110,20,255},3},{0.82f,0.22f,20,-.20f,1.8f,9,{220,60,10,255},3},
         {0.68f,0.72f,32,.25f,3.0f,15,{255,150,30,255},6},{0.10f,0.75f,18,-.30f,4.5f,8,{180,40,5,255},3},
         {0.45f,0.10f,16,.22f,2.8f,7,{240,90,15,255},4},{0.25f,0.60f,14,-.40f,6.2f,6,{255,130,25,255},3},
         {0.58f,0.30f,12,.32f,7.8f,5,{200,70,10,255},6},{0.22f,0.45f,9,-.18f,3.3f,4,{255,140,35,255},4}},
        // Wave 2: OCEAN WORLD — blue/cyan circles & soft hexagons
        {{0.12f,0.22f,24,.15f,0.2f,11,{40,180,255,255},6},{0.85f,0.18f,19,-.12f,1.4f,9,{20,140,220,255},6},
         {0.72f,0.75f,30,.20f,2.7f,14,{60,200,255,255},4},{0.08f,0.78f,17,-.22f,4.3f,8,{10,160,240,255},6},
         {0.48f,0.09f,15,.16f,3.5f,7,{30,190,250,255},6},{0.32f,0.62f,13,-.32f,5.9f,6,{50,170,230,255},4},
         {0.60f,0.38f,11,.24f,7.3f,5,{15,150,210,255},6},{0.20f,0.52f,8,-.20f,3.0f,4,{40,200,245,255},6}},
        // Wave 3: ABYSSAL TRENCH — near-black teal bioluminescent pentagons
        {{0.18f,0.25f,20,.10f,0.8f,9,{0,180,170,255},5},{0.80f,0.15f,16,-.10f,2.0f,7,{0,210,190,255},5},
         {0.65f,0.80f,26,.15f,3.2f,12,{5,160,150,255},6},{0.05f,0.70f,14,-.18f,4.8f,6,{0,190,180,255},5},
         {0.42f,0.12f,12,.12f,2.5f,5,{10,200,185,255},5},{0.28f,0.58f,10,-.28f,6.5f,4,{0,170,160,255},6},
         {0.55f,0.32f,8,.18f,8.2f,3,{5,215,200,255},5},{0.15f,0.48f,7,-.15f,4.0f,3,{0,195,175,255},5}},
        // Wave 4: TOXIC SWAMP — acid green jagged stars
        {{0.10f,0.20f,25,.40f,0.3f,10,{120,255,30,255},3},{0.85f,0.18f,21,-.28f,1.5f,10,{80,220,20,255},6},
         {0.70f,0.78f,34,.32f,2.8f,15,{160,255,50,255},3},{0.08f,0.72f,19,-.36f,4.6f,8,{100,240,25,255},4},
         {0.50f,0.07f,17,.24f,3.1f,8,{140,250,40,255},6},{0.28f,0.65f,15,-.44f,6.0f,6,{90,230,22,255},3},
         {0.62f,0.33f,13,.34f,7.6f,5,{130,245,35,255},3},{0.20f,0.48f,10,-.26f,2.5f,4,{110,235,28,255},6}},
        // Wave 5: SPORE WASTES — sickly olive/yellow, slow tumbling blobs
        {{0.14f,0.22f,23,.12f,0.6f,10,{180,200,20,255},6},{0.84f,0.20f,17,-.10f,1.8f,8,{155,178,14,255},4},
         {0.68f,0.76f,29,.16f,3.0f,13,{195,210,22,255},6},{0.06f,0.76f,15,-.20f,4.4f,7,{170,192,18,255},6},
         {0.46f,0.11f,13,.14f,2.9f,6,{185,205,21,255},4},{0.30f,0.63f,11,-.30f,6.3f,5,{160,185,15,255},6},
         {0.61f,0.36f,9,.22f,7.5f,4,{175,198,19,255},4},{0.18f,0.51f,7,-.16f,3.2f,3,{165,190,17,255},6}},
        // Wave 6: VOID RIFT — deep purple ultraviolet spiky stars
        {{0.13f,0.21f,27,.38f,0.1f,12,{180,60,255,255},4},{0.87f,0.17f,22,-.22f,1.6f,10,{140,40,220,255},6},
         {0.73f,0.79f,35,.28f,2.6f,16,{200,80,255,255},4},{0.06f,0.77f,19,-.34f,4.2f,8,{160,55,240,255},3},
         {0.49f,0.08f,17,.20f,3.4f,8,{190,70,250,255},6},{0.31f,0.64f,15,-.42f,5.8f,6,{150,45,230,255},4},
         {0.63f,0.34f,12,.30f,7.2f,5,{170,50,245,255},3},{0.21f,0.49f,9,-.24f,2.7f,4,{155,42,235,255},6}},
        // Wave 7: PHANTOM GATE — deep indigo near-invisible ghostly shapes
        {{0.16f,0.23f,30,.08f,0.9f,14,{130,80,220,255},6},{0.81f,0.19f,25,-.06f,2.1f,11,{110,60,200,255},4},
         {0.67f,0.81f,38,.12f,3.3f,18,{150,100,240,255},6},{0.04f,0.73f,22,-.16f,4.9f,9,{120,70,210,255},6},
         {0.44f,0.10f,20,.10f,2.6f,9,{140,90,230,255},4},{0.27f,0.67f,17,-.26f,6.6f,7,{100,55,195,255},6},
         {0.57f,0.31f,14,.20f,8.4f,6,{160,110,245,255},6},{0.17f,0.46f,11,-.14f,4.1f,5,{115,65,205,255},4}},
        // Wave 8: CRYSTAL CAVERNS — teal/mint angular crystals
        {{0.11f,0.22f,24,.18f,0.4f,11,{0,220,210,255},4},{0.86f,0.16f,20,-.14f,1.3f,9,{20,240,220,255},6},
         {0.72f,0.76f,31,.22f,2.9f,14,{10,200,190,255},4},{0.07f,0.79f,18,-.24f,4.7f,8,{30,230,215,255},3},
         {0.47f,0.09f,16,.16f,3.6f,7,{0,210,200,255},6},{0.33f,0.63f,14,-.34f,6.1f,6,{15,225,208,255},4},
         {0.61f,0.37f,11,.26f,7.4f,5,{5,235,222,255},3},{0.22f,0.50f,8,-.20f,3.1f,4,{25,215,205,255},6}},
        // Wave 9: RESONANCE CORE — gold/amber pulsing hexagons
        {{0.12f,0.21f,26,.20f,0.3f,12,{255,210,40,255},6},{0.84f,0.17f,21,-.16f,1.7f,10,{220,170,20,255},6},
         {0.71f,0.78f,33,.24f,3.1f,15,{255,230,60,255},6},{0.07f,0.77f,20,-.28f,4.5f,9,{240,195,30,255},4},
         {0.48f,0.08f,18,.18f,3.3f,8,{255,220,50,255},6},{0.30f,0.62f,16,-.38f,6.0f,7,{230,180,25,255},6},
         {0.62f,0.35f,13,.28f,7.7f,6,{245,205,35,255},6},{0.20f,0.51f,10,-.22f,2.8f,5,{255,215,45,255},6}},
        // Wave 10: STELLAR FORGE — blazing orange/white forge sparks
        {{0.10f,0.20f,28,.44f,0.2f,13,{255,180,40,255},3},{0.86f,0.15f,23,-.30f,1.4f,11,{255,140,20,255},4},
         {0.73f,0.80f,36,.36f,2.7f,17,{255,200,60,255},3},{0.05f,0.75f,21,-.40f,4.3f,9,{240,160,30,255},3},
         {0.48f,0.07f,19,.28f,3.0f,9,{255,190,50,255},6},{0.28f,0.65f,17,-.48f,5.7f,7,{230,150,25,255},3},
         {0.63f,0.33f,14,.36f,7.0f,6,{255,170,35,255},4},{0.21f,0.47f,11,-.28f,2.6f,5,{245,185,45,255},3}},
        // Wave 11: SHARD TEMPEST — electric gold spinning stars
        {{0.11f,0.19f,25,.50f,0.0f,12,{255,240,80,255},6},{0.87f,0.15f,20,-.34f,1.1f,9,{220,200,50,255},4},
         {0.75f,0.77f,30,.40f,2.4f,14,{255,250,100,255},6},{0.06f,0.81f,17,-.42f,4.0f,8,{240,220,70,255},6},
         {0.49f,0.09f,15,.30f,3.2f,7,{255,245,90,255},6},{0.31f,0.63f,13,-.52f,5.6f,6,{210,185,45,255},6},
         {0.61f,0.36f,10,.38f,7.0f,5,{248,235,75,255},6},{0.19f,0.52f,8,-.30f,2.9f,4,{230,210,60,255},4}},
        // Wave 12: QUANTUM DEPTHS — hot pink/magenta deep pulsers
        {{0.13f,0.22f,27,.26f,0.5f,12,{255,60,180,255},6},{0.85f,0.18f,22,-.18f,1.6f,10,{220,30,140,255},4},
         {0.70f,0.79f,34,.30f,2.6f,16,{255,80,200,255},6},{0.07f,0.78f,20,-.30f,4.4f,9,{240,50,165,255},3},
         {0.50f,0.08f,18,.22f,3.5f,8,{255,70,190,255},4},{0.30f,0.64f,16,-.40f,5.9f,7,{210,25,130,255},6},
         {0.62f,0.34f,13,.30f,7.3f,6,{245,65,185,255},3},{0.21f,0.50f,10,-.24f,2.7f,5,{225,40,155,255},6}},
        // Wave 13: EVENT HORIZON — near-black with blinding violet sparks
        {{0.14f,0.23f,32,.14f,0.7f,15,{200,50,255,255},4},{0.83f,0.17f,26,-.10f,2.0f,12,{160,30,220,255},6},
         {0.68f,0.80f,40,.18f,3.1f,19,{220,70,255,255},4},{0.05f,0.74f,24,-.18f,4.8f,10,{180,40,240,255},3},
         {0.45f,0.11f,22,.14f,2.5f,10,{210,60,250,255},6},{0.26f,0.66f,19,-.28f,6.7f,8,{150,25,210,255},4},
         {0.58f,0.30f,16,.22f,8.5f,7,{230,80,255,255},3},{0.16f,0.47f,12,-.16f,4.2f,6,{170,35,230,255},6}},
        // Wave 14: SOLAR INFERNO — blazing white-orange sunbursts
        {{0.10f,0.19f,30,.52f,0.1f,14,{255,200,80,255},6},{0.88f,0.14f,25,-.36f,1.2f,12,{255,160,40,255},4},
         {0.74f,0.81f,38,.42f,2.5f,18,{255,220,100,255},6},{0.04f,0.74f,22,-.46f,4.1f,10,{240,170,50,255},6},
         {0.47f,0.08f,20,.32f,2.9f,9,{255,210,90,255},6},{0.27f,0.66f,18,-.56f,5.5f,8,{230,155,35,255},4},
         {0.64f,0.32f,15,.40f,6.9f,7,{255,190,65,255},6},{0.22f,0.46f,12,-.32f,2.4f,6,{248,180,55,255},6}},
        // Wave 15: CORONA BREACH — white-gold eruption halos
        {{0.12f,0.20f,28,.22f,0.4f,13,{255,250,200,255},6},{0.86f,0.16f,23,-.16f,1.5f,11,{255,240,170,255},6},
         {0.72f,0.78f,35,.26f,2.8f,16,{255,255,210,255},6},{0.06f,0.76f,21,-.26f,4.6f,9,{245,235,180,255},6},
         {0.48f,0.09f,19,.18f,3.4f,9,{255,248,195,255},6},{0.29f,0.64f,17,-.36f,6.2f,8,{240,230,165,255},6},
         {0.63f,0.35f,14,.24f,7.6f,7,{255,244,185,255},6},{0.20f,0.50f,11,-.20f,3.0f,6,{248,238,175,255},6}},
        // Wave 16: OBSIDIAN WASTES — cold steel/charcoal angular shards
        {{0.11f,0.22f,26,.16f,0.6f,12,{80,160,220,255},4},{0.85f,0.18f,21,-.12f,1.9f,10,{60,140,200,255},6},
         {0.70f,0.79f,33,.20f,3.2f,15,{90,170,230,255},4},{0.07f,0.77f,19,-.22f,4.7f,9,{70,150,210,255},3},
         {0.47f,0.10f,17,.14f,3.5f,8,{85,165,225,255},6},{0.30f,0.63f,15,-.32f,6.4f,7,{55,130,195,255},4},
         {0.62f,0.36f,12,.22f,7.9f,6,{95,175,235,255},3},{0.21f,0.51f,9,-.18f,3.3f,5,{65,145,205,255},6}},
        // Wave 17: ASHEN CITADEL — smouldering amber/dark ember forms
        {{0.13f,0.21f,25,.18f,0.5f,11,{200,120,35,255},4},{0.84f,0.17f,20,-.14f,1.8f,9,{180,100,25,255},6},
         {0.71f,0.80f,32,.22f,3.0f,15,{220,140,45,255},4},{0.06f,0.78f,18,-.24f,4.6f,8,{190,110,30,255},3},
         {0.49f,0.08f,16,.16f,3.3f,8,{210,130,40,255},6},{0.31f,0.63f,14,-.34f,6.1f,7,{170,95,22,255},4},
         {0.61f,0.35f,11,.26f,7.7f,6,{215,135,42,255},3},{0.20f,0.50f,8,-.20f,3.1f,5,{185,105,28,255},6}},
        // Wave 18: THE VOID — ghostly lavender wisps, slow and ethereal
        {{0.14f,0.24f,35,.06f,1.0f,16,{180,100,255,255},6},{0.82f,0.18f,28,-.05f,2.2f,13,{150,70,240,255},4},
         {0.67f,0.82f,44,.09f,3.4f,20,{200,130,255,255},6},{0.04f,0.75f,26,-.12f,5.0f,11,{160,85,245,255},6},
         {0.44f,0.11f,24,.08f,2.7f,11,{190,115,255,255},4},{0.25f,0.68f,21,-.20f,6.8f,9,{140,60,230,255},6},
         {0.57f,0.29f,18,.14f,8.7f,8,{210,140,255,255},6},{0.15f,0.45f,14,-.10f,4.3f,7,{155,75,240,255},4}},
        // Wave 19: VOID SINGULARITY — blinding white-gold max chaos
        {{0.10f,0.18f,40,.70f,0.0f,19,{255,240,200,255},8},{0.90f,0.14f,34,-.50f,1.0f,16,{255,220,160,255},6},
         {0.75f,0.83f,50,.60f,2.3f,24,{255,250,210,255},8},{0.03f,0.72f,30,-.60f,3.9f,14,{240,200,140,255},4},
         {0.46f,0.07f,28,.44f,2.8f,13,{255,235,180,255},6},{0.25f,0.70f,25,-.70f,5.3f,11,{230,190,130,255},8},
         {0.65f,0.28f,22,.54f,6.8f,10,{255,245,195,255},4},{0.18f,0.44f,18,-.46f,3.8f,8,{245,215,155,255},6}},
    };

    int wi = std::min(wave, 19);
    const S* shapes = waveShapes[wi];

    for(int si=0;si<8;si++){
        const S& s=shapes[si];
        float sx=s.x*SCREEN_W+sinf(t*.12f+s.ph)*12;
        float sy=s.y*SCREEN_H+cosf(t*.09f+s.ph)*8;
        float ang=t*s.sp+s.ph;
        // Wave-specific alpha: void/singularity waves are more subtle; forge/inferno more vivid
        float baseAlpha=(wi>=18)?.06f:(wi>=13)?.12f:(wi>=9)?.11f:.10f;
        float alpha=baseAlpha+sinf(t*.35f+s.ph)*.05f;

        int n=s.sides;
        Vector2 outer[8],inner[8];
        for(int i=0;i<n;i++){
            float a=ang+i*(2*PI/n);
            outer[i]={sx+cosf(a)*s.sz,  sy+sinf(a)*s.sz};
            inner[i]={sx+cosf(a+PI/n)*s.sz2,sy+sinf(a+PI/n)*s.sz2};
        }
        // outer wireframe only (skip fill triangles — too faint to notice)
        for(int i=0;i<n;i++) DrawLineV(outer[i],outer[(i+1)%n],ColorAlpha(s.col,alpha*1.4f));
        // inner ring
        for(int i=0;i<n;i++) DrawLineV(inner[i],inner[(i+1)%n],ColorAlpha(s.col,alpha*.55f));
        // center dot
        DrawCircleV({sx,sy},1.5f,ColorAlpha(s.col,alpha*2.f));
    }
}

void DrawPlanets(Game& g, float t){
    for(auto& p:g.planets){
        float px=p.pos.x - g.cameraPos.x*p.parallax;
        float py=p.pos.y - g.cameraPos.y*p.parallax;
        float r=p.radius;
        Color ac=p.col; // atmosphere color
        Color rc=p.ringCol; // ring/accent color

        // ── Far atmosphere glow (outermost) ──
        for(int i=4;i>=0;i--){
            float gr=r*(1.f+.22f*i);
            float ga=(.008f+.012f*(5-i))*(.8f+.2f*sinf(t*.35f+p.phase));
            DrawCircleV({px,py},gr,ColorAlpha(ac,ga));
        }

        // ── Back ring half (behind planet) ──
        if(p.hasRing){
            float ra=p.ringAngle;
            int segs=48;
            // Outer ring
            float rx1=r*2.2f, ry1=r*.40f;
            // Inner ring edge
            float rx2=r*1.50f;
            // Draw outer ring back half
            for(int i=0;i<segs;i++){
                float a1=i*(2*PI/segs), a2=(i+1)*(2*PI/segs);
                if(sinf(a1)>0) continue; // back half only
                float x1=px+cosf(a1)*rx1, y1=py+sinf(a1)*ry1*sinf(ra);
                float x2=px+cosf(a2)*rx1, y2=py+sinf(a2)*ry1*sinf(ra);
                float fade=0.3f+0.3f*fabsf(cosf(a1));
                DrawLineEx({x1,y1},{x2,y2},3.0f,ColorAlpha(rc,fade*.35f));
                DrawLineEx({x1,y1},{x2,y2},1.2f,ColorAlpha(VP_WHITE,fade*.12f));
            }
            // Inner ring gap fill (dark)
            for(int i=0;i<segs;i++){
                float a1=i*(2*PI/segs), a2=(i+1)*(2*PI/segs);
                if(sinf(a1)>0) continue;
                float x1=px+cosf(a1)*rx2, y1=py+sinf(a1)*rx2*.18f*sinf(ra);
                float x2=px+cosf(a2)*rx2, y2=py+sinf(a2)*rx2*.18f*sinf(ra);
                DrawLineEx({x1,y1},{x2,y2},1.8f,ColorAlpha(rc,.18f));
            }
        }

        // ── Planet body base ──
        // Darkened base fill (deep space color)
        DrawCircleV({px,py},r,{5,2,18,255});
        // Main color layer (two-tone gradient via sectors)
        DrawCircleSector({px,py},r, 0,180,20,ColorAlpha(ac,.82f));
        DrawCircleSector({px,py},r,180,360,20,ColorAlpha(ac,.65f));

        // ── Surface detail by pattern type ──
        if(p.patternType==0){
            // CONTINENTAL WORLD — tectonic plates, oceans, forests
            // Ocean fill (slightly darker base)
            DrawCircleV({px,py},r,ColorAlpha({(unsigned char)(ac.r/3),(unsigned char)(ac.g/3+20),(unsigned char)(ac.b/2),255},.40f));

            // Continent blobs using fixed offsets (deterministic from phase seed)
            float seed=p.phase;
            struct CBlob{ float ox,oy,rx,ry,ang; Color c; };
            CBlob blobs[]={
                { cosf(seed)*r*.35f,      sinf(seed)*r*.25f,      r*.38f, r*.22f, seed,         ColorAlpha(ac,.90f)},
                { cosf(seed+2.1f)*r*.3f,  sinf(seed+2.1f)*r*.3f,  r*.28f, r*.18f, seed+1.f,     ColorAlpha(ac,.85f)},
                { cosf(seed+4.2f)*r*.28f, sinf(seed+4.2f)*r*.2f,  r*.22f, r*.14f, seed+2.2f,    ColorAlpha(ac,.80f)},
                { cosf(seed+1.f)*r*.12f,  sinf(seed+3.f)*r*.35f,  r*.16f, r*.10f, seed+3.f,     ColorAlpha(ac,.75f)},
            };
            for(auto& b2:blobs){
                // Ellipse approximation: fill a circle sector-based shape
                int steps=18;
                for(int i=0;i<steps-1;i++){
                    float a1=i*(2*PI/steps), a2=(i+1)*(2*PI/steps);
                    float cx1=cosf(a1+b2.ang)*b2.rx+b2.ox+px;
                    float cy1=sinf(a1+b2.ang)*b2.ry+b2.oy+py;
                    float cx2=cosf(a2+b2.ang)*b2.rx+b2.ox+px;
                    float cy2=sinf(a2+b2.ang)*b2.ry+b2.oy+py;
                    // clip to planet disc
                    float d1=sqrtf((cx1-px)*(cx1-px)+(cy1-py)*(cy1-py));
                    float d2=sqrtf((cx2-px)*(cx2-px)+(cy2-py)*(cy2-py));
                    if(d1<r && d2<r)
                        DrawTriangle({px+b2.ox,py+b2.oy},{cx1,cy1},{cx2,cy2},b2.c);
                }
            }
            // Forest fringe (darker green edge on continents)
            Color forestC={0,(unsigned char)(ac.g/2+30),0,255};
            for(auto& b2:blobs){
                int steps=18;
                for(int i=0;i<steps;i++){
                    float a1=i*(2*PI/steps);
                    float ex=cosf(a1+b2.ang)*b2.rx+b2.ox+px;
                    float ey=sinf(a1+b2.ang)*b2.ry+b2.oy+py;
                    float d=sqrtf((ex-px)*(ex-px)+(ey-py)*(ey-py));
                    if(d<r*.95f) DrawCircleV({ex,ey},r*.025f,ColorAlpha(forestC,.55f));
                }
            }
            for(int i=0;i<6;i++){
                float by=py - r*.7f + i*(r*.26f);
                float fy=by - py;
                if(fabsf(fy)>=r*.92f) continue;
                float chord=sqrtf(r*r - fy*fy);
                float waveOff=sinf(p.phase*1.3f+i*1.7f+t*.04f)*r*.08f;
                float w2=chord*(0.5f+0.35f*sinf(p.phase+i*2.f));
                float bh=std::max(2.f, r*.045f);
                // Centre the band, apply wave, then clamp so it never exits the chord
                float cx=px + waveOff;
                float lx=cx - w2*.5f;
                float rx=cx + w2*.5f;
                // clamp to planet disc at this y
                lx=std::max(lx, px-chord*.97f);
                rx=std::min(rx, px+chord*.97f);
                if(rx>lx)
                    DrawRectangle((int)lx,(int)by,(int)(rx-lx),(int)bh,ColorAlpha(VP_WHITE,.15f));
            }
            // Polar ice cap (top)
            DrawCircleSector({px,py-r*.82f},r*.25f,0,180,12,ColorAlpha(VP_WHITE,.32f));
            DrawCircleSector({px,py-r*.88f},r*.15f,0,180,8, ColorAlpha(VP_WHITE,.20f));
            // Night-side city lights (bottom-right dark area)
            for(int i=0;i<8;i++){
                float la=p.phase+i*0.7f;
                float lx=px+cosf(la)*r*(.4f+sinf(la*2.1f)*.3f);
                float ly=py+sinf(la)*r*(.3f+cosf(la*1.3f)*.25f);
                float ld=sqrtf((lx-px)*(lx-px)+(ly-py)*(ly-py));
                if(ld<r*.9f){
                    float lbright=.5f+.5f*sinf(t*2.5f+i*1.3f);
                    DrawCircleV({lx,ly},1.5f,ColorAlpha(VP_YELLOW,lbright*.35f));
                }
            }
        } else if(p.patternType==1){
            // DEAD ROCKY WORLD — craters, lava veins, no atmosphere
            // Base rocky texture via bands clipped to disc (single rect per band)
            for(int i=0;i<8;i++){
                float by=py-r+i*(r*.28f);
                float bh2=std::max(2.f,r*.05f);
                Color bc=(i%3==0)?ColorAlpha(VP_BLACK,.14f):(i%3==1)?ColorAlpha(ac,.06f):ColorAlpha({0,0,0,255},.04f);
                for(int row=(int)by; row<(int)(by+bh2); row++){
                    float fy2=(float)row - py;
                    if(fabsf(fy2)>=r*.92f) continue;
                    float hw=sqrtf(std::max(0.f, r*r - fy2*fy2))*.92f;
                    DrawRectangle((int)(px-hw), row, (int)(hw*2.f), 1, bc);
                }
            }
            // Impact craters — ringed structures with central peaks
            float cpos[][3]={{.42f,-.28f,1.f},{-.32f,.38f,.8f},{.08f,.52f,.65f},{-.48f,-.15f,.9f},{.2f,-.48f,.55f}};
            for(int i=0;i<5;i++){
                float cx2=px+cpos[i][0]*r, cy2=py+cpos[i][1]*r;
                float cr=r*(.10f+i*.028f)*cpos[i][2];
                float cd=sqrtf((cx2-px)*(cx2-px)+(cy2-py)*(cy2-py));
                if(cd+cr>r*.92f) continue; // skip if outside planet
                // Crater floor (dark)
                DrawCircleV({cx2,cy2},cr,ColorAlpha(VP_BLACK,.35f));
                // Shadow arc on interior (directional lighting)
                DrawCircleSector({cx2+cr*.2f,cy2+cr*.2f},cr*.85f,200,380,10,ColorAlpha(VP_BLACK,.28f));
                // Rim highlight
                DrawCircleLinesV({cx2,cy2},cr,ColorAlpha(VP_WHITE,.18f));
                DrawCircleLinesV({cx2,cy2},cr+1.f,ColorAlpha(VP_WHITE,.08f));
                // Central peak
                if(cr>r*.09f) DrawCircleV({cx2-cr*.1f,cy2-cr*.1f},cr*.18f,ColorAlpha(VP_WHITE,.22f));
                // Ejecta rays (4 radial streaks outward)
                for(int j=0;j<4;j++){
                    float ra=cpos[i][0]+j*(PI*.5f)+p.phase;
                    float rlen=cr*(1.8f+sinf(ra)*0.5f);
                    Vector2 rEnd={cx2+cosf(ra)*rlen,cy2+sinf(ra)*rlen};
                    float rd=sqrtf((rEnd.x-px)*(rEnd.x-px)+(rEnd.y-py)*(rEnd.y-py));
                    if(rd<r*.9f) DrawLineEx({cx2,cy2},rEnd,0.8f,ColorAlpha(VP_WHITE,.10f));
                }
            }
            // Lava/magma vein network
            Color lavaC=rc; // use ring color as lava tint
            for(int i=0;i<5;i++){
                float va=p.phase+i*1.26f;
                float vx1=px+cosf(va)*r*.25f, vy1=py+sinf(va)*r*.2f;
                float vx2=px+cosf(va+.5f)*r*.5f, vy2=py+sinf(va+.6f)*r*.45f;
                float vd=sqrtf((vx2-px)*(vx2-px)+(vy2-py)*(vy2-py));
                if(vd<r*.85f){
                    float lp=.3f+.5f*sinf(t*3.f+i*1.4f); // lava pulse
                    DrawLineEx({vx1,vy1},{vx2,vy2},2.2f,ColorAlpha(lavaC,lp*.25f));
                    DrawLineEx({vx1,vy1},{vx2,vy2},0.8f,ColorAlpha(VP_YELLOW,lp*.18f));
                }
            }
        } else {
            // GAS GIANT / STORM WORLD — dramatic swirling bands, great storm eye
            Color bandCols[6];
            for(int i=0;i<6;i++){
                float blend=(i%2==0)?.0f:.45f;
                bandCols[i]=LerpC(ac,{5,2,18,255},blend);
            }
            for(int i=0;i<9;i++){
                float by=py - r*(0.95f) + i*(r*2*.95f/9.f);
                float bh=std::max(2.f, r*2*.95f/9.f);
                Color bc=bandCols[i%6];
                // warp applied as a y-shift into the chord calculation so bands
                // wiggle naturally without ever bleeding outside the circle
                float warp=sinf(p.phase*1.1f+i*0.9f+t*.06f)*r*.07f;
                bool topEdge=true;
                for(int row=(int)by; row<(int)(by+bh); row++){
                    float fy2=(float)row - py + warp;
                    if(fabsf(fy2)>=r*.93f) continue;
                    float hw=sqrtf(r*r - fy2*fy2)*.93f;
                    DrawRectangle((int)(px-hw), row, (int)(hw*2.f), 1, ColorAlpha(bc,.72f));
                    if(topEdge){
                        DrawRectangle((int)(px-hw), row, (int)(hw*2.f), 1, ColorAlpha(VP_WHITE,.05f));
                        topEdge=false;
                    }
                }
            }
            // Turbulence wisps along band edges
            for(int i=0;i<5;i++){
                float wy=py-r*.5f+i*r*.25f;
                float wy_dy=wy-py;
                if(fabsf(wy_dy)>=r*.9f) continue;
                float wchord=sqrtf(r*r-wy_dy*wy_dy);
                for(int j=0;j<3;j++){
                    float wx=px-wchord*.8f+j*(wchord*.8f)+sinf(t*.2f+i+j)*r*.05f;
                    float wd=sqrtf((wx-px)*(wx-px)+(wy-py)*(wy-py));
                    if(wd<r*.88f)
                        DrawCircleV({wx,wy},r*.05f,ColorAlpha(VP_WHITE,.06f+j*.02f));
                }
            }
            // Great storm eye — animated rotating oval
            float eyeX=px+cosf(t*.08f+p.phase)*r*.2f;
            float eyeY=py+sinf(t*.06f+p.phase)*r*.12f;
            float eyeR=r*.22f;
            float eyeD=sqrtf((eyeX-px)*(eyeX-px)+(eyeY-py)*(eyeY-py));
            if(eyeD+eyeR<r*.88f){
                // Outer swirl ring
                DrawCircleV({eyeX,eyeY},eyeR*1.4f,ColorAlpha(rc,.12f));
                DrawNeonCircle({eyeX,eyeY},eyeR*1.35f,rc,.25f);
                // Mid storm wall
                DrawCircleV({eyeX,eyeY},eyeR,ColorAlpha(rc,.22f));
                DrawNeonCircle({eyeX,eyeY},eyeR,rc,.45f);
                // Inner calm eye
                DrawCircleV({eyeX,eyeY},eyeR*.45f,ColorAlpha(VP_WHITE,.12f));
                DrawCircleV({eyeX,eyeY},eyeR*.2f, ColorAlpha(VP_WHITE,.20f));
                // Swirl spokes
                for(int i=0;i<6;i++){
                    float sa=t*.35f+i*(2*PI/6.f)+p.phase;
                    Vector2 s1={eyeX+cosf(sa)*eyeR*.55f, eyeY+sinf(sa)*eyeR*.55f};
                    Vector2 s2={eyeX+cosf(sa+.4f)*eyeR*1.25f, eyeY+sinf(sa+.4f)*eyeR*1.25f};
                    DrawLineEx(s1,s2,1.2f,ColorAlpha(VP_WHITE,.14f));
                }
            }
        }

        // ── Night-side shadow (directional lighting from top-left) ──
        DrawCircleSector({px,py},r,215,395,24,ColorAlpha({0,0,8,255},.52f));

        // ── Specular highlight (top-left limb) ──
        DrawCircleV({px-r*.30f,py-r*.34f},r*.26f,ColorAlpha(VP_WHITE,.12f));
        DrawCircleV({px-r*.24f,py-r*.28f},r*.13f,ColorAlpha(VP_WHITE,.11f));
        DrawCircleV({px-r*.20f,py-r*.24f},r*.06f,ColorAlpha(VP_WHITE,.14f));

        // ── Atmosphere rim (inner glow at edge) ──
        DrawCircleLinesV({px,py},r-1,ColorAlpha(ac,.15f));
        DrawCircleLinesV({px,py},r,  ColorAlpha(ac,.35f+.10f*sinf(t*.5f+p.phase)));
        DrawCircleLinesV({px,py},r+1,ColorAlpha(ac,.14f));
        DrawCircleLinesV({px,py},r+3,ColorAlpha(ac,.06f));

        // ── Ring system ──
        if(p.hasRing){
            float ra=p.ringAngle;
            int segs=48;
            float rx1=r*2.2f, ry1=r*.40f;
            float rx2=r*1.50f;
            // Redraw planet over back rings
            DrawCircleV({px,py},r,{5,2,18,255});
            DrawCircleSector({px,py},r, 0,180,20,ColorAlpha(ac,.82f));
            DrawCircleSector({px,py},r,180,360,20,ColorAlpha(ac,.65f));
            DrawCircleSector({px,py},r,215,395,24,ColorAlpha({0,0,8,255},.52f));
            DrawCircleV({px-r*.30f,py-r*.34f},r*.26f,ColorAlpha(VP_WHITE,.12f));
            DrawCircleLinesV({px,py},r,ColorAlpha(ac,.35f));
            // Front ring half
            for(int i=0;i<segs;i++){
                float a1=i*(2*PI/segs), a2=(i+1)*(2*PI/segs);
                if(sinf(a1)<=0) continue; // front half only
                float x1=px+cosf(a1)*rx1, y1=py+sinf(a1)*ry1*sinf(ra);
                float x2=px+cosf(a2)*rx1, y2=py+sinf(a2)*ry1*sinf(ra);
                float fade=0.3f+0.5f*fabsf(cosf(a1));
                DrawLineEx({x1,y1},{x2,y2},3.2f,ColorAlpha(rc,fade*.60f));
                DrawLineEx({x1,y1},{x2,y2},1.2f,ColorAlpha(VP_WHITE,fade*.20f));
            }
            // Inner ring (front half)
            for(int i=0;i<segs;i++){
                float a1=i*(2*PI/segs), a2=(i+1)*(2*PI/segs);
                if(sinf(a1)<=0) continue;
                float x1=px+cosf(a1)*rx2, y1=py+sinf(a1)*rx2*.18f*sinf(ra);
                float x2=px+cosf(a2)*rx2, y2=py+sinf(a2)*rx2*.18f*sinf(ra);
                DrawLineEx({x1,y1},{x2,y2},1.6f,ColorAlpha(rc,.38f));
            }
        }
    }
}

void UpdateShootingStars(Game& g, float dt){
    g.shootStarTimer-=dt;
    if(g.shootStarTimer<=0){
        g.shootStarTimer=RandF(5.f,14.f); // much rarer
        ShootStar ss;
        if(RandI(0,1)==0){ ss.pos={RandF(0,(float)SCREEN_W),RandF(-5,5)};ss.vel={RandF(-40,40),RandF(100,180)};}
        else              { ss.pos={RandF(-5,5),RandF(0,SCREEN_H*.4f)};  ss.vel={RandF(100,200),RandF(20,80)};}
        ss.len=RandF(10,28); ss.active=true;  // shorter
        ss.maxLife=ss.life=RandF(.3f,.6f);
        // Per-wave shooting star color palettes
        Color waveColPalettes[28][3]={
            {{255,80,160,255},{255,140,100,255},{255,200,220,255}},  // 0 crimson nebula
            {{255,130,30,255},{255,80,10,255}, {255,200,80,255}},    // 1 ember fields
            {{40,200,255,255},{20,140,220,255},{180,240,255,255}},   // 2 ocean world
            {{0,200,185,255},{0,160,150,255}, {150,240,230,255}},    // 3 abyssal trench
            {{140,255,40,255},{80,220,20,255},{200,255,120,255}},    // 4 toxic swamp
            {{180,200,20,255},{130,155,12,255},{220,235,80,255}},    // 5 spore wastes
            {{200,80,255,255},{140,40,220,255},{220,160,255,255}},   // 6 void rift
            {{160,100,240,255},{90,50,180,255},{210,160,255,255}},   // 7 phantom gate
            {{20,230,215,255},{10,190,180,255},{160,255,245,255}},   // 8 crystal caverns
            {{255,215,50,255},{220,175,25,255},{255,245,160,255}},   // 9 resonance core
            {{255,175,40,255},{240,140,20,255},{255,225,110,255}},   // 10 stellar forge
            {{255,240,80,255},{220,205,50,255},{255,255,180,255}},   // 11 shard tempest
            {{255,60,185,255},{220,30,145,255},{255,160,220,255}},   // 12 quantum depths
            {{220,70,255,255},{160,30,220,255},{240,180,255,255}},   // 13 event horizon
            {{255,200,70,255},{255,140,20,255},{255,240,140,255}},   // 14 solar inferno
            {{255,255,210,255},{255,240,170,255},{255,250,230,255}}, // 15 corona breach
            {{80,165,225,255},{55,130,195,255},{180,220,245,255}},   // 16 obsidian wastes
            {{210,130,40,255},{180,100,22,255},{240,185,90,255}},    // 17 ashen citadel
            {{180,100,255,255},{130,60,220,255},{220,160,255,255}},  // 18 the void
            {{255,245,200,255},{230,200,140,255},{255,255,230,255}}, // 19 void singularity
            {{255,80,160,255},{255,40,140,255},{255,180,220,255}},   // 20
            {{180,80,255,255},{140,50,220,255},{220,160,255,255}},   // 21
            {{70,150,210,255},{50,120,190,255},{160,210,240,255}},   // 22
            {{200,120,35,255},{180,100,25,255},{240,180,90,255}},    // 23
            {{160,80,240,255},{100,40,190,255},{210,150,255,255}},   // 24
            {{80,30,160,255},{50,15,120,255},{160,100,220,255}},     // 25
            {{150,50,220,255},{100,20,180,255},{200,120,255,255}},   // 26
            {{20,5,40,255},  {10,2,20,255},   {60,20,100,255}},     // 27 absolute void
        };
        int wi=std::min(g.wave,27);
        ss.col=waveColPalettes[wi][RandI(0,2)];
        g.shootStars.push_back(ss);
    }
    for(auto& ss:g.shootStars){
        if(!ss.active)continue;
        ss.pos.x+=ss.vel.x*dt; ss.pos.y+=ss.vel.y*dt;
        ss.life-=dt;
        if(ss.life<=0||ss.pos.x>SCREEN_W+50||ss.pos.y>SCREEN_H+50)ss.active=false;
    }
    g.shootStars.erase(std::remove_if(g.shootStars.begin(),g.shootStars.end(),[](ShootStar& s){return !s.active;}),g.shootStars.end());
}

void DrawShootingStars(Game& g){
    for(auto& ss:g.shootStars){
        if(!ss.active)continue;
        float a=ss.life/ss.maxLife;
        float spd=sqrtf(ss.vel.x*ss.vel.x+ss.vel.y*ss.vel.y);
        float nx=ss.vel.x/spd, ny=ss.vel.y/spd;
        float len=ss.len*a;
        Vector2 tail={ss.pos.x-nx*len,ss.pos.y-ny*len};
        DrawLineEx(tail,ss.pos,1.5f,ColorAlpha(ss.col,a*.12f)); // much more transparent
        DrawLineEx(tail,ss.pos,0.8f,ColorAlpha(ss.col,a*.40f));
        DrawCircleV(ss.pos,1.2f*a,ColorAlpha(VP_WHITE,a*.7f));
    }
}


// ══════════════════════════════════════════════════════════════
//  EYE CANDY
// ══════════════════════════════════════════════════════════════

// Aurora ribbon streaks across the sky
void DrawAurora(float t, int wave){
    // ── Per-WAVE aurora color and behavior — each wave unique ──
    struct AuroraTheme { Color c1,c2,c3; float speedMult; float alphaMult; int ribbonCount; };
    AuroraTheme themes[28]={
        // Wave 0: CRIMSON NEBULA — deep crimson/magenta/rose, medium drift
        {{200,20,60,255},{255,60,140,255},{255,100,80,255},1.0f,1.0f,5},
        // Wave 1: EMBER FIELDS — blazing orange/amber/deep red, fast violent
        {{180,40,8,255},{255,110,20,255},{255,180,40,255},1.4f,1.1f,6},
        // Wave 2: OCEAN WORLD — deep blue/cyan/aquamarine, slow gentle
        {{10,60,160,255},{20,180,255,255},{60,230,255,255},0.6f,0.9f,5},
        // Wave 3: ABYSSAL TRENCH — near-black teal, very slow bioluminescent
        {{0,40,60,255},{0,180,170,255},{10,220,200,255},0.4f,0.7f,4},
        // Wave 4: TOXIC SWAMP — acid green/yellow-green, jittery fast
        {{40,120,10,255},{120,255,30,255},{200,255,60,255},1.6f,1.0f,6},
        // Wave 5: SPORE WASTES — sickly olive/murky yellow, slow creeping
        {{55,75,5,255},{130,160,15,255},{200,220,40,255},0.5f,0.75f,4},
        // Wave 6: VOID RIFT — ultraviolet/deep purple/violet, strobing fast
        {{60,10,120,255},{150,40,255,255},{200,100,255,255},2.0f,1.3f,7},
        // Wave 7: PHANTOM GATE — near-black indigo/ghostly lavender, ultra-slow ethereal
        {{12,4,28,255},{80,30,160,255},{190,120,255,255},0.35f,0.65f,3},
        // Wave 8: CRYSTAL CAVERNS — teal/mint/ice-white, moderate crystal shimmer
        {{5,90,100,255},{20,220,210,255},{160,255,240,255},0.9f,0.85f,5},
        // Wave 9: RESONANCE CORE — rich amber/gold/warm white, pulsing medium
        {{160,90,10,255},{255,200,40,255},{255,245,140,255},1.1f,1.05f,5},
        // Wave 10: STELLAR FORGE — fiery orange/yellow/white forge blaze, fast
        {{200,70,10,255},{255,165,35,255},{255,230,100,255},1.5f,1.15f,6},
        // Wave 11: SHARD TEMPEST — hot gold/white/electric yellow, spinning fast
        {{160,120,5,255},{255,230,60,255},{255,255,180,255},1.8f,1.1f,7},
        // Wave 12: QUANTUM DEPTHS — hot pink/fuchsia/deep magenta, pulsing
        {{130,5,80,255},{255,50,160,255},{255,130,210,255},1.2f,1.1f,6},
        // Wave 13: EVENT HORIZON — absolute black edges/violent violet rim, strobing
        {{15,2,25,255},{100,20,180,255},{220,70,255,255},2.5f,1.4f,7},
        // Wave 14: SOLAR INFERNO — scorching red/orange/white-hot, violent fast
        {{200,25,2,255},{255,100,15,255},{255,220,80,255},1.7f,1.2f,7},
        // Wave 15: CORONA BREACH — blinding white/gold/pale eruption, intense
        {{220,170,50,255},{255,235,150,255},{255,255,220,255},1.3f,1.25f,6},
        // Wave 16: OBSIDIAN WASTES — cold charcoal/steel-blue/ice, slow sharp
        {{18,18,30,255},{60,160,220,255},{160,210,240,255},0.65f,0.7f,4},
        // Wave 17: ASHEN CITADEL — smouldering dark/ember-orange/ash, medium
        {{30,12,4,255},{160,90,20,255},{230,160,60,255},0.9f,0.85f,5},
        // Wave 18: THE VOID — pure void deep purple/ghostly lavender wisps, ultra-slow
        {{4,1,12,255},{80,25,140,255},{200,120,255,255},0.28f,0.6f,3},
        // Wave 19: VOID SINGULARITY — pitch black/blinding gold rim/white, max chaos
        {{2,0,5,255},{200,160,60,255},{255,245,200,255},3.2f,1.7f,8},
        // Wave 20+: beyond — pink/hotpink/coral surge
        {{255,30,100,255},{255,100,200,255},{255,180,230,255},2.0f,1.3f,6},
        // Wave 21
        {{100,30,200,255},{180,80,255,255},{220,160,255,255},2.2f,1.4f,7},
        // Wave 22
        {{10,6,22,255},{50,20,100,255},{130,70,200,255},0.8f,0.7f,4},
        // Wave 23
        {{80,35,8,255},{200,120,30,255},{255,190,80,255},1.0f,0.9f,5},
        // Wave 24
        {{3,0,8,255},{40,10,80,255},{160,60,220,255},0.3f,0.55f,3},
        // Wave 25
        {{1,0,2,255},{20,5,40,255},{100,40,160,255},0.25f,0.5f,3},
        // Wave 26 (dying violet)
        {{50,0,80,255},{120,20,200,255},{200,80,255,255},1.8f,0.8f,5},
        // Wave 27 (absolute void — near invisible)
        {{0,0,0,255},{5,0,10,255},{30,5,50,255},0.2f,0.35f,2},
    };
    int wi = std::min(wave, 27);
    AuroraTheme& th=themes[wi];

    struct AuroraRibbon{ float ySeed,phase,speed,width,hue; };
    static AuroraRibbon ribbons[]={
        {.18f,0.f,  .11f,20,0.f},
        {.32f,.8f,  .09f,14,1.f},
        {.12f,1.6f, .13f,10,2.f},
        {.44f,2.4f, .08f,16,0.5f},
        {.27f,3.2f, .12f,12,1.5f},
        {.08f,4.0f, .16f, 8,0.8f},
        {.38f,4.8f, .10f,11,1.2f},
        {.22f,5.6f, .14f,17,0.3f},
    };
    // Cap ribbons at 4 and step x by 8px — ~10x fewer draw calls than original
    int rc=std::min(th.ribbonCount,4);
    for(int ri=0;ri<rc;ri++){
        auto& r=ribbons[ri];
        float baseY=r.ySeed*(float)SCREEN_H*0.55f;
        for(int x=0;x<SCREEN_W;x+=8){
            float xf=(float)x/SCREEN_W;
            float wave1=sinf(xf*4.f+t*r.speed*th.speedMult+r.phase)*18;
            float wave2=sinf(xf*7.f+t*r.speed*1.3f*th.speedMult+r.phase+1.2f)*9;
            float cy2=baseY+wave1+wave2;
            float aph=(.4f+.35f*sinf(xf*3+t*r.speed*.7f*th.speedMult+r.phase))*.32f*th.alphaMult;
            Color col;
            float h=fmodf(r.hue+xf*.5f+t*.07f*th.speedMult,3.f);
            if(h<1)      col=LerpC(th.c1,th.c2,h);
            else if(h<2) col=LerpC(th.c2,th.c3,h-1);
            else         col=LerpC(th.c3,th.c1,h-2);
            int halfW=(int)(r.width*.5f);
            for(int s=-halfW;s<halfW;s+=2){
                float sf=fabsf((float)s/r.width);
                float fa=aph*(1-sf*sf)*(1-sf*sf);
                if(fa<0.01f) continue;
                DrawRectangle(x,(int)(cy2+s),8,2,ColorAlpha(col,fa));
            }
        }
    }
}

// Hyperspace particle stream — fast streaks rushing past
struct HyperPt{ Vector2 pos,vel; float life,maxLife; Color col; };
static std::vector<HyperPt> hyperPts;
static float hyperSpawnT=0;
void UpdateHyper(float t,float dt,Game& g){
    hyperSpawnT-=dt;
    if(hyperSpawnT<=0){
        hyperSpawnT=RandF(.015f,.035f);
        float angle=g.playerAngle+PI+RandF(-.5f,.5f);
        float spd=RandF(200,420);
        Color cols[]={VP_CYAN,VP_PURPLE,VP_PINK,VP_MINT,VP_LAVENDER};
        HyperPt p;
        p.pos={RandF(0,(float)SCREEN_W),RandF(0,(float)SCREEN_H)};
        p.vel={cosf(angle)*spd,sinf(angle)*spd};
        p.life=p.maxLife=RandF(.12f,.28f);
        p.col=cols[RandI(0,4)];
        hyperPts.push_back(p);
    }
    for(auto& p:hyperPts) p.life-=dt;
    hyperPts.erase(std::remove_if(hyperPts.begin(),hyperPts.end(),[](HyperPt& p){return p.life<=0;}),hyperPts.end());
}
void DrawHyper(){
    for(auto& p:hyperPts){
        float a=p.life/p.maxLife;
        float len=(1-a)*28+6;
        Vector2 tail={p.pos.x-p.vel.x*p.life,p.pos.y-p.vel.y*p.life};
        DrawLineEx(tail,p.pos,1.5f,ColorAlpha(p.col,a*.7f));
        DrawLineEx(tail,p.pos,.8f,ColorAlpha(VP_WHITE,a*.3f));
        DrawCircleV(p.pos,1.2f,ColorAlpha(VP_WHITE,a*.5f));
    }
}

// Nebula lightning — occasional electric arcs between nebula clusters
static float lightningT=0; static float lightningLife=0;
static std::vector<Vector2> lightningPts;
void UpdateLightning(float dt){
    lightningT-=dt; lightningLife-=dt;
    if(lightningT<=0){
        lightningT=RandF(4.f,9.f);
        lightningLife=RandF(.12f,.22f);
        lightningPts.clear();
        float x=RandF(40,(float)SCREEN_W-40), y=RandF(10,(float)SCREEN_H*.6f);
        float ex=x+RandF(-180,180), ey=y+RandF(-80,80);
        int segs=RandI(6,12);
        for(int i=0;i<=segs;i++){
            float f=(float)i/segs;
            float jx=RandF(-18,18)*(1-fabsf(f-.5f)*1.5f);
            float jy=RandF(-12,12)*(1-fabsf(f-.5f)*1.5f);
            lightningPts.push_back({x+(ex-x)*f+jx, y+(ey-y)*f+jy});
        }
    }
}
void DrawLightning(){
    if(lightningLife<=0||lightningPts.size()<2) return;
    float a=std::min(1.f,lightningLife*10);
    for(int i=0;i<(int)lightningPts.size()-1;i++){
        DrawLineEx(lightningPts[i],lightningPts[i+1],2.5f,ColorAlpha(VP_WHITE,a*.9f));
        DrawLineEx(lightningPts[i],lightningPts[i+1],5.f,ColorAlpha(VP_CYAN,a*.35f));
        DrawLineEx(lightningPts[i],lightningPts[i+1],10.f,ColorAlpha(VP_PURPLE,a*.08f));
    }
}

// Ambient floating dust motes
struct DustMote{ Vector2 pos,vel; float life,maxLife,r; Color col; };
static std::vector<DustMote> dustMotes;
static float dustSpawnT=0;
void UpdateDust(float dt,Game& g){
    dustSpawnT-=dt;
    if(dustSpawnT<=0){
        dustSpawnT=RandF(.08f,.18f);
        // Per-zone dust color palettes
        Color zoneCols[6][5]={
            {VP_PURPLE,VP_CYAN,VP_PINK,VP_LAVENDER,VP_MINT},           // Zone1: classic vaporwave
            {VP_TEAL,VP_DEEPCYAN,VP_MINT,{0,200,200,255},{0,160,180,255}}, // Zone2: deep ocean — blue/teal
            {VP_PURPLE,VP_INDIGO,VP_LAVENDER,{80,20,140,255},{140,60,210,255}}, // Zone3: void — dark purple
            {VP_ORANGE,VP_CORAL,VP_RED,{255,120,30,255},{200,60,10,255}}, // Zone4: inferno — orange/red embers
            {{40,180,220,255},{20,120,180,255},{160,220,240,255},VP_DEEPBLUE,{80,160,200,255}}, // Zone5: obsidian — icy blue
            {VP_HOTPINK,VP_PINK,VP_PURPLE,VP_CORAL,VP_LAVENDER},       // Endgame: chaos
        };
        int zi=(g.wave<=5)?0:(g.wave<=10)?1:(g.wave<=15)?2:(g.wave<=20)?3:(g.wave<=24)?4:5;
        DustMote d;
        d.pos={RandF(0,(float)SCREEN_W),RandF(0,(float)SCREEN_H)};
        float ang=RandF(0,2*PI);
        float spd=RandF(4,18);
        d.vel={cosf(ang)*spd,sinf(ang)*spd};
        d.life=d.maxLife=RandF(3.f,8.f);
        d.r=RandF(.8f,2.2f);
        d.col=zoneCols[zi][RandI(0,4)];
        dustMotes.push_back(d);
    }
    for(auto& d:dustMotes){d.life-=dt;d.pos.x+=d.vel.x*dt;d.pos.y+=d.vel.y*dt;}
    dustMotes.erase(std::remove_if(dustMotes.begin(),dustMotes.end(),[](DustMote& d){return d.life<=0;}),dustMotes.end());
    if((int)dustMotes.size()>80) dustMotes.erase(dustMotes.begin());
}
void DrawDust(){
    for(auto& d:dustMotes){
        float a=(d.life/d.maxLife)*(.5f-d.r*.12f);
        DrawCircleV(d.pos,d.r,ColorAlpha(d.col,a));
    }
}

// Screen-space chromatic aberration flicker on damage/warps
static float aberrationTimer=0;
void TriggerAberration(float dur){ aberrationTimer=std::max(aberrationTimer,dur); }
void DrawAberration(float dt){
    if(aberrationTimer<=0) return;
    aberrationTimer-=dt;
    float a=aberrationTimer*2.f; if(a>1)a=1;
    // stronger layered RGB split — 4 passes, increasing offset
    for(int pass=0;pass<3;pass++){
        float f=(pass+1)*1.5f;
        int sx=(int)(a*f), sy=(int)(a*f*.4f);
        DrawRectangle(sx, 0,SCREEN_W,SCREEN_H,ColorAlpha({255,0,0,255},  a*.030f*(1.f/(pass+1))));
        DrawRectangle(-sx,0,SCREEN_W,SCREEN_H,ColorAlpha({0,255,255,255},a*.030f*(1.f/(pass+1))));
        DrawRectangle(0, sy,SCREEN_W,SCREEN_H,ColorAlpha({0,0,255,255},  a*.020f*(1.f/(pass+1))));
        DrawRectangle(0,-sy,SCREEN_W,SCREEN_H,ColorAlpha({255,255,0,255},a*.012f*(1.f/(pass+1))));
    }
    // white flash on first frame
    if(aberrationTimer>0.12f)
        DrawRectangle(0,0,SCREEN_W,SCREEN_H,ColorAlpha(VP_WHITE,(aberrationTimer-.12f)*0.08f));
}

// Radial sector ring (expands from player on overdrive activate)
struct SectorRing{ Vector2 pos; float r,maxR,life,maxLife; Color col; };
static std::vector<SectorRing> sectorRings;
void SpawnSectorRing(Vector2 pos,Color col,float maxR,float life){
    sectorRings.push_back({pos,0,maxR,life,life,col});
}
void UpdateSectorRings(float dt){ for(auto& r:sectorRings){r.life-=dt;r.r=r.maxR*(1-r.life/r.maxLife);} sectorRings.erase(std::remove_if(sectorRings.begin(),sectorRings.end(),[](SectorRing& r){return r.life<=0;}),sectorRings.end()); }
void DrawSectorRings(Game& g){
    for(auto& r:sectorRings){
        Vector2 sp=W2S(g,r.pos);
        float a=r.life/r.maxLife;
        DrawNeonCircle(sp,r.r,r.col,a*.75f);
        DrawNeonCircle(sp,r.r-4,r.col,a*.25f);
    }
}

void DrawNebulas(Game& g,float t){
    // Vaporwave sun (fixed backdrop)
    DrawVaporwaveSun(t, g.wave);
    DrawPlanets(g,t);
    DrawFloatingShapes(t, g.wave);
    // Nebulas (now in hot pink / purple / cyan tones)
    for(auto& n:g.nebulas){
        float sx=n.pos.x-g.cameraPos.x*.05f+sinf(t*.12f+n.driftOffset)*6;
        float sy=n.pos.y-g.cameraPos.y*.05f+cosf(t*.09f+n.driftOffset)*4;
        sx=fmodf(sx+(float)SCREEN_W*20,(float)SCREEN_W);
        sy=fmodf(sy+(float)SCREEN_H*20,(float)SCREEN_H);
        float a2=n.alpha*(.6f+.4f*sinf(t*.3f+n.driftOffset));
        for(int i=0;i<3;i++) DrawCircleV({sx+i*8.f,sy-i*5.f},n.radius*(1-.25f*i),ColorAlpha(n.col,a2*(.55f-.15f*i)));
        // slow internal swirl — two faint rotating arcs
        float swA=t*.08f+n.driftOffset;
        for(int si=0;si<2;si++){
            float sa2=swA+si*PI;
            Vector2 s1={sx+cosf(sa2)*n.radius*.45f,         sy+sinf(sa2)*n.radius*.45f};
            Vector2 s2={sx+cosf(sa2+PI*.6f)*n.radius*.62f,  sy+sinf(sa2+PI*.6f)*n.radius*.62f};
            DrawLineEx(s1,s2,n.radius*.04f,ColorAlpha(VP_WHITE,a2*.06f));
        }
    }
    // ── Dynamic wave-reactive nebula wash ──
    {
        float cx2=g.cameraPos.x*.05f, cy2=g.cameraPos.y*.05f;
        float heat=(g.state==PLAYING)?(g.chiGauge/100.f):0.3f;
        // Per-wave nebula wash colors (28 waves)
        Color waveNebulaColors[28][3]={
            {{120,10,40,255},{80,5,25,255},{160,20,60,255}},     // 0 CRIMSON NEBULA — deep red/magenta
            {{160,30,8,255}, {120,15,5,255},{200,50,12,255}},    // 1 EMBER FIELDS — smoldering orange-red
            {{5,40,120,255}, {3,25,80,255}, {8,60,150,255}},     // 2 OCEAN WORLD — deep blue
            {{2,20,55,255},  {1,10,35,255}, {3,35,70,255}},      // 3 ABYSSAL TRENCH — near black deep blue
            {{35,90,10,255}, {20,60,5,255}, {50,120,15,255}},    // 4 TOXIC SWAMP — acid green
            {{55,75,5,255},  {38,55,3,255}, {72,95,7,255}},      // 5 SPORE WASTES — olive/sickly yellow
            {{55,5,95,255},  {35,3,65,255}, {75,8,125,255}},     // 6 VOID RIFT — deep violet
            {{10,4,28,255},  {6,2,18,255},  {18,6,42,255}},      // 7 PHANTOM GATE — near black indigo
            {{5,85,95,255},  {3,60,70,255}, {8,110,120,255}},    // 8 CRYSTAL CAVERNS — teal
            {{160,110,8,255},{120,80,5,255},{200,140,12,255}},   // 9 RESONANCE CORE — amber/gold
            {{180,65,8,255}, {140,45,5,255},{220,85,12,255}},    // 10 STELLAR FORGE — orange forge glow
            {{200,165,12,255},{160,130,8,255},{240,195,18,255}}, // 11 SHARD TEMPEST — hot gold
            {{120,4,70,255}, {85,2,50,255}, {155,6,90,255}},    // 12 QUANTUM DEPTHS — deep magenta
            {{3,0,8,255},    {2,0,5,255},   {6,0,14,255}},       // 13 EVENT HORIZON — absolute black
            {{200,30,2,255}, {160,18,1,255},{240,50,4,255}},     // 14 SOLAR INFERNO — blazing red-orange
            {{220,180,60,255},{180,145,40,255},{240,200,80,255}},// 15 CORONA BREACH — white-gold eruption
            {{18,18,30,255}, {12,12,22,255},{24,28,40,255}},     // 16 OBSIDIAN WASTES — cold charcoal
            {{50,24,6,255},  {35,16,4,255}, {65,32,8,255}},      // 17 ASHEN CITADEL — smoldering ash
            {{4,1,12,255},   {2,0,7,255},   {8,2,20,255}},       // 18 THE VOID — pure void darkness
            {{1,0,3,255},    {0,0,2,255},   {3,0,6,255}},        // 19 VOID SINGULARITY — absolute void
            {{160,5,90,255},{120,3,65,255},{200,8,115,255}},     // 20 w20 — magenta carnage
            {{120,40,200,255},{90,28,160,255},{150,55,230,255}}, // 21 CORONA BREACH — violet surge
            {{12,8,25,255},  {7,4,15,255},  {20,12,38,255}},     // 22 OBSIDIAN WASTES — obsidian darkness
            {{180,90,12,255},{140,65,8,255},{220,115,18,255}},   // 23 ASHEN CITADEL — ember siege
            {{2,0,6,255},    {1,0,3,255},   {4,0,10,255}},       // 24 THE VOID — deeper void
            {{0,0,1,255},    {0,0,0,255},   {1,0,2,255}},        // 25 VOID SINGULARITY — total darkness
            {{80,0,120,255}, {60,0,90,255}, {110,0,150,255}},    // 26 w26 — dying violet flicker
            {{0,0,0,255},    {0,0,0,255},   {5,0,8,255}},        // 27 ABSOLUTE VOID — nothing
        };
        int wn = std::min(g.wave, 27);
        Color nc1=waveNebulaColors[wn][0];
        Color nc2=waveNebulaColors[wn][1];
        Color nc3=waveNebulaColors[wn][2];
        for(int i=0;i<4;i++){
            float nx2=fmodf(SCREEN_W*(0.18f+i*0.22f)+sinf(t*.05f+i*1.5f)*55-cx2+SCREEN_W*20,SCREEN_W);
            float ny2=fmodf(SCREEN_H*(0.25f+i*0.18f)+cosf(t*.04f+i*2.1f)*40-cy2+SCREEN_H*20,SCREEN_H);
            float nr2=SCREEN_W*(0.22f+0.06f*sinf(t*.07f+i));
            Color nc=(i%3==0)?nc1:(i%3==1)?nc2:nc3;
            float na=0.040f+heat*0.018f+0.010f*sinf(t*.25f+i);
            DrawCircleV({nx2,ny2},nr2,ColorAlpha(nc,na));
            DrawCircleV({nx2,ny2},nr2*.5f,ColorAlpha(nc,na*0.6f));
        }
    }
}

void DrawStars(Game& g,float t){
    // Per-wave mood tint for stars — shifts the starfield color to match the level theme
    Color waveMoodTint[28]={
        {255,60,140,255},   // 0 crimson nebula
        {255,100,30,255},   // 1 ember fields
        {40,180,255,255},   // 2 ocean world
        {0,180,170,255},    // 3 abyssal trench
        {120,255,30,255},   // 4 toxic swamp
        {155,175,15,255},   // 5 spore wastes
        {180,60,255,255},   // 6 void rift
        {130,80,220,255},   // 7 phantom gate
        {20,220,210,255},   // 8 crystal caverns
        {255,210,40,255},   // 9 resonance core
        {255,175,40,255},   // 10 stellar forge
        {255,240,80,255},   // 11 shard tempest
        {255,60,180,255},   // 12 quantum depths
        {200,50,255,255},   // 13 event horizon
        {255,200,70,255},   // 14 solar inferno
        {255,250,200,255},  // 15 corona breach
        {80,165,225,255},   // 16 obsidian wastes
        {200,120,35,255},   // 17 ashen citadel
        {180,100,255,255},  // 18 the void
        {255,245,200,255},  // 19 void singularity
        {255,80,160,255},   // 20+
        {180,80,255,255},{70,150,210,255},{200,120,35,255},
        {160,80,240,255},{80,30,160,255},{150,50,220,255},{20,5,40,255}
    };
    int wi=std::min(g.wave,27);
    Color moodTint=waveMoodTint[wi];

    for(auto& s:g.stars){
        float pf=(s.layer==0)?.06f:(s.layer==1)?.28f:.70f;
        float sx=fmodf(s.pos.x-g.cameraPos.x*pf+(float)SCREEN_W*50,(float)SCREEN_W);
        float sy=fmodf(s.pos.y-g.cameraPos.y*pf+(float)SCREEN_H*50,(float)SCREEN_H);
        float b=s.brightness*(.4f+.6f*sinf(t*s.twinkleSpeed+s.twinkleOffset));
        // Blend stored star tint with wave mood for foreground/mid stars
        Color tint = (s.layer>=1) ? LerpC(s.tint, moodTint, 0.35f) : s.tint;

        if(s.layer==2){
            // Bright foreground stars: glow + cross diffraction (only very bright ones)
            if(b>.75f){
                DrawCircleV({sx,sy},s.radius+2,ColorAlpha(tint,b*.12f));
                // diffraction spikes (4-pointed, only cardinal)
                float sp=s.radius+2.5f+b*1.5f;
                DrawLineEx({sx-sp,sy},{sx+sp,sy},1.f,ColorAlpha(tint,b*.4f));
                DrawLineEx({sx,sy-sp},{sx,sy+sp},1.f,ColorAlpha(tint,b*.4f));
            }
            DrawCircleV({sx,sy},s.radius,ColorAlpha(VP_WHITE,b*.8f));
            DrawCircleV({sx,sy},s.radius*.6f,ColorAlpha(tint,b));
        } else if(s.layer==1){
            // Medium stars: subtle glow
            if(b>.5f) DrawCircleV({sx,sy},s.radius+1.2f,ColorAlpha(tint,b*.15f));
            DrawCircleV({sx,sy},s.radius,ColorAlpha(tint,b*.85f));
        } else {
            // Background stars: just dots, but very occasionally streak
            DrawCircleV({sx,sy},s.radius,ColorAlpha(s.tint,b*.55f));
            // rare micro-streak when twinkle peaks — looks like a distant ship
            if(b>0.88f && sinf(s.twinkleOffset*7.3f)>0.72f){
                float sAngle=s.twinkleOffset*3.1f; // fixed direction per star
                float sLen=2.5f+s.radius;
                DrawLineEx({sx-cosf(sAngle)*sLen,sy-sinf(sAngle)*sLen},
                           {sx+cosf(sAngle)*sLen,sy+sinf(sAngle)*sLen},
                           .6f,ColorAlpha(s.tint,b*.40f));
            }
        }
    }
}

void DrawWorldGrid(Game& g,float t){
    // Throttle: skip intersection decorations every other frame
    static int gridFrame=0; gridFrame++;
    bool drawNodes=(gridFrame%2==0);
    float spd=sqrtf(g.playerVel.x*g.playerVel.x+g.playerVel.y*g.playerVel.y);
    float spdBoost=std::min(1.f,spd/PLAYER_SPEED)*.06f;
    int camX=(int)g.cameraPos.x, camY=(int)g.cameraPos.y;
    int w=g.wave;

    // ══════════════════════════════════════════════════════════════
    //  PER-ZONE GRID THEMES — dramatically different for each zone
    // ══════════════════════════════════════════════════════════════

    if(w<=5){
        // ── ZONE 1: Each wave gets a distinct grid personality ──
        if(w==0){
            // Wave 0: CRIMSON NEBULA — hot pink/magenta classic retro grid, large cells
            float pulse =(.07f+.04f*sinf(t*1.1f)+spdBoost);
            float pulse2=(.05f+.03f*sinf(t*1.4f+1.f)+spdBoost*.7f);
            Color lineA={255,40,150,255}, lineB={140,10,80,255}, lineC={255,80,200,255};
            int step=330;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
                bool major=(((wx%(step*4)+step*4)%(step*4))==0);
                DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(major?lineA:lineB,major?pulse*1.8f:pulse));
                if(major) DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(lineC,pulse*.4f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                bool major=(((wy%(step*4)+step*4)%(step*4))==0);
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(major?VP_PURPLE:VP_DARKPURP,major?pulse2*1.6f:pulse2));
                if(major) DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(VP_LAVENDER,pulse2*.45f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                bool major=(((wx%(step*4)+step*4)%(step*4))==0&&((wy%(step*4)+step*4)%(step*4))==0);
                float gp=sinf(t*2.2f+wx*.003f+wy*.002f)*.5f+.5f;
                if(major){
                    float ds=5.f+gp*2.f;
                    DrawLineEx({(float)sx,(float)sy-ds},{(float)sx+ds,(float)sy},1.5f,ColorAlpha(lineA,gp*.5f));
                    DrawLineEx({(float)sx+ds,(float)sy},{(float)sx,(float)sy+ds},1.5f,ColorAlpha(lineA,gp*.5f));
                    DrawLineEx({(float)sx,(float)sy+ds},{(float)sx-ds,(float)sy},1.5f,ColorAlpha(lineA,gp*.5f));
                    DrawLineEx({(float)sx-ds,(float)sy},{(float)sx,(float)sy-ds},1.5f,ColorAlpha(lineA,gp*.5f));
                    DrawCircleV({(float)sx,(float)sy},2.f,ColorAlpha(VP_WHITE,gp*.7f));
                } else { DrawCircleV({(float)sx,(float)sy},1.0f,ColorAlpha(VP_LAVENDER,gp*.2f)); }
            }}
        } else if(w==1){
            // Wave 1: EMBER FIELDS — orange/red lava-crack grid, narrow dense
            float pulse=(.06f+.05f*sinf(t*1.8f)+spdBoost);
            Color lineA={255,100,20,255}, lineB={160,35,5,255}, lineC={255,180,40,255};
            int step=250;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0);
                DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(major?lineA:lineB,major?pulse*2.0f:pulse*.8f));
                if(major) DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(lineC,pulse*.35f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                bool major=(((wy%(step*3)+step*3)%(step*3))==0);
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(major?lineB:lineB,major?pulse*1.4f:pulse*.5f));
            }
            // Ember sparks at major intersections
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                float gp=sinf(t*3.5f+wx*.004f+wy*.003f)*.5f+.5f;
                if(major && gp>0.45f){
                    DrawCircleV({(float)sx,(float)sy},2.5f+gp*2.f,ColorAlpha(lineC,gp*.18f));
                    DrawCircleV({(float)sx,(float)sy},1.5f,ColorAlpha(VP_WHITE,gp*.75f));
                    // ember flicker spikes
                    float sl=3.f+gp*4.f;
                    DrawLineEx({(float)sx,(float)sy-sl},{(float)sx,(float)sy+sl},1.2f,ColorAlpha(lineA,gp*.6f));
                } else if(gp>0.8f){ DrawCircleV({(float)sx,(float)sy},0.8f,ColorAlpha(lineB,gp*.3f)); }
            }}
        } else if(w==2){
            // Wave 2: OCEAN WORLD — deep blue/cyan wave ripple grid
            float pulse=(.045f+.03f*sinf(t*.9f)+spdBoost*.05f);
            Color lineA={30,180,255,255}, lineB={10,80,160,255}, lineC={80,230,255,255};
            int step=280;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
                bool major=(((wx%(step*4)+step*4)%(step*4))==0);
                DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(major?lineA:lineB,major?pulse*1.6f:pulse));
            }
            // Ripple horizontal lines with sinusoidal Y offset
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                bool major=(((wy%(step*3)+step*3)%(step*3))==0);
                float waveShift=(int)(sinf(t*.5f+(wy*.002f))*6.f);
                DrawLine(0,(int)(sy+waveShift),SCREEN_W,(int)(sy+waveShift),ColorAlpha(major?lineA:lineB,major?pulse*1.8f:pulse*.6f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*1.5f+wx*.0025f+wy*.002f)*.5f+.5f;
                bool major=(((wx%(step*4)+step*4)%(step*4))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major){
                    DrawCircleV({(float)sx,(float)sy},5.f+gp*3.f,ColorAlpha(lineA,gp*.10f));
                    DrawCircleV({(float)sx,(float)sy},2.f,ColorAlpha(lineC,gp*.55f));
                    DrawNeonCircle({(float)sx,(float)sy},4.f,lineA,gp*.22f);
                } else if(gp>0.7f){ DrawCircleV({(float)sx,(float)sy},0.9f,ColorAlpha(lineB,gp*.25f)); }
            }}
        } else if(w==3){
            // Wave 3: ABYSSAL TRENCH — near-black diagonal fishnet, glowing bioluminescent nodes
            float pulse=(.030f+.020f*sinf(t*.7f)+spdBoost*.03f);
            Color lineA={0,180,170,255}, lineB={0,60,55,255}, lineC={10,220,200,255};
            int step=210;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-4||sx>SCREEN_W+4)continue;
                DrawLine(sx,0,sx+SCREEN_H/2,SCREEN_H,ColorAlpha(lineA,pulse*.6f));
                DrawLine(sx,0,sx-SCREEN_H/2,SCREEN_H,ColorAlpha(lineB,pulse*.4f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(lineB,pulse*.3f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*1.2f+wx*.003f+wy*.0022f)*.5f+.5f;
                bool major=(((wx%(step*4)+step*4)%(step*4))==0&&((wy%(step*4)+step*4)%(step*4))==0);
                if(major){
                    DrawCircleV({(float)sx,(float)sy},5.f+gp*3.f,ColorAlpha(lineA,gp*.14f));
                    DrawCircleV({(float)sx,(float)sy},2.5f,ColorAlpha(lineC,gp*.60f));
                    DrawNeonCircle({(float)sx,(float)sy},4.f,lineA,gp*.28f);
                } else if(gp>0.78f){ DrawCircleV({(float)sx,(float)sy},0.7f,ColorAlpha(lineA,gp*.35f)); }
            }}
        } else if(w==4){
            // Wave 4: TOXIC SWAMP — acid green sickly irregular grid, jagged
            float pulse=(.055f+.04f*sinf(t*2.2f)+spdBoost*.06f);
            Color lineA={120,255,30,255}, lineB={50,110,10,255}, lineC={180,255,60,255};
            int step=230;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0);
                // Slightly irregular — jagged by adding vertical jitter
                float jit=sinf(t*4.f+wx*.008f)*2.f;
                DrawLine(sx+(int)jit,0,sx-(int)jit,SCREEN_H,ColorAlpha(major?lineA:lineB,major?pulse*1.9f:pulse*.7f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(lineB,pulse*.45f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*2.8f+wx*.004f+wy*.003f)*.5f+.5f;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major && gp>0.4f){
                    DrawCircleV({(float)sx,(float)sy},3.f+gp*2.f,ColorAlpha(lineA,gp*.15f));
                    DrawCircleV({(float)sx,(float)sy},1.8f,ColorAlpha(lineC,gp*.65f));
                } else if(gp>0.8f){ DrawCircleV({(float)sx,(float)sy},0.9f,ColorAlpha(lineB,gp*.3f)); }
            }}
        } else {
            // Wave 5: SPORE WASTES — murky olive sparse grid with drifting spore motes
            float pulse=(.038f+.022f*sinf(t*.65f)+spdBoost*.04f);
            Color lineA={155,175,15,255}, lineB={60,75,5,255}, lineC={190,210,22,255};
            int step=300;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
                DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(lineB,pulse*.9f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                bool major=(((wy%(step*3)+step*3)%(step*3))==0);
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(major?lineA:lineB,major?pulse*1.5f:pulse*.6f));
            }
            // Floating spore blobs at nodes
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*1.0f+wx*.003f+wy*.0018f)*.5f+.5f;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major){
                    // Spore blob — multi-ring diffuse orb
                    DrawCircleV({(float)sx,(float)sy},6.f+gp*4.f,ColorAlpha(lineA,gp*.10f));
                    DrawCircleV({(float)sx,(float)sy},3.f+gp*2.f,ColorAlpha(lineA,gp*.15f));
                    DrawCircleV({(float)sx,(float)sy},1.8f,ColorAlpha(lineC,gp*.50f));
                } else if(gp>0.72f){ DrawCircleV({(float)sx,(float)sy},1.0f,ColorAlpha(lineB,gp*.25f)); }
            }}
            // Drifting spore mote streaks
            for(int i=0;i<5;i++){
                float my=SCREEN_H*(0.12f+i*0.18f)+sinf(t*.3f+i*1.5f)*15.f;
                float mx=fmodf(SCREEN_W*0.05f+i*SCREEN_W*.22f+t*12.f+camX*.02f,SCREEN_W);
                DrawCircleV({mx,my},2.2f,ColorAlpha(lineA,.08f+.04f*sinf(t*1.2f+i)));
                DrawCircleV({mx,my},.9f,ColorAlpha(lineC,.18f));
            }
        }

    } else if(w<=10){
        // ── ZONE 2: Waves 6-10 — each with distinct grid style ──
        if(w==6){
            // Wave 6: VOID RIFT — deep purple diagonal rift lines, sparking violet nodes
            float pulse=.05f+.035f*sinf(t*2.2f)+spdBoost*.05f;
            Color lineA={180,60,255,255}, lineB={55,8,100,255}, lineC={220,120,255,255};
            int step=260;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-4||sx>SCREEN_W+4)continue;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0);
                DrawLine(sx,0,sx+SCREEN_H/3,SCREEN_H,ColorAlpha(major?lineA:lineB,major?pulse*1.9f:pulse*.65f));
                DrawLine(sx,0,sx-SCREEN_H/3,SCREEN_H,ColorAlpha(lineB,pulse*.45f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(lineB,pulse*.35f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*2.8f+wx*.0038f+wy*.003f)*.5f+.5f;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major && gp>0.38f){
                    float sl=4.f+gp*5.f;
                    DrawLineEx({(float)sx-sl,(float)sy},{(float)sx+sl,(float)sy},1.6f,ColorAlpha(lineA,gp*.7f));
                    DrawLineEx({(float)sx,(float)sy-sl},{(float)sx,(float)sy+sl},1.6f,ColorAlpha(lineA,gp*.7f));
                    DrawCircleV({(float)sx,(float)sy},1.5f,ColorAlpha(lineC,gp*.85f));
                    if(gp>0.82f) DrawNeonCircle({(float)sx,(float)sy},sl,lineA,gp*.14f);
                } else { DrawCircleV({(float)sx,(float)sy},0.8f,ColorAlpha(lineB,gp*.2f)); }
            }}
        } else if(w==7){
            // Wave 7: PHANTOM GATE — near-invisible deep indigo whisper grid, ghost orbs
            float pulse=.022f+.015f*sinf(t*.6f)+spdBoost*.025f;
            Color lineA={100,50,200,255}, lineB={20,8,45,255}, lineC={190,140,255,255};
            int step=310;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
                DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(lineB,pulse*.8f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(lineB,pulse*.6f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*0.8f+wx*.002f+wy*.0015f)*.5f+.5f;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major){
                    // Ghost orb — multi-ring faint halo
                    DrawCircleV({(float)sx,(float)sy},8.f+gp*5.f,ColorAlpha(lineA,gp*.07f));
                    DrawCircleV({(float)sx,(float)sy},4.f+gp*2.f,ColorAlpha(lineA,gp*.12f));
                    DrawCircleV({(float)sx,(float)sy},2.f,ColorAlpha(lineC,gp*.45f));
                } else if(gp>0.82f){ DrawCircleV({(float)sx,(float)sy},0.7f,ColorAlpha(lineA,gp*.18f)); }
            }}
        } else if(w==8){
            // Wave 8: CRYSTAL CAVERNS — teal diamond lattice grid, crystal node reflections
            float pulse=.042f+.028f*sinf(t*1.0f)+spdBoost*.04f;
            Color lineA={20,220,210,255}, lineB={5,70,65,255}, lineC={80,255,240,255};
            int step=240;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            // Diamond grid — rotated 45°
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-4||sx>SCREEN_W+4)continue;
                DrawLine(sx,0,sx+step/2,step/2,ColorAlpha(lineA,pulse*.7f));
                DrawLine(sx,0,sx-step/2,step/2,ColorAlpha(lineB,pulse*.5f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(lineB,pulse*.3f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*2.0f+wx*.003f+wy*.0022f)*.5f+.5f;
                bool major=(((wx%(step*4)+step*4)%(step*4))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major){
                    // Crystal reflection — diamond outline
                    float ds=4.f+gp*3.f;
                    DrawLineEx({(float)sx,(float)sy-ds},{(float)sx+ds,(float)sy},1.3f,ColorAlpha(lineA,gp*.55f));
                    DrawLineEx({(float)sx+ds,(float)sy},{(float)sx,(float)sy+ds},1.3f,ColorAlpha(lineA,gp*.55f));
                    DrawLineEx({(float)sx,(float)sy+ds},{(float)sx-ds,(float)sy},1.3f,ColorAlpha(lineA,gp*.55f));
                    DrawLineEx({(float)sx-ds,(float)sy},{(float)sx,(float)sy-ds},1.3f,ColorAlpha(lineA,gp*.55f));
                    DrawCircleV({(float)sx,(float)sy},1.8f,ColorAlpha(lineC,gp*.70f));
                } else if(gp>0.7f){ DrawCircleV({(float)sx,(float)sy},0.8f,ColorAlpha(lineA,gp*.28f)); }
            }}
        } else if(w==9){
            // Wave 9: RESONANCE CORE — amber/gold hexagonal resonance grid, pulsing rings
            float pulse=.05f+.035f*sinf(t*1.5f)+spdBoost*.05f;
            Color lineA={255,210,40,255}, lineB={100,70,8,255}, lineC={255,245,140,255};
            int step=270;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0);
                DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(major?lineA:lineB,major?pulse*1.7f:pulse*.6f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                bool major=(((wy%(step*3)+step*3)%(step*3))==0);
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(major?lineA:lineB,major?pulse*1.4f:pulse*.5f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*2.5f+wx*.0035f+wy*.0028f)*.5f+.5f;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major){
                    // Pulsing resonance ring
                    float rr=4.f+gp*6.f;
                    DrawNeonCircle({(float)sx,(float)sy},rr,lineA,gp*.20f);
                    DrawNeonCircle({(float)sx,(float)sy},rr*.6f,lineC,gp*.15f);
                    DrawCircleV({(float)sx,(float)sy},2.2f,ColorAlpha(lineC,gp*.75f));
                } else if(gp>0.65f){ DrawCircleV({(float)sx,(float)sy},1.0f,ColorAlpha(lineA,gp*.30f)); }
            }}
        } else {
            // Wave 10: STELLAR FORGE — blazing orange/white forge grid, heat shimmer
            float pulse=.06f+.04f*sinf(t*1.9f)+spdBoost*.06f;
            Color lineA={255,175,40,255}, lineB={120,55,8,255}, lineC={255,225,100,255};
            int step=280;
            int gStartX=camX-((camX%step+step)%step)-step;
            int gStartY=camY-((camY%step+step)%step)-step;
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
                int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0);
                // Heat shimmer: vertical lines waver slightly
                float shimmer=(int)(sinf(t*5.f+wx*.01f)*1.5f);
                DrawLine(sx+(int)shimmer,0,sx-(int)shimmer,SCREEN_H,ColorAlpha(major?lineA:lineB,major?pulse*2.0f:pulse*.7f));
            }
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
                bool major=(((wy%(step*3)+step*3)%(step*3))==0);
                DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(major?lineB:lineB,major?pulse*1.3f:pulse*.4f));
            }
            for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){ for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*3.2f+wx*.004f+wy*.003f)*.5f+.5f;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major && gp>0.40f){
                    // Forge spark cross
                    float sl=3.f+gp*5.f;
                    DrawLineEx({(float)sx-sl,(float)sy},{(float)sx+sl,(float)sy},1.5f,ColorAlpha(lineA,gp*.75f));
                    DrawLineEx({(float)sx,(float)sy-sl},{(float)sx,(float)sy+sl},1.5f,ColorAlpha(lineA,gp*.75f));
                    DrawCircleV({(float)sx,(float)sy},2.f,ColorAlpha(lineC,gp*.85f));
                    if(gp>0.82f){ DrawCircleV({(float)sx,(float)sy},4.f+gp*3.f,ColorAlpha(lineA,gp*.10f)); }
                } else if(gp>0.78f){ DrawCircleV({(float)sx,(float)sy},0.9f,ColorAlpha(lineB,gp*.28f)); }
            }}
        }

    } else if(w<=15){
        // ── ZONE 3: VOID RIFT — ultra-dark, glitchy purple/violet scanlines, sparking nodes ──
        float pulse=.055f+.03f*sinf(t*2.f)+spdBoost*.05f;
        int step=270;
        int gStartX=camX-((camX%step+step)%step)-step;
        int gStartY=camY-((camY%step+step)%step)-step;
        // Scanline-style: dense horizontal, sparse vertical
        for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
            int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
            bool major=(((wy%(step*4)+step*4)%(step*4))==0);
            // Glitch offset — line jitters slightly
            float jitter=major?sinf(t*8.3f+wy*.01f)*2.f:0.f;
            DrawLine(0,(int)(sy+jitter),SCREEN_W,(int)(sy+jitter),ColorAlpha(VP_PURPLE,major?pulse*2.f:pulse*.7f));
            if(major){
                // Double scanline for major
                DrawLine(0,(int)(sy+jitter+2),SCREEN_W,(int)(sy+jitter+2),ColorAlpha(VP_DEEPPURP,pulse*.9f));
            }
        }
        for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
            int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
            DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(VP_INDIGO,pulse*.4f));
        }
        // Sparking cross nodes at intersections
        for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*3.5f+wx*.004f+wy*.003f)*.5f+.5f;
                bool major=(((wx%(step*4)+step*4)%(step*4))==0&&((wy%(step*4)+step*4)%(step*4))==0);
                if(major && gp>0.4f){
                    // Electric spark cross
                    float sl=4.f+gp*5.f;
                    DrawLineEx({(float)sx-sl,(float)sy},{(float)sx+sl,(float)sy},1.8f,ColorAlpha(VP_PURPLE,gp*.8f));
                    DrawLineEx({(float)sx,(float)sy-sl},{(float)sx,(float)sy+sl},1.8f,ColorAlpha(VP_PURPLE,gp*.8f));
                    DrawCircleV({(float)sx,(float)sy},1.5f,ColorAlpha(VP_WHITE,gp*.9f));
                    // occasional arc
                    if(gp>0.85f) DrawNeonCircle({(float)sx,(float)sy},sl,VP_PURPLE,gp*.15f);
                } else {
                    DrawCircleV({(float)sx,(float)sy},0.8f,ColorAlpha(VP_INDIGO,gp*.2f));
                }
            }
        }
        // Glitch horizontal flash bars
        if(sinf(t*7.3f)>0.92f){
            float gy=SCREEN_H*fmodf(t*.31f,1.f);
            DrawRectangle(0,(int)gy,SCREEN_W,2,ColorAlpha(VP_PURPLE,.08f));
        }

    } else if(w<=20){
        // ── ZONE 4: SOLAR INFERNO — burning orange/red hex grid, radial heat ripples ──
        float pulse=.06f+.04f*sinf(t*1.3f)+spdBoost*.06f;
        int step=300;
        int gStartX=camX-((camX%step+step)%step)-step;
        int gStartY=camY-((camY%step+step)%step)-step;
        // Hex approximation: offset every other row
        for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
            int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
            bool major=(((wx%(step*3)+step*3)%(step*3))==0);
            Color lc=major?VP_ORANGE:VP_DARKRED;
            float la=major?pulse*1.8f:pulse*.7f;
            DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(lc,la));
        }
        for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
            int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
            bool major=(((wy%(step*3)+step*3)%(step*3))==0);
            // Wavy heat distortion on horizontal lines
            for(int x=0;x<SCREEN_W;x+=4){
                float heatWave=sinf(x*.015f+t*2.2f)*2.f;
                DrawRectangle(x,(int)(sy+heatWave),4,1,ColorAlpha(major?VP_CORAL:VP_RED,major?pulse*1.5f:pulse*.6f));
            }
        }
        // Heat node glows
        for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*2.8f+wx*.003f+wy*.004f)*.5f+.5f;
                bool major=(((wx%(step*3)+step*3)%(step*3))==0&&((wy%(step*3)+step*3)%(step*3))==0);
                if(major){
                    DrawCircleV({(float)sx,(float)sy},6.f+gp*3.f,ColorAlpha(VP_ORANGE,gp*.10f));
                    DrawCircleV({(float)sx,(float)sy},3.f,ColorAlpha(VP_YELLOW,gp*.6f));
                    DrawNeonCircle({(float)sx,(float)sy},4.f,VP_ORANGE,gp*.3f);
                } else {
                    DrawCircleV({(float)sx,(float)sy},1.2f,ColorAlpha(VP_CORAL,gp*.22f));
                }
            }
        }
        // Radial heat shimmer from center
        {
            float cx2=SCREEN_W*.5f, cy2=SCREEN_H*.5f;
            for(int i=0;i<5;i++){
                float pr=(100.f+i*80.f)+sinf(t*1.5f+i*.8f)*15.f;
                DrawCircleLinesV({cx2,cy2},pr,ColorAlpha(VP_ORANGE,.025f-.003f*i));
            }
        }

    } else if(w<=24){
        // ── ZONE 5: OBSIDIAN / VOID — near-black terrain, razor-thin white lines, cold cyan accents ──
        float pulse=.04f+.02f*sinf(t*.9f)+spdBoost*.035f;
        int step=210; // tighter grid — feels claustrophobic
        int gStartX=camX-((camX%step+step)%step)-step;
        int gStartY=camY-((camY%step+step)%step)-step;
        // Hair-thin dark lines — just barely visible
        for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
            int sx=wx-camX; if(sx<-2||sx>SCREEN_W+2)continue;
            bool major=(((wx%(step*5)+step*5)%(step*5))==0);
            Color zc1=major?VP_CYAN:Color{20,20,35,255};
            DrawLine(sx,0,sx,SCREEN_H,ColorAlpha(zc1,major?pulse*3.f:pulse*.5f));
        }
        for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
            int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
            bool major=(((wy%(step*5)+step*5)%(step*5))==0);
            Color zc2=major?VP_DEEPCYAN:Color{15,15,28,255};
            DrawLine(0,sy,SCREEN_W,sy,ColorAlpha(zc2,major?pulse*2.5f:pulse*.4f));
        }
        // Coldly glowing node crosses at major intersections
        for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                bool major=(((wx%(step*5)+step*5)%(step*5))==0&&((wy%(step*5)+step*5)%(step*5))==0);
                float gp=sinf(t*1.5f+wx*.004f+wy*.003f)*.5f+.5f;
                if(major){
                    DrawCircleV({(float)sx,(float)sy},8.f,ColorAlpha(VP_CYAN,gp*.06f));
                    DrawCircleV({(float)sx,(float)sy},3.f,ColorAlpha(VP_CYAN,gp*.4f));
                    DrawCircleV({(float)sx,(float)sy},1.2f,ColorAlpha(VP_WHITE,gp*.8f));
                    // Thin cross
                    DrawLineEx({(float)sx-8,(float)sy},{(float)sx+8,(float)sy},0.8f,ColorAlpha(VP_DEEPCYAN,gp*.4f));
                    DrawLineEx({(float)sx,(float)sy-8},{(float)sx,(float)sy+8},0.8f,ColorAlpha(VP_DEEPCYAN,gp*.4f));
                } else if(gp>0.9f){
                    DrawCircleV({(float)sx,(float)sy},0.6f,ColorAlpha(VP_CYAN,gp*.25f));
                }
            }
        }
        // Dark void creep — subtle dark rectangle wash flickering at edges
        if(sinf(t*1.1f)>0.6f){
            DrawRectangle(0,0,12,SCREEN_H,ColorAlpha(VP_BLACK,.12f));
            DrawRectangle(SCREEN_W-12,0,12,SCREEN_H,ColorAlpha(VP_BLACK,.12f));
        }

    } else {
        // ── ENDGAME: ABSOLUTE VOID — collapsing grid, fracturing neon, maximum chaos ──
        float pulse=.08f+.05f*sinf(t*3.f)+spdBoost*.08f;
        float collapse=std::min(1.f,(float)(w-25)/3.f); // 0→1 as w goes 25→27
        int step=(int)(180-collapse*80); // grid shrinks/densifies as game progresses
        int gStartX=camX-((camX%step+step)%step)-step;
        int gStartY=camY-((camY%step+step)%step)-step;
        // All four diagonal directions — total chaos grid
        for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
            int sx=wx-camX; if(sx<-4||sx>SCREEN_W+4)continue;
            // Fracture: lines are broken/jagged
            for(int seg=0;seg<8;seg++){
                int y0s=seg*SCREEN_H/8, y1s=(seg+1)*SCREEN_H/8;
                float jx=sinf(t*5.f+wx*.007f+seg*1.3f)*(collapse*8.f);
                DrawLine((int)(sx+jx),y0s,(int)(sx+jx),y1s,ColorAlpha(VP_HOTPINK,pulse*(0.5f+collapse*.6f)));
            }
        }
        for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
            int sy=wy-camY; if(sy<-2||sy>SCREEN_H+2)continue;
            for(int seg=0;seg<8;seg++){
                int x0s=seg*SCREEN_W/8, x1s=(seg+1)*SCREEN_W/8;
                float jy=sinf(t*4.7f+wy*.008f+seg*1.1f)*(collapse*6.f);
                DrawLine(x0s,(int)(sy+jy),x1s,(int)(sy+jy),ColorAlpha(VP_PURPLE,pulse*(0.4f+collapse*.5f)));
            }
        }
        // Diagonal fracture lines
        for(int i=0;i<6;i++){
            float a=t*.8f+i*(PI/3.f);
            float cx2=SCREEN_W*.5f+cosf(a*1.3f)*SCREEN_W*.3f;
            float cy2=SCREEN_H*.5f+sinf(a*.9f)*SCREEN_H*.3f;
            float len=80.f+50.f*collapse;
            DrawLineEx({cx2-cosf(a)*len,cy2-sinf(a)*len},{cx2+cosf(a)*len,cy2+sinf(a)*len},
                1.2f,ColorAlpha(VP_PINK,(0.04f+0.08f*collapse)*pulse*8.f));
        }
        // Collapsing nodes — radiate outward
        for(int wx=gStartX;wx<=camX+SCREEN_W+step;wx+=step){
            for(int wy=gStartY;wy<=camY+SCREEN_H+step;wy+=step){
                int sx=wx-camX,sy=wy-camY;
                if(sx<1||sx>SCREEN_W-1||sy<1||sy>SCREEN_H-1)continue;
                float gp=sinf(t*4.f+wx*.005f+wy*.004f)*.5f+.5f;
                if(gp>0.5f){
                    Color nc=(gp>0.8f)?VP_WHITE:(gp>0.65f)?VP_HOTPINK:VP_PURPLE;
                    DrawCircleV({(float)sx,(float)sy},1.5f+gp*2.f,ColorAlpha(nc,gp*.4f));
                    if(gp>0.88f) DrawNeonCircle({(float)sx,(float)sy},4.f+gp*4.f,nc,gp*.2f);
                }
            }
        }
        // Screen-edge collapse vignette
        float vigA=0.06f+collapse*.12f;
        DrawRectangleGradientH(0,0,60,SCREEN_H,ColorAlpha(VP_BLACK,vigA),{0,0,0,0});
        DrawRectangleGradientH(SCREEN_W-60,0,60,SCREEN_H,{0,0,0,0},ColorAlpha(VP_BLACK,vigA));
        DrawRectangleGradientV(0,0,SCREEN_W,60,ColorAlpha(VP_BLACK,vigA),{0,0,0,0});
        DrawRectangleGradientV(0,SCREEN_H-60,SCREEN_W,60,{0,0,0,0},ColorAlpha(VP_BLACK,vigA));
    }
}

void DrawHazards(Game& g,float t){
    for(auto& h:g.hazards){
        if(!h.active)continue;
        Vector2 sp=W2S(g,h.pos); if(!OnScreen(sp,80))continue;
        float rot=h.angle;

        if(h.type==0){
            // ── ASTEROID — layered irregular heptagon with danger glow ──
            int sides=7;
            float warnPulse=0.45f+0.35f*sinf(t*2.5f+h.angle);

            // outer danger glow
            DrawCircleV(sp,h.radius+14,ColorAlpha(VP_CORAL,.04f*warnPulse));
            DrawCircleV(sp,h.radius+8, ColorAlpha(VP_CORAL,.07f*warnPulse));

            // rock body fill
            Vector2 pts[7];
            for(int i=0;i<sides;i++){
                float aa=rot+i*(2*PI/sides);
                float ri=h.radius*(.78f+.22f*sinf(i*1.3f+h.angle*.5f));
                pts[i]={sp.x+cosf(aa)*ri,sp.y+sinf(aa)*ri};
            }
            for(int i=1;i<sides-1;i++)
                DrawTriangle(sp,pts[i],pts[(i+1)%sides],ColorAlpha(VP_DARKPURP,.30f));

            // neon edge with jagged variation
            for(int i=0;i<sides;i++){
                float edgeA=0.42f+0.18f*sinf(t*1.5f+i*.9f);
                DrawNeonLine(pts[i],pts[(i+1)%sides],VP_GRAY,1.2f,edgeA);
            }
            // inner secondary outline (smaller, counter-rotated)
            for(int i=0;i<sides;i++){
                float aa=-rot*1.3f+i*(2*PI/sides);
                float ri=h.radius*.52f*(0.85f+0.15f*sinf(i*2.1f));
                Vector2 ip={sp.x+cosf(aa)*ri,sp.y+sinf(aa)*ri};
                Vector2 ip2={sp.x+cosf(aa+2*PI/sides)*h.radius*.52f*(0.85f+0.15f*sinf((i+1)*2.1f)),
                             sp.y+sinf(aa+2*PI/sides)*h.radius*.52f*(0.85f+0.15f*sinf((i+1)*2.1f))};
                DrawNeonLine(ip,ip2,VP_MIDGRAY,.7f,.28f);
            }
            // surface crack lines
            DrawLineEx(pts[0],pts[3],0.7f,ColorAlpha(VP_CORAL,.18f));
            DrawLineEx(pts[2],pts[5],0.7f,ColorAlpha(VP_CORAL,.14f));
            // core orange glow
            DrawCircleV(sp,h.radius*.22f,ColorAlpha(VP_CORAL,.14f+.08f*warnPulse));
            DrawNeonCircle(sp,h.radius*.18f,VP_CORAL,.20f*warnPulse);

        } else {
            // ── WARP RIFT — vortex hazard with spinning arms and void center ──
            float r=h.radius;
            float spin=rot;
            float vortex=0.5f+0.5f*sinf(t*3.8f+h.angle);

            // outer ambient haze
            DrawCircleV(sp,r+18,ColorAlpha(VP_DEEPPURP,.06f*vortex));
            DrawCircleV(sp,r+10,ColorAlpha(VP_PURPLE,.10f*vortex));

            // 4 spinning vortex arms
            for(int i=0;i<4;i++){
                float a1=spin+i*(PI*.5f);
                float a2=spin+(i+.6f)*(PI*.5f);
                Vector2 p1={sp.x+cosf(a1)*r,      sp.y+sinf(a1)*r};
                Vector2 p2={sp.x+cosf(a2)*r*.82f,  sp.y+sinf(a2)*r*.82f};
                Vector2 pm={sp.x+cosf((a1+a2)*.5f)*r*.55f,sp.y+sinf((a1+a2)*.5f)*r*.55f};
                DrawTriangle(p1,pm,p2,ColorAlpha(VP_PURPLE,.12f));
                DrawNeonLine(p1,pm,VP_CYAN,1.0f,.58f);
                DrawNeonLine(pm,p2,VP_CYAN,1.0f,.45f);
            }
            // outer ring
            DrawNeonCircle(sp,r,VP_CYAN,.55f+.25f*vortex);
            // mid ring (counter-spin)
            DrawNeonCircle(sp,r*.62f,VP_PURPLE,.38f+.20f*vortex);
            // inner ring
            DrawNeonCircle(sp,r*.32f,VP_LAVENDER,.45f*vortex);

            // cross lines
            DrawNeonLine({sp.x-r*.72f,sp.y},{sp.x+r*.72f,sp.y},VP_PURPLE,.6f,.32f+vortex*.12f);
            DrawNeonLine({sp.x,sp.y-r*.72f},{sp.x,sp.y+r*.72f},VP_PURPLE,.6f,.32f+vortex*.12f);
            // diagonal cross
            DrawNeonLine({sp.x-r*.5f,sp.y-r*.5f},{sp.x+r*.5f,sp.y+r*.5f},VP_CYAN,.5f,.20f);
            DrawNeonLine({sp.x+r*.5f,sp.y-r*.5f},{sp.x-r*.5f,sp.y+r*.5f},VP_CYAN,.5f,.20f);

            // void core (pulsing dark center)
            DrawCircleV(sp,r*.25f,ColorAlpha(VP_BLACK,.85f));
            DrawCircleV(sp,r*.20f,ColorAlpha(VP_DEEPPURP,.60f+.30f*vortex));
            DrawNeonCircle(sp,r*.18f,VP_PURPLE,.70f*vortex);
            DrawCircleV(sp,r*.08f,ColorAlpha(VP_CYAN,.55f+.35f*vortex));
            DrawCircleV(sp,r*.04f,VP_WHITE);
        }
    }
}

// ══════════════════════════════════════════════════════════════
//  ENTITY DRAW
// ══════════════════════════════════════════════════════════════

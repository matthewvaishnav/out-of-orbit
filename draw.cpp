// draw.cpp — Ship silhouette, player, bullets, and pickups rendering
#include "void_blaster.h"

void DrawShipSilhouette(int shipType, Vector2 sc, float a, float t, Color trailCol, bool thrusterOn, bool dashOn, int upgradeTier){
    float ca=cosf(a), sa=sinf(a);
    float eng=(sinf(t*11)+1)*.5f; // engine pulse
    
    // Upgraded hull glow effect — grows stronger per tier
    if(upgradeTier > 0){
        float glowAlpha = 0.15f * (upgradeTier / 10.f);
        float glowRadius = 22.f + upgradeTier * 1.5f;
        DrawCircleV(sc, glowRadius, ColorAlpha(trailCol, glowAlpha));
    }

    switch(shipType){

    // ═══════════════════════════════════════════════════════════
    case 0:{ // INTERCEPTOR — hypersonic needle-class fighter
    // Long tapered spine + large swept delta wings + twin nacelle engines
    // Silhouette: 60px nose-to-tail, wingspan ±24px — very different to Phantom's flat disc
    // ═══════════════════════════════════════════════════════════
        float ep         = sinf(t * 11.f);
        float sensorBlink= 0.5f + 0.5f * sinf(t * 4.5f);

        // ── Silhouette: long needle spine + large swept delta wings ──
        // +fwd/-fwd = nose/tail   +lat/-lat = right/left (using -sa*lat, +ca*lat for left)
        Vector2 tipFwd   = {sc.x + ca*30,          sc.y + sa*30};
        Vector2 cockpitL = {sc.x + ca*18 - sa*3,   sc.y + sa*18 + ca*3};
        Vector2 cockpitR = {sc.x + ca*18 + sa*3,   sc.y + sa*18 - ca*3};
        Vector2 fuseFL   = {sc.x + ca*12 - sa*5,   sc.y + sa*12 + ca*5};
        Vector2 fuseFR   = {sc.x + ca*12 + sa*5,   sc.y + sa*12 - ca*5};
        Vector2 wRootL   = {sc.x + ca*6  - sa*8,   sc.y + sa*6  + ca*8};
        Vector2 wRootR   = {sc.x + ca*6  + sa*8,   sc.y + sa*6  - ca*8};
        // Swept wings — large, delta style
        Vector2 wTipL    = {sc.x - ca*10 - sa*24,  sc.y - sa*10 + ca*24};
        Vector2 wTipR    = {sc.x - ca*10 + sa*24,  sc.y - sa*10 - ca*24};
        Vector2 wInL     = {sc.x - ca*16 - sa*10,  sc.y - sa*16 + ca*10};
        Vector2 wInR     = {sc.x - ca*16 + sa*10,  sc.y - sa*16 - ca*10};
        Vector2 aftL     = {sc.x - ca*18 - sa*5,   sc.y - sa*18 + ca*5};
        Vector2 aftR     = {sc.x - ca*18 + sa*5,   sc.y - sa*18 - ca*5};
        // Twin engine nacelles flanking spine
        Vector2 engL     = {sc.x - ca*20 - sa*6,   sc.y - sa*20 + ca*6};
        Vector2 engR     = {sc.x - ca*20 + sa*6,   sc.y - sa*20 - ca*6};
        Vector2 tailTip  = {sc.x - ca*26,          sc.y - sa*26};

        // ── Hull fill ──
        DrawTriangle(tipFwd,  cockpitL, cockpitR, ColorAlpha(VP_INDIGO,   .75f));
        DrawTriangle(cockpitL,fuseFL,   fuseFR,   ColorAlpha(VP_INDIGO,   .68f));
        DrawTriangle(cockpitL,fuseFR,   cockpitR, ColorAlpha(VP_INDIGO,   .68f));
        // Wing panels (delta surfaces)
        DrawTriangle(fuseFL, wRootL,  wTipL,   ColorAlpha(VP_DEEPPURP, .58f));
        DrawTriangle(fuseFL, wTipL,   wInL,    ColorAlpha(VP_DEEPPURP, .50f));
        DrawTriangle(fuseFL, wInL,    aftL,    ColorAlpha(VP_DEEPPURP, .44f));
        DrawTriangle(fuseFR, wTipR,   wInR,    ColorAlpha(VP_DEEPPURP, .50f));
        DrawTriangle(fuseFR, wRootR,  wTipR,   ColorAlpha(VP_DEEPPURP, .58f));
        DrawTriangle(fuseFR, wInR,    aftR,    ColorAlpha(VP_DEEPPURP, .44f));
        // Center aft spine
        DrawTriangle(aftL,   tailTip, aftR,    ColorAlpha(VP_DEEPPURP, .40f));

        // ── Outer edges ──
        DrawNeonLine(tipFwd,  cockpitL, VP_CYAN,     2.0f, .95f);
        DrawNeonLine(tipFwd,  cockpitR, VP_CYAN,     2.0f, .95f);
        DrawNeonLine(cockpitL,fuseFL,   VP_CYAN,     1.5f, .82f);
        DrawNeonLine(cockpitR,fuseFR,   VP_CYAN,     1.5f, .82f);
        DrawNeonLine(fuseFL,  wRootL,   VP_CYAN,     1.3f, .74f);
        DrawNeonLine(fuseFR,  wRootR,   VP_CYAN,     1.3f, .74f);
        DrawNeonLine(wRootL,  wTipL,    VP_DEEPCYAN, 1.5f, .82f);
        DrawNeonLine(wRootR,  wTipR,    VP_DEEPCYAN, 1.5f, .82f);
        DrawNeonLine(wTipL,   wInL,     VP_DEEPCYAN, 1.1f, .68f);
        DrawNeonLine(wTipR,   wInR,     VP_DEEPCYAN, 1.1f, .68f);
        DrawNeonLine(wInL,    aftL,     VP_DEEPCYAN, 1.0f, .58f);
        DrawNeonLine(wInR,    aftR,     VP_DEEPCYAN, 1.0f, .58f);
        DrawNeonLine(aftL,    tailTip,  VP_DEEPCYAN,  .9f, .48f);
        DrawNeonLine(aftR,    tailTip,  VP_DEEPCYAN,  .9f, .48f);
        // Spine glow line
        DrawNeonLine(tipFwd, tailTip, VP_CYAN, .55f, .14f);

        // ── Internal panel lines ──
        // Fuselage cross-ribs
        for (int i = 0; i < 4; i++) {
            float fx = 14.f - i * 6.f;
            float hw = 4.5f + i * 1.8f;
            Vector2 rl = {sc.x + ca*fx - sa*hw, sc.y + sa*fx + ca*hw};
            Vector2 rr = {sc.x + ca*fx + sa*hw, sc.y + sa*fx - ca*hw};
            DrawLineEx(rl, rr, .55f, ColorAlpha(VP_CYAN, .13f + .04f*sinf(t*3.f+i)));
        }
        // Wing surface panel line (parallel to leading edge, inset)
        Vector2 wPanL1 = {sc.x + ca*3  - sa*12, sc.y + sa*3  + ca*12};
        Vector2 wPanL2 = {sc.x - ca*14 - sa*17, sc.y - sa*14 + ca*17};
        Vector2 wPanR1 = {sc.x + ca*3  + sa*12, sc.y + sa*3  - ca*12};
        Vector2 wPanR2 = {sc.x - ca*14 + sa*17, sc.y - sa*14 - ca*17};
        DrawLineEx(wPanL1, wPanL2, .5f, ColorAlpha(VP_DEEPCYAN, .20f));
        DrawLineEx(wPanR1, wPanR2, .5f, ColorAlpha(VP_DEEPCYAN, .20f));
        // Aileron notch on trailing wing
        Vector2 ailL = {sc.x - ca*12 - sa*15, sc.y - sa*12 + ca*15};
        Vector2 ailR = {sc.x - ca*12 + sa*15, sc.y - sa*12 - ca*15};
        DrawNeonLine(wInL, ailL, VP_CYAN, .65f, .28f);
        DrawNeonLine(wInR, ailR, VP_CYAN, .65f, .28f);

        // ── Sensor array (triple-prong forward spike) ──
        for (int s = -1; s <= 1; s++) {
            float sOff = s * 2.f;
            Vector2 spikeBase = {sc.x + ca*28 - sa*sOff, sc.y + sa*28 + ca*sOff};
            Vector2 spikeTip  = {sc.x + ca*(35+s) - sa*(sOff*1.5f), sc.y + sa*(35+s) + ca*(sOff*1.5f)};
            DrawLineEx(spikeBase, spikeTip, 1.2f - fabsf(s)*.3f, ColorAlpha(VP_DEEPCYAN, .68f - fabsf(s)*.2f));
            if (s == 0) DrawCircleV(spikeTip, 2.2f, ColorAlpha(VP_CYAN, .72f + sensorBlink*.26f));
        }

        // ── Cockpit canopy (elongated teardrop) ──
        Vector2 ck = {sc.x + ca*20, sc.y + sa*20};
        DrawCircleV(ck, 5.f, ColorAlpha(VP_INDIGO, .72f));
        DrawNeonCircle(ck, 4.5f, VP_CYAN, .68f);
        // Glare streak across canopy
        DrawLineEx({ck.x - sa*3.5f + ca*1, ck.y + ca*3.5f + sa*1},
                   {ck.x + sa*0   + ca*2,  ck.y - ca*0   + sa*2}, 1.1f, ColorAlpha(VP_WHITE, .60f));
        DrawCircleV(ck, 2.5f, ColorAlpha(VP_DEEPCYAN, .55f));
        DrawCircleV(ck, 1.1f, ColorAlpha(VP_WHITE, .88f));

        // ── Intake scoops (mid-fuselage, side-mounted) ──
        for (int s = -1; s <= 1; s += 2) {
            float lat = (float)s * 9.f;
            Vector2 intakeF = {sc.x + ca*8  - sa*lat, sc.y + sa*8  + ca*lat};
            Vector2 intakeA = {sc.x + ca*0  - sa*(lat*.9f), sc.y + sa*0  + ca*(lat*.9f)};
            DrawLineEx(intakeF, intakeA, 3.f, ColorAlpha(VP_INDIGO, .78f));
            DrawNeonLine(intakeF, intakeA, VP_CYAN, .8f, .42f);
            DrawCircleV(intakeF, 2.5f, ColorAlpha(VP_DEEPCYAN, .52f));
            // Intake lip
            DrawLineEx({intakeF.x - sa*(s*1.5f), intakeF.y + ca*(s*1.5f)},
                       {intakeA.x - sa*(s*1.5f), intakeA.y + ca*(s*1.5f)}, .4f, ColorAlpha(VP_CYAN, .18f));
        }

        // ── Twin engine nacelles (aft, flanking spine) ──
        for (int si = -1; si <= 1; si += 2) {
            Vector2 nac = {sc.x - ca*20 - sa*(si*6.f), sc.y - sa*20 + ca*(si*6.f)};
            // Nacelle housing tube
            DrawCircleV(nac, 7.f, ColorAlpha(VP_DEEPPURP, .74f));
            DrawNeonCircle(nac, 6.5f, VP_DEEPCYAN, .48f);
            DrawCircleV(nac, 4.5f, ColorAlpha(VP_INDIGO, .62f));
            // Structural rings
            for (int ri2 = 0; ri2 < 4; ri2++) {
                float ringA = a + ri2 * (PI/2.f);
                DrawLineEx(nac, {nac.x+cosf(ringA)*5.5f, nac.y+sinf(ringA)*5.5f}, .6f, ColorAlpha(VP_CYAN, .20f));
            }
            // Strut to aft hull
            Vector2 strutEnd = (si==-1) ? aftL : aftR;
            DrawLineEx(nac, strutEnd, 2.5f, ColorAlpha(VP_INDIGO, .62f));
            DrawNeonLine(nac, strutEnd, VP_CYAN, .7f, .30f);
        }

        // ── Engine exhaust ──
        if (thrusterOn || dashOn) {
            Color ec = dashOn ? VP_WHITE : VP_CYAN;
            Color ec2 = dashOn ? VP_WHITE : VP_DEEPCYAN;
            for (int si = -1; si <= 1; si += 2) {
                Vector2 nac = {sc.x - ca*20 - sa*(si*6.f), sc.y - sa*20 + ca*(si*6.f)};
                float ep2 = sinf(t * 11.f + si * 1.3f);
                // ── Flame cone: stacked blobs along exhaust axis, no tentacle lines ──
                // Each blob is wider at nozzle, tapers toward tip
                static const float flameOffsets[]  = {0.f, 0.30f, 0.55f, 0.74f, 0.88f, 0.97f};
                static const float flameSizes[]    = {7.5f, 6.2f, 4.8f, 3.2f, 1.9f, 0.9f};
                static const float flameAlphas[]   = {0.70f,0.55f,0.42f,0.28f,0.16f,0.08f};
                float coneLen = dashOn ? 52.f : (28.f + ep2*6.f);
                for(int fi=0;fi<6;fi++){
                    // Small perpendicular turbulence — NOT angle-changing (avoids tentacles)
                    float perturb = sinf(t*7.f + fi*1.4f + si) * (1.5f * (1.f - flameOffsets[fi]));
                    Vector2 fp = {
                        nac.x - ca*coneLen*flameOffsets[fi] + (-sa)*perturb,
                        nac.y - sa*coneLen*flameOffsets[fi] +  ca *perturb
                    };
                    float sz = flameSizes[fi] * (dashOn ? 1.6f : 1.f);
                    Color fc = (fi<2) ? VP_WHITE : (fi<4) ? ec : ec2;
                    DrawCircleV(fp, sz * (1.f + ep2*.15f), ColorAlpha(fc, flameAlphas[fi]*(dashOn?1.4f:1.f)));
                }
                // Outer glow halo at nozzle
                DrawCircleV(nac, (10.f+ep2*4)*1.4f, ColorAlpha(VP_DEEPCYAN, .06f));
                DrawNeonCircle(nac, 7.5f+ep2*2.5f, ec, dashOn ? .95f : .85f);
                DrawCircleV(nac, 4.5f, ColorAlpha(VP_WHITE, .82f));
                DrawCircleV(nac, 2.2f, VP_WHITE);
                if (dashOn) {
                    // Extra wide dash afterburner cone
                    for(int fi=0;fi<4;fi++){
                        float fp2 = 0.1f + fi*0.25f;
                        Vector2 dp = {nac.x - ca*72.f*fp2, nac.y - sa*72.f*fp2};
                        DrawCircleV(dp, 9.f*(1.f-fp2*0.8f), ColorAlpha(VP_WHITE, .20f*(1.f-fp2)));
                    }
                    DrawNeonCircle(nac, 11, VP_WHITE, .58f);
                }
            }
        } else {
            for (int si = -1; si <= 1; si += 2) {
                Vector2 nac = {sc.x - ca*20 - sa*(si*6.f), sc.y - sa*20 + ca*(si*6.f)};
                float ep2 = sinf(t*3.5f + si*.9f);
                DrawCircleV(nac, 5.f + ep2*.5f, ColorAlpha(VP_DEEPCYAN, .24f));
                DrawNeonCircle(nac, 4.f, VP_CYAN, .44f);
                DrawCircleV(nac, 2.2f, ColorAlpha(VP_CYAN, .52f));
                DrawCircleV(nac, 1.f,  ColorAlpha(VP_WHITE, .62f));
            }
        }

        // ── Wingtip nav lights ──
        float nb = sinf(t * 4.5f + 1.3f);
        DrawCircleV(wTipL, nb > 0 ? 2.6f : 1.0f, ColorAlpha({255,40,60,255},  fabsf(nb) * .85f));
        DrawCircleV(wTipR, nb < 0 ? 2.6f : 1.0f, ColorAlpha({50,255,100,255}, fabsf(nb) * .85f));
        break;}

    // ═══════════════════════════════════════════════════════════
    case 1:{ // BRAWLER — siege-class heavy gunship
    // Very short, very wide flying tank — shoulder span ±22px, only 35px long
    // Shoulder-mounted double-barrel siege cannons extending past the nose
    // ═══════════════════════════════════════════════════════════
        float pistonPump  = sinf(t * 6.f) * .5f + .5f;
        float ep          = sinf(t * 11.f);
        float armorFlicker= 0.92f + 0.08f * sinf(t * 23.f);

        // ── Silhouette: short wide flying tank ──
        // The widest point (shoulders) is near the nose, not the center
        Vector2 nose    = {sc.x + ca*18,           sc.y + sa*18};
        Vector2 brow    = {sc.x + ca*12 - sa*10,   sc.y + sa*12 + ca*10};
        Vector2 browR   = {sc.x + ca*12 + sa*10,   sc.y + sa*12 - ca*10};
        Vector2 shldrL  = {sc.x + ca*4  - sa*22,   sc.y + sa*4  + ca*22};  // broad shoulders
        Vector2 shldrR  = {sc.x + ca*4  + sa*22,   sc.y + sa*4  - ca*22};
        Vector2 waistL  = {sc.x - ca*4  - sa*18,   sc.y - sa*4  + ca*18};
        Vector2 waistR  = {sc.x - ca*4  + sa*18,   sc.y - sa*4  - ca*18};
        Vector2 engBL   = {sc.x - ca*12 - sa*12,   sc.y - sa*12 + ca*12};
        Vector2 engBR   = {sc.x - ca*12 + sa*12,   sc.y - sa*12 - ca*12};
        Vector2 tailL   = {sc.x - ca*16 - sa*6,    sc.y - sa*16 + ca*6};
        Vector2 tailR   = {sc.x - ca*16 + sa*6,    sc.y - sa*16 - ca*6};
        Vector2 tailTip = {sc.x - ca*17,            sc.y - sa*17};

        // ── Hull fill — heavy armor plate shading ──
        DrawTriangle(nose,   brow,   browR,   ColorAlpha(VP_DEEPPURP, .82f));
        DrawTriangle(brow,   shldrL, waistL,  ColorAlpha(VP_DEEPPURP, .74f));
        DrawTriangle(browR,  waistR, shldrR,  ColorAlpha(VP_DEEPPURP, .74f));
        DrawTriangle(brow,   waistL, waistR,  ColorAlpha(VP_INDIGO,   .68f));
        DrawTriangle(brow,   waistR, browR,   ColorAlpha(VP_INDIGO,   .68f));
        DrawTriangle(waistL, engBL,  engBR,   ColorAlpha(VP_DEEPPURP, .60f));
        DrawTriangle(waistL, engBR,  waistR,  ColorAlpha(VP_DEEPPURP, .60f));
        DrawTriangle(engBL,  tailL,  tailR,   ColorAlpha(VP_DEEPPURP, .50f));
        DrawTriangle(engBL,  tailR,  engBR,   ColorAlpha(VP_DEEPPURP, .50f));

        // ── Hull edges — thick plated ──
        DrawNeonLine(nose,   brow,    VP_ORANGE, 2.2f, 1.00f);
        DrawNeonLine(nose,   browR,   VP_ORANGE, 2.2f, 1.00f);
        DrawNeonLine(brow,   shldrL,  VP_ORANGE, 1.8f, .90f);
        DrawNeonLine(browR,  shldrR,  VP_ORANGE, 1.8f, .90f);
        DrawNeonLine(shldrL, waistL,  VP_GOLD,   1.6f, .80f);
        DrawNeonLine(shldrR, waistR,  VP_GOLD,   1.6f, .80f);
        DrawNeonLine(waistL, engBL,   VP_ORANGE, 1.4f, .70f);
        DrawNeonLine(waistR, engBR,   VP_ORANGE, 1.4f, .70f);
        DrawNeonLine(engBL,  tailL,   VP_GOLD,   1.2f, .60f);
        DrawNeonLine(engBR,  tailR,   VP_GOLD,   1.2f, .60f);
        DrawNeonLine(tailL,  tailTip, VP_ORANGE, 1.0f, .50f);
        DrawNeonLine(tailR,  tailTip, VP_ORANGE, 1.0f, .50f);
        // Horizontal armor braces
        DrawNeonLine(brow,   browR,   VP_GOLD, 1.2f, .55f);
        DrawNeonLine(shldrL, shldrR,  VP_GOLD,  .8f, .32f);
        DrawNeonLine(waistL, waistR,  VP_GOLD,  .8f, .32f);

        // ── Armor panel rivets and weld seams ──
        for (int i = 0; i < 5; i++) {
            float fx = 10.f - i * 4.f;
            float hw = 8.f + i * 2.8f;
            Vector2 rl = {sc.x + ca*fx - sa*hw, sc.y + sa*fx + ca*hw};
            Vector2 rr = {sc.x + ca*fx + sa*hw, sc.y + sa*fx - ca*hw};
            DrawLineEx(rl, rr, .5f, ColorAlpha(VP_GOLD, .14f));
            DrawCircleV(rl, 1.3f, ColorAlpha(VP_ORANGE, .42f * armorFlicker));
            DrawCircleV(rr, 1.3f, ColorAlpha(VP_ORANGE, .42f * armorFlicker));
        }
        // Diagonal X-frame bracing on torso
        DrawLineEx(brow,  waistR, .4f, ColorAlpha(VP_GOLD, .12f));
        DrawLineEx(browR, waistL, .4f, ColorAlpha(VP_GOLD, .12f));

        // ── Shoulder-mounted double-barrel siege cannons ──
        // They extend significantly PAST the nose — dominant visual feature
        for (int s = -1; s <= 1; s += 2) {
            float lat = (float)s * 17.f;
            Vector2 mount    = {sc.x + ca*5  - sa*lat, sc.y + sa*5  + ca*lat};
            Vector2 barrelB  = {sc.x + ca*10 - sa*lat, sc.y + sa*10 + ca*lat};
            float recoil = pistonPump * 2.5f;
            Vector2 barrelT  = {sc.x + ca*(28-recoil) - sa*(lat*.88f), sc.y + sa*(28-recoil) + ca*(lat*.88f)};

            // Mount housing with gear ring
            DrawCircleV(mount, 8.f, ColorAlpha(VP_DEEPPURP, .78f));
            DrawNeonCircle(mount, 7.5f, VP_ORANGE, .68f);
            DrawCircleV(mount, 5.5f, ColorAlpha(VP_INDIGO, .68f));
            // Gear teeth
            for (int g = 0; g < 8; g++) {
                float ga = a + g*(PI/4.f) + t * 1.5f * s;
                Vector2 gTip = {mount.x + cosf(ga)*7.8f, mount.y + sinf(ga)*7.8f};
                DrawCircleV(gTip, 1.4f, ColorAlpha(VP_GOLD, .40f * armorFlicker));
            }
            // Double barrels (side by side)
            for (int b2 = -1; b2 <= 1; b2 += 2) {
                float bOff = (float)b2 * 1.8f;
                Vector2 bB2 = {barrelB.x - sa*bOff, barrelB.y + ca*bOff};
                Vector2 bT2 = {barrelT.x - sa*bOff, barrelT.y + ca*bOff};
                DrawLineEx(bB2, bT2, 4.f, ColorAlpha(VP_DEEPPURP, .90f));
                DrawNeonLine(bB2, bT2, VP_GOLD, 1.1f, .80f);
                // Heat vent slots along barrel
                for (int v = 0; v < 4; v++) {
                    float vf = .25f + v * .18f;
                    Vector2 vp2 = {bB2.x + (bT2.x - bB2.x)*vf, bB2.y + (bT2.y - bB2.y)*vf};
                    DrawLineEx({vp2.x - sa*2.8f, vp2.y + ca*2.8f},
                               {vp2.x + sa*2.8f, vp2.y - ca*2.8f}, .6f, ColorAlpha(VP_ORANGE, .28f));
                }
                // Muzzle brake
                DrawCircleV(bT2, 5.f, ColorAlpha(VP_ORANGE, .90f));
                DrawNeonCircle(bT2, 4.5f, VP_ORANGE, .72f);
                DrawCircleV(bT2, 2.8f, ColorAlpha(VP_YELLOW, .82f));
                DrawCircleV(bT2, 1.4f, ColorAlpha(VP_WHITE, .92f));
                // Muzzle flash (driven by piston pump)
                if (pistonPump > 0.7f) {
                    float flareA = pistonPump - 0.7f;
                    DrawCircleV(bT2, 7.f * flareA, ColorAlpha(VP_YELLOW, .55f * flareA));
                }
            }
        }

        // ── Armored nose plate with viewport slit ──
        DrawCircleV(nose, 6.f, ColorAlpha(VP_DEEPPURP, .65f));
        DrawNeonCircle(nose, 5.5f, VP_ORANGE, .72f);
        DrawCircleV(nose, 2.f, ColorAlpha(VP_WHITE, .80f));

        // ── Hexagonal armored cockpit (set back from nose) ──
        Vector2 ck = {sc.x + ca*10, sc.y + sa*10};
        DrawCircleV(ck, 7.5f, ColorAlpha(VP_DEEPPURP, .70f));
        for (int i = 0; i < 6; i++) {
            float ha1 = a + i*(PI/3.f), ha2 = a + (i+1)*(PI/3.f);
            DrawNeonLine({ck.x+cosf(ha1)*7.f, ck.y+sinf(ha1)*7.f},
                         {ck.x+cosf(ha2)*7.f, ck.y+sinf(ha2)*7.f}, VP_ORANGE, 1.0f, .68f);
        }
        DrawCircleV(ck, 4.8f, ColorAlpha(VP_INDIGO, .62f));
        DrawCircleV(ck, 2.8f, ColorAlpha(VP_GOLD, .52f));
        DrawCircleV(ck, 1.4f, ColorAlpha(VP_WHITE, .80f));
        // Weld line to nose
        DrawLineEx(ck, nose, .5f, ColorAlpha(VP_GOLD, .22f));

        // ── Triple engine bank ──
        Vector2 eC  = {sc.x - ca*15,          sc.y - sa*15};
        Vector2 eSL = {sc.x - ca*14 - sa*9,   sc.y - sa*14 + ca*9};
        Vector2 eSR = {sc.x - ca*14 + sa*9,   sc.y - sa*14 - ca*9};
        for (int ni = 0; ni < 3; ni++) {
            Vector2 ex2 = (ni==0)?eC:(ni==1)?eSL:eSR;
            float nr = (ni==0) ? 9.f : 6.5f;
            DrawCircleV(ex2, nr+4,  ColorAlpha(VP_DEEPPURP, .74f));
            DrawNeonCircle(ex2, nr+2, VP_ORANGE, .58f);
            DrawNeonCircle(ex2, nr,   VP_GOLD,   .36f);
            DrawCircleV(ex2, nr*.5f, ColorAlpha(VP_INDIGO, .62f));
            for (int ri2 = 0; ri2 < 6; ri2++) {
                float ra = a + ri2*(PI/3.f) + t*.9f;
                DrawLineEx(ex2, {ex2.x+cosf(ra)*(nr+1.5f), ex2.y+sinf(ra)*(nr+1.5f)},
                    .5f, ColorAlpha(VP_ORANGE, .22f));
            }
        }
        if (thrusterOn || dashOn) {
            Color ec = dashOn ? VP_WHITE : VP_ORANGE;
            static const float flOff[]  = {0.f, 0.28f, 0.52f, 0.72f, 0.87f, 0.96f};
            static const float flSz[]   = {9.5f, 7.8f, 5.8f, 3.8f, 2.2f, 1.0f};
            static const float flAlpha[]= {0.68f,0.52f,0.38f,0.24f,0.13f,0.06f};
            for (int ni = 0; ni < 3; ni++) {
                Vector2 ex2 = (ni==0)?eC:(ni==1)?eSL:eSR;
                float nr = (ni==0) ? 9.f : 6.5f;
                float ep2 = sinf(t*11.f + ni*.8f);
                float coneLen = dashOn ? 65.f : (ni==0 ? 38.f+ep2*7.f : 26.f+ep2*5.f);
                float szMul = (ni==0) ? 1.f : 0.72f;
                // Fire cone blobs
                for(int fi=0;fi<6;fi++){
                    float perturb = sinf(t*7.2f + fi*1.6f + ni*2.1f) * (2.5f*(1.f-flOff[fi]));
                    Vector2 fp = {
                        ex2.x - ca*coneLen*flOff[fi] + (-sa)*perturb,
                        ex2.y - sa*coneLen*flOff[fi] +   ca*perturb
                    };
                    Color fc = (fi<2)?VP_WHITE:(fi<3)?VP_YELLOW:(fi<5)?VP_ORANGE:Color{200,60,10,255};
                    float fSz = flSz[fi]*szMul*(dashOn?1.5f:1.f);
                    DrawCircleV(fp, fSz*(1.f+ep2*.12f), ColorAlpha(fc, flAlpha[fi]*(dashOn?1.3f:1.f)));
                }
                // Nozzle glow
                DrawCircleV(ex2, (nr+ep2*4)*1.7f, ColorAlpha(VP_RED, .07f));
                DrawNeonCircle(ex2, nr+ep2*3, VP_ORANGE, .46f);
                DrawNeonCircle(ex2, nr+ep2*2, ec, .90f);
                DrawCircleV(ex2, nr*.52f, ColorAlpha(VP_YELLOW, .82f));
                DrawCircleV(ex2, nr*.26f, ColorAlpha(VP_WHITE, .92f));
            }
            if (dashOn) {
                DrawNeonCircle(eC, 18, VP_WHITE, .55f);
                // Full-width dash bloom
                for(int fi=0;fi<5;fi++){
                    float fp2=0.08f+fi*0.18f;
                    Vector2 dp={eC.x-ca*80.f*fp2, eC.y-sa*80.f*fp2};
                    DrawCircleV(dp, 14.f*(1.f-fp2*.75f), ColorAlpha(VP_WHITE, .18f*(1.f-fp2)));
                }
            }
        } else {
            for (int ni = 0; ni < 3; ni++) {
                Vector2 ex2 = (ni==0)?eC:(ni==1)?eSL:eSR;
                float nr = (ni==0) ? 9.f : 6.5f;
                float ep2 = sinf(t*4.f+ni);
                DrawCircleV(ex2, nr*.65f+ep2*.5f, ColorAlpha(VP_ORANGE, .36f));
                DrawNeonCircle(ex2, nr*.52f, VP_ORANGE, .34f);
                DrawCircleV(ex2, nr*.28f, ColorAlpha(VP_WHITE, .52f));
            }
        }
        break;}

    // ═══════════════════════════════════════════════════════════
    case 2:{ // PHANTOM — void-stealth flying wing
    // True flying-wing silhouette: wider than long (±30px span, 22px front-back)
    // Serrated W-notch trailing edge, buried engines, ECM wingtip emitters
    // Completely distinct from Interceptor's needle shape
    // ═══════════════════════════════════════════════════════════
        float gp    = 0.55f + 0.45f * sinf(t * 3.5f);
        float flick = 0.80f + 0.20f * sinf(t * 31.f);  // high-freq ECM flicker
        float ep2   = sinf(t * 9.f);

        // ── Silhouette — wide delta wing, MUCH wider than long ──
        // The entire ship is a flying wing — fuselage IS the wing
        Vector2 noseTip  = {sc.x + ca*14,           sc.y + sa*14};
        Vector2 noseL    = {sc.x + ca*10 - sa*6,    sc.y + sa*10 + ca*6};
        Vector2 noseR    = {sc.x + ca*10 + sa*6,    sc.y + sa*10 - ca*6};
        // Leading edge sweeps sharply to very wide wingtips
        Vector2 leadL    = {sc.x + ca*4  - sa*16,   sc.y + sa*4  + ca*16};
        Vector2 leadR    = {sc.x + ca*4  + sa*16,   sc.y + sa*4  - ca*16};
        Vector2 shouldL  = {sc.x + ca*0  - sa*24,   sc.y + sa*0  + ca*24};
        Vector2 shouldR  = {sc.x + ca*0  + sa*24,   sc.y + sa*0  - ca*24};
        Vector2 wtipL    = {sc.x - ca*4  - sa*30,   sc.y - sa*4  + ca*30};
        Vector2 wtipR    = {sc.x - ca*4  + sa*30,   sc.y - sa*4  - ca*30};
        // Trailing edge — W-shaped stealth notch (4 segments, never parallel to leading edge)
        Vector2 trailL   = {sc.x - ca*8  - sa*27,   sc.y - sa*8  + ca*27};
        Vector2 notch1L  = {sc.x - ca*14 - sa*18,   sc.y - sa*14 + ca*18};
        Vector2 notch1R  = {sc.x - ca*14 + sa*18,   sc.y - sa*14 - ca*18};
        Vector2 centDip  = {sc.x - ca*16,            sc.y - sa*16};
        Vector2 trailR   = {sc.x - ca*8  + sa*27,   sc.y - sa*8  - ca*27};

        // ── Hull fill — dark low-observable panels ──
        float sf = .52f * flick;
        // Left wing
        DrawTriangle(noseTip, noseL,   leadL,    ColorAlpha(VP_DEEPPURP, sf * .92f));
        DrawTriangle(noseL,   leadL,   shouldL,  ColorAlpha(VP_INDIGO,   sf * .80f));
        DrawTriangle(shouldL, wtipL,   trailL,   ColorAlpha(VP_DEEPPURP, sf * .72f));
        DrawTriangle(shouldL, trailL,  notch1L,  ColorAlpha(VP_INDIGO,   sf * .66f));
        // Right wing
        DrawTriangle(noseTip, leadR,   noseR,    ColorAlpha(VP_DEEPPURP, sf * .92f));
        DrawTriangle(noseR,   shouldR, leadR,    ColorAlpha(VP_INDIGO,   sf * .80f));
        DrawTriangle(shouldR, trailR,  wtipR,    ColorAlpha(VP_DEEPPURP, sf * .72f));
        DrawTriangle(shouldR, notch1R, trailR,   ColorAlpha(VP_INDIGO,   sf * .66f));
        // Center fuselage spine
        DrawTriangle(noseL,   noseR,   notch1L,  ColorAlpha(VP_INDIGO,   sf * .84f));
        DrawTriangle(noseR,   notch1R, notch1L,  ColorAlpha(VP_INDIGO,   sf * .84f));
        DrawTriangle(notch1L, notch1R, centDip,  ColorAlpha(VP_DEEPPURP, sf * .74f));

        // ── Hull edges — clean stealth geometry ──
        // Leading edges
        DrawNeonLine(noseTip, noseL,   VP_PURPLE,   2.0f, .94f * flick);
        DrawNeonLine(noseTip, noseR,   VP_PURPLE,   2.0f, .94f * flick);
        DrawNeonLine(noseL,   leadL,   VP_PURPLE,   1.6f, .82f * flick);
        DrawNeonLine(noseR,   leadR,   VP_PURPLE,   1.6f, .82f * flick);
        DrawNeonLine(leadL,   shouldL, VP_LAVENDER, 1.4f, .72f * flick);
        DrawNeonLine(leadR,   shouldR, VP_LAVENDER, 1.4f, .72f * flick);
        DrawNeonLine(shouldL, wtipL,   VP_PURPLE,   1.3f, .68f * flick);
        DrawNeonLine(shouldR, wtipR,   VP_PURPLE,   1.3f, .68f * flick);
        // Serrated trailing edge (W-notch)
        DrawNeonLine(wtipL,   trailL,  VP_LAVENDER, 1.1f, .62f * flick);
        DrawNeonLine(trailL,  notch1L, VP_PURPLE,   1.1f, .68f * flick);
        DrawNeonLine(notch1L, centDip, VP_LAVENDER, 1.2f, .70f * flick);
        DrawNeonLine(centDip, notch1R, VP_LAVENDER, 1.2f, .70f * flick);
        DrawNeonLine(notch1R, trailR,  VP_PURPLE,   1.1f, .68f * flick);
        DrawNeonLine(trailR,  wtipR,   VP_LAVENDER, 1.1f, .62f * flick);

        // ── Stealth panel facets (angled surface lines, never orthogonal) ──
        DrawLineEx(noseTip, notch1L, .45f, ColorAlpha(VP_PURPLE,   .16f * flick));
        DrawLineEx(noseTip, notch1R, .45f, ColorAlpha(VP_PURPLE,   .16f * flick));
        DrawLineEx(noseTip, centDip, .40f, ColorAlpha(VP_LAVENDER, .12f * flick));
        DrawLineEx(leadL,   notch1L, .40f, ColorAlpha(VP_LAVENDER, .12f * flick));
        DrawLineEx(leadR,   notch1R, .40f, ColorAlpha(VP_LAVENDER, .12f * flick));
        DrawLineEx(shouldL, notch1L, .38f, ColorAlpha(VP_PURPLE,   .10f * flick));
        DrawLineEx(shouldR, notch1R, .38f, ColorAlpha(VP_PURPLE,   .10f * flick));

        // ── Phase-shift afterimage echoes (stealth shimmer) ──
        for (int gi = 1; gi <= 4; gi++) {
            float gOff = gi * 4.f;
            float gA   = .042f - gi * .008f;
            Color gc   = (gi%2==0) ? Color{160,80,255,255} : Color{80,220,180,255};
            Vector2 gsc2 = {sc.x - ca*gOff, sc.y - sa*gOff};
            DrawNeonLine({gsc2.x + ca*14,       gsc2.y + sa*14},
                         {gsc2.x + ca*0 - sa*24, gsc2.y + sa*0 + ca*24}, gc, .5f, gA);
            DrawNeonLine({gsc2.x + ca*14,       gsc2.y + sa*14},
                         {gsc2.x + ca*0 + sa*24, gsc2.y + sa*0 - ca*24}, gc, .5f, gA);
        }

        // ── Phase-shift pulse rings ──
        for (int ri = 0; ri < 3; ri++) {
            float rph = fmodf(t * .6f + ri / 3.f, 1.f);
            DrawNeonCircle(sc, 8.f + rph*24.f, VP_PURPLE, (1.f-rph) * .18f * gp);
        }

        // ── Cockpit: ultra-narrow slit visor (barely visible — stealth) ──
        Vector2 ck = {sc.x + ca*10, sc.y + sa*10};
        DrawCircleV(ck, 3.5f, ColorAlpha(VP_DEEPPURP, .62f * gp));
        DrawNeonCircle(ck, 3.0f, VP_PURPLE, .62f);
        // Horizontal slit across visor
        DrawLineEx({ck.x - sa*5.5f, ck.y + ca*5.5f}, {ck.x + sa*5.5f, ck.y - ca*5.5f},
                   1.0f, ColorAlpha(VP_LAVENDER, .58f));
        DrawCircleV(ck, 1.5f, ColorAlpha(VP_LAVENDER, .52f));
        DrawCircleV(ck, .7f,  ColorAlpha(VP_WHITE, .78f));

        // ── Twin engine bays (buried in trailing notches — hidden from front) ──
        for (int si = -1; si <= 1; si += 2) {
            Vector2 bay = {sc.x - ca*12 - sa*(si*10.f), sc.y - sa*12 + ca*(si*10.f)};
            DrawCircleV(bay, 7.f, ColorAlpha(VP_DEEPPURP, .20f));
            DrawNeonCircle(bay, 6.f, VP_PURPLE, .22f * gp);
            // Pentagon blades inside engine bay
            for (int i = 0; i < 5; i++) {
                float ha = t*2.2f + i*(2*PI/5.f), hb = ha + PI/5.f;
                DrawLineEx({bay.x+cosf(ha)*5.f, bay.y+sinf(ha)*5.f},
                           {bay.x+cosf(hb)*5.f, bay.y+sinf(hb)*5.f},
                           .65f, ColorAlpha(VP_LAVENDER, .24f + .12f*gp));
            }
            if (thrusterOn || dashOn) {
                Color ec = dashOn ? VP_WHITE : VP_PURPLE;
                float ep2 = sinf(t*9.f + si*.8f);
                float coneLen = dashOn ? 55.f : (22.f + ep2*5.f);
                // Fire cone blobs — shorter than Interceptor, more purple, narrower
                static const float pFlOff[]  = {0.f, 0.30f, 0.56f, 0.76f, 0.90f, 0.98f};
                static const float pFlSz[]   = {6.5f, 5.2f, 3.8f, 2.4f, 1.3f, 0.6f};
                static const float pFlAlpha[]= {0.62f,0.48f,0.35f,0.22f,0.12f,0.05f};
                for(int fi=0;fi<6;fi++){
                    float perturb = sinf(t*8.f + fi*1.5f + si*1.8f) * (1.2f*(1.f-pFlOff[fi]));
                    Vector2 fp = {
                        bay.x - ca*coneLen*pFlOff[fi] + (-sa)*perturb,
                        bay.y - sa*coneLen*pFlOff[fi] +   ca*perturb
                    };
                    Color fc = (fi<2) ? VP_WHITE : (fi<4) ? ec : VP_DEEPPURP;
                    DrawCircleV(fp, pFlSz[fi]*(dashOn?1.5f:1.f)*(1.f+ep2*.1f), ColorAlpha(fc, pFlAlpha[fi]*(dashOn?1.4f:1.f)));
                }
                // Nozzle halo
                DrawNeonCircle(bay, 8.5f+ep2*1.5f, VP_LAVENDER, .38f);
                DrawNeonCircle(bay, 6.5f+ep2*.8f,  ec, .90f);
                DrawCircleV(bay, 3.5f, ColorAlpha(VP_LAVENDER, .70f));
                DrawCircleV(bay, 1.8f, ColorAlpha(VP_WHITE, .85f));
                if (dashOn) {
                    for(int fi=0;fi<4;fi++){
                        float fp2=0.1f+fi*0.22f;
                        Vector2 dp={bay.x-ca*62.f*fp2, bay.y-sa*62.f*fp2};
                        DrawCircleV(dp, 8.f*(1.f-fp2*.8f), ColorAlpha(VP_WHITE, .18f*(1.f-fp2)));
                    }
                    DrawNeonCircle(bay, 11, VP_WHITE, .52f);
                }
            } else {
                DrawNeonCircle(bay, 4.5f + sinf(t*4+si), VP_PURPLE, .40f * gp);
                DrawCircleV(bay, 2.2f, ColorAlpha(VP_PURPLE, .38f));
                DrawCircleV(bay, .9f,  ColorAlpha(VP_WHITE, .52f));
            }
        }

        // ── ECM emitters on wingtips (pulsing electronic warfare nodes) ──
        for (int si = -1; si <= 1; si += 2) {
            Vector2 wt = (si==-1) ? wtipL : wtipR;
            float ecmPulse = sinf(t * 8.5f + si * 2.f);
            DrawCircleV(wt, 4.f, ColorAlpha(VP_DEEPPURP, .52f));
            DrawNeonCircle(wt, 3.5f, VP_LAVENDER, .38f + ecmPulse * .22f);
            DrawCircleV(wt, 2.2f, ColorAlpha(VP_PURPLE, .52f + ecmPulse * .28f));
            DrawCircleV(wt, 1.1f, ColorAlpha(VP_WHITE, .58f));
            // Expanding ECM pulse ring
            float ecmR = fmodf(t * 1.2f + (si+1) * .5f, 1.f);
            DrawNeonCircle(wt, 5.f + ecmR * 14.f, VP_LAVENDER, (1.f-ecmR) * .28f * gp);
        }
        break;}

    // ═══════════════════════════════════════════════════════════
    case 3:{ // TITAN — flying Shinto shrine-fortress
    // Octagonal hull + torii + lanterns — unchanged, already feels like a space artifact
    // ═══════════════════════════════════════════════════════════
        float rPulse   = sinf(t * 5.5f) * .5f + .5f;
        float bellSway = sinf(t * 2.2f) * 4.f;
        float runePhase = fmodf(t * .4f, 1.f);

        // ── LAYER 1: octagonal stone base platform ──
        float hr = 18.f;
        Vector2 hull[8];
        for (int i = 0; i < 8; i++) {
            float aa = a + i*(2*PI/8) + PI/8.f;
            float breathe = 1.f + .02f*sinf(t*1.5f+i*.8f);
            hull[i] = {sc.x+cosf(aa)*hr*breathe, sc.y+sinf(aa)*hr*breathe};
        }
        for (int i = 1; i < 7; i++)
            DrawTriangle(hull[0], hull[i], hull[i+1], ColorAlpha(VP_INDIGO, i%2==0 ? .52f : .40f));
        for (int i = 0; i < 8; i++)
            DrawTriangle(sc, hull[i], hull[(i+1)%8], ColorAlpha(VP_DEEPPURP, .15f));
        for (int i = 0; i < 8; i++)
            DrawNeonLine(hull[i], hull[(i+1)%8], VP_MINT, 1.6f, .90f);

        // ── LAYER 2: glowing rune circle ──
        for (int i = 0; i < 6; i++) {
            float ra = a + t*.18f + i*(2*PI/6);
            float rb = ra + PI/6;
            float rr = hr * .55f;
            int seg = (int)(runePhase*6);
            float alpha2 = (i==seg) ? .75f : .22f;
            Vector2 rp1 = {sc.x+cosf(ra)*rr, sc.y+sinf(ra)*rr};
            Vector2 rp2 = {sc.x+cosf(rb)*rr, sc.y+sinf(rb)*rr};
            DrawNeonLine(rp1, rp2, VP_MINT, 1.0f, alpha2*rPulse);
            DrawCircleV({(rp1.x+rp2.x)*.5f,(rp1.y+rp2.y)*.5f}, 2.f, ColorAlpha(VP_MINT, alpha2*.8f));
        }

        // ── LAYER 3: torii gate frame ──
        Vector2 toriiTop  = {sc.x + ca*14,         sc.y + sa*14};
        Vector2 toriiL    = {sc.x + ca*6  - sa*16,  sc.y + sa*6  + ca*16};
        Vector2 toriiR    = {sc.x + ca*6  + sa*16,  sc.y + sa*6  - ca*16};
        Vector2 toriiBaseL= {sc.x          - sa*14,  sc.y          + ca*14};
        Vector2 toriiBaseR= {sc.x          + sa*14,  sc.y          - ca*14};
        DrawLineEx(toriiL, toriiBaseL, 4.f, ColorAlpha(VP_INDIGO, .85f));
        DrawLineEx(toriiR, toriiBaseR, 4.f, ColorAlpha(VP_INDIGO, .85f));
        DrawNeonLine(toriiL, toriiBaseL, VP_TEAL, 1.4f, .70f);
        DrawNeonLine(toriiR, toriiBaseR, VP_TEAL, 1.4f, .70f);
        DrawLineEx(toriiL, toriiR, 5.f, ColorAlpha(VP_INDIGO, .90f));
        DrawNeonLine(toriiL, toriiR, VP_MINT, 1.8f, .85f);
        DrawLineEx(toriiTop, toriiL, 3.5f, ColorAlpha(VP_INDIGO, .80f));
        DrawLineEx(toriiTop, toriiR, 3.5f, ColorAlpha(VP_INDIGO, .80f));
        DrawNeonLine(toriiTop, toriiL, VP_MINT, 1.2f, .75f);
        DrawNeonLine(toriiTop, toriiR, VP_MINT, 1.2f, .75f);
        DrawCircleV(toriiTop, 4.f, ColorAlpha(VP_GOLD, .60f + rPulse*.20f));
        DrawNeonCircle(toriiTop, 3.5f, VP_GOLD, .70f);
        DrawCircleV(toriiTop, 2.f, ColorAlpha(VP_WHITE, .70f));

        // ── LAYER 4: hanging bells ──
        for (int i = 0; i < 3; i++) {
            float bx = -.5f + i*.5f;
            Vector2 hook = {sc.x + ca*6 + sa*bx*12, sc.y + sa*6 - ca*bx*12};
            float sway2  = bellSway * (1.f + i*.2f);
            Vector2 bell = {hook.x - ca*8 + sa*sway2*.15f, hook.y - sa*8 - ca*sway2*.15f};
            DrawLineEx(hook, bell, 1.f, ColorAlpha(VP_TEAL, .50f));
            DrawCircleV(bell, 3.5f-i*.3f, ColorAlpha(VP_GOLD, .55f));
            DrawCircleLinesV(bell, 3.5f-i*.3f, ColorAlpha(VP_MINT, .60f));
            DrawCircleV(bell, 1.5f, ColorAlpha(VP_WHITE, .40f + rPulse*.20f));
        }

        // ── LAYER 5: orbiting prayer beads ──
        for (int i = 0; i < 12; i++) {
            float ba = a + t*.6f + i*(2*PI/12);
            float br = hr*1.15f + sinf(t*2+i)*1.5f;
            Vector2 bp = {sc.x + cosf(ba)*br, sc.y + sinf(ba)*br};
            float bs = 1.8f + .5f*sinf(t*3+i*1.3f);
            DrawCircleV(bp, bs,     ColorAlpha(VP_TEAL, .50f));
            DrawCircleV(bp, bs*.5f, ColorAlpha(VP_WHITE, .35f));
        }

        // ── LAYER 6: central shrine lantern core ──
        DrawCircleV(sc, hr*.35f, ColorAlpha(VP_INDIGO, .40f));
        DrawNeonCircle(sc, hr*.32f, VP_TEAL, .45f * rPulse);
        DrawCircleV(sc, hr*.20f, ColorAlpha(VP_MINT, .35f + rPulse*.15f));
        DrawNeonCircle(sc, hr*.18f, VP_MINT, .60f);
        DrawCircleV(sc, hr*.10f, ColorAlpha(VP_WHITE, .60f + rPulse*.20f));
        DrawCircleV(sc, hr*.04f, VP_WHITE);

        // ── LAYER 7: four shrine-lantern engines ──
        float offs[4] = {-12.f, -4.f, 4.f, 12.f};
        for (int i = 0; i < 4; i++) {
            Vector2 ex = {sc.x - ca*16 + sa*offs[i], sc.y - sa*16 - ca*offs[i]};
            bool outer2 = (i==0 || i==3);
            float nr = outer2 ? 6.f : 5.f;
            DrawCircleV(ex, nr+2.f, ColorAlpha(VP_INDIGO, .65f));
            DrawNeonCircle(ex, nr+1.f, VP_TEAL, .40f);
            DrawCircleV(ex, nr, ColorAlpha(VP_MINT, .15f + rPulse*.10f));
            DrawNeonCircle(ex, nr*.7f, VP_MINT, .50f);
            for (int ri = 0; ri < 4; ri++) {
                float ra = a + ri*(PI/2) + t*.3f;
                DrawLineEx(ex, {ex.x+cosf(ra)*(nr+1), ex.y+sinf(ra)*(nr+1)}, .8f, ColorAlpha(VP_TEAL, .35f));
            }
            if (thrusterOn || dashOn) {
                Color ec = dashOn ? VP_WHITE : VP_MINT;
                float er = outer2 ? 8.f : 6.5f;
                float ep2 = sinf(t*11 + i*.8f);
                float coneLen = dashOn ? 42.f : (er==8.f ? 24.f+ep2*5.f : 18.f+ep2*4.f);
                // Fire cone blobs — teal/mint sacred flame
                static const float tFlOff[]  = {0.f, 0.30f, 0.56f, 0.76f, 0.90f, 0.98f};
                static const float tFlSz[]   = {er, er*.78f, er*.56f, er*.36f, er*.20f, er*.10f};
                static const float tFlAlpha[]= {0.65f,0.50f,0.36f,0.22f,0.11f,0.05f};
                for(int fi=0;fi<6;fi++){
                    float perturb = sinf(t*6.5f + fi*1.4f + i*1.8f) * (1.8f*(1.f-tFlOff[fi]));
                    Vector2 fp = {
                        ex.x - ca*coneLen*tFlOff[fi] + (-sa)*perturb,
                        ex.y - sa*coneLen*tFlOff[fi] +   ca*perturb
                    };
                    Color fc = (fi<2)?VP_WHITE:(fi<4)?ec:VP_TEAL;
                    DrawCircleV(fp, tFlSz[fi]*(dashOn?1.4f:1.f)*(1.f+ep2*.1f), ColorAlpha(fc, tFlAlpha[fi]*(dashOn?1.3f:1.f)));
                }
                // Nozzle glow
                DrawCircleV(ex, (er+ep2*4.f)*1.8f, ColorAlpha(VP_TEAL, .07f));
                DrawNeonCircle(ex, er+ep2*2.5f, ec, .90f);
                DrawCircleV(ex, er*.48f, ColorAlpha(VP_WHITE, .84f));
                DrawCircleV(ex, er*.24f, VP_WHITE);
                if (dashOn) {
                    DrawCircleV(ex, er*2.f, ColorAlpha(VP_WHITE, .10f));
                    DrawNeonCircle(ex, er+7, VP_WHITE, .52f);
                }
            } else {
                float ep2 = sinf(t*3.5f + i*.9f);
                DrawCircleV(ex, nr*.7f+ep2*.6f, ColorAlpha(VP_TEAL, .30f));
                DrawNeonCircle(ex, nr*.5f, VP_MINT, .40f * rPulse);
                DrawCircleV(ex, nr*.25f, ColorAlpha(VP_WHITE, .50f));
            }
        }
        break;}
    }
}
void DrawPlayer(Game& g,Vector2 sc,float t){
    // ── Engine trail — fire core → colored smoke → dark dissipating cloud ──
    ShipType st=(ShipType)g.selectedShip;
    Color tCol=(st==SHIP_INTERCEPTOR)?VP_CYAN:(st==SHIP_BRAWLER)?VP_ORANGE:(st==SHIP_PHANTOM)?VP_PURPLE:VP_MINT;
    // Secondary smoke color (cooler, darker version of ship color)
    Color smokeCol = (st==SHIP_INTERCEPTOR) ? Color{0,60,90,255}
                   : (st==SHIP_BRAWLER)     ? Color{90,30,0,255}
                   : (st==SHIP_PHANTOM)     ? Color{50,0,90,255}
                   :                          Color{0,60,30,255};

    int trailN=(int)g.engineTrail.size();
    bool dashing=(g.dashTimer>0);

    for(int ti=0;ti<trailN;ti++){
        auto& tp=g.engineTrail[ti];
        Vector2 sp=W2S(g,tp.pos);
        if(sp.x<-60||sp.x>SCREEN_W+60||sp.y<-60||sp.y>SCREEN_H+60) continue;

        float lf   = tp.life;           // 1.0 (fresh) → 0.0 (gone)
        float age  = 1.f - lf;          // 0.0 (fresh) → 1.0 (old)
        float fresh= (float)(trailN-ti)/trailN; // 1=newest 0=oldest in queue

        if(dashing){
            // ── DASH: bright white plasma streak ──
            float r2 = 8.f * lf;
            DrawCircleV(sp, r2*2.8f, ColorAlpha(VP_WHITE, lf*.04f));
            DrawCircleV(sp, r2*1.4f, ColorAlpha(VP_WHITE, lf*.18f));
            DrawCircleV(sp, r2,      ColorAlpha(VP_WHITE, lf*.60f));
            DrawCircleV(sp, r2*.4f,  ColorAlpha(VP_WHITE, lf*.95f));
            // Connect consecutive points as bright streak
            if(ti>0){
                Vector2 sp2=W2S(g,g.engineTrail[ti-1].pos);
                if(sp2.x>=-60&&sp2.x<=SCREEN_W+60&&sp2.y>=-60&&sp2.y<=SCREEN_H+60)
                    DrawLineEx(sp,sp2, r2*2.f, ColorAlpha(VP_WHITE, lf*.22f));
            }
        } else {
            // ── NORMAL: layered fire → colored glow → smoke dissipation ──
            // Size: starts small (hot), then EXPANDS as it cools (smoke rises & spreads)
            // Max expansion at ~40% life, then gradually fades
            float expandFactor = 1.f + age*3.2f * std::max(0.f, 1.f - age*1.8f);
            float r2 = (4.5f + expandFactor*5.5f) * lf;

            // Phase 1: HOT CORE (very fresh only — first ~30% of life)
            if(age < 0.30f){
                float hotT  = 1.f - age/0.30f;   // 1=brand new, 0=cooling
                // White-hot innermost
                DrawCircleV(sp, r2*0.28f, ColorAlpha(VP_WHITE,  hotT * lf * .90f));
                // Ship-colored inner ring
                DrawCircleV(sp, r2*0.55f, ColorAlpha(tCol,      hotT * lf * .72f));
                // Mid flame body
                DrawCircleV(sp, r2*0.85f, ColorAlpha(tCol,      hotT * lf * .42f));
            }

            // Phase 2: GLOWING EMBER BODY (0%–60% of life)
            if(age < 0.60f){
                float glowT = 1.f - age/0.60f;
                DrawCircleV(sp, r2*1.0f, ColorAlpha(tCol,      glowT * lf * .28f));
                DrawCircleV(sp, r2*1.4f, ColorAlpha(tCol,      glowT * lf * .12f));
            }

            // Phase 3: SMOKE PUFF (expands, fades to near-black smoke cloud)
            // This is the bulk of the visual — wide dark cloud that lingers
            float smokeAlpha = lf * (0.06f + age*0.10f) * std::max(0.f, 1.f-age*.6f);
            DrawCircleV(sp, r2*2.0f,  ColorAlpha(smokeCol,  smokeAlpha));
            DrawCircleV(sp, r2*2.8f,  ColorAlpha(smokeCol,  smokeAlpha*.45f));
            DrawCircleV(sp, r2*3.8f,  ColorAlpha(smokeCol,  smokeAlpha*.18f));

            // Connect nearest consecutive points with a tapered streak (motion continuity)
            if(ti>0 && fresh>0.7f){
                Vector2 sp2=W2S(g,g.engineTrail[ti-1].pos);
                if(sp2.x>=-60&&sp2.x<=SCREEN_W+60&&sp2.y>=-60&&sp2.y<=SCREEN_H+60){
                    float strkA = (1.f - age/0.20f) * lf * .18f;
                    if(strkA > 0.01f)
                        DrawLineEx(sp, sp2, r2*1.2f, ColorAlpha(tCol, strkA));
                }
            }
        }
    }
    bool blink=(g.invincTimer>0&&!g.tranceActive&&(int)(t*10)%2==0);
    // Skip drawing regular ship during wave transition animation to prevent overlap
    if(blink || g.waveTransTimer>0) return;

    float a=g.playerAngle;

    // ── Shield bubble ──
    if(g.shieldHp>0){
        float sr=36+sinf(t*4)*2.5f;
        DrawCircleV(sc,sr+4,ColorAlpha(VP_CYAN,.04f));
        DrawCircleV(sc,sr+2,ColorAlpha(VP_CYAN,.07f));
        DrawNeonCircle(sc,sr,VP_CYAN,.85f);
        DrawNeonCircle(sc,sr-3,VP_CYAN,.25f);
        // hexagonal grid on bubble surface — 7 hex cells projected on circle
        for(int hi=0;hi<6;hi++){
            float ha=t*.3f+hi*(PI/3.f);
            float hcx=sc.x+cosf(ha)*sr*.55f, hcy=sc.y+sinf(ha)*sr*.55f;
            for(int hj=0;hj<6;hj++){
                float ha2=hj*(PI/3.f);
                float ha3=(hj+1)*(PI/3.f);
                Vector2 hp1={hcx+cosf(ha2)*sr*.25f,hcy+sinf(ha2)*sr*.25f};
                Vector2 hp2={hcx+cosf(ha3)*sr*.25f,hcy+sinf(ha3)*sr*.25f};
                DrawLineEx(hp1,hp2,.7f,ColorAlpha(VP_CYAN,.18f));
            }
        }
        for(int i=0;i<6;i++){
            float ta=a+i*(PI/3.f)+t*1.2f;
            Vector2 tp={sc.x+cosf(ta)*sr,sc.y+sinf(ta)*sr};
            DrawCircleV(tp,3.f,ColorAlpha(VP_CYAN,.7f));
            DrawCircleV(tp,1.5f,VP_WHITE);
        }
    }

    // ── Overdrive aura ──
    if(g.tranceActive){
        float tp2=sinf(t*8)*.5f+.5f;
        DrawCircleV(sc,42+tp2*8,ColorAlpha(VP_HOTPINK,.04f+tp2*.03f));
        DrawNeonCircle(sc,36+tp2*9,VP_HOTPINK,.5f+tp2*.3f);
        DrawNeonCircle(sc,26,VP_PURPLE,.35f);
        // radial energy spokes
        for(int i=0;i<12;i++){
            float sa2=a+i*(2*PI/12.f)+t*1.8f;
            float r1=18+sinf(t*5+i)*3.f;
            float r2=28+sinf(t*4+i*1.3f+1)*5.f;
            DrawLineEx({sc.x+cosf(sa2)*r1,sc.y+sinf(sa2)*r1},
                       {sc.x+cosf(sa2)*r2,sc.y+sinf(sa2)*r2},
                       .8f,ColorAlpha(VP_HOTPINK,tp2*.28f));
        }
        for(int i=0;i<8;i++){
            float pa=a+i*(2*PI/8.f)+t*4.f;
            float pr=28+sinf(t*6+i)*.5f;
            Vector2 op={sc.x+cosf(pa)*pr,sc.y+sinf(pa)*pr};
            DrawCircleV(op,3.5f,ColorAlpha(tCol,.85f));
            DrawCircleV(op,2.f,ColorAlpha(VP_WHITE,.9f));
        }
    }

    // ── Gun heat glow — tight heat shimmer that builds with temperature ──
    {
        float heatPct = g.chiGauge / 100.f;
        bool t1=g.chiGauge>=40.f, t2=g.chiGauge>=70.f, t3=g.chiGauge>=88.f;
        bool venting = g.heatVentTimer > 0.f;
        if(heatPct > 0.08f || venting){
            Color heatCol = venting ? Color{255,80,0,255}
                          : t3      ? Color{255,60,0,255}
                          : t2      ? Color{255,150,0,255}
                          : Color{255,210,0,255};
            float cp = t3||venting ? sinf(t*(venting?20.f:11.f))*.5f+.5f : heatPct;
            // Single tight heat ring around hull — no outer halos
            float ringR = 16.f + heatPct*10.f;
            DrawNeonCircle(sc, ringR, heatCol, heatPct*0.28f + cp*(t3||venting ? 0.18f : 0.06f));
            // Vent state: steam wisps ejected backward
            if(venting && (int)(GetTime()*30)%2==0){
                float ba = sc.x > 0 ? PI : 0.f; // away from ship center
                float wa = g.playerAngle + PI + RandF(-.6f,.6f);
                SpawnParticles(g,{sc.x+cosf(wa)*14,sc.y+sinf(wa)*14},{255,200,100,255},1,25,false);
            }
        }
        // Speed streak — kept, cleaned up to single pass
        float speed = sqrtf(g.playerVel.x*g.playerVel.x+g.playerVel.y*g.playerVel.y);
        float spdFac = std::min(1.f, speed/220.f);
        if(spdFac > 0.20f){
            float nx2=-g.playerVel.x/speed, ny2=-g.playerVel.y/speed;
            DrawLineEx(sc,{sc.x+nx2*24*spdFac,sc.y+ny2*24*spdFac},
                       4.f*spdFac,ColorAlpha(tCol,.10f*spdFac));
        }
    }

    // ── Ship body ──
    DrawShipSilhouette(g.selectedShip, sc, a, t, tCol, g.thrusterOn, g.dashTimer>0, g.shipUpgradeTier);

    // ── Weapon cannon tip ──
    Color pCol=PrayerColor(g.prayer);
    float ct=GetTime();
    Vector2 cannonBase={sc.x+cosf(a)*9,sc.y+sinf(a)*9};
    Vector2 cannon={sc.x+cosf(a)*22,sc.y+sinf(a)*22};

    // barrel glow line (prayer-colored)
    DrawNeonLine(cannonBase,cannon,pCol,2.0f,.55f);
    DrawNeonLine(cannonBase,cannon,VP_YELLOW,0.8f,.22f);

    bool isIdle=(g.idleShootTimer>2.0f);
    float mGlow=sinf(ct*8.f)*.3f+.7f;

    if(isIdle){
        // resting: dim breathe, no effects
        float breathe=0.32f+0.18f*sinf(ct*1.4f);
        DrawCircleV(cannon,8.0f, ColorAlpha(pCol,breathe*.05f));
        DrawNeonCircle(cannon,5.5f,pCol,breathe*.40f);
        DrawCircleV(cannon,3.0f, ColorAlpha(VP_WHITE,breathe*.32f));
        DrawCircleV(cannon,1.4f, ColorAlpha(pCol,breathe*.68f));
    } else {
        // active: weapon-specific cannon charge visual
        if(g.prayer==PRAYER_FIRE){
            // SOLAR FLARE: miniature charging sun — corona spins faster as it charges
            float chargePulse=0.6f+0.4f*sinf(ct*12.f);
            DrawCircleV(cannon,12.f,ColorAlpha(VP_RED,.07f*mGlow));
            DrawCircleV(cannon,8.5f,ColorAlpha(VP_ORANGE,.15f*mGlow));
            DrawNeonCircle(cannon,6.5f+sinf(ct*8)*2.0f,VP_RED,.50f*mGlow);
            DrawNeonCircle(cannon,4.5f,VP_ORANGE,.75f*mGlow);
            DrawCircleV(cannon,3.5f,ColorAlpha(VP_ORANGE,.90f));
            DrawCircleV(cannon,2.0f,ColorAlpha(VP_YELLOW,.95f));
            DrawCircleV(cannon,1.0f,VP_WHITE);
            // 8 radial corona flares spinning fast
            for(int i=0;i<8;i++){
                float fa=ct*7.f+i*(2*PI/8.f);
                float fr=6.5f+sinf(ct*9+i)*2.2f;
                DrawLineEx(cannon,{cannon.x+cosf(fa)*fr,cannon.y+sinf(fa)*fr},1.1f,ColorAlpha(VP_ORANGE,mGlow*.40f));
                if(i%2==0) DrawCircleV({cannon.x+cosf(fa)*fr,cannon.y+sinf(fa)*fr},1.3f,ColorAlpha(VP_YELLOW,chargePulse*.80f));
            }
        } else if(g.prayer==PRAYER_ICE){
            // VOID LANCE: narrow charged needle — thin crosshair shimmer
            DrawCircleV(cannon,8.f,ColorAlpha(VP_DEEPCYAN,.08f*mGlow));
            DrawNeonCircle(cannon,5.0f+sinf(ct*6)*0.8f,VP_CYAN,.50f*mGlow);
            DrawNeonCircle(cannon,3.5f,VP_LAVENDER,.70f*mGlow);
            DrawCircleV(cannon,2.5f,ColorAlpha(VP_CYAN,.90f));
            DrawCircleV(cannon,1.4f,VP_WHITE);
            // 4-point crosshair rotating slowly — feels like a targeting reticle
            for(int i=0;i<4;i++){
                float fa=ct*(-0.8f)+i*(PI*.5f);
                float r1=4.0f, r2=9.0f+sinf(ct*4+i)*.8f;
                DrawLineEx({cannon.x+cosf(fa)*r1,cannon.y+sinf(fa)*r1},
                           {cannon.x+cosf(fa)*r2,cannon.y+sinf(fa)*r2},
                           1.0f,ColorAlpha(VP_CYAN,mGlow*.60f));
                DrawCircleV({cannon.x+cosf(fa)*r2,cannon.y+sinf(fa)*r2},1.2f,ColorAlpha(VP_LAVENDER,mGlow*.75f));
            }
        } else { // SOUL SWARM
            // Soul Swarm: 3 wisps orbiting the cannon mouth before launch
            DrawCircleV(cannon,10.f,ColorAlpha(VP_PURPLE,.07f*mGlow));
            DrawNeonCircle(cannon,6.5f+sinf(ct*5)*1.5f,VP_PURPLE,.28f*mGlow);
            DrawNeonCircle(cannon,4.5f,VP_LAVENDER,.55f*mGlow);
            DrawCircleV(cannon,3.0f,ColorAlpha(VP_PURPLE,.88f));
            DrawCircleV(cannon,1.8f,ColorAlpha(VP_LAVENDER,.85f));
            DrawCircleV(cannon,0.9f,VP_WHITE);
            // 3 pre-launch wisps orbiting tight then releasing
            for(int i=0;i<3;i++){
                float wa=ct*(i==0?6.f:-4.5f)+i*(2*PI/3.f);
                float wr=5.5f+sinf(ct*3.f+i)*1.8f;
                Color wc=(i==0)?VP_PURPLE:(i==1)?VP_LAVENDER:VP_HOTPINK;
                Vector2 wp={cannon.x+cosf(wa)*wr,cannon.y+sinf(wa)*wr};
                DrawCircleV(wp,2.8f,ColorAlpha(wc,mGlow*.75f));
                DrawCircleV(wp,1.4f,ColorAlpha(VP_WHITE,mGlow*.85f));
            }
            DrawNeonCircle(cannon,8.5f+sinf(ct*2.f)*2.f,VP_PURPLE,.14f*mGlow);
        }
    }

    // ── Pulse aura ──
    float pp=sinf(t*2.2f)*.5f+.5f;
    DrawNeonCircle(sc,PLAYER_RADIUS+2+pp*3.5f,tCol,pp*.12f);
}
void DrawBullet(Bullet& b,Vector2 sp,float t){
    float spd=sqrtf(b.vel.x*b.vel.x+b.vel.y*b.vel.y);
    float nx=spd>1?b.vel.x/spd:0,ny=spd>1?b.vel.y/spd:0;
    float px=-ny, py=nx;

    bool isSolar =(b.prayerType==0);
    bool isLance =(b.prayerType==1);
    bool isSwarm =(b.prayerType==2);
    bool isTrance=(b.prayerType==3);

    if(isTrance){
        // ── OVERDRIVE COMET ── (~20 draw calls)
        float cyc=fmodf(t*5.5f+b.pos.x*.009f+b.pos.y*.006f,3.f);
        Color c1=cyc<1?LerpC(VP_PINK,VP_CYAN,cyc):cyc<2?LerpC(VP_CYAN,VP_PURPLE,cyc-1):LerpC(VP_PURPLE,VP_PINK,cyc-2);
        Color c2=cyc<1?LerpC(VP_CYAN,VP_PURPLE,cyc):cyc<2?LerpC(VP_PURPLE,VP_PINK,cyc-1):LerpC(VP_PINK,VP_CYAN,cyc-2);
        // trail — 3 layers only
        DrawLineEx({sp.x-nx*28,sp.y-ny*28},sp,8.f,ColorAlpha(c2,.05f));
        DrawLineEx({sp.x-nx*14,sp.y-ny*14},sp,4.5f,ColorAlpha(c1,.22f));
        DrawLineEx({sp.x-nx*5, sp.y-ny*5 },sp,2.0f,ColorAlpha(VP_WHITE,.60f));
        // 3 spinning orbit dots
        float spin=t*10.f+b.pos.x*.025f;
        for(int i=0;i<3;i++){
            float pa=spin+i*(2*PI/3.f);
            float pr=b.glowR+6.f;
            Color sc2=(i==0)?c1:(i==1)?c2:VP_WHITE;
            DrawLineEx(sp,{sp.x+cosf(pa)*pr,sp.y+sinf(pa)*pr},1.0f,ColorAlpha(sc2,.35f));
            DrawCircleV({sp.x+cosf(pa)*pr,sp.y+sinf(pa)*pr},2.2f,ColorAlpha(sc2,.88f));
        }
        // body
        DrawNeonCircle(sp,b.glowR+2,c1,.55f);
        DrawCircleV(sp,5.0f,ColorAlpha(c1,.90f));
        DrawCircleV(sp,2.8f,ColorAlpha(VP_WHITE,.95f));
        DrawCircleV(sp,1.2f,VP_WHITE);
        DrawNeonCircle(sp,b.glowR+9,VP_HOTPINK,.15f+.08f*sinf(t*6));

    } else if(isSolar){
        // ── SOLAR FLARE BOMB ── (~18 draw calls)
        float age=1.f-(b.burstTimer/90.f);
        float pulse=sinf(t*9.f+b.pos.x*.03f);
        float fuseA=(b.burstTimer/90.f);
        float fuseFlicker=(fuseA<0.25f)?(0.5f+0.5f*sinf(t*35.f)):1.f;
        float fuseR=12.f*fuseA+4.f;
        // outer haze — 1 ring
        DrawCircleV(sp,16.f+pulse*3.f,ColorAlpha(VP_ORANGE,.05f+age*.02f));
        // fuse ring — 2 rings
        DrawNeonCircle(sp,fuseR,VP_YELLOW,(.65f+.20f*pulse)*fuseFlicker);
        DrawNeonCircle(sp,fuseR*.72f,VP_ORANGE,(.40f+.15f*pulse)*fuseFlicker);
        // sun body — 4 layers
        DrawCircleV(sp,9.0f+pulse*.7f, ColorAlpha(VP_RED,.70f));
        DrawCircleV(sp,7.0f+pulse*.6f, ColorAlpha(VP_ORANGE,.80f));
        DrawCircleV(sp,4.5f+pulse*.4f, ColorAlpha(VP_YELLOW,.92f));
        DrawCircleV(sp,1.5f,           ColorAlpha(VP_WHITE,1.f));
        // corona flares — 6 rays (halved)
        float spinSpd=2.5f+age*9.f;
        for(int i=0;i<6;i++){
            float fa=t*spinSpd+i*(2*PI/6.f);
            float fLen=(i%2==0?10.f:6.5f)+age*3.f;
            Color fc=i%2==0?VP_RED:VP_ORANGE;
            DrawLineEx(sp,{sp.x+cosf(fa)*fLen,sp.y+sinf(fa)*fLen},
                       i%2==0?1.6f:1.0f,ColorAlpha(fc,(.38f)*fuseFlicker));
        }
        DrawNeonCircle(sp,10.5f+pulse*1.5f,VP_RED,.18f*fuseFlicker);

    } else if(isLance){
        // ── VOID LANCE ── (~14 draw calls)
        float streakLen=28.f+spd*.038f;
        float crysRot=t*3.5f+b.pos.x*.04f;
        // speed cone — 3 layers
        DrawLineEx({sp.x-nx*streakLen*1.4f,sp.y-ny*streakLen*1.4f},sp,5.5f,ColorAlpha(VP_CYAN,.07f));
        DrawLineEx({sp.x-nx*streakLen,sp.y-ny*streakLen},sp,2.5f,ColorAlpha(VP_CYAN,.28f));
        DrawLineEx({sp.x-nx*streakLen*.5f,sp.y-ny*streakLen*.5f},sp,1.2f,ColorAlpha(VP_WHITE,.72f));
        // lance body
        DrawCircleV(sp,6.5f,ColorAlpha(VP_CYAN,.60f));
        DrawCircleV(sp,3.5f,ColorAlpha(VP_LAVENDER,.85f));
        DrawCircleV(sp,1.4f,VP_WHITE);
        DrawNeonCircle(sp,6.5f,VP_CYAN,.70f);
        // 4-point crystal shards
        for(int i=0;i<4;i++){
            float ca2=crysRot+i*(PI*.5f);
            float cLen=7.5f+sinf(t*8+i)*1.f;
            Color sc=(i%2==0)?VP_CYAN:VP_LAVENDER;
            DrawLineEx(sp,{sp.x+cosf(ca2)*cLen,sp.y+sinf(ca2)*cLen},1.0f,ColorAlpha(sc,.72f));
            DrawCircleV({sp.x+cosf(ca2)*cLen,sp.y+sinf(ca2)*cLen},1.8f,ColorAlpha(sc,.88f));
        }

    } else if(isSwarm){
        // ── SOUL SWARM ── (~16 draw calls)
        float glow=0.55f+0.45f*sinf(t*7.f+b.pos.x*.022f+b.pos.y*.018f);
        float age2=1.f-(b.life/160.f);
        // tail — 3 layers
        DrawLineEx({sp.x-nx*16,sp.y-ny*16},sp,6.0f,ColorAlpha(b.col,.06f));
        DrawLineEx({sp.x-nx*8, sp.y-ny*8 },sp,3.0f,ColorAlpha(b.col,.22f));
        DrawLineEx({sp.x-nx*3, sp.y-ny*3 },sp,1.2f,ColorAlpha(VP_WHITE,.55f));
        // soul thread — 3 dots (halved)
        for(int ti=0;ti<3;ti++){
            float ta=t*6.f+ti*(2*PI/3.f)+(age2*3.f);
            float tr=b.glowR+2.f;
            Vector2 tpos={sp.x+cosf(ta)*tr,sp.y+sinf(ta)*tr};
            DrawCircleV(tpos,1.8f,ColorAlpha(b.col,.70f+.20f*glow));
        }
        // wisp body
        DrawCircleV(sp,b.glowR+4.f,ColorAlpha(b.col,.14f+.07f*glow));
        DrawNeonCircle(sp,b.glowR,b.col,.55f+.28f*glow);
        DrawCircleV(sp,3.5f,ColorAlpha(VP_LAVENDER,.75f));
        DrawCircleV(sp,2.0f,ColorAlpha(b.col,.95f));
        DrawCircleV(sp,1.0f,VP_WHITE);
        // 2 orbiting micro-souls
        for(int i=0;i<2;i++){
            float wa=t*9.f+i*(PI)+ age2*5.f;
            float wr=8.f-age2*3.5f;
            Vector2 wpos={sp.x+cosf(wa)*wr,sp.y+sinf(wa)*wr};
            DrawCircleV(wpos,2.5f,ColorAlpha(b.col,.70f));
            DrawCircleV(wpos,1.0f,ColorAlpha(VP_WHITE,.90f));
        }

    } else {
        // ── DEFAULT / EXPLOSIVE / PIERCING → dispatched by shipType ──
        float streakLen=14.f+spd*.032f;

        if(b.shipType==0){
            // ════════════════════════════════════════════════════
            // INTERCEPTOR — twin-linked railgun bolt
            // Thin electric needle with crackling arc forks along the trail
            // ════════════════════════════════════════════════════
            float needleLen = streakLen * 2.2f;

            // Outer velocity haze
            DrawLineEx({sp.x-nx*needleLen*1.6f,sp.y-ny*needleLen*1.6f},sp,
                       4.5f,ColorAlpha(b.col,.06f));
            // Mid glow cone
            DrawLineEx({sp.x-nx*needleLen,sp.y-ny*needleLen},sp,
                       2.2f,ColorAlpha(b.col,.30f));
            // Bright inner core
            DrawLineEx({sp.x-nx*needleLen*.45f,sp.y-ny*needleLen*.45f},sp,
                       1.0f,ColorAlpha(VP_WHITE,.85f));

            // Electric arc forks — 3 random-ish forks branching off the trail
            float forkSeed=b.pos.x*0.031f+b.pos.y*0.017f;
            for(int fi=0;fi<3;fi++){
                float ft  = 0.25f + fi*0.22f; // position along trail (0=tip, 1=back)
                float fx  = sp.x - nx*needleLen*ft;
                float fy  = sp.y - ny*needleLen*ft;
                // Fork branches ±perpendicular
                float forkLen = 5.f + sinf(forkSeed*7.3f+fi*2.1f+t*15.f)*3.f;
                float forkAng = (fi%2==0 ? 1.f:-1.f) * (0.45f + sinf(t*18.f+fi)*0.15f);
                float fa = forkAng;
                float fcx = cosf(fa)*px - sinf(fa)*py; // rotated perp
                float fcy = sinf(fa)*px + cosf(fa)*py;
                float tipX = fx + fcx*forkLen;
                float tipY = fy + fcy*forkLen;
                DrawLineEx({fx,fy},{tipX,tipY},0.8f,ColorAlpha(b.col,(0.55f - fi*0.12f)));
                DrawCircleV({tipX,tipY},1.0f,ColorAlpha(VP_WHITE,.70f));
                // secondary micro-fork off tip
                if(fi==1){
                    float mfx=tipX+cosf(fa+0.6f)*px*forkLen*.5f - sinf(fa+0.6f)*py*forkLen*.5f;
                    float mfy=tipY+cosf(fa+0.6f)*py*forkLen*.5f + sinf(fa+0.6f)*px*forkLen*.5f;
                    DrawLineEx({tipX,tipY},{mfx,mfy},0.5f,ColorAlpha(VP_WHITE,.35f));
                }
            }

            // Tip: tight neon capsule — bright core dot + glow ring
            DrawNeonCircle(sp, 4.5f, b.col, .80f);
            DrawCircleV(sp, 3.0f, ColorAlpha(VP_WHITE,.65f));
            DrawCircleV(sp, 1.5f, ColorAlpha(b.col,.95f));
            DrawCircleV(sp, 0.8f, VP_WHITE);

        } else if(b.shipType==1){
            // ════════════════════════════════════════════════════
            // BRAWLER — heavy siege shell
            // Big slow cannonball: thick casing, rifling spin,
            // concussive pressure wave rings radiating outward
            // ════════════════════════════════════════════════════
            float shellRot = t * 4.5f + b.pos.x * .025f + b.pos.y * .018f;
            float shellAge = 1.f - (b.life / 105.f); // 0=just fired 1=old
            float pulse    = 0.8f + 0.2f*sinf(t*9.f + b.pos.x*.03f);

            // Thick shell wake — heat distortion trail
            DrawLineEx({sp.x-nx*streakLen*1.2f,sp.y-ny*streakLen*1.2f},sp,
                       9.f, ColorAlpha(b.col,.06f));
            DrawLineEx({sp.x-nx*streakLen*.8f,sp.y-ny*streakLen*.8f},sp,
                       5.5f, ColorAlpha(b.col,.15f));
            DrawLineEx({sp.x-nx*streakLen*.35f,sp.y-ny*streakLen*.35f},sp,
                       2.5f, ColorAlpha(VP_WHITE,.30f));

            // Pressure rings — 2 expanding rings behind the slug as it punches air
            for(int ri=0;ri<2;ri++){
                float rAge  = fmodf(shellAge*3.f + ri*0.5f, 1.f);
                float rDist = 8.f + rAge*22.f; // expands behind shot
                Vector2 rCenter={sp.x - nx*rDist, sp.y - ny*rDist};
                float rRadius = 4.f + rAge*10.f;
                DrawCircleLinesV(rCenter, rRadius,
                    ColorAlpha(b.col, (1.f-rAge)*0.30f));
            }

            // Shell casing: thick outer glow
            DrawCircleV(sp, 12.f + pulse*.8f, ColorAlpha(b.col,.08f));
            DrawNeonCircle(sp, 10.0f + pulse*.5f, b.col, .38f*pulse);

            // Rifling grooves — 4 spinning marks on the shell
            for(int ri=0;ri<4;ri++){
                float ra = shellRot + ri*(PI*.5f);
                // Groove: arc from inner to outer radius
                Vector2 g1 = {sp.x+cosf(ra)*5.f,           sp.y+sinf(ra)*5.f};
                Vector2 g2 = {sp.x+cosf(ra+0.5f)*8.5f,     sp.y+sinf(ra+0.5f)*8.5f};
                DrawLineEx(g1, g2, 1.4f, ColorAlpha(b.col, .75f));
                DrawCircleV(g2, 1.2f, ColorAlpha({255,210,100,255}, .60f));
            }

            // Shell body layers
            DrawCircleV(sp, 8.0f, ColorAlpha(b.col,.50f));
            DrawCircleV(sp, 6.0f, ColorAlpha({255,200,90,255},.72f));
            DrawCircleV(sp, 3.5f, ColorAlpha(VP_YELLOW,.90f));
            DrawCircleV(sp, 1.5f, VP_WHITE);

            // Nose flare — bright leading spike
            DrawLineEx(sp, {sp.x+nx*8.f, sp.y+ny*8.f}, 2.5f,
                ColorAlpha(VP_WHITE, .70f));
            DrawCircleV({sp.x+nx*8.f, sp.y+ny*8.f}, 2.0f,
                ColorAlpha({255,240,180,255},.85f));

        } else if(b.shipType==2){
            // ════════════════════════════════════════════════════
            // PHANTOM — phase dagger
            // Semi-transparent blade that flickers in/out of phase;
            // leaves 3 ghost afterimage copies fading behind it
            // ════════════════════════════════════════════════════
            float phase   = fmodf(t*22.f + b.pos.x*.045f + b.pos.y*.033f, 2.f*PI);
            float flick   = 0.55f + 0.45f*sinf(phase); // visibility pulse
            float ghostFade = 1.f - (b.life / (float)(b.burstTimer > 0 ? (int)b.burstTimer : 100));

            // Ghost afterimages — 3 copies trailing behind
            for(int gi=0;gi<3;gi++){
                float gDist = (gi+1) * (streakLen * .55f);
                float gFade = flick * (0.28f - gi*0.07f) * (1.f - ghostFade*0.5f);
                if(gFade <= 0.f) continue;
                Vector2 gp  = {sp.x - nx*gDist, sp.y - ny*gDist};
                // Ghost dagger silhouette
                DrawLineEx({gp.x-nx*5.f,gp.y-ny*5.f},{gp.x+nx*5.f,gp.y+ny*5.f},
                           1.8f - gi*0.4f, ColorAlpha(b.col, gFade));
                // Perpendicular ghost cross
                DrawLineEx({gp.x-px*3.f,gp.y-py*3.f},{gp.x+px*3.f,gp.y+py*3.f},
                           0.8f, ColorAlpha(b.col, gFade*.5f));
                DrawCircleV(gp, 1.5f - gi*.3f, ColorAlpha(VP_WHITE, gFade*.7f));
            }

            // Long ghostly trail
            DrawLineEx({sp.x-nx*streakLen*2.8f,sp.y-ny*streakLen*2.8f},sp,
                       2.5f, ColorAlpha(b.col,.04f*flick));
            DrawLineEx({sp.x-nx*streakLen*1.5f,sp.y-ny*streakLen*1.5f},sp,
                       1.5f, ColorAlpha(b.col,.18f*flick));
            DrawLineEx({sp.x-nx*streakLen*.5f,sp.y-ny*streakLen*.5f},sp,
                       .8f,  ColorAlpha(VP_WHITE,.60f*flick));

            // Phase shimmer — thin blade shape along travel axis
            DrawLineEx({sp.x-nx*7.f,sp.y-ny*7.f},{sp.x+nx*10.f,sp.y+ny*10.f},
                       3.5f, ColorAlpha(b.col,.40f*flick));
            DrawLineEx({sp.x-nx*5.f,sp.y-ny*5.f},{sp.x+nx*8.f,sp.y+ny*8.f},
                       1.5f, ColorAlpha(VP_WHITE,.70f*flick));

            // Blade tip cross (dagger guard)
            float guardW = 5.f*flick;
            DrawLineEx({sp.x+px*guardW, sp.y+py*guardW},
                       {sp.x-px*guardW, sp.y-py*guardW},
                       1.2f, ColorAlpha(b.col,.55f*flick));

            // Core dot — flickers more aggressively
            DrawNeonCircle(sp, 3.5f, b.col, .70f*flick);
            DrawCircleV(sp, 2.5f, ColorAlpha(VP_WHITE,.75f*flick));
            DrawCircleV(sp, 1.2f, ColorAlpha(b.col,.95f));
            // Phase blink: random hard-cut visibility
            if(sinf(phase*3.7f) > 0.6f)
                DrawCircleV(sp, 4.0f, ColorAlpha(VP_WHITE,.25f));

        } else {
            // ════════════════════════════════════════════════════
            // TITAN — gravity slug
            // Massive slow orb; 3 orbital distortion rings revolve
            // around it as it warps space on its path
            // ════════════════════════════════════════════════════
            float gravRot  = t * 2.8f + b.pos.x * .022f;
            float gravPulse= 0.75f + 0.25f*sinf(t*4.5f + b.pos.y*.02f);
            float massAge  = 1.f - (b.life / (float)(b.burstTimer > 0 ? (int)b.burstTimer : 110));

            // Space-warp distortion field — large soft glow
            DrawCircleV(sp, 22.f, ColorAlpha(b.col,.04f));
            DrawCircleV(sp, 16.f, ColorAlpha(b.col,.07f));

            // Heavy wake — wide compressed air trail
            DrawLineEx({sp.x-nx*streakLen*1.1f,sp.y-ny*streakLen*1.1f},sp,
                       10.f, ColorAlpha(b.col,.08f));
            DrawLineEx({sp.x-nx*streakLen*.7f,sp.y-ny*streakLen*.7f},sp,
                       5.f, ColorAlpha(b.col,.18f));
            DrawLineEx({sp.x-nx*streakLen*.3f,sp.y-ny*streakLen*.3f},sp,
                       2.f, ColorAlpha(VP_WHITE,.35f));

            // 3 orbital gravity rings — tilted on different axes
            for(int ri=0;ri<3;ri++){
                float ringAng  = gravRot + ri*(2.f*PI/3.f);
                float ringTilt = 0.30f + ri*0.22f; // orbit tilt (ellipse compression)
                float ringR    = 11.5f + ri*1.8f;
                int   segs     = 20;
                for(int si2=0;si2<segs;si2++){
                    float a1 = ringAng + si2*(2.f*PI/segs);
                    float a2 = ringAng + (si2+1)*(2.f*PI/segs);
                    float x1 = sp.x + cosf(a1)*ringR;
                    float y1 = sp.y + sinf(a1)*ringR*ringTilt;
                    float x2 = sp.x + cosf(a2)*ringR;
                    float y2 = sp.y + sinf(a2)*ringR*ringTilt;
                    // Brightest at the front of each orbit
                    float brightness = 0.25f + 0.45f*fabsf(sinf(a1));
                    DrawLineEx({x1,y1},{x2,y2}, 1.2f,
                        ColorAlpha(ri==0?b.col:ri==1?VP_WHITE:ACCENT_GOLD,
                                   brightness * gravPulse * 0.75f));
                }
                // Orbital node (bright dot at periapsis)
                float nodeX = sp.x + cosf(ringAng)*ringR;
                float nodeY = sp.y + sinf(ringAng)*ringR*ringTilt;
                DrawCircleV({nodeX,nodeY}, 2.2f,
                    ColorAlpha(ri==1?VP_WHITE:b.col, .90f*gravPulse));
            }

            // Slug body — layered dense core
            DrawNeonCircle(sp, 9.5f*gravPulse, b.col, .50f);
            DrawCircleV(sp, 8.5f, ColorAlpha(b.col,.55f));
            DrawCircleV(sp, 6.5f, ColorAlpha(ACCENT_GOLD,.68f));
            DrawCircleV(sp, 4.2f, ColorAlpha({255,240,200,255},.85f));
            DrawCircleV(sp, 2.2f, ColorAlpha(VP_WHITE,.95f));
            DrawCircleV(sp, 1.0f, VP_WHITE);

            // Impact proximity warning — grows as slug ages
            if(massAge > 0.65f){
                float warningA = (massAge - 0.65f) / 0.35f;
                DrawCircleLinesV(sp, 13.f + warningA*4.f,
                    ColorAlpha(b.col, warningA * .40f));
            }
        }
    }
}

void DrawPickup(Pickup& pk,Vector2 sp,float t){
    float pulse=.65f+.35f*sinf(t*5.5f+pk.spin);
    float alpha=std::min(1.f,(float)pk.life/40.f);
    float r=7.f*pulse;
    float aa=pk.spin+t*1.8f;

    // ── ground shadow ──
    DrawEllipse((int)sp.x,(int)sp.y+9,(int)(r*.85f),(int)(r*.25f),ColorAlpha({0,0,0,255},alpha*.28f));

    // ── outer glow haze (3 layers) ──
    DrawCircleV(sp,r+10,ColorAlpha(VP_GOLD,alpha*.04f));
    DrawCircleV(sp,r+6, ColorAlpha(VP_GOLD,alpha*.08f));
    DrawCircleV(sp,r+2, ColorAlpha(VP_GOLD,alpha*.14f));

    // ── 8 animated light-ray spokes ──
    for(int i=0;i<8;i++){
        float ra=aa+i*(2*PI/8);
        float rLen=(r+8)+3.f*sinf(t*4+i*1.2f);
        float rA=alpha*(0.18f+0.12f*sinf(t*3.5f+i));
        DrawLineEx(sp,{sp.x+cosf(ra)*rLen,sp.y+sinf(ra)*rLen},
                   0.8f,ColorAlpha(VP_YELLOW,rA));
    }

    // ── gem body: 8-facet diamond ──
    // top crown
    Vector2 top={sp.x,sp.y-r};
    // girdle: 8 vertices around widest point
    float girdleY=sp.y-r*.12f;
    float girdleR=r*.92f;
    Vector2 gird[8];
    for(int i=0;i<8;i++){
        float ga=aa+i*(2*PI/8);
        gird[i]={sp.x+cosf(ga)*girdleR,girdleY+sinf(ga)*r*.22f};
    }
    // pavilion bottom
    Vector2 bot={sp.x,sp.y+r*.82f};
    Vector2 botMid={sp.x,sp.y+r*.42f};

    // top facets (crown to girdle)
    for(int i=0;i<8;i++){
        float fi=(float)i/8.f;
        Color fc=i%2==0 ? ColorAlpha(VP_GOLD,alpha*(0.30f+fi*.10f))
                        : ColorAlpha(VP_ORANGE,alpha*(0.18f+fi*.08f));
        DrawTriangle(top,gird[i],gird[(i+1)%8],fc);
    }
    // lower facets (girdle to pavilion)
    for(int i=0;i<8;i++){
        float fi=(float)i/8.f;
        Color fc=i%2==0 ? ColorAlpha(VP_ORANGE,alpha*(0.22f+fi*.08f))
                        : ColorAlpha(VP_YELLOW,alpha*(0.14f+fi*.06f));
        DrawTriangle(gird[i],bot,gird[(i+1)%8],fc);
    }

    // ── gem edges ──
    // crown edges
    for(int i=0;i<8;i++){
        DrawNeonLine(top,gird[i],VP_YELLOW,.9f,alpha*.80f);
        DrawNeonLine(gird[i],gird[(i+1)%8],VP_GOLD,.7f,alpha*.60f);
    }
    // pavilion edges
    for(int i=0;i<8;i++)
        DrawNeonLine(gird[i],bot,VP_ORANGE,.8f,alpha*.65f);

    // ── inner table facet ──
    float tableR=r*.42f;
    Vector2 table[4];
    for(int i=0;i<4;i++){
        float ta=aa+PI*.25f+i*(PI*.5f);
        table[i]={sp.x+cosf(ta)*tableR,sp.y-r*.22f+sinf(ta)*tableR*.4f};
    }
    DrawTriangle(table[0],table[1],table[2],ColorAlpha(VP_WHITE,alpha*.12f));
    DrawTriangle(table[2],table[3],table[0],ColorAlpha(VP_WHITE,alpha*.12f));
    for(int i=0;i<4;i++) DrawNeonLine(table[i],table[(i+1)%4],VP_WHITE,.7f,alpha*.55f);

    // ── specular glint (top-left, moves with pulse) ──
    DrawCircleV({top.x-1.5f,top.y+2.5f},2.2f*pulse,ColorAlpha(VP_WHITE,alpha*.85f));
    DrawCircleV({top.x-1.5f,top.y+2.5f},1.0f,       ColorAlpha(VP_WHITE,alpha));

    // ── center sparkle ──
    DrawCircleV(sp,r*.18f,ColorAlpha(VP_WHITE,alpha*pulse*.75f));
}


// ══════════════════════════════════════════════════════════════
//  HUD PANELS
// ══════════════════════════════════════════════════════════════


// ══════════════════════════════════════════════════════════════
//  ENEMY & BOSS RENDERING
// ══════════════════════════════════════════════════════════════

void DrawEnemy(Enemy& e, Vector2 sp, float t){
    if(!e.active) return;
    float r    = e.radius;
    // Face direction of travel; fallback to stored angle when stationary
    float spd2 = sqrtf(e.vel.x*e.vel.x + e.vel.y*e.vel.y);
    float dir  = (spd2 > 2.f) ? atan2f(e.vel.y, e.vel.x) : e.angle;
    float ph   = e.angle; // used for animation phase offsets only
    float slow = 0.5f + 0.5f*sinf(t*2.5f + ph);
    float fast = 0.5f + 0.5f*sinf(t*7.f  + ph);
    float hp01 = (e.maxHp > 0) ? (float)e.hp / e.maxHp : 1.f;
    Color c    = e.col;
    if(e.flashTimer > 0) c = LerpC(c, VP_WHITE, (float)e.flashTimer / 8.f);
    // Basis vectors: fwd = nose direction, rgt = right perpendicular
    float fca = cosf(dir), fsa = sinf(dir);
    float rca = -fsa,      rsa = fca;

    #define FWD(f,s) Vector2{sp.x + fca*(f) + rca*(s), sp.y + fsa*(f) + rsa*(s)}
    switch(e.type){

    case E_BASIC:{
        // ── GRUB FIGHTER — insectoid alien assault craft ──
        // Bulbous abdomen-fuselage, swept mandible wings, compound-eye cockpit. Faces travel direction.
        float pulse = 0.5f + 0.5f*sinf(t*5.f + ph);
        Vector2 nose   = FWD( r*0.78f,  0);
        Vector2 ckL    = FWD( r*0.38f,  r*0.20f);
        Vector2 ckR    = FWD( r*0.38f, -r*0.20f);
        Vector2 midsL  = FWD( r*0.05f,  r*0.34f);
        Vector2 midsR  = FWD( r*0.05f, -r*0.34f);
        Vector2 mwTipL = FWD( r*0.55f,  r*1.00f);
        Vector2 mwTipR = FWD( r*0.55f, -r*1.00f);
        Vector2 mwRtL  = FWD( r*0.10f,  r*0.58f);
        Vector2 mwRtR  = FWD( r*0.10f, -r*0.58f);
        Vector2 aftL   = FWD(-r*0.50f,  r*0.28f);
        Vector2 aftR   = FWD(-r*0.50f, -r*0.28f);
        Vector2 tail   = FWD(-r*0.82f,  0);
        DrawTriangle(nose, ckL, ckR,     ColorAlpha(c, 0.42f));
        DrawTriangle(ckL,  midsL, midsR, ColorAlpha(c, 0.32f));
        DrawTriangle(ckL,  midsR, ckR,   ColorAlpha(c, 0.32f));
        DrawTriangle(midsL, aftL, aftR,  ColorAlpha(c, 0.22f));
        DrawTriangle(midsL, aftR, midsR, ColorAlpha(c, 0.22f));
        DrawTriangle(aftL,  tail, aftR,  ColorAlpha(c, 0.16f));
        DrawTriangle(mwRtL, mwTipL, midsL, ColorAlpha(c, 0.20f));
        DrawTriangle(mwRtR, midsR, mwTipR, ColorAlpha(c, 0.20f));
        DrawNeonLine(nose, ckL,   c, 2.4f, 0.93f);
        DrawNeonLine(nose, ckR,   c, 2.4f, 0.93f);
        DrawNeonLine(ckL,  midsL, c, 1.8f, 0.80f);
        DrawNeonLine(ckR,  midsR, c, 1.8f, 0.80f);
        DrawNeonLine(midsL, aftL, c, 1.5f, 0.65f);
        DrawNeonLine(midsR, aftR, c, 1.5f, 0.65f);
        DrawNeonLine(aftL,  tail, c, 1.2f, 0.55f);
        DrawNeonLine(aftR,  tail, c, 1.2f, 0.55f);
        DrawNeonLine(mwRtL, mwTipL, c, 2.0f, 0.88f);
        DrawNeonLine(mwTipL, midsL, c, 1.5f, 0.70f);
        DrawNeonLine(mwRtR, mwTipR, c, 2.0f, 0.88f);
        DrawNeonLine(mwTipR, midsR, c, 1.5f, 0.70f);
        DrawLineEx(nose, tail, 0.7f, ColorAlpha(c, 0.22f));
        Vector2 ck = FWD(r*0.45f, 0);
        DrawCircleV(ck, r*0.20f, ColorAlpha(c, 0.45f));
        DrawNeonCircle(ck, r*0.19f, c, 0.90f);
        for(int fi=0;fi<3;fi++){
            float fa = dir + (fi-1)*(PI/5.f);
            Vector2 lens = {ck.x+cosf(fa)*r*0.08f, ck.y+sinf(fa)*r*0.08f};
            DrawCircleV(lens, r*0.07f, ColorAlpha(VP_WHITE, 0.62f + fast*0.25f));
            DrawCircleV(lens, r*0.035f, ColorAlpha(c, 0.80f));
        }
        DrawNeonCircle(mwTipL, r*0.11f, c, 0.84f + pulse*0.14f);
        DrawCircleV(mwTipL, r*0.055f, ColorAlpha(VP_WHITE, 0.75f));
        DrawNeonCircle(mwTipR, r*0.11f, c, 0.84f + pulse*0.14f);
        DrawCircleV(mwTipR, r*0.055f, ColorAlpha(VP_WHITE, 0.75f));
        for(int s=-1;s<=1;s+=2){
            Vector2 eng = FWD(-r*0.74f, s*r*0.14f);
            float ef = 0.5f + 0.5f*sinf(t*9.f + s*0.8f + ph);
            DrawNeonCircle(eng, r*0.11f, c, 0.72f + ef*0.20f);
            DrawCircleV(eng, r*0.055f, ColorAlpha(VP_WHITE, 0.72f));
        }
        DrawNeonCircle(sp, r*1.02f, c, 0.12f);
        break;
    }

    case E_FAST:{
        // ── RAZORFIN INTERCEPTOR — sleek alien dart-fighter, extra-long needle fuselage ──
        float pulse = 0.5f + 0.5f*sinf(t*9.f + ph);
        float bLen  = r*1.55f;
        Vector2 probeTip = FWD(bLen + r*0.32f, 0);
        Vector2 nose     = FWD(bLen, 0);
        Vector2 fuseFL   = FWD(r*0.60f,  r*0.13f);
        Vector2 fuseFR   = FWD(r*0.60f, -r*0.13f);
        Vector2 finTipL  = FWD(-r*0.05f,  r*0.90f);
        Vector2 finTipR  = FWD(-r*0.05f, -r*0.90f);
        Vector2 finRtL   = FWD( r*0.25f,  r*0.32f);
        Vector2 finRtR   = FWD( r*0.25f, -r*0.32f);
        Vector2 aftL     = FWD(-r*0.65f,  r*0.15f);
        Vector2 aftR     = FWD(-r*0.65f, -r*0.15f);
        Vector2 tail     = FWD(-bLen*0.90f, 0);
        Vector2 stabTipL = FWD(-r*0.55f,  r*0.32f);
        Vector2 stabTipR = FWD(-r*0.55f, -r*0.32f);
        DrawTriangle(nose, fuseFL, fuseFR, ColorAlpha(c, 0.38f));
        DrawTriangle(fuseFL, aftL, aftR,   ColorAlpha(c, 0.25f));
        DrawTriangle(fuseFL, aftR, fuseFR, ColorAlpha(c, 0.25f));
        DrawTriangle(aftL, tail, aftR,     ColorAlpha(c, 0.18f));
        DrawTriangle(finRtL, finTipL, aftL, ColorAlpha(c, 0.18f));
        DrawTriangle(finRtR, aftR, finTipR, ColorAlpha(c, 0.18f));
        DrawNeonLine(nose, fuseFL,    c, 2.0f, 0.90f);
        DrawNeonLine(nose, fuseFR,    c, 2.0f, 0.90f);
        DrawNeonLine(fuseFL, aftL,    c, 1.5f, 0.72f);
        DrawNeonLine(fuseFR, aftR,    c, 1.5f, 0.72f);
        DrawNeonLine(aftL, tail,      c, 1.2f, 0.58f);
        DrawNeonLine(aftR, tail,      c, 1.2f, 0.58f);
        DrawNeonLine(finRtL, finTipL, c, 1.6f, 0.82f);
        DrawNeonLine(finTipL, aftL,   c, 1.2f, 0.60f);
        DrawNeonLine(finRtR, finTipR, c, 1.6f, 0.82f);
        DrawNeonLine(finTipR, aftR,   c, 1.2f, 0.60f);
        DrawNeonLine(aftL, stabTipL,  c, 1.0f, 0.52f);
        DrawNeonLine(aftR, stabTipR,  c, 1.0f, 0.52f);
        DrawLineEx(nose, tail, 0.5f, ColorAlpha(c, 0.20f));
        DrawLineEx(nose, probeTip, r*0.07f, ColorAlpha(c, 0.70f));
        DrawNeonLine(nose, probeTip, VP_WHITE, 0.9f, 0.52f);
        DrawNeonCircle(probeTip, r*0.08f, VP_WHITE, 0.78f + pulse*0.20f);
        DrawCircleV(probeTip, r*0.04f, VP_WHITE);
        Vector2 ck = FWD(r*0.72f, 0);
        DrawNeonCircle(ck, r*0.14f, c, 0.72f);
        DrawCircleV(ck, r*0.07f, ColorAlpha(VP_WHITE, 0.80f));
        DrawLineEx(FWD(r*0.72f,-r*0.11f), FWD(r*0.72f, r*0.11f), 1.2f, ColorAlpha(VP_WHITE, 0.55f));
        for(int s=-1;s<=1;s+=2){
            Vector2 eng = FWD(-bLen*0.88f, s*r*0.08f);
            float ef = 0.5f + 0.5f*sinf(t*12.f + s + ph);
            DrawNeonCircle(eng, r*0.10f, c, 0.82f + ef*0.16f);
            DrawCircleV(eng, r*0.05f, ColorAlpha(VP_WHITE, 0.80f));
        }
        DrawLineEx(tail, FWD(-bLen*1.35f, 0), r*0.10f, ColorAlpha(c, 0.18f + pulse*0.08f));
        break;
    }

    case E_TANK:{
        // ── WARLORD CRUISER — massive alien heavy assault platform ──
        float pulse = 0.5f + 0.5f*sinf(t*3.f + ph);
        Vector2 noseTip = FWD( r*0.85f,  0);
        Vector2 noseL   = FWD( r*0.55f,  r*0.62f);
        Vector2 noseR   = FWD( r*0.55f, -r*0.62f);
        Vector2 midL    = FWD( r*0.05f,  r*0.90f);
        Vector2 midR    = FWD( r*0.05f, -r*0.90f);
        Vector2 aftL    = FWD(-r*0.60f,  r*0.74f);
        Vector2 aftR    = FWD(-r*0.60f, -r*0.74f);
        Vector2 tailL   = FWD(-r*0.92f,  r*0.34f);
        Vector2 tailR   = FWD(-r*0.92f, -r*0.34f);
        DrawTriangle(noseTip, noseL, noseR, ColorAlpha(c, 0.42f));
        DrawTriangle(noseL, midL, midR,     ColorAlpha(c, 0.32f));
        DrawTriangle(noseL, midR, noseR,    ColorAlpha(c, 0.32f));
        DrawTriangle(midL, aftL, aftR,      ColorAlpha(c, 0.26f));
        DrawTriangle(midL, aftR, midR,      ColorAlpha(c, 0.26f));
        DrawTriangle(aftL, tailL, tailR,    ColorAlpha(c, 0.20f));
        DrawTriangle(aftL, tailR, aftR,     ColorAlpha(c, 0.20f));
        // HP-reactive plating
        auto pc = [&](float s) -> Color { return s<hp01 ? c : VP_DARKRED; };
        auto pa = [&](float s) -> float { return s<hp01 ? 0.90f : 0.28f; };
        DrawNeonLine(noseTip, noseL, pc(0.9f), 3.0f, pa(0.9f));
        DrawNeonLine(noseTip, noseR, pc(0.9f), 3.0f, pa(0.9f));
        DrawNeonLine(noseL, midL,    pc(0.7f), 2.4f, pa(0.7f));
        DrawNeonLine(noseR, midR,    pc(0.7f), 2.4f, pa(0.7f));
        DrawNeonLine(midL,  aftL,    pc(0.5f), 2.2f, pa(0.5f));
        DrawNeonLine(midR,  aftR,    pc(0.5f), 2.2f, pa(0.5f));
        DrawNeonLine(aftL, tailL,    pc(0.3f), 2.0f, pa(0.3f));
        DrawNeonLine(aftR, tailR,    pc(0.3f), 2.0f, pa(0.3f));
        DrawNeonLine(tailL, tailR,   pc(0.1f), 1.8f, pa(0.1f));
        DrawNeonLine(noseL, noseR, c, 1.0f, 0.30f);
        DrawNeonLine(midL,  midR,  c, 1.0f, 0.28f);
        DrawNeonLine(aftL,  aftR,  c, 0.8f, 0.22f);
        for(int i=0;i<5;i++){
            float ff=0.6f-i*0.25f, rw=0.55f+i*0.08f;
            DrawCircleV(FWD(r*ff,  r*rw*0.72f), r*0.040f, ColorAlpha(c, 0.55f+slow*0.20f));
            DrawCircleV(FWD(r*ff, -r*rw*0.72f), r*0.040f, ColorAlpha(c, 0.55f+slow*0.20f));
        }
        // Forward dual cannon
        for(int s=-1;s<=1;s+=2){
            Vector2 bBase = FWD(r*0.70f, s*r*0.10f);
            Vector2 bTip  = FWD(r*1.28f, s*r*0.10f);
            DrawLineEx(bBase, bTip, r*0.14f, ColorAlpha(c, 0.70f));
            DrawNeonLine(bBase, bTip, c, 2.0f, 0.88f);
            DrawNeonCircle(bTip, r*0.11f, c, 0.92f);
            DrawCircleV(bTip, r*0.055f, ColorAlpha(VP_WHITE, 0.78f));
        }
        Vector2 canBase = FWD(r*0.72f, 0);
        DrawCircleV(canBase, r*0.24f, ColorAlpha(c, 0.48f));
        DrawNeonCircle(canBase, r*0.24f, c, 0.82f);
        // 4 shoulder turrets
        float tPos[4][2] = {{0.25f,0.85f},{0.25f,-0.85f},{-0.22f,0.80f},{-0.22f,-0.80f}};
        for(int ti=0;ti<4;ti++){
            Vector2 tmnt = FWD(r*tPos[ti][0], r*tPos[ti][1]);
            Vector2 tbrl = FWD(r*tPos[ti][0]+r*0.50f, r*tPos[ti][1]);
            DrawCircleV(tmnt, r*0.16f, ColorAlpha(c, 0.45f));
            DrawNeonCircle(tmnt, r*0.16f, c, 0.78f);
            DrawNeonLine(tmnt, tbrl, c, 1.6f, 0.80f);
            DrawNeonCircle(tbrl, r*0.09f, c, 0.85f+pulse*0.12f);
        }
        // Command bridge
        Vector2 bridge = FWD(r*0.12f, 0);
        DrawCircleV(bridge, r*0.27f, ColorAlpha(c, 0.40f));
        DrawNeonCircle(bridge, r*0.27f, c, 0.88f);
        for(int i=0;i<8;i++){
            float ha1=dir+i*(PI/4.f), ha2=ha1+PI/4.f;
            DrawNeonLine({bridge.x+cosf(ha1)*r*0.24f,bridge.y+sinf(ha1)*r*0.24f},
                         {bridge.x+cosf(ha2)*r*0.24f,bridge.y+sinf(ha2)*r*0.24f}, c, 0.7f, 0.35f);
        }
        DrawCircleV(bridge, r*0.12f, ColorAlpha(VP_WHITE, 0.58f));
        DrawCircleV(bridge, r*0.06f, VP_WHITE);
        // 6 engine nozzles at rear
        for(int i=0;i<6;i++){
            float lat=(i-2.5f)*r*0.25f;
            Vector2 eng=FWD(-r*0.86f, lat);
            float ef=0.4f+0.6f*sinf(t*7.f+i*0.7f+ph);
            DrawNeonCircle(eng, r*0.12f, c, 0.68f+ef*0.22f);
            DrawCircleV(eng, r*0.06f, ColorAlpha(VP_WHITE, 0.65f+ef*0.25f));
        }
        break;
    }

    case E_GOLDEN:{
        // ── GILDED MONARCH — alien royal warship, 5-wing ceremonial hull ──
        float pulse = 0.5f + 0.5f*sinf(t*4.f + ph);
        Color gc=VP_GOLD, gc2=VP_YELLOW, gc3={255,138,18,255};
        DrawNeonFill(sp, r*0.55f, gc, 0.28f, 0.f);
        DrawNeonCircle(sp, r*0.55f, gc, 0.72f);
        DrawNeonCircle(sp, r*0.40f, gc3, 0.45f);
        for(int i=0;i<5;i++){
            float wa=dir+i*(2*PI/5.f);
            float cs2=cosf(wa),ss2=sinf(wa);
            float wca2=cosf(wa+PI*0.16f),wsa2=sinf(wa+PI*0.16f);
            float wca3=cosf(wa-PI*0.12f),wsa3=sinf(wa-PI*0.12f);
            Vector2 root ={sp.x+cs2*r*0.52f, sp.y+ss2*r*0.52f};
            Vector2 tip  ={sp.x+cs2*r*1.08f, sp.y+ss2*r*1.08f};
            Vector2 traiL={sp.x+wca2*r*0.74f,sp.y+wsa2*r*0.74f};
            Vector2 leadE={sp.x+wca3*r*0.70f,sp.y+wsa3*r*0.70f};
            bool isFwd=(i==0);
            DrawTriangle(root, tip, traiL, ColorAlpha(isFwd?gc:gc3, 0.30f));
            DrawTriangle(root, leadE, tip,  ColorAlpha(gc, 0.22f));
            DrawNeonLine(root, tip,   gc,  isFwd?2.8f:2.0f, 0.92f);
            DrawNeonLine(tip,  traiL, gc3, 1.4f, 0.65f);
            DrawNeonLine(leadE,tip,   gc,  1.2f, 0.55f);
            DrawNeonCircle(tip, r*0.09f, gc2, 0.80f+pulse*0.18f);
            DrawCircleV(tip, r*0.045f, VP_WHITE);
            Vector2 pmid={(root.x+tip.x)*0.55f,(root.y+tip.y)*0.55f};
            DrawNeonLine(tip, pmid, VP_WHITE, 0.6f, 0.25f+slow*0.12f);
        }
        for(int i=0;i<12;i++){
            float ga=dir+t*0.5f+i*(2*PI/12.f);
            DrawCircleV({sp.x+cosf(ga)*r*0.60f,sp.y+sinf(ga)*r*0.60f},
                        r*0.048f, ColorAlpha(gc2,0.60f+slow*0.25f));
        }
        DrawCircleV(sp, r*0.32f, ColorAlpha(gc, 0.50f));
        DrawNeonCircle(sp, r*0.30f, gc, 0.95f);
        for(int i=0;i<6;i++){
            float ga=dir+t*1.2f+i*(PI/3.f);
            DrawNeonLine(sp,{sp.x+cosf(ga)*r*0.25f,sp.y+sinf(ga)*r*0.25f}, gc2, 0.8f, 0.42f);
        }
        DrawNeonCircle(sp, r*0.15f, VP_WHITE, 0.85f+fast*0.12f);
        DrawCircleV(sp, r*0.07f, VP_WHITE);
        DrawNeonLine(FWD(r*0.58f,0), FWD(r*1.22f,0), gc, 2.2f, 0.80f);
        DrawNeonCircle(FWD(r*1.22f,0), r*0.10f, gc2, 0.88f+pulse*0.10f);
        DrawNeonCircle(sp, r*1.10f, gc, 0.14f+slow*0.08f);
        break;
    }

    case E_CURSED:{
        // ── SPECTER CORVETTE — void-corrupted ghost warship ──
        float pulse   = 0.5f + 0.5f*sinf(t*3.8f + ph);
        float flicker = 0.80f + 0.20f*sinf(t*22.f + ph);
        Color cDark   = {(unsigned char)(c.r/3),(unsigned char)(c.g/3),(unsigned char)(c.b/3),255};
        Vector2 hullPts[12];
        for(int i=0;i<12;i++){
            float ha=dir+i*(2*PI/12.f);
            float hfwd=(i<6)?1.12f:0.78f;
            float hr2=r*(0.92f+0.06f*sinf(t*2.8f+i*1.3f+ph));
            hullPts[i]={sp.x+cosf(ha)*hr2*hfwd, sp.y+sinf(ha)*hr2*hfwd};
        }
        for(int i=1;i<11;i++)
            DrawTriangle(sp, hullPts[i], hullPts[i+1], ColorAlpha(c, 0.08f*flicker));
        DrawNeonPoly(hullPts, 12, c, 1.8f, 0.70f*flicker);
        DrawNeonFill(sp, r*0.62f, cDark, 0.45f, 0.f);
        DrawNeonCircle(sp, r*0.62f, c, 0.55f*flicker);
        for(int s=-1;s<=1;s+=2){
            for(int w=0;w<2;w++){
                float wfwd=w==0?r*0.22f:-r*0.12f;
                float wlat=s*(w==0?r*0.82f:r*0.70f);
                float wext=w==0?r*0.56f:r*0.44f;
                Vector2 wRoot=FWD(wfwd,wlat);
                Vector2 wTip =FWD(wfwd-wext,wlat*(w==0?1.15f:1.10f));
                DrawNeonLine(wRoot,wTip,c,w==0?1.8f:1.4f,0.72f*flicker);
                DrawNeonCircle(wTip,r*0.09f,c,0.72f*flicker+fast*0.18f);
                DrawCircleV(wTip,r*0.045f,ColorAlpha(VP_WHITE,0.65f));
            }
        }
        Vector2 eyePos=FWD(r*0.74f,0);
        DrawCircleV(eyePos, r*0.24f, ColorAlpha(c,0.38f*flicker));
        DrawNeonCircle(eyePos, r*0.22f, c, 0.80f*flicker);
        DrawCircleV(eyePos, r*0.11f, ColorAlpha(BLACK,0.90f));
        DrawNeonCircle(eyePos, r*0.11f, c, 0.60f*flicker);
        DrawLineEx(FWD(r*0.74f,-r*0.09f),FWD(r*0.74f,r*0.09f),
                   r*0.07f,ColorAlpha(VP_RED,0.78f+pulse*0.20f));
        DrawCircleV(eyePos, r*0.045f, ColorAlpha(VP_RED,0.92f));
        for(int i=0;i<5;i++){
            float ta=dir+i*(2*PI/5.f)+PI*0.2f;
            float wig=sinf(t*3.f+i*1.9f+ph)*r*0.22f;
            Vector2 tbase={sp.x+cosf(ta)*r*0.58f, sp.y+sinf(ta)*r*0.58f};
            Vector2 ttip ={sp.x+cosf(ta)*r*1.08f+cosf(ta+PI*0.5f)*wig,
                           sp.y+sinf(ta)*r*1.08f+sinf(ta+PI*0.5f)*wig};
            DrawNeonLine(tbase,ttip,c,1.3f,0.48f*flicker);
            DrawCircleV(ttip,r*0.065f,ColorAlpha(VP_RED,0.55f+fast*0.30f));
        }
        float sphase=fmodf(t*0.85f+ph,1.f);
        DrawNeonCircle(sp,r*0.5f+sphase*r*0.7f,c,(1.f-sphase)*0.28f*flicker);
        break;
    }

    case E_BOMB:{
        // ── NOVA MINE — armed proximity warhead, 8 contact spines, forward-facing. ──
        float swell    =1.f+0.06f*sinf(t*3.5f+ph);
        float countdown=0.5f+0.5f*sinf(t*9.f+ph);
        DrawNeonFill(sp, r*swell, c, 0.22f, 0.f);
        DrawNeonCircle(sp, r*swell, c, 0.78f);
        DrawNeonCircle(sp, r*swell*0.75f, c, 0.35f);
        for(int i=0;i<8;i++){
            float a1=dir+i*(2*PI/8.f), a2=a1+(2*PI/8.f)*0.62f;
            DrawNeonLine({sp.x+cosf(a1)*r*0.86f*swell,sp.y+sinf(a1)*r*0.86f*swell},
                         {sp.x+cosf(a2)*r*0.86f*swell,sp.y+sinf(a2)*r*0.86f*swell},
                         c,1.6f,0.60f);
        }
        for(int i=0;i<8;i++){
            float sa=dir+i*(2*PI/8.f);
            Vector2 sBase={sp.x+cosf(sa)*r*0.78f*swell,sp.y+sinf(sa)*r*0.78f*swell};
            Vector2 sMid ={sp.x+cosf(sa)*r*1.06f*swell,sp.y+sinf(sa)*r*1.06f*swell};
            Vector2 sTip ={sp.x+cosf(sa)*r*1.34f*swell,sp.y+sinf(sa)*r*1.34f*swell};
            bool armed=(i==0);
            DrawLineEx(sBase,sTip,r*0.09f,ColorAlpha(c,0.62f));
            DrawNeonLine(sBase,sTip,c,1.5f,0.80f);
            DrawNeonCircle(sTip,r*0.12f,armed?VP_YELLOW:c,0.88f+countdown*(armed?0.10f:0.04f));
            DrawCircleV(sTip,r*0.06f,ColorAlpha(armed?VP_WHITE:c,0.80f+countdown*0.14f));
            DrawNeonCircle(sMid,r*0.07f,c,0.52f);
        }
        for(int i=0;i<8;i++){
            float a1=dir+i*(2*PI/8.f),a2=dir+(i+1)*(2*PI/8.f);
            DrawNeonLine({sp.x+cosf(a1)*r*0.72f*swell,sp.y+sinf(a1)*r*0.72f*swell},
                         {sp.x+cosf(a2)*r*0.72f*swell,sp.y+sinf(a2)*r*0.72f*swell},
                         c,0.7f,0.25f);
        }
        Color warnCol={255,(unsigned char)(60+(int)(countdown*120)),20,255};
        DrawCircleV(sp,r*0.35f*swell,ColorAlpha(c,0.45f));
        DrawNeonCircle(sp,r*0.33f*swell,warnCol,0.80f+countdown*0.18f);
        DrawCircleV(sp,r*0.17f*swell,ColorAlpha(warnCol,0.72f+countdown*0.22f));
        DrawCircleV(sp,r*0.08f*swell,ColorAlpha(VP_WHITE,0.82f+countdown*0.15f));
        float wring=fmodf(t*1.8f+ph,1.f);
        DrawNeonCircle(sp,r*0.4f+wring*r*0.7f,warnCol,(1.f-wring)*0.32f);
        break;
    }

    case E_CRYSTAL:{
        // ── LATTICE SENTINEL — crystalline alien warship, 6 faceted wing-blades ──
        float pulse=0.5f+0.5f*sinf(t*2.8f+ph);
        Color cw={
            (unsigned char)std::min(255,(int)c.r+55),
            (unsigned char)std::min(255,(int)c.g+55),
            (unsigned char)std::min(255,(int)c.b+35),255};
        Vector2 hexPts[6];
        for(int i=0;i<6;i++){
            float ha=dir+i*(2*PI/6.f);
            hexPts[i]={sp.x+cosf(ha)*r*0.50f,sp.y+sinf(ha)*r*0.50f};
        }
        for(int i=1;i<5;i++)
            DrawTriangle(hexPts[0],hexPts[i],hexPts[i+1],ColorAlpha(c,0.22f));
        for(int i=0;i<6;i++)
            DrawNeonLine(hexPts[i],hexPts[(i+1)%6],c,1.8f,0.75f);
        for(int i=0;i<6;i++){
            float sa=dir+i*(2*PI/6.f);
            float len=r*(0.94f+0.12f*sinf(t*2.f+i*1.1f+ph));
            float wid=r*0.23f;
            float cs2=cosf(sa),ss2=sinf(sa),px3=-ss2,py3=cs2;
            Vector2 tip ={sp.x+cs2*len,sp.y+ss2*len};
            Vector2 root={sp.x+cs2*r*0.44f,sp.y+ss2*r*0.44f};
            Vector2 bl  ={root.x+px3*wid,root.y+py3*wid};
            Vector2 br  ={root.x-px3*wid,root.y-py3*wid};
            Vector2 notL={sp.x+cs2*len*0.60f+px3*wid*0.44f,sp.y+ss2*len*0.60f+py3*wid*0.44f};
            Vector2 notR={sp.x+cs2*len*0.60f-px3*wid*0.44f,sp.y+ss2*len*0.60f-py3*wid*0.44f};
            bool isFwd=(i==0);
            DrawTriangle(bl,tip,notL,ColorAlpha(c,isFwd?0.28f:0.20f));
            DrawTriangle(br,notR,tip,ColorAlpha(c,isFwd?0.28f:0.20f));
            DrawTriangle(bl,notL,notR,ColorAlpha(cw,0.14f));
            DrawTriangle(bl,notR,br,  ColorAlpha(cw,0.14f));
            DrawNeonLine(bl,tip,c,isFwd?2.2f:1.8f,isFwd?0.92f:0.80f);
            DrawNeonLine(br,tip,c,isFwd?2.2f:1.8f,isFwd?0.92f:0.80f);
            DrawNeonLine(bl,br, c,1.2f,0.45f);
            DrawNeonLine(notL,notR,c,0.8f,0.32f);
            Vector2 fmid={(tip.x+root.x)*0.52f,(tip.y+root.y)*0.52f};
            DrawNeonLine(tip,fmid,VP_WHITE,0.7f,0.28f+slow*0.18f);
            DrawNeonCircle(tip,r*0.09f,isFwd?VP_WHITE:cw,isFwd?(0.74f+fast*0.22f):(0.55f+fast*0.18f));
        }
        float innerSpin=-t*0.8f+ph;
        for(int i=0;i<3;i++){
            float ia=innerSpin+i*(2*PI/3.f);
            float len2=r*0.42f;
            float cs3=cosf(ia),ss3=sinf(ia),px3=-ss3,py3=cs3;
            Vector2 it2={sp.x+cs3*len2,sp.y+ss3*len2};
            DrawTriangle({sp.x+px3*r*0.10f,sp.y+py3*r*0.10f},it2,
                         {sp.x-px3*r*0.10f,sp.y-py3*r*0.10f},ColorAlpha(cw,0.22f));
            DrawNeonLine({sp.x+px3*r*0.10f,sp.y+py3*r*0.10f},it2,cw,1.2f,0.60f);
            DrawNeonLine({sp.x-px3*r*0.10f,sp.y-py3*r*0.10f},it2,cw,1.2f,0.60f);
        }
        DrawNeonFill(sp,r*0.30f,VP_WHITE,0.22f,0.f);
        DrawNeonCircle(sp,r*0.30f,c,0.88f);
        DrawCircleV(sp,r*0.14f,ColorAlpha(VP_WHITE,0.70f+fast*0.22f));
        DrawCircleV(sp,r*0.06f,VP_WHITE);
        float res=fmodf(t*1.3f+ph,1.f);
        DrawNeonCircle(sp,r*0.40f+res*r*0.58f,c,(1.f-res)*0.32f);
        break;
    }

    case E_VOID:
    default:{
        // ── VOID SCYTHE — dark matter warship, crescent blade facing travel direction ──
        float pulse=0.5f+0.5f*sinf(t*2.8f+ph);
        int NPTS=11;
        Vector2 outerArc[11], innerArc[11];
        for(int i=0;i<NPTS;i++){
            float ang=dir-PI*0.55f+i*(PI*1.10f/(NPTS-1));
            outerArc[i]={sp.x+cosf(ang)*r*1.04f,sp.y+sinf(ang)*r*1.04f};
            float ir=r*(0.54f+0.08f*sinf(t*1.8f+i*0.6f+ph));
            innerArc[i]={sp.x+cosf(ang)*ir,sp.y+sinf(ang)*ir};
        }
        for(int i=0;i<NPTS-1;i++){
            DrawTriangle(outerArc[i],outerArc[i+1],innerArc[i],  ColorAlpha(c,0.20f));
            DrawTriangle(outerArc[i+1],innerArc[i+1],innerArc[i],ColorAlpha(c,0.14f));
        }
        DrawNeonPoly(outerArc,NPTS,c,2.4f,0.90f);
        DrawNeonPoly(innerArc,NPTS,c,1.2f,0.45f);
        for(int i=1;i<NPTS-1;i+=2)
            DrawNeonLine(outerArc[i],innerArc[i],c,0.8f,0.28f);
        DrawNeonCircle(outerArc[0],     r*0.13f,c,0.90f+fast*0.08f);
        DrawCircleV(outerArc[0],        r*0.065f,ColorAlpha(VP_WHITE,0.82f));
        DrawNeonCircle(outerArc[NPTS-1],r*0.13f,c,0.90f+fast*0.08f);
        DrawCircleV(outerArc[NPTS-1],   r*0.065f,ColorAlpha(VP_WHITE,0.82f));
        DrawCircleV(sp,r*0.44f,ColorAlpha(BLACK,0.90f));
        DrawNeonCircle(sp,r*0.44f,c,0.60f);
        Vector2 eyePos=FWD(r*0.10f,0);
        DrawCircleV(eyePos,r*0.23f,ColorAlpha(BLACK,0.95f));
        DrawNeonCircle(eyePos,r*0.23f,c,0.70f);
        DrawNeonCircle(eyePos,r*0.11f,VP_WHITE,0.30f+fast*0.18f);
        DrawCircleV(eyePos,r*0.05f,ColorAlpha(VP_WHITE,0.80f));
        for(int ring=1;ring<=3;ring++){
            float rr=r*(0.65f+ring*0.22f);
            float warp=sinf(t*0.8f+ring*1.6f+ph)*r*0.07f;
            DrawNeonCircle({sp.x+cosf(dir+ring)*warp,sp.y+sinf(dir+ring)*warp},
                           rr,c,(0.32f-ring*0.07f)*pulse);
        }
        for(int i=0;i<5;i++){
            float fa=dir+PI*0.5f+i*(PI/4.5f)-PI*0.5f;
            float fir=r*0.30f, forr=r*0.50f+2.f*sinf(t*4.f+i);
            DrawNeonLine({sp.x+cosf(fa)*fir,sp.y+sinf(fa)*fir},
                         {sp.x+cosf(fa+0.22f)*forr,sp.y+sinf(fa+0.22f)*forr},
                         c,1.2f,0.38f+slow*0.18f);
        }
        float dp=fmodf(t*0.55f+ph*0.4f,1.f);
        DrawNeonCircle(sp,r*0.3f+dp*r*1.1f,c,(1.f-dp)*0.20f);
        break;
    }
    }
    #undef FWD


    // Low-HP warning pulse — brightness flare around enemy
    if(e.maxHp > 0 && hp01 < 0.35f){
        PrayerType weak = EnemyWeakness(e.type);
        Color wc = PrayerColor(weak);
        float warningA = (0.35f - hp01) / 0.35f;
        DrawNeonCircle(sp, r + 4.f + 2.f*fast, wc, warningA * (0.4f + fast*0.3f));
    }
}

// ──────────────────────────────────────────────────────────────

void DrawBoss(Boss& b, Vector2 sp, float t){
    if(!b.active) return;
    float r   = b.radius;
    float ph  = b.angle;
    float pulse = 0.5f + 0.5f*sinf(t*3.f);
    float hp01 = (b.maxHp > 0) ? (float)b.hp / b.maxHp : 1.f;
    Color c   = b.col;

    if(b.flashTimer > 0){
        float fw = (float)b.flashTimer / 10.f;
        c = LerpC(c, VP_WHITE, fw);
    }

    // Outer aura
    DrawCircleV(sp, r*1.55f, ColorAlpha(c, 0.06f + 0.04f*pulse));

    // Phase-based body
    if(b.phase >= 2){
        // Phase 2: jagged second ring
        float spin2 = t * 0.7f + ph;
        Vector2 outer[8];
        for(int i=0;i<8;i++){
            float a = spin2 + i*(2*PI/8.f);
            float rr = r + 8.f*sinf(t*4.f + i*0.9f);
            outer[i] = {sp.x+cosf(a)*rr, sp.y+sinf(a)*rr};
        }
        DrawNeonPoly(outer, 8, c, 1.f, 0.45f);
    }

    // Main body ring
    DrawNeonFill(sp, r, c, 0.15f, 0.f);
    DrawNeonCircle(sp, r,       c, 0.95f);
    DrawNeonCircle(sp, r*0.72f, c, 0.5f);

    // Rotating orbital arms
    int armCount = (b.phase >= 2) ? 6 : 4;
    float armSpin = t * 1.2f + ph;
    for(int i=0;i<armCount;i++){
        float a  = armSpin + i*(2*PI/armCount);
        float a2 = a + 0.4f;
        float r1 = r*0.55f, r2p = r*1.05f;
        DrawNeonLine({sp.x+cosf(a)*r1,  sp.y+sinf(a)*r1},
                     {sp.x+cosf(a2)*r2p, sp.y+sinf(a2)*r2p}, c, 1.8f, 0.65f);
        DrawCircleV({sp.x+cosf(a2)*r2p, sp.y+sinf(a2)*r2p}, 3.f, ColorAlpha(c, 0.7f));
    }

    // HP bar rendered as arc segments inside the body
    int hpSegs = (int)(hp01 * 12);
    Color hpCol = (hp01 > 0.5f) ? VP_YELLOW : VP_RED;
    for(int i=0;i<hpSegs;i++){
        float a1 = -PI*0.5f + i*(2*PI/12.f);
        float a2 = a1 + (2*PI/12.f)*0.75f;
        DrawNeonLine({sp.x+cosf(a1)*r*0.6f, sp.y+sinf(a1)*r*0.6f},
                     {sp.x+cosf(a2)*r*0.6f, sp.y+sinf(a2)*r*0.6f}, hpCol, 2.5f, 0.75f);
    }

    // Core
    DrawNeonFill(sp, r*0.28f, VP_WHITE, 0.3f + pulse*0.2f, 0.f);
    DrawNeonCircle(sp, r*0.28f, VP_WHITE, 0.7f + pulse*0.25f);

    // Boss name label
    if(!b.name.empty()){
        int tw = MeasureText(b.name.c_str(), 7);
        DrawText(b.name.c_str(), (int)(sp.x - tw*0.5f), (int)(sp.y + r + 8), 7,
                 ColorAlpha(c, 0.75f + pulse*0.2f));
    }

    // Symbol
    if(!b.sym.empty()){
        int sw = MeasureText(b.sym.c_str(), 14);
        DrawText(b.sym.c_str(), (int)(sp.x - sw*0.5f), (int)(sp.y - 7), 14,
                 ColorAlpha(VP_WHITE, 0.55f + pulse*0.3f));
    }
}

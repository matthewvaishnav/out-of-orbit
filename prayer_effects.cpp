// prayer_effects.cpp — Per-frame DoT, freeze decay, and ghost orb logic

#include "void_blaster.h"

void UpdatePrayerEffects(Game& g, float dt) {

    for (auto& e : g.enemies) {
        if (!e.active) continue;

        // ── FIRE: burn tick ──────────────────────────────────────
        if (e.burnTimer > 0.f) {
            e.burnTimer -= dt;
            float dmgThisFrame = e.burnStacks * 3.f * dt;
            e.hp -= (int)dmgThisFrame;
            if (RandF(0.f, 1.f) < dmgThisFrame * 0.5f)
                SpawnParticles(g, e.pos, VP_ORANGE, 1, 40.f, true);
            if (e.burnTimer <= 0.f) {
                e.burnStacks = 0;
                e.burnTimer  = 0.f;
            }
            if (e.hp <= 0) KillEnemy(g, e);
        }

        // ── ICE: freeze decay ────────────────────────────────────
        if (e.frozenTimer > 0.f) {
            e.frozenTimer -= dt;
            e.frozenMult   = 0.35f;
            if (e.frozenTimer <= 0.f) {
                e.frozenTimer = 0.f;
                e.frozenMult  = 1.f;
                e.frozenAmp   = false;
            }
        } else {
            e.frozenMult = 1.f;
        }

        // Apply freeze slow — damp velocity toward zero each frame
        if (e.frozenMult < 1.f) {
            float damp = 1.f - (1.f - e.frozenMult) * dt * 6.f;
            e.vel.x *= damp;
            e.vel.y *= damp;
        }
    }

    // ── SPIRIT: ghost orb update ─────────────────────────────────
    if (g.ghostOrbActive) {
        g.ghostOrbAngle += dt * 2.4f;
        g.ghostOrbTimer -= dt;

        if (g.ghostOrbTimer <= 0.f) {
            g.ghostOrbTimer = 0.9f;

            float orbX = g.playerPos.x + cosf(g.ghostOrbAngle) * 38.f;
            float orbY = g.playerPos.y + sinf(g.ghostOrbAngle) * 38.f;

            // Find nearest enemy
            Enemy* nearest  = nullptr;
            float  nearDist = 1e9f;
            for (auto& e : g.enemies) {
                if (!e.active) continue;
                float dx = e.pos.x - orbX, dy = e.pos.y - orbY;
                float d  = dx*dx + dy*dy;
                if (d < nearDist) { nearDist = d; nearest = &e; }
            }

            if (nearest) {
                float dx = nearest->pos.x - orbX;
                float dy = nearest->pos.y - orbY;
                float d  = sqrtf(dx*dx + dy*dy);
                if (d > 0.01f) {
                    Bullet b{};
                    b.pos         = {orbX, orbY};
                    b.vel         = {(dx/d)*280.f, (dy/d)*280.f};
                    b.life        = 80;
                    b.active      = true;
                    b.col         = VP_LAVENDER;
                    b.glowR       = 6.f;
                    b.prayerType  = 2;
                    b.damage      = 2;
                    b.homing      = true;
                    b.homingStr   = 2.f;
                    b.shipType    = g.selectedShip;
                    BulletPoolPush(g.bullets, g.bulletRover, b);
                    SpawnParticles(g, {orbX, orbY}, VP_PURPLE, 2, 60.f, true);
                }
            }
        }
    }
}

void DrawGhostOrb(Game& g, float t) {
    if (!g.ghostOrbActive) return;

    float orbX = g.playerPos.x + cosf(g.ghostOrbAngle) * 38.f;
    float orbY = g.playerPos.y + sinf(g.ghostOrbAngle) * 38.f;

    Vector2 sp = W2S(g, {orbX, orbY});
    if (!OnScreen(sp, 20.f)) return;

    float pulse = 0.6f + 0.4f * sinf(t * 5.f);

    // Orbit trail
    for (int i = 1; i <= 8; i++) {
        float ta  = g.ghostOrbAngle - i * 0.18f;
        float tx  = g.playerPos.x + cosf(ta) * 38.f;
        float ty  = g.playerPos.y + sinf(ta) * 38.f;
        Vector2 ts = W2S(g, {tx, ty});
        DrawNeonCircle(ts, 3.f * (1.f - i/9.f), VP_PURPLE, (1.f - i/9.f) * 0.5f);
    }

    // Orb body
    DrawNeonFill(sp, 7.f * pulse, VP_PURPLE, 0.18f, 0.85f);
    DrawNeonCircle(sp, 11.f * pulse, VP_LAVENDER, 0.35f);
    DrawNeonCircle(sp, 18.f, VP_PURPLE, 0.12f);
}

#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define MAX_PARTICLES 50
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 960

typedef enum {
  ENEMY = 0,
  POWER_UP_HEALTH = 1,
  POWER_UP_INVIS = 2,
  POWER_UP_DOUBLE_FIRE_DAMAGE = 4,
  POWER_UP_DOUBLE_ENEMY_DAMAGE = 8,
  POWER_UP_SPREAD = 16,
  BOX = 32,
  PROJECTILE = 64,
  BOSS_ORCA = 128,
  BOSS_EEL = 256,
  BOSS_KRAKEN = 512,
  ENEMY_RINGS = 1024,
  ENEMY_OIL = 2048,
  ENEMY_STRAW = 4096,
  ENEMY_JELLYFISH = 4096 * 2,
  ENEMY_ANCHOR = 4096 * 4,
  GAP = 4096 * 8,
  POWERUP = 4096 * 16,
} ParticleType;

#define SINGLE_LINE_PATTERN COUNT 6
ParticleType signleLinePatterns[6][5] = {
    {ENEMY, ENEMY, ENEMY, ENEMY, ENEMY}, {GAP, ENEMY, POWERUP, ENEMY, GAP},
    {GAP, GAP, GAP, GAP, GAP},           {ENEMY, ENEMY, GAP, GAP, ENEMY},
    {POWERUP, GAP, ENEMY, ENEMY, GAP},   {ENEMY, ENEMY, GAP, ENEMY, POWERUP}};

#define MULTI_LINE_PATTERN_COUNT 4
ParticleType multipleLinePattern[12][5] = {
    {ENEMY, ENEMY, ENEMY, ENEMY, ENEMY},
    {ENEMY, ENEMY, BOSS_EEL, ENEMY, ENEMY},
    {ENEMY, ENEMY, GAP, ENEMY, ENEMY},

    {ENEMY, BOSS_EEL, ENEMY, BOSS_EEL, ENEMY},
    {ENEMY, GAP, ENEMY, GAP, ENEMY},
    {ENEMY, ENEMY, ENEMY, ENEMY, ENEMY},

    {ENEMY, ENEMY, ENEMY, ENEMY, ENEMY},
    {BOSS_ORCA, GAP, ENEMY, BOSS_ORCA, GAP},
    {GAP, GAP, ENEMY, GAP, GAP},

    {ENEMY, ENEMY, ENEMY, ENEMY, ENEMY},
    {GAP, ENEMY, POWERUP, ENEMY, GAP},
    {GAP, GAP, GAP, GAP, GAP},
};

typedef struct {
  int x;
  int y;
  int dy;
  int dx;
  Texture2D image;
  int w;
  int h;
  int frameWidth;
  int frameHeight;
  unsigned int numberOfFrames;
  unsigned int frameNumber;
  int health;
  ParticleType type;
  bool isAlive;
} Particle;

typedef struct {
  Particle particles[MAX_PARTICLES];
  int speed;
  int count;
} ParticleSystem;

Particle powerupParticles[6];
Particle enemyParticles[5];
Particle bossParticles[3];

void particle_init();
void particle_free();
ParticleSystem *particle_system_init(int speed);
void particle_system_free(ParticleSystem *system);
void particle_draw(Particle *particle);
void particle_draw_system(ParticleSystem *system);
void particle_update_system(ParticleSystem *system);
void particle_update(Particle *particle);
void particle_create(Particle *particle);
void particle_animate(Particle *particle, unsigned long frameCount);
void particle_update_animation(ParticleSystem *system, unsigned long count);

void particle_boss_system_create_particle(ParticleSystem *system);
void particle_power_system_create_particle(ParticleSystem *system);
void particle_enemy_system_create_particle(ParticleSystem *system);
void particle_create_boss(Particle *particle);
void particle_create_powerup(Particle *particle);
void particle_create_enemy(Particle *particle);
void particle_queue_pattern(ParticleSystem *powerup, ParticleSystem *enemy,
                            ParticleSystem *boss, unsigned long framCount);
int interval(unsigned long frameCount, const int fps);
int gap(unsigned long frameCount);

unsigned long lastPatternTime = 0;

int main() {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandy Shore Tech Demo 4");
  SetTargetFPS(60);
  particle_init();
  ParticleSystem *enemies = particle_system_init(20);
  ParticleSystem *bosses = particle_system_init(5);
  ParticleSystem *powerups = particle_system_init(10);
  unsigned long count = 0;

  while (!WindowShouldClose()) {
    count++;
    if (GetRandomValue(0, 100) > 80) {
      particle_enemy_system_create_particle(enemies);
    }
    particle_update_system(enemies);
    particle_update_animation(enemies, count);

    if (GetRandomValue(0, 200) > 180) {
      particle_boss_system_create_particle(bosses);
    }
    particle_update_system(bosses);
    particle_update_animation(bosses, count);

    if (GetRandomValue(0, 100) > 90) {
      particle_power_system_create_particle(powerups);
    }
    particle_update_system(powerups);
    particle_update_animation(powerups, count);

    BeginDrawing();

    ClearBackground(RAYWHITE);
    particle_draw_system(enemies);
    particle_draw_system(bosses);
    particle_draw_system(powerups);
    EndDrawing();
  }

  CloseWindow();
  particle_system_free(enemies);
  particle_system_free(powerups);
  particle_system_free(bosses);
  particle_free();

  return 0;
}

ParticleSystem *particle_system_init(int speed) {
  ParticleSystem *system = MemAlloc(sizeof(ParticleSystem));
  system->speed = speed;
  system->count = 0;
  for (int i = 0; i < MAX_PARTICLES; i++) {
    system->particles[i].isAlive = false;
  }
  return system;
}

void particle_system_free(ParticleSystem *system) { MemFree(system); }

void particle_draw(Particle *particle) {
  int frame = particle->frameNumber % particle->numberOfFrames;
  Rectangle source = {frame * particle->frameWidth, 0, particle->frameWidth,
                      particle->frameHeight};
  Vector2 position = {particle->x, particle->y};
  DrawTextureRec(particle->image, source, position, WHITE);
}

void particle_draw_system(ParticleSystem *system) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (system->particles[i].isAlive) {
      particle_draw(&system->particles[i]);
    }
  }
}

void particle_update_system(ParticleSystem *system) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    particle_update(&system->particles[i]);
  }
}

void particle_update(Particle *particle) {
  if (particle->isAlive) {
    if (particle->y > SCREEN_HEIGHT + particle->h) {
      particle->isAlive = false;
    } else {
      particle->y += particle->dy;
    }
  }
}

void particle_create_enemy(Particle *particle) {
  int random = GetRandomValue(0, 4);
  memcpy(particle, &enemyParticles[random], sizeof(Particle));
  particle->x = GetRandomValue(0, SCREEN_WIDTH);
  particle->isAlive = true;
}

void particle_create_powerup(Particle *particle) {
  int random = GetRandomValue(0, 5);
  memcpy(particle, &powerupParticles[random], sizeof(Particle));
  particle->x = GetRandomValue(0, SCREEN_WIDTH);
  particle->isAlive = true;
}

void particle_create_boss(Particle *particle) {
  int random = GetRandomValue(0, 2);
  memcpy(particle, &bossParticles[random], sizeof(Particle));
  particle->x = GetRandomValue(0, SCREEN_WIDTH);
  particle->isAlive = true;
}

void particle_enemy_system_create_particle(ParticleSystem *system) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!system->particles[i].isAlive) {
      particle_create_enemy(&system->particles[i]);
      return;
    }
  }
}

void particle_power_system_create_particle(ParticleSystem *system) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!system->particles[i].isAlive) {
      particle_create_powerup(&system->particles[i]);
      return;
    }
  }
}

void particle_boss_system_create_particle(ParticleSystem *system) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!system->particles[i].isAlive) {
      particle_create_boss(&system->particles[i]);
      return;
    }
  }
}

void particle_init() {
  powerupParticles[0].dx = 0;
  powerupParticles[0].dy = 1;
  powerupParticles[0].image = LoadTexture("../src/assets/images/crate.png");
  powerupParticles[0].w = 48;
  powerupParticles[0].h = 25;
  powerupParticles[0].frameWidth = 24;
  powerupParticles[0].frameHeight = 25;
  powerupParticles[0].numberOfFrames = 2;
  powerupParticles[0].frameNumber = 0;
  powerupParticles[0].health = 10;
  powerupParticles[0].type = BOX;
  powerupParticles[0].isAlive = true;

  powerupParticles[1].dx = 0;
  powerupParticles[1].dy = 1;
  powerupParticles[1].image =
      LoadTexture("../src/assets/images/cake_slice.png");
  powerupParticles[1].w = 60;
  powerupParticles[1].h = 28;
  powerupParticles[1].frameWidth = 30;
  powerupParticles[1].frameHeight = 28;
  powerupParticles[1].numberOfFrames = 2;
  powerupParticles[1].frameNumber = 0;
  powerupParticles[1].health = 0;
  powerupParticles[1].type = POWER_UP_HEALTH | BOX;
  powerupParticles[1].isAlive = true;

  powerupParticles[2].dx = 0;
  powerupParticles[2].dy = 1;
  powerupParticles[2].image = LoadTexture("../src/assets/images/coconut.png");
  powerupParticles[2].w = 102;
  powerupParticles[2].h = 40;
  powerupParticles[2].frameWidth = 102 / 3;
  powerupParticles[2].frameHeight = 40;
  powerupParticles[2].numberOfFrames = 3;
  powerupParticles[2].frameNumber = 0;
  powerupParticles[2].health = 0;
  powerupParticles[2].type = POWER_UP_INVIS | BOX;
  powerupParticles[2].isAlive = true;

  powerupParticles[3].dx = 0;
  powerupParticles[3].dy = 1;
  powerupParticles[3].image = LoadTexture("../src/assets/images/mango.png");
  powerupParticles[3].w = 175;
  powerupParticles[3].h = 27;
  powerupParticles[3].frameWidth = 175 / 7;
  powerupParticles[3].frameHeight = 27;
  powerupParticles[3].numberOfFrames = 7;
  powerupParticles[3].frameNumber = 0;
  powerupParticles[3].health = 0;
  powerupParticles[3].type = POWER_UP_DOUBLE_FIRE_DAMAGE | BOX;
  powerupParticles[3].isAlive = true;

  powerupParticles[4].dx = 0;
  powerupParticles[4].dy = 1;
  powerupParticles[4].image = LoadTexture("../src/assets/images/soda.png");
  powerupParticles[4].w = 200;
  powerupParticles[4].h = 50;
  powerupParticles[4].frameWidth = 200 / 4;
  powerupParticles[4].frameHeight = 50;
  powerupParticles[4].numberOfFrames = 4;
  powerupParticles[4].frameNumber = 0;
  powerupParticles[4].health = 0;
  powerupParticles[4].type = POWER_UP_DOUBLE_ENEMY_DAMAGE | BOX;
  powerupParticles[4].isAlive = true;

  powerupParticles[5].dx = 0;
  powerupParticles[5].dy = 1;
  powerupParticles[5].image = LoadTexture("../src/assets/images/tea.png");
  powerupParticles[5].w = 39;
  powerupParticles[5].h = 21;
  powerupParticles[5].frameWidth = 39 / 3;
  powerupParticles[5].frameHeight = 21;
  powerupParticles[5].numberOfFrames = 3;
  powerupParticles[5].frameNumber = 0;
  powerupParticles[5].health = 0;
  powerupParticles[5].type = POWER_UP_SPREAD | BOX;
  powerupParticles[5].isAlive = true;

  enemyParticles[0].dx = 0;
  enemyParticles[0].dy = 1;
  enemyParticles[0].image = LoadTexture("../src/assets/images/straw.png");
  enemyParticles[0].w = 124;
  enemyParticles[0].h = 30;
  enemyParticles[0].frameWidth = 124 / 4;
  enemyParticles[0].frameHeight = 30;
  enemyParticles[0].numberOfFrames = 4;
  enemyParticles[0].frameNumber = 0;
  enemyParticles[0].health = 3;
  enemyParticles[0].type = ENEMY;
  enemyParticles[0].isAlive = true;

  enemyParticles[1].dx = 0;
  enemyParticles[1].dy = 1;
  enemyParticles[1].image = LoadTexture("../src/assets/images/rings.png");
  enemyParticles[1].w = 64;
  enemyParticles[1].h = 32;
  enemyParticles[1].frameWidth = 32;
  enemyParticles[1].frameHeight = 32;
  enemyParticles[1].numberOfFrames = 2;
  enemyParticles[1].frameNumber = 0;
  enemyParticles[1].health = 6;
  enemyParticles[1].type = ENEMY;
  enemyParticles[1].isAlive = true;

  enemyParticles[2].dx = 0;
  enemyParticles[2].dy = 1;
  enemyParticles[2].image = LoadTexture("../src/assets/images/anchor.png");
  enemyParticles[2].w = 48;
  enemyParticles[2].h = 24;
  enemyParticles[2].frameWidth = 24;
  enemyParticles[2].frameHeight = 24;
  enemyParticles[2].numberOfFrames = 2;
  enemyParticles[2].frameNumber = 0;
  enemyParticles[2].health = 9;
  enemyParticles[2].type = ENEMY;
  enemyParticles[2].isAlive = true;

  enemyParticles[3].dx = 0;
  enemyParticles[3].dy = 1;
  enemyParticles[3].image = LoadTexture("../src/assets/images/jellyfish.png");
  enemyParticles[3].w = 96;
  enemyParticles[3].h = 32;
  enemyParticles[3].frameWidth = 32;
  enemyParticles[3].frameHeight = 32;
  enemyParticles[3].numberOfFrames = 3;
  enemyParticles[3].frameNumber = 0;
  enemyParticles[3].health = 9;
  enemyParticles[3].type = ENEMY;
  enemyParticles[3].isAlive = true;

  enemyParticles[4].dx = 0;
  enemyParticles[4].dy = 1;
  enemyParticles[4].image = LoadTexture("../src/assets/images/oil.png");
  enemyParticles[4].w = 40;
  enemyParticles[4].h = 17;
  enemyParticles[4].frameWidth = 20;
  enemyParticles[4].frameHeight = 17;
  enemyParticles[4].numberOfFrames = 2;
  enemyParticles[4].frameNumber = 0;
  enemyParticles[4].health = 32768;
  enemyParticles[4].type = ENEMY;
  enemyParticles[4].isAlive = true;

  bossParticles[0].dx = 0;
  bossParticles[0].dy = 1;
  bossParticles[0].image = LoadTexture("../src/assets/images/orca.png");
  bossParticles[0].w = 200;
  bossParticles[0].h = 56;
  bossParticles[0].frameWidth = 100;
  bossParticles[0].frameHeight = 56;
  bossParticles[0].numberOfFrames = 2;
  bossParticles[0].frameNumber = 0;
  bossParticles[0].health = 25;
  bossParticles[0].type = ENEMY;
  bossParticles[0].isAlive = true;

  bossParticles[1].dx = 0;
  bossParticles[1].dy = 1;
  bossParticles[1].image = LoadTexture("../src/assets/images/eel.png");
  bossParticles[1].w = 320;
  bossParticles[1].h = 64;
  bossParticles[1].frameWidth = 320 / 5;
  bossParticles[1].frameHeight = 64;
  bossParticles[1].numberOfFrames = 5;
  bossParticles[1].frameNumber = 0;
  bossParticles[1].health = 50;
  bossParticles[1].type = ENEMY;
  bossParticles[1].isAlive = true;

  bossParticles[2].dx = 0;
  bossParticles[2].dy = 1;
  bossParticles[2].image = LoadTexture("../src/assets/images/kraken.png");
  bossParticles[2].w = 192;
  bossParticles[2].h = 58;
  bossParticles[2].frameWidth = 192 / 3;
  bossParticles[2].frameHeight = 58;
  bossParticles[2].numberOfFrames = 3;
  bossParticles[2].frameNumber = 0;
  bossParticles[2].health = 100;
  bossParticles[2].type = ENEMY;
  bossParticles[2].isAlive = true;
}

void particle_free() {
  for (int i = 0; i < 6; i++) {
    UnloadTexture(powerupParticles[i].image);
  }
  for (int i = 0; i < 5; i++) {
    UnloadTexture(enemyParticles[i].image);
  }
  for (int i = 0; i < 3; i++) {
    UnloadTexture(bossParticles[i].image);
  }
}

void particle_animate(Particle *particle, unsigned long frameCount) {
  if (frameCount % 30 == 0) {
    particle->frameNumber++;
  }
}

void particle_update_animation(ParticleSystem *system, unsigned long count) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    particle_animate(&system->particles[i], count);
  }
}

void particle_queue_pattern(ParticleSystem *powerup, ParticleSystem *enemy,
                            ParticleSystem *boss, unsigned long framCount) {
  // Can I generate a pattern at this time?
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (powerup->particles[i].y < 0 && powerup->particles[i].isAlive) {
      return;
    }
    if (enemy->particles[i].y < 0 && enemy->particles[i].isAlive) {
      return;
    }
    if (boss->particles[i].y < 0 && boss->particles[i].isAlive) {
      return;
    }
  }
  // Do I delay generation?
  // Do I generate a single line or multiple pattern
  // Which patterd do I choose:
}

int interval(unsigned long frameCount, const int fps) {
  int time = frameCount * fps;
  if (time < 30) {
    return 0;
  } else if (time < 75) {
    return 1;
  } else if (time < 150) {
    return 2;
  } else if (time < 240) {
    return 3;
  } else {
    return 4;
  }
}

int gap(unsigned long frameCount) {
  int delay = 8 - 2 * interval(frameCount, 60);
  return delay > 0 ? delay : 0;
}

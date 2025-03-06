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
} ParticleType;

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
void particle_system_create_particle(ParticleSystem *system);
void particle_animate(Particle *particle, unsigned long frameCount);
void particle_update_animation(ParticleSystem *system, unsigned long count);

int main(void) {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandy Shore Tech Demo 1");
  SetTargetFPS(60);
  particle_init();
  ParticleSystem *system = particle_system_init(20);
  unsigned long count = 0;

  while (!WindowShouldClose()) {
    count++;
    if (GetRandomValue(0, 100) > 80) {
      particle_system_create_particle(system);
    }
    particle_update_system(system);
    particle_update_animation(system, count);

    BeginDrawing();

    ClearBackground(RAYWHITE);
    particle_draw_system(system);
    EndDrawing();
  }

  CloseWindow();
  particle_system_free(system);
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
void particle_system_create_particle(ParticleSystem *system) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!system->particles[i].isAlive) {
      particle_create_enemy(&system->particles[i]);
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
  powerupParticles[1].type = POWER_UP_HEALTH;
  powerupParticles[1].isAlive = true;

  powerupParticles[2].dx = 0;
  powerupParticles[2].dy = 1;
  powerupParticles[2].image = LoadTexture("../src/assets/images/coconut.png");
  powerupParticles[2].w = 17 * 3;
  powerupParticles[2].h = 20;
  powerupParticles[2].frameWidth = 17;
  powerupParticles[2].frameHeight = 20;
  powerupParticles[2].numberOfFrames = 3;
  powerupParticles[2].frameNumber = 0;
  powerupParticles[2].health = 0;
  powerupParticles[2].type = POWER_UP_INVIS;
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
  powerupParticles[3].type = POWER_UP_DOUBLE_FIRE_DAMAGE;
  powerupParticles[3].isAlive = true;

  powerupParticles[4].dx = 0;
  powerupParticles[4].dy = 1;
  powerupParticles[4].image = LoadTexture("../src/assets/images/soda.png");
  powerupParticles[4].w = 48;
  powerupParticles[4].h = 32;
  powerupParticles[4].frameWidth = 48 / 4;
  powerupParticles[4].frameHeight = 32;
  powerupParticles[4].numberOfFrames = 4;
  powerupParticles[4].frameNumber = 0;
  powerupParticles[4].health = 0;
  powerupParticles[4].type = POWER_UP_DOUBLE_ENEMY_DAMAGE;
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
  powerupParticles[5].type = POWER_UP_SPREAD;
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
  bossParticles[0].w = 150 * 2;
  bossParticles[0].h = 84;
  bossParticles[0].frameWidth = 150;
  bossParticles[0].frameHeight = 84;
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
  bossParticles[1].numberOfFrames = 4;
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

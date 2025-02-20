#include "raylib.h"
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
  BOX = 32
} ParticleType;

typedef struct {
  int x;
  int y;
  int dy;
  int dx;
  Image image;
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

int main(void) {

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandy Shore Tech Demo 1");
  SetTargetFPS(60);
  particle_init();
  ParticleSystem *system = particle_system_init(20);

  while (!WindowShouldClose()) {

    if (GetRandomValue(0, 100) > 80) {
      particle_system_create_particle(system);
    }

    particle_update_system(system);
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
  DrawRectangle(particle->x, particle->y, particle->w, particle->h,
                particle->color);
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

void particle_create(Particle *particle) {
  particle->x = GetRandomValue(0, SCREEN_WIDTH);
  particle->y = 0;
  particle->dy = 1;
  particle->w = 32;
  particle->h = 32;
  particle->type = (ParticleType)GetRandomValue(0, 6);
  particle->isAlive = true;
  if (particle->type != ENEMY) {
    particle->type = (ParticleType)(particle->type | BOX);
    particle->color = GREEN;
  } else {
    particle->color = RED;
  }
}
void particle_system_create_particle(ParticleSystem *system) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!system->particles[i].isAlive) {
      particle_create(&system->particles[i]);
      return;
    }
  }
}

void particle_init() {
  powerupParticles[0].dx = 0;
  powerupParticles[0].dy = 1;
  powerupParticles[0].image = LoadImage("./assets/images/crate.png");
  powerupParticles[0].w = 32;
  powerupParticles[0].h = 32;
  powerupParticles[0].frameWidth = 32;
  powerupParticles[0].frameHeight = 32;
  powerupParticles[0].numberOfFrames = 1;
  powerupParticles[0].frameNumber = 0;
  powerupParticles[0].health = 10;
  powerupParticles[0].type = BOX;
  powerupParticles[0].isAlive = true;

  powerupParticles[1].dx = 0;
  powerupParticles[1].dy = 1;
  powerupParticles[1].image = LoadImage("./assets/images/cake_slice.png");
  powerupParticles[1].w = 32;
  powerupParticles[1].h = 32;
  powerupParticles[1].frameWidth = 32;
  powerupParticles[1].frameHeight = 32;
  powerupParticles[1].numberOfFrames = 1;
  powerupParticles[1].frameNumber = 0;
  powerupParticles[1].health = 10;
  powerupParticles[1].type = POWER_UP_HEALTH;
  powerupParticles[1].isAlive = true;

  powerupParticles[2].dx = 0;
  powerupParticles[2].dy = 1;
  powerupParticles[2].image = LoadImage("./assets/images/coconut.png");
  powerupParticles[2].w = 32;
  powerupParticles[2].h = 32;
  powerupParticles[2].frameWidth = 32;
  powerupParticles[2].frameHeight = 32;
  powerupParticles[2].numberOfFrames = 1;
  powerupParticles[2].frameNumber = 0;
  powerupParticles[2].health = 10;
  powerupParticles[2].type = POWER_UP_INVIS;
  powerupParticles[2].isAlive = true;

  powerupParticles[3].dx = 0;
  powerupParticles[3].dy = 1;
  powerupParticles[3].image = LoadImage("./assets/images/mango.png");
  powerupParticles[3].w = 32;
  powerupParticles[3].h = 32;
  powerupParticles[3].frameWidth = 32;
  powerupParticles[3].frameHeight = 32;
  powerupParticles[3].numberOfFrames = 1;
  powerupParticles[3].frameNumber = 0;
  powerupParticles[3].health = 10;
  powerupParticles[3].type = POWER_UP_DOUBLE_FIRE_DAMAGE;
  powerupParticles[3].isAlive = true;

  powerupParticles[4].dx = 0;
  powerupParticles[4].dy = 1;
  powerupParticles[4].image = LoadImage("./assets/images/not_coke.png");
  powerupParticles[4].w = 32;
  powerupParticles[4].h = 32;
  powerupParticles[4].frameWidth = 32;
  powerupParticles[4].frameHeight = 32;
  powerupParticles[4].numberOfFrames = 1;
  powerupParticles[4].frameNumber = 0;
  powerupParticles[4].health = 10;
  powerupParticles[4].type = POWER_UP_DOUBLE_ENEMY_DAMAGE;
  powerupParticles[4].isAlive = true;

  powerupParticles[5].dx = 0;
  powerupParticles[5].dy = 1;
  powerupParticles[5].image = LoadImage("./assets/images/tea.png");
  powerupParticles[5].w = 32;
  powerupParticles[5].h = 32;
  powerupParticles[5].frameWidth = 32;
  powerupParticles[5].frameHeight = 32;
  powerupParticles[5].numberOfFrames = 1;
  powerupParticles[5].frameNumber = 0;
  powerupParticles[5].health = 10;
  powerupParticles[5].type = POWER_UP_SPREAD;
  powerupParticles[5].isAlive = true;

  enemyParticles[0].dx = 0;
  enemyParticles[0].dy = 1;
  enemyParticles[0].image = LoadImage("./assets/images/straw.png");
  enemyParticles[0].w = 32;
  enemyParticles[0].h = 32;
  enemyParticles[0].frameWidth = 32;
  enemyParticles[0].frameHeight = 32;
  enemyParticles[0].numberOfFrames = 1;
  enemyParticles[0].frameNumber = 0;
  enemyParticles[0].health = 3;
  enemyParticles[0].type = ENEMY;
  enemyParticles[0].isAlive = true;

  enemyParticles[1].dx = 0;
  enemyParticles[1].dy = 1;
  enemyParticles[1].image = LoadImage("./assets/images/rings.png");
  enemyParticles[1].w = 32;
  enemyParticles[1].h = 32;
  enemyParticles[1].frameWidth = 32;
  enemyParticles[1].frameHeight = 32;
  enemyParticles[1].numberOfFrames = 1;
  enemyParticles[1].frameNumber = 0;
  enemyParticles[1].health = 6;
  enemyParticles[1].type = ENEMY;
  enemyParticles[1].isAlive = true;

  enemyParticles[2].dx = 0;
  enemyParticles[2].dy = 1;
  enemyParticles[2].image = LoadImage("./assets/images/anchor.png");
  enemyParticles[2].w = 32;
  enemyParticles[2].h = 32;
  enemyParticles[2].frameWidth = 32;
  enemyParticles[2].frameHeight = 32;
  enemyParticles[2].numberOfFrames = 1;
  enemyParticles[2].frameNumber = 0;
  enemyParticles[2].health = 9;
  enemyParticles[2].type = ENEMY;
  enemyParticles[2].isAlive = true;
}

void particle_free() {}

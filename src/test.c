#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#define MAX_PARTICLES 500
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
  CHARACTER_PROJECTILE = 4096 * 32,
  BOSS_PROJECTILE = 4096 * 64,
} ParticleType;

#define SINGLE_LINE_PATTERN_COUNT 6
ParticleType singleLinePatterns[6][12] = {
    {GAP, ENEMY, GAP, ENEMY, GAP, ENEMY, GAP, ENEMY, GAP, ENEMY, GAP, ENEMY},
    {GAP, GAP, GAP, ENEMY, GAP, POWERUP, GAP, ENEMY, GAP, GAP},
    {GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP},
    {GAP, GAP, ENEMY, ENEMY, ENEMY, GAP, GAP, ENEMY, ENEMY, ENEMY, GAP},
    {POWERUP, GAP, ENEMY, GAP, GAP, ENEMY, ENEMY, ENEMY, GAP, ENEMY, GAP, GAP},
    {ENEMY, ENEMY, GAP, ENEMY, POWERUP, ENEMY, ENEMY, GAP, ENEMY, POWERUP, GAP,
     ENEMY}};

#define MULTI_LINE_PATTERN_COUNT 4
ParticleType multipleLinePattern[4][3][12] = {
    {{ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY,
      ENEMY, ENEMY},
     {ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, BOSS_EEL, ENEMY, ENEMY, ENEMY, ENEMY,
      ENEMY, ENEMY},
     {ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, GAP, GAP, ENEMY, ENEMY, ENEMY, ENEMY,
      ENEMY}},

    {{ENEMY, ENEMY, ENEMY, ENEMY, BOSS_EEL, ENEMY, ENEMY, BOSS_EEL, ENEMY,
      ENEMY, ENEMY, ENEMY},
     {ENEMY, ENEMY, ENEMY, ENEMY, GAP, ENEMY, ENEMY, GAP, ENEMY, ENEMY, ENEMY,
      ENEMY},
     {ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY,
      ENEMY, ENEMY}},

    {{ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY,
      ENEMY, ENEMY},
     {GAP, GAP, GAP, BOSS_ORCA, GAP, ENEMY, ENEMY, BOSS_ORCA, GAP, GAP, GAP,
      GAP},
     {GAP, GAP, GAP, GAP, GAP, ENEMY, ENEMY, GAP, GAP, GAP, GAP, GAP}},

    {{ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY, ENEMY,
      ENEMY, ENEMY},
     {GAP, GAP, ENEMY, ENEMY, POWERUP, GAP, GAP, POWERUP, ENEMY, ENEMY, GAP,
      GAP},
     {GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP, GAP}},
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

void particle_boss_system_create_particle(ParticleSystem *system,
                                          int particleType, int x);
void particle_power_system_create_particle(ParticleSystem *system,
                                           int particleType, int x);
void particle_enemy_system_create_particle(ParticleSystem *system,
                                           int particleType, int x);
void particle_create_boss(Particle *particle, int particleType, int x);
void particle_create_powerup(Particle *particle, int particleType, int x);
void particle_create_enemy(Particle *particle, int particleType, int x);
void particle_queue_pattern(ParticleSystem *powerup, ParticleSystem *enemy,
                            ParticleSystem *boss, unsigned long frameCount);
int interval(unsigned long frameCount, const int fps);
void parse_input(double *shark_acceleration);
void draw_character(unsigned long frameCount);
void healthBar(int health);
void particle_spawn_projectiles(ParticleSystem *system,
                                unsigned long frameCount);
void player_particle_collision(ParticleSystem *powerup, ParticleSystem *enemy,
                               ParticleSystem *boss);
void player_projectile_collision(ParticleSystem *projectiles,
                                 ParticleSystem *powerup, ParticleSystem *enemy,
                                 ParticleSystem *boss);

void background(unsigned long frameCount);
void powerup_particle_update_animation(ParticleSystem *system,
                                       unsigned long count);
void powerup_particle_draw_system(ParticleSystem *system);

Texture2D sand;
Texture2D tiki;
Texture2D palm;
Texture2D rock;
Particle shark;
Particle shark_projectile;
double projectile_interval = 1.0; // in seconds
int num_shark_projectiles = 0;
double shark_acceleration = 0;
unsigned long score = 0;
Texture2D hearts;
Texture2D half;
Texture2D empty;

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sandy Shore Tech Demo 4");
  InitAudioDevice();
  SetTargetFPS(60);
  particle_init();
  ParticleSystem *enemies = particle_system_init(20);
  ParticleSystem *bosses = particle_system_init(5);
  ParticleSystem *powerups = particle_system_init(10);
  ParticleSystem *character_projectiles = particle_system_init(1);
  unsigned long count = 0;
  Music bgMusic = LoadMusicStream("../src/assets/sounds/background_music.mp3");
  PlayMusicStream(bgMusic);
  SetMusicVolume(bgMusic, 0.5f);

  while (!WindowShouldClose()) {
    if (IsCursorOnScreen()) {
      DisableCursor();
    }

    count++;
    if (count % 20 == 0) {
      score++;
    }
    particle_queue_pattern(powerups, enemies, bosses, count);

    particle_update_system(enemies);
    particle_update_animation(enemies, count);

    particle_update_system(bosses);
    particle_update_animation(bosses, count);

    particle_update_system(powerups);
    powerup_particle_update_animation(powerups, count);

    particle_spawn_projectiles(character_projectiles, count);
    particle_update_system(character_projectiles);

    parse_input(&shark_acceleration);
    player_particle_collision(powerups, enemies, bosses);
    player_projectile_collision(character_projectiles, powerups, enemies,
                                bosses);

    UpdateMusicStream(bgMusic);

    BeginDrawing();
    ClearBackground(RAYWHITE);
    background(count);
    particle_draw_system(enemies);
    particle_draw_system(bosses);
    powerup_particle_draw_system(powerups);
    particle_draw_system(character_projectiles);
    healthBar(shark.health);
    DrawRectangle(SCREEN_WIDTH - ((int)log10(score)) * 10 - 110, 0,
                  SCREEN_WIDTH, 40, WHITE);
    DrawText(TextFormat("Score: %d", score),
             SCREEN_WIDTH - ((int)log10(score)) * 10 + 10 - 110, 10, 20, BLACK);
    // draw player
    // call the function to draw the score and lives
    draw_character(count);
    EndDrawing();
  }

  UnloadMusicStream(bgMusic);
  CloseWindow();
  particle_system_free(enemies);
  particle_system_free(powerups);
  particle_system_free(bosses);
  particle_system_free(character_projectiles);
  particle_free();

  return 0;
}

void parse_input(double *acceleration) {
  int lastKey = GetKeyPressed();
  bool left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
  bool right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
  if (right && shark.x + 1 + (20 * *acceleration) < 482 && !left) {
    if (*acceleration < 0) {
      *acceleration = 0;
    }
    shark.x += 1 + (20 * *acceleration);
    *acceleration += 0.005;
  } else if (left && shark.x + -1 + (20 * *acceleration) > 130 && !right) {
    if (*acceleration > 0) {
      *acceleration = 0;
    }
    shark.x += -1 + (20 * *acceleration);
    *acceleration -= 0.005;
  } else {
    *acceleration = 0;
  }
}

void draw_character(unsigned long frameCount) {
  if (frameCount % 15 == 0) {
    shark.frameNumber += 1;
  }
  int frame = shark.frameNumber % shark.numberOfFrames;
  Rectangle source = {frame * shark.frameWidth, 0, shark.frameWidth,
                      shark.frameHeight};
  Vector2 position = {shark.x, shark.y};
  DrawTextureRec(shark.image, source, position, WHITE);
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

void particle_spawn_projectiles(ParticleSystem *system,
                                unsigned long frameCount) {
  if (frameCount % (int)(projectile_interval * 60) == 0) {
    int index = 0;
    while (index < MAX_PARTICLES) {
      if (!system->particles[index].isAlive) {
        break;
      }
      index++;
    }
    Particle *projectile = &system->particles[index];
    memcpy(projectile, &shark_projectile, sizeof(Particle));
    projectile->x =
        shark.x + (shark.frameWidth / 2 - projectile->frameWidth / 2);
    projectile->y = shark.y - shark_projectile.frameHeight;
  }
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

void powerup_particle_draw_system(ParticleSystem *system) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (system->particles[i].isAlive) {
      // printf("DRAW POWER UP FOR TYPE %d\n", system->particles[i].type);
      if ((system->particles[i].type & BOX) == BOX) {
        if (system->particles[i].health >= 2) {
          printf("DRAW BOX\n");
          powerupParticles[0].frameNumber = 0;
          powerupParticles[0].x = system->particles[i].x;
          powerupParticles[0].y = system->particles[i].y;
          particle_draw(&powerupParticles[0]);
        } else if (system->particles[i].health == 1) {
          printf("DRAW BROKEN BOX\n");
          powerupParticles[0].frameNumber = 1;
          powerupParticles[0].x = system->particles[i].x;
          powerupParticles[0].y = system->particles[i].y;
          particle_draw(&powerupParticles[0]);
        }
      } else {
        //	    printf("DRAW POWERUP\n");
        particle_draw(&system->particles[i]);
      }
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
    if (particle->health <= 0 && (particle->type < 1 || particle->type > 48)) {
      particle->isAlive = false;
    }

    if (particle->y > SCREEN_HEIGHT + particle->h) {
      particle->isAlive = false;
    } else if (particle->type == PROJECTILE &&
               particle->y + particle->frameHeight < 0) {
      particle->isAlive = false;
    } else {
      particle->y += particle->dy;
    }
  }
}

void particle_create_enemy(Particle *particle, int particleType, int x) {
  memcpy(particle, &enemyParticles[particleType], sizeof(Particle));
  particle->x = x;
  particle->y = -particle->frameHeight;
  particle->isAlive = true;
}

void particle_create_powerup(Particle *particle, int particleType, int x) {
  memcpy(particle, &powerupParticles[particleType], sizeof(Particle));
  particle->x = x;
  particle->y = -particle->frameHeight;
  particle->isAlive = true;
}

void particle_create_boss(Particle *particle, int particleType, int x) {
  memcpy(particle, &bossParticles[particleType], sizeof(Particle));
  particle->x = x;
  particle->y = -particle->frameHeight;
  particle->isAlive = true;
}

void particle_enemy_system_create_particle(ParticleSystem *system,
                                           int particleType, int x) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!system->particles[i].isAlive) {
      particle_create_enemy(&system->particles[i], particleType, x);
      return;
    }
  }
}

void particle_power_system_create_particle(ParticleSystem *system,
                                           int particleType, int x) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!system->particles[i].isAlive) {
      particle_create_powerup(&system->particles[i], particleType, x);
      return;
    }
  }
}

void particle_boss_system_create_particle(ParticleSystem *system,
                                          int particleType, int x) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (!system->particles[i].isAlive) {
      particle_create_boss(&system->particles[i], particleType, x);
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
  powerupParticles[0].health = 2;
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
  powerupParticles[1].health = 2;
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
  powerupParticles[2].health = 2;
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
  powerupParticles[3].health = 2;
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
  powerupParticles[4].health = 2;
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
  powerupParticles[5].health = 2;
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
  enemyParticles[0].health = 1;
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
  enemyParticles[1].health = 2;
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
  enemyParticles[2].health = 3;
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
  enemyParticles[3].health = 3;
  enemyParticles[3].type = ENEMY;
  enemyParticles[3].isAlive = true;

  enemyParticles[4].dx = 0;
  enemyParticles[4].dy = 1;
  enemyParticles[4].image = LoadTexture("../src/assets/images/oilspill.png");
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
  bossParticles[0].health = 8;
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
  bossParticles[1].health = 10;
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
  bossParticles[2].health = 12;
  bossParticles[2].type = ENEMY;
  bossParticles[2].isAlive = true;

  sand = LoadTexture("../src/assets/images/sand.png");
  tiki = LoadTexture("../src/assets/images/tiki.png");
  palm = LoadTexture("../src/assets/images/palmtree.png");
  rock = LoadTexture("../src/assets/images/rock.png");
  shark.image = LoadTexture("../src/assets/images/shark.png");
  shark.isAlive = true;
  shark.frameNumber = 0;
  shark.frameWidth = 30;
  shark.frameHeight = 80;
  shark.x = SCREEN_WIDTH / 2 - shark.frameWidth / 2;
  shark.y = SCREEN_HEIGHT - shark.frameHeight - 20;
  shark.numberOfFrames = 4;
  shark.health = 6;

  shark_projectile.image = LoadTexture("../src/assets/images/harpoon.png");
  shark_projectile.isAlive = true;
  shark_projectile.frameWidth = 15;
  shark_projectile.frameHeight = 39;
  shark_projectile.frameNumber = 1;
  shark_projectile.numberOfFrames = 1;
  shark_projectile.health = 1;
  shark_projectile.dy = -2;
  shark_projectile.type = PROJECTILE;
  hearts = LoadTexture("../src/assets/images/heart1.png");
  half = LoadTexture("../src/assets/images/heart2.png");
  empty = LoadTexture("../src/assets/images/heart3.png");
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

void powerup_particle_update_animation(ParticleSystem *system,
                                       unsigned long count) {
  Particle *p;
  for (int i = 0; i < MAX_PARTICLES; i++) {
    p = &system->particles[i];
    if ((p->type & BOX) == BOX) {
      if (p->health >= 2) {
        // Draw unbroken box
        printf("ANIMATION UPDATE BOX\n");
        p->frameNumber = 0;
      } else if (p->health == 1) {
        // Draw broken box
        printf("ANIMATION UPDATE BROKEN BOX\n");
        p->frameNumber = 1;
      } else {
        // Remove the box 'property'
        printf("ANIMATION UPDATE CONVERT TO POWER UP\n");
        p->type = BOX ^ p->type;
      }
    } else {
      particle_animate(p, count);
    }
  }
}

unsigned long nextPatternTime = 0;
unsigned int lastInterval = 0;
void particle_queue_pattern(ParticleSystem *powerup, ParticleSystem *enemy,
                            ParticleSystem *boss, unsigned long frameCount) {
  if (frameCount < nextPatternTime) {
    return;
  }
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
  if (lastInterval == interval(frameCount, 60)) {
    int r = GetRandomValue(0, SINGLE_LINE_PATTERN_COUNT - 1);
    for (int i = 4; i < 12 + 4; i++) {
      switch ((int)singleLinePatterns[r][i]) {
      case ENEMY:
        int e = GetRandomValue(0, 4);
        particle_enemy_system_create_particle(enemy, e, i * 32);
        break;
      case POWERUP:
        printf("SPAWNING POWER\n");
        int f = GetRandomValue(1, 5);
        particle_power_system_create_particle(powerup, f, i * 32);
        break;
      }
    }
  } else {
    int r = GetRandomValue(0, MULTI_LINE_PATTERN_COUNT - 1);
    for (int j = 0; j < 3; j++) {
      for (int i = 4; i < 12 + 4; i++) {
        switch ((int)multipleLinePattern[r][j][i]) {
        case ENEMY:
          int e = GetRandomValue(0, 4);
          particle_enemy_system_create_particle(enemy, e, i * 32);
          break;
        case POWERUP:
          int f = GetRandomValue(1, 5);
          particle_power_system_create_particle(powerup, f, i * 32);
          break;
        case BOSS_ORCA:
          particle_boss_system_create_particle(boss, 0, i * 32);
          break;
        case BOSS_EEL:
          particle_boss_system_create_particle(boss, 1, i * 32);
          break;
        case BOSS_KRAKEN:
          particle_boss_system_create_particle(boss, 2, i * 32);
          break;
        }
      }
    }
  }
  // Do I generate a single line or multiple pattern
  // Which patterd do I choose:

  lastInterval = interval(frameCount, 60);
  nextPatternTime = frameCount + lastInterval * 60;
}

int interval(unsigned long frameCount, const int fps) {
  int time = frameCount / fps;
  if (time < 30) {
    score++;
    return 6;
  } else if (time < 75) {
    score += 2;
    return 4;
  } else if (time < 150) {
    score += 3;
    return 2;
  } else if (time < 240) {
    score += 4;
    return 1;
  } else {
    score += 5;
    return 0;
  }
}

void background(unsigned long frameCount) {
  // sand
  for (int i = -32 + frameCount % 32; i < SCREEN_HEIGHT; i += 32) {
    for (int j = 0; j < SCREEN_WIDTH; j += 32) {
      DrawTexture(sand, j, i, WHITE);
    }
  }
  // rock
  for (int i = -32 + frameCount % 32; i < SCREEN_HEIGHT; i += 32) {
    DrawTexture(rock, 32 * 3, i, WHITE);
    DrawTexture(rock, SCREEN_WIDTH - 32 * 4, i, WHITE);
  }

  // trees
  Rectangle r = {32 * (frameCount / 10 % 5), 0, 32, 64};
  Vector2 p;
  for (int i = -128 + frameCount % 128; i < SCREEN_HEIGHT; i += 128) {
    p.x = 0;
    p.y = i;
    DrawTextureRec(palm, r, p, WHITE);
    p.x = SCREEN_WIDTH - 32;
    DrawTextureRec(palm, r, p, WHITE);
  }

  // tiki
  r.x = 32 * (frameCount / 5 % 5);
  for (int i = -64 + frameCount % 128; i < SCREEN_HEIGHT; i += 128) {
    p.y = i;
    p.x = 32;
    DrawTextureRec(tiki, r, p, WHITE);
    p.x = SCREEN_WIDTH - 32 * 2;
    DrawTextureRec(tiki, r, p, WHITE);
  }
  r.x = 32 * (frameCount / 5 % 5);
  for (int i = -128 + frameCount % 128; i < SCREEN_HEIGHT; i += 128) {
    p.x = 32 * 2;
    p.y = i;
    DrawTextureRec(tiki, r, p, WHITE);
    p.x = SCREEN_WIDTH - 32 * 3;
    DrawTextureRec(tiki, r, p, WHITE);
  }
}

void healthBar(int health) {
  // 3 hearts with half hearts
  // draw hearts
  if (health <= 0) {
    CloseWindow();
  }
  int lost = 6 - health;
  if (health % 2 == 0) {
    for (int i = 0; i < health / 2; i++) {
      DrawTexture(hearts, (i * 32) + 10, 0, WHITE);
    }
  } else {
    for (int i = 0; i < health / 2; i++) {
      DrawTexture(hearts, (i * 32) + 10, 0, WHITE);
    }
    DrawTexture(half, (health / 2) * 32 + 10, 0, WHITE);
  }

  if (lost % 2 == 0) {
    for (int i = 0; i < lost / 2; i++) {
      DrawTexture(empty, (health / 2 + i) * 32 + 10, 0, WHITE);
    }
  } else {
    for (int i = 0; i < lost / 2; i++) {
      DrawTexture(empty, (health / 2 + i + 1) * 32 + 10, 0, WHITE);
    }
  }
}

void player_particle_collision(ParticleSystem *powerup, ParticleSystem *enemy,
                               ParticleSystem *boss) {
  // find if particles are above 880px
  // check if each particle is colliding with charecter
  // if they are, delete particle and take away one life
  // if not, do nothing
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (enemy->particles[i].y > 800) {

      Rectangle enemyTmp = (Rectangle){
          enemy->particles[i].x, enemy->particles[i].y,
          enemy->particles[i].frameWidth, enemy->particles[i].frameHeight};

      Rectangle sharkTmp = {shark.x, shark.y, shark.frameWidth,
                            shark.frameHeight};
      if (CheckCollisionRecs(sharkTmp, enemyTmp) &&
          enemy->particles[i].isAlive == true) {
        enemyTmp = (Rectangle){0, 0, 0, 0};
        enemy->particles[i].isAlive = false;
        shark.health -= 1;
      }
    }

    // player and powerup collision
    if (powerup->particles[i].y > 800) {

      Rectangle powerupTemp = (Rectangle){
          powerup->particles[i].x, powerup->particles[i].y,
          powerup->particles[i].frameWidth, powerup->particles[i].frameHeight};

      Rectangle sharkTemp = {shark.x, shark.y, shark.frameWidth,
                             shark.frameHeight};
      if (CheckCollisionRecs(sharkTemp, powerupTemp) &&
          powerup->particles[i].isAlive == true) {
        powerup->particles[i].isAlive = false;
        // add player powerup ability
      }
    }
  }
}

void player_projectile_collision(ParticleSystem *projectile,
                                 ParticleSystem *powerup, ParticleSystem *enemy,
                                 ParticleSystem *boss) {
  for (int i = 0; i < 14; i++) {
    if (projectile->particles[i].isAlive) {
      for (int j = 0; j < MAX_PARTICLES; j++) {
        Rectangle enemyTmp = (Rectangle){
            enemy->particles[j].x, enemy->particles[j].y,
            enemy->particles[j].frameWidth, enemy->particles[j].frameHeight};

        Rectangle projectileTmp = {projectile->particles[i].x,
                                   projectile->particles[i].y,
                                   projectile->particles[i].frameWidth * 2,
                                   projectile->particles[i].frameHeight};
        if (CheckCollisionRecs(projectileTmp, enemyTmp) &&
            enemy->particles[j].isAlive == true) {
          enemyTmp = (Rectangle){0, 0, 0, 0};
          projectile->particles[i].isAlive = false;
          enemy->particles[j].health -= 1;
        }

        if (powerup->particles[j].isAlive &&
            (powerup->particles[j].type & BOX) == BOX) {
          // Check for box collision
          Rectangle powerupTmp =
              (Rectangle){powerup->particles[j].x, powerup->particles[j].y,
                          powerup->particles[j].frameWidth,
                          powerup->particles[j].frameHeight};
          if (CheckCollisionRecs(projectileTmp, powerupTmp)) {
            powerup->particles[j].health--;
          }
        }
      }
    }
  }
}

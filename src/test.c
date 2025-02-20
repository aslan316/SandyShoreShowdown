#include "raylib.h"
#define MAX_PARTICLES 50
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 960

typedef enum{
	ENEMY=0,
	POWER_UP_HEALTH=1,
	POWER_UP_INVIS=2,
	POWER_UP_DOUBLE_FIRE_DAMAGE=4,
	POWER_UP_DOUBLE_ENEMY_DAMAGE=8,
	POWER_UP_SPREAD=16,
	BOX=32
} ParticleType;

typedef struct {
	int x;
	int y;
	int dy;
	int dx;
	Color color;
	int w;
	int h;
	int health;
	ParticleType type;
	bool isAlive;
}Particle;

typedef struct{
	Particle particles[MAX_PARTICLES];
	int speed;
	int count;
}ParticleSystem;

ParticleSystem* particle_system_init(int speed);
void particle_system_free(ParticleSystem* system);
void particle_draw(Particle* particle);
void particle_draw_system(ParticleSystem* system);
void particle_update_system(ParticleSystem* system);
void particle_update(Particle* particle);
void particle_create(Particle* particle);
void particle_system_create_particle(ParticleSystem* system);

int main(void){

	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT, "Sandy Shore Tech Demo 1");
	SetTargetFPS(60);
	ParticleSystem* system=particle_system_init(20);
	
	while(!WindowShouldClose()){

		if(GetRandomValue(0,100) > 80){
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
	return 0;
}

ParticleSystem* particle_system_init(int speed){
	ParticleSystem* system = MemAlloc(sizeof(ParticleSystem));
	system->speed=speed;
	system->count=0;
	for(int i=0; i < MAX_PARTICLES; i++){
		system->particles[i].isAlive = false;
	}
	return system;
}

void particle_system_free(ParticleSystem* system){
	MemFree(system);
}

void particle_draw(Particle* particle){
	DrawRectangle(particle->x, particle->y, particle->w, particle->h, particle->color);
}

void particle_draw_system(ParticleSystem* system){
	for(int i=0; i<MAX_PARTICLES; i++){
		if(system->particles[i].isAlive){
			particle_draw(&system->particles[i]);
		}
	}
}

void particle_update_system(ParticleSystem* system){
	for(int i=0; i<MAX_PARTICLES; i++){
		particle_update(&system->particles[i]);
	}
}

void particle_update(Particle* particle){
	if(particle->isAlive){
		if(particle->y > SCREEN_HEIGHT + particle->h){
			particle->isAlive = false;
		}else{
			particle->y += particle->dy;
		}
	}
}

void particle_create(Particle* particle){
	particle->x = GetRandomValue(0, SCREEN_WIDTH);
	particle->y =0;
	particle->dy=1;
	particle->w=32;
	particle->h=32;
	particle->type = (ParticleType)GetRandomValue(0, 6);
  particle->isAlive = true;
	if(particle->type != ENEMY){
		particle->type = (ParticleType)(particle->type | BOX);
		particle->color = GREEN;
	}else{
		particle->color = RED;
	}
}
void particle_system_create_particle(ParticleSystem* system){
	for(int i=0; i < MAX_PARTICLES; i++){
		if(!system->particles[i].isAlive){
			particle_create(&system->particles[i]);
			return;
		}
	}
}

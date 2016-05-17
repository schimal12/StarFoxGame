//Erick Alan Cervantes A01215567
//Sebastian Chimal Montes de Oca A01214839

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define WORLD_SIZE 5
#define ENEMY_RATE 1000
#define POWER_RATE 6000
#define ENEMY_SHOOT_RATE 3000
#define ENEMY_SHIELD 5
#define FIRE_RATE 150
#define ENEMY_TYPES 2
#define POWER_TYPES 2
#define PLAYER_AMMUNITION 60
#define _USE_MATH_DEFINES
#define DIFFICULTY_SCORE 10
#define DEBUG false
#define SPACEBAR 32

#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <cmath>
#include <math.h>
#include <stdlib.h>
#include "SOIL.h"
#include "glm.h"
#include <time.h>
#include <algorithm>
#include "irrKlang.h"
using namespace std;
#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;

#ifdef _WIN32
#include "glut.h"
#elif __APPLE__
#include <GLUT/GLUT.h>
#endif

void Init();
void Display();
void Reshape();

//Entrega 1


typedef struct Context {
    GLboolean AK = false;
    GLboolean WK = false;
    GLboolean SK = false;
    GLboolean DK = false;
    GLboolean FIRE = false;
    GLfloat SENSITIVITY = 6;
    GLfloat T_FIRE = FIRE_RATE;
} Context;



typedef struct State {
    GLfloat *T;
    GLfloat *R;
} State;


typedef struct ParallaxLayer {
    char* texture;
    GLint texID;
    GLdouble pace;
    GLdouble duration;
} ParallaxLayer;

typedef struct Parallax {
    ParallaxLayer* layers;
    GLint size;
} Parallax;

typedef struct Weapon {
    State state;
    GLint ammunition = PLAYER_AMMUNITION;
    GLint magazine = PLAYER_AMMUNITION;
    GLint rate;
} Weapon;

typedef struct Animation {
    GLfloat *start;
    GLfloat *end;
};

typedef struct Bullet {
    State state;
    Animation T;
    Animation R;
    GLfloat collider;
    GLfloat frames;
    GLfloat frame = 0;
    GLboolean active = true;
} Bullet;

typedef struct Player {
    Context context;
    State state;
    Weapon weapon;
    char* model;
    GLMmodel* modID;
    GLint shield = 100;
    GLfloat collider = 0.5;
    GLint damage = 2;
} Player;

typedef struct Enemy {
    GLint type = 0;
    State state;
    Animation T;
    GLMmodel* modID;
    GLfloat collider;
    GLfloat frames;
    GLfloat frame = 0;
    GLfloat shootframe = 0;
    GLint shield = ENEMY_SHIELD;
    GLboolean active = true;
    GLboolean crashed = false;
    GLint damage;
}Enemigo;

typedef struct PowerUp {
    GLint type = 0;
    State state;
    Animation T;
    GLfloat collider;
    GLfloat frames;
    GLfloat frame = 0;
    GLboolean active = true;
    GLint quantity = 0;
};

typedef struct World {
    Enemy *enemies;
    GLint enemiesN = 0;
    Bullet *bullets;
    GLint bulletsN = 0;
    PowerUp *powers;
    GLint powersN = 0;
    Bullet *enmbullets;
    GLint ebulletsN = 0;
    GLfloat enemyTime = ENEMY_RATE;
    GLfloat shootTime = ENEMY_SHOOT_RATE;
    GLfloat powerupTime = 0.0;
    GLint score = 0;
    GLint level = 1;
};

typedef struct Config {
    Enemy *enemyTypes = (Enemy *)malloc(sizeof(Enemy)*ENEMY_TYPES);
    PowerUp *powerTypes = (PowerUp *)malloc(sizeof(PowerUp)*POWER_TYPES);
    GLfloat enemySpeed = 7000.0;
    GLfloat enemyRate = ENEMY_RATE;
    GLfloat enemyShootRate = ENEMY_SHOOT_RATE;
};

Parallax parallax;
Player player;
World world;
Config config;

GLfloat lightT1P[] = { 0.0,0.0,WORLD_SIZE*5 };
GLfloat lightT2P[] = { -WORLD_SIZE,WORLD_SIZE,WORLD_SIZE * 5 };
GLfloat lightT3P[] = { WORLD_SIZE,-WORLD_SIZE,WORLD_SIZE * 5 };
GLfloat lightT4P[] = { WORLD_SIZE,WORLD_SIZE,WORLD_SIZE * 5 };
GLfloat lightT5P[] = { -WORLD_SIZE,-WORLD_SIZE,WORLD_SIZE * 5 };

GLfloat MOVEX = 0.0;
GLfloat MOVEY = 0.0;

GLfloat white[] = { 1.0,1.0,1.0,1.0 };
GLfloat dgray[] = { 0.1,0.1,0.1,1.0 };

GLfloat colorT1[] = { 0.13,0.59,0.95 };
GLfloat colorT1C[] = { 0.49,0.54,0.96 };

GLdouble deltaTime = 0;
GLdouble oldTime = 0;

GLuint B;

GLint powerup = 0;
GLint estado = 0;

GLboolean arriba = false;
GLint puntuacionTotal = 0;
GLint maximoGolpes = 5;

bool niveles[5] = {false};
bool textoNiveles[6] = {false};

char* nivel1 = "Nivel 1";
char* nivel2 = "Nivel 2";
char* nivel3 = "Nivel 3";
char* nivel4 = "Nivel 4";
char* nivel5 = "Nivel Final";

void initParallaxFrames() {
    GLint FRAMES = 3;
    parallax.layers = (ParallaxLayer *)malloc(sizeof(ParallaxLayer)*FRAMES);
    parallax.size = FRAMES;
    
    ParallaxLayer layer0;
    char name0[] = "Stars.png";
    layer0.texture = name0;
    layer0.pace = 0.0;
    layer0.texID = -1;
    layer0.duration = 10000.0;
    parallax.layers[0] = layer0;
    
    ParallaxLayer layer1;
    char name1[] = "Transparent.png";
    layer1.texture = name1;
    layer1.pace = 0.0;
    layer1.texID = -1;
    layer1.duration = 10000.0 / 2;
    parallax.layers[1] = layer1;
    
    ParallaxLayer layer2;
    char name2[] = "Transparent.png";
    layer2.texture = name2;
    layer2.pace = 0.0;
    layer2.texID = -1;
    layer2.duration = 10000.0 / 4;
    parallax.layers[2] = layer2;
    
    
    for (int cont = 0; cont < FRAMES; cont += 1) {
        parallax.layers[cont].texID = SOIL_load_OGL_texture(
                                                            parallax.layers[cont].texture,
                                                            SOIL_LOAD_AUTO,
                                                            SOIL_CREATE_NEW_ID,
                                                            SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                                                            );
    }
}



void loadModels() {
    char nameP[] = "./Models/Arwing/a2.obj";
    player.model = nameP;
    player.modID = glmReadOBJ(player.model);
    
    char nameE[] = "./Models/Rock/r2.obj";
    config.enemyTypes[0].type = 0;
    config.enemyTypes[0].modID = glmReadOBJ(nameE);
    config.enemyTypes[0].collider = 0.6;
    config.enemyTypes[0].shield = 10;
    config.enemyTypes[0].damage = 20;
    
    char nameS[] = "./Models/Dragon/Dragon.obj";
    config.enemyTypes[1].type = 1;
    config.enemyTypes[1].modID = glmReadOBJ(nameS);
    config.enemyTypes[1].collider = 0.4;
    config.enemyTypes[1].shield = 5;
    config.enemyTypes[1].damage = 10;
    
    config.powerTypes[0].type = 0;
    config.powerTypes[0].collider = 0.4;
    config.powerTypes[0].quantity = 20;
    
    config.powerTypes[1].type = 1;
    config.powerTypes[1].collider = 0.4;
    config.powerTypes[1].quantity = 15;
}

void initPlayer() {
    player.state.T = (GLfloat *)malloc(sizeof(GLfloat) * 3);
    player.state.T[0] = -(WORLD_SIZE - (-WORLD_SIZE + player.collider * 3.5));
    player.state.T[1] = 0.0;
    player.state.T[2] = -0.5;
    
    player.state.R = (GLfloat *)malloc(sizeof(GLfloat) * 3);
    player.state.R[0] = 0.0;
    player.state.R[1] = 90.0;
    player.state.R[2] = 0.0;
    
    player.weapon.state.T = (GLfloat *)malloc(sizeof(GLfloat) * 3);
    player.weapon.state.T[0] = 0.6;
    player.weapon.state.T[1] = -0.13;
    player.weapon.state.T[2] = 0.0;
    
    player.weapon.rate = 10;
    player.shield = 100;
    player.damage = 2;
}

void Init()
{
    
    ISoundEngine* starfox1 = createIrrKlangDevice();
    
    starfox1->play2D("starfox.ogg", true);
    
    glClearColor(0.007, 0.003, 0.07, 1);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    initParallaxFrames();
    loadModels();
    
    initPlayer();
    
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    glLightfv(GL_LIGHT0, GL_POSITION, lightT1P);
    glLightfv(GL_LIGHT0, GL_AMBIENT, white);
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    glLightfv(GL_LIGHT1, GL_POSITION, lightT2P);
    glLightfv(GL_LIGHT1, GL_AMBIENT, white);
    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT2, GL_SPECULAR, white);
    glLightfv(GL_LIGHT2, GL_POSITION, lightT3P);
    glLightfv(GL_LIGHT2, GL_AMBIENT, white);
    glEnable(GL_LIGHT3);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT3, GL_SPECULAR, white);
    glLightfv(GL_LIGHT3, GL_POSITION, lightT4P);
    glLightfv(GL_LIGHT3, GL_AMBIENT, white);
    glEnable(GL_LIGHT4);
    glLightfv(GL_LIGHT4, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT4, GL_SPECULAR, white);
    glLightfv(GL_LIGHT4, GL_POSITION, lightT5P);
    glLightfv(GL_LIGHT4, GL_AMBIENT, white);
}

GLdouble* toWorldCoordinates(int x, int y) {
    GLdouble* res = (GLdouble *)malloc(sizeof(GLdouble) * 3);
    res[0] = ((GLdouble)(x - 250) / 250) + (-1)*MOVEX;
    res[1] = ((GLdouble)(-1)*(y - 250) / 250) + MOVEY;
    res[2] = 0;
    return res;
}

int* screenCoordinate(GLfloat* point) {
    int* res = (int *)malloc(sizeof(int) * 2);
    GLdouble TEMPX = point[0] / WORLD_SIZE;
    GLdouble TEMPY = point[1] / WORLD_SIZE;
    int WIDTH = WINDOW_WIDTH / 2;
    int HEIGHT = WINDOW_HEIGHT / 2;
    
    
    res[0] = (TEMPX * WIDTH) + WIDTH;
    res[1] = ((1 - TEMPY) * HEIGHT);
    
    return res;
}

GLfloat* getDirectionVector() {
    GLfloat* vector = (GLfloat *)malloc(sizeof(GLfloat) * 2);
    vector[0] = 0.0;
    vector[1] = 0.0;
    
    if (player.context.AK) {
        vector[0] -= 1.0;
    }
    if (player.context.WK) {
        vector[1] += 1.0;
    }
    if (player.context.SK) {
        vector[1] -= 1.0;
    }
    if (player.context.DK) {
        vector[0] += 1.0;
    }
    
    GLfloat	vecm = sqrt(vector[0] * vector[0] + vector[1] * vector[1]);
    if (vecm != 0.0) {
        vector[0] /= vecm;
        vector[1] /= vecm;
        
    }
    GLfloat rate = player.context.SENSITIVITY*deltaTime*0.001;
    vector[0] *= rate;
    vector[1] *= rate;
    
    return vector;
}

void updateEnemies(bool add) {
    Enemy *list = (Enemy *)malloc(sizeof(Enemy)*(world.enemiesN));
    GLint activeEnemies = 0;
    for (int cont = 0; cont < world.enemiesN; cont += 1) {
        if (world.enemies[cont].state.T[0] > (-WORLD_SIZE) && world.enemies[cont].active) {
            activeEnemies += 1;
        }
        else {
            world.enemies[cont].active = false;
            //cout << "Borrando Enemigo " << cont << " " << world.enemies[cont].state.T[0] <<"\n";
        }
        list[cont] = world.enemies[cont];
    }
    
    free(world.enemies);
    if (add) {
        world.enemies = (Enemy *)malloc(sizeof(Enemy)*(activeEnemies + 1));
    }
    else {
        world.enemies = (Enemy *)malloc(sizeof(Enemy)*(activeEnemies));
    }
    
    GLint con = 0;
    for (int cont = 0; cont < world.enemiesN; cont += 1) {
        if (list[cont].active) {
            world.enemies[con] = list[cont];
            con += 1;
        }
    }
    free(list);
    
    world.enemiesN = activeEnemies;
}

void generateEnemies() {
    updateEnemies(false);
    if (world.enemyTime >= config.enemyRate) {
        updateEnemies(true);
        srand(glutGet(GLUT_ELAPSED_TIME));
        GLint type = (rand() % ENEMY_TYPES);
        
        srand(glutGet(GLUT_ELAPSED_TIME));
        GLfloat zone = (rand() % (WORLD_SIZE * 2)) + 1;
        
        
        world.enemies[world.enemiesN].collider = config.enemyTypes[type].collider;
        world.enemies[world.enemiesN].modID = config.enemyTypes[type].modID;
        world.enemies[world.enemiesN].active = true;
        world.enemies[world.enemiesN].frame = 0.0;
        world.enemies[world.enemiesN].shootframe = ENEMY_SHOOT_RATE;
        world.enemies[world.enemiesN].type = type;
        world.enemies[world.enemiesN].shield = config.enemyTypes[type].shield;
        world.enemies[world.enemiesN].damage = config.enemyTypes[type].damage;
        world.enemies[world.enemiesN].crashed = false;
        
        world.enemies[world.enemiesN].T.start = (GLfloat *)malloc(sizeof(GLfloat) * 3);
        world.enemies[world.enemiesN].T.start[0] = WORLD_SIZE;
        world.enemies[world.enemiesN].T.start[1] = (((zone / (WORLD_SIZE * 2)) - (WORLD_SIZE / 10.0)) * 2.0 * (WORLD_SIZE - 1)) - config.enemyTypes[type].collider*0.7;
        world.enemies[world.enemiesN].T.start[2] = -0.5;
        
        world.enemies[world.enemiesN].state.T = (GLfloat *)malloc(sizeof(GLfloat) * 3);
        world.enemies[world.enemiesN].state.T[0] = world.enemies[world.enemiesN].T.start[0];
        world.enemies[world.enemiesN].state.T[1] = world.enemies[world.enemiesN].T.start[1];
        world.enemies[world.enemiesN].state.T[2] = world.enemies[world.enemiesN].T.start[2];
        
        switch(type){
            case 0:
                world.enemies[world.enemiesN].frames = config.enemySpeed;
                zone = (rand() % (WORLD_SIZE * 2)) + 1;
                
                world.enemies[world.enemiesN].T.end = (GLfloat *)malloc(sizeof(GLfloat) * 3);
                world.enemies[world.enemiesN].T.end[0] = -WORLD_SIZE;
                world.enemies[world.enemiesN].T.end[1] = (((zone / (WORLD_SIZE * 2)) - (WORLD_SIZE / 10.0)) * 2.0 * (WORLD_SIZE - 1)) - config.enemyTypes[type].collider*0.7;
                world.enemies[world.enemiesN].T.end[2] = -0.5;
                
                world.enemies[world.enemiesN].state.R = (GLfloat *)malloc(sizeof(GLfloat));
                world.enemies[world.enemiesN].state.R[0] = 0.0;
                break;
            case 1:
                GLfloat speed = (rand() % (101));
                world.enemies[world.enemiesN].frames = config.enemySpeed - 2000.0*(speed/100);
                
                world.enemies[world.enemiesN].T.end = (GLfloat *)malloc(sizeof(GLfloat) * 3);
                world.enemies[world.enemiesN].T.end[0] = -WORLD_SIZE;
                world.enemies[world.enemiesN].T.end[1] = world.enemies[world.enemiesN].T.start[1];
                world.enemies[world.enemiesN].T.end[2] = -0.5;
                
                world.enemies[world.enemiesN].state.R = (GLfloat *)malloc(sizeof(GLfloat));
                world.enemies[world.enemiesN].state.R[0] = -90.0;
                break;
        }
        
        
        world.enemiesN += 1;
        //printf("Enemies: %d\n",world.enemiesN);
        
        world.enemyTime = 0.0;
    } else {
        world.enemyTime += deltaTime;
    }
}


void updatePowerUps(bool add) {
    PowerUp *list = (PowerUp *)malloc(sizeof(PowerUp)*(world.powersN));
    GLint activePowers = 0;
    for (int cont = 0; cont < world.powersN; cont += 1) {
        if (world.powers[cont].state.T[0] >(-WORLD_SIZE) && world.powers[cont].active) {
            activePowers += 1;
        }
        else {
            world.powers[cont].active = false;
            //cout << "Borrando PowerUp " << cont << " " << world.powers[cont].state.T[0] <<"\n";
        }
        list[cont] = world.powers[cont];
    }
    
    free(world.powers);
    if (add) {
        world.powers = (PowerUp *)malloc(sizeof(PowerUp)*(activePowers + 1));
    }
    else {
        world.powers = (PowerUp *)malloc(sizeof(PowerUp)*(activePowers));
    }
    
    GLint con = 0;
    for (int cont = 0; cont < world.powersN; cont += 1) {
        if (list[cont].active) {
            world.powers[con] = list[cont];
            con += 1;
        }
    }
    free(list);
    
    world.powersN = activePowers;
}

void generatePowerUps() {
    updatePowerUps(false);
    if (world.powerupTime >= POWER_RATE) {
        updatePowerUps(true);
        srand(glutGet(GLUT_ELAPSED_TIME));
        GLint type = (rand() % POWER_TYPES);
        
        srand(glutGet(GLUT_ELAPSED_TIME));
        GLfloat zone = (rand() % (WORLD_SIZE * 2)) + 1;
        
        
        world.powers[world.powersN].collider = config.powerTypes[type].collider;
        world.powers[world.powersN].active = true;
        world.powers[world.powersN].frame = 0.0;
        world.powers[world.powersN].type = type;
        world.powers[world.powersN].quantity = config.powerTypes[type].quantity;
        
        world.powers[world.powersN].T.start = (GLfloat *)malloc(sizeof(GLfloat) * 3);
        world.powers[world.powersN].T.start[0] = WORLD_SIZE;
        world.powers[world.powersN].T.start[1] = (((zone / (WORLD_SIZE * 2)) - (WORLD_SIZE / 10.0)) * 2.0 * (WORLD_SIZE - 1)) - config.powerTypes[type].collider*0.7;
        world.powers[world.powersN].T.start[2] = -0.5;
        
        world.powers[world.powersN].state.T = (GLfloat *)malloc(sizeof(GLfloat) * 3);
        world.powers[world.powersN].state.T[0] = world.powers[world.powersN].T.start[0];
        world.powers[world.powersN].state.T[1] = world.powers[world.powersN].T.start[1];
        world.powers[world.powersN].state.T[2] = world.powers[world.powersN].T.start[2];
        
        
        GLfloat speed = (rand() % (101));
        world.powers[world.powersN].frames = config.enemySpeed + 1000;
        
        world.powers[world.powersN].T.end = (GLfloat *)malloc(sizeof(GLfloat) * 3);
        world.powers[world.powersN].T.end[0] = -WORLD_SIZE;
        world.powers[world.powersN].T.end[1] = world.powers[world.powersN].T.start[1];
        world.powers[world.powersN].T.end[2] = -0.5;
        
        world.powers[world.powersN].state.R = (GLfloat *)malloc(sizeof(GLfloat));
        world.powers[world.powersN].state.R[0] = -90.0;
        
        
        world.powersN += 1;
        world.powerupTime = 0.0;
    }
    else {
        world.powerupTime += deltaTime;
    }
}


GLdouble linearCurve(GLfloat time, GLfloat start, GLfloat end, GLfloat duration) {
    return (end - start)*(time) / duration + start;
}

void drawCollisionSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat size) {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(x, y, z);
    glRotated(90, 0, 1, 0);
    glutWireSphere(size, 10, 10);
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void drawPowerUps() {
    
    for (int cont = 0; cont < world.powersN; cont += 1) {
        glEnable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        world.powers[cont].state.T[0] = linearCurve(world.powers[cont].frame, world.powers[cont].T.start[0], world.powers[cont].T.end[0], world.powers[cont].frames);
        world.powers[cont].state.T[1] = linearCurve(world.powers[cont].frame, world.powers[cont].T.start[1], world.powers[cont].T.end[1], world.powers[cont].frames);
        glTranslated(world.powers[cont].state.T[0], world.powers[cont].state.T[1], world.powers[cont].state.T[2]);
        glRotated(world.powers[cont].state.R[0], 0, 1, 0);
        if (world.powers[cont].type == 0) {
            glColor3f(0.0, 1.0, 0.0);
        }
        else {
            glColor3f(0.0, 0.0, 1.0);
        }
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glutSolidTorus(0.1, 0.3, 80, 80);
        world.powers[cont].frame += 1.0 * deltaTime;
        world.powers[cont].state.R[0] += 100.0*(deltaTime / 1000.0);
        if (DEBUG) {
            glColor3f(0.0, 0.0, 1.0);
            drawCollisionSphere(world.powers[cont].state.T[0], world.powers[cont].state.T[1], world.powers[cont].state.T[2], world.powers[cont].collider);
        }
        glColor3f(1.0, 1.0, 1.0);
        glDisable(GL_COLOR_MATERIAL);
    }
    
}

void drawEnemyLifeBar(Enemy enemy) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);
    glColor3f(0.0,1.0,0.0);
    glPushMatrix();
    Enemy proto = config.enemyTypes[enemy.type];
    GLfloat value = (enemy.shield*10.0) / proto.shield;
    glTranslated(0.5-(value/20.0),-0.4,0);
    glScaled(value, 0.3, 0);
    glutSolidCube(0.1);
    glPopMatrix();
    glColor3f(1.0, 1.0, 1.0);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

void drawEnemies() {
    for (int cont = 0; cont < world.enemiesN; cont += 1) {
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        world.enemies[cont].state.T[0] = linearCurve(world.enemies[cont].frame, world.enemies[cont].T.start[0], world.enemies[cont].T.end[0], world.enemies[cont].frames);
        world.enemies[cont].state.T[1] = linearCurve(world.enemies[cont].frame, world.enemies[cont].T.start[1], world.enemies[cont].T.end[1], world.enemies[cont].frames);
        glTranslated(world.enemies[cont].state.T[0], world.enemies[cont].state.T[1], world.enemies[cont].state.T[2]);
        if (world.enemies[cont].type == 0) {
            glRotated(world.enemies[cont].state.R[0], 1, 1, 1);
        } else {
            drawEnemyLifeBar(world.enemies[cont]);
            glRotated(world.enemies[cont].state.R[0], 0, 1, 0);
        }
        glColor3b(1.0,0.0,0.0);
        glmDraw(world.enemies[cont].modID, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
        world.enemies[cont].frame += 1.0 * deltaTime;
        if (world.enemies[cont].type == 0) {
            world.enemies[cont].state.R[0] += 100.0*(deltaTime / 1000.0);
        }
        if (DEBUG) {
            glColor3f(0.0, 0.0, 1.0);
            drawCollisionSphere(world.enemies[cont].state.T[0], world.enemies[cont].state.T[1], world.enemies[cont].state.T[2], world.enemies[cont].collider);
        }
        glColor3f(1.0, 1.0, 1.0);
        glEnable(GL_LIGHTING);
        glDisable(GL_COLOR_MATERIAL);
    }
}

void drawPlayer() {
    glTranslatef(player.state.T[0], player.state.T[1], player.state.T[2]);
    glRotated(player.state.R[1], 0, 1, 0);
    glRotated(player.state.R[0], 1, 0, 0);
    glRotated(player.state.R[2], 0, 0, 1);
    
    glmDraw(player.modID, GLM_TEXTURE | GLM_SMOOTH | GLM_MATERIAL);
    if (DEBUG) {
        glColor3f(0.0, 0.0, 1.0);
        drawCollisionSphere(player.state.T[0], player.state.T[1], player.state.T[2], player.collider);
        glColor3f(1.0, 1.0, 0.0);
        drawCollisionSphere(player.state.T[0]+player.weapon.state.T[0], player.state.T[1] + player.weapon.state.T[1], player.state.T[2] + +player.weapon.state.T[2], 0.1);
    }
}

void drawBullets() {
    for (int cont = 0; cont < world.bulletsN; cont += 1) {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        world.bullets[cont].state.T[0] = linearCurve(world.bullets[cont].frame, world.bullets[cont].T.start[0], world.bullets[cont].T.end[0], world.bullets[cont].frames);
        glTranslated(world.bullets[cont].state.T[0], world.bullets[cont].state.T[1], world.bullets[cont].state.T[2]);
        glColor3f(1.0, 1.0, 0.0);
        glutSolidSphere(0.05, 20, 20);
        glColor3f(1.0, 1.0, 1.0);
        if (DEBUG) {
            drawCollisionSphere(world.bullets[cont].state.T[0], world.bullets[cont].state.T[1], world.bullets[cont].state.T[2], world.bullets[cont].collider);
        }
        world.bullets[cont].frame += 1 * deltaTime;
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }
}

void drawEnemyBullets() {
    for (int cont = 0; cont < world.ebulletsN; cont += 1) {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        world.enmbullets[cont].state.T[0] = linearCurve(world.enmbullets[cont].frame, world.enmbullets[cont].T.start[0], world.enmbullets[cont].T.end[0], world.enmbullets[cont].frames);
        glTranslated(world.enmbullets[cont].state.T[0], world.enmbullets[cont].state.T[1], world.enmbullets[cont].state.T[2]);
        glColor3f(1.0, 1.0, 0.0);
        glutSolidSphere(0.05, 20, 20);
        glColor3f(1.0, 1.0, 1.0);
        if (DEBUG) {
            drawCollisionSphere(world.enmbullets[cont].state.T[0], world.enmbullets[cont].state.T[1], world.enmbullets[cont].state.T[2], world.enmbullets[cont].collider);
        }
        world.enmbullets[cont].frame += 1 * deltaTime;
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    }
}


void movementContext() {
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    if (player.context.WK) {
        player.state.R[0] = -20;
    }
    else {
        if (player.context.SK) {
            player.state.R[0] = 20;
        }
        else {
            player.state.R[0] = 0.0;
        }
    }
    
    
    GLfloat *DV = getDirectionVector();
    player.state.T[0] += DV[0];
    player.state.T[1] += DV[1];
    player.state.T[0] = max(-1.0*WORLD_SIZE + player.collider*3.3, (double)player.state.T[0]);
    player.state.T[0] = min(1.0*WORLD_SIZE - player.collider*3.3, (double)player.state.T[0]);
    player.state.T[1] = max(-1.0*WORLD_SIZE + player.collider*2.5, (double)player.state.T[1]);
    player.state.T[1] = min(1.0*WORLD_SIZE - player.collider*2.5, (double)player.state.T[1]);
}

void updateBullets(bool add) {
    Bullet *list = (Bullet *)malloc(sizeof(Bullet)*(world.bulletsN));
    GLint activeBullets = 0;
    for (int cont = 0; cont < world.bulletsN; cont += 1) {
        if (world.bullets[cont].state.T[0] < WORLD_SIZE && world.bullets[cont].active) {
            activeBullets += 1;
        }
        else {
            world.bullets[cont].active = false;
            //cout << "Borrando Bala " << cont << "\n";
        }
        list[cont] = world.bullets[cont];
    }
    
    free(world.bullets);
    
    
    if (add) {
        world.bullets = (Bullet *)malloc(sizeof(Bullet)*(activeBullets + 1));
    } else {
        world.bullets = (Bullet *)malloc(sizeof(Bullet)*(activeBullets));
    }
    GLint con = 0;
    for (int cont = 0; cont < world.bulletsN; cont += 1) {
        if (list[cont].active) {
            world.bullets[con] = list[cont];
            con += 1;
        }
    }
    free(list);
    
    world.bulletsN = activeBullets;
}

void fireContext() {
    if (player.context.FIRE && player.context.T_FIRE >= FIRE_RATE && player.weapon.ammunition){
        updateBullets(true);
        world.bullets[world.bulletsN].collider = 0.1;
        world.bullets[world.bulletsN].frames = 600.0 * (WORLD_SIZE - player.state.T[0] + player.weapon.state.T[0]) / (WORLD_SIZE - (-WORLD_SIZE + player.collider * 3.5));
        world.bullets[world.bulletsN].frame = 0.0;
        world.bullets[world.bulletsN].active = true;
        
        world.bullets[world.bulletsN].T.start = (GLfloat *)malloc(sizeof(GLfloat) * 3);
        world.bullets[world.bulletsN].T.start[0] = player.state.T[0] + player.weapon.state.T[0];
        world.bullets[world.bulletsN].T.start[1] = player.state.T[1] + player.weapon.state.T[1];
        world.bullets[world.bulletsN].T.start[2] = player.state.T[2] + player.weapon.state.T[2];
        
        world.bullets[world.bulletsN].T.end = (GLfloat *)malloc(sizeof(GLfloat) * 3);
        world.bullets[world.bulletsN].T.end[0] = WORLD_SIZE;
        world.bullets[world.bulletsN].T.end[1] = player.state.T[1] + player.weapon.state.T[1];
        world.bullets[world.bulletsN].T.end[2] = player.state.T[2] + player.weapon.state.T[2];
        
        world.bullets[world.bulletsN].state.T = (GLfloat *)malloc(sizeof(GLfloat) * 3);
        world.bullets[world.bulletsN].state.T[0] = 0;
        world.bullets[world.bulletsN].state.T[1] = world.bullets[world.bulletsN].T.start[1];
        world.bullets[world.bulletsN].state.T[2] = world.bullets[world.bulletsN].T.start[2];
        
        world.bulletsN += 1;
        player.weapon.ammunition -= 1;
        player.weapon.ammunition = max(0, player.weapon.ammunition);
        player.context.FIRE = false;
        player.context.T_FIRE = 0;
        //printf("Bullets: %d\n", world.bulletsN);
    }else{
        updateBullets(false);
        player.context.FIRE = false;
        player.context.T_FIRE += deltaTime;
    }
}

void processContext() {
    
    movementContext();
    
    drawPlayer();
    
    fireContext();
    
    drawBullets();
    
    
}

void updateEnemyBullets(bool add) {
    Bullet *list = (Bullet *)malloc(sizeof(Bullet)*(world.ebulletsN));
    GLint activeBullets = 0;
    for (int cont = 0; cont < world.ebulletsN; cont += 1) {
        if (world.enmbullets[cont].state.T[0] > -WORLD_SIZE && world.enmbullets[cont].active) {
            activeBullets += 1;
        }
        else {
            world.enmbullets[cont].active = false;
            //cout << "Borrando Bala " << cont << "\n";
        }
        list[cont] = world.enmbullets[cont];
    }
    
    free(world.enmbullets);
    
    
    if (add) {
        world.enmbullets = (Bullet *)malloc(sizeof(Bullet)*(activeBullets + 1));
    }
    else {
        world.enmbullets = (Bullet *)malloc(sizeof(Bullet)*(activeBullets));
    }
    GLint con = 0;
    for (int cont = 0; cont < world.ebulletsN; cont += 1) {
        if (list[cont].active) {
            world.enmbullets[con] = list[cont];
            con += 1;
        }
    }
    free(list);
    
    world.ebulletsN = activeBullets;
}

void shootPlayer() {
    for (int cont = 0; cont < world.enemiesN; cont += 1) {
        if (world.enemies[cont].type == 1 && world.enemies[cont].shootframe >= ENEMY_SHOOT_RATE) {
            updateEnemyBullets(true);
            world.enmbullets[world.ebulletsN].collider = 0.1;
            world.enmbullets[world.ebulletsN].frames = 2000.0 * (WORLD_SIZE + world.enemies[cont].state.T[0] - 0.4 + 0.05) / (WORLD_SIZE - (-WORLD_SIZE + world.enemies[cont].collider * 3.5));;
            //cout << "Frames"<< world.enmbullets[world.ebulletsN].frames <<"\n";
            world.enmbullets[world.ebulletsN].frame = 0.0;
            world.enmbullets[world.ebulletsN].active = true;
            
            world.enmbullets[world.ebulletsN].T.start = (GLfloat *)malloc(sizeof(GLfloat) * 3);
            world.enmbullets[world.ebulletsN].T.start[0] = world.enemies[cont].state.T[0] - world.enemies[cont].collider + 0.05;
            world.enmbullets[world.ebulletsN].T.start[1] = world.enemies[cont].state.T[1];
            world.enmbullets[world.ebulletsN].T.start[2] = world.enemies[cont].state.T[2];
            
            world.enmbullets[world.ebulletsN].T.end = (GLfloat *)malloc(sizeof(GLfloat) * 3);
            world.enmbullets[world.ebulletsN].T.end[0] = - WORLD_SIZE;
            world.enmbullets[world.ebulletsN].T.end[1] = world.enemies[cont].state.T[1];
            world.enmbullets[world.ebulletsN].T.end[2] = world.enemies[cont].state.T[2];
            
            world.enmbullets[world.ebulletsN].state.T = (GLfloat *)malloc(sizeof(GLfloat) * 3);
            world.enmbullets[world.ebulletsN].state.T[0] = 0;
            world.enmbullets[world.ebulletsN].state.T[1] = world.enmbullets[world.ebulletsN].T.start[1];
            world.enmbullets[world.ebulletsN].state.T[2] = world.enmbullets[world.ebulletsN].T.start[2];
            
            world.ebulletsN += 1;
            world.enemies[cont].shootframe = 0;
        } else {
            updateEnemyBullets(false);
            world.enemies[cont].shootframe += 1.0 * deltaTime;
        }
    }
}

void updateDeltaTime() {
    GLdouble newTime = glutGet(GLUT_ELAPSED_TIME);
    deltaTime = newTime - oldTime;
    oldTime = newTime;
}

void showParallaxLayers() {
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glClear(GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0,1.0,1.0);
    
    for (int cont = 0; cont < parallax.size; cont += 1) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        ParallaxLayer layer = parallax.layers[cont];
        parallax.layers[cont].pace = parallax.layers[cont].pace > parallax.layers[cont].duration ? 0.0 : parallax.layers[cont].pace;
        
        glTranslated((-(WORLD_SIZE)* 2 * (parallax.layers[cont].pace / parallax.layers[cont].duration)), 0, 0);
        parallax.layers[cont].pace += deltaTime;
        
        GLfloat DEPTH = 1 - cont*0.1;
        
        glBindTexture(GL_TEXTURE_2D, layer.texID);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-WORLD_SIZE, WORLD_SIZE, DEPTH);
        
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-WORLD_SIZE, -WORLD_SIZE, DEPTH);
        
        glTexCoord2f(1.0, 0.0);
        glVertex3f(WORLD_SIZE, -WORLD_SIZE, DEPTH);
        
        glTexCoord2f(1.0, 1.0);
        glVertex3f(WORLD_SIZE, WORLD_SIZE, DEPTH);
        glEnd();
        
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(WORLD_SIZE, WORLD_SIZE, DEPTH);
        
        glTexCoord2f(0.0, 0.0);
        glVertex3f(WORLD_SIZE, -WORLD_SIZE, DEPTH);
        
        glTexCoord2f(1.0, 0.0);
        glVertex3f(WORLD_SIZE + WORLD_SIZE * 2, -WORLD_SIZE, DEPTH);
        
        glTexCoord2f(1.0, 1.0);
        glVertex3f(WORLD_SIZE + WORLD_SIZE * 2, WORLD_SIZE, DEPTH);
        glEnd();
        
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    
}

GLdouble FOVY(GLdouble size, GLdouble dist) {
    GLdouble theta = 2.0*atan((size / 2.0) / dist);
    return(180.0*theta / M_PI);
}

void orthoCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-WORLD_SIZE, WORLD_SIZE, -WORLD_SIZE, WORLD_SIZE, -WORLD_SIZE, WORLD_SIZE);
}

void perspectiveCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOVY(WORLD_SIZE, WORLD_SIZE), 1.0, 0.1, 10 * WORLD_SIZE);
    gluLookAt(
              0, 0, WORLD_SIZE*1.55,
              0, 0, -1,
              0, 1, 0
              );
}

GLfloat distanceBetween(GLfloat* p1, GLfloat* p2) {
    GLfloat vector[] = {
        p1[0] - p2[0],
        p1[1] - p2[1]
    };
    return sqrt(vector[0]*vector[0]+vector[1]*vector[1]);
}

bool collidingObjects(GLfloat* p1,GLfloat c1, GLfloat* p2, GLfloat c2) {
    GLfloat radios = c1 + c2;
    return distanceBetween(p1, p2) <= radios;
}

void detectCollisions() {
    for (int cont = 0; cont < world.bulletsN; cont += 1) {
        for (int con = 0; con < world.enemiesN; con += 1) {
            if (collidingObjects(world.enemies[con].state.T, world.enemies[con].collider, world.bullets[cont].state.T, world.bullets[cont].collider)) {
                //printf("Impact between Bullet %i and Enemy %i-%i\n",cont,con, world.enemies[con].shield);
                world.bullets[cont].active = false;
                world.enemies[con].shield -= 1;
            }
        }
    }
    for (int cont = 0; cont < world.enemiesN; cont += 1) {
        Enemy enemy = world.enemies[cont];
        if (collidingObjects(enemy.state.T, enemy.collider, player.state.T, player.collider) && !world.enemies[cont].crashed) {
            //printf("Impact between Player and Enemy %i-%i\n", cont, enemy.shield);
            world.enemies[cont].crashed = true;
            player.shield -= world.enemies[cont].damage;
            world.enemies[cont].shield -= player.damage;
        }
    }
    for (int cont = 0; cont < world.ebulletsN; cont += 1) {
        Bullet enemy = world.enmbullets[cont];
        if (collidingObjects(enemy.state.T, enemy.collider, player.state.T, player.collider)) {
            //printf("Impact between Player and Enemy %i-%i\n", cont, enemy.shield);
            player.shield -= 5;
            world.enmbullets[cont].active = false;
        }
    }
    for (int cont = 0; cont < world.powersN; cont += 1) {
        if (collidingObjects(world.powers[cont].state.T, world.powers[cont].collider, player.state.T, player.collider)) {
            world.powers[cont].active = false;
            if (world.powers[cont].type == 0) {
                player.shield += world.powers[cont].quantity;
                if (player.shield > 100.0) {
                    player.shield = 100.0;
                }
            } else {
                player.weapon.ammunition += world.powers[cont].quantity;
                if (player.weapon.magazine < player.weapon.ammunition) {
                    player.weapon.ammunition = player.weapon.magazine;
                }
            }
        }
    }
    for (int cont = 0; cont < world.enemiesN; cont += 1) {
        world.enemies[cont].shield = max(0, world.enemies[cont].shield);
        if (world.enemies[cont].shield <= 0) {
            world.enemies[cont].active = false;
            world.score += 1;
        }
    }
    if (player.shield <= 0) {
        player.shield = max(0, player.shield);
        estado = 2;
    }
}

void insertText(float x, float y, const char *string, void *font) {
    const char *c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void drawPlayerLifeBar() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(
                 ((-2 * WORLD_SIZE) / 3.0) + 0.2,
                 -WORLD_SIZE + 0.6,
                 0.0);
    glScaled(WORLD_SIZE / 1.5, 0.4, 0);
    glColor4d(0.007, 0.003, 0.07, 0.3);
    glutSolidCube(1.0);
    
    glLoadIdentity();
    GLfloat value = (((WORLD_SIZE / 1.5) - 0.2)*player.shield) / (100.0);
    glTranslated(
                 (((-2 * WORLD_SIZE) / 3.0) + 0.2) - ((WORLD_SIZE / 1.5) - 0.2) / 2.0 + value / 2,
                 -WORLD_SIZE + 0.6,
                 0.0);
    glScaled(value, 0.4 - 0.2, 0);
    glColor4d(0.0, 1.0, 0.0, 0.5);
    glutSolidCube(1.0);
    glLoadIdentity();
    glColor4f(1.0, 1.0, 1.0, 1.0);
    insertText((WORLD_SIZE * 3) / 4 - (0.1), -WORLD_SIZE + 0.25, "Ammo", GLUT_BITMAP_HELVETICA_12);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

void drawWeaponStatus() {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(
                 ((2 * WORLD_SIZE) / 3.0) - 0.2,
                 -WORLD_SIZE + 0.6,
                 0.0);
    glScaled(WORLD_SIZE / 1.5, 0.4, 0);
    glColor4d(0.007, 0.003, 0.07, 0.3);
    glutSolidCube(1.0);
    
    GLfloat value = (((WORLD_SIZE / 1.5) - 0.2)*player.weapon.ammunition) / (player.weapon.magazine);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(
                 ((2 * WORLD_SIZE) / 3.0) - 0.2 + ((WORLD_SIZE / 1.5) - 0.2) / 2.0 - value / 2,
                 -WORLD_SIZE + 0.6,
                 0.0);
    glScaled(value, 0.2, 0);
    glColor4d(0.0, 0.0, 1.0, 0.5);
    glutSolidCube(1.0);
    glLoadIdentity();
    glColor4f(1.0, 1.0, 1.0, 1.0);
    insertText((-WORLD_SIZE*3)/4 - (0.4), -WORLD_SIZE + 0.25, "Shield", GLUT_BITMAP_HELVETICA_12);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}


void InsertarTexto(float x, float y,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void scoreText() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4f(1.0, 1.0, 1.0, 1.0);
    
    char integer_string[128];
    GLint integer = world.score;
    
    sprintf(integer_string, "%d", integer);
    
    char other_string[64] = "Score: ";
    
    strcat(other_string, integer_string);
    insertText(-WORLD_SIZE + 0.2, WORLD_SIZE - 0.4, other_string, GLUT_BITMAP_HELVETICA_18);
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
}

void levelText() {
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor4f(1.0, 1.0, 1.0, 1.0);
    
    char integer_string[128];
    GLint integer = world.level;
    
    sprintf(integer_string, "%d", integer);
    
    char other_string[64] = "Level: ";
    
    strcat(other_string, integer_string);
    insertText(WORLD_SIZE - 1.5, WORLD_SIZE - 0.4, other_string, GLUT_BITMAP_HELVETICA_18);
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
}

void drawInterface() {
    
    drawPlayerLifeBar();
    
    drawWeaponStatus();
    
    scoreText();
    
    levelText();
    
}

void updateDifficulty() {
    GLint difficulty = ((world.score) / DIFFICULTY_SCORE) + 1;
    if (difficulty > world.level) {
        world.level = difficulty;
        player.shield += 20;
        player.weapon.ammunition = player.weapon.magazine;
        config.enemySpeed -= 50 * world.level;
        config.enemyRate -= 50 * world.level;
    }
    if (difficulty > 3) {
        estado = 3;
    }
}

void Display()
{
    updateDeltaTime();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if(estado == 0){
        
        glLoadIdentity();
        showParallaxLayers();
        orthoCamera();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glColor3f(1.0, 1.0, 1.0);
        InsertarTexto(0 - 0.5,WORLD_SIZE -1,"Controls");
        InsertarTexto(0 - 0.05,3,"W");
        InsertarTexto(0 - 0.1, 3 - 0.5, "Up");
        InsertarTexto(-2,2,"A");
        InsertarTexto(-2 - 0.2,2 - 0.5, "Back");
        InsertarTexto(2, 2, "D");
        InsertarTexto(2 - 0.25, 2 - 0.5, "Forth");
        InsertarTexto(0 - 0.05, 1, "S");
        InsertarTexto(0 - 0.35, 1 - 0.5, "Down");
        
        InsertarTexto(0 - 0.4, -0.5, "Space");
        InsertarTexto(0 - 0.4, -0.5 - 0.5, "Shoot");
        
        InsertarTexto(0 - 1.8,-2,"Press Any Key to Continue");
    }else{
        if(estado == 1){
            updateDifficulty();
            
            orthoCamera();
            showParallaxLayers();
            
            
            perspectiveCamera();
            processContext();
            generateEnemies();
            shootPlayer();
            drawEnemies();
            drawEnemyBullets();
            generatePowerUps();
            drawPowerUps();
            
            detectCollisions();
            
            orthoCamera();
            drawInterface();
            
        }else{
            if(estado == 2){
                glLoadIdentity();
                showParallaxLayers();
                orthoCamera();
                glDisable(GL_TEXTURE_2D);
                glDisable(GL_LIGHTING);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glColor3f(1.0, 0.0, 0.0);
                InsertarTexto(-1,0,"Game Over");
            }else{
                if(estado == 3){
                    glLoadIdentity();
                    showParallaxLayers();
                    orthoCamera();
                    glDisable(GL_TEXTURE_2D);
                    glDisable(GL_LIGHTING);
                    glMatrixMode(GL_MODELVIEW);
                    glLoadIdentity();
                    glColor3f(1.0, 0.0, 0.0);
                    InsertarTexto(-1,0,"You Win");
                }
            }
        }
    }
    
    
    glutSwapBuffers();
    glutPostRedisplay();
    
}

void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    
}

void keyboardController(unsigned char key, int x, int y) {
    
    if(estado==0){
        estado = 1;
    }else{
        switch (key)
        {
            case 'a':case 'A':
                player.context.AK = true;
                break;
            case 'w':case 'W':
                player.context.WK = true;
                break;
            case 's':case 'S':
                player.context.SK = true;
                break;
            case 'd':case 'D':
                player.context.DK = true;
                break;
            case SPACEBAR:
                player.context.FIRE = true;
                //cout << "Disparo\n";
                break;
            default:
                
                break;
        }
    }
}

void keyboardRelease(unsigned char key, int x, int y) {
    //cout << "Release " << key << " " << x << " " << y << "\n";
    switch (key)
    {
        case 'a':case 'A':
            player.context.AK = false;
            break;
        case 'w':case 'W':
            player.context.WK = false;
            break;
        case 's':case 'S':
            player.context.SK = false;
            break;
        case 'd':case 'D':
            player.context.DK = false;
            break;
        default:
            
            break;
    }
}


int main(int artcp, char **argv)
{
    glutInit(&artcp, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Star Fox");
    glutKeyboardUpFunc(keyboardRelease);
    glutKeyboardFunc(keyboardController);
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    Init();
    oldTime = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
    return 0;
}
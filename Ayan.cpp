#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
using namespace std;

struct Vec3 { float x,y,z; };

enum RingType { NORMAL, BONUS };

struct Ring {
    Vec3 pos;
    bool collected;
    float radius;
    RingType type;
};

vector<Ring> rings;
int score = 0;
int missedRings = 0;
bool gameOver = false;

Vec3 planePos = {0,2,0};
float planeYaw = 0;
float planePitch = 0;

float speed = 6.5f;
float camDistance = 8.0f;
float camHeight = 2.5f;

float spawnTimer = 0;
const float spawnInterval = 1.0f;

int winW=900, winH=600;
int lastTime=0;

float deg2rad(float d){ return d*3.1415926f/180.0f; }

float dist3(const Vec3&a,const Vec3&b){
    return sqrt(
        (a.x-b.x)*(a.x-b.x)+
        (a.y-b.y)*(a.y-b.y)+
        (a.z-b.z)*(a.z-b.z));
}

/*RESET */
void resetGame(){
    rings.clear();
    score = 0;
    missedRings = 0;
    gameOver = false;
    planePos = {0,2,0};
    planeYaw = planePitch = 0;
    spawnTimer = 0;
}

/* SPAWN */
void spawnRing(){
    float yawRad = deg2rad(planeYaw);
    Vec3 forward = {-sinf(yawRad),0,-cosf(yawRad)};

    Ring r;
    float fd = 35 + rand()%20;

    r.pos.x = planePos.x + forward.x*fd + (rand()%40-20)*0.3f;
    r.pos.y = max(1.0f, planePos.y + (rand()%20-10)*0.2f);
    r.pos.z = planePos.z + forward.z*fd + (rand()%40-20)*0.3f;

    r.radius = 1.6f;
    r.collected = false;


    r.type = (rand()%6==0)?BONUS:NORMAL;

    rings.push_back(r);
}

/*PLANE */
void drawPlane(){
    glPushMatrix();

    glColor3f(0.95,0.95,0.95);
    glPushMatrix();
        glScalef(0.35,0.35,1.9);
        glutSolidSphere(1,28,28);
    glPopMatrix();

    glColor3f(0.85,0.2,0.2);
    glPushMatrix();
        glTranslatef(0,0,1.15);
        glRotatef(-90,1,0,0);
        glutSolidCone(0.35,0.7,24,24);
    glPopMatrix();

    glColor3f(1,1,1);
    glPushMatrix();
        glTranslatef(0,0,0.1);
        glScalef(2.8,0.08,0.6);
        glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(0,0,-0.95);
        glScalef(1.2,0.06,0.45);
        glutSolidCube(1);
    glPopMatrix();

    glColor3f(0.85,0.2,0.2);
    glPushMatrix();
        glTranslatef(0,0.18,-1.05);
        glScalef(0.08,0.36,0.4);
        glutSolidCube(1);
    glPopMatrix();

    glColor3f(0.08,0.18,0.28);
    glPushMatrix();
        glTranslatef(0,0.15,0.55);
        glutSolidSphere(0.45,20,20);
    glPopMatrix();

    glPopMatrix();
}

/*SQUARE OBJECT  */
void drawRing(const Ring&r){
    glPushMatrix();
        glTranslatef(r.pos.x,r.pos.y,r.pos.z);

        float s=r.radius*2, t=0.25f;

        if(r.type==BONUS) glColor3f(1,0,0);
        else glColor3f(1,0.85,0.1);

        glPushMatrix(); glTranslatef(0,s/2,0); glScalef(s,t,t); glutSolidCube(1); glPopMatrix();
        glPushMatrix(); glTranslatef(0,-s/2,0); glScalef(s,t,t); glutSolidCube(1); glPopMatrix();
        glPushMatrix(); glTranslatef(-s/2,0,0); glScalef(t,s,t); glutSolidCube(1); glPopMatrix();
        glPushMatrix(); glTranslatef(s/2,0,0); glScalef(t,s,t); glutSolidCube(1); glPopMatrix();

    glPopMatrix();
}

/*DISPLAY */
void display(){
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float yaw=deg2rad(planeYaw), pitch=deg2rad(planePitch);
    Vec3 forward={-sinf(yaw)*cosf(pitch),sinf(pitch),-cosf(yaw)*cosf(pitch)};

    gluLookAt(
        planePos.x-forward.x*camDistance,
        planePos.y+camHeight,
        planePos.z-forward.z*camDistance,
        planePos.x,planePos.y,planePos.z,
        0,1,0
    );

    GLfloat lp[]={0,80,0,1};
    glLightfv(GL_LIGHT0,GL_POSITION,lp);
    glClearColor(0.53,0.81,0.98,1);

    for(auto&r:rings) if(!r.collected) drawRing(r);

    glPushMatrix();
        glTranslatef(planePos.x,planePos.y,planePos.z);
        glRotatef(planeYaw,0,1,0);
        glRotatef(planePitch,1,0,0);
        drawPlane();
    glPopMatrix();

    stringstream ss; ss<<"Score: "<<score;
    stringstream ms; ms<<"Missed: "<<missedRings<<"/5";

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity(); gluOrtho2D(0,winW,0,winH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    glDisable(GL_LIGHTING);
    glColor3f(1,1,1);
    glRasterPos2f(10,winH-30); for(char c:ss.str()) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);
    glRasterPos2f(10,winH-55); for(char c:ms.str()) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);
    if(gameOver){
        glRasterPos2f(winW/2-120,winH/2);
        string t="GAME OVER! Press R";
        for(char c:t) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,c);
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

/*FIXED MISS LOGIC */
void update(int){
    if(gameOver){ glutTimerFunc(16,update,0); return; }

    int now=glutGet(GLUT_ELAPSED_TIME);
    float dt=(lastTime==0)?0.016f:(now-lastTime)/1000.0f;
    lastTime=now;

    float yaw=deg2rad(planeYaw), pitch=deg2rad(planePitch);
    Vec3 forward={-sinf(yaw)*cosf(pitch),sinf(pitch),-cosf(yaw)*cosf(pitch)};

    planePos.x+=forward.x*speed*dt;
    planePos.y+=forward.y*speed*dt;
    planePos.z+=forward.z*speed*dt;

    spawnTimer+=dt;
    if(spawnTimer>=spawnInterval){
        spawnRing();
        spawnTimer=0;
    }

    for(int i=rings.size()-1;i>=0;i--){
        float d=dist3(planePos,rings[i].pos);

        // COLLECT
        if(d<rings[i].radius){
            score+=(rings[i].type==BONUS)?50:10;
            rings.erase(rings.begin()+i);
            continue;
        }

        // MISS (plane passed it)
        Vec3 rel={rings[i].pos.x-planePos.x,
                  rings[i].pos.y-planePos.y,
                  rings[i].pos.z-planePos.z};
        float proj=rel.x*forward.x+rel.y*forward.y+rel.z*forward.z;

        if(proj<-5){
            rings.erase(rings.begin()+i);
            missedRings++;
            if(missedRings>=5) gameOver=true;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16,update,0);
}

/*INPUT  */
void keyboard(unsigned char k,int,int){
    if(k==27) exit(0);
    if(k=='a'||k=='A') planeYaw+=3.5f;
    if(k=='d'||k=='D') planeYaw-=3.5f;
    if(k=='w'||k=='W'){ planePitch+=2; if(planePitch>30) planePitch=30; }
    if(k=='s'||k=='S'){ planePitch-=2; if(planePitch<-20) planePitch=-20; }
    if(k=='r'||k=='R') resetGame();
}

void initGL(){
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
}

void reshape(int w,int h){
    winW=w; winH=h;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60,(float)w/h,0.1,1000);
    glMatrixMode(GL_MODELVIEW);
}


int main(int argc,char**argv){
    srand(time(0));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(winW,winH);
    glutCreateWindow("Aeroplane Game");

    initGL();
    resetGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    lastTime=glutGet(GLUT_ELAPSED_TIME);
    glutTimerFunc(16,update,0);

    glutMainLoop();
    return 0;
}

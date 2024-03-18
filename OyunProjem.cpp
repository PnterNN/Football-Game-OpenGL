#include <GL/glut.h>
#include < GL/freeglut.h>
#include <cmath>
#include <stdio.h>
#include <string>
#include <ctime>

#define M_PI 3.14159265358979323846

time_t startTime;
int gameDuration = 5 * 60; 

float cameraX = 530.0f;
float cameraY = 900.0f;
float cameraZ = 500.0f;
float cameraYaw = 0.0f;
float cameraPitch = -45.0f;
bool finished = false;
bool hittedball = false;
int score[2] = { 0, 0 };

struct Player {
    float x, y, z;
    float velocityX, velocityY, velocityZ;
    float size;
    float speed;
    float color[3]; 
    bool keyPressed[256]; 
};

Player players[2];

struct Ball {
    float x, y, z;
    float radius;
    float velocityX, velocityY, velocityZ;
    float gravity;
    float bounceFactor;
};
Ball ball = { 530.0, 350.0, 100.0, 20.0, 0.0, 0.0, 0.0, 0.1, 0.3 };


void freezePlayersAndBall() {
    for (int i = 0; i < 2; ++i) {
        players[i].velocityX = 0.0;
        players[i].velocityY = 0.0;
        players[i].velocityZ = 0.0;
    }

    ball.velocityX = 0.0;
    ball.velocityY = 0.0;
    ball.velocityZ = 0.0;

}

void drawScore() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0, 1.0, 1.0);

    time_t currentTime = time(nullptr);
    int elapsedTime = difftime(currentTime, startTime);
    int remainingTime = gameDuration - elapsedTime;

    int minutes = remainingTime / 60;
    int seconds = remainingTime % 60;

    if (remainingTime <= 0) {
        finished = true;
        glutIdleFunc(nullptr);
        freezePlayersAndBall();
        std::string timeUpText;
        if (score[0] > score[1]) {
            timeUpText = "Kazanan 1. Oyuncu (Sifirlamak icin 'R')";
            glRasterPos2f((glutGet(GLUT_WINDOW_WIDTH) - 320) / 2, 20);
        }
        else if (score[1] > score[0]) {
            timeUpText = "Kazanan 2. Oyuncu (Sifirlamak icin 'R')";
            glRasterPos2f((glutGet(GLUT_WINDOW_WIDTH) - 320) / 2, 20);
        }
        else {
            timeUpText = "Berabere (Sifirlamak icin 'R')";
            glRasterPos2f((glutGet(GLUT_WINDOW_WIDTH) - 230) / 2, 20);
            
        }
        for (char const& c : timeUpText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
        
    }
    else {
        std::string timerText = "Kalan Sure: " + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);

        glColor3f(1.0, 1.0, 1.0);
        glRasterPos2f((glutGet(GLUT_WINDOW_WIDTH) - 170) / 2, 20);
        for (char const& c : timerText) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }



    std::string player1Score = "Oyuncu 1 Skor: " + std::to_string(score[0]);
    glRasterPos2f(10, 20);
    for (char const& c : player1Score) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    std::string player2Score = "Oyuncu 2 Skor: " + std::to_string(score[1]);
    glRasterPos2f(glutGet(GLUT_WINDOW_WIDTH) - 160, 20);
    for (char const& c : player2Score) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

void drawPlayers() {
    for (int i = 0; i < 2; ++i) {
        glColor3fv(players[i].color);
        glPushMatrix();
        glTranslatef(players[i].x, players[i].y, players[i].z);
        glutSolidSphere(players[i].size, 20, 20);
        glPopMatrix();
    }
}
void drawBall() {
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(ball.x, ball.y, ball.z);
    glutSolidSphere(ball.radius, 20, 20);
    glPopMatrix();
}
void resetBall() {
    ball.x = 530.0;
    ball.y = 350.0;
    ball.z = 100.0;
    ball.velocityX = 0.0;
    ball.velocityY = 0.0;
    ball.velocityZ = 0.0;
    ball.gravity = 0.1;
    hittedball = false; 
}

void resetPlayers() {
    players[0].x = 860.0;
    players[0].y = 350.0;
    players[0].z = 20.0;
    players[0].velocityX = 0.0;
    players[0].velocityY = 0.0;
    players[0].velocityZ = 0.0;
    players[1].x = 200.0;
    players[1].y = 350.0;
    players[1].z = 20.0;
    players[1].velocityX = 0.0;
    players[1].velocityY = 0.0;
    players[1].velocityZ = 0.0;
}

bool checkGoal() {
    float ballX = ball.x;
    float ballY = ball.y;

    float goal1StartX = 50.0;
    float goal1EndX = 50.0;
    float goal1StartY = 250.0;
    float goal1EndY = 450.0;

    float goal2StartX = 1010.0;
    float goal2EndX = 1010.0;
    float goal2StartY = 250.0;
    float goal2EndY = 450.0;


    if (ballX >= goal1StartX - ball.radius && ballX <= goal1StartX + ball.radius) {
        if (ballY >= goal1StartY && ballY <= goal1EndY) {
            if (ball.z > 80) return false;
            score[1]++; 
            resetBall(); 
            resetPlayers();
            return true; 
        }
    }

    if (ballX >= goal2StartX - ball.radius && ballX <= goal2StartX + ball.radius) {
        if (ballY >= goal2StartY && ballY <= goal2EndY) {
            if (ball.z > 80) return false;
            score[0]++;
            resetBall();
            resetPlayers(); 

            return true; 
        }
    }

    return false; 
}

void hitBall(float targetX, float targetY, float targetZ) {
    float deltaX = targetX - ball.x;
    float deltaY = targetY - ball.y;
    float deltaZ = targetZ - ball.z;
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

    
    if (distance < 20 + ball.radius + 5) {
        if (!hittedball) {
            hittedball = true;
            
            float velocityMagnitude = 10.0;
           
            float hitDirectionX = (targetX - ball.x) / distance;
            float hitDirectionY = (targetY - ball.y) / distance;
          
            float playerSpeedMagnitude = sqrt(players[0].velocityX * players[0].velocityX + players[0].velocityY * players[0].velocityY);
            ball.velocityX = -hitDirectionX * velocityMagnitude + players[0].velocityX * 0.1;
            ball.velocityY = -hitDirectionY * velocityMagnitude + players[0].velocityY * 0.1;
            ball.velocityZ = (targetZ - ball.z) / distance * velocityMagnitude + 8; 
            ball.gravity = 0.1;
        }
    }
    else {
        hittedball = false;
    }
}

void updateBall() {
    ball.velocityZ -= ball.gravity;
    ball.x += ball.velocityX;
    ball.y += ball.velocityY;
    ball.z += ball.velocityZ;

    if (ball.z < ball.radius) {
        ball.z = ball.radius;
        ball.velocityZ *= -ball.bounceFactor;
    }
    if (ball.x < 50.0 + ball.radius) {
        ball.x = 50.0 + ball.radius;
        ball.velocityX *= -ball.bounceFactor;
    }
    if (ball.x > 1010.0 - ball.radius) {
        ball.x = 1010.0 - ball.radius;
        ball.velocityX *= -ball.bounceFactor;
    }
    if (ball.y < 50.0 + ball.radius) {
        ball.y = 50.0 + ball.radius;
        ball.velocityY *= -ball.bounceFactor;
    }
    if (ball.y > 650.0 - ball.radius) {
        ball.y = 650.0 - ball.radius;
        ball.velocityY *= -ball.bounceFactor;
    }

}

void drawFieldLines() {
    glLineWidth(2.0);
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(50.0, 50.0, 0.0);
    glVertex3f(50.0, 650.0, 0.0);
    glVertex3f(1010.0, 650.0, 0.0);
    glVertex3f(1010.0, 50.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(530.0, 50.0, 0.0);
    glVertex3f(530.0, 650.0, 0.0);
    glEnd();
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float theta = i * M_PI / 180;
        glVertex3f(530.0 + 50.0 * cos(theta), 350.0 + 50.0 * sin(theta), 0.0);
    }
    glEnd();
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(50.0, 250.0, 0.0);
    glVertex3f(150.0, 250.0, 0.0);
    glVertex3f(150.0, 450.0, 0.0);
    glVertex3f(50.0, 450.0, 0.0);
    glEnd();
    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(1010.0, 250.0, 0.0);
    glVertex3f(910.0, 250.0, 0.0);
    glVertex3f(910.0, 450.0, 0.0);
    glVertex3f(1010.0, 450.0, 0.0);
    glEnd();
    glColor3f(1.0, 1.0, 1.0);
}

void updatePlayer() {
    for (int i = 0; i < 2; ++i) {
        players[i].x += players[i].velocityX;
        players[i].y += players[i].velocityY;
        players[i].z += players[i].velocityZ;

        if (players[i].x < 50.0) players[i].x = 50.0;
        if (players[i].x > 1010.0) players[i].x = 1010.0;
        if (players[i].y < 50.0) players[i].y = 50.0;
        if (players[i].y > 650.0) players[i].y = 650.0;

        
        players[i].velocityZ -= 0.5; 

        if (players[i].z <= 0.0) {
            players[i].z = 0.0; 
            players[i].velocityZ = 0.0; 
        }

        players[i].z += players[i].velocityZ;

        if (players[i].z < 0.0) players[i].z = 0.0;
    }
}
void drawShadow(float x, float y , float radius) {
    glColor3f(0.2, 0.2, 0.2); 


    glPushMatrix();
    glTranslatef(x, y, -7.0); 
    glutSolidSphere(radius * 1.1, 20, 20); 
    glPopMatrix();
}
void drawScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ,
        530.0, 350.0, 0.0,
        0.0, 0.0, 1.0);

    drawFieldLines();
    drawBall();
    drawPlayers();

    updatePlayer();

    
    checkGoal();
    drawScore();
    drawShadow(ball.x, ball.y , ball.radius);
    drawShadow(players[0].x, players[0].y, players[0].size);
    drawShadow(players[1].x, players[1].y, players[1].size);
    

    glutSwapBuffers();
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1500.0f); 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void init() {
    glClearColor(0.23, 0.36, 0.19, 0.0);
    glEnable(GL_DEPTH_TEST);
    startTime = time(nullptr);
}

void update(int value) {
    updateBall();
    cameraX = ball.x;
    cameraY = ball.y - 450;

    hitBall(players[0].x, players[0].y, players[0].z);
    hitBall(players[1].x, players[1].y, players[1].z);

    if (players[0].keyPressed['r'] || players[0].keyPressed['R']) {
        finished = false;
        score[0] = 0;
        score[1] = 0;
        resetBall();
        resetPlayers();
        startTime = time(nullptr);
    }
    if (!finished) {
        if (players[0].keyPressed['d']) players[0].x += players[0].speed;
        else if (players[0].keyPressed['a']) players[0].x -= players[0].speed;
        if (players[0].keyPressed['w']) players[0].y += players[0].speed;
        else if (players[0].keyPressed['s']) players[0].y -= players[0].speed;
        if (players[0].keyPressed[' '] && players[0].z == 0) players[0].velocityZ = 8.0; 

        if (players[1].keyPressed['6']) players[1].x += players[1].speed;
        else if (players[1].keyPressed['4']) players[1].x -= players[1].speed;
        if (players[1].keyPressed['8']) players[1].y += players[1].speed;
        else if (players[1].keyPressed['5']) players[1].y -= players[1].speed;
        if (players[1].keyPressed['\r'] && players[1].z == 0) players[1].velocityZ = 8.0; 
    }
    
    glutPostRedisplay(); 
    glutTimerFunc(16, update, 0); 
}
void handleKeypress(unsigned char key, int x, int y) {
    players[0].keyPressed[key] = true; 
    players[1].keyPressed[key] = true; 
}

void handleKeyRelease(unsigned char key, int x, int y) {
    players[0].keyPressed[key] = false; 
    players[1].keyPressed[key] = false; 
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1060, 700);
    glutCreateWindow("BOLA V2");


    players[0].x = 860.0;
    players[0].y = 350.0;
    players[0].z = 20.0;
    players[0].velocityX = 0.0;
    players[0].velocityY = 0.0;
    players[0].velocityZ = 0.0;
    players[0].size = 10.0;
    players[0].speed = 5.0;
    players[0].color[0] = 0.0; 
    players[0].color[1] = 0.0; 
    players[0].color[2] = 1.0; 
    for (int i = 0; i < 256; ++i) {
        players[0].keyPressed[i] = false;
    }

   
    players[1].x = 200.0;
    players[1].y = 350.0;
    players[1].z = 20.0;
    players[1].velocityX = 0.0;
    players[1].velocityY = 0.0;
    players[1].velocityZ = 0.0;
    players[1].size = 10.0;
    players[1].speed = 5.0;
    players[1].color[0] = 1.0; 
    players[1].color[1] = 0.0; 
    players[1].color[2] = 0.0; 
    
    for (int i = 0; i < 256; ++i) {
        players[1].keyPressed[i] = false;
    }

    init();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(handleKeypress);
    glutKeyboardUpFunc(handleKeyRelease);

    glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}

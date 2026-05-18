/*
 * Bangladesh Panorama - 4 Scenes Combined
 *
 * Press 1  ->  Scene 1 : Barisal Village (Guava Garden)
 * Press 2  ->  Scene 2 : Three River Point View  (DEFAULT)
 * Press 3  ->  Scene 3 : Highway Road - Cumilla Moynamoti
 * Press 4  ->  Scene 4 : Expressway / City View
 *
 *  Scene 1 Controls
 *   R = Rain ON          E = Rain OFF
 *   N = Night mode       D = Day mode
 *   A = Car forward      B = Car backward      S = Car stop
 *   Right-click = Boat speed up   Left-click = Boat slow / stop
 *
 *  Scene 2 Controls
 *   D = Day    N = Night    R = Rain
 *   Arrow Left / Right = move ship
 *   Left-click hold = slow    Right-click hold = fast
 *
 *  Scene 3 Controls
 *   M = Day mode    N = Night mode
 *   R = Rain intensity UP (hold to increase gradually)
 *   E = Rain intensity DOWN (hold to decrease gradually)
 *   T = Rain OFF (instant)
 *   U = Shuffle car lanes    V = Restore original lanes
 *   Arrow UP = Stop vehicles    Arrow DOWN = Resume vehicles
 *   Left-click = slow    Right-click = fast
 *   ESC = reset scene 3 to day
 *
 *  Scene 4 Controls
 *   D = Day    N = Night    R = Rain ON    S = Rain OFF
 *   Left-click = fast    Right-click = stop
 */

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>


#define PI 3.14159265358979323846f

//  GLOBAL: which scene is active  (1-4)
int currentScene = 1; // start on River View

//  SHARED RAIN BUFFER
#define NUM_DROPS 120
float s2_rainX[NUM_DROPS];
float s2_rainY[NUM_DROPS];
float s4_rainX[NUM_DROPS];
float s4_rainY[NUM_DROPS];

//  SHARED UTILITY: draw filled circle (all scenes)

void drawCirclePx(float cx, float cy, float r, int segs) {
  glBegin(GL_TRIANGLE_FAN);
  for (int i = 0; i < segs; i++) {
    float theta = 2.0f * PI * (float)i / (float)segs;
    glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
  }
  glEnd();
}

// Scene-4 Bresenham circle fill
static int s4_xc, s4_yc;
void s4_plot_point(int x, int y) {
  glBegin(GL_QUADS);
  glVertex2f((float)(s4_xc + x), (float)(s4_yc + y));
  glVertex2f((float)(s4_xc + x), (float)(s4_yc - y));
  glVertex2f((float)(s4_xc + y), (float)(s4_yc - x));
  glVertex2f((float)(s4_xc + y), (float)(s4_yc + x));
  glVertex2f((float)(s4_xc - x), (float)(s4_yc - y));
  glVertex2f((float)(s4_xc - x), (float)(s4_yc + y));
  glVertex2f((float)(s4_xc - y), (float)(s4_yc + x));
  glVertex2f((float)(s4_xc - y), (float)(s4_yc - x));
  glEnd();
}
void s4_Circle(int cx, int cy, int r) {
  s4_xc = cx;
  s4_yc = cy;
  int x = 0, y = r, pk = 1 - r;
  s4_plot_point(x, y);
  while (x < y) {
    x++;
    if (pk < 0)
      pk = pk + 2 * x + 1;
    else {
      y--;
      pk = pk + 2 * (x - y) + 1;
    }
    s4_plot_point(x, y);
  }
}
//---------------------------------------------------------------------------------------------------------------

//  SCENE 1 - Barisal Village / Guava Garden

// --- S1 state ---
float s1_run3 = 0.0f;
float s1_bird = 0.0f, s1_birdWing = 0.0f;
float s1_run2 = 0.0f;
int s1_rainday = 0, s1_night = 0;
float s1_carSpeed = 0.0f;
float s1_boat1X = 100.0f, s1_boat2X = 600.0f, s1_boatSpeed = 1.0f;

void s1_drawCircle(float cx, float cy, float r) {
  drawCirclePx(cx, cy, r, 100);
}

void s1_drawTree(float x, float y, float s) {
  glColor3f(0.4f, 0.2f, 0.0f);
  glBegin(GL_QUADS);
  glVertex2f(x - 5 * s, y);
  glVertex2f(x + 5 * s, y);
  glVertex2f(x + 5 * s, y + 40 * s);
  glVertex2f(x - 5 * s, y + 40 * s);
  glEnd();
  glColor3f(0.0f, 0.7f, 0.1f);
  glBegin(GL_TRIANGLES);
  glVertex2f(x - 30 * s, y + 30 * s);
  glVertex2f(x, y + 90 * s);
  glVertex2f(x + 30 * s, y + 30 * s);
  glEnd();
  glBegin(GL_TRIANGLES);
  glVertex2f(x - 22 * s, y + 55 * s);
  glVertex2f(x, y + 115 * s);
  glVertex2f(x + 22 * s, y + 55 * s);
  glEnd();
}
void s1_drawForest(float startX, float endX, float y) {
  for (float x = startX; x <= endX; x += 70) {
    glColor3f(0.0f, 0.5f, 0.0f);
    s1_drawTree(x, y, 0.70f);
    glColor3f(0.0f, 0.65f, 0.0f);
    s1_drawTree(x + 25, y + 15, 0.60f);
  }
}
void s1_drawCloud(float x, float y) {
  glColor3f(1, 1, 1);
  s1_drawCircle(x, y, 40);
  s1_drawCircle(x - 35, y - 10, 30);
  s1_drawCircle(x + 35, y - 10, 30);
  s1_drawCircle(x + 5, y - 20, 35);
}
void s1_drawRainCloud(float x, float y) {
  glColor3f(0.38f, 0.38f, 0.42f);
  s1_drawCircle(x, y, 50);
  s1_drawCircle(x - 45, y - 12, 38);
  s1_drawCircle(x + 45, y - 12, 38);
  s1_drawCircle(x + 5, y - 25, 42);
  s1_drawCircle(x - 20, y - 30, 32);
  s1_drawCircle(x + 20, y - 30, 32);
}
void s1_drawRoundTree(float x, float y, float s) {
  glColor3f(0.45f, 0.25f, 0.05f);
  glBegin(GL_QUADS);
  glVertex2f(x - 6 * s, y);
  glVertex2f(x + 6 * s, y);
  glVertex2f(x + 6 * s, y + 45 * s);
  glVertex2f(x - 6 * s, y + 45 * s);
  glEnd();
  glColor3f(0.0f, 0.55f, 0.0f);
  s1_drawCircle(x, y + 75 * s, 28 * s);
  s1_drawCircle(x - 18 * s, y + 60 * s, 24 * s);
  s1_drawCircle(x + 18 * s, y + 60 * s, 24 * s);
  s1_drawCircle(x, y + 45 * s, 22 * s);
}
void s1_drawHouse(float x, float y) {
  glPushMatrix();
  glTranslatef(x, y, 0);
  glColor3ub(210, 170, 80);
  glBegin(GL_QUADS);
  glVertex2f(0, 0);
  glVertex2f(0, 70);
  glVertex2f(180, 70);
  glVertex2f(180, 0);
  glEnd();
  glColor3ub(120, 40, 40);
  glBegin(GL_TRIANGLES);
  glVertex2f(-15, 70);
  glVertex2f(90, 130);
  glVertex2f(195, 70);
  glEnd();
  glColor3f(1, 1, 1);
  glBegin(GL_QUADS);
  glVertex2f(75, 0);
  glVertex2f(75, 45);
  glVertex2f(105, 45);
  glVertex2f(105, 0);
  glEnd();
  glColor3f(0.7f, 0.9f, 1.0f);
  glBegin(GL_QUADS);
  glVertex2f(20, 30);
  glVertex2f(20, 55);
  glVertex2f(50, 55);
  glVertex2f(50, 30);
  glEnd();
  glBegin(GL_QUADS);
  glVertex2f(130, 30);
  glVertex2f(130, 55);
  glVertex2f(160, 55);
  glVertex2f(160, 30);
  glEnd();
  glPopMatrix();
}
void s1_drawFence(float x, float y, float width) {
  float postW = 10, postH = 55, spacing = 14, railH = 7;
  float railY1 = y + postH * 0.68f, railY2 = y + postH * 0.32f;
  int numPosts = (int)(width / (postW + spacing)) + 1;
  glColor3ub(80, 50, 15);
  glBegin(GL_QUADS);
  glVertex2f(x, y - 3);
  glVertex2f(x + width, y - 3);
  glVertex2f(x + width, y + 2);
  glVertex2f(x, y + 2);
  glEnd();
  float railYs[2] = {railY1, railY2};
  for (int ri = 0; ri < 2; ri++) {
    float ry = railYs[ri];
    glColor3ub(90, 55, 18);
    glBegin(GL_QUADS);
    glVertex2f(x, ry + 2);
    glVertex2f(x + width, ry + 2);
    glVertex2f(x + width, ry + railH + 2);
    glVertex2f(x, ry + railH + 2);
    glEnd();
    glColor3ub(150, 95, 40);
    glBegin(GL_QUADS);
    glVertex2f(x, ry);
    glVertex2f(x + width, ry);
    glVertex2f(x + width, ry + railH);
    glVertex2f(x, ry + railH);
    glEnd();
  }
  for (int i = 0; i < numPosts; i++) {
    float px = x + i * (postW + spacing);
    glColor3ub(178, 118, 55);
    glBegin(GL_QUADS);
    glVertex2f(px + 2, y);
    glVertex2f(px + postW - 1, y);
    glVertex2f(px + postW - 1, y + postH);
    glVertex2f(px + 2, y + postH);
    glEnd();
    glColor3ub(178, 118, 55);
    glBegin(GL_TRIANGLES);
    glVertex2f(px + 2, y + postH);
    glVertex2f(px + postW / 2.0f, y + postH + 15);
    glVertex2f(px + postW - 1, y + postH);
    glEnd();
  }
}
void s1_drawCar(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    if (s1_night) {
        glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 1.0f, 0.6f, 0.18f);
        glVertex2f(120, 22);
        glVertex2f(340, -15);
        glVertex2f(340, 45);
        glEnd();
        glBegin(GL_TRIANGLES);
        glColor4f(1.0f, 1.0f, 0.8f, 0.35f);
        glVertex2f(120, 22);
        glVertex2f(280, 0);
        glVertex2f(280, 35);
        glEnd();
        glColor3f(1.0f, 1.0f, 0.7f);
        s1_drawCircle(120, 22, 5);
        glColor3f(1.0f, 0.0f, 0.0f);
        s1_drawCircle(0, 20, 4);
    }
    else {
        glColor3f(1.0f, 1.0f, 0.9f);
        s1_drawCircle(120, 22, 4);
        glColor3f(0.8f, 0.8f, 0.8f);
        s1_drawCircle(118, 22, 2);
        glColor3f(0.8f, 0.0f, 0.0f);
        s1_drawCircle(0, 20, 3);
    }

    glColor3f(1, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(0, 30);
    glVertex2f(120, 30);
    glVertex2f(120, 0);
    glEnd();
    glColor3f(0.8f, 0, 0);
    glBegin(GL_QUADS);
    glVertex2f(20, 30);
    glVertex2f(35, 55);
    glVertex2f(90, 55);
    glVertex2f(105, 30);
    glEnd();
    if (s1_night)
        glColor3f(0.2f, 0.3f, 0.4f);
    else
        glColor3f(0.7f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(35, 35);
    glVertex2f(35, 50);
    glVertex2f(55, 50);
    glVertex2f(55, 35);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(65, 35);
    glVertex2f(65, 50);
    glVertex2f(85, 50);
    glVertex2f(85, 35);
    glEnd();
    glColor3f(0, 0, 0);
    s1_drawCircle(25, 0, 14);
    s1_drawCircle(95, 0, 14);
    glColor3f(0.8f, 0.8f, 0.8f);
    s1_drawCircle(25, 0, 7);
    s1_drawCircle(95, 0, 7);
    glPopMatrix();
}
void s1_drawBoat(float x, float y) {
  glPushMatrix();
  glTranslatef(x, y, 0);
  glColor3f(0.8f, 0.3f, 0.1f);
  glBegin(GL_POLYGON);
  glVertex2f(0, 0);
  glVertex2f(20, -15);
  glVertex2f(90, -15);
  glVertex2f(110, 0);
  glEnd();
  glColor3f(0.3f, 0.2f, 0.0f);
  glLineWidth(2);
  glBegin(GL_LINES);
  glVertex2f(50, 0);
  glVertex2f(50, 50);
  glEnd();
  glLineWidth(1);
  glColor3f(1, 1, 1);
  glBegin(GL_TRIANGLES);
  glVertex2f(50, 50);
  glVertex2f(50, 0);
  glVertex2f(85, 25);
  glEnd();
  glPopMatrix();
}
void s1_drawBird(float x, float y, float wing) {
  glColor3f(0, 0, 0);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x - 15, y + sinf(wing) * 10);
  glVertex2f(x, y);
  glVertex2f(x + 15, y + sinf(wing) * 10);
  glEnd();
}
void s1_drawRain() {
  glColor3f(0.7f, 0.8f, 1.0f);
  for (int i = 0; i < 300; i++) {
    float x = (float)(rand() % 1000), y = (float)(rand() % 1000);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + 5, y - 15);
    glEnd();
  }
}
void s1_display() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 1000, 0, 1000);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  if (s1_night)
    glColor3f(0.02f, 0.05f, 0.20f);
  else if (s1_rainday)
    glColor3f(0.30f, 0.32f, 0.38f);
  else
    glColor3f(0.0f, 0.6f, 0.9f);
  glBegin(GL_QUADS);
  glVertex2i(0, 550);
  glVertex2i(1000, 550);
  glVertex2i(1000, 1000);
  glVertex2i(0, 1000);
  glEnd();

  s1_drawForest(0, 1000, 520);
  s1_drawForest(0, 1000, 490);
  s1_drawForest(0, 1000, 450);

  glColor3ub(0, (s1_rainday ? 100 : 128), 0);
  glBegin(GL_QUADS);
  glVertex2i(0, 550);
  glVertex2i(1000, 550);
  glVertex2i(1000, 0);
  glVertex2i(0, 0);
  glEnd();

  if (!s1_night) {
    float r = s1_rainday ? 0.85f : 1.0f, g = s1_rainday ? 0.75f : 0.9f,
          b = s1_rainday ? 0.50f : 0.0f;
    glColor3f(r, g, b);
    s1_drawCircle(900, 880, 35);
  } else {
    glColor3f(1.0f, 1.0f, 0.8f);
    s1_drawCircle(900, 880, 30);
  }

  glPushMatrix();
  glTranslatef(s1_run3, 0, 0);
  if (s1_rainday) {
    s1_drawRainCloud(150, 880);
    s1_drawRainCloud(450, 920);
    s1_drawRainCloud(250, 770);
    s1_drawRainCloud(700, 850);
  } else {
    s1_drawCloud(150, 880);
    s1_drawCloud(450, 920);
    s1_drawCloud(250, 770);
    s1_drawCloud(700, 850);
  }
  glPopMatrix();

  glColor3f(s1_rainday ? 0.25f : 0.3f, s1_rainday ? 0.45f : 0.6f,
            s1_rainday ? 0.70f : 1.0f);
  glBegin(GL_QUADS);
  glVertex2i(0, 80);
  glVertex2i(1000, 80);
  glVertex2i(1000, 220);
  glVertex2i(0, 220);
  glEnd();

  s1_drawBoat(s1_boat1X, 170);
  s1_drawBoat(s1_boat2X, 140);

  glColor3f(0.3f, 0.3f, 0.3f);
  glBegin(GL_QUADS);
  glVertex2i(0, 280);
  glVertex2i(1000, 280);
  glVertex2i(1000, 330);
  glVertex2i(0, 330);
  glEnd();
  glColor3f(1, 1, 0);
  for (int i = 0; i < 20; i++) {
    glBegin(GL_QUADS);
    glVertex2f(i * 55.f, 302);
    glVertex2f(i * 55.f + 30, 302);
    glVertex2f(i * 55.f + 30, 307);
    glVertex2f(i * 55.f, 307);
    glEnd();
  }

  float treeData[][3] = {
      {40, 330, .35f},  {100, 330, .35f}, {160, 330, .35f}, {50, 430, .80f},
      {100, 430, .86f}, {240, 330, .35f}, {300, 330, .35f}, {360, 330, .35f},
      {390, 460, .80f}, {470, 330, .35f}, {540, 330, .35f}, {610, 330, .35f},
      {625, 460, .80f}, {710, 330, .35f}, {780, 330, .35f}, {850, 330, .35f},
      {890, 430, .80f}, {950, 430, .86f}, {940, 330, .35f}, {70, 235, .45f},
      {150, 230, .40f}, {250, 240, .45f}, {300, 235, .40f}, {400, 230, .40f},
      {500, 240, .45f}, {600, 235, .40f}, {660, 230, .45f}, {700, 235, .40f},
      {770, 230, .45f}, {820, 240, .45f}, {930, 230, .40f}, {60, 20, .45f},
      {140, 25, .40f},  {230, 15, .50f},  {340, 20, .45f},  {430, 18, .40f},
      {520, 22, .50f},  {640, 18, .45f},  {730, 25, .40f},  {820, 15, .50f},
      {920, 20, .45f}};
  for (int i = 0; i < 41; i++)
    s1_drawTree(treeData[i][0], treeData[i][1], treeData[i][2]);

  s1_drawHouse(180, 390);
  s1_drawFence(360, 393, 60);
  s1_drawFence(13, 393, 167);
  s1_drawHouse(420, 390);
  s1_drawFence(600, 393, 55);
  s1_drawHouse(650, 390);
  s1_drawFence(830, 393, 155);

  glColor3f(0.55f, 0.45f, 0.35f);
  float pathW[][4] = {
      {255, 390, 285, 330}, {495, 390, 525, 330}, {725, 390, 755, 330}};
  for (int i = 0; i < 3; i++) {
    glBegin(GL_QUADS);
    glVertex2f(pathW[i][0], pathW[i][1]);
    glVertex2f(pathW[i][2], pathW[i][1]);
    glVertex2f(pathW[i][2], pathW[i][3]);
    glVertex2f(pathW[i][0], pathW[i][3]);
    glEnd();
  }

  s1_drawRoundTree(120, 520, 1.5f);
  s1_drawRoundTree(450, 530, 0.9f);
  s1_drawRoundTree(580, 525, 1.3f);
  s1_drawRoundTree(860, 530, 1.5f);
  s1_drawRoundTree(720, 530, 0.8f);
  s1_drawRoundTree(260, 525, 0.9f);

  glPushMatrix();
  glTranslatef(s1_run2, 0, 0);
  s1_drawCar(-250, 295);
  glPopMatrix();

  if (!s1_rainday) {
    glPushMatrix();
    glTranslatef(s1_bird, 0, 0);
    s1_drawBird(100, 900, s1_birdWing);
    s1_drawBird(150, 930, s1_birdWing + 1);
    glPopMatrix();
  }
  if (s1_rainday)
    s1_drawRain();
}

// ---------------------------------------------------------------------------------------------------------------
//  SCENE 2 - Three River Point View
// ---------------------------------------------------------------------------------------------------------------
float s2_move = 0, s2_move2 = 0, s2_mcloud2 = 0, s2_mcloud3 = 0;
float s2_fishX = -40, s2_fishDir = 1;
float s2_a = -1;
float s2_speedMul = 1;
int s2_mode = 0; // 0=day 1=night 2=rain

void s2_DrawCircle(float cx, float cy, float r, int seg) {
  drawCirclePx(cx, cy, r, seg);
}

void s2_fish() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();
  glTranslatef(s2_fishX, -18, 0);
  glScalef((s2_fishDir < 0) ? -1.5f : 1.5f, 1.5f, 1.0f);
  glColor3ub(198, 195, 181);
  s2_DrawCircle(0, 0, 2, 80);
  glColor3ub(102, 99, 98);
  s2_DrawCircle(0.2f, -0.45f, 1.05f, 60);
  glBegin(GL_TRIANGLES);
  glColor3ub(102, 99, 98);
  glVertex2f(-1.9f, 0);
  glVertex2f(-4.5f, 1.8f);
  glVertex2f(-4.5f, -1.8f);
  glEnd();
  glBegin(GL_TRIANGLES);
  glColor3ub(102, 99, 98);
  glVertex2f(-0.5f, 1.9f);
  glVertex2f(1.2f, 1.9f);
  glVertex2f(0.4f, 3.5f);
  glEnd();
  glBegin(GL_TRIANGLES);
  glColor3ub(102, 99, 98);
  glVertex2f(0.4f, -1.8f);
  glVertex2f(1.8f, -1.8f);
  glVertex2f(1.0f, -3.1f);
  glEnd();
  glColor3ub(0, 0, 0);
  s2_DrawCircle(1.2f, 0.5f, 0.38f, 30);
  glColor3ub(255, 255, 255);
  s2_DrawCircle(1.34f, 0.64f, 0.12f, 15);
  glBegin(GL_LINES);
  glColor3ub(160, 50, 0);
  glVertex2f(1.93f, 0.08f);
  glVertex2f(2.5f, -0.18f);
  glEnd();
  glPopMatrix();
}
void s2_tree() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  float td[][4] = {{-32, 10, -32, 20}, {-25, 10, -25, 20}, {-10, 10, -10, 20}};
  for (int i = 0; i < 3; i++) {
    glBegin(GL_POLYGON);
    glColor3ub(68, 70, 68);
    glVertex2f(td[i][0] - .75f, td[i][1]);
    glVertex2f(td[i][0] - .75f, td[i][1] + 8);
    glVertex2f(td[i][0] + .75f, td[i][1] + 8);
    glVertex2f(td[i][0] + .75f, td[i][1]);
    glEnd();
    float tx = td[i][2], ty = td[i][3];
    glBegin(GL_POLYGON);
    glColor3ub(88, 193, 90);
    glVertex2f(tx - 3, ty - 5);
    glVertex2f(tx + 3, ty - 5);
    glVertex2f(tx, ty + 2);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3ub(88, 193, 90);
    glVertex2f(tx - 3, ty - 3);
    glVertex2f(tx + 3, ty - 3);
    glVertex2f(tx, ty + 4);
    glEnd();
  }
}
void s2_ship() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();
  glTranslatef(s2_a, 0, 0);
  glBegin(GL_POLYGON);
  glColor3ub(68, 70, 68);
  glVertex2f(-38, -25);
  glVertex2f(2, -25);
  glVertex2f(-8, -38);
  glVertex2f(-28, -38);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(201, 190, 49);
  glVertex2f(-28, -38);
  glVertex2f(-8, -38);
  glVertex2f(-7, -37);
  glVertex2f(-27, -37);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(204, 102, 0);
  glVertex2f(-38, -25);
  glVertex2f(2, -25);
  glVertex2f(2, -24);
  glVertex2f(-38, -24);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(160, 160, 160);
  glVertex2f(-35.5f, -24);
  glVertex2f(-0.5f, -24);
  glVertex2f(-0.5f, -20);
  glVertex2f(-35.5f, -20);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(204, 0, 204);
  glVertex2f(-34.5f, -20);
  glVertex2f(-0.5f, -20);
  glVertex2f(-3, -15);
  glVertex2f(-34.5f, -15);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(204, 255, 255);
  glVertex2f(-9, -19);
  glVertex2f(-1, -19);
  glVertex2f(-2.5f, -16);
  glVertex2f(-9, -16);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(255, 128, 0);
  glVertex2f(-34.5f, -15);
  glVertex2f(-3, -15);
  glVertex2f(-5, -13);
  glVertex2f(-33, -13);
  glEnd();
  float cols[3][2] = {{-28, -25}, {-21, -17}, {-13, -9}};
  for (int i = 0; i < 3; i++) {
    glBegin(GL_POLYGON);
    glColor3ub(0, 0, 255);
    glVertex2f(cols[i][0], -13);
    glVertex2f(cols[i][0] + 3, -13);
    glVertex2f(cols[i][0] + 3, -8);
    glVertex2f(cols[i][0], -8);
    glEnd();
    glBegin(GL_POLYGON);
    glColor3ub(0, 255, 255);
    glVertex2f(cols[i][0], -8);
    glVertex2f(cols[i][0] + 3, -8);
    glVertex2f(cols[i][0] + 3, -7);
    glVertex2f(cols[i][0], -7);
    glEnd();
  }
  float pts[][3] = {
      {-31, -29, 1.8f},    {-22, -29, 1.8f},    {-14, -29, 1.8f},
      {-6, -29, 1.8f},     {-29, -18, 1.0f},    {-23, -18, 1.0f},
      {-18, -18, 1.0f},    {-12, -18, 1.0f},    {-30, -21.5f, 1.2f},
      {-24, -21.5f, 1.2f}, {-19, -21.5f, 1.2f}, {-13, -21.5f, 1.2f},
      {-7, -21.5f, 1.2f}};
  glColor3ub(255, 255, 255);
  for (int i = 0; i < 13; i++)
    s2_DrawCircle(pts[i][0], pts[i][1], pts[i][2], 200);
  glPopMatrix();
}
void s2_boat2() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();
  glTranslatef(s2_move2 + 28, -7.5f, 0);
  glScalef(0.85f, 0.85f, 1);
  glBegin(GL_POLYGON);
  glColor3ub(0, 220, 0);
  glVertex2f(-8, -5);
  glVertex2f(8, -5);
  glVertex2f(11, 0);
  glVertex2f(-9, 0);
  glEnd();
  glBegin(GL_TRIANGLES);
  glColor3ub(220, 0, 0);
  glVertex2f(0, 1);
  glVertex2f(5, 1);
  glVertex2f(0, 14);
  glEnd();
  glBegin(GL_TRIANGLES);
  glColor3ub(220, 0, 0);
  glVertex2f(-1, 1);
  glVertex2f(3, 1);
  glVertex2f(-1, 8);
  glEnd();
  glPopMatrix();
}
void s2_boat1() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();
  glTranslatef(s2_move + 8, 2.5f, 0);
  glScalef(0.75f, 0.75f, 1);
  glBegin(GL_POLYGON);
  glColor3ub(0, 210, 0);
  glVertex2f(-8, -5);
  glVertex2f(6, -5);
  glVertex2f(9, 0);
  glVertex2f(-9, 0);
  glEnd();
  glBegin(GL_TRIANGLES);
  glColor3ub(255, 30, 30);
  glVertex2f(0, 1);
  glVertex2f(6, 1);
  glVertex2f(0, 15);
  glEnd();
  glBegin(GL_TRIANGLES);
  glColor3ub(255, 30, 30);
  glVertex2f(-6, 1);
  glVertex2f(-1, 1);
  glVertex2f(-1, 8);
  glEnd();
  glPopMatrix();
}
void s2_drawCloud(float tx, float pty[][2], int cnt, int c) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();
  glTranslatef(tx, 0, 0);
  glColor3ub(c, c, c);
  for (int i = 0; i < cnt; i++)
    s2_DrawCircle(pty[i][0], pty[i][1], 3, 200);
  glPopMatrix();
}
void s2_sun() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  /* Sun/Moon drawn at fixed position - no movement */
  if (s2_mode == 1) {
    glColor3ub(230, 230, 200);
    s2_DrawCircle(35, 34, 3, 200);
    glColor3ub(20, 24, 82);
    s2_DrawCircle(36.2f, 34.8f, 2.3f, 200);
  } else {
    glColor3ub(255, 255, 0);
    s2_DrawCircle(35, 34, 3, 200);
  }
}
void s2_rain() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glLineWidth(1.5f);
  glBegin(GL_LINES);
  glColor3ub(150, 200, 255);
  for (int i = 0; i < NUM_DROPS; i++) {
    glVertex2f(s2_rainX[i], s2_rainY[i]);
    glVertex2f(s2_rainX[i] + 0.3f, s2_rainY[i] - 2.5f);
  }
  glEnd();
  glLineWidth(1);
}
void s2_display() {
  if (s2_mode == 0)
    glClearColor(0.588f, 0.839f, 0.965f, 1);
  else if (s2_mode == 1)
    glClearColor(0.05f, 0.05f, 0.25f, 1);
  else
    glClearColor(0.4f, 0.4f, 0.5f, 1);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if (s2_mode == 0)
    glColor3ub(150, 214, 246);
  else if (s2_mode == 1)
    glColor3ub(13, 13, 64);
  else
    glColor3ub(100, 100, 120);
  glBegin(GL_POLYGON);
  glVertex2f(-40, 40);
  glVertex2f(40, 40);
  glVertex2f(40, 10);
  glVertex2f(-40, 10);
  glEnd();

  s2_sun();

  if (s2_mode == 1) {
    float sp[][2] = {{-35, 38}, {-30, 36}, {-20, 39}, {-15, 37}, {-5, 38},
                     {5, 36},   {15, 39},  {25, 37},  {32, 38},  {38, 36},
                     {-38, 33}, {-28, 31}, {0, 32},   {20, 34},  {30, 31}};
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-40, 40, -40, 40);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3ub(255, 255, 255);
    for (int i = 0; i < 15; i++)
      s2_DrawCircle(sp[i][0], sp[i][1], 0.3f, 10);
  }

  int cc = (s2_mode == 1) ? 180 : 255;
  float c1[][2] = {{-26, 34}, {-24, 35}, {-24, 33},
                   {-22, 35}, {-22, 33}, {-20, 34}};
  float c2[][2] = {{-12, 34}, {-10, 35}, {-8, 33},
                   {-6, 35},  {-4, 33},  {-2, 34}};
  float c3[][2] = {{8, 34}, {9, 35}, {10, 33}, {11, 35}, {14, 33}, {17, 34}};
  s2_drawCloud(s2_move, c1, 6, cc);
  s2_drawCloud(s2_mcloud2, c2, 6, cc);
  s2_drawCloud(s2_mcloud3, c3, 6, cc);
  s2_tree();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  if (s2_mode == 0)
    glColor3ub(46, 155, 209);
  else if (s2_mode == 1)
    glColor3ub(10, 60, 110);
  else
    glColor3ub(30, 100, 150);
  glBegin(GL_POLYGON);
  glVertex2f(-40, 10);
  glVertex2f(40, 10);
  glVertex2f(40, -40);
  glVertex2f(-40, -40);
  glEnd();

  glBegin(GL_POLYGON);
  glColor3ub(88, 193, 90);
  glVertex2f(-35, 10);
  glVertex2f(-28, 15);
  glVertex2f(-23, 10);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(24, 156, 38);
  glVertex2f(-24, 10);
  glVertex2f(-17, 20);
  glVertex2f(-10, 10);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(24, 156, 38);
  glVertex2f(-11, 10);
  glVertex2f(0, 15);
  glVertex2f(10, 10);
  glEnd();

  s2_fish();
  s2_boat1();
  s2_boat2();
  s2_ship();
  if (s2_mode == 2)
    s2_rain();
}

// ---------------------------------------------------------------------------------------------------------------
//  SCENE 3 - Highway Road / Cumilla Moynamoti
// ---------------------------------------------------------------------------------------------------------------
float s3_carOnePos = 0, s3_carTwoPos = 800;
float s3_cloudPos = 0, s3_birdX = 0;
float s3_rainLevel = 0.0f;
int s3_isRaining = 0;
int s3_stopped = 0, s3_night = 0;
float s3_speed = 1.0f;
int s3_laneSwapped = 0;
float s3_car1YOff = 0.0f;
float s3_car2YOff = 0.0f;

void s3_dc(float x, float y, float r) { drawCirclePx(x, y, r, 100); }

void s3_road() {
  glBegin(GL_POLYGON);
  glColor3f(0.2f, 0.2f, 0.2f);
  glVertex2i(0, 200);
  glVertex2i(800, 200);
  glVertex2i(800, 400);
  glVertex2i(0, 400);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(153, 102, 51);
  glVertex2i(0, 380);
  glVertex2i(800, 380);
  glVertex2i(800, 405);
  glVertex2i(0, 405);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(153, 102, 51);
  glVertex2i(0, 200);
  glVertex2i(800, 200);
  glVertex2i(800, 220);
  glVertex2i(0, 220);
  glEnd();
  glBegin(GL_LINES);
  glColor3f(1, 1, 1);
  glVertex2i(0, 220);
  glVertex2i(800, 220);
  glEnd();
  int dv = 0;
  glBegin(GL_LINES);
  glColor3f(1, 1, 1);
  for (int i = 0; i < 20; i++) {
    glVertex2i(dv + 20, 300);
    glVertex2i(dv + 40, 300);
    dv += 40;
  }
  glEnd();
  int cx1 = 0, cx2 = 40;
  for (int i = 0; i < 20; i++) {
    glBegin(GL_POLYGON);
    glColor3ub((i % 2 == 0) ? 1 : 217, (i % 2 == 0) ? 1 : 217,
               (i % 2 == 0) ? 1 : 217);
    glVertex2i(cx1, 180);
    glVertex2i(cx2, 180);
    glVertex2i(cx2, 210);
    glVertex2i(cx1, 210);
    glEnd();
    cx1 = cx2;
    cx2 += 40;
  }
  int tx1 = 0, tx2 = 40;
  for (int i = 0; i < 20; i++) {
    glBegin(GL_POLYGON);
    glColor3ub((i % 2 == 1) ? 1 : 217, (i % 2 == 1) ? 1 : 217,
               (i % 2 == 1) ? 1 : 217);
    glVertex2i(tx1, 400);
    glVertex2i(tx2, 400);
    glVertex2i(tx2, 420);
    glVertex2i(tx1, 420);
    glEnd();
    tx1 = tx2;
    tx2 += 40;
  }
}
void s3_grass() {
  glBegin(GL_POLYGON);
  glColor3f(0, 0.80f, 0);
  glVertex2i(0, 0);
  glVertex2i(800, 0);
  glVertex2i(800, 180);
  glVertex2i(0, 180);
  glEnd();
  int td = 0;
  for (int i = 0; i < 4; i++) {
    glBegin(GL_POLYGON);
    glColor3ub(102, 51, 0);
    glVertex2i(60 + td, 30);
    glVertex2i(70 + td, 30);
    glVertex2i(65 + td, 150);
    glEnd();
    glColor3f(0, 0.6f, 0);
    s3_dc(50 + td, 100, 20);
    s3_dc(80 + td, 100, 20);
    s3_dc(58 + td, 130, 18);
    s3_dc(72 + td, 130, 18);
    s3_dc(65 + td, 150, 14);
    td += 200;
  }
  td = 100;
  for (int i = 0; i < 4; i++) {
    glBegin(GL_POLYGON);
    glColor3ub(102, 51, 0);
    glVertex2i(60 + td, 100);
    glVertex2i(70 + td, 100);
    glVertex2i(65 + td, 220);
    glEnd();
    glColor3f(0, 0.6f, 0);
    s3_dc(50 + td, 170, 20);
    s3_dc(80 + td, 170, 20);
    s3_dc(58 + td, 200, 18);
    s3_dc(72 + td, 200, 18);
    s3_dc(65 + td, 220, 14);
    td += 200;
  }
}
void s3_birds() {
  glColor3ub(0, 0, 0);
  glLineWidth(2);
  for (int i = 0; i < 6; i++) {
    float bx = s3_birdX + i * 40.f, by = 650 + (i % 2 == 0 ? 30.f : 0.f);
    glBegin(GL_LINE_STRIP);
    glVertex2f(bx, by + 10);
    glVertex2f(bx + 10, by + 20);
    glVertex2f(bx + 20, by + 10);
    glVertex2f(bx + 30, by + 20);
    glVertex2f(bx + 40, by + 10);
    glEnd();
  }
  glLineWidth(1);
}
void s3_clouds() {
  glColor3f(s3_night ? 0.75f : 1, s3_night ? 0.75f : 1, s3_night ? 0.75f : 1);
  s3_dc(100 + s3_cloudPos, 730, 33);
  s3_dc(55 + s3_cloudPos, 730, 23);
  s3_dc(145 + s3_cloudPos, 730, 23);
  s3_dc(400 + s3_cloudPos, 730, 33);
  s3_dc(355 + s3_cloudPos, 730, 23);
  s3_dc(445 + s3_cloudPos, 730, 23);
}
void s3_hills() {
  glColor3ub(s3_night ? 20 : 80, s3_night ? 50 : 160, s3_night ? 20 : 60);
  s3_dc(150, 420, 200);
  glColor3ub(s3_night ? 15 : 60, s3_night ? 45 : 140, s3_night ? 15 : 50);
  s3_dc(400, 420, 250);
  glColor3ub(s3_night ? 20 : 80, s3_night ? 50 : 160, s3_night ? 20 : 60);
  s3_dc(650, 420, 220);
}
void s3_bgTrees() {
  int tx[] = {50, 150, 250, 350, 450, 550, 650, 750};
  for (int i = 0; i < 8; i++) {
    int x = tx[i];
    glColor3ub(s3_night ? 40 : 102, s3_night ? 20 : 51, 0);
    glBegin(GL_POLYGON);
    glVertex2i(x - 5, 420);
    glVertex2i(x + 5, 420);
    glVertex2i(x + 5, 480);
    glVertex2i(x - 5, 480);
    glEnd();
    glColor3ub(s3_night ? 10 : 0, s3_night ? 60 : 153, s3_night ? 10 : 0);
    s3_dc(x, 480, 25);
    s3_dc(x - 15, 460, 20);
    s3_dc(x + 15, 460, 20);
    s3_dc(x, 500, 20);
  }
}
void s3_building(float x, float w, float h, int r, int g, int b, int btype) {
  glColor3ub(s3_night ? r / 3 : r, s3_night ? g / 3 : g, s3_night ? b / 3 : b);
  glBegin(GL_POLYGON);
  glVertex2i((int)x, 420);
  glVertex2i((int)(x + w), 420);
  glVertex2i((int)(x + w), (int)(420 + h));
  glVertex2i((int)x, (int)(420 + h));
  glEnd();
  if (btype == 1) {
    glBegin(GL_TRIANGLES);
    glVertex2i((int)x, (int)(420 + h));
    glVertex2i((int)(x + w), (int)(420 + h));
    glVertex2i((int)(x + w / 2), (int)(420 + h + 40));
    glEnd();
  }
  int rows = (int)((h - 30) / 30), cols = (int)((w - 10) / 20);
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) {
      float wx = x + 10 + j * 20, wy = 440 + i * 30;
      if (s3_night)
        glColor3ub((i + j) % 3 == 0 ? 255 : 50, (i + j) % 3 == 0 ? 255 : 50,
                   (i + j) % 3 == 0 ? 100 : 50);
      else
        glColor3ub(200, 240, 255);
      glBegin(GL_POLYGON);
      glVertex2f(wx, wy);
      glVertex2f(wx + 10, wy);
      glVertex2f(wx + 10, wy + 15);
      glVertex2f(wx, wy + 15);
      glEnd();
    }
}
void s3_buildings() {
  s3_building(20, 70, 200, 200, 180, 150, 2);
  s3_building(110, 80, 180, 0, 150, 150, 0);
  s3_building(210, 80, 250, 255, 150, 50, 0);
  s3_building(310, 80, 280, 180, 180, 180, 2);
  s3_building(410, 90, 220, 150, 100, 200, 0);
  s3_building(520, 70, 260, 255, 150, 180, 1);
  s3_building(610, 80, 230, 255, 200, 50, 2);
  s3_building(710, 70, 190, 100, 180, 255, 0);
}
void s3_carOne() {
  glPushMatrix();
  glTranslatef(s3_carOnePos, s3_car1YOff, 0);
  glBegin(GL_POLYGON);
  glColor3ub(0, 50, 50);
  glVertex2i(0, 330);
  glVertex2i(120, 330);
  glVertex2i(120, 345);
  glVertex2i(115, 350);
  glVertex2i(90, 350);
  glVertex2i(70, 365);
  glVertex2i(40, 365);
  glVertex2i(20, 350);
  glVertex2i(0, 350);
  glEnd();
  if (s3_night) {
    glBegin(GL_TRIANGLES);
    glColor3f(.4f, .4f, .2f);
    glVertex2f(120, 345);
    glVertex2f(220, 325);
    glVertex2f(220, 365);
    glEnd();
  }
  glBegin(GL_POLYGON);
  glColor3ub(204, 204, 255);
  glVertex2i(25, 350);
  glVertex2i(85, 350);
  glVertex2i(67, 362);
  glVertex2i(43, 362);
  glEnd();
  glColor3f(0, 0, 0);
  s3_dc(15, 330, 10);
  s3_dc(90, 330, 10);
  glPopMatrix();
}
void s3_carTwo() {
  glPushMatrix();
  glTranslatef(s3_carTwoPos, s3_car2YOff, 0);
  glBegin(GL_POLYGON);
  glColor3ub(204, 0, 0);
  glVertex2i(0, 260);
  glVertex2i(5, 255);
  glVertex2i(120, 255);
  glVertex2i(120, 290);
  glVertex2i(5, 290);
  glVertex2i(0, 285);
  glEnd();
  if (s3_night) {
    glBegin(GL_TRIANGLES);
    glColor3f(0.8f, 0.8f, 0.2f);
    glVertex2f(0, 275);
    glVertex2f(-100, 250);
    glVertex2f(-100, 300);
    glEnd();
  }
  glBegin(GL_POLYGON);
  glColor3ub(204, 204, 255);
  glVertex2i(90, 270);
  glVertex2i(110, 270);
  glVertex2i(110, 280);
  glVertex2i(90, 280);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(204, 204, 255);
  glVertex2i(65, 270);
  glVertex2i(85, 270);
  glVertex2i(85, 280);
  glVertex2i(65, 280);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(204, 204, 255);
  glVertex2i(40, 270);
  glVertex2i(60, 270);
  glVertex2i(60, 280);
  glVertex2i(40, 280);
  glEnd();
  glBegin(GL_POLYGON);
  glColor3ub(204, 204, 255);
  glVertex2i(15, 270);
  glVertex2i(35, 270);
  glVertex2i(35, 280);
  glVertex2i(15, 280);
  glEnd();
  glColor3f(0, 0, 0);
  s3_dc(20, 255, 8);
  s3_dc(105, 255, 8);
  s3_dc(85, 255, 8);
  glPopMatrix();
}
void s3_drawRain() {
  if (s3_rainLevel <= 0.0f)
    return;

  float t = s3_rainLevel / 100.0f;
  if (t > 1.0f) t = 1.0f;

  int drops = (int)(50.0f + t * 950.0f);
  float streakX = 3.0f + t * 7.0f;
  float streakY = 4.0f + t * 12.0f;

  // Color: soft blue at low intensity, near-white at high intensity
  float cr = 0.6f + t * 0.4f;
  float cg = 0.8f + t * 0.2f;
  float cb = 1.0f;
  glColor3f(cr, cg, cb);

  for (int i = 0; i < drops; i++) {
    float x = (float)(rand() % 800), y = (float)(rand() % 800);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x + streakX, y - streakY);
    glEnd();
  }
}
void s3_display() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 800, 0, 800);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClearColor(s3_night ? 0.13f : 0.0f, s3_night ? 0.13f : 0.7f,
               s3_night ? 0.13f : 1.0f, 1);
  glBegin(GL_POLYGON);
  glColor3ub(s3_night ? 30 : 135, s3_night ? 30 : 180, s3_night ? 60 : 255);
  glVertex2i(0, 420);
  glVertex2i(800, 420);
  glVertex2i(800, 800);
  glVertex2i(0, 800);
  glEnd();
  if (!s3_night) {
    glColor3ub(255, 255, 0);
    s3_dc(300, 760, 30);
  } else {
    glColor3ub(220, 220, 200);
    s3_dc(680, 720, 25);
  }
  s3_clouds();
  s3_birds();
  s3_hills();
  s3_bgTrees();
  s3_buildings();
  s3_road();
  s3_carOne();
  s3_carTwo();
  s3_grass();
  if (s3_rainLevel > 0)
    s3_drawRain();
}

// ---------------------------------------------------------------------------------------------------------------
//  SCENE 4 - Expressway / City View
// ---------------------------------------------------------------------------------------------------------------
float s4_cloudX = 0, s4_speed = 2;
float s4_bus1X = -200, s4_bus2X = 900;
int s4_isNight = 0, s4_isRaining = 0;

void s4_sky() {
  glBegin(GL_QUADS);
  glColor3f(s4_isNight ? 0.1f : 0.4f, s4_isNight ? 0.1f : 0.8f,
            s4_isNight ? 0.4f : 1.0f);
  glVertex2f(0, 450);
  glVertex2f(800, 450);
  glVertex2f(800, 800);
  glVertex2f(0, 800);
  glEnd();
}
void s4_sun() {
  glColor3f(s4_isNight ? 0.8f : 1.0f, s4_isNight ? 0.8f : 1.0f,
            s4_isNight ? 0.8f : 0.0f);
  s4_Circle(650, 700, 30);
}
void s4_cloud() {
  glPushMatrix();
  glTranslatef(s4_cloudX, 0, 0);
  glColor3f(0.8f, 0.9f, 0.9f);
  s4_Circle(30, 680, 20);
  s4_Circle(40, 690, 25);
  s4_Circle(60, 670, 15);
  s4_Circle(70, 665, 20);
  s4_Circle(50, 655, 20);
  glPopMatrix();
}
void s4_ground() {
  glBegin(GL_QUADS);
  glColor3f(0, 0.6f, 0);
  glVertex2f(0, 150);
  glVertex2f(800, 150);
  glVertex2f(800, 450);
  glVertex2f(0, 450);
  glEnd();
}
void s4_road() {
  glBegin(GL_QUADS);
  glColor3f(0, 0, 0);
  glVertex2f(0, 0);
  glVertex2f(800, 0);
  glVertex2f(800, 150);
  glVertex2f(0, 150);
  glEnd();
  for (int i = 0; i < 9; i++) {
    glLineWidth(7);
    glBegin(GL_LINES);
    glColor3f(1, 1, 1);
    glVertex2f(i * 95.f, 75);
    glVertex2f(i * 95.f + 30, 75);
    glEnd();
  }
  glLineWidth(1);
}
void s4_tree() {
  for (int i = 0; i < 9; i++) {
    float x = 100.f * i;
    glColor3f(0.29f, 0.14f, 0.02f);
    glBegin(GL_QUADS);
    glVertex2f(x, 160);
    glVertex2f(x + 10, 160);
    glVertex2f(x + 10, 210);
    glVertex2f(x, 210);
    glEnd();
    glColor3f(0, 1, 0);
    float ly[] = {210, 230, 250, 270};
    for (int j = 0; j < 4; j++) {
      glBegin(GL_TRIANGLES);
      glVertex2f(x - 20 + j * 2, ly[j]);
      glVertex2f(x + 30 - j * 2, ly[j]);
      glVertex2f(x + 5, ly[j] + 40);
      glEnd();
    }
  }
}
void s4_buildings() {
  // B1
  glBegin(GL_QUADS);
  glColor3f(0.4f, 0.6f, 0.9f);
  glVertex2f(50, 250);
  glVertex2f(150, 250);
  glVertex2f(150, 550);
  glVertex2f(50, 550);
  glEnd();
  glColor3f(s4_isNight ? 0.7f : 1.0f, s4_isNight ? 0.7f : 1.0f,
            s4_isNight ? 0.0f : 0.6f);
  float w1[][4] = {{60, 280, 90, 320}, {110, 280, 140, 320},
                   {60, 330, 90, 370}, {110, 330, 140, 370},
                   {60, 380, 90, 420}, {110, 380, 140, 420},
                   {60, 430, 90, 470}, {110, 430, 140, 470},
                   {60, 480, 90, 520}, {110, 480, 140, 520}};
  for (int i = 0; i < 10; i++) {
    glBegin(GL_QUADS);
    glVertex2f(w1[i][0], w1[i][1]);
    glVertex2f(w1[i][2], w1[i][1]);
    glVertex2f(w1[i][2], w1[i][3]);
    glVertex2f(w1[i][0], w1[i][3]);
    glEnd();
  }
  // B2
  glBegin(GL_QUADS);
  glColor3f(0.6f, 0.6f, 0.6f);
  glVertex2f(200, 250);
  glVertex2f(320, 250);
  glVertex2f(320, 570);
  glVertex2f(200, 570);
  glEnd();
  glColor3f(s4_isNight ? 0.7f : 0.7f, s4_isNight ? 0.7f : 1.0f,
            s4_isNight ? 0.0f : 1.0f);
  float w2[][4] = {{220, 260, 255, 310}, {265, 260, 300, 310},
                   {220, 320, 255, 370}, {265, 320, 300, 370},
                   {220, 380, 255, 430}, {265, 380, 300, 430},
                   {220, 440, 255, 490}, {265, 440, 300, 490},
                   {220, 500, 255, 550}, {265, 500, 300, 550}};
  for (int i = 0; i < 10; i++) {
    glBegin(GL_QUADS);
    glVertex2f(w2[i][0], w2[i][1]);
    glVertex2f(w2[i][2], w2[i][1]);
    glVertex2f(w2[i][2], w2[i][3]);
    glVertex2f(w2[i][0], w2[i][3]);
    glEnd();
  }
  // B3
  glBegin(GL_QUADS);
  glColor3f(1.0f, 0.7f, 0.4f);
  glVertex2f(370, 250);
  glVertex2f(470, 250);
  glVertex2f(470, 550);
  glVertex2f(370, 550);
  glEnd();
  glColor3f(s4_isNight ? 0.7f : 0.95f, s4_isNight ? 0.7f : 0.95f,
            s4_isNight ? 0.0f : 1.0f);
  float w3[][4] = {
      {385, 260, 415, 295}, {425, 260, 455, 295}, {385, 305, 415, 340},
      {425, 305, 455, 340}, {385, 350, 415, 385}, {425, 350, 455, 385},
      {385, 395, 415, 430}, {425, 395, 455, 430}, {385, 440, 415, 475},
      {425, 440, 455, 475}, {385, 485, 415, 520}, {425, 485, 455, 520}};
  for (int i = 0; i < 12; i++) {
    glBegin(GL_QUADS);
    glVertex2f(w3[i][0], w3[i][1]);
    glVertex2f(w3[i][2], w3[i][1]);
    glVertex2f(w3[i][2], w3[i][3]);
    glVertex2f(w3[i][0], w3[i][3]);
    glEnd();
  }
}
void s4_bus1() {
  glPushMatrix();
  glTranslatef(s4_bus1X + 30, -20, 0);
  glColor3f(1.0f, 0.5f, 0);
  glBegin(GL_QUADS);
  glVertex2f(0, 60);
  glVertex2f(160, 60);
  glVertex2f(160, 110);
  glVertex2f(0, 110);
  glEnd();
  glColor3f(0.7f, 0.9f, 1);
  for (int i = 0; i < 4; i++) {
    glBegin(GL_QUADS);
    glVertex2f(10 + i * 35, 80);
    glVertex2f(40 + i * 35, 80);
    glVertex2f(40 + i * 35, 100);
    glVertex2f(10 + i * 35, 100);
    glEnd();
  }
  glColor3f(0, 0, 0);
  s4_Circle(30, 60, 10);
  s4_Circle(130, 60, 10);
  if (s4_isNight) {
    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 0.5f);
    glVertex2f(160, 82);
    glVertex2f(220, 60);
    glVertex2f(220, 105);
    glEnd();
  }
  glPopMatrix();
}
void s4_bus2() {
  glPushMatrix();
  glTranslatef(s4_bus2X - 30, 50, 0);
  glColor3f(0, 0.6f, 1);
  glBegin(GL_QUADS);
  glVertex2f(0, 60);
  glVertex2f(160, 60);
  glVertex2f(160, 110);
  glVertex2f(0, 110);
  glEnd();
  glColor3f(1, 1, 0.7f);
  for (int i = 0; i < 4; i++) {
    glBegin(GL_QUADS);
    glVertex2f(10 + i * 35, 80);
    glVertex2f(40 + i * 35, 80);
    glVertex2f(40 + i * 35, 100);
    glVertex2f(10 + i * 35, 100);
    glEnd();
  }
  glColor3f(0, 0, 0);
  s4_Circle(30, 60, 10);
  s4_Circle(130, 60, 10);
  if (s4_isNight) {
    glBegin(GL_TRIANGLES);
    glColor3f(1, 1, 0.5f);
    glVertex2f(0, 82);
    glVertex2f(-60, 60);
    glVertex2f(-60, 105);
    glEnd();
  }
  glPopMatrix();
}
void s4_drawRain() {
  glLineWidth(1.5f);
  glBegin(GL_LINES);
  glColor3ub(150, 200, 255);
  for (int i = 0; i < NUM_DROPS; i++) {
    glVertex2f(s4_rainX[i], s4_rainY[i]);
    glVertex2f(s4_rainX[i] + 1, s4_rainY[i] - 8);
  }
  glEnd();
  glLineWidth(1);
}
void s4_display() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, 800, 0, 800);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  s4_sky();
  s4_sun();
  s4_cloud();
  s4_ground();
  s4_buildings();
  s4_tree();
  s4_road();
  s4_bus2();
  s4_bus1();
  if (s4_isRaining)
    s4_drawRain();
}

// ---------------------------------------------------------------------------------------------------------------
//  MASTER GLUT CALLBACKS
// ---------------------------------------------------------------------------------------------------------------
void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  switch (currentScene) {
  case 1:
    s1_display();
    break;
  case 2:
    s2_display();
    break;
  case 3:
    s3_display();
    break;
  case 4:
    s4_display();
    break;
  }
  glutSwapBuffers();
}

void update(int value) {
  // Scene 1
  s1_run3 += 0.2f;
  if (s1_run3 > 1200)
    s1_run3 = -400;
  s1_run2 += s1_carSpeed;
  if (s1_run2 > 1300)
    s1_run2 = -300;
  if (s1_run2 < -350)
    s1_run2 = 1200;
  s1_bird += 1.0f;
  if (s1_bird > 1200)
    s1_bird = -300;
  s1_birdWing += 0.2f;
  s1_boat1X += s1_boatSpeed;
  if (s1_boat1X > 1050)
    s1_boat1X = -120;
  s1_boat2X -= s1_boatSpeed;
  if (s1_boat2X < -120)
    s1_boat2X = 1050;

  // Scene 2
  s2_move += 0.2f * s2_speedMul;
  if (s2_move > 60)
    s2_move = -60;
  s2_move2 -= 0.2f * s2_speedMul;
  if (s2_move2 < -80)
    s2_move2 = 60;
  s2_mcloud2 += 0.2f * s2_speedMul;
  if (s2_mcloud2 > 60)
    s2_mcloud2 = -60;
  s2_mcloud3 -= 0.2f * s2_speedMul;
  if (s2_mcloud3 < -80)
    s2_mcloud3 = 60;
  s2_fishX += 0.25f * s2_fishDir * s2_speedMul;
  if (s2_fishX > 45) {
    s2_fishDir = -1;
    s2_fishX = 45;
  }
  if (s2_fishX < -45) {
    s2_fishDir = 1;
    s2_fishX = -45;
  }
  if (s2_mode == 2) {
    for (int i = 0; i < NUM_DROPS; i++) {
      s2_rainY[i] -= 1.5f;
      if (s2_rainY[i] < -40) {
        s2_rainY[i] = 40;
        s2_rainX[i] = -40 + (float)(rand() % 800) / 10.f;
      }
    }
  }

  // Scene 3
  if (!s3_stopped) {
    s3_carOnePos += 2 * s3_speed;
    s3_carTwoPos -= 2 * s3_speed;
  }
  if (s3_carOnePos > 1000)
    s3_carOnePos = -200;
  if (s3_carTwoPos < -200)
    s3_carTwoPos = 1000;
  s3_cloudPos += 0.5f * s3_speed;
  if (s3_cloudPos > 800)
    s3_cloudPos = -500;
  s3_birdX += 1.5f * s3_speed;
  if (s3_birdX > 800)
    s3_birdX = -200;

  // Scene 4
  s4_cloudX += s4_speed;
  if (s4_cloudX > 900)
    s4_cloudX = -100;
  s4_bus1X += s4_speed;
  if (s4_bus1X > 900)
    s4_bus1X = -200;
  s4_bus2X -= s4_speed;
  if (s4_bus2X < -200)
    s4_bus2X = 900;
  if (s4_isRaining) {
    for (int i = 0; i < NUM_DROPS; i++) {
      s4_rainY[i] -= 1.5f;
      if (s4_rainY[i] < 0) {
        s4_rainY[i] = 800;
        s4_rainX[i] = (float)(rand() % 800);
      }
    }
  }

  glutPostRedisplay();
  glutTimerFunc(30, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
  // Scene switch
  if (key == '1') {
    currentScene = 1;
    glutPostRedisplay();
    return;
  }
  if (key == '2') {
    currentScene = 2;
    glutPostRedisplay();
    return;
  }
  if (key == '3') {
    currentScene = 3;
    glutPostRedisplay();
    return;
  }
  if (key == '4') {
    currentScene = 4;
    glutPostRedisplay();
    return;
  }

  // Per-scene keys
  switch (currentScene) {
  case 1:
    switch (key) {
    case 'r':
    case 'R':
      s1_rainday = 1;
      break;
    case 'e':
    case 'E':
      s1_rainday = 0;
      break;
    case 'n':
    case 'N':
      s1_night = 1;
      break;
    case 'd':
    case 'D':
      s1_night = 0;
      break;
    case 'a':
    case 'A':
      s1_carSpeed = 3.0f;
      break;
    case 'b':
    case 'B':
      s1_carSpeed = -3.0f;
      break;
    case 's':
    case 'S':
      s1_carSpeed = 0.0f;
      break;
    case 27:
      exit(0);
    }
    break;
  case 2:
    if (key == 'd' || key == 'D')
      s2_mode = 0;
    if (key == 'n' || key == 'N')
      s2_mode = 1;
    if (key == 'r' || key == 'R')
      s2_mode = 2;
    if (key == 27)
      exit(0);
    break;
  case 3:
    switch (key) {
    case 'r':
    case 'R':
      s3_rainLevel += 5.0f;
      if (s3_rainLevel > 100.0f) s3_rainLevel = 100.0f;
      s3_isRaining = 1;
      break;
    case 'e':
    case 'E':
      s3_rainLevel -= 5.0f;
      if (s3_rainLevel <= 0.0f) {
        s3_rainLevel = 0.0f;
        s3_isRaining = 0;
      }
      break;
    case 't':
    case 'T':
      s3_rainLevel = 0.0f;
      s3_isRaining = 0;
      break;
    case 'u':
    case 'U':
      s3_laneSwapped = 1;
      s3_car1YOff = -70.0f;
      s3_car2YOff = 70.0f;
      break;
    case 'v':
    case 'V':
      s3_laneSwapped = 0;
      s3_car1YOff = 0.0f;
      s3_car2YOff = 0.0f;
      break;
    case 'm':
      s3_night = 0;
      break;
    case 'n':
      s3_night = 1;
      break;
    case 27:
      s3_speed = 1.0f;
      s3_stopped = 0;
      s3_rainLevel = 0.0f;
      s3_isRaining = 0;
      s3_night = 0;
      s3_laneSwapped = 0;
      s3_car1YOff = 0.0f;
      s3_car2YOff = 0.0f;
      s3_carOnePos = 0.0f;
      s3_carTwoPos = 800.0f;
      s3_cloudPos = 0.0f;
      s3_birdX = 0.0f;
      printf("Normal mode restored\n");
      break;
    }
    break;
  case 4:
    if (key == 'n')
      s4_isNight = 1;
    if (key == 'd')
      s4_isNight = 0;
    if (key == 'r' || key == 'R')
      s4_isRaining = 1;
    if (key == 's' || key == 'S')
      s4_isRaining = 0;
    if (key == 27)
      exit(0);
    break;
  }
  glutPostRedisplay();
}

void specialKey(int key, int x, int y) {
  switch (currentScene) {
  case 2:
    if (key == GLUT_KEY_RIGHT) {
      s2_a += 1;
      if (s2_a > 1000)
        s2_a = -1000;
    }
    if (key == GLUT_KEY_LEFT) {
      s2_a -= 1;
      if (s2_a < -1000)
        s2_a = 1000;
    }
    break;
  case 3:
    if (key == GLUT_KEY_UP)
      s3_stopped = 1;
    if (key == GLUT_KEY_DOWN)
      s3_stopped = 0;
    break;
  }
  glutPostRedisplay();
}

void mouseFunc(int button, int state, int x, int y) {
  switch (currentScene) {
  case 1:
    if (state == GLUT_DOWN) {
      if (button == GLUT_RIGHT_BUTTON) {
        s1_boatSpeed += 1;
        if (s1_boatSpeed > 6)
          s1_boatSpeed = 6;
      }
      if (button == GLUT_LEFT_BUTTON) {
        s1_boatSpeed -= 1;
        if (s1_boatSpeed < 0)
          s1_boatSpeed = 0;
      }
    }
    break;
  case 2:
    if (state == GLUT_DOWN) {
      if (button == GLUT_LEFT_BUTTON)
        s2_speedMul = 0.4f;
      if (button == GLUT_RIGHT_BUTTON)
        s2_speedMul = 2.5f;
    }
    if (state == GLUT_UP)
      s2_speedMul = 1.0f;
    break;
  case 3:
    if (state == GLUT_DOWN) {
      if (button == GLUT_RIGHT_BUTTON)
        s3_speed = 3.0f;
      if (button == GLUT_LEFT_BUTTON)
        s3_speed = 0.5f;
      if (button == GLUT_MIDDLE_BUTTON)
        s3_speed = 1.0f;
    }
    break;
  case 4:
    if (state == GLUT_DOWN) {
      if (button == GLUT_LEFT_BUTTON)
        s4_speed = 2.5f;
      if (button == GLUT_RIGHT_BUTTON)
        s4_speed = 0.0f;
    }
    break;
  }
  glutPostRedisplay();
}

// ---------------------------------------------------------------------------------------------------------------
//  MAIN
// ---------------------------------------------------------------------------------------------------------------
int main(int argc, char **argv) {
  // Init rain buffers
  for (int i = 0; i < NUM_DROPS; i++) {
    s2_rainX[i] = -40.f + (float)(rand() % 800) / 10.f;
    s2_rainY[i] = -40.f + (float)(rand() % 800) / 10.f;
    s4_rainX[i] = (float)(rand() % 800);
    s4_rainY[i] = (float)(rand() % 800);
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(1300, 700);
  glutInitWindowPosition(0, 0);
  glutCreateWindow(
      "Bangladesh Panorama  |  Keys: 1=Village  2=River  3=Highway  4=City");

  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKey);
  glutMouseFunc(mouseFunc);
  glutTimerFunc(30, update, 0);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-40, 40, -40, 40);

  printf("\n====  Bangladesh Panorama  ====\n");
  printf("  Press 1 -> Barisal Village (Guava Garden)\n");
  printf("  Press 2 -> Three River Point View  [DEFAULT]\n");
  printf("  Press 3 -> Highway Road - Cumilla Moynamoti\n");
  printf("  Press 4 -> City Expressway View\n\n");
  printf("--- Scene 1 keys ---\n");
  printf("  R=Rain ON  E=Rain OFF  N=Night  D=Day\n");
  printf("  A=Car fwd  B=Car back  S=Car stop\n");
  printf("  RClick=BoatFast  LClick=BoatSlow\n\n");
  printf("--- Scene 2 keys ---\n");
  printf("  D=Day  N=Night  R=Rain\n");
  printf("  Left/Right arrow = move ship\n");
  printf("  LClick=slow  RClick=fast\n\n");
  printf("--- Scene 3 keys ---\n");
  printf("  M=Day  N=Night\n");
  printf("  R=Hold to INCREASE rain intensity  E=Hold to DECREASE\n");
  printf("  T=Rain OFF instantly\n");
  printf("  U=Shuffle car lanes  V=Restore original lanes\n");
  printf("  Up arrow=Stop  Down arrow=Go\n");
  printf("  LClick=slow  RClick=fast  ESC=reset all\n\n");
  printf("--- Scene 4 keys ---\n");
  printf("  D=Day  N=Night  R=Rain  S=Rain OFF\n");
  printf("  LClick=fast  RClick=stop\n\n");

  glutMainLoop();
  return 0;
}

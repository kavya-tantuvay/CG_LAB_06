#include <GL/glut.h>
#include <cmath>
#include <string>
#include <vector>

// Window dimensions
const int WINDOW_WIDTH = 1400;
const int WINDOW_HEIGHT = 700;

// Circle parameters - Automatically set
int centerX = 0, centerY = 0;
int radius = 5;
bool isDrawing = true;

// Animation parameters
float animationProgress = 0.0f;
bool animating = true;
const float ANIMATION_SPEED = 0.015f;

// Store points for both algorithms
std::vector<std::pair<int, int>> midpointPoints;
std::vector<std::pair<int, int>> bresenhamPoints;

// Pixel scale for better visibility
const int PIXEL_SCALE = 40;

// Function to plot 8 symmetric points
void plot8Points(int xc, int yc, int x, int y, std::vector<std::pair<int, int>>& points) {
    points.push_back({xc + x, yc + y});
    points.push_back({xc - x, yc + y});
    points.push_back({xc + x, yc - y});
    points.push_back({xc - x, yc - y});
    points.push_back({xc + y, yc + x});
    points.push_back({xc - y, yc + x});
    points.push_back({xc + y, yc - x});
    points.push_back({xc - y, yc - x});
}

// Mid-point Circle Drawing Algorithm
void midpointCircle(int xc, int yc, int r) {
    midpointPoints.clear();
    int x = 0, y = r;
    int p = 1 - r;

    plot8Points(xc, yc, x, y, midpointPoints);

    while (x < y) {
        x++;
        if (p < 0) {
            p = p + 2 * x + 1;
        } else {
            y--;
            p = p + 2 * (x - y) + 1;
        }
        plot8Points(xc, yc, x, y, midpointPoints);
    }
}

// Bresenham's Circle Drawing Algorithm
void bresenhamCircle(int xc, int yc, int r) {
    bresenhamPoints.clear();
    int x = 0, y = r;
    int d = 3 - 2 * r;

    plot8Points(xc, yc, x, y, bresenhamPoints);

    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot8Points(xc, yc, x, y, bresenhamPoints);
    }
}

// Draw a scaled pixel
void drawScaledPixel(int x, int y, int offsetX, int centerOffsetX, int centerOffsetY, float size = 8.0f, bool glowing = false) {
    int screenX = offsetX + centerOffsetX + (x * PIXEL_SCALE);
    int screenY = centerOffsetY + (y * PIXEL_SCALE);

    if (glowing) {
        // Draw glow effect
        glPointSize(size + 6.0f);
        glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
        glBegin(GL_POINTS);
        glVertex2i(screenX, screenY);
        glEnd();
    }

    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex2i(screenX, screenY);
    glEnd();
}

// Draw text
void drawText(const char* text, int x, int y, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2i(x, y);
    while (*text) {
        glutBitmapCharacter(font, *text);
        text++;
    }
}

// Draw gradient background
void drawBackground() {
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.05f, 0.15f);
    glVertex2i(0, 0);
    glVertex2i(WINDOW_WIDTH, 0);
    glColor3f(0.15f, 0.1f, 0.25f);
    glVertex2i(WINDOW_WIDTH, WINDOW_HEIGHT);
    glVertex2i(0, WINDOW_HEIGHT);
    glEnd();
}

// Draw enhanced grid
void drawGrid(int offsetX, int centerOffsetX, int centerOffsetY) {
    glLineWidth(1.0f);

    // Draw grid lines
    for (int i = -10; i <= 10; i++) {
        if (i == 0) {
            glColor4f(0.8f, 0.2f, 0.2f, 0.6f);
            glLineWidth(2.5f);
        } else {
            glColor4f(0.3f, 0.3f, 0.4f, 0.25f);
            glLineWidth(1.0f);
        }

        // Vertical lines
        int x = offsetX + centerOffsetX + (i * PIXEL_SCALE);
        glBegin(GL_LINES);
        glVertex2i(x, centerOffsetY - 10 * PIXEL_SCALE);
        glVertex2i(x, centerOffsetY + 10 * PIXEL_SCALE);
        glEnd();

        // Horizontal lines
        if (i == 0) {
            glColor4f(0.2f, 0.8f, 0.2f, 0.6f);
            glLineWidth(2.5f);
        } else {
            glColor4f(0.3f, 0.3f, 0.4f, 0.25f);
            glLineWidth(1.0f);
        }

        int y = centerOffsetY + (i * PIXEL_SCALE);
        glBegin(GL_LINES);
        glVertex2i(offsetX + centerOffsetX - 10 * PIXEL_SCALE, y);
        glVertex2i(offsetX + centerOffsetX + 10 * PIXEL_SCALE, y);
        glEnd();

        // Draw coordinate labels
        if (i != 0 && abs(i) <= 6) {
            char label[5];
            sprintf(label, "%d", i);
            glColor3f(0.5f, 0.5f, 0.6f);
            drawText(label, offsetX + centerOffsetX + (i * PIXEL_SCALE) - 5, centerOffsetY - 12, GLUT_BITMAP_HELVETICA_10);
            drawText(label, offsetX + centerOffsetX + 10, centerOffsetY + (i * PIXEL_SCALE) - 4, GLUT_BITMAP_HELVETICA_10);
        }
    }

    // Draw origin marker
    glColor3f(1.0f, 1.0f, 0.0f);
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    glVertex2i(offsetX + centerOffsetX, centerOffsetY);
    glEnd();

    // Draw origin glow
    glColor4f(1.0f, 1.0f, 0.0f, 0.3f);
    glPointSize(20.0f);
    glBegin(GL_POINTS);
    glVertex2i(offsetX + centerOffsetX, centerOffsetY);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackground();

    int centerOffsetX = 300;
    int centerOffsetY = 350;

    // Main title
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText("Circle Drawing Algorithms Comparison", WINDOW_WIDTH/2 - 200, WINDOW_HEIGHT - 30, GLUT_BITMAP_HELVETICA_18);

    // Parameters display
    glColor3f(1.0f, 1.0f, 0.5f);
    char params[100];
    sprintf(params, "Center: (%d, %d)  |  Radius: %d", centerX, centerY, radius);
    drawText(params, WINDOW_WIDTH/2 - 110, WINDOW_HEIGHT - 60, GLUT_BITMAP_HELVETICA_18);

    // Left panel - Midpoint Algorithm
    glColor3f(0.4f, 0.8f, 1.0f);
    drawText("Mid-Point Circle Algorithm", 180, WINDOW_HEIGHT - 100, GLUT_BITMAP_HELVETICA_18);
    drawGrid(50, centerOffsetX, centerOffsetY);

    // Right panel - Bresenham Algorithm
    glColor3f(1.0f, 0.6f, 0.4f);
    drawText("Bresenham's Circle Algorithm", 880, WINDOW_HEIGHT - 100, GLUT_BITMAP_HELVETICA_18);
    drawGrid(750, centerOffsetX, centerOffsetY);

    if (isDrawing) {
        int pointsToDraw = animating ? (int)(midpointPoints.size() * animationProgress) : midpointPoints.size();

        // Draw Midpoint circle
        for (int i = 0; i < pointsToDraw && i < midpointPoints.size(); i++) {
            float alpha = 0.8f + 0.2f * sin(i * 0.5f);
            glColor4f(0.2f, 0.9f, 1.0f, alpha);
            drawScaledPixel(midpointPoints[i].first, midpointPoints[i].second,
                          50, centerOffsetX, centerOffsetY, 8.0f, i == pointsToDraw - 1);
        }

        // Draw Bresenham circle
        for (int i = 0; i < pointsToDraw && i < bresenhamPoints.size(); i++) {
            float alpha = 0.8f + 0.2f * sin(i * 0.5f);
            glColor4f(1.0f, 0.5f, 0.2f, alpha);
            drawScaledPixel(bresenhamPoints[i].first, bresenhamPoints[i].second,
                          750, centerOffsetX, centerOffsetY, 8.0f, i == pointsToDraw - 1);
        }

        // Display statistics with enhanced styling
        char stats[100];

        // Left panel stats
        glColor3f(0.4f, 0.8f, 1.0f);
        sprintf(stats, "Total Points: %d", (int)midpointPoints.size());
        drawText(stats, 100, 80);
        sprintf(stats, "Points Drawn: %d", pointsToDraw);
        drawText(stats, 100, 55);

        // Right panel stats
        glColor3f(1.0f, 0.6f, 0.4f);
        sprintf(stats, "Total Points: %d", (int)bresenhamPoints.size());
        drawText(stats, 800, 80);
        sprintf(stats, "Points Drawn: %d", pointsToDraw);
        drawText(stats, 800, 55);

        // Progress bar
        if (animating) {
            int barWidth = 300;
            int barX = WINDOW_WIDTH/2 - barWidth/2;
            int barY = 30;

            // Bar background
            glColor4f(0.2f, 0.2f, 0.3f, 0.5f);
            glBegin(GL_QUADS);
            glVertex2i(barX, barY);
            glVertex2i(barX + barWidth, barY);
            glVertex2i(barX + barWidth, barY + 15);
            glVertex2i(barX, barY + 15);
            glEnd();

            // Bar foreground
            glColor4f(0.3f, 0.9f, 1.0f, 0.8f);
            glBegin(GL_QUADS);
            glVertex2i(barX, barY);
            glVertex2i(barX + (int)(barWidth * animationProgress), barY);
            glVertex2i(barX + (int)(barWidth * animationProgress), barY + 15);
            glVertex2i(barX, barY + 15);
            glEnd();

            glColor3f(1.0f, 1.0f, 1.0f);
            sprintf(stats, "%.0f%%", animationProgress * 100);
            drawText(stats, WINDOW_WIDTH/2 - 15, barY + 3, GLUT_BITMAP_HELVETICA_12);
        }
    }

    // Instructions
    glColor3f(0.6f, 0.6f, 0.6f);
    drawText("Press 'R' to Restart Animation  |  Press 'ESC' to Exit", WINDOW_WIDTH/2 - 210, 10);

    glutSwapBuffers();
}

void timer(int value) {
    if (animating) {
        animationProgress += ANIMATION_SPEED;
        if (animationProgress >= 1.0f) {
            animationProgress = 1.0f;
            animating = false;
        }
        glutPostRedisplay();
    }
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC
        exit(0);
    } else if (key == 'r' || key == 'R') {
        animating = true;
        animationProgress = 0.0f;
        glutPostRedisplay();
    }
}

void init() {
    glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Generate circle points automatically
    midpointCircle(centerX, centerY, radius);
    bresenhamCircle(centerX, centerY, radius);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Circle Drawing Algorithms - Center(0,0) Radius 5");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, timer, 0);

    glutMainLoop();
    return 0;
}

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include "header.h"
#include "constants.h"
#include "functions.h"

GLfloat playerPositions[2][6][2]; // Positions of players for two teams
int ballPossession[2] = {0, 6}; // Player index possessing the ball
GLfloat ballX[2], ballY[2]; // Ball position
GLfloat ballRadius = 0.03f; // Ball radius
int movementDelay = 60; // Number of frames to wait before moving the ball
int lastPlayer[2] = {-1, -1};

void drawCircle(GLfloat x, GLfloat y, GLfloat radius, int numSegments, GLfloat r, GLfloat g, GLfloat b) {
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // Center of circle
    for (int i = 0; i <= numSegments; i++) { 
        GLfloat angle = 2.0f * M_PI * (float)i / (float)numSegments; 
        glVertex2f(x + (cos(angle) * radius), y + (sin(angle) * radius));
    }
    glEnd();
}

void drawFieldAndPlayers() {
    // Field background
    glColor3f(0.0f, 0.8f, 0.0f); // Green
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);\
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    // Field lines
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glBegin(GL_LINES);
    glVertex2f(-0.9f, -0.5f);
    glVertex2f(0.9f, -0.5f);
    glVertex2f(-0.9f, 0.5f);
    glVertex2f(0.9f, 0.5f);
    glEnd();

    // Draw players
    for (int team = 0; team < 2; team++) {
        for (int player = 0; player < 6; player++) {
            float x = playerPositions[team][player][0];
            float y = playerPositions[team][player][1];
            drawCircle(x, y, 0.05f, 20, player == 0 ? 1.0f : 0.0f, team == 0 ? 0.0f : 1.0f, player == 0 ? 0.0f : 1.0f);
        }
    }

    // Draw the ball
    drawCircle(ballX[0], ballY[0], ballRadius, 20, 1.0f, 1.0f, 1.0f);

    // Draw the ball
    drawCircle(ballX[1], ballY[1], ballRadius, 20, 0.0f, 0.0f, 0.0f);
}

void updateBallPosition(int i) {
    // Move the ball between players after a delay
    if (movementDelay > 0) {
        movementDelay--;
        return;
    }

    // Move the ball between players
    int currentTeam = ballPossession[i] / 6;
    int currentPlayer = ballPossession[i] % 6;

    int nextPlayer = (currentPlayer + 1) % 6; // Next player in the team

    if(lastPlayer[i] == 5){
        ballX[i] = playerPositions[currentTeam^1][0][0];
        ballY[i] = playerPositions[currentTeam^1][0][1];

        ballPossession[i] = (currentTeam^1) * 6 + nextPlayer;

        lastPlayer[i] = -1;

        movementDelay = 60;
        return;
    }

    ballX[i] = playerPositions[currentTeam][nextPlayer][0];
    ballY[i] = playerPositions[currentTeam][nextPlayer][1];

    // Update ball possession
    ballPossession[i] = currentTeam * 6 + nextPlayer;

    lastPlayer[i] = currentPlayer;

    // Reset movement delay
    movementDelay = 60; // Adjust this value to change the delay (slower value = slower ball movement)
}

void alarm_handler(int signum) {
    printf("Program terminated after %d seconds.\n", GAME_LENGTH);
    exit(EXIT_SUCCESS);
}

int main(void) {

    readFile("args.txt");

    if(signal(SIGALRM, alarm_handler) == -1){
        perror("Setting up a signal");
        exit(-3);
    };

    alarm(GAME_LENGTH);


    GLFWwindow* window;

    // Initialize player positions
    // For simplicity, let's position players in a circle
    for (int team = 0; team < 2; team++) {
        for (int player = 0; player < 6; player++) {
            float angle = 2 * M_PI * player / 6;
            playerPositions[team][player][0] = cos(angle) * 0.2f + (team == 0 ? -0.5f : 0.5f);
            playerPositions[team][player][1] = sin(angle) * 0.2f;
        }
    }

    // Initialize the library
    if (!glfwInit()) {
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Football Field with Teams", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Set initial ball position (first player of first team)
    ballX[0] = playerPositions[0][0][0];
    ballY[0] = playerPositions[0][0][1];
    ballX[1] = playerPositions[1][0][0];
    ballY[1] = playerPositions[1][0][1];

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw the field, players, and ball
        drawFieldAndPlayers();

        // Update ball position
        updateBallPosition(0);

        // Update ball position
        updateBallPosition(1);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Варивант 10: n-угольная пирамида, усеченная наклонной плоскостью (n = 5).

#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

const float PI = std::acos(-1.0);

float rotationAngleX = 0.0f;
float rotationAngleY = 0.0f;
float rotationAngleZ = 0.0f;

bool drawPoints = false;
bool interpolateColors = false;

float randomFloat() {
	return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

void customglSetRandomColor() {
	glColor3f(randomFloat(), randomFloat(), randomFloat());
}

struct Point {
	float x, y, z;

	Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

std::vector<Point> pyramidVertices;

void createPyramid() {
	float topRadius = 0.5f;
	float bottomRadius = 1.0f;
	float height = 2.0f;

	for (int i = 0; i < 5; ++i) {
		float angle = 2 * PI * i / 5;
		float x = bottomRadius * std::cos(-angle);
		float z = bottomRadius * std::sin(-angle);
		pyramidVertices.push_back(Point(x, -3.0f, z));
	}

	for (int i = 0; i < 5; ++i) {
		float angle = 2 * PI * i / 5;
		float xTop = topRadius * std::cos(-angle);
		float zTop = topRadius * std::sin(-angle);

		float yTop = 4.0f;
		if (i == 0) yTop = 4.6f;
		if (i == 2 || i == 3) yTop = 3.0f;

		pyramidVertices.push_back(Point(xTop, yTop, zTop));
	}
}

float bottomColorR = randomFloat();
float bottomColorG = randomFloat();
float bottomColorB = randomFloat();

float p1ColorR[7] = { randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat() };
float p1ColorG[7] = { randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat() };
float p1ColorB[7] = { randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat() };

float p2ColorR[7] = { randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat() };
float p2ColorG[7] = { randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat() };
float p2ColorB[7] = { randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat() };

float topColorR = randomFloat();
float topColorG = randomFloat();
float topColorB = randomFloat();

void drawPyramid() {
	if (!drawPoints)
	{
		glBegin(GL_POLYGON);
		for (size_t i = 5; i > 0; i--) {
			if (i % 2) {
				glColor3f(bottomColorR, bottomColorG, bottomColorB);
			}
			else {
				glColor3f(topColorR, topColorG, topColorB);
			}
			glVertex3f(pyramidVertices[i - 1].x, pyramidVertices[i - 1].y, pyramidVertices[i - 1].z);
		}
		glEnd();

		for (size_t i = 0; i < 5; ++i) {
			glBegin(GL_POLYGON);

			glColor3f(p1ColorR[i], p1ColorG[i], p1ColorB[i]);
			glVertex3f(pyramidVertices[i].x, pyramidVertices[i].y, pyramidVertices[i].z);
			glVertex3f(pyramidVertices[(i + 1) % 5].x, pyramidVertices[(i + 1) % 5].y, pyramidVertices[(i + 1) % 5].z);

			glColor3f(p2ColorR[i], p2ColorG[i], p2ColorB[i]);
			glVertex3f(pyramidVertices[(i + 1) % 5 + 5].x,
				pyramidVertices[(i + 1) % 5 + 5].y,
				pyramidVertices[(i + 1) % 5 + 5].z);
			glVertex3f(pyramidVertices[i + 5].x, pyramidVertices[i + 5].y, pyramidVertices[i + 5].z);
			glEnd();
		}

		glBegin(GL_POLYGON);
		for (size_t i = 5; i < 10; ++i) {
			if (i % 2) {
				glColor3f(topColorR, topColorG, topColorB);
			}
			else {
				glColor3f(bottomColorR, bottomColorG, bottomColorB);
			}
			glVertex3f(pyramidVertices[i].x, pyramidVertices[i].y, pyramidVertices[i].z);
		}
		glEnd();
	}
	else {
		glBegin(GL_POINTS);
		for (size_t i = 0; i < pyramidVertices.size(); ++i) {
			glVertex3f(pyramidVertices[i].x, pyramidVertices[i].y, pyramidVertices[i].z);
		}
		glEnd();
	}
}

void resetProjection() {
	rotationAngleX = 0.0f;
	rotationAngleY = 0.0f;
	rotationAngleZ = 0.0f;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_1:
			glPolygonMode(GL_FRONT, GL_LINE);
			drawPoints = false;
			break;
		case GLFW_KEY_2:
			drawPoints = true;
			break;
		case GLFW_KEY_3:
			glPolygonMode(GL_FRONT, GL_FILL);
			glShadeModel(GL_FLAT);
			drawPoints = false;
			interpolateColors = false;

			bottomColorR = randomFloat();
			bottomColorG = randomFloat();
			bottomColorB = randomFloat();

			for (int i = 0; i < 7; i++) {
				p1ColorR[i] = randomFloat();
				p1ColorG[i] = randomFloat();
				p1ColorB[i] = randomFloat();

				p2ColorR[i] = randomFloat();
				p2ColorG[i] = randomFloat();
				p2ColorB[i] = randomFloat();
			}

			topColorR = randomFloat();
			topColorG = randomFloat();
			topColorB = randomFloat();
			break;
		case GLFW_KEY_4:
			glPolygonMode(GL_FRONT, GL_FILL);
			glShadeModel(GL_SMOOTH);

			drawPoints = false;
			interpolateColors = true;

			bottomColorR = randomFloat();
			bottomColorG = randomFloat();
			bottomColorB = randomFloat();

			for (int i = 0; i < 7; i++) {
				p1ColorR[i] = randomFloat();
				p1ColorG[i] = randomFloat();
				p1ColorB[i] = randomFloat();

				p2ColorR[i] = randomFloat();
				p2ColorG[i] = randomFloat();
				p2ColorB[i] = randomFloat();
			}

			topColorR = randomFloat();
			topColorG = randomFloat();
			topColorB = randomFloat();
			break;
		case GLFW_KEY_5:
			// Вращение относительно оси X
			rotationAngleX += 10.0f;
			break;
		case GLFW_KEY_6:
			// Вращение относительно оси Y
			rotationAngleY += 10.0f;
			break;
		case GLFW_KEY_7:
			// Вращение относительно оси Z
			rotationAngleZ += 10.0f;
			break;
			break;
		case GLFW_KEY_8:
			// Возврат в исходную проекцию
			resetProjection();
			break;
		default:
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	static double lastX = xpos;
	static double lastY = ypos;

	float deltaX = static_cast<float>(xpos - lastX);
	float deltaY = static_cast<float>(ypos - lastY);

	rotationAngleY += deltaX * 0.1f;
	rotationAngleX += deltaY * 0.1f;

	lastX = xpos;
	lastY = ypos;
}

int main(void) {
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1280, 960, "Elliptical Cylinder", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: %d\n" << error << std::endl;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetKeyCallback(window, keyCallback);

	createPyramid();

	glPointSize(10.0f);
	glPolygonMode(GL_BACK, GL_LINE);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL Error: %d\n" << error << std::endl;
	}

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {
		/* Set up the viewport and perspective */
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-7.0, 7.0, -7.0, 7.0, -7.0, 7.0);

		/* Camera transformation */

		/* Apply rotation */
		glRotatef(rotationAngleX, 1.0f, 0.0f, 0.0f);
		glRotatef(rotationAngleY, 0.0f, 1.0f, 0.0f);
		glRotatef(rotationAngleZ, 0.0f, 1.0f, 1.0f);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		drawPyramid();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		error = glGetError();
		if (error != GL_NO_ERROR) {
			std::cerr << "OpenGL Error: %d\n" << error << std::endl;
		}
	}

	glfwTerminate();

	return 0;
}

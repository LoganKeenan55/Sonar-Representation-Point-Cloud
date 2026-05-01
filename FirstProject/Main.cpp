#include"model.h"

#include<fstream>
#include<sstream>
#include <cstdlib>

int gScreenWidth = 1920;
int gScreenHeight = 1080;


struct pointVertex {
	glm::vec3 position;
	float intensity;
};

std::vector<pointVertex> loadPointCloud(const std::string& fileName) {
	std::vector<pointVertex> points;

	std::ifstream file(fileName);

	if (!file.is_open()) {
		std::cout << "Can't open fake point cloud file :( " << fileName << std::endl;
	}

	float x, y, z, intensity;

	while (file >> x >> y >> z >> intensity) {
		//x y z intensity
		pointVertex currentPoint;

		currentPoint.position = glm::vec3(x, y, z);
		currentPoint.intensity = intensity;
		points.push_back(currentPoint);
	}
	return points;
}

float getRandomFloat(float min, float max) {
	return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

std::vector<pointVertex> generatePoints() {
	std::vector<pointVertex> points;
	for (int x = 0; x < 50; x++) {
		for (int y = 0; y < 50; y++) {

			for (int z = 0; z < 150; z++) {

				pointVertex p;

				//x = left/right
				//y = forward/back
				//z = height
				float depth = -z;
				float randomX = getRandomFloat(0.0, 1.0);
				float randomY = getRandomFloat(0.0, 1.0);
				float randomZ = getRandomFloat(0.0, 1.0);
				p.position = glm::vec3(x + randomX, depth + randomZ, y + randomY);

				//background sediment
				float base = 0.1f + 0.1f * sin(x * 0.2f) * cos(y * 0.2f);

				//burried object
				if (x > 20 && x < 30 &&
					y > 20 && y < 30 &&
					z > 10 && z < 50)
				{
					p.intensity = 1.0f; //strong return
				}
				else {
					p.intensity = base; //weak return
				}

				points.push_back(p);
			}
		}
	}
	return points;
}




int main() {
	float gateBottem = 0.0f;
	float gateTop = 50.0f;
	float gateMoveSpeed = 30.0f;



	//load fake jsf
	std::vector<pointVertex> points;
	points = generatePoints();

	Camera camera(gScreenWidth, gScreenHeight, glm::vec3(0.0f, 0.5f, 3.0f));

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(gScreenWidth, gScreenHeight, "Hello, World!", NULL, NULL);


	if (!window) {
		std::cout << "Failed to create a window" << std::endl;
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, gScreenWidth, gScreenHeight);

	
	
	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(pointVertex), points.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pointVertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(pointVertex), (void*)offsetof(pointVertex, intensity));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	Shader pointShader("pointcloud.vert", "pointcloud.frag");
	
	glm::mat4 model = glm::mat4(1.0f);

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;


	while (!glfwWindowShouldClose(window)) {

		deltaTime = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();

		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.handleInput(window, deltaTime);
		camera.updateMatrix(60.0f, 0.1f, 1000.0f);

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
			gateBottem -= gateMoveSpeed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
			gateBottem += gateMoveSpeed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			gateTop -= gateMoveSpeed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			gateTop += gateMoveSpeed * deltaTime;
		}



		pointShader.Activate();
		pointShader.setMat4("model", model);

		pointShader.setFloat("gateBottem", gateBottem);
		pointShader.setFloat("gateTop", gateTop);

		camera.Matrix(pointShader, "camMatrix");

		glBindVertexArray(VAO);

		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, points.size());

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	pointShader.Delete();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
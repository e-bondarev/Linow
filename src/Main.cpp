#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define LINOW_USE_GLM
#include "../Linow.h"

int main() {
  GLFWwindow* window;

  glfwInit();
  glfwDefaultWindowHints();
  window = glfwCreateWindow(800, 600, "Linow", nullptr, nullptr);
  glfwMakeContextCurrent(window);
	glewInit();

  glm::mat4x4 proj = glm::ortho(-800.0f / 2.0f, 800.0f / 2.0f, -600.0f / 2.0f, 600.0f / 2.0f);
  glm::mat4x4 view = glm::mat4x4(1);

  Linow::Init();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    glClear(GL_COLOR_BUFFER_BIT);

    static float value { 0.0f };
    value += 0.1f;

    Linow::Clear();
      Linow::AddLine(Linow::Vec3(value, 0, 0), Linow::Vec3(25, 25, 0), Linow::Color(0, 0, 1, 1));
    Linow::Render(glm::value_ptr(proj), glm::value_ptr(view));

    glfwSwapBuffers(window);
  }

  Linow::Shutdown();

  return 0;
}
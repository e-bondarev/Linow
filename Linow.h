/*
  Usage:

  0.0 Make sure the header file <GL/glew.h> is available for this header file.
  0.1 Include this header file in your project.

  1. Initialization:      

    Linow::Init(glm::value_ptr(proj));

  2. (In your rendering loop)

    Linow::Clear();

      Linow::AddLine(Linow::Vec3(0, 0, 0), Linow::Vec3(25, 25, 0), Linow::Color(0, 0, 1, 1));

    Linow::Render();

  Options:

    #define LINOW_USE_GLM     -     if you want to use GLM's implementation of vectors (recommended)    
    Note: make sure the <glm/glm.hpp> header is available for this header.

    #define LINOW_DEBUGGING   -     if you want Linow to log out when its state changes. (Creating Shaders, VAO, VBO, Shutdown)

    Make sure you define these macros before including this header file, like this:    
      #define LINOW_USE_GLM
      #include "Linow.h"
*/

#pragma once

#ifdef LINOW_USE_GLM
# include <glm/glm.hpp>
#endif

/*
  --------------------- Dependencies --------------------- 
*/
#include <GL/glew.h>

#include <vector>
#include <string>
#include <array>
#include <map>

/*
  --------------------- Memory helper --------------------- 
*/
#include <memory>

namespace Linow {

template <typename T>
using Ptr = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Ptr<T> CreatePtr(Args&& ... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

/*
  --------------------- Aliases and debugging --------------------- 
*/
using Str = std::string;

template <typename T>
using Vec = std::vector<T>;

template <typename T, int n>
using Array = std::array<T, n>;

template <typename T0, typename T1>
using Map = std::map<T0, T1>;

/*
  This doesn't create any assembly code.
*/
#define LINOW_VOID_ASSEMBLY         ((void)0)

#ifdef LINOW_DEBUGGING
# define LINOW_LOG_OUT(x)           std::cout << "[Linow] " << x << '\n'
# define LINOW_NEW_LINE()           std::cout << '\n'
#else
# define LINOW_LOG_OUT(x)           LINOW_VOID_ASSEMBLY
# define LINOW_NEW_LINE()           LINOW_VOID_ASSEMBLY
#endif

/*
  --------------------- Aliases to use with GLM --------------------- 
*/
#ifndef LINOW_USE_GLM
  struct Vec2 {
    float x, y;
    Vec2(float x = 0.0f, float y = 0.0f) { this->x = x; this->y = y; }
  };

  struct Vec3 {
    float x, y, z;
    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) { this->x = x; this->y = y; this->z = z; }
  };

  struct Vec4 {
    float x, y, z, w;
    Vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f) { this->x = x; this->y = y; this->z = z; this->w = w; }
  };

  using Color = Vec4;
#else
  using Vec2 = glm::vec2;
  using Vec3 = glm::vec3;
  using Vec4 = glm::vec4;
  using Color = Vec4;
#endif

/*
  --------------------- Forward declaration of all the classes --------------------- 
*/
struct Line;

class VBO;
class VAO;

class Shader;

/*
  --------------------- Global variables --------------------- 
*/
Ptr<VAO> vao;
Ptr<VBO> vbo;

Vec<Line> lines;
Ptr<Shader> lineShader;

/*
  --------------------- Interface --------------------- 
*/
void Init(const float* projection);
  void Render(const float* projection, const float* view);
  void Clear();
void Shutdown();

template <typename... Args>
void AddLine(Args&&... args);

void AddQuad(Vec2 start, Vec2 end, Color color = Color(1.0f, 0.0f, 0.0f, 1.0f));

/*
  --------------------- Implementation --------------------- 
*/
struct Line {
  Array<Vec3, 2> points;
  Color color { 1, 0, 0, 1 };
  
  inline Line(const Vec3& start, const Vec3& end, const Color& color = Color(1.0f, 0.0f, 0.0f, 1.0f)) {
    points[0] = start;
    points[1] = end;
    this->color = color;
  }
};

const Str vsCode = R"(
  #version 440 core

  layout (location = 0) in vec3 in_Point0;
  layout (location = 1) in vec3 in_Point1;
  layout (location = 2) in vec4 in_Color;

  uniform mat4 u_Proj;
  uniform mat4 u_View;
  out vec4 color;
  void main() {
    color = in_Color;
    gl_Position = u_Proj * u_View * vec4(gl_VertexID == 0 ? in_Point0 : in_Point1, 1.0);
  }
)";

const Str fsCode = R"(
  #version 440 core
  in vec4 color;
  out vec4 out_Color;
  uniform vec4 u_Color;
  void main() {
    out_Color = color;
  }
)";

class Shader {
public:
	template <typename... Args>
	Shader(const Str& vsCode, const Str& fsCode, Args&&... args) {
		handle = glCreateProgram();
		vsHandle = CreateShader(vsCode, GL_VERTEX_SHADER);
		fsHandle = CreateShader(fsCode, GL_FRAGMENT_SHADER);

		Link();

		Vec<Str> uniforms{ args... };

		for (int i = 0; i < uniforms.size(); i++) {
			CreateUniform(uniforms[i]);
		}

    LINOW_LOG_OUT("Shader created.");
	}

	~Shader() {    
    Unbind();
    glDeleteProgram(handle);

    LINOW_LOG_OUT("Shader destroyed.");
  }

	void Bind() const {
    glUseProgram(handle);
  }

	void Unbind() const {
    glUseProgram(0);
  }

	void CreateUniform(const Str& name) {
    int location = glGetUniformLocation(handle, name.c_str());

    uniformLocations.insert(
      std::pair<Str, int>(
        name.c_str(),
        location
        )
    );
  }

	template <typename T>
	inline void SetListVec2(const Str& name, const Vec<T>& vec) {
		glUniform2fv(uniformLocations.at(name), vec.size(), reinterpret_cast<const float*>(&vec[0]));
	}

	void SetMat4x4(const Str& name, float const* const matrix) {
	  glUniformMatrix4fv(uniformLocations.at(name), 1, GL_FALSE, matrix);
  }

private:
  GLuint handle { 0 };
	GLuint vsHandle { 0 };
	GLuint fsHandle { 0 };
	Map<Str, GLuint> uniformLocations;

	void Link() const {
    glLinkProgram(handle);

    if (vsHandle != 0) glDetachShader(handle, vsHandle); glDeleteShader(vsHandle);
    if (fsHandle != 0) glDetachShader(handle, fsHandle); glDeleteShader(fsHandle);

    glValidateProgram(handle);
  }

	GLuint CreateShader(const Str& shaderCode, GLuint shaderType) const {
    const unsigned int shaderID = glCreateShader(shaderType);
    const char* c_str = shaderCode.c_str();

    glShaderSource(shaderID, 1, &c_str, NULL);
    glCompileShader(shaderID);

    int status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
    if (!status) {
      int length;
      glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
      Str log;
      log.resize(length);
      glGetShaderInfoLog(shaderID, length, &length, &log[0]);

      Str errorFunctionName = "--------[ " __FUNCTION__ " ]--------";
      LINOW_LOG_OUT(errorFunctionName);
      LINOW_LOG_OUT("Error occured while compiling a shader: " << log);
      LINOW_NEW_LINE();

      return -1;
    }

    glAttachShader(handle, shaderID);

    return shaderID;
  }

	Shader(const Shader&) = delete;
	Shader operator=(const Shader&) = delete;
};

class VBO {
public:
  VBO(int attribute = 0) {
    GLuint startAttribute = attribute;
    
		glGenBuffers(1, &handle);
		glBindBuffer(GL_ARRAY_BUFFER, handle);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3) * 2 + sizeof(Vec4), reinterpret_cast<void *>(0));
      glVertexAttribDivisor(0, 1);

      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vec3) * 2 + sizeof(Vec4), reinterpret_cast<void *>(3 * sizeof(float)));
      glVertexAttribDivisor(1, 1);

      glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vec3) * 2 + sizeof(Vec4), reinterpret_cast<void *>(3 * sizeof(float) + 3 * sizeof(float)));
      glVertexAttribDivisor(2, 1);

      attributes.push_back(0);
      attributes.push_back(1);
      attributes.push_back(2);

      glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3) * 2 + sizeof(Vec4) * 512, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

    LINOW_LOG_OUT("Dynamic VBO created.");
  }

  ~VBO() {
    Unbind();
    glDeleteBuffers(1, &handle);   

    LINOW_LOG_OUT("Dynamic VBO destroyed."); 
  }

  void Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, handle);
    
    for (const auto attrib : attributes) {
      glEnableVertexAttribArray(attrib);
    }
  }

  void Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  template <typename T, bool bind = true>
  inline void Update(const Vec<T>& vec, int amount, int pos = 0) const {
    if (bind) {
      Bind();
    }
    glBufferSubData(GL_ARRAY_BUFFER, pos, sizeof(Vec3) * 2 + sizeof(Vec4) * amount, vec.data());
  }

private:
  GLuint handle { 0 };
  Vec<GLuint> attributes;

	VBO(const VBO&) = delete;
	VBO operator=(const VBO&) = delete;
};

class VAO {
public:
  VAO() {
		glGenVertexArrays(1, &handle);

    LINOW_LOG_OUT("VAO created.");
  }

  ~VAO() {
    Unbind();
    glDeleteVertexArrays(1, &handle);

    LINOW_LOG_OUT("VAO destroyed.");
  }

  void Bind() {    
		glBindVertexArray(handle);
  }

  void Unbind() {
		glBindVertexArray(0);
  }

private:
  GLuint handle { 0 };

  Ptr<VBO> vbo;

	VAO(const VAO&) = delete;
	VAO operator=(const VAO&) = delete;
};

inline void Init(const float* projection) {
  lineShader = CreatePtr<Shader>(vsCode, fsCode, "u_Proj", "u_View");
  lineShader->Bind();
    lineShader->SetMat4x4("u_Proj", projection);

  vao = CreatePtr<VAO>();
  vao->Bind();
  vbo = CreatePtr<VBO>();

  LINOW_LOG_OUT("Initialization succeeded.");
}

inline void Render(const float* projection, const float* view) {    
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  lineShader->Bind();
    lineShader->SetMat4x4("u_Proj", projection);
    lineShader->SetMat4x4("u_View", view);

  vao->Bind();
  vbo->Update(lines, lines.size());
    glDrawArraysInstanced(GL_TRIANGLES, 0, 3, lines.size());

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

inline void Clear() {
  lines.clear();
}

inline void Shutdown() {
  lineShader.reset();
  vao.reset();
  vbo.reset();

  Clear();

  LINOW_LOG_OUT("Shutdown succeeded.");
}

template <typename... Args>
inline void AddLine(Args&&... args) {
  lines.emplace_back(std::forward<Args>(args)...);
}

inline void AddQuad(Vec3 start, Vec3 end, Color color) {
  Line left(start, Vec3(start.x, end.y, 0), color);
  Line right(Vec3(end.x, start.y, 0), end, color);
  Line top(start, Vec3(end.x, start.y, 0), color);
  Line bottom(Vec3(start.x, end.y, 0), end, color);

  AddLine(left);
  AddLine(right);
  AddLine(top);
  AddLine(bottom);
}

}
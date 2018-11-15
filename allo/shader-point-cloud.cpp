// MAT201B
// Fall 2015
// Author(s): Karl Yerkes
// Modifed for AlloLib and 240C in 2018
//
// Shows how to:
// - Use shaders in AlloSystem
// - Render a "point cloud"
//

#include "al/core.hpp"
using namespace al;

// below, 'vertexSource' is a string (aka char*) containing the GLSL source
// code (not C++ source code) of a "vertex shader" (aka vertex program). this
// is a program that runs on the GPU for each vertex you send. say you make a
// Mesh, add 7 vertices and then draw the Mesh. the program below would run 7
// times, once for each vertex in the Mesh. each time, the position of the
// vertex would be in the variable gl_Vertex.
//
// generally, the goal of a vertex program, is to transform the given vertex
// position into "clip space", the rectangle that goes from (-1, -1) to (1, 1),
// so that the GPU can generate "fragments" (see fragmentShader below).
// however, the vertex program can operate on any vertex information (e.g.,
// position, normal, color, texture coordinates, etc.).
//
// the goal of this particular vertex program is to adjust the size of an
// OpenGL point based on the distance between the point (a vertex) and the
// viewer. this gives the viewer a sense of perspective for a point cloud.
//
const char* vertexSource = R"(
#version 330
uniform mat4 MV;
uniform mat4 P;
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
out vec4 finalColor;

// aesthetic scale factor. point "bigness"
//
float scaleFactor = 50.0;

void main() {
  // standard vertex transform. the minimal vertex shader just does this.
  //
  gl_Position = P * MV * vec4(position, 1.0);

  // determine distance from viewer to point and set point size.
  //
  float distance = length((MV * vec4(position, 1.0)).xyz);
  float pointSize = scaleFactor / distance;

  // don't let points get below 1 pixel.
  //
  gl_PointSize = max(pointSize, 1.0);

  // when points would be smaller than 1 pixel, make them dim
  //
  finalColor = vec4(color, min(pointSize, 1.0));
}
)";

// this is a fragment shader. this program will run on each "fragment". a
// fragment is just a pixel that needs a color. when the GPU draws stuff on the
// screen, it only does as much work as it needs to. if you draw a triangle,
// all the pixels on the edge and inside the triangle are fragments and all the
// other pixels are not.
//
// generally, the goal of a fragment program is to use available information to
// set the color of the fragment. variables passed from vertex program to
// fragment program are *interpolated*.
//
// this is a very minimal fragment program. it just colors the fragment
// acording to the color given by the vertex program.
//
const char* fragmentSource = R"(
#version 330
in vec4 finalColor;
out vec4 fragmentColor;
void main() {
  fragmentColor = finalColor;
}
)";

#define N (1000)
#define CLOUD_WIDTH (5.0)

struct AlloApp : App {
  ShaderProgram shade;
  Mesh pointMesh;

  void onCreate() override {
    pointMesh.primitive(Mesh::POINTS);
    for (int i = 0; i < N; i++) {
      pointMesh.vertex(
          Vec3f(rnd::uniformS(), rnd::uniformS(), rnd::uniformS()) *
          CLOUD_WIDTH);
      pointMesh.color(HSV(rnd::uniform(), 1.0, 1.0));
    }

    // GLSL programs are compiled by the GPU while your C++ program is running.
    // in AlloSystem, this is how we compile a vertex program and fragment
    // program (you always have to have both) into a whole program.
    //
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    shade.compile(vertexSource, fragmentSource);
  }

  void onAnimate(double dt) override {}

  void onDraw(Graphics& g) override {
    g.clear(0.23);
    g.meshColor();
    g.depthTesting(true);
    g.blending(true);
    g.blendModeTrans();

    g.shader(shade);
    g.draw(pointMesh);
  }
};

int main() { AlloApp().start(); }

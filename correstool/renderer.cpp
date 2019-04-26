#include "renderer.h"
#include "triangle.h"
#include <GL/freeglut.h>
#include <vector>
#include <iostream>
using namespace std;
void renderSolid(Mesh m);
void renderWireFrame(Mesh m);
int WINDOWWIDTH, WINDOWHEIGHT;
float ZOOMLEVEL = 1;
float ROTATEX, ROTATEY, TRANSLATEX, TRANSLATEY;
bool hasMarkup;
std::vector<glm::vec3> colors{
    glm::vec3(128, 0, 0),
    glm::vec3(154, 99, 36),
    glm::vec3(128, 128, 0),
    glm::vec3(0, 128, 128),
    glm::vec3(0, 0, 117),
    glm::vec3(230, 25, 75),
    glm::vec3(245, 130, 49),
    glm::vec3(255, 225, 25),
    glm::vec3(188, 246, 12),
    glm::vec3(60, 180, 75),
    glm::vec3(70, 240, 240),
    glm::vec3(67, 99, 216),
    glm::vec3(145, 30, 180),
    glm::vec3(240, 50, 230),
    glm::vec3(250, 190, 190),
    glm::vec3(255, 216, 177),
    glm::vec3(170, 255, 195),
    glm::vec3(230, 190, 255),
};

Renderer::Renderer(Mesh left, Mesh right, int width, int height) : leftMesh(left), rightMesh(right)
{
  rendertype = RENDERTYPE::SOLID;
  WINDOWWIDTH = width;
  WINDOWHEIGHT = height;
  ROTATEX = 0.0;
  ROTATEY = 0.0;
  TRANSLATEX = 0.0;
  TRANSLATEY = 0.0;
  activePanel = PANEL::NONE;
}
RENDERTYPE Renderer::getRenderType()
{
  return rendertype;
}
void Renderer::setRenderType(RENDERTYPE type)
{
  rendertype = type;
  glutPostRedisplay();
}
Mesh Renderer::getLeftModel()
{
  return leftMesh;
}
Mesh Renderer::getRightModel()
{
  return rightMesh;
}
void Renderer::setLeftModel(Mesh m)
{
  leftMesh = m;
}
void Renderer::setRightModel(Mesh m)
{
  rightMesh = m;
}

void Renderer::RenderLeft()
{
  glViewport(0, 0, WINDOWWIDTH / 2, WINDOWHEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  gluPerspective(/* field of view in degree */ 40.0,
                 /* aspect ratio */ (float)WINDOWWIDTH / (float)WINDOWHEIGHT,
                 /* Z near */ 0.01, /* Z far */ 100.0);
  gluLookAt(0.0, 0.5, 3.0, /* eye is at (0,0,5) */
            0.0, 0.0, 0.0, /* center is at (0,0,0) */
            0.0, 1.0, 0.); /* up is in positive Y direction */
  /* Adjust cube position to be asthetic angle. */

  glTranslatef(TRANSLATEX, TRANSLATEY, ZOOMLEVEL);
  glRotatef(ROTATEX, 1.0, 0.0, 0.0);
  glRotatef(ROTATEY, 0.0, 1.0, 0.0);
  switch (rendertype)
  {
  case RENDERTYPE::SOLID:
    renderSolid(leftMesh);
    break;
  case RENDERTYPE::WIREFRAME:
    renderWireFrame(leftMesh);
    break;
  default:
    break;
  }
}
void Renderer::RenderRight()
{
  glViewport(WINDOWWIDTH / 2, 0, WINDOWWIDTH / 2, WINDOWHEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(/* field of view in degree */ 40.0,
                 /* aspect ratio */ (float)WINDOWWIDTH / (float)WINDOWHEIGHT,
                 /* Z near */ 0.01, /* Z far */ 100.0);
  gluLookAt(0.0, 0.5, 3.0, /* eye is at (0,0,5) */
            0.0, 0.0, 0.0, /* center is at (0,0,0) */
            0.0, 1.0, 0.); /* up is in positive Y direction */
  /* Adjust cube position to be asthetic angle. */
  glTranslatef(TRANSLATEX, TRANSLATEY, ZOOMLEVEL);
  glRotatef(ROTATEX, 1.0, 0.0, 0.0);
  glRotatef(ROTATEY, 0.0, 1.0, 0.0);
  switch (rendertype)
  {
  case RENDERTYPE::SOLID:
    renderSolid(rightMesh);
    break;
  case RENDERTYPE::WIREFRAME:
    renderWireFrame(rightMesh);
    break;
  default:
    break;
  }
  glTranslatef(TRANSLATEX, TRANSLATEY, ZOOMLEVEL);
  glRotatef(ROTATEX, 1.0, 0.0, 0.0);
  glRotatef(ROTATEY, 0.0, 1.0, 0.0);
}
void Renderer::renderSelections()
{
  glDisable(GL_LIGHTING);
  //Left Selections
  std::vector<Triangle> triangles = leftMesh.getTriangles();
  glViewport(0, 0, WINDOWWIDTH / 2, WINDOWHEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  gluPerspective(/* field of view in degree */ 40.0,
                 /* aspect ratio */ (float)WINDOWWIDTH / (float)WINDOWHEIGHT,
                 /* Z near */ 0.01, /* Z far */ 100.0);
  gluLookAt(0.0, 0.5, 3.0, /* eye is at (0,0,5) */
            0.0, 0.0, 0.0, /* center is at (0,0,0) */
            0.0, 1.0, 0.); /* up is in positive Y direction */
  /* Adjust cube position to be asthetic angle. */

  glTranslatef(TRANSLATEX, TRANSLATEY, ZOOMLEVEL);
  glRotatef(ROTATEX, 1.0, 0.0, 0.0);
  glRotatef(ROTATEY, 0.0, 1.0, 0.0);
  std::vector<TriangleVertexIndex> leftSelection = correslist.getLeftPoints();
  for (int i = 0; i < leftSelection.size(); i++)
  {
    Triangle t = triangles.at(leftSelection.at(i).triangle);
    int vertexIndex = leftSelection.at(i).vertex;
    std::vector<glm::vec3> points = t.getPoints();
    glm::vec3 color = colors.at(i % colors.size());
    glBegin(GL_TRIANGLES);
    if (vertexIndex == 0)
    {
      glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
    }
    else
    {
      glColor3f(0.8, 0.8, 0.8);
    }

    glVertex3f(points.at(0).x, points.at(0).y, points.at(0).z);
    if (vertexIndex == 1)
    {
      glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
    }
    else
    {
      glColor3f(0.8, 0.8, 0.8);
    }
    glVertex3f(points.at(1).x, points.at(1).y, points.at(1).z);
    if (vertexIndex == 2)
    {
      glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
    }
    else
    {
      glColor3f(0.8, 0.8, 0.8);
    }
    glVertex3f(points.at(2).x, points.at(2).y, points.at(2).z);
    glEnd();
  }
  triangles = rightMesh.getTriangles();
  glViewport(WINDOWWIDTH / 2, 0, WINDOWWIDTH / 2, WINDOWHEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glGetIntegerv(GL_VIEWPORT, viewport);

  gluPerspective(/* field of view in degree */ 40.0,
                 /* aspect ratio */ (float)WINDOWWIDTH / (float)WINDOWHEIGHT,
                 /* Z near */ 0.01, /* Z far */ 100.0);
  gluLookAt(0.0, 0.5, 3.0, /* eye is at (0,0,5) */
            0.0, 0.0, 0.0, /* center is at (0,0,0) */
            0.0, 1.0, 0.); /* up is in positive Y direction */
  /* Adjust cube position to be asthetic angle. */

  glTranslatef(TRANSLATEX, TRANSLATEY, ZOOMLEVEL);
  glRotatef(ROTATEX, 1.0, 0.0, 0.0);
  glRotatef(ROTATEY, 0.0, 1.0, 0.0);
  std::vector<TriangleVertexIndex> rightSelection = correslist.getRightPoints();
  for (int i = 0; i < rightSelection.size(); i++)
  {
    Triangle t = triangles.at(rightSelection.at(i).triangle);
    int vertexIndex = rightSelection.at(i).vertex;
    std::vector<glm::vec3> points = t.getPoints();
    glm::vec3 color = colors.at(i % colors.size());
    glBegin(GL_TRIANGLES);
    if (vertexIndex == 0)
    {
      glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
    }
    else
    {
      glColor3f(0.8, 0.8, 0.8);
    }

    glVertex3f(points.at(0).x, points.at(0).y, points.at(0).z);
    if (vertexIndex == 1)
    {
      glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
    }
    else
    {
      glColor3f(0.8, 0.8, 0.8);
    }
    glVertex3f(points.at(1).x, points.at(1).y, points.at(1).z);
    if (vertexIndex == 2)
    {
      glColor3f(color.r / 255.0, color.g / 255.0, color.b / 255.0);
    }
    else
    {
      glColor3f(0.8, 0.8, 0.8);
    }
    glVertex3f(points.at(2).x, points.at(2).y, points.at(2).z);
    glEnd();
  }
  glEnable(GL_LIGHTING);
}
void Renderer::initLights()
{
  //here you set the lights and parameters, example with one light
  float LightAmbient[] = {0.5f, 0.5f, 0.5f, 1.0f};
  float LightEmission[] = {1.0f, 1.0f, 0.8f, 1.0f};
  float LightDiffuse[] = {1.0f, 1.0f, 0.8f, 1.0f};
  float LightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
  float LightDirection[] = {-0.5f, -0.5f, -0.5f};
  float LightPosition2[] = {0.5f, 0.5f, 0.5f};
  glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, LightSpecular);
  glLightfv(GL_LIGHT0, GL_POSITION, LightDirection);

  glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition2);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);
}
void Renderer::initMaterial()
{
  float MatAmbient[] = {0.2f, 0.2f, 0.2f, 1.0f};
  float MatDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
  float MatSpecular[] = {0.1f, 0.1f, 0.0f, 0.1f};
  float MatShininess = 60;
  float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MatAmbient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MatDiffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, MatSpecular);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, MatShininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black);
}
void Renderer::displayFunc()
{
  initLights();
  initMaterial();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  renderSelections();
  RenderLeft();
  RenderRight();

  glutSwapBuffers();
}

void Renderer::onReshape(int width, int height)
{
  WINDOWWIDTH = width;
  WINDOWHEIGHT = height;
}
void Renderer::zoomin()
{
  ZOOMLEVEL += 0.01;
  glutPostRedisplay();
}
void Renderer::zoomout()
{
  ZOOMLEVEL -= 0.01;
  glutPostRedisplay();
}
void Renderer::rotate(int x, int y)
{
  ROTATEX += 0.08 * y;
  ROTATEY += 0.08 * x;
  glutPostRedisplay();
}
void Renderer::translate(int x, int y)
{
  TRANSLATEX += 0.001 * x;
  TRANSLATEY += -0.001 * y;
  glutPostRedisplay();
}
void renderSolid(Mesh m)
{
  glBegin(GL_TRIANGLES);
  vector<Triangle> triangles1 = m.getTriangles();
  for (Triangle triangle : triangles1)
  {
    vector<glm::vec3> points = triangle.getPoints();
    vector<glm::vec3> normals = triangle.getNormals();

    for (size_t i = 0; i < points.size(); i++)
    {
      glNormal3f(normals.at(i).x, normals.at(i).y, normals.at(i).z);
      glVertex3f(points.at(i).x, points.at(i).y, points.at(i).z);
    }
  }
  glEnd();
}
void renderWireFrame(Mesh m)
{
  vector<Triangle> triangles2 = m.getTriangles();
  for (Triangle triangle : triangles2)
  {
    vector<glm::vec3> points = triangle.getPoints();
    vector<glm::vec3> normals = triangle.getNormals();
    glBegin(GL_LINE_LOOP);
    for (size_t i = 0; i < points.size(); i++)
    {

      glNormal3f(normals.at(i).x, normals.at(i).y, normals.at(i).z);
      glVertex3f(points.at(i).x, points.at(i).y, points.at(i).z);
    }
    glEnd();
  }
}
void Renderer::setActive(PANEL panel)
{
  activePanel = panel;
  glViewport(0, 0, WINDOWWIDTH / 2, WINDOWHEIGHT);
}
PANEL Renderer::getPanel(int x, int y)
{
  if (x <= WINDOWWIDTH / 2)
  {
    return PANEL::LEFT;
  }
  else
  {
    return PANEL::RIGHT;
  }
}

glm::vec2 Renderer::screenToViewportCoordinates(int x, int y)
{
  int yv = WINDOWHEIGHT - y;
  int xv = x % (WINDOWWIDTH / 2);
  return glm::vec2(xv, yv);
}
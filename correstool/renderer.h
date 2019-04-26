#ifndef __DT_RENDER__
#define __DT_RENDER__
#include <glm/glm.hpp>
#include "mesh.h"
#include "correspondence.h"
enum class RENDERTYPE
{
  WIREFRAME,
  SOLID,
  NONE,
};
enum class PANEL
{
  RIGHT,
  LEFT,
  NONE
};
class Renderer
{
private:
  glm::vec3 cameraPosition;
  RENDERTYPE rendertype;
  Mesh leftMesh;
  Mesh rightMesh;
  PANEL activePanel;

  void initLights(void);
  void initMaterial(void);
  void RenderLeft();
  void RenderRight();
  void renderSelections();

public:
  Renderer(Mesh left, Mesh right, int width, int height);
  CorrespondenceList correslist;
  void setRenderType(RENDERTYPE type);
  void setLeftModel(Mesh m);
  void setRightModel(Mesh m);
  Mesh getLeftModel();
  Mesh getRightModel();
  RENDERTYPE getRenderType();
  void displayFunc(void);
  void renderSelection(void);
  void onReshape(int width, int height);
  void zoomin();
  void zoomout();
  void rotate(int x, int y);
  void translate(int x, int y);
  void setActive(PANEL panel);
  PANEL getPanel(int x, int y);
  glm::vec2 screenToViewportCoordinates(int x, int y);
  void renderSelectedTriangle(PANEL panel, int index);
  static int findTriangle(Mesh m, int x, int y);
};
#endif
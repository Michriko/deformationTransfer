#include "controls.h"
#include "correspondence.h"
#include <iostream>
#include <GL/freeglut.h>
Controls::Controls(Renderer *r) : r(r)
{
  mouseLPressed = false;
  mouseRPressed = false;
}
void Controls::onMousePressed(int key, int state, int x, int y)
{
  switch (key)
  {
  case 0:
    if (state == 0)
    {
      oldMouseX = x;
      oldMousey = y;
      mouseLPressed = true;
    }
    else
    {
      mouseLPressed = false;
    }
    break;
  case 2:
    if (state == 0)
    {
      oldMouseX = x;
      oldMousey = y;
      mouseRPressed = true;
    }
    else
    {
      mouseRPressed = false;
    }
    break;
  case 3:
    r->zoomin();
    break;
  case 4:
    r->zoomout();
    break;
  default:
    break;
  }
}
void Controls::onMouseMoved(int x, int y)
{

  int diffX = x - oldMouseX;
  int diffY = y - oldMousey;
  oldMouseX = x;
  oldMousey = y;
  if (mouseLPressed)
  {

    r->rotate(diffX, diffY);
  }
  else if (mouseRPressed)
  {
    r->translate(diffX, diffY);
  }
  glutPostRedisplay();
}
void Controls::onKeyPressed(unsigned char key, int x, int y)
{
  if (key == 'w')
  {
    if (r->getRenderType() == RENDERTYPE::SOLID)
    {
      r->setRenderType(RENDERTYPE::WIREFRAME);
    }
    else
    {
      r->setRenderType(RENDERTYPE::SOLID);
    }
  }
  if (key == 'l')
  {
    if (r->getPanel(x, y) != PANEL::LEFT)
    {
      return;
    }
    glm::vec2 viewportCoordinates = r->screenToViewportCoordinates(x, y);
    Mesh m = r->getLeftModel();
    r->setActive(PANEL::LEFT);
    int index = Renderer::findTriangle(m, viewportCoordinates.x, viewportCoordinates.y);
    if (index != -1)
    {
      r->correslist.upsertTriangleLeft(index);
      glutPostRedisplay();
    }
  }
  if (key == 'r')
  {
    if (r->getPanel(x, y) != PANEL::RIGHT)
    {
      return;
    }
    glm::vec2 viewportCoordinates = r->screenToViewportCoordinates(x, y);
    Mesh m = r->getRightModel();
    r->setActive(PANEL::RIGHT);
    int index = Renderer::findTriangle(m, viewportCoordinates.x, viewportCoordinates.y);
    if (index != -1)
    {
      r->correslist.upsertTriangleRight(index);
      glutPostRedisplay();
    }
  }
  if (key == 's')
  {
    r->correslist.saveCorresFile("correspondences.csv", r->getLeftModel(), r->getRightModel());
  }
}
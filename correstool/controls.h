#ifndef __DT_CONTROLS__
#define __DT_CONTROLS__
#include "renderer.h"
enum CTRL_STATE
{
  MOVE,
  SELECT
};

class Controls
{
private:
  Renderer *r;
  int oldMouseX, oldMousey;
  bool mouseLPressed, mouseRPressed;

public:
  Controls(Renderer *r);
  void onMousePressed(int key, int state, int x, int y);
  void onMouseMoved(int x, int y);
  void onMouseWheel(int button, int dir, int x, int y);
  void onKeyPressed(unsigned char key, int x, int y);
  void onReshape(int width, int height);
};
#endif
using namespace sf;

struct player
{ 
  int x, y, dir;
  Color color;

  void setup(Color c, int _x, int _y, int _dir)
  {
    x = _x;
    y = _y;
    color = c;
    dir =  _dir % 4;
  }

  void tick()
  {
    if (dir == 0) y += 1;
    if (dir == 1) x -= 1;
    if (dir == 2) x += 1;
    if (dir == 3) y -= 1;

    if (x >= W)  x = 0;  if (x < 0) x = W-1;
    if (y >= H)  y = 0;  if (y < 0) y = H-1;
  }

  Vector3f getColor(){ return Vector3f(color.r, color.g, color.b); }
};

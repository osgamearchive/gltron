#ifndef Vector3_H
#define Vector3_H

class Vector3 {
 public:
  Vector3() { };
  Vector3(float a, float b, float c) {
    x = a; y = b; z = c;
  };
  float x, y, z;
};

#endif

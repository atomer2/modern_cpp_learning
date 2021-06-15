#include <iostream>
#include <cmath>

struct Point {
private:
  Point(float x, float y) : x(x), y(y) {}
  struct PointFactory {
  private:
    PointFactory() {}

  public:
    static Point NewCartesian(float x, float y) { return {x, y}; }
    static Point NewPolar(float r, float theta) {
      return {r * cos(theta), r * sin(theta)};
    }
  };

public:
  float x, y;
  static PointFactory Factory;
  friend std::ostream &operator<<(std::ostream &os, const Point &p) {
    os << "X is : " << p.x << ", Y is : " << p.y << std::endl;
    return os;
  }
};

int main() {
  auto pp = Point::Factory.NewCartesian(2, 3);
  std::cout << pp;
  return 0;
}

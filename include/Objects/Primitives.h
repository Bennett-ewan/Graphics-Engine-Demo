#pragma once

//#include <Graphics/Mesh.h>

namespace Graphics { struct Vertex; }
namespace Shapes
{
  class Primitive
  {
  public:
    Primitive() {}
    virtual ~Primitive() {}

    void Set(const std::vector<Graphics::Vertex>& vertices, const std::vector<GLuint> indices);

    const std::vector<Graphics::Vertex>& Get_Vertices() { return vertices; }
    const std::vector<GLuint>& Get_Indices() { return indices; }

    unsigned Get_Num_Vertices() { return static_cast<unsigned>(vertices.size()); }
    unsigned Get_Num_Indices() { return static_cast<unsigned>(indices.size()); }

  private:
    std::vector<Graphics::Vertex> vertices;
    std::vector<GLuint> indices;

  protected:
    void compute_tangents();
  };


  class Triangle : public Primitive
  {
  public:
    Triangle();
  };

  class Quad : public Primitive
  {
  public:
    Quad();
  };

  class Pyramid : public Primitive
  {
  public:
    Pyramid();
  };

  class Cube : public Primitive
  {
  public:
    Cube();
  };


  class Sphere : public Primitive
  {
  public:
    Sphere();
  };
}


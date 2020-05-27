#include <stdafx.h>

#include <Graphics/Mesh.h>
#include <Objects/Primitives.h>

namespace Shapes
{
  void Primitive::Set(const std::vector<Graphics::Vertex>& vertices_, const std::vector<GLuint> indices_)
  {
    vertices = vertices_;
    indices = indices_;
  }



  Triangle::Triangle() : Primitive()
  {
    std::vector<Graphics::Vertex> vert
    {
      { glm::vec3(0.f, 0.5f, 0.f),				glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.5f, 1.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, 0.f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, 0.f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) }
    };

    std::vector<GLuint> ind
    {
      0, 1, 2
    };

    Set(vert, ind);
    compute_tangents();
  }


  Quad::Quad() : Primitive()
  {
    std::vector<Graphics::Vertex> vert
    {
      //Position							              	//Color					      		//Texcoords				     	//Normals
      { glm::vec3(-0.5f, 0.5f, 0.f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, 0.f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, 0.f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, 0.5f, 0.f),				glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) }
    };

    std::vector<GLuint> ind
    {
      0, 1, 2,
      0, 2, 3
    };

    Set(vert, ind);
    compute_tangents();
  }

  Pyramid::Pyramid() : Primitive()
  {
    std::vector<Graphics::Vertex> vert
    {
      //Position							              	//Color				      			//Texcoords					    //Normals
      //Triangle front
      { glm::vec3(0.f, 0.5f, 0.f),				  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.5f, 1.f),		glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, 0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 1.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      
      //Triangle left
      { glm::vec3(0.f, 0.5f, 0.f),				  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.5f, 1.f),		glm::vec3(-1.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(-1.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, 0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(-1.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
                                                                                                                        
      //Triangle back                                                                                                    
      { glm::vec3(0.f, 0.5f, 0.f),				  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.5f, 1.f),		glm::vec3(0.f, 1.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 1.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 1.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      
      //Triangles right
      { glm::vec3(0.f, 0.5f, 0.f),				  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.5f, 1.f),		glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(1.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },

      { glm::vec3(-0.5f, -0.5f, 0.5f),      glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f),		glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, -0.5f),     glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, -0.5f),      glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
                                                                                                                         
      { glm::vec3(-0.5f, -0.5f, 0.5f),      glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f),		glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, 0.5f),	      glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f),		glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },


      ////Top
      //{ glm::vec3(0.f, 0.5f, 0.f),				  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.5f, 1.f),		glm::vec3(0.f, 0.f, 1.f) },
      ////Front left
      //{ glm::vec3(-0.5f, -0.5f, 0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, 1.f) },
      ////Front Right
      //{ glm::vec3(0.5f, -0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f) },
      ////Back left      
      //{ glm::vec3(-0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(-1.f, 0.f, 0.f) },
      ////Back right
      //{ glm::vec3(0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, -1.f) },

    };


    std::vector<GLuint> ind;
    //{
    //  0, 1, 2,
    //  0, 2, 1,
    //  0, 4, 3,
    //  0, 2, 4,
    //
    //  3, 1, 2,
    //  3, 2, 4
    //};

    Set(vert, ind);
    compute_tangents();
  }

  Cube::Cube() : Primitive()
  {
    std::vector<Graphics::Vertex> vert
    {
      //Position								                       //Color							//Texcoords					//Normals
      //Front
      { glm::vec3(-0.5f, 0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, 0.5f),		 	glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, 0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f),		glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      
      //Back
      { glm::vec3(0.5f, 0.5f, -0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f),		glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, 0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f),		glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },

      //Right
      { glm::vec3(0.5f, 0.5f, 0.5f),        glm::vec3(1.f, 1.f, 1.f),   glm::vec2(0.f, 1.f),    glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, 0.5f),       glm::vec3(1.f, 1.f, 1.f),   glm::vec2(0.f, 0.f),    glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 0.f),    glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, 0.5f, -0.5f),       glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 1.f),    glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },

      //Left
      { glm::vec3(-0.5f, 0.5f, -0.5f),      glm::vec3(1.f, 1.f, 1.f),   glm::vec2(0.f, 1.f),    glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, -0.5f),     glm::vec3(1.f, 1.f, 1.f),   glm::vec2(0.f, 0.f),    glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, 0.5f),			glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 0.f),    glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, 0.5f, 0.5f),       glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 1.f),    glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },

      //Top
      { glm::vec3(-0.5f, 0.5f, -0.5f),      glm::vec3(1.f, 1.f, 1.f),   glm::vec2(0.f, 1.f),    glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, 0.5f, 0.5f),       glm::vec3(1.f, 1.f, 1.f),   glm::vec2(0.f, 0.f),    glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, 0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 0.f),    glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, 0.5f, -0.5f),       glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 1.f),    glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },

      //Bottom
      { glm::vec3(-0.5f, -0.5f, 0.5f),      glm::vec3(1.f, 1.f, 1.f),   glm::vec2(0.f, 1.f),    glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(-0.5f, -0.5f, -0.5f),       glm::vec3(1.f, 1.f, 1.f),   glm::vec2(0.f, 0.f),  glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, -0.5f),			  glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 0.f),  glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) },
      { glm::vec3(0.5f, -0.5f, 0.5f),       glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 1.f),    glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f) }
      
      //Front
      //{ glm::vec3(-0.5f, -0.5f, 0.5f),      glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 1.f),		glm::vec3(0.f, 0.f, 1.f) },
      //{ glm::vec3(0.5f, -0.5f, 0.5f),	      glm::vec3(1.f, 1.f, 1.f),		glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, 1.f) },
      //{ glm::vec3(0.5f, 0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, 1.f) },
      //{ glm::vec3(-0.5f, 0.5f, 0.5f),			  glm::vec3(1.f, 1.f, 1.f),		glm::vec2(1.f, 1.f),		glm::vec3(0.f, 0.f, 1.f) },
      //
      //{ glm::vec3(-0.5f, -0.5f, -0.5f),	    glm::vec3(1.f, 1.f, 1.f),	  glm::vec2(0.f, 1.f),		glm::vec3(0.f, 0.f, -1.f) },
      //{ glm::vec3(0.5f, -0.5f, -0.5f),			glm::vec3(1.f, 1.f, 1.f),	  glm::vec2(0.f, 0.f),		glm::vec3(0.f, 0.f, -1.f) },
      //{ glm::vec3(0.5f, 0.5f, -0.5f),	    	glm::vec3(1.f, 1.f, 1.f),	  glm::vec2(1.f, 0.f),		glm::vec3(0.f, 0.f, -1.f) },
      //{ glm::vec3(-0.5f, 0.5f, -0.5f),    	glm::vec3(1.f, 1.f, 1.f),	  glm::vec2(1.f, 1.f),		glm::vec3(0.f, 0.f, -1.f) }
    };

    std::vector<GLuint> ind
    {
      0, 1, 2,
      0, 2, 3,
    
      4, 5, 6,
      4, 6, 7,
    
      8, 9, 10,
      8, 10, 11,
    
      12, 13, 14,
      12, 14, 15,
    
      16, 17, 18,
      16, 18, 19,
    
      20, 21, 22,
      20, 22, 23
    };

    Set(vert, ind);
    compute_tangents();
  }


  Sphere::Sphere()
  {
    float radius = 0.5f;
    int sectorCount = 40;
    int stackCount = 40;
    
    std::vector<Graphics::Vertex> vert;
    std::vector<unsigned> ind;
    

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * glm::pi<float>() / sectorCount;
    float stackStep = glm::pi<float>() / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
      stackAngle = glm::pi<float>() / 2 - i * stackStep;        // starting from pi/2 to -pi/2
      xy = radius * cosf(stackAngle);             // r * cos(u)
      z = radius * sinf(stackAngle);              // r * sin(u)

      // add (sectorCount+1) vertices per stack
      // the first and last vertices have same position and normal, but different tex coords
      for (int j = 0; j <= sectorCount; ++j)
      {
        sectorAngle = j * sectorStep;           // starting from 0 to 2pi

        // vertex position (x, y, z)
        x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
        y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
        Graphics::Vertex vertex;
        vertex.pos = glm::vec3(x, y, z);

        // normalized vertex normal (nx, ny, nz)
        nx = x * lengthInv;
        ny = y * lengthInv;
        nz = z * lengthInv;
        vertex.normal = glm::vec3(nx, ny, nz);

        // vertex tex coord (s, t) range between [0, 1]
        s = (float)j / sectorCount;
        t = (float)i / stackCount;
        vertex.texcoord = glm::vec2(s, t);
        vertex.color = glm::vec3(1.f, 1.f, 1.f);

        vert.push_back(vertex);
      }
    }
    int k1, k2;

    for (int i = 0; i < stackCount; ++i)
    {
      k1 = i * (sectorCount + 1);     // beginning of current stack
      k2 = k1 + sectorCount + 1;      // beginning of next stack

      for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
      {
        // 2 triangles per sector excluding first and last stacks
        // k1 => k2 => k1+1
        if (i != 0)
        {
          ind.push_back(k1);
          ind.push_back(k2);
          ind.push_back(k1 + 1);
        }

        // k1+1 => k2 => k2+1
        if (i != (stackCount - 1))
        {
          ind.push_back(k1 + 1);
          ind.push_back(k2);
          ind.push_back(k2 + 1);
        }
      }
    }

    Set(vert, ind);
    compute_tangents();
  }

  void Primitive::compute_tangents()
  {
    if (indices.size() == 0)
    {
      for (unsigned i = 0; i < vertices.size(); i += 3)
      {
        Graphics::Vertex& v0 = vertices[i];
        Graphics::Vertex& v1 = vertices[i + 1];
        Graphics::Vertex& v2 = vertices[i + 2];

        glm::vec3 deltaPos1 = v1.pos - v0.pos;
        glm::vec3 deltaPos2 = v2.pos - v0.pos;

        glm::vec2 deltaUV1 = v1.texcoord - v0.texcoord;
        glm::vec2 deltaUV2 = v2.texcoord - v0.texcoord;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

        vertices[i].tangent = tangent;
        vertices[i + 1].tangent = tangent;
        vertices[i + 2].tangent = tangent;

        vertices[i].bitangent = bitangent;
        vertices[i + 1].bitangent = bitangent;
        vertices[i + 2].bitangent = bitangent;
      }
    }
    else
    {
      for (unsigned i = 0; i < indices.size(); i += 3)
      {
        Graphics::Vertex& v0 = vertices[indices[i]];
        Graphics::Vertex& v1 = vertices[indices[i + 1]];
        Graphics::Vertex& v2 = vertices[indices[i + 2]];

        glm::vec3 deltaPos1 = v1.pos - v0.pos;
        glm::vec3 deltaPos2 = v2.pos - v0.pos;

        glm::vec2 deltaUV1 = v1.texcoord - v0.texcoord;
        glm::vec2 deltaUV2 = v2.texcoord - v0.texcoord;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
        glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;
        vertices[indices[i]].tangent = tangent;
        vertices[indices[i + 1]].tangent = tangent;
        vertices[indices[i + 2]].tangent = tangent;

        vertices[indices[i]].bitangent = bitangent;
        vertices[indices[i + 1]].bitangent = bitangent;
        vertices[indices[i + 2]].bitangent = bitangent;


      }
    }

  }
}
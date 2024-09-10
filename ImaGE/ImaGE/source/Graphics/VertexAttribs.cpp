#include <pch.h>
#include <fstream>
#include <sstream>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "VertexAttribs.h"
#include <assimp/scene.h>

namespace
{
  void AddVertexAttribs(std::vector<glm::vec3>& dest, aiVector3D const* arr, unsigned count);
}

namespace Graphics
{
  void VertexAttribs::AddMeshes(aiMesh const* mesh)
  {
    AddVertexAttribs(points, mesh->mVertices, mesh->mNumVertices);

    if (mesh->HasNormals())
    {
      AddVertexAttribs(normals, mesh->mNormals, mesh->mNumVertices);
    }
    else
    {
      normals.reserve(mesh->mNumVertices);
      for (unsigned i{}; i < mesh->mNumVertices; ++i)
      {
        normals.emplace_back(0.f, 1.f, 0.f);
      }
    }

    // transfer indices
    for (unsigned i{}, totalCount{ static_cast<unsigned>(indices.size()) };
      i < mesh->mNumFaces; ++i)
    {
      auto const& face{ mesh->mFaces[i] };
      totalCount += face.mNumIndices;
      indices.reserve(totalCount);
      for (unsigned j{}; j < face.mNumIndices; ++j) {
        indices.emplace_back(face.mIndices[j]);
      }
    }

  }

  VertexAttribs GeneratePoint()
  {
    VertexAttribs mesh{};

    mesh.points = {
      { 0.f, 0.f, 0.f }
    };

    return mesh;
  }

  VertexAttribs GenerateLine()
  {
    VertexAttribs mesh{};

    mesh.points = {
      { -0.5f, 0.f, 0.0f },
      {  0.5f, 0.f, 0.0f }
    };

    return mesh;
  }

  VertexAttribs GenerateTriangle()
  {
    VertexAttribs mesh{};

    mesh.points = {
      { -0.5f, -0.5f, 0.0f },
      {  0.5f, -0.5f, 0.0f },
      {  0.0f,  0.5f, 0.0f }
    };

    mesh.indices = {
      0, 1, 2
    };

    return mesh;
  }


  VertexAttribs GeneratePlane(int slices) {
    VertexAttribs mesh{};

    const float radius = 1000.0f;
    const float theta = static_cast<float>(2.f * glm::pi<float>()) / static_cast<float>(slices);

    // Compute the center vertex
    mesh.points.emplace_back(0.f, 0.f, 0.f);
    mesh.normals.emplace_back(0.f, 1.f, 0.f);
    mesh.texCoords.emplace_back(0.5f, 0.5f);

    // Compute each vertex on the circle
    for (int i = 0; i <= slices; ++i) {
      float angle = theta * static_cast<float>(i);
      float x = radius * cosf(angle);
      float z = radius * sinf(angle);
      mesh.points.emplace_back(x, 0.f, z);
      mesh.normals.emplace_back(0.f, 1.f, 0.f);
      mesh.texCoords.emplace_back((cosf(angle) + 1.0f) / 2.0f, (sinf(angle) + 1.0f) / 2.0f);
    }

    // Generate indices for triangle fan
    for (int i = 1; i <= slices; ++i) {
      mesh.indices.push_back(0);
      mesh.indices.push_back(i);
      mesh.indices.push_back(i + 1);
    }

    return mesh;
  }



  VertexAttribs GenerateCube(GLfloat side /*= 1.0f*/)
  {
    VertexAttribs mesh{};
    mesh.points = {
      // Front
      { -side, -side, side }, { side, -side, side }, { side,  side, side },  { -side,  side, side },
      // Right
      { side, -side, side }, { side, -side, -side }, { side,  side, -side }, { side,  side, side },
      // Back
      { -side, -side, -side }, { -side,  side, -side }, { side,  side, -side }, { side, -side, -side },
      // Left
      { -side, -side, side }, { -side,  side, side }, { -side,  side, -side }, { -side, -side, -side },
      // Bottom
      { -side, -side, side }, { -side, -side, -side }, { side, -side, -side }, { side, -side, side },
      // Top
      { -side,  side, side }, { side,  side, side }, { side,  side, -side }, { -side,  side, -side }
    };

    mesh.normals = {
      // Front
      { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f },
      // Right
      { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
      // Back
      { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f },{  0.0f, 0.0f, -1.0f },
      // Left
      { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f },
      // Bottom
      { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, -1.0f, 0.0f },
      // Top
      { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }
    };

    mesh.texCoords = {
      // Front
      { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
      // Right
      { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
      // Back
      { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
      // Left
      { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
      // Bottom
      { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
      // Top
     { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
    };

    mesh.indices = {
        0,1,2,0,2,3,
        4,5,6,4,6,7,
        8,9,10,8,10,11,
        12,13,14,12,14,15,
        16,17,18,16,18,19,
        20,21,22,20,22,23
    };

    return mesh;
  }


  VertexAttribs GenerateSphere(float radius /*= 1.0f*/, GLuint nSlices /*= 20*/, GLuint nStacks /*= 20*/)
  {
    const int nVerts = (nSlices + 1) * (nStacks + 1);
    const int nFaces = (nSlices * 2 * (nStacks - 1)) * 3;
    VertexAttribs mesh{};

    mesh.points.reserve(nVerts);
    mesh.normals.reserve(nVerts);
    mesh.texCoords.reserve(nVerts);
    mesh.indices.resize(nFaces);

    // Generate positions and normals
    GLfloat theta, phi;
    GLfloat thetaFac = glm::two_pi<float>() / nSlices;
    GLfloat phiFac = glm::pi<float>() / nStacks;
    GLfloat nx, ny, nz, s, t;
    GLuint idx = 0;
    for (GLuint i = 0; i <= nSlices; ++i) {
      theta = i * thetaFac;
      s = (GLfloat)i / nSlices;
      for (GLuint j = 0; j <= nStacks; ++j) {
        phi = j * phiFac;
        t = (GLfloat)j / nStacks;
        nx = sinf(phi) * cosf(theta);
        ny = sinf(phi) * sinf(theta);
        nz = cosf(phi);
        mesh.points.emplace_back(radius * nx, radius * ny, radius * nz);
        mesh.normals.emplace_back(nx, ny, nz);
        mesh.texCoords.emplace_back(s, t);
      }
    }

    // Generate the element list
    idx = 0;
    for (GLuint i = 0; i < nSlices; i++) {
      GLuint stackStart = i * (nStacks + 1);
      GLuint nextStackStart = (i + 1) * (nStacks + 1);
      for (GLuint j = 0; j < nStacks; j++)
        if (j == 0)
        {
          mesh.indices[idx] = stackStart;
          mesh.indices[idx + 1] = stackStart + 1;
          mesh.indices[idx + 2] = nextStackStart + 1;
          idx += 3;
        }
        else if (j == nStacks - 1)
        {
          mesh.indices[idx] = stackStart + j;
          mesh.indices[idx + 1] = stackStart + j + 1;
          mesh.indices[idx + 2] = nextStackStart + j;
          idx += 3;
        }
        else
        {
          mesh.indices[idx] = stackStart + j;
          mesh.indices[idx + 1] = stackStart + j + 1;
          mesh.indices[idx + 2] = nextStackStart + j + 1;
          mesh.indices[idx + 3] = nextStackStart + j;
          mesh.indices[idx + 4] = stackStart + j;
          mesh.indices[idx + 5] = nextStackStart + j + 1;
          idx += 6;
        }
    }

    return mesh;
  }
} // namespace Graphics


namespace
{
  void AddVertexAttribs(std::vector<glm::vec3>& dest, aiVector3D const* arr, unsigned count)
  {
    dest.reserve(dest.size() + count);
    aiVector3D const* ptr{ arr };
    for (unsigned i{}; i < count; ++i, ++ptr)
    {
      dest.emplace_back(ptr->x, ptr->y, ptr->z);
    }
  }
} // anonymous namespace
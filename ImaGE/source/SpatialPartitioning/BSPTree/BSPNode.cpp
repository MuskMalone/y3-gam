#include <pch.h>
#include "BSPNode.h"

void SerializeVec4(std::string const& name, glm::vec4 const& vec, rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator)
{
  rapidjson::Value nameJson{ name.c_str(), allocator };
  rapidjson::Value vecJson;
  vecJson.SetArray();

  vecJson.PushBack(vec.x, allocator);
  vecJson.PushBack(vec.y, allocator);
  vecJson.PushBack(vec.z, allocator);
  vecJson.PushBack(vec.w, allocator);

  value.AddMember(nameJson, vecJson.Move(), allocator);
}

// Spatial Partitioning
namespace SP
{
  unsigned BSPNode::clrIdx{};

  void BSPNode::Initialize()
  {
    // create VAO
    GLuint vbo;
    glCreateBuffers(1, &vbo);

    glNamedBufferStorage(vbo, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

    // vtx pos arr
    glCreateVertexArrays(1, &vao);

    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(glm::vec3));
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    glBindVertexArray(0); // unbind VAO

    if (leftChild) {
      leftChild->Initialize();
    }
    if (rightChild) {
      rightChild->Initialize();
    }
  }


  void BSPNode::Draw(Graphics::ShaderProgram& shaders) const
  {
    shaders.SetUniform("uVtxClr", drawClr);
    shaders.SetUniform("uMdlTransform", glm::mat4(1.f));

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));

    if (leftChild) {
      leftChild->Draw(shaders);
    }
    if (rightChild) {
      rightChild->Draw(shaders);
    }
  }

  void BSPNode::Serialize(rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator) const
  {
    rapidjson::Value nodeJson{ rapidjson::kObjectType };
    SerializeVec4("drawClr", drawClr, nodeJson, allocator);

    {
      rapidjson::Value leftName{ "leftChild", allocator };
      rapidjson::Value rightName{ "rightChild", allocator };
      rapidjson::Value leftChildJson{ leftChild ? rapidjson::kTrueType : rapidjson::kFalseType };
      rapidjson::Value rightChildJson{ rightChild ? rapidjson::kTrueType : rapidjson::kFalseType };

      nodeJson.AddMember(leftName, leftChildJson, allocator);
      nodeJson.AddMember(rightName, rightChildJson, allocator);
    }

    rapidjson::Value nameJson{ "vertices", allocator };
    rapidjson::Value vecArr{ rapidjson::kArrayType };

    for (auto const& vertex : vertices)
    {
      rapidjson::Value vecJson(rapidjson::kArrayType);
      vecJson.PushBack(vertex.x, allocator);
      vecJson.PushBack(vertex.y, allocator);
      vecJson.PushBack(vertex.z, allocator);
      vecArr.PushBack(vecJson, allocator);
    }
    nodeJson.AddMember(nameJson, vecArr, allocator);
    value.PushBack(nodeJson, allocator);

    if (leftChild) { leftChild->Serialize(value, allocator); }
    if (rightChild) { rightChild->Serialize(value, allocator); }
  }

  void BSPNode::Deserialize(rapidjson::Value const& arr, rapidjson::SizeType idx)
  {
    auto elem{ arr[idx].GetObject() };

    auto verticesArr{ elem["vertices"].GetArray() };
    vertices.reserve(verticesArr.Size());

    for (auto const& pt : verticesArr)
    {
      auto vecJson{ pt.GetArray() };
      vertices.emplace_back(vecJson[0].GetFloat(), vecJson[1].GetFloat(), vecJson[2].GetFloat());
    }

    {
      auto clrJson{ elem["drawClr"].GetArray() };
      drawClr = { clrJson[0].GetFloat(), clrJson[1].GetFloat(), clrJson[2].GetFloat(), clrJson[3].GetFloat() };
    }

    if (elem["leftChild"].GetBool())
    {
      leftChild = std::make_shared<BSPNode>();
      leftChild->Deserialize(arr, ++idx);
    }
    if (elem["rightChild"].GetBool())
    {
      rightChild = std::make_shared<BSPNode>();
      rightChild->Deserialize(arr, ++idx);
    }
  }

} // namespace SP


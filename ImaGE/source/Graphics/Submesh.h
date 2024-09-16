namespace Graphics{
    struct Submesh {
        uint32_t baseVtx;  // Start vertex in the vertex buffer
        uint32_t baseIdx;   // Start index in the index buffer
        uint32_t vtxCount;
        uint32_t idxCount;  // Number of indices this submesh draws
        uint32_t materialIdx;  // Reference to the material to use
        glm::mat4 transform;  // Transform of the submesh, default to identity matrix

        // Constructors
        Submesh() : baseVtx{}, baseIdx{}, vtxCount{}, idxCount {}, materialIdx{}, transform{ glm::mat4{ 1.f } } {}
        Submesh(uint32_t vtx, uint32_t idx, uint32_t vtxCnt, uint32_t idxCnt, uint32_t matIdx, const glm::mat4& xform)
            : baseVtx{ vtx }, baseIdx{ idx }, vtxCount{vtxCnt}, idxCount{ idxCnt }, materialIdx{ matIdx }, transform{ xform } {}
    };
}
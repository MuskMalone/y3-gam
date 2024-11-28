#version 460 core

in vec2 fragTexCoord; // Received texture coordinate from vertex shader

uniform sampler2D u_ScreenTex;
uniform usampler2D u_EntityTex;
uniform int u_SelectedEntity;
uniform vec2 u_TexelSize;

out vec4 fragColor;

void main() {
    uint centerID = texture(u_EntityTex, fragTexCoord).r;

    if (centerID != u_SelectedEntity) {
        discard; // Ignore pixels not belonging to the highlighted entity
    }

    // Detect edges by comparing neighboring texels within a radius
    bool isEdge = false;
    float radius = 5.0; // Radius in texels for thicker highlight
    float radiusSquared = radius * radius;

    for (int x = -int(radius); x <= int(radius); ++x) {
        for (int y = -int(radius); y <= int(radius); ++y) {
            vec2 offset = vec2(x, y) * u_TexelSize;
            vec2 neighborCoord = fragTexCoord + offset;

            // Skip sampling if neighborCoord is out of bounds
            if (neighborCoord.x < 0.0 || neighborCoord.x > 1.0 ||
                neighborCoord.y < 0.0 || neighborCoord.y > 1.0) {
                continue;
            }

            // Check for edges
            if (x * x + y * y <= radiusSquared) { // Check within circular radius
                if (texture(u_EntityTex, neighborCoord).r != u_SelectedEntity) {
                    isEdge = true;
                    break; // Exit early if an edge is found
                }
            }
        }
        if (isEdge) break; // Exit outer loop early if an edge is found
    }

    if (isEdge) {
        fragColor = vec4(1.0, 0.5, 0.0, 1.0); // Orange highlight
    } else {
        discard; // Ignore non-edge pixels
    }
}

#version 460 core

layout(location = 0) out int entityID;  // Output an integer

uniform int u_InstanceID;  // Instance ID, passed in as uniform

void main() {
    entityID = u_InstanceID;  // Set the output color to the instance ID
}

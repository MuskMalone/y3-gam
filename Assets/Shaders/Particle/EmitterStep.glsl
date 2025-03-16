#version 460 core
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
//#extension GL_ARB_gpu_shader_uint64 : enable 
#include "..\\Assets\\Shaders\\Particle\\Common.glsl"
// local work group is 100 large. I believe ideal local size would be GCD(num_cores, num_particles)
// More testing needed
layout( local_size_x = 1000, local_size_y = 1, local_size_z = 1 ) in;

// uniform control variables
uniform float DT;

uniform uint bufferMaxCount = 1000000;
//from -1 to 1

float hello;

float random(){ //retun
    uint idx = atomicAdd(Variables[VARIABLE_RAND_IDX], uint(1));
	return RandomFloats[idx % bufferMaxCount];
    // return 1.0;
}

//from 0 to 1
float rand(){
    return fract(sin(dot(vec2(random()), vec2(12.9898, 78.233))) * 43758.5453);
    // return 0.0;
}
float randRange(vec2 range){
    return range.x + ((range.y - range.x) * rand());
    // return 0.0;
}
vec3 randDir(float startAngle, float endAngle, float magnitude) {
    // Generate a random angle in spherical coordinates
    float theta = radians(randRange(vec2(0.0, 360.0))); // Random azimuth angle (0 to 360 degrees)
    float phi = radians(randRange(vec2(startAngle, endAngle))); // Random polar angle (start to end)

    // Convert spherical coordinates to Cartesian coordinates
    float sinPhi = sin(phi);
    float x = sinPhi * cos(theta);
    float y = sinPhi * sin(theta);
    float z = cos(phi);

    // Create the 3D direction vector and scale it by the magnitude
    return vec3(x, y, z) * magnitude;
    // return vec3(0);
}

void spawnParticle(Particle pctl){
    uint idx = atomicAdd(Variables[VARIABLE_PARTICLE_COUNT], uint(1));
    Particles[idx % bufferMaxCount] = pctl;
    ParticlesStart[idx % bufferMaxCount] = pctl;
}
void spawnParticlePoint(uint emtidx){
    spawnParticle(Particle(
        Emitters[emtidx].col, //
        vec3(Emitters[emtidx].vertices[0]),
        randDir(0, 360, Emitters[emtidx].speed), // velocity'        
        Emitters[emtidx].gravity, 
        Emitters[emtidx].size,
        Emitters[emtidx].angvel, 
        0, 
        Emitters[emtidx].lifetime, 
        int(emtidx),
        true
    ));
}

// Helper: generates a random direction within a cone around a base direction.
// Uses rand() (already defined) and the spread angle in degrees.
vec3 randomConeDirection(vec3 baseDir, float spreadAngle) {
    vec3 normDir = normalize(baseDir);
    // Build an orthonormal basis (u,v,normDir)
    vec3 up = abs(normDir.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 u = normalize(cross(up, normDir));
    vec3 v = cross(normDir, u);
    
    // Interpolate between cos(spreadAngle) and 1.0 to get a random cone offset.
    float cosTheta = mix(cos(radians(spreadAngle)), 1.0, rand());
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    float phi = 2.0 * 3.14159265 * rand();
    return (sinTheta * cos(phi)) * u + (sinTheta * sin(phi)) * v + (cosTheta) * normDir;
}

// For 1-vertex (point) cone preset.
// If the emitter’s velocity has significant magnitude, use it as the base direction.
void spawnConeEmitterParticlePoint(uint emtidx) {
    const float EPSILON = 1e-6;
    vec3 baseVel = Emitters[emtidx].vel;
    vec3 particleVel = (length(baseVel) >= EPSILON)
        ? randomConeDirection(baseVel, Emitters[emtidx].spreadAngle) * Emitters[emtidx].speed
        : randDir(0.0, 360.0, Emitters[emtidx].speed);
    spawnParticle(Particle(
        Emitters[emtidx].col,
        vec3(Emitters[emtidx].vertices[0]),
        particleVel,
        Emitters[emtidx].gravity,
        Emitters[emtidx].size,
        Emitters[emtidx].angvel,
        0,
        Emitters[emtidx].lifetime,
        int(emtidx),
        true
    ));
}

// For 2-vertex (line) emitter preset: default version.
// Picks a random point on the line between vertices[0] and vertices[1] and
// emits a particle in a random direction perpendicular to the line (with spread).
void spawnLineEmitterParticle(uint emtidx) {
    // Random point along the line.
    vec3 pos = mix(vec3(Emitters[emtidx].vertices[0]), vec3(Emitters[emtidx].vertices[1]), rand());
    // Compute the line direction.
    vec3 lineDir = vec3(Emitters[emtidx].vertices[1]) - vec3(Emitters[emtidx].vertices[0]);
    vec3 normLine = normalize(lineDir);
    // Pick an arbitrary perpendicular base direction.
    vec3 up = abs(normLine.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 perp = normalize(cross(normLine, up));
    // Use the cone helper to add some spread around the perpendicular.
    vec3 particleVel = randomConeDirection(perp, Emitters[emtidx].spreadAngle) * Emitters[emtidx].speed;
    spawnParticle(Particle(
        Emitters[emtidx].col,
        pos,
        particleVel,
        Emitters[emtidx].gravity,
        Emitters[emtidx].size,
        Emitters[emtidx].angvel,
        0,
        Emitters[emtidx].lifetime,
        int(emtidx),
        true
    ));
}

// For 2-vertex (line) cone preset:
// Like the point cone preset but with the emitter’s base position chosen along the line.
void spawnLineConeEmitterParticle(uint emtidx) {
    vec3 pos = mix(vec3(Emitters[emtidx].vertices[0]), vec3(Emitters[emtidx].vertices[1]), rand());
    const float EPSILON = 1e-6;
    vec3 baseVel = Emitters[emtidx].vel;
    vec3 particleVel = (length(baseVel) >= EPSILON)
        ? randomConeDirection(baseVel, Emitters[emtidx].spreadAngle) * Emitters[emtidx].speed
        : randDir(0.0, 360.0, Emitters[emtidx].speed);
    spawnParticle(Particle(
        Emitters[emtidx].col,
        pos,
        particleVel,
        Emitters[emtidx].gravity,
        Emitters[emtidx].size,
        Emitters[emtidx].angvel,
        0,
        Emitters[emtidx].lifetime,
        int(emtidx),
        true
    ));
}

// Helper: returns a random point on a quadrilateral plane (assumes vertices 0,1,2,3 form the plane).
vec3 randomPointOnPlane(uint emtidx) {
    float s = rand();
    float t = rand();
    vec3 v0 = vec3(Emitters[emtidx].vertices[0]);
    vec3 v1 = vec3(Emitters[emtidx].vertices[1]);
    vec3 v2 = vec3(Emitters[emtidx].vertices[2]);
    vec3 v3 = vec3(Emitters[emtidx].vertices[3]);
    // Bilinear interpolation:
    return mix(mix(v0, v1, s), mix(v3, v2, s), t);
}

// For 4-vertex (plane) emitter preset: default version.
// Emits from a random point on the plane, with a direction given by the plane’s normal (plus spread).
void spawnPlaneEmitterParticle(uint emtidx) {
    vec3 pos = randomPointOnPlane(emtidx);
    // Compute plane normal from vertices 0,1,3.
    vec3 v0 = vec3(Emitters[emtidx].vertices[0]);
    vec3 v1 = vec3(Emitters[emtidx].vertices[1]);
    vec3 v3 = vec3(Emitters[emtidx].vertices[3]);
    vec3 normal = normalize(cross(v1 - v0, v3 - v0));
    vec3 particleVel = randomConeDirection(normal, Emitters[emtidx].spreadAngle) * Emitters[emtidx].speed;
    spawnParticle(Particle(
        Emitters[emtidx].col,
        pos,
        particleVel,
        Emitters[emtidx].gravity,
        Emitters[emtidx].size,
        Emitters[emtidx].angvel,
        0,
        Emitters[emtidx].lifetime,
        int(emtidx),
        true
    ));
}

// For 4-vertex (plane) cone preset:
// Like the cone preset for points but with the base position chosen on the plane.
void spawnPlaneConeEmitterParticle(uint emtidx) {
    vec3 pos = randomPointOnPlane(emtidx);
    const float EPSILON = 1e-6;
    vec3 baseVel = Emitters[emtidx].vel;
    vec3 particleVel = (length(baseVel) >= EPSILON)
        ? randomConeDirection(baseVel, Emitters[emtidx].spreadAngle) * Emitters[emtidx].speed
        : randDir(0.0, 360.0, Emitters[emtidx].speed);
    spawnParticle(Particle(
        Emitters[emtidx].col,
        pos,
        particleVel,
        Emitters[emtidx].gravity,
        Emitters[emtidx].size,
        Emitters[emtidx].angvel,
        0,
        Emitters[emtidx].lifetime,
        int(emtidx),
        true
    ));
}

// Helper: returns a random point inside an irregular cube.
// Assumes vertices 0-3 form the top face and 4-7 form the bottom face.
vec3 randomPointInCube(uint emtidx) {
    float s = rand();
    float t = rand();
    float u = rand();
    vec3 v0 = vec3(Emitters[emtidx].vertices[0]);
    vec3 v1 = vec3(Emitters[emtidx].vertices[1]);
    vec3 v2 = vec3(Emitters[emtidx].vertices[2]);
    vec3 v3 = vec3(Emitters[emtidx].vertices[3]);
    // top face via bilinear interpolation.
    vec3 top = mix(mix(v0, v1, s), mix(v3, v2, s), t);
    vec3 v4 = vec3(Emitters[emtidx].vertices[4]);
    vec3 v5 = vec3(Emitters[emtidx].vertices[5]);
    vec3 v6 = vec3(Emitters[emtidx].vertices[6]);
    vec3 v7 = vec3(Emitters[emtidx].vertices[7]);
    // bottom face.
    vec3 bottom = mix(mix(v4, v5, s), mix(v7, v6, s), t);
    return mix(bottom, top, u);
}

// For 8-vertex (cube) emitter preset.
// Spawns a particle at a random point inside the cube, with a random direction.
void spawnCubeEmitterParticle(uint emtidx) {
    vec3 pos = randomPointInCube(emtidx);
    vec3 particleVel = randDir(0.0, 360.0, Emitters[emtidx].speed);
    spawnParticle(Particle(
        Emitters[emtidx].col,
        pos,
        particleVel,
        Emitters[emtidx].gravity,
        Emitters[emtidx].size,
        Emitters[emtidx].angvel,
        0,
        Emitters[emtidx].lifetime,
        int(emtidx),
        true
    ));
}

// New version of spawnEmitterParticle: dispatches based on vertex count and type.
// For 1 vertex: if type is EMT_TYPE_GRADUAL, use the original point preset;
// otherwise use the cone preset.
// For 2 vertices: if type is EMT_TYPE_RAIN, use line emitter; else use line cone emitter.
// For 4 vertices: if type is EMT_TYPE_DUST, use plane emitter; else use plane cone emitter.
// For 8 vertices: always use the cube preset.
void spawnEmitterParticle(uint emtidx) {
    int vCount = Emitters[emtidx].vCount;
    int particlesPerFrame = Emitters[emtidx].particlesPerFrame;
    for (int i = 0; i < particlesPerFrame; ++i) {
        if(vCount == VCOUNT_POINT) { // 1 vertex
            if(Emitters[emtidx].type == EMT_TYPE_GRADUAL) {
                spawnParticlePoint(emtidx);
            } else {
                spawnConeEmitterParticlePoint(emtidx);
            }
        } else if(vCount == VCOUNT_LINE) { // 2 vertices
            if(Emitters[emtidx].type == EMT_TYPE_RAIN) {
                spawnLineEmitterParticle(emtidx);
            } else {
                spawnLineConeEmitterParticle(emtidx);
            }
        } else if(vCount == VCOUNT_QUAD) { // 4 vertices
            if(Emitters[emtidx].type == EMT_TYPE_DUST) {
                spawnPlaneEmitterParticle(emtidx);
            } else {
                spawnPlaneConeEmitterParticle(emtidx);
            }
        } else if(vCount == VCOUNT_IRREGULAR) { // irregular cube emitter
            spawnCubeEmitterParticle(emtidx);
        }
    }
}


void main() {
    // gid used as index into SSBO to find the particle
    // that any particular instance is controlling
    uint gid = gl_GlobalInvocationID.x;
    if (Emitters[gid].alive == true){
        //for each emitter
        Emitters[gid].time += DT;
        if (Emitters[gid].time >= Emitters[gid].frequency){
            Emitters[gid].time = 0.0;
            spawnEmitterParticle(gid);
        }
    }
}
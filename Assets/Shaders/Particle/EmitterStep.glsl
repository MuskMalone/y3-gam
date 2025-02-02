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

uniform uint bufferMaxCount = 5000000;
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

// vec2 randDirInRange(vec2 baseDir, float angleDegree, float magnitude) {
    
//     float angleRange = radians(angleDegree);
//     // Normalize the base direction
//     vec2 normBaseDir = normalize(baseDir);

//     // Generate a random angle offset within the specified range
//     float angleOffset = randRange(vec2(-angleRange / 2.0, angleRange / 2.0));

//     // Create a rotation matrix
//     float s = sin(angleOffset);
//     float c = cos(angleOffset);
//     mat2 rotMat = mat2(
//         c, -s,
//         s, c
//     );

//     // Apply the rotation and scale by magnitude
//     return (normBaseDir * rotMat) * magnitude;
// }
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

//emtidz is the indx of the Emitter
//vtx1 and vtx2 are the indices of the vertices
//angleRange is the range of angles in degrees wrt to normal
// void spawnParticleRange(uint emtidx, int vtx1, int vtx2, float angleRange){
//     //rotate by 90 degrees to find normal;
//     vec2 vec = vec2(Emitters[emtidx].vertices[vtx2] - Emitters[emtidx].vertices[vtx1]);
//     vec2 nml = normalize(vec2(-vec.y, vec.x));
//     vec2 point = vec2(Emitters[emtidx].vertices[vtx1]) + (rand() * vec);
    
//     spawnParticle(Particle(
//         Emitters[emtidx].col, //
//         vec4(point, 0, 1),
//         randDirInRange(nml, angleRange, Emitters[emtidx].speed), // velocity
//         Emitters[emtidx].gravity,
//         Emitters[emtidx].size,
//         Emitters[emtidx].gravity, 
//         0, 
//         Emitters[emtidx].lifetime, 
//         Emitters[emtidx].angvel, 
//         int(emtidx),
//         true
//     ));
// }

void spawnEmitterParticle(uint emtidx){
    int type = Emitters[emtidx].type;
    int vCount = Emitters[emtidx].vCount;
    int particlesPerFrame = Emitters[emtidx].particlesPerFrame;
    // if (vCount == 1){ // is point
        // if (type == EMT_TYPE_GRADUAL){
            //inside the if block so that lesser comparisions per loop
            for (int i = 0; i < particlesPerFrame; ++i){
                spawnParticlePoint(emtidx);
            }
        // }
    // }
    // else if (vCount == 2){// is line
    //     if (type == EMT_TYPE_GRADUAL){
    //         for (int i = 0; i < particlesPerFrame; ++i){
    //             spawnParticleRange(emtidx, 0, 1, 360);
    //         }
    //     }
    //     else if (type == EMT_TYPE_RAIN){
    //         for (int i = 0; i < particlesPerFrame; ++i){
    //             spawnParticleRange(emtidx, 0, 1, 30);}
    //     }
	// 	else if (type == EMT_TYPE_LAZER){
    //         for (int i = 0; i < particlesPerFrame; ++i){
    //             spawnParticleRange(emtidx, 0, 1, 0);}
    //     }
    // }
    // else if (vCount == 4){// is quad
    //     if (type == EMT_TYPE_GRADUAL){
    //         for (int i = 0; i < particlesPerFrame; ++i){
    //             int randCode = int(randRange(vec2(1, 4.99)));
    //             int start, end, angle;
    //             if (randCode == 1) {start = 0; end = 1;}
    //             else if (randCode == 2) {start = 1; end = 2;}
    //             else if (randCode == 3) {start = 2; end = 3;}
    //             else if (randCode == 4) {start = 3; end = 0;}
    //             spawnParticleRange(emtidx, start, end, 180);}
    //     }
    //     else if (type == EMT_TYPE_DUST){
    //         vec4 maxVert = max(Emitters[emtidx].vertices[0], Emitters[emtidx].vertices[2]); // max values from top left and bottom right
    //         vec4 minVert = min(Emitters[emtidx].vertices[0], Emitters[emtidx].vertices[2]); // min values from top left and bottom right

    //         for (int i = 0; i < particlesPerFrame; ++i){
                
    //             vec2 point = vec2(randRange(vec2(minVert.x, maxVert.x)), randRange(vec2(minVert.y, maxVert.y)));
    //             spawnParticle(Particle(
	// 			    Emitters[emtidx].col, //
	// 			    vec4(point, 0, 1),
	// 			    randDir(0, 360, Emitters[emtidx].speed), // velocity
	// 			    Emitters[emtidx].gravity,
	// 			    Emitters[emtidx].size,
	// 			    Emitters[emtidx].gravity, 
	// 			    0, 
	// 			    Emitters[emtidx].lifetime, 
	// 			    Emitters[emtidx].angvel, 
	// 			    int(emtidx),
	// 			    true
	// 		    ));}
    //     }
    //     else if (type == EMT_TYPE_DISINTEGRATE){
            
    //         vec2 maxVert = vec2(max(Emitters[emtidx].vertices[0], Emitters[emtidx].vertices[2])); // max values from top left and bottom right
    //         vec2 minVert = vec2(min(Emitters[emtidx].vertices[0], Emitters[emtidx].vertices[2])); // min values from top left and bottom right
    //         vec2 center = (maxVert + minVert) / 2;
    //         vec2 dims = maxVert - minVert;
    //         vec2 invdims = dims / float(particlesPerFrame);

    //         for (int i = 0; i < particlesPerFrame; ++i){
    //             for (int j = 0; j < particlesPerFrame; ++j){
    //                 vec2 point = vec2(minVert) + (vec2(invdims) * vec2(float(i), float(j)) + vec2(invdims * 0.5));
    //                 spawnParticle(Particle(
	// 			        Emitters[emtidx].col, //
	// 			        vec4(point, 0, 1),
	// 			        randDir(0, 360, Emitters[emtidx].speed),
	// 			        Emitters[emtidx].gravity,
	// 			        invdims,
	// 			        Emitters[emtidx].gravity, 
	// 			        0, 
	// 			        Emitters[emtidx].lifetime, 
	// 			        Emitters[emtidx].angvel, 
	// 			        int(emtidx),
	// 			        true
	// 		        ));
    //             }
    //         }
    //     }


    // }
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
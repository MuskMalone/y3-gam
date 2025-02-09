/**
 * @file Emitter.glsl
 * @author t.chenghian@digipen.edu
*/

#version 460 core
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

#include "..\\Assets\\Shaders\\Particle\\Common.glsl"

// local work group is 100 large. I believe ideal local size would be GCD(num_cores, num_particles)
// More testing needed
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1 ) in;

// uniform vec2 uTexcoords[4];
// uniform uvec2 uTexhdl;

//variables to store the new emitter
// uniform vec4 uEmtvertices[4]; // Each vec4 is 16 bytes, total 64 bytes
// uniform vec4 uEmtcol;         // 16 bytes (vec3 is aligned like vec4)
// uniform vec2 uEmtsize;    // 8 bytes (vec2 is aligned to 8 bytes)

// uniform float uEmtrot;    // 4 bytes
// uniform float uEmtlifetime; // 4 bytes
// uniform float uEmtangvel;  // 4 bytes
// uniform float uEmtspeed;

// uniform float uEmtfrequency;  // 4 bytes
// uniform int uEmttype;         // 4 bytes
// // 1 for point, 2 for line, 4 for rect
// uniform int uEmtvCount;       // 4 byte
// uniform int uEmtpreset;
// uniform int uEmtparticlesPerFrame;

uniform Emitter emitter;

uniform int emtTargetIdx = -1;

uniform int spawnEmitter = 0;

void main() {
        if (spawnEmitter == 1){ // spawn emitter
            Emitters[emtTargetIdx] = emitter;
            Emitters[emtTargetIdx].time = 0.0;       // 4 bytes, but due to the vec3 above, you can expect padding here

        }
        
        else if (spawnEmitter == -1){ //delete emitter
            Emitters[emtTargetIdx].alive = false;       // 4 bytes (bools are often treated as 4 bytes for alignment)
            
        }
        else{ //edit emitter
            Emitter prevEmitter = Emitters[emtTargetIdx];
            Emitters[emtTargetIdx] = emitter;
            Emitters[emtTargetIdx].time = prevEmitter.time;
            // Emitters[emtTargetIdx].alive = true;

        }
}
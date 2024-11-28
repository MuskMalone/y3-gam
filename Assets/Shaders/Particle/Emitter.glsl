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
uniform vec4 uEmtvertices[4]; // Each vec4 is 16 bytes, total 64 bytes
uniform vec4 uEmtcol;         // 16 bytes (vec3 is aligned like vec4)

uniform vec2 uEmtgravity; // 8 bytes
uniform vec2 uEmtsize;    // 8 bytes (vec2 is aligned to 8 bytes)
uniform float uEmtrot;    // 4 bytes
uniform float uEmtlifetime; // 4 bytes
uniform float uEmtangvel;  // 4 bytes
uniform float uEmtspeed;

uniform float uEmtfrequency;  // 4 bytes
// type of emmission
//0: smoke
//1: fire
//2: burst
//3: burst with gravity
//4: gradual emission
uniform int uEmttype;         // 4 bytes
// 1 for point, 2 for line, 4 for rect
uniform int uEmtvCount;       // 4 byte
uniform int uEmtpreset;
uniform int uEmtparticlesPerFrame;
uniform int emtTargetIdx = -1;

uniform int spawnEmitter = 0;

void main() {
        // if (spawnEmitter == 1){ // spawn emitter
        //     Emitters[emtTargetIdx].vertices[0] = uEmtvertices[0]; // Each vec4 is 16 bytes, total 64 bytes
        //     Emitters[emtTargetIdx].vertices[1] = uEmtvertices[1]; // Each vec4 is 16 bytes, total 64 bytes
        //     Emitters[emtTargetIdx].vertices[2] = uEmtvertices[2]; // Each vec4 is 16 bytes, total 64 bytes
        //     Emitters[emtTargetIdx].vertices[3] = uEmtvertices[3]; // Each vec4 is 16 bytes, total 64 bytes

        //     Emitters[emtTargetIdx].col = uEmtcol;         // 16 bytes (vec3 is aligned like vec4)

        //     Emitters[emtTargetIdx].gravity = uEmtgravity; // 8 bytes
        //     Emitters[emtTargetIdx].size = uEmtsize;    // 8 bytes (vec2 is aligned to 8 bytes)
        //     Emitters[emtTargetIdx].rot = uEmtrot;    // 4 bytes
        //     Emitters[emtTargetIdx].lifetime = uEmtlifetime; // 4 bytes
        //     Emitters[emtTargetIdx].angvel = uEmtangvel;  // 4 bytes
        //     Emitters[emtTargetIdx].speed = uEmtspeed;

        //     Emitters[emtTargetIdx].time = 0.0;       // 4 bytes, but due to the vec3 above, you can expect padding here
        //     Emitters[emtTargetIdx].frequency = uEmtfrequency;  // 4 bytes
        //     Emitters[emtTargetIdx].type = uEmttype;         // 4 bytes
        //     Emitters[emtTargetIdx].vCount = uEmtvCount;       // 4 bytes
        //     Emitters[emtTargetIdx].preset = uEmtpreset;
        //     Emitters[emtTargetIdx].particlesPerFrame = uEmtparticlesPerFrame;
            
        //     Emitters[emtTargetIdx].alive = true;       // 4 bytes (bools are often treated as 4 bytes for alignment)

        //     // TexHdls[emtTargetIdx].texCoords[0] = uTexcoords[0];
        //     // TexHdls[emtTargetIdx].texCoords[1] = uTexcoords[1];
        //     // TexHdls[emtTargetIdx].texCoords[2] = uTexcoords[2];
        //     // TexHdls[emtTargetIdx].texCoords[3] = uTexcoords[3];

        //     // TexHdls[emtTargetIdx].texHdl = uTexhdl;
        // }
        
        // else if (spawnEmitter == -1){ //delete emitter
        //     Emitters[emtTargetIdx].alive = false;       // 4 bytes (bools are often treated as 4 bytes for alignment)
            
        //     // TexHdls[emtTargetIdx].texCoords[0] = vec2(0);
        //     // TexHdls[emtTargetIdx].texCoords[1] = vec2(0);
        //     // TexHdls[emtTargetIdx].texCoords[2] = vec2(0);
        //     // TexHdls[emtTargetIdx].texCoords[3] = vec2(0);

        //     // TexHdls[emtTargetIdx].texHdl = uvec2(0);
        // }
        // else{ //edit emitter
        //     Emitters[emtTargetIdx].vertices[0] = uEmtvertices[0]; // Each vec4 is 16 bytes, total 64 bytes
        //     Emitters[emtTargetIdx].vertices[1] = uEmtvertices[1]; // Each vec4 is 16 bytes, total 64 bytes
        //     Emitters[emtTargetIdx].vertices[2] = uEmtvertices[2]; // Each vec4 is 16 bytes, total 64 bytes
        //     Emitters[emtTargetIdx].vertices[3] = uEmtvertices[3]; // Each vec4 is 16 bytes, total 64 bytes

        //     Emitters[emtTargetIdx].col = uEmtcol;         // 16 bytes (vec3 is aligned like vec4)

        //     Emitters[emtTargetIdx].gravity = uEmtgravity; // 8 bytes
        //     Emitters[emtTargetIdx].size = uEmtsize;    // 8 bytes (vec2 is aligned to 8 bytes)
        //     Emitters[emtTargetIdx].rot = uEmtrot;    // 4 bytes
        //     Emitters[emtTargetIdx].lifetime = uEmtlifetime; // 4 bytes
        //     Emitters[emtTargetIdx].angvel = uEmtangvel;  // 4 bytes
        //     Emitters[emtTargetIdx].speed = uEmtspeed;

        //     Emitters[emtTargetIdx].frequency = uEmtfrequency;  // 4 bytes
        //     Emitters[emtTargetIdx].type = uEmttype;         // 4 bytes
        //     Emitters[emtTargetIdx].vCount = uEmtvCount;       // 4 bytes
        //     Emitters[emtTargetIdx].preset = uEmtpreset;
        //     Emitters[emtTargetIdx].particlesPerFrame = uEmtparticlesPerFrame;

        //     // TexHdls[emtTargetIdx].texCoords[0] = uTexcoords[0];
        //     // TexHdls[emtTargetIdx].texCoords[1] = uTexcoords[1];
        //     // TexHdls[emtTargetIdx].texCoords[2] = uTexcoords[2];
        //     // TexHdls[emtTargetIdx].texCoords[3] = uTexcoords[3];

        //     // TexHdls[emtTargetIdx].texHdl = uTexhdl;
        // }
}
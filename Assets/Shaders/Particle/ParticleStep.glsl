
#version 460 core
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable
// #extension GL_ARB_bindless_texture : enable

#include "..\\Assets\\Shaders\\Particle\\Common.glsl"

// local work group is 100 large. I believe ideal local size would be GCD(num_cores, num_particles)
// More testing needed
layout( local_size_x = 1000, local_size_y = 1, local_size_z = 1 ) in;

// uniform control variables
uniform float DT;

uniform uint bufferMaxCount;

//lerp
vec2 linearLerp(vec2 a, vec2 b, float t){
	return a + (b - a) * t;
}
vec3 linearLerp(vec3 a, vec3 b, float t){
	return a + (b - a) * t;
}
vec4 linearLerp(vec4 a, vec4 b, float t){
	return a + (b - a) * t;
}
float linearLerp(float a, float b, float t){
	return a + (b - a) * t;
}

void main() {
    // gid used as index into SSBO to find the particle
    // that any particular instance is controlling
    uint gid = gl_GlobalInvocationID.x;
        
    if (Particles[gid].alive == true){

    //     Particles[gid].age += DT;
    //     Particles[gid].vel += Particles[gid].gravity;
    //     Particles[gid].pos += vec4(Particles[gid].vel, 0, 1) * DT;
    //     Particles[gid].rot += Particles[gid].angvel * DT;
        
    //     switch(Emitters[Particles[gid].emtIdx].preset){
    //     case ALPHA_OVER_LIFETIME:{
    //         Particles[gid].col.a = linearLerp(ParticlesStart[gid].col.a, 0, Particles[gid].age / Particles[gid].lifetime);
    //         }
	// 		break;
    //     case SIZE_OVER_LIFETIME:{
    //         Particles[gid].size = linearLerp(ParticlesStart[gid].size, vec2(0, 0), Particles[gid].age / Particles[gid].lifetime);
    //         }
    //         break;
    //     case ALPHA_SIZE_DECR_OVER_LIFETIME:{
    //         Particles[gid].col.a = linearLerp(ParticlesStart[gid].col.a, 0, Particles[gid].age / Particles[gid].lifetime);

    //         Particles[gid].size = linearLerp(ParticlesStart[gid].size, vec2(0, 0), Particles[gid].age / Particles[gid].lifetime);
    //         }
    //         break;
    //     case ALPHA_SIZE_INCR_OVER_LIFETIME:{
    //             Particles[gid].col.a = linearLerp(ParticlesStart[gid].col.a, 0, Particles[gid].age / Particles[gid].lifetime);
    //             Particles[gid].size = linearLerp(vec2(0), ParticlesStart[gid].size, Particles[gid].age / Particles[gid].lifetime);
    //         }
    //         break;
    //     }
    //     //if particles are dead
    //     if (Particles[gid].age >= Particles[gid].lifetime){
    //         Particles[gid].alive = false;
    //     }

    }
}
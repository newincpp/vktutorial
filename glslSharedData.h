#include "glslCompat.h"

struct SH1 {
    vec3 coef0;
    vec3 coef1;
    vec3 coef2;
};

struct SH2 {
    vec3 coef0;
    vec3 coef1;
    vec3 coef2;
    vec3 coef3;
    vec3 coef4;
    vec3 coef5;
    vec3 coef6;
    vec3 coef7;
    vec3 coef8;
};
struct SHCoef4 {
    vec3 coef0;
    vec3 coef1;
    vec3 coef2;
    vec3 coef3;
};

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    float time;
    vec3 debugcolour;
    SH2 envIrradiance;
} glslDecl(ubo) ;

#include "glslCompatUndef.h"

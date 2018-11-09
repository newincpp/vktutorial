#ifdef __cplusplus

#include <glm/glm.hpp>
typedef ::glm::vec2 vec2;
typedef ::glm::vec3 vec3;
typedef ::glm::vec4 vec4;
typedef ::glm::ivec4 ivec4;
typedef ::glm::mat4 mat4;

#define layout(X)
#define in
#define uniform struct
#define glslDecl(X)
#else
#define glslDecl(X) X
#endif

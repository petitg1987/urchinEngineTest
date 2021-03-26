#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Projection {
    mat4 matrix;
} projection;
layout(std140, set = 0, binding = 1) uniform Translate {
    ivec2 distance;
} translate;

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoord;

layout(location = 0) out vec2 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;
    vec4 position = projection.matrix * vec4((vertexPosition + translate.distance), 1.0, 1.0);

    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

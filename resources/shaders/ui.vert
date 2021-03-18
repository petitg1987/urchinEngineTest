#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform mat3 mProjection;
uniform ivec2 translateDistance;

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 texCoord;

out vec2 textCoordinates;
invariant gl_Position;

void main() {
    textCoordinates = texCoord;
    vec3 position = mProjection * vec3((vertexPosition + translateDistance), 1.0);

    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

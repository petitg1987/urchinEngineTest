#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(std140, set = 0, binding = 0) uniform Projection {
    mat4 mProjection;
} projection;
layout(std140, set = 0, binding = 1) uniform View {
    mat4 mView;
} view;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 texCoord;

layout(location = 0) out vec3 texCoordinates;
invariant gl_Position;

void main() {
    texCoordinates = texCoord;

    gl_Position = projection.mProjection * (view.mView * vec4(vertexPosition, 1.0));
}

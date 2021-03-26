#version 450
#extension GL_ARB_separate_shader_objects : enable

//values are replaced at compilation time:
#define MAX_VERTICES 0
#define NUMBER_LAYER 0

layout(std140, set = 0, binding = 0) uniform Layer {
    int layersToUpdate;
} layer;

layout(triangles) in;
layout(triangle_strip, max_vertices = MAX_VERTICES) out;

layout(location = 0) in vec2 texCoordinates[];

layout(location = 0) smooth out vec2 vertexTexCoordinates;

const uint POWER_TWO_TAB[13] = uint[](1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096);

void main() {
    for (int layerIndex = 0; layerIndex < NUMBER_LAYER; layerIndex++) {
        if ((uint(layer.layersToUpdate) & POWER_TWO_TAB[layerIndex]) != uint(0)) {
            gl_Layer = layerIndex;

            gl_Position = gl_in[0].gl_Position;
            vertexTexCoordinates = texCoordinates[0];
            EmitVertex();
            gl_Position = gl_in[1].gl_Position;
            vertexTexCoordinates = texCoordinates[1];
            EmitVertex();
            gl_Position = gl_in[2].gl_Position;
            vertexTexCoordinates = texCoordinates[2];
            EmitVertex();

            EndPrimitive();
        }
    }
}

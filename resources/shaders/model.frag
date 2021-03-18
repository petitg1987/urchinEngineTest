#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform float ambientFactor;
uniform sampler2D diffuseTex; //binding 20
uniform sampler2D normalTex; //binding 21

in vec3 t, b, n;
in vec2 textCoordinates;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragNormalAndAmbient;

void main() {
    //diffuse
    fragColor = texture2D(diffuseTex, textCoordinates);

    //normal and ambient factor
    mat3 tbnMatrix = mat3(normalize(t), normalize(b), normalize(n));
    vec3 texNormal = normalize(vec3(texture2D(normalTex, textCoordinates)) * 2.0 - 1.0);
    vec3 normal = ((tbnMatrix * texNormal) + 1.0) / 2.0;
    fragNormalAndAmbient = vec4(normal, ambientFactor);
}

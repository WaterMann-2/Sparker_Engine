#version 450

layout(binding = 0) UniformBufferObject{
    mat4 model;
    mat4 view;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 texCoords;
layout(location = 3) in vec3 color;

layout(location = 0) out vec3 fragColor;

void main(){
    gl_Position = ubo.model * ubo.view * vec4(inPosition, 1.0);
    fragColor = color;
}
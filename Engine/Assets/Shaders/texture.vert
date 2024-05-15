#version 460 core

//layout (location = 0) in vec3 aPosition;
//layout (location = 1) in vec3 aColor;
//layout (location = 2) in vec2 aTexCoord;
//
//out vec3 ourColor;
//out vec2 texCoord;
//
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
//
//void main() {
//	gl_Position = projection * view * model * vec4(aPosition, 1.0);
//    ourColor = aColor;
//    texCoord = aTexCoord;
//}

// SKINNED MESH
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout(location = 3) in ivec4 aBoneIndices;
layout(location = 4) in vec4 aBoneWeights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 boneMatrices[100];

out vec3 ourColor;
out vec2 texCoord;

//void main() {
//    mat4 boneTransform = boneMatrices[aBoneIndices[0]] * aBoneWeights[0];
//    boneTransform += boneMatrices[aBoneIndices[1]] * aBoneWeights[1];
//    boneTransform += boneMatrices[aBoneIndices[2]] * aBoneWeights[2];
//    boneTransform += boneMatrices[aBoneIndices[3]] * aBoneWeights[3];
//
//    vec4 transformedPosition = boneTransform * vec4(aPosition, 1.0);
//    gl_Position = projection * view * model * transformedPosition;
//
//    ourColor = aColor;
//    texCoord = aTexCoord;
//}

void main() {
    vec4 position = vec4(aPosition, 1.0);  // Use the original vertex position without bone transformation.
    gl_Position = projection * view * model * position;

    ourColor = aColor;
    texCoord = aTexCoord;
}

//void main() {
//    mat4 boneTransform = boneMatrices[aBoneIndices[0]] * aBoneWeights[0];
//    boneTransform += boneMatrices[aBoneIndices[1]] * aBoneWeights[1];
//    boneTransform += boneMatrices[aBoneIndices[2]] * aBoneWeights[2];
//    boneTransform += boneMatrices[aBoneIndices[3]] * aBoneWeights[3];
//
//    vec4 transformedPosition = boneTransform * vec4(aPosition, 1.0);
//    gl_Position = projection * view * model * transformedPosition;
//
//    // Coloring based on bone index for debugging
//    float colorFactor = float(aBoneIndices[0] % 10) / 10.0;
//    ourColor = vec3(colorFactor, 0.0, 1.0 - colorFactor);
//    texCoord = aTexCoord;
//}
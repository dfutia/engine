#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIds;
layout (location = 4) in vec4 aBoneWeights;

// Outputs to fragment shader
out vec3 ourColor;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main() {
//    vec4 totalPosition = vec4(0.0f);
//    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
//    {
//        if(aBoneIds[i] == -1) 
//            continue;
//        if(aBoneIds[i] >=MAX_BONES) 
//        {
//            totalPosition = vec4(aPosition,1.0f);
//            break;
//        }
//        vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPosition,1.0f);
//        totalPosition += localPosition * aBoneWeights[i];
//   }
//	
//    mat4 viewModel = view * model;
//    gl_Position =  projection * viewModel * totalPosition;

    mat4 boneTransform = finalBonesMatrices[aBoneIds[0]] * aBoneWeights[0];
    boneTransform += finalBonesMatrices[aBoneIds[1]] * aBoneWeights[1];
    boneTransform += finalBonesMatrices[aBoneIds[2]] * aBoneWeights[2];
    boneTransform += finalBonesMatrices[aBoneIds[3]] * aBoneWeights[3];

    vec4 posL = model * boneTransform * vec4(aPosition, 1.0);

    gl_Position =  projection * view * posL;

    // Pass color and texture coordinate to fragment shader
    ourColor = aColor;
    texCoord = aTexCoord;
}
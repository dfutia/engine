#pragma once
#ifndef ANIMATION_CLIP_H
#define ANIMATION_CLIP_H

#include <assimp/scene.h>

#include <vector>

struct AnimationClip {
    int id;
    float duration;
    float ticksPerSecond;
    std::vector<aiNodeAnim*> channels;

    void print();
};

AnimationClip loadAnimationClip(const aiAnimation* animation);

#endif 

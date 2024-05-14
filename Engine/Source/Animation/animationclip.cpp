#include "animationclip.h"

#include <spdlog/spdlog.h>

void AnimationClip::print() {
    spdlog::info("ID {}", this->id);
    spdlog::info("Duration {}", this->duration);
    spdlog::info("TicksPerSecond {}", this->ticksPerSecond);
}

AnimationClip loadAnimationClip(const aiAnimation* animation) {
    AnimationClip clip;

    clip.id = 1;
    clip.duration = animation->mDuration;
    clip.ticksPerSecond = animation->mTicksPerSecond;

    for (unsigned int i = 0; i < animation->mNumChannels; ++i) {
        aiNodeAnim* channel = animation->mChannels[i];
        clip.channels.push_back(channel);
    }

    return clip;
}
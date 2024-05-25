#include "bone.h"
#include <util.h>

#include <assimp/scene.h>
#include <spdlog/spdlog.h>

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel) : name(name), id(id), localTransform(1.0f) {
	numPositions = channel->mNumPositionKeys;

	for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex)
	{
		aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition data;
		data.position = getGLMVec(aiPosition);
		data.timeStamp = timeStamp;
		positions.push_back(data);
	}

	numRotations = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
	{
		aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation data;
		data.orientation = getGLMQuat(aiOrientation);
		data.timeStamp = timeStamp;
		rotations.push_back(data);
	}

	numScalings = channel->mNumScalingKeys;
	for (int keyIndex = 0; keyIndex < numScalings; ++keyIndex)
	{
		aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
		float timeStamp = channel->mScalingKeys[keyIndex].mTime;
		KeyScale data;
		data.scale = getGLMVec(scale);
		data.timeStamp = timeStamp;
		scales.push_back(data);
	}
}

void Bone::update(float animationTime) {
	glm::mat4 translation = interpolatePosition(animationTime);
	glm::mat4 rotation = interpolateRotation(animationTime);
	glm::mat4 scale = interpolateScaling(animationTime);
	localTransform = translation * rotation * scale;
}

int Bone::getPositionIndex(float animationTime)
{
	for (int index = 0; index < numPositions - 1; ++index)
	{
		if (animationTime < positions[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

int Bone::getRotationIndex(float animationTime)
{
	for (int index = 0; index < numRotations - 1; ++index)
	{
		if (animationTime < rotations[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

int Bone::getScaleIndex(float animationTime)
{
	for (int index = 0; index < numScalings - 1; ++index)
	{
		if (animationTime < scales[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

glm::mat4 Bone::interpolatePosition(float animationTime)
{
	if (1 == numPositions)
		return glm::translate(glm::mat4(1.0f), positions[0].position);

	int p0Index = getPositionIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(positions[p0Index].timeStamp, positions[p1Index].timeStamp, animationTime);
	glm::vec3 finalPosition = glm::mix(positions[p0Index].position, positions[p1Index].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::interpolateRotation(float animationTime)
{
	if (1 == numRotations)
	{
		auto rotation = glm::normalize(rotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int p0Index = getRotationIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(rotations[p0Index].timeStamp,
		rotations[p1Index].timeStamp, animationTime);
	glm::quat finalRotation = glm::slerp(rotations[p0Index].orientation, rotations[p1Index].orientation
		, scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);

}

glm::mat4 Bone::interpolateScaling(float animationTime)
{
	if (1 == numScalings)
		return glm::scale(glm::mat4(1.0f), scales[0].scale);

	int p0Index = getScaleIndex(animationTime);
	int p1Index = p0Index + 1;
	float scaleFactor = getScaleFactor(scales[p0Index].timeStamp,
		scales[p1Index].timeStamp, animationTime);
	glm::vec3 finalScale = glm::mix(scales[p0Index].scale, scales[p1Index].scale
		, scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}
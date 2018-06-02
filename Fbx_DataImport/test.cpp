#include "stdafx.h"

void FBXExporter::ProcessJointsAndAnimations(FbxNode* inNode) {
	FbxMesh* currMesh = inNode->GetMesh(); unsigned int numOfDeformers = currMesh->GetDeformerCount();
	//This geometry transform is something I cannot understand
	// I think it is from MotionBuilder 
	// If you are using Maya for your models, 99% this is just an 
	// identity matrix 
	// But I am taking it into account anyways......
	FbxAMatrix geometryTransform = Utilities::GetGeometryTransformation(inNode);
	// A deformer is a FBX thing, which contains some clusters 
	// A cluster contains a link, which is basically a joint 
	// Normally, there is only one deformer in a mesh
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex) {
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin) { continue; } unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex) {
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName); FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix; FbxAMatrix globalBindposeInverseMatrix; currCluster->GetTransformMatrix(transformMatrix);
			// The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);
			// The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;
			// Update the information in mSkeleton
			mSkeleton.mJoints[currJointIndex].mGlobalBindposeInverse = globalBindposeInverseMatrix;
			mSkeleton.mJoints[currJointIndex].mNode = currCluster->GetLink();
			// Associate each joint with the control points it affects unsigned int 
			numOfIndices = currCluster->GetControlPointIndicesCount(); for (unsigned int i = 0; i < numOfIndices; ++i) {
				BlendingIndexWeightPair currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.mBlendingIndex = currJointIndex; currBlendingIndexWeightPair.mBlendingWeight = currCluster->GetControlPointWeights()[i]; mControlPoints[currCluster->GetControlPointIndices()[i]]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
			}
			// Get animation information 
			// Now only supports one take 
			FbxAnimStack* currAnimStack = mFBXScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName(); mAnimationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = mFBXScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart(); FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;
			Keyframe** currAnim = &mSkeleton.mJoints[currJointIndex].mAnimation; for (
				FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i) {
				FbxTime currTime; currTime.SetFrame(i, FbxTime::eFrames24);
				*currAnim = new Keyframe(); (*currAnim)->mFrameNum = i;
				FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
				(*currAnim)->mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				currAnim = &((*currAnim)->mNext);
			}
		}
	}
	// Some of the control points only have less than 4 joints // affecting them.
	// For a normal renderer, there are usually 4 joints 
	// I am adding more dummy joints if there isn't enough BlendingIndexWeightPair 
	currBlendingIndexWeightPair; currBlendingIndexWeightPair.mBlendingIndex = 0;
	currBlendingIndexWeightPair.mBlendingWeight = 0; for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr) {
		for (unsigned int i = itr->second->mBlendingInfo.size(); i <= 4; ++i) {
			itr->second->mBlendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}
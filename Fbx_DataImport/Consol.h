#pragma once


#include "stdafx.h"
#include <fbxsdk.h>

void GetControlPointsAndNormal(XMFLOAT3 *xmPos, XMFLOAT3 *normal, FbxMesh *lMesh)
{
	int lControlPointsCount = lMesh->GetControlPointsCount();
	FbxVector4 *lControlPoints = lMesh->GetControlPoints();

	xmPos = new XMFLOAT3[lControlPointsCount];
	normal = new XMFLOAT3[lControlPointsCount];
	cout << "==Vertex==" << endl;
	for (int i = 0; i < lControlPointsCount; ++i)
	{
		int temp = i * 3;
		FbxVector4 value = lControlPoints[i];
		xmPos[i].x = (float)value[0];
		xmPos[i].y = (float)value[1];
		xmPos[i].z = (float)value[2];
		FBXSDK_printf("%d : %f , %f , %f  /\t", i, xmPos[i].x, xmPos[i].y, xmPos[i].z);

		for (int j = 0 ; j < lMesh->GetElementNormalCount(); ++j)
		{
			FbxGeometryElementNormal *lNormal = lMesh->GetElementNormal(j);
			FbxVector4 lVec = lNormal->GetDirectArray().GetAt(i);

			normal[i].x = lVec.Buffer()[0];
			normal[i].y = lVec.Buffer()[1];
			normal[i].z = lVec.Buffer()[2];
			FBXSDK_printf("normal : %f , %f , %f\n /", normal[i].x, normal[i].y, normal[i].z);
		}
	}
}

/*
	if you input the index, you get a ControlPoint Index.
*/
void GetPolygonVertexIndex(UINT *xmIndex, FbxMesh *lMesh)
{
	int size_index = lMesh->GetPolygonVertexCount();
	xmIndex = new UINT[size_index];

	cout << endl << "==Index==" << endl;
	for (int i = 0; i < size_index; ++i)
	{
		xmIndex[i] = lMesh->GetPolygonVertices()[i];

		FBXSDK_printf("%d  \t", xmIndex[i]);
	}
}

/*
*/
void getUVCoordinate(XMFLOAT2 *xmCoord, FbxMesh *lMesh)
{
	int num = 0;
	cout << endl << "== UV Coordinate ==" << endl;

	xmCoord = new XMFLOAT2[lMesh->GetControlPointsCount()];
	for (int i = 0; i < lMesh->GetPolygonCount(); ++i)
	{
		int size_polygon = lMesh->GetPolygonSize(i);
		for (int j = 0; j < size_polygon; ++j)
		{
			int lControlPointIndex = lMesh->GetPolygonVertex(i, j);

			for (int l = 0; l < lMesh->GetElementUVCount(); ++l)
			{
				FbxGeometryElementUV* uv = lMesh->GetElementUV(l);
				FbxVector2 lVec = uv->GetDirectArray().GetAt(lControlPointIndex);

				xmCoord[num].x = lVec.Buffer()[0];
				xmCoord[num].y = lVec.Buffer()[1];
				FBXSDK_printf("%f , %f \t", xmCoord[num].x, xmCoord[num].y);

				++num;
			}
		}
	}
}

void FbxContent(FbxNode *lNode)
{
	FbxMesh *lMesh = (FbxMesh*)lNode->GetNodeAttribute();

	cout << endl << endl;
	FbxGeometry *lGeometry = (FbxGeometry*)lMesh;
	FbxCluster *lCluster;
	int lSkinCount = 0;
	int lClusterCount = 0;

	lSkinCount = lGeometry->GetDeformerCount(FbxDeformer::eSkin);

	for (int i = 0; i != lSkinCount; ++i)
	{
		lClusterCount = ((FbxSkin *)lGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();
		for (int j = 0; j != lClusterCount; ++j)
		{
			lCluster = ((FbxSkin *)lGeometry->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);

			int indexCount = lCluster->GetControlPointIndicesCount();
			int *lIndices = lCluster->GetControlPointIndices();
			double *lWeight = lCluster->GetControlPointWeights();

			cout << lCluster->GetName() << endl;
			int *link = new int[indexCount];
			double *weight = new double[indexCount];
			for (int k = 0; k < indexCount; ++k)
			{
				link[k] = lIndices[k];
				weight[k] = lWeight[k];
				cout << link[k] << "  ..  " << weight[k] << endl;
			}
		}
	}
}


/*
==========================================
==========================================
*/
void Ontimer(FbxTime &m_fbxFrameTime, FbxTime &m_pfbxCurrentTimes,
	FbxTime &m_pfbxStartTimes, FbxTime &m_pfbxStopTimes)
{
	if (m_pfbxStopTimes > m_pfbxStartTimes)
	{
		m_pfbxCurrentTimes += m_fbxFrameTime;
		if (m_pfbxCurrentTimes > m_pfbxStopTimes)
			m_pfbxCurrentTimes = m_pfbxStartTimes;
	}
}


void GetSkinCluster(FbxMesh *pfbxMesh, FbxTime& rfbxTime, FbxVector4 *pfbxv4Vertices, int nVertices)
{
	FbxSkin *pfbxSkinDeformer = (FbxSkin *)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType nSkinningType = pfbxSkinDeformer->GetSkinningType();

	if ((nSkinningType == FbxSkin::eLinear) || (nSkinningType == FbxSkin::eRigid))
	{
		FbxAMatrix *pfbxmtxClusterDeformations = new FbxAMatrix[nVertices];
		memset(pfbxmtxClusterDeformations, 0, nVertices * sizeof(FbxAMatrix));

		double *pfClusterWeights = new double[nVertices];
		memset(pfClusterWeights, 0, nVertices * sizeof(double));

		FbxCluster::ELinkMode nClusterMode = ((FbxSkin *)pfbxMesh->GetDeformer(0, FbxDeformer::eSkin))->GetCluster(0)->GetLinkMode();

		int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int i = 0; i < nSkinDeformers; i++)
		{
			FbxSkin *pfbxSkinDeformer = (FbxSkin *)pfbxMesh->GetDeformer(i, FbxDeformer::eSkin);
			int nClusters = pfbxSkinDeformer->GetClusterCount();

			for (int j = 0; j < nClusters; j++)
			{
				FbxCluster *pfbxCluster = pfbxSkinDeformer->GetCluster(j);
				if (!pfbxCluster->GetLink()) continue;

				FbxAMatrix fbxmtxClusterDeformation;
				
				/////////////////////////////////////////////////////////////////////////////
				FbxCluster::ELinkMode nClusterMode = pfbxCluster->GetLinkMode();
				if ((nClusterMode == FbxCluster::eNormalize) || (nClusterMode == FbxCluster::eTotalOne))
				{
					FbxAMatrix fbxmtxGeometryOffset = GetAttributeNodeTransform(pfbxMesh->GetNode());

					FbxAMatrix fbxmtxClusterTransform;
					pfbxCluster->GetTransformMatrix(fbxmtxClusterTransform);

					FbxAMatrix fbxmtxClusterLinkTransform;
					pfbxCluster->GetTransformLinkMatrix(fbxmtxClusterLinkTransform);

					FbxAMatrix fbxmtxLinkNodeGlobal = pfbxCluster->GetLink()->EvaluateGlobalTransform(rfbxTime);

					fbxmtxClusterDeformation = fbxmtxLinkNodeGlobal * fbxmtxClusterLinkTransform.Inverse() * fbxmtxClusterTransform * fbxmtxGeometryOffset;
				}
				/////////////////////////////////////////////////////////////////////////////

				int *pnIndices = pfbxCluster->GetControlPointIndices();
				double *pfWeights = pfbxCluster->GetControlPointWeights();

				int nIndices = pfbxCluster->GetControlPointIndicesCount();
				for (int k = 0; k < nIndices; k++)
				{
					int nIndex = pnIndices[k];
					double fWeight = pfWeights[k];
					if ((nIndex >= nVertices) || (fWeight == 0.0)) continue;

					FbxAMatrix fbxmtxInfluence = fbxmtxClusterDeformation;
					MatrixScale(fbxmtxInfluence, fWeight);

					MatrixAdd(pfbxmtxClusterDeformations[nIndex], fbxmtxInfluence);
					pfClusterWeights[nIndex] += fWeight;
				}
			}
		}

		for (int i = 0; i < nVertices; i++)
		{
			if (pfClusterWeights[i] != 0.0)
			{
				FbxVector4 fbxv4Vertex = pfbxv4Vertices[i];
				pfbxv4Vertices[i] = pfbxmtxClusterDeformations[i].MultT(fbxv4Vertex);
				if (nClusterMode == FbxCluster::eNormalize)
				{
					pfbxv4Vertices[i] /= pfClusterWeights[i];
				}
			}
		}

		delete[] pfbxmtxClusterDeformations;
		delete[] pfClusterWeights;

		//buffer ¿¡ vertex¸¦ ³Ñ°ÜÁÜ
		/*
		if (m_ppd3dPipelineStates) pd3dCommandList->SetPipelineState(m_ppd3dPipelineStates[0]);
		pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);
		*/
	}
}

FbxAMatrix GetAttributeNodeTransform(FbxNode *pfbxNode)
{
	const FbxVector4 T = pfbxNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 R = pfbxNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 S = pfbxNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return(FbxAMatrix(T, R, S));
}


void DrawNode(FbxNode *pfbxNode, FbxTime& rfbxTime, FbxAMatrix& fbxmtxAttributeGlobalTransform, FbxAMatrix& fbxmtxRootTransform)
{
	FbxNodeAttribute *pfbxNodeAttribute = pfbxNode->GetNodeAttribute();

	if (pfbxNodeAttribute)
	{
		if (pfbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh *pfbxMesh = pfbxNode->GetMesh();
			int nVertices = pfbxMesh->GetControlPointsCount();

			if (nVertices > 0)
			{
				FbxVector4 *pfbxv4Vertices = new FbxVector4[nVertices];
				memcpy(pfbxv4Vertices, pfbxMesh->GetControlPoints(), nVertices * sizeof(FbxVector4));


				FbxAMatrix fbxmtxWorldTransform = fbxmtxRootTransform;
				int nSkinDeformers = pfbxMesh->GetDeformerCount(FbxDeformer::eSkin);
				if (nSkinDeformers > 0)
				{
					int nClusters = 0;
					for (int i = 0; i < nSkinDeformers; ++i)
						nClusters += ((FbxSkin *)(pfbxMesh->GetDeformer(i, FbxDeformer::eSkin)))->GetClusterCount();
					if (nClusters > 0)
						GetSkinCluster(pfbxMesh, rfbxTime, pfbxv4Vertices, nVertices);
				}
				else
					fbxmtxWorldTransform *= fbxmtxAttributeGlobalTransform;
				
			}
		}
	}
}
void FbxAnimStackDraw(FbxNode *pfbxNode, FbxTime& rfbxTime, FbxAMatrix& fbxmtxRootTransform)
{
	if (pfbxNode->GetNodeAttribute())
	{
		FbxAMatrix fbxmtxNodeGlobalTransform = pfbxNode->EvaluateGlobalTransform(rfbxTime);
		FbxAMatrix fbxmtxGeometryOffset = GetAttributeNodeTransform(pfbxNode);
		FbxAMatrix fbxmtxAttributeGlobalTransform = fbxmtxNodeGlobalTransform * fbxmtxGeometryOffset;

		DrawNode(pfbxNode, rfbxTime, fbxmtxAttributeGlobalTransform, fbxmtxRootTransform);
	}

	for (int i = 0; i < pfbxNode->GetChildCount(); ++i)
	{
		cout << endl << "Child : " << i << "=====" << endl;
		FbxContent(pfbxNode->GetChild(i));
		cout << endl << endl;
	}
}





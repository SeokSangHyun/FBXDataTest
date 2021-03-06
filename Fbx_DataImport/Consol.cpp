
#include "Consol.h"


/*
	Return a string-based representation based on the attribute type.
*/
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}



//GeoMerty;;
/*
==애니메이션에 가져와야하는 정보들==
KeyTime : 프레임 시간
KeyMatrix : 행렬()

Vertex Matrix
-translate
-rotate
-scale
-
Index

BONE HIERARCHY

Offset Matrix
-
-
-
-

AnimationClip
-
*/
using namespace std;

int main()
{

	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;
	bool lResult;

	// FBX SDK 준비
	lSdkManager = lSdkManager->Create();

	// IOSetting Object 생성. 이 오브젝트는 모든 import/export 세팅
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	FbxString lPath = FbxGetApplicationDirectory();
	lSdkManager->LoadPluginsDirectory(lPath.Buffer());

	lScene = FbxScene::Create(lSdkManager, "My Scene");

	FbxString lFilePath("Allosaurus_Default.fbx");
	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(lFilePath.Buffer(), -1, lSdkManager->GetIOSettings());
	lImporter->Import(lScene);

	FbxNode *lNode = lScene->GetRootNode();
	
	FbxTime time = 0;
	FbxAMatrix world;
	FbxAnimStackDraw(lNode, time, world);


	FbxAMatrix world;
	world.SetIdentity();
	FbxTime time = 0;
	FbxAnimStackDraw(lNode,time, world);
}


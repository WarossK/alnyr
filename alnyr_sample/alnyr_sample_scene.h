#pragma once

#include <alnyr_src/alnyr_scene.h>
#include <alnyr_src/alnyr_math.h>

class SampleScene : public alnyr::alnyrScene
{
public:
	SampleScene() :
		alnyrScene(new alnyr::alnyrResourceGroup)
	{
		alnyr::alnyrStaticMesh static_mesh;
		std::vector<Vector3> vertices
		{
			Vector3(-1.0f, 1.0f, 0.0f),
			Vector3(1.0f, 1.0f, 0.0f),
			Vector3(-1.0f, -1.0f, 0.0f),
			Vector3(1.0f, -1.0f, 0.0f)
		};
		static_mesh.Initialize(vertices);

		GetResourceGroup()->AddMeshResource(std::move(static_mesh));
	}

	void SceneInitialize(){}
	void SceneUninitialize(){}
};
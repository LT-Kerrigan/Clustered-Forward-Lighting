#pragma once

#include "../SettingsType/GSetting.h"

#include "../../Rendering/View/Camera.h"
#include "../../ASSIMP/ModelMesh.h"

#include <vector>

class GBuffer :
	public GSetting
{
public:
	GBuffer(Camera* camera, std::vector<ModelMesh*>* modelsInFrame);
	virtual ~GBuffer();

	void LinkShaders();
	void RegenerateShaders();

	void Initialise();
	void Apply();

	GBufferData* GetGBuffer() 
	{
		return SGBuffer;
	}

private:
	void LocateUniforms() {}
	void InitGBuffer();
	void InitAttachments();

	void FillGBuffer();

	GLuint gBuffer;
	GLuint gPosition;
	GLuint gNormal;
	GLuint gAlbedo; 
	GLuint attachments[3] =
	{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	GLuint rboDepth;

	Shader* geometryPass;

	Camera* camera;
	std::vector<ModelMesh*>* modelsInFrame;
	GBufferData* SGBuffer;
};


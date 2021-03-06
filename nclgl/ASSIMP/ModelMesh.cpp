#include "ModelMesh.h"

ModelMesh::ModelMesh(vector<Vertex> vertices, vector<unsigned int> indices,
	vector<Texture> textures, vector<Texture> heights, BoundingBox AABB)
{
	this->vertices	= vertices;
	this->indices	= indices;
	this->textures	= textures;
	this->heights	= heights;

	box = AABB;
	boundingRadius		= 1.0f;
	distanceFromCamera	= 0.0f;

	SetupMesh();

	transform.SetPositionVector(Vector3(0, 0, 0));
	transform.SetScalingVector(Vector3(1, 1, 1));
}

void ModelMesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices.at(0), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

	//Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	//Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	//Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	//Vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	//Vertex tangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

void ModelMesh::Draw(Shader& shader)
{
	//Bind all textures of the mesh
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		//Activate proper texture unit before binding
		glActiveTexture(GL_TEXTURE0 + i);
		glUniform1i(glGetUniformLocation(shader.GetProgram(), textures[i].type.c_str()), i);

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(),
		"modelMatrix"), 1, false, (float*)&transform);

	//Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void ModelMesh::DrawShadow(Shader& shader)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.GetProgram(),
		"modelMatrix"), 1, false, (float*)&transform);

	//Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
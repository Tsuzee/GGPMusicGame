//Darren Farr GGP
#pragma once

#include "DXCore.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "Vertex.h"
#include <fstream>
#include <vector>

class Mesh
{

public:
	Mesh();
	Mesh(char*, ID3D11Device*, bool);
	Mesh(Vertex*, int, UINT*, int, ID3D11Device*);
	~Mesh();

	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	unsigned int GetIndexCount();

	std::vector<Vertex> GetVertexCollection();
	std::vector<UINT> GetIndexCollection();

	void CalculateTangents(Vertex*, int, unsigned int*, int);

private:
	ID3D11Buffer* vertBuffer;
	ID3D11Buffer* indBuffer;
	unsigned int indCount;

	bool hasNormalMap;

	std::vector<Vertex> vertexCollection;
	std::vector<UINT> indexCollection;

	void CreateGeometry(Vertex*, int, UINT*, ID3D11Device*);
};

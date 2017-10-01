#include "Collider.h"
#include <iostream>

Collider::Collider()
{
	
}

Collider::Collider(Mesh* _mesh) : mesh(_mesh)
{
	boundingSphere = 0.0f;

	std::vector<Vertex> meshVertexCollection = mesh->GetVertexCollection();

	CreateBoundingVolumes(meshVertexCollection, boundingSphere);
}

Collider::~Collider()
{}

void Collider::CreateBoundingVolumes(std::vector<Vertex> &vertPosArray,	float &boundingSphere)
{
	XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (UINT i = 0; i < vertPosArray.size(); i++)
	{
		// The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
		// that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
		// covered by the bounding volume

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, vertPosArray[i].Position.x);    // Find smallest x value in model
		minVertex.y = min(minVertex.y, vertPosArray[i].Position.y);    // Find smallest y value in model
		minVertex.z = min(minVertex.z, vertPosArray[i].Position.z);    // Find smallest z value in model

																	   //Get the largest vertex 
		maxVertex.x = max(maxVertex.x, vertPosArray[i].Position.x);    // Find largest x value in model
		maxVertex.y = max(maxVertex.y, vertPosArray[i].Position.y);    // Find largest y value in model
		maxVertex.z = max(maxVertex.z, vertPosArray[i].Position.z);    // Find largest z value in model
	}

	// Compute distance between maxVertex and minVertex
	float distX = (maxVertex.x - minVertex.x) / 2.0f;
	float distY = (maxVertex.y - minVertex.y) / 2.0f;
	float distZ = (maxVertex.z - minVertex.z) / 2.0f;

	// Compute bounding sphere (distance between min and max bounding box vertices)
	// boundingSphere = sqrt(distX*distX + distY*distY + distZ*distZ) / 2.0f;
	boundingSphere = XMVectorGetX(XMVector3Length(XMVectorSet(distX, distY, distZ, 0.0f)));
}

float Collider::GetBoudingSphere()
{
	return boundingSphere;
}
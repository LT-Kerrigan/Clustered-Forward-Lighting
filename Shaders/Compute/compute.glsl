#version 430 core

const int numTiles = 1000;
const int numLights = 100;

uniform int numZTiles;
uniform int numLightsInFrustum;

layout(local_size_x = 10, local_size_y = 10, local_size_z = 10) in;

struct Tile
{
	float x;
	float y;
	float z;
	float width;
	float height;
	float length;

	float _padding[6];
};

//faces - xyz is normal, w is distance from origin
struct CubePlanes
{
	vec4 faces[6];
	vec4 positions[6];
};

layout (std430, binding = 3) buffer TileLightsBuffer
{
	int lightIndexes[numTiles];
	int tileLights[numTiles][numLights];
};

layout(std430, binding = 4) buffer CubePlanesBuffer
{
	CubePlanes cubePlanes[];
};

layout(std430, binding = 5) buffer ScreenSpaceDataBuffer
{
	float indexes[100];
	//float padding[9];
	vec4 numLightsIn;

	vec4 data[];
};

layout(binding = 0) uniform atomic_uint count;

bool SphereInPlane(vec4 plane, vec4 light)
{
	if (dot(light.xyz, plane.xyz) + plane.w <= -light.w)
	{
		return false;
	}

	return true;
}

bool SphereInside(CubePlanes cube, vec4 light)
{
	for (int i = 0; i < 6; i++)
	{
		if (!SphereInPlane(cube.faces[i], light))
		{
			return false;
		}
	}

	return true;
}

bool SphereIntersecting(CubePlanes cube, vec4 light)
{
	float distSqr = light.w * light.w;

	if (light.x < cube.positions[0].x)
	{
		distSqr -= pow(light.x - cube.positions[0].x, 2);
	}
	else if (light.x > cube.positions[1].x)
	{
		distSqr -= pow(light.x - cube.positions[1].x, 2);
	}

	if (light.y < cube.positions[5].y)
	{
		distSqr -= pow(light.y - cube.positions[5].y, 2);
	}
	else if (light.y > cube.positions[4].y)
	{
		distSqr -= pow(light.y - cube.positions[4].y, 2);
	}

	if (light.z < cube.positions[2].z)
	{
		distSqr -= pow(light.z - cube.positions[2].z, 2);
	}
	else if (light.z > cube.positions[3].z)
	{
		distSqr -= pow(light.z - cube.positions[3].z, 2);
	}

	return distSqr > 0;
}

bool SphereColliding(CubePlanes cube, vec4 light)
{
	return (SphereInside(cube, light) || SphereIntersecting(cube, light));
}

void main()
{
	uint index = gl_GlobalInvocationID.x + numZTiles *
		(gl_GlobalInvocationID.y + numZTiles * gl_GlobalInvocationID.z);

	int intersections = 0;

	uint c = atomicCounter(count);
	for (int i = 0; i < c; i++)
	{
		bool colliding = SphereColliding(cubePlanes[index], data[i]);

		if (colliding)
		{
			tileLights[index][intersections] = int(indexes[i]);
			intersections++;
		}
	}

	lightIndexes[index] = intersections;
}


#pragma once
#include <vector>
#include <string>
#include <iostream>

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>

struct Triangle {
	Eigen::Vector3d v[3];
};

struct Mesh {
	std::vector<std::vector<float>> vetex;
	std::vector<std::vector<int>> face;
	
	std::vector<Triangle> triangles;
	// for visualization
	Eigen::Vector3d centroid;
	Eigen::Vector3d radius;

	int vetexNum = 0;
	int faceNum = 0;
	void readMesh(const std::string & file);
	void writeMesh(const std::string & file);
};

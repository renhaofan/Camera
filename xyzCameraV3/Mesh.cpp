#include "Mesh.hpp"

#include <cassert>
#include <sstream>
#include <fstream>

void Mesh::readMesh(const std::string & file) {
	std::ifstream infile;
	infile.open(file);
	assert(infile.is_open());

	float xx = 0.f, yy = 0.f, zz = 0.f; // for centroid;


	std::string line;
	while (getline(infile, line)) {
		if (line[0] == '#') {
			continue;
		}
		std::istringstream ss(line);
		char type;
		float v[3]; 
		int f[3];
		if (line[0] == 'v') {
			ss >> type >> v[0] >> v[1] >> v[2];
			this->vetex.push_back(std::vector<float>(v, v + 3));
			xx += v[0]; yy += v[1]; zz += v[2];
			// cout << v[0] << " " <<  v[1] << " " <<  v[2] << endl; 
		}
		else if (line[0] == 'f') {
			ss >> type >> f[0] >> f[1] >> f[2];
			this->face.push_back(std::vector<int>(f, f + 3));
			// cout << f[0] << " " <<  f[1] << " " <<  f[2] << endl; 
		}
	}
	infile.close();

	Triangle t;
	for (size_t i = 0; i < this->face.size(); i++) {
		auto faceId = this->face[i];
		auto vetexId1 = faceId[0] - 1;
		auto vetexId2 = faceId[1] - 1;
		auto vetexId3 = faceId[2] - 1;
		t.v[0] = Eigen::Vector3d(this->vetex[vetexId1][0], this->vetex[vetexId1][1], this->vetex[vetexId1][2]);
		t.v[1] = Eigen::Vector3d(this->vetex[vetexId2][0], this->vetex[vetexId2][1], this->vetex[vetexId2][2]);
		t.v[2] = Eigen::Vector3d(this->vetex[vetexId3][0], this->vetex[vetexId3][1], this->vetex[vetexId3][2]);
		this->triangles.push_back(t);
	}

	// std::cout << "read mesh successful" << std::endl;
	
	this->faceNum = this->face.size();
	this->vetexNum = this->vetex.size();
	this->centroid = Eigen::Vector3d(xx / this->vetexNum, yy / this->vetexNum, zz / this->vetexNum);
	

	Eigen::Vector3d flag = this->centroid - t.v[0];
	double distance = flag.norm();

	for (Triangle t : this->triangles) {
		for (int i = 0; i < 3; i++) {
			flag = this->centroid - t.v[i];
			if (flag.norm() > distance) {
				distance = flag.norm();
			}
		}
	}
	this->radius = Eigen::Vector3d(distance, distance, distance);
}

void Mesh::writeMesh(const std::string & file) {
	std::ofstream outfile;
	outfile.open(file);
	assert(outfile.is_open());

	outfile << "# " << this->vetex.size() << " vertices, "
		<< this->face.size() << " face" << "\n";
	for (std::vector<float> v : vetex) {
		outfile << "v";
		outfile << " " << v[0] << " " << v[1] << " " << v[2] << "\n";
	}

	for (std::vector<int> f : face) {
		outfile << "f";
		outfile << " " << f[0] << " " << f[1] << " " << f[2] << "\n";
	}
	outfile.close();
}
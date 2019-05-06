#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include "OBJLoader.h"

OBJLoader::OBJLoader(): file(false) {
}

void OBJLoader::setFile(std::string file_path){
	if(!file) {
		this->file_path = file_path;
		file = true;
	}
}

std::string OBJLoader::getFile() {
	return file_path;
}

bool OBJLoader::parseFile(){
	std::ifstream file(file_path.c_str());
	std::string line;
	std::string flag;
	std::string v1,v2,v3,v4;
	float x,y,z;
	int vertex_index = 0;
	int vertex_normal_index = 0;
	int vertex_texture_index = 0;
	
	glm::vec3 vertex;
	FaceVertex face_vertex1,face_vertex2,face_vertex3,face_vertex4;
	
	if(!file.is_open()){
		return false;
	}
	while(std::getline(file,line)) {
		flag = line.substr(0,2);
		if(flag == "# ") {
			std::istringstream data(line.substr(2));
			//std::cout << "Comment: " << data.str() << "\n";
		}
		if(flag == "g "){
    		std::istringstream data(line.substr(2));
    		data >> x >> y >> z;
    		//std::cout << "Group found: " << data.str() << "\n";
    	}
    	if(flag == "v "){
    		std::istringstream data(line.substr(2));
    		data >> vertex.x >> vertex.y >> vertex.z;
    		//std::cout << "Vertex " << vertex_index++ << " found: " << vertex.x << "," << vertex.y << "," << vertex.z << "\n";
    		vertices.push_back(vertex);
    	}
    	flag = line.substr(0,3);
    	if(flag == "vn "){
    		std::istringstream data(line.substr(3));
    		data >> x >> y >> z;
    		//std::cout << "Normal vertex " << vertex_normal_index++ << " found: " << x << "," << y << "," << z << "\n";
    	}
    	flag = line.substr(0,2);
    	if(flag == "f "){
    		std::istringstream data(line.substr(2));
    		data >> v1 >> v2 >> v3 >> v4;
    		//std::cout << "Face found: " << v1 << "," << v2 << "," << v3 << "," << v4 << "\n";
			face_vertex1 = getFaceVertex(v1);
			//std::cout << "\t1. Pos: " << --face_vertex1.position << "\tTex: " << face_vertex1.texture << "\tNor: " << face_vertex1.normal << "\n";
			//std::cout << "\t1. Pos: " << face_vertex1.position << "\tTex: " << face_vertex1.texture << "\tNor: " << face_vertex1.normal << "\n";
			face_vertex2 = getFaceVertex(v2);
			//std::cout << "\t2. Pos: " << --face_vertex2.position << "\tTex: " << face_vertex2.texture << "\tNor: " << face_vertex2.normal << "\n";
			//std::cout << "\t2. Pos: " << face_vertex2.position << "\tTex: " << face_vertex2.texture << "\tNor: " << face_vertex2.normal << "\n";
			face_vertex3 = getFaceVertex(v3);
			//std::cout << "\t3. Pos: " << --face_vertex3.position << "\tTex: " << face_vertex3.texture << "\tNor: " << face_vertex3.normal << "\n";
			//std::cout << "\t3. Pos: " << face_vertex3.position << "\tTex: " << face_vertex3.texture << "\tNor: " << face_vertex3.normal << "\n";
			face_vertex4 = getFaceVertex(v4);
			//std::cout << "\t4. Pos: " << --face_vertex4.position << "\tTex: " << face_vertex4.texture << "\tNor: " << face_vertex4.normal << "\n";
			//std::cout << "\t4. Pos: " << face_vertex4.position << "\tTex: " << face_vertex4.texture << "\tNor: " << face_vertex4.normal << "\n";
			--face_vertex1.position;
			--face_vertex2.position;
			--face_vertex3.position;
			--face_vertex4.position;
			if (face_vertex4.position != -1 ) {
				faces.push_back(face_vertex1.position);
				faces.push_back(face_vertex2.position);
				faces.push_back(face_vertex4.position);
				faces.push_back(face_vertex2.position);
				faces.push_back(face_vertex3.position);
				faces.push_back(face_vertex4.position);
			}
			else {
				faces.push_back(face_vertex1.position);
				faces.push_back(face_vertex2.position);
				faces.push_back(face_vertex3.position);
			}
    	}
    	flag = line.substr(0,3);
    	if(flag == "vt "){
    		std::istringstream data(line.substr(3));
    		data >> x >> y >> z;
    		//std::cout << "Texture vertex " << vertex_texture_index++ << " found: " << x << "," << y << "," << z << "\n";
    	}
    }
	file.close();
	return true;
}

FaceVertex OBJLoader::getFaceVertex(std::string data){
	FaceVertex value;
	bool position = false;
	bool texture = false;
	bool normal = false;
	std::string buffer_position = "";
	std::string buffer_texture = "";
	std::string buffer_normal = "";
	int index = 0;
	char c = data[index];
	while(c != '\0') {
		if(isdigit(c) && !position) {
			buffer_position += c;
			c = data[++index];
		}
		else if(c == '/' && !position){
			position = true;

			c = data[++index];
		}
		else if(isdigit(c) && position && !texture) {
			buffer_texture += c;
			c = data[++index];
		}
		else if(c == '/' && position && !texture){
			texture = true;
			c = data[++index];
		}
		else if(isdigit(c) && position && texture && !normal) {
			buffer_normal += c;
			c = data[++index];
		}
	}
	value.position = atoi(buffer_position.c_str());
	value.texture = atoi(buffer_texture.c_str());
	value.normal = atoi(buffer_normal.c_str());
	return value;
}

std::vector<glm::vec3> OBJLoader::getVertices() {
	return vertices;
}

std::vector<unsigned int> OBJLoader::getFaces(){
	return faces;
}

float* OBJLoader::getVertexArray(){
	vertexArray = new float[vertices.size()*3];
	for(int i = 0; i < vertices.size(); i++) {
		vertexArray[3*i] = vertices[i].x;
		vertexArray[3*i+1] = vertices[i].y;
		vertexArray[3*i+2] = vertices[i].z;
	}
	return vertexArray;
}

unsigned int* OBJLoader::getIndexArray(){
	return &faces[0];
}
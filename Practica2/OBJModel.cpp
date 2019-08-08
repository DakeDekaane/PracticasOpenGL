#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstdlib>
#include "OBJModel.h"

OBJModel::OBJModel(): fileSet(false), mtlExists(false) {
}

OBJModel::OBJModel(std::string file_path) : file_path(file_path), fileSet(true) ,mtlExists(false) {
}

bool OBJModel::setFile(std::string file_path){
	//Specified file already loaded (validate so we don't load the same data again)
	if (fileSet && this->file_path == file_path) {
		return true;
	}

	//If new file, validate it
	std::ifstream file(file_path.c_str());
	bool success = !file.fail();
	
	//Clearing data for new file
	position_vertices.clear();
	normal_vertices.clear();
	texture_vertices.clear();
	for (auto& it : meshes) {
		it->faces_indices.clear();
	}
	meshes.clear();
	mtl_file = "";
	mtlExists = false;

	//Set the file path
	this->file_path = file_path;
	if (!fileSet && success) {
		fileSet = true;
	}
	return success;
}

bool OBJModel::parseFile(){
	//File has not been set
	if (!fileSet) {
		std::cout << "File has not been set." << std::endl;
		return false;
	}

	//Needed variables
	std::ifstream file(file_path.c_str());
	std::string line;
	std::string flag;
	std::vector<std::string> vertices_strings;
	bool active_mtl = false;
	FaceVertex tmp_face_1, tmp_face_2, tmp_face_3;
	std::string tmp_string_1, tmp_string_2, tmp_string_3, tmp_string_ex;
	glm::vec3 position_v, normal_v, texture_v;
	int vertex_index = 0;
	int vertex_normal_index = 0;
	int vertex_texture_index = 0;
	
	//Actual parsing
	if(file.fail()) {
		std::cout << "File " + file_path + " could not be opened." << std::endl;
		return false;
	}
	while(std::getline(file,line)) {
		flag = line.substr(0, 7);
		if (flag == "mtllib ") {
			std::istringstream data(line.substr(7));
			data >> mtl_file;
			mtl_file = "../models/" + mtl_file;
			mtlExists = true;
		}
		flag = line.substr(0,2);
		if(flag == "# ") {
			std::istringstream data(line.substr(2));
			//std::cout << "Comment: " << data.str() << "\n";
		}
		if(flag == "g "){
    		std::istringstream data(line.substr(2));
    		//data >> x >> y >> z;
    		//std::cout << "Group found: " << data.str() << "\n";
    	}
    	if(flag == "v "){
    		std::istringstream data(line.substr(2));
    		data >> position_v.x >> position_v.y >> position_v.z;
    		//std::cout << "Vertex " << vertex_index++ << " found: " << vertex.x << "," << vertex.y << "," << vertex.z << "\n";
			position_vertices.push_back(position_v);
    	}
    	flag = line.substr(0,3);
    	if(flag == "vn "){
    		std::istringstream data(line.substr(3));
    		data >> normal_v.x >> normal_v.y >> normal_v.z;
    		//std::cout << "Normal vertex " << vertex_normal_index++ << " found: " << x << "," << y << "," << z << "\n";
			normal_vertices.push_back(normal_v);
    	}
		if (flag == "vt ") {
			std::istringstream data(line.substr(3));
			data >> texture_v.x >> texture_v.y >> texture_v.z;
			//std::cout << "Normal vertex " << vertex_normal_index++ << " found: " << x << "," << y << "," << z << "\n";
			texture_vertices.push_back(texture_v);
		}
		flag = line.substr(0, 7);
		if (flag == "usemtl ") {
			active_mtl = true;
			meshes.push_back(std::make_shared<Mesh>());
			std::istringstream data(line.substr(7));
			std::cout << "Active material: " + data.str() << std::endl;
			data >> meshes.back()->material;
			//meshes.back()->material = active_mtl;
		}
    	flag = line.substr(0,2);
    	if(flag == "f "){
			if (!active_mtl) {
				std::cout << "Mesh with no material\n";
				active_mtl = true;
				meshes.push_back(std::make_shared<Mesh>());
			}
			std::istringstream data(line.substr(2));
			//Considering non-triangular faces:
			//Getting the first triangle
			data >> tmp_string_1 >> tmp_string_2 >> tmp_string_3;
    		//std::cout << "Face found: " << v1 << "," << v2 << "," << v3 << "," << v4 << "\n";
			tmp_face_1 = getFaceVertex(tmp_string_1);
			//std::cout << "\t1. Pos: " << --face_vertex1.position << "\tTex: " << face_vertex1.texture << "\tNor: " << face_vertex1.normal << "\n";
			//std::cout << "\t1. Pos: " << face_vertex1.position << "\tTex: " << face_vertex1.texture << "\tNor: " << face_vertex1.normal << "\n";
			tmp_face_2 = getFaceVertex(tmp_string_2);
			//std::cout << "\t2. Pos: " << --face_vertex2.position << "\tTex: " << face_vertex2.texture << "\tNor: " << face_vertex2.normal << "\n";
			//std::cout << "\t2. Pos: " << face_vertex2.position << "\tTex: " << face_vertex2.texture << "\tNor: " << face_vertex2.normal << "\n";
			tmp_face_3 = getFaceVertex(tmp_string_3);
			//std::cout << "\t3. Pos: " << --face_vertex3.position << "\tTex: " << face_vertex3.texture << "\tNor: " << face_vertex3.normal << "\n";
			//std::cout << "\t3. Pos: " << face_vertex3.position << "\tTex: " << face_vertex3.texture << "\tNor: " << face_vertex3.normal << "\n";
			
			//Adjusting indices and pushing into indices array for EBO
			--tmp_face_1.position;
			--tmp_face_2.position;
			--tmp_face_3.position;
			
			meshes.back()->faces_indices.push_back(tmp_face_1.position);
			meshes.back()->faces_indices.push_back(tmp_face_2.position);
			meshes.back()->faces_indices.push_back(tmp_face_3.position);

			//Getting the rest of vertices, implementing a triangle fan, and pushing the rest of indices
			while (data >> tmp_string_ex) {
				tmp_face_2 = tmp_face_3;
				tmp_face_3 = getFaceVertex(tmp_string_ex);
				--tmp_face_3.position;
				meshes.back()->faces_indices.push_back(tmp_face_1.position);
				meshes.back()->faces_indices.push_back(tmp_face_2.position);
				meshes.back()->faces_indices.push_back(tmp_face_3.position);
			}

		}
    }
	file.close();
	return true;
}

FaceVertex OBJModel::getFaceVertex(std::string data){
	FaceVertex value;
	bool position = false;
	bool texture = false;
	bool normal = false;
	std::string buffer_position = "";
	std::string buffer_texture = "";
	std::string buffer_normal = "";
	int index = 0;
	char c = data[index];
	
	//Parsing the face vertex data to get corresponding position, normal, and texture vertices indices
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

bool OBJModel::load() {
	// First we parse the set file
	parseMtl();
	bool success = parseFile();
	if (!success) {
		return success;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	
	//Generate and bind VBO, VAO and EBO
	for (auto& it : meshes) {
		glGenBuffers(1, &(it->EBO));
	}
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, position_vertices.size() * sizeof(glm::vec3), &position_vertices[0], GL_STATIC_DRAW);
	for (auto& it : meshes) {
		
	}
	// Position attribute in VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glBindVertexArray(0);
	
	return success;
}

void OBJModel::render() {
	//Render VAO
	glBindVertexArray(VAO);
	unsigned int total_data = 0;
	for (auto& it : meshes) {
		std::cout << "Rendering " << it->material << std::endl;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, it->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, it->faces_indices.size() * sizeof(unsigned int), &((it->faces_indices)[0]), GL_STATIC_DRAW);
		glDrawElements(GL_TRIANGLES, it->faces_indices.size(), GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);
}

OBJModel::~OBJModel() {	
	// Deallocate resources (VAO,VBO)
	//for (auto& it : meshes) {
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	//}
}

bool OBJModel::parseMtl() {
	if (!mtlExists) {
		std::cout << "No mtl file to load." << std::endl;
		return false;
	}
	std::cout << "Mtl File: " + mtl_file << "\n";
	//Needed variables
	std::ifstream file(mtl_file.c_str());
	std::string line;
	std::string flag;
	std::string tmp;
	std::string tmp_name;
	float Ns;
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	glm::vec3 Ke;
	float Ni;
	float d;
	float illum;
	bool newmtl;

	if (file.fail()) {
		std::cout << "Mtl file " + mtl_file + " could not be opened." << std::endl;
		return false;
	}

	while (std::getline(file, line)) {
		flag = line.substr(0, 7);
		if (flag == "newmtl ") {
			std::istringstream data(line.substr(7));
			data >> tmp_name;
			std::cout << "New material found: " + tmp_name << std::endl;
			
			std::getline(file, line);
			flag = line.substr(0, 3);
			if (flag == "Ns ") {
				std::istringstream data(line.substr(3));
				data >> Ns;
				std::cout << "Ns: " << Ns << "\n";
			}
			std::getline(file, line);
			flag = line.substr(0, 3);
			if (flag == "Ka ") {
				std::istringstream data(line.substr(3));
				std::cout << "Ka: " << data.str() << "\n";
				data >> Ka.x >> Ka.y >> Ka.z;
			}
			std::getline(file, line);
			flag = line.substr(0, 3);
			if (flag == "Kd ") {
				std::istringstream data(line.substr(3));
				std::cout << "Kd: " << data.str() << "\n";
				data >> Kd.x >> Kd.y >> Ka.z;
			}
			std::getline(file, line);
			flag = line.substr(0, 3);
			if (flag == "Ks ") {
				std::istringstream data(line.substr(3));
				std::cout << "Ks: " << data.str() << "\n";
				data >> Ks.x >> Ks.y >> Ks.z;
			}
			std::getline(file, line);
			flag = line.substr(0, 3);
			if (flag == "Ke ") {
				std::istringstream data(line.substr(3));
				std::cout << "Ke: " << data.str() << "\n";
				data >> Ke.x >> Ke.y >> Ke.z;
			}
			std::getline(file, line);
			flag = line.substr(0, 3);
			if (flag == "Ni ") {
				std::istringstream data(line.substr(3));
				data >> Ni;
				std::cout << "Ni: " << Ni << "\n";
			}
			std::getline(file, line);
			flag = line.substr(0, 2);
			if (flag == "d ") {
				std::istringstream data(line.substr(2));
				data >> d;
				std::cout << "d: " << d << "\n";
			}
			std::getline(file, line);
			flag = line.substr(0, 6);
			if (flag == "illum ") {
				std::istringstream data(line.substr(6));
				data >> illum;
				std::cout << "illum: " << illum << "\n";
			}
			materials[tmp_name] = Material(Ns, Ka, Kd, Ks, Ke, Ni, d, illum);
		}
	}
	file.close();
	return true;
}
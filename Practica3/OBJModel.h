#include <vector>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

struct FaceVertex {
	unsigned int position;
	unsigned int normal;
	unsigned int texture;
};

struct Material {
	float Ns;
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	glm::vec3 Ke;
	float Ni;
	float d;
	float illum;
	Material() {}
	Material(float Ns, glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, glm::vec3 Ke, float Ni, float d, float illum):
		Ns(Ns), Ka(Ka), Kd(Kd), Ks(Ks), Ke(Ke), Ni(Ni), d(d), illum(illum) { }
};

class Mesh {
public:
	std::string material;
	std::vector<unsigned int> faces_indices;
	unsigned int EBO;

	Mesh() : material(""), faces_indices(std::vector<unsigned int>(0)){

	}
};

class OBJModel {
	bool fileSet;
	std::string file_path;
	
	std::vector<glm::vec3> position_vertices;
	std::vector<glm::vec3> normal_vertices;
	std::vector<glm::vec3> texture_vertices;

	bool mtlExists;
	std::string mtl_file;
	std::map<std::string, Material> materials;
	std::vector<std::shared_ptr<Mesh>> meshes;
	
	glm::vec3 max, min;
	
	unsigned int VAO;
	unsigned int VBO;

	FaceVertex getFaceVertex(std::string data);
	bool parseFile();
	bool parseMtl();

public:
	OBJModel();
	OBJModel(std::string file_path);
	bool setFile(std::string file_path);
	bool load();
	void render();
	~OBJModel();
};
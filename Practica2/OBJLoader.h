#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct FaceVertex {
	unsigned int position;
	unsigned int normal;
	unsigned int texture;
};

class OBJLoader {
	std::string file_path;
	std::vector<glm::vec3> position_vertices;
	std::vector<glm::vec3> normal_vertices;
	std::vector<glm::vec3> texture_vertices;
	std::vector<unsigned int> faces_indices;
	bool fileSet;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	glm::vec3 max, min;
	
	FaceVertex getFaceVertex(std::string data);
	bool parseFile();

public:
	OBJLoader();
	OBJLoader(std::string file_path);
	bool setFile(std::string file_path);
	bool load();
	void render();
	~OBJLoader();
};
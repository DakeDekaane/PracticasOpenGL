#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct FaceVertex {
	int position;
	int texture;
	int normal;
};

struct Face {
	FaceVertex vertex1;
	FaceVertex vertex2;
	FaceVertex vertex3;
	FaceVertex vertex4;
};


class OBJLoader{
private:
	std::string file_path;
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> faces;
	bool file;
	FaceVertex getFaceVertex(std::string data);
	float* vertexArray;
	unsigned int* indexArray;

public:
	OBJLoader();
	void setFile(std::string file_path);
	std::string getFile();
	bool parseFile();
	std::vector<glm::vec3> getVertices();
	std::vector<unsigned int> getFaces();
	float* getVertexArray();
	unsigned int* getIndexArray();
};
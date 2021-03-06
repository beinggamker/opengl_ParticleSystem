#include "FileLoader.h"

using namespace std;

string readFile(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);

	if (!fileStream.is_open()) {
		cerr << "Could not read file " << filePath << ". File does not exist." << endl;
		exit(1);
	}

	string line = "";
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

void loadShader(GLuint shaderID, const char* filepath) {
	string shaderString = readFile(filepath);
	const GLchar* shaderSource = shaderString.c_str();


	glShaderSource(shaderID, 1, &shaderSource, NULL);
	glCompileShader(shaderID);

	//Double check the shader compiled
	GLint status;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
	if (!status) {
		char buffer[512];
		glGetShaderInfoLog(shaderID, 512, NULL, buffer);
		printf("Shader Compile Failed. Info:\n\n%s\n", buffer);
		exit(1);
	}
}

void loadobj(const char* path, vector < float >& out_vertices,
	vector < float >& out_uvs,
	vector < float >& out_normals) {

	vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	vector<glm::vec3> temp_vertices;
	vector<glm::vec2> temp_uvs;
	vector<glm::vec3> temp_normals;

	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Cannot open the obj file !\n");
		exit(1);
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			//std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("obj file not supported by loader\n");
				exit(2);
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex.x);
		out_vertices.push_back(vertex.y);
		out_vertices.push_back(vertex.z);

		unsigned int uvIndex = uvIndices[i];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		out_uvs.push_back(uv.x);
		out_uvs.push_back(uv.y);

		unsigned int normalIndex = normalIndices[i];
		glm::vec3 normal = temp_normals[normalIndex - 1];
		out_normals.push_back(normal.x);
		out_normals.push_back(normal.y);
		out_normals.push_back(normal.z);

	}
}

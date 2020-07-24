#ifndef MESH_H_
#define MESH_H_
#include <vector>

struct Vertex {
	std::vector<Vertex*> adj;
	double x, y, z;
	void *info;
};

struct Mesh {
	int n, m;
	Vertex *vertices;
};

typedef int (*KernelFunc)(Vertex *v);

class MeshHelper {
public:
	MeshHelper();
	virtual ~MeshHelper();
	
	int Generate(const Vertex *vertices, int n, const int *edges, int m);
	int LoadMesh(const char *path);
	int Save(const char *path) const;
	int Loop(KernelFunc func, int infoSize);
	void PrintInfo() const;
	const Vertex* GetVertices() const;
	int GetNumVer() const;
	void SetInfo(int index, void *info);

private:
	Mesh *mesh;
	Vertex *auxVer;
};

#endif

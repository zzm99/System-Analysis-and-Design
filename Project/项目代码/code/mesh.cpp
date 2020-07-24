#include "mesh.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <unordered_set>

bool isEmptyChar(char ch) {
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool readTrimLine(FILE *file, char *buf) {
	static char ch;
	static int idx, res;
	idx = 0;
	while ((res = fscanf(file, "%c", &ch)) == 1) {
		if (!isEmptyChar(ch)) {
			break;
		}
	}
	if (res != 1) {
		return false;
	}
	buf[idx++] = ch;
	while (fscanf(file, "%c", &ch) == 1) {
		if (ch == '\n') {
			break;
		}
		buf[idx++] = ch;
	}
	while (isEmptyChar(buf[idx - 1])) {
		--idx;
	}
	buf[idx] = '\0';
	return true;
}

bool contains(const char *T, const char *P) {
	int lent = strlen(T);
	int lenp = strlen(P);
	for (int i = 0; i <= lent - lenp; ++i) {
		int j;
		for (j = 0; j < lenp; ++j) {
			if (T[i + j] != P[j]) {
				break;
			}
		}
		if (j == lenp) {
			return true;
		}
	}
	return false;
}

MeshHelper::MeshHelper() {
	mesh = new Mesh;
}

MeshHelper::~MeshHelper() {
	delete[] mesh->vertices;
	delete mesh;
}

int MeshHelper::Generate(const Vertex *vertices, int n, const int *edges, int m) {
	mesh->n = n;
	mesh->vertices = new Vertex[n];
	auxVer = new Vertex[n];
	for (int i = 0; i < n; ++i) {
		mesh->vertices[i] = vertices[i];
		mesh->vertices[i].adj.clear();
	}
	auto se = new std::unordered_set<int>[n];
	for (int i = 0; i < m; ++i) {
		se[edges[i * 2]].insert(edges[i * 2 + 1]);
		se[edges[i * 2 + 1]].insert(edges[i * 2]);
	}
	mesh->m = 0;
	for (int i = 0; i < n; ++i) {
		mesh->m += se[i].size();
	}
	for (int i = 0; i < n; ++i) {
		for (auto it : se[i]) {
			mesh->vertices[i].adj.push_back(mesh->vertices + it);
		}
	}
	delete[] se;
	return 0;
}

int MeshHelper::LoadMesh(const char *path) {
	FILE *file = fopen(path, "r");
	if (file == nullptr) {
		return -1;
	}
	char buf[200];
	Vertex *vertices;
	std::vector<int> edges;
	int n = -666;
	while (readTrimLine(file, buf)) {
		if (contains(buf, "nbNod")) {
			n = 0;
			int i;
			for (i = 0; buf[i]; ++i) {
				if (buf[i] >= '0' && buf[i] <= '9') {
					n = buf[i] - '0';
					break;
				}
			}
			for (++i; buf[i]; ++i) {
				if (buf[i] < '0' || buf[i] > '9') {
					break;
				}
				n = n * 10 + (buf[i] - '0');
			}
		}
		else if (contains(buf, "msh.POS =")) {
			if (n == -666) {
				return -1;
			}
			vertices = new Vertex[n];
			for (int i = 0; i < n; ++i) {
				readTrimLine(file, buf);
				sscanf(buf, "%lf %lf 0;", &vertices[i].x, &vertices[i].y);
				vertices[i].z = 0;
			}
		}
		else if (contains(buf, "LINES")) {
			int t1, t2;
			while (readTrimLine(file, buf)) {
				if (contains(buf, "];")) {
					break;
				}
				sscanf(buf, "%d %d 0", &t1, &t2);
				edges.push_back(t1 - 1);
				edges.push_back(t2 - 1);
			}
		}
		else if (contains(buf, "TRIANGLES")) {
			int t1, t2, t3;
			while (readTrimLine(file, buf)) {
				if (contains(buf, "]")) {
					break;
				}
				sscanf(buf, "%d %d %d 0", &t1, &t2, &t3);
				edges.push_back(t1 - 1);
				edges.push_back(t2 - 1);
				edges.push_back(t1 - 1);
				edges.push_back(t3 - 1);
				edges.push_back(t2 - 1);
				edges.push_back(t3 - 1);
			}
		}
	}
	int res = Generate(vertices, n, &edges[0], edges.size() / 2);
	delete[] vertices;
	fclose(file);
	return res;
}

int MeshHelper::Save(const char *path) const {
	FILE *file = fopen(path, "w");
	if (file == nullptr) {
		return -1;
	}
	fprintf(file, "%% Matlab mesh\n");
	fprintf(file, "%% Created by libmesh.so\n");
	fprintf(file, "%% ASCII\n");
	fprintf(file, "clear msh;\n");
	fprintf(file, "msh.nbNod = %d;\n", mesh->n);
	fprintf(file, "msh.POS = [\n");
	for (int i = 0; i < mesh->n; ++i) {
		fprintf(file, "%f %f 0\n", mesh->vertices[i].x, mesh->vertices[i].y);
	}
	fprintf(file, "];\n");
	fprintf(file, "msh.MAX = max(msh.POS);\n");
	fprintf(file, "msh.MIN = min(msh.POS);\n");
	fprintf(file, "msh.LINES = [\n");
	for (int i = 0; i < mesh->n; ++i) {
		for (auto v : mesh->vertices[i].adj) {
			int j = (int)(v - mesh->vertices);
			if (i < j) {
				fprintf(file, "%d %d 0\n", i + 1, j + 1);
			}
		}
	}
	fprintf(file, "];\n");

	return 0;
}

int MeshHelper::Loop(KernelFunc func, int infoSize) {
	for (int i = 0; i < mesh->n; ++i) {
		auxVer[i] = mesh->vertices[i];
		auxVer[i].info = new char[infoSize];
		memcpy(auxVer[i].info, mesh->vertices[i].info, infoSize);
	}
	int res;
	for (int i = 0; i < mesh->n; ++i) {
		res = func(auxVer + i);
		if (res != 0) {
			return res;
		}
	}
	for (int i = 0; i < mesh->n; ++i) {
		memcpy(mesh->vertices[i].info, auxVer[i].info, infoSize);
		delete (char*)auxVer[i].info;
	}

	return 0;
}

void MeshHelper::PrintInfo() const {
	printf("Number of vertices = %d\n", mesh->n);
	printf("Number of edges = %d\n", mesh->m / 2);
	printf("Vertices info:\n");
	for (int i = 0; i < mesh->n; ++i) {
		printf("%d: position : (%f, %f)", i, mesh->vertices[i].x, mesh->vertices[i].y);
		printf(" adj vertex: ");
		for (auto v : mesh->vertices[i].adj) {
			printf(" %ld", v - mesh->vertices);
		}
		printf(" info: ");
		printf(" %lf\n", *(double*)mesh->vertices[i].info);
	}
}

const Vertex* MeshHelper::GetVertices() const {
	return mesh->vertices;
}

int MeshHelper::GetNumVer() const {
	return mesh->n;
}

void MeshHelper::SetInfo(int index, void *info) {
	mesh->vertices[index].info = info;
}

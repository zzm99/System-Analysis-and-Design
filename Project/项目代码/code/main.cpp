#include "mesh.h"
#include <cstdio>

int myKernelFun(Vertex *v) {
	double sum = *(double*)v->info;
	for (auto u : v->adj) {
		sum += *(double*)u->info;
	}
	*(double*)(v->info) = sum / (v->adj.size() + 1);
	return 0;
}

int main() {
	/* TEST 1 */
	/* Example 1: Kernel Function */
	MeshHelper *mh = new MeshHelper();
	mh->LoadMesh("testmesh.m");
	const Vertex *vertices = mh->GetVertices();
	int n = mh->GetNumVer();
	for (int i = 0; i < n; ++i) {
		double *p = new double;
		*p = vertices[i].x * 100.0;
		mh->SetInfo(i, p);
	}
	
	printf("------------original mesh data---------------------\n");
	mh->PrintInfo();

	printf("------------start loop-----------------------------\n");
	int loop_num = 10;
	for(int i = 0; i < loop_num; i++) {
		mh->Loop(myKernelFun, sizeof(double));
		if(i == 5) {
			printf("---track data after loop %d----\n", i);
			mh->PrintInfo();			
			printf("------------------------------\n");
		}
	}
	printf("------------end loop-------------------------------\n");

	printf("------------after loop by kernel function----------\n");
	mh->PrintInfo();

	/* TEST 2 */
	/* Example 2: Generate New Mesh */
	MeshHelper *nh = new MeshHelper();
	Vertex *verts = new Vertex[5];
	for (int i = 0; i < 4; ++i) {
		verts[i].x = (i & 1) * 10.0;
		verts[i].y = (i >> 1 & 1) * 10.0;
	}
	verts[4].x = 5.0;
	verts[4].y = 5.0;
	int *edges = new int[16];
	int cur = 0, nex;
	for (int i = 0; i < 4; ++i) {
		nex = cur ^ ((i & 1) + 1);
		edges[i * 2] = cur;
		edges[i * 2 + 1] = nex;
		cur = nex;
	}
	for (int i = 0; i < 4; ++i) {
		edges[(i + 4) * 2] = i;
		edges[(i + 4) * 2 + 1] = 4;
	}
	nh->Generate(verts, 5, edges, 8);
	nh->Save("mymesh.m");
	
	delete[] verts;
	delete[] edges;
	
	delete mh;
	delete nh;
	return 0;
}

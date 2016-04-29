#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../General/include/flist.h"
#include "../General/include/points.h"
#include "../General/include/curve.h"
#include "../General/include/nodes.h"
#include "../General/include/plane_geo.h"
#include "../General/include/curvepoly.h"
#include "../BSpline/include/bspline.h"
#include "../General/include/fatal.h"
#include "../General/include/v.h"

#include "include/rr_grid.h"

#ifndef BSPLN_DEGREE
#define BSPLN_DEGREE 3
#endif

#ifndef TINY
#define TINY 1.0e-8
#endif
#ifndef SIGN
#define SIGN(a,b)    ( (a) >= (b) ? (1.0) : -(1.0) )
#endif
#ifndef ABS
#define ABS(a)    ( (a) >= (0) ? (a) : -(a) )
#endif
#ifndef MAX
#define MAX(a,b)    ( (a) > (b) ? (a) : (b) )
#endif
#ifndef MIN
#define MIN(a,b)    ( (a) < (b) ? (a) : (b) )
#endif

#define SHIFT_CURVE

#ifdef DEBUG_CORE
#define VPRINT(x) fprintf(stderr,"%10s = [%4f, %4f, %4f]\n",#x,x[0], x[1], x[2])
#define VPRINTF(x,f) fprintf(f,"%10s = [%4f, %4f, %4f]\n",#x,x[0], x[1], x[2])
#endif

static int getScaledRegion(struct Flist **arc, struct Point **line,
			   struct Point **lines, float s[][2]);
static int calcShapeFunction(struct Point *line,
			     struct Point *f0, struct Point *f1,
			     float x0, float x3, float y0, float y3);
static int getShapeFunctions(struct Point **lines, struct Point **f);

int MeshRR_CoreRegion(struct Nodelist *n, struct curve *ml, struct region *reg, 
					  struct region *reg0, struct region *reg1, struct region *reg3,
					  float angle14)
{
	int i, j, ii1;
	int offset, newnodes;

	int ispline;
#ifdef MODIFY_CORE
	int node, prevnode;
	float delta, ddelta;
#endif

	float u1[3], v1[3], v2[3], v3[3];
	float p[3], p1[3], p2[3], p3[3], s[2][2];
	float para, invpara, ppara;

	// lines for scaled iso-meshing, x = arc, y = l, z = param.
	struct Point *lines[4];
	struct Point *f[8];

	struct node **tmpnode = NULL;

#ifdef DEBUG_CORE
	char fn[111];
	char fngnu[111];
	FILE *fp;
	FILE *fpgnu;
	int jx, ix;
	static int fcount = 0;

	sprintf(fngnu,"rr_coregnu_%02d.txt", fcount);
	if( (fpgnu = fopen(fngnu,"w+")) == NULL) {
		fprintf(stderr,"Shit happened opening file '%s'!\n",fngnu);
		exit(-1);
	}
	sprintf(fn,"rr_debugcore_%02d.txt", fcount++);
	if( (fp = fopen(fn,"w+")) == NULL) {
		fprintf(stderr,"Shit happened opening file '%s'!\n",fn);
		exit(-1);
	}
	fprintf(fp," MeshRR_CoreRegion %d\n",fcount);
	fprintf(fpgnu,"# line no. phi*r  l  z   para\n");
#endif
	
// **************************************************
	// init
	u1[2] = v1[2] = v2[2] = v3[2] = 0.0;
	p[2]  = p1[2] = p2[2] = p3[2] = 0.0;
#ifdef MODIFY_CORE
	ddelta = 90.0/180.0*M_PI;
#endif

	ii1 = reg3->nodes[1]->num - reg->arc[2]->num;

// **************************************************
	// mem. check
	for(i = 0; i < reg->numl; i++) {
		if(reg->nodes[i]) {
			FreeIlistStruct(reg->nodes[i]);
			reg->nodes[i] = NULL;
		}
	}
	if(reg->nodes[reg->numl]) {
		FreeIlistStruct(reg->nodes[reg->numl]);
		reg->nodes[reg->numl] = NULL;
	}

	offset = n->num;
#ifdef DEBUG_CORE
	fprintf(fp," offset = %d\n", offset);
#endif

// **************************************************
	// copy existing nodes
	reg->nodes[0] = nCopynIlistStruct(reg0->nodes[3],ii1,reg0->nodes[3]->num-2*ii1);
	reg->nodes[1] = nCopynIlistStruct(reg1->nodes[2], ii1, reg3->nodes[1]->num-ii1);
 	reg->nodes[2] = nCopynIlistStruct(reg3->nodes[1],ii1, reg3->nodes[1]->num-ii1);
	reg->nodes[3] = AllocIlistStruct(reg->line[3]->nump+1);
	reg->nodes[reg->numl] = nCopynIlistStruct(reg0->nodes[3],ii1,reg0->nodes[3]->num-2*ii1);
// **************************************************
	// scale and move region, reg->numl == 4 !!!!!
	for(i = 0; i < 4; i++) lines[i] = AllocPointStruct();
	for(i = 0; i < 8; i++) f[i] = AllocPointStruct();
	getScaledRegion(reg->arc, reg->line, lines, s);
	getShapeFunctions(lines,f);

// **************************************************
	// straight lines
#ifdef DEBUG_CORE
	fprintf(fpgnu,"\n# straight lines\n");
#endif
	for(i = ispline; i < reg->line[1]->nump-1; i++) {
		para    = reg->para[1]->list[i];
		invpara = 1.0 - para;
		u1[0] = reg->arc[2]->list[i] - reg->arc[1]->list[i];
		u1[1] = reg->line[2]->y[i]   - reg->line[1]->y[i];
		p1[0] = reg->arc[1]->list[i];
		p1[1] = reg->line[1]->y[i];
		Add2Ilist(reg->nodes[reg->numl], reg->nodes[1]->list[i]);
		for(j = 1; j < reg->line[0]->nump-1; j++) {
			ppara = reg->para[0]->list[j]*invpara 
				+ reg->para[3]->list[j]*para;
			p[0] = p1[0] + ppara * u1[0];
			p[1] = p1[1] + ppara * u1[1];
			Add2Ilist(reg->nodes[reg->numl],n->num);
			AddVNode(n, p, ARC);
#ifdef DEBUG_CORE
			fprintf(fpgnu,"%3d %10.6f %10.6f %10.6f   %10.6f\n",
					i,p[0],p[1],p[2],ppara);
#endif
		}
		Add2Ilist(reg->nodes[reg->numl], reg->nodes[2]->list[i]);
#ifdef DEBUG_CORE
		fprintf(fpgnu,"\n");
#endif
	} // end i
	
// **************************************************
	// points on line 3.4
	Add2Ilist(reg->nodes[reg->numl], reg->nodes[1]->list[reg->nodes[1]->num-1]);
	for(i = 1; i < reg->line[3]->nump-1; i++) {
		p[0] = reg->arc[3]->list[i];
		p[1] = reg->line[3]->y[i];
		Add2Ilist(reg->nodes[reg->numl], n->num);
		AddVNode(n, p, ARC);		
		tmpnode++;
	}
	Add2Ilist(reg->nodes[reg->numl], reg->nodes[2]->list[reg->nodes[2]->num-1]);

	// boundary line 3.4
	Add2Ilist(reg->nodes[3], reg->nodes[1]->list[reg->nodes[1]->num-1]);
	tmpnode = n->n + n->num - reg->line[3]->nump+2;
	for(i = 1; i < reg->line[3]->nump-1; i++) {
		Add2Ilist(reg->nodes[3],(*tmpnode)->index);
		tmpnode++;
	}
	Add2Ilist(reg->nodes[3], reg->nodes[2]->list[reg->nodes[2]->num-1]);

	newnodes = n->num - offset;
	CalcNodeRadius(&n->n[n->num-newnodes], ml, newnodes);
	CalcNodeAngle(&n->n[n->num-newnodes], ml, newnodes);	

// **************************************************

#ifdef DEBUG_CORE
	fprintf(fp,"core\n");
	for(i = 0; i < reg->numl+1; i++) {
		fprintf(fp," ** reg->nodes[%d] **\n", i);
		DumpIlist2File(reg->nodes[i], fp);
	}
	fprintf(fp,"n->num - offset = %d\n",n->num-offset); 
	fclose(fp);
	fclose(fpgnu);

	sprintf(fn,"rr_corenodes_%02d.txt", fcount-1);
	if( (fp = fopen(fn,"w+")) == NULL) {
		fprintf(stderr,"Shit happened opening file '%s'!\n",fn);
		exit(-1);
	}
	fprintf(fp,"#   ID\t phi\t l\t r\t s\t\t x\t y\t z\t\n");   
	for(i = 0; i < reg->nodes[1]->num; i++) {
		ix = i * reg->nodes[0]->num;
		for(j = 0; j < reg->nodes[0]->num; j++) {
			jx = ix + j;
			fprintf(fp," %10d   %10.6f  %10.6f  %10.6f  %10.6f     %10.6f  %10.6f  %10.6f\n",
					n->n[reg->nodes[reg->numl]->list[jx]]->id, n->n[reg->nodes[reg->numl]->list[jx]]->phi, 
					n->n[reg->nodes[reg->numl]->list[jx]]->l, n->n[reg->nodes[reg->numl]->list[jx]]->r, 
					n->n[reg->nodes[reg->numl]->list[jx]]->phi*n->n[reg->nodes[reg->numl]->list[jx]]->r, 
					n->n[reg->nodes[reg->numl]->list[jx]]->x, 
					n->n[reg->nodes[reg->numl]->list[jx]]->y, n->n[reg->nodes[reg->numl]->list[jx]]->z);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n\n");
	for(i = 0; i < reg->nodes[0]->num; i++) {
		for(j = 0; j < reg->nodes[1]->num; j++) {
			jx = i + j*reg->nodes[0]->num;
			fprintf(fp," %10d   %10.6f  %10.6f  %10.6f  %10.6f     %10.6f  %10.6f  %10.6f\n",
					n->n[reg->nodes[reg->numl]->list[jx]]->id, n->n[reg->nodes[reg->numl]->list[jx]]->phi, 
					n->n[reg->nodes[reg->numl]->list[jx]]->l, n->n[reg->nodes[reg->numl]->list[jx]]->r, 
					n->n[reg->nodes[reg->numl]->list[jx]]->phi*n->n[reg->nodes[reg->numl]->list[jx]]->r, 
					n->n[reg->nodes[reg->numl]->list[jx]]->x, 
					n->n[reg->nodes[reg->numl]->list[jx]]->y, n->n[reg->nodes[reg->numl]->list[jx]]->z);
		}
		fprintf(fp,"\n");
	}
	for(i = 0; i < reg->numl; i++) {
		fprintf(fp,"\n\n");
		for(j = 0; j < reg->nodes[i]->num; j++) {
			fprintf(fp," %10d   %10.6f  %10.6f  %10.6f  %10.6f     %10.6f  %10.6f  %10.6f\n",
					n->n[reg->nodes[i]->list[j]]->id, n->n[reg->nodes[i]->list[j]]->phi, 
					n->n[reg->nodes[i]->list[j]]->l, n->n[reg->nodes[i]->list[j]]->r, 
					n->n[reg->nodes[i]->list[j]]->phi*n->n[reg->nodes[i]->list[j]]->r, 
					n->n[reg->nodes[i]->list[j]]->x, 
					n->n[reg->nodes[i]->list[j]]->y, n->n[reg->nodes[i]->list[j]]->z);
		}
	}
	fclose(fp);
#endif
	
	for(i = 0; i < 4; i++) FreePointStruct(lines[i]);
	for(i = 0; i < 8; i++) FreePointStruct(f[i]);
	return(0);
}

// **************************************************

// **************************************************
static int getScaledRegion(struct Flist **arc, struct Point **line,
			   struct Point **lines, float s[][2])
{
	int i,j, ii;

	float p[4][2], x[3], len, max[2], min[2];

#ifdef DEBUG_CORE
	static int count = 0;
	char fn[123];
	FILE *fp;
	sprintf(fn,"scaled_region_%02d.dat",count++);
	if( (fp = fopen(fn,"w+")) == NULL)
		fprintf(stderr," could not open file '%s'!\n",fn);
#endif

	// ****************************************
	// corner points
	p[0][0] = arc[0]->list[0];
	p[0][1] = line[0]->y[0];
	p[1][0] = arc[1]->list[arc[1]->num-1];
	p[1][1] = line[1]->y[line[1]->nump-1];
	
	p[2][0] = arc[2]->list[arc[2]->num-1];
	p[2][1] = line[2]->y[line[2]->nump-1];
	p[3][0] = arc[2]->list[0];
	p[3][1] = line[2]->y[0];

	// ****************************************
	// centre point and scaling factor
	s[0][0] = s[0][1] = s[1][0] = s[1][1] = 0.0;
	max[0] = max[1] = -9.e+99;
	min[0] = min[1] =  9.e+99;
	for(i = 0; i < 4; i++) {
		max[0] = MAX(max[0], p[i][0]);
		min[0] = MIN(min[0], p[i][0]);
		max[1] = MAX(max[1], p[i][1]);
		min[1] = MIN(min[1], p[i][1]);
	}
	s[0][0] = 0.5*(max[0]+min[0]);
	s[0][1] = 0.5*(max[1]+min[1]);

	s[1][0] = 0.5*(fabs(p[0][0] - p[3][0]) +
		       fabs(p[1][0] - p[2][0]));
	s[1][1] = 0.5*(fabs(p[0][1] - p[1][1]) +
		       fabs(p[3][1] - p[2][1]));
	// ****************************************
	// new lines by shifted and scaled coordinates
	x[2] = 0.0;
	for(i = 0; i < 2; i++) {
		for(j = 0; j < line[i]->nump; j++) {
			x[0] = (arc[i]->list[j]-s[0][0])/s[1][0];
			x[1] = (line[i]->y[j]-s[0][1])/s[1][1];
			AddVPoint(lines[i],x);
		}
	}
	for(i = 2, ii = 3; i < 4; i++, ii--) {
		for(j = 0; j < line[i]->nump; j++) {
			x[0] = (arc[i]->list[j]-s[0][0])/s[1][0];
			x[1] = (line[i]->y[j]-s[0][1])/s[1][1];
			AddVPoint(lines[ii],x);
		}
	}

	// parameters
	len = 0.0;
	for(i = 0; i < 4; i++) {
		for(j = 1; j < lines[i]->nump; j++) {
			len += sqrt(pow(lines[i]->x[j],2)+
				    pow(lines[i]->y[j],2));
			lines[i]->z[j] = len;
		}
		len = 1./len;
		for(j = 0; j < lines[i]->nump; j++) {
			lines[i]->z[j] *= len;
		}
	}

#ifdef DEBUG_CORE
	fprintf(fp,"# centre : %15.5f %15.5f\n# scaling: %15.5f %15.5f\n",
		s[0][0],s[0][1],s[1][0],s[1][1]);
	for(i = 0; i < 4 && fp; i++) {
		fprintf(fp,"# line no. %d\n",i);
		for(j = 0; j < lines[i]->nump; j++) {
			fprintf(fp,"%4d %15.5f %15.5f %15.5f\n",j,
				lines[i]->x[j],lines[i]->y[j],lines[i]->z[j]);
		}
		fprintf(fp,"\n\n");
	}

	if(fp) fclose(fp);
#endif
	return 0;
}

// **************************************************
static int getShapeFunctions(struct Point **lines, struct Point **f)
{
       	float x[4], y[4];

#ifdef DEBUG_CORE
	int i,j;
	static int count = 0;
	char fn[123];
	FILE *fp;
	sprintf(fn,"shape_funcs_%02d.dat",count++);
	if( (fp = fopen(fn,"w+")) == NULL)
		fprintf(stderr," could not open file '%s'!\n",fn);
#endif

	// ****************************************
	// corner points
	x[0] = lines[0]->x[0]; y[0] = lines[0]->y[0];
	x[1] = lines[2]->x[0]; y[1] = lines[2]->y[0];
	x[3] = lines[3]->x[0]; y[3] = lines[3]->y[0];
	x[2] = lines[3]->x[lines[0]->nump-1];
	y[2] = lines[3]->y[lines[0]->nump-1];

	// ****************************************

	calcShapeFunction(lines[0],f[0],f[7],x[0],x[3],y[0],y[3]);
	calcShapeFunction(lines[1],f[1],f[2],x[0],x[1],y[0],y[1]);
	calcShapeFunction(lines[2],f[3],f[4],x[1],x[2],y[1],y[2]);
	calcShapeFunction(lines[3],f[6],f[5],x[3],x[2],y[3],y[2]);


	fprintf(stderr," Absturz wegen fehlender Knoten folgt\n");


#ifdef DEBUG_CORE
	for(i = 0; i < 8 && fp; i++) {
		fprintf(fp,"# f[%d]\n",i);
		for(j = 0;j < f[i]->nump; j++) {
			fprintf(fp," %15.5f  %15.5f\n",f[i]->x[j], f[i]->y[j]);
		}
		fprintf(fp,"\n\n");
	}

	// build lines from shape functions
	
	fprintf(stderr," Shape functions abchecken mit Berandung\n");


	if(fp) fclose(fp);
#endif

	return 0;

}

static int calcShapeFunction(struct Point *line,
			     struct Point *f0, struct Point *f1,
			     float x0, float x3, float y0, float y3)

{
	int i;
	float z, a, b;

	if( (z = x3*y0 - x0*y3) == 0.0) {
		fprintf(stderr,"iso_meshcore.c: Corners are identical!\n");
		exit(-1);
	}
	for(i = 0; i < line->nump; i++) {
		a = (line->y[i]*x3-line->x[i]*y3)/ z;
		b = (line->x[i]-a*x0) / x3;
		AddPoint(f0,line->z[i],a,0.0);
		AddPoint(f1,line->z[i],b,0.0);
	}

	return 0;
}

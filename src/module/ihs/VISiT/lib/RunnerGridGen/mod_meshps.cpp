#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../General/include/flist.h"
#include "../General/include/points.h"
#include "../General/include/curve.h"
#include "../General/include/nodes.h"
#include "../General/include/fatal.h"
#include "../General/include/v.h"

#include "include/rr_grid.h"

#ifndef SMALL
#define SMALL 0.0001
#endif
#ifndef ABS
#define ABS(a)    ( (a) >= (0) ? (a) : -(a) )
#endif

#ifdef DEBUG_PS
#define VPRINT(x) fprintf(stderr,"%10s = [%4f, %4f, %4f]\n",#x,x[0], x[1], x[2])
#define VPRINTF(x,f) fprintf(f,"%10s = [%4f, %4f, %4f]\n",#x,x[0], x[1], x[2])
#endif

int MeshMod_PSRegion(struct Nodelist *n, struct curve *ml, struct Ilist *psnod,
struct Ilist *psle, struct region *reg, struct region *reg0,int ii1)
{
   int i, j;
   int offset, newnodes;                          // here current region starts

   float u1[3], u2[3];
   float p[3];

   int *itmp;

   struct node **tmpnode = NULL;

#ifdef DEBUG_PS
   char fn[111];
   FILE *fp;
   static int count = 0;

   sprintf(fn,"rr_debugps_%02d.txt", count++);
   if( (fp = fopen(fn,"w+")) == NULL)
   {
      fprintf(stderr,"Shit happened opening file '%s'!\n",fn);
      exit(-1);
   }
   fprintf(fp," MeshRR_PSRegion %d\n",count);
#endif

   // mem. check
   for(i = 0; i < reg->numl; i++)
   {
      if(reg->nodes[i])
      {
         FreeIlistStruct(reg->nodes[i]);
         reg->nodes[i] = NULL;
      }
      reg->nodes[i] = AllocIlistStruct(reg->line[i]->nump+1);
   }
   if(reg->nodes[reg->numl])
   {
      FreeIlistStruct(reg->nodes[reg->numl]);
      reg->nodes[reg->numl+1] = NULL;
   }
   reg->nodes[reg->numl] = AllocIlistStruct(reg->line[0]->nump * reg->line[1]->nump + 1);

   offset = n->num;

#ifdef DEBUG_PS
   fprintf(fp," offset = %d\n", offset);
#endif
   // upper part
   for(i = 0; i <= ii1; i++)
   {
      Add2Ilist(reg->nodes[reg->numl], reg0->nodes[3]->list[reg0->nodes[3]->num-(i+1)]);
      // create nodes, leave out first node.
      u1[0] = reg->arc[2]->list[i] - reg->arc[1]->list[i];
      u1[1] = reg->line[2]->y[i]   - reg->line[1]->y[i];
      u2[0] = reg->arc[1]->list[i];
      u2[1] = reg->line[1]->y[i];
      for(j = 1; j < reg->line[0]->nump; j++)
      {
         p[0] = u2[0] + reg->para[0]->list[j]*u1[0];
         p[1] = u2[1] + reg->para[0]->list[j]*u1[1];
         Add2Ilist(reg->nodes[reg->numl], n->num);
         AddVNode(n, p, ARC);
      }
   }

   // rest
   for(i = ii1+1; i < reg->line[1]->nump; i++)
   {
      u1[0] = reg->arc[2]->list[i] - reg->arc[1]->list[i];
      u1[1] = reg->line[2]->y[i]   - reg->line[1]->y[i];
      u2[0] = reg->arc[1]->list[i];
      u2[1] = reg->line[1]->y[i];
      for(j = 0; j < reg->line[0]->nump; j++)
      {
         p[0] = u2[0] + reg->para[0]->list[j]*u1[0];
         p[1] = u2[1] + reg->para[0]->list[j]*u1[1];
         Add2Ilist(reg->nodes[reg->numl], n->num);
         AddVNode(n, p, ARC);
      }
   }
   Add2Ilist(reg->nodes[0], reg0->nodes[2]->list[reg0->nodes[2]->num-1]);
   newnodes = n->num - offset;
   CalcNodeRadius(&n->n[n->num-newnodes], ml, newnodes);
   CalcNodeAngle(&n->n[n->num-newnodes], ml, newnodes);

   // get boundary nodes
   // extension
   tmpnode = n->n + offset;
   for(i = 0; i < reg->line[0]->nump-1; i++)
   {
      Add2Ilist(psle, (*tmpnode)->index);
      Add2Ilist(reg->nodes[0], (*tmpnode)->index);
      tmpnode++;
   }
   // envelope
   for(i = 0; i <= ii1; i++)
      Add2Ilist(reg->nodes[1],
         reg0->nodes[3]->list[reg0->nodes[3]->num-(i+1)]);
   tmpnode = n->n + offset + (ii1+1)*(reg->line[0]->nump-1);
   for(i = ii1+1; i < reg->line[1]->nump; i++)
   {
      Add2Ilist(reg->nodes[1], (*tmpnode)->index);
      tmpnode += reg->line[0]->nump;
   }
   // blade surface
   itmp = reg->nodes[reg->numl]->list+reg->line[0]->nump-1;
   for(i = 0; i < reg->line[2]->nump; i++)
   {
      Add2Ilist(psnod, (*itmp));
      Add2Ilist(reg->nodes[2], (*itmp));
      itmp += reg->line[0]->nump;
   }
   // te ext.
   tmpnode = n->n + (n->num - reg->line[3]->nump);
   for(i = 0; i < reg->line[0]->nump; i++)
   {
      Add2Ilist(reg->nodes[3], (*tmpnode)->index);
      tmpnode++;
   }

#ifdef DEBUG_PS
   fprintf(fp,"psle\n");
   DumpIlist2File(psle, fp);
   fprintf(fp,"psnod\n");
   DumpIlist2File(psnod, fp);

   for(i = 0; i < reg->numl+1; i++)
   {
      fprintf(fp," ** reg->nodes[%d] **\n", i);
      DumpIlist2File(reg->nodes[i], fp);
   }
#endif

#ifdef DEBUG_PS
   fclose(fp);
#endif
   return(0);
}

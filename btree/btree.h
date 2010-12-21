/******************************************************************************/
/*                                                                            */
/*  Name:        btree.h                                                      */
/*                                                                            */
/*  Written by: David Galbraith                                               */
/*                                                                            */
/*  Date:         October 1995                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*  This code is hereby released to the public domain.  You may use and/or    */
/*  modify it in any way you choose without restriction.                      */
/*                                                                            */
/******************************************************************************/

#ifndef __BTREE_H
#define __BTREE_H
#endif

/* binary tree structure declarations */

struct btnode {
        void *data;
        struct btnode *left;
        struct btnode *right;
};

typedef struct btnode TNode;

typedef struct btnode *TNodePtr;

typedef int (*CompareFunction) (void *, void*);

typedef void (*NodeFunction) (void *);

void tFree(TNodePtr *proot);

int tInsert(TNodePtr *proot, void *data, CompareFunction tcf);

void *tDelete(TNodePtr *proot, void *data, CompareFunction tcf);

void *tSearch(TNodePtr *proot, void *data, CompareFunction tcf);

void tTraverse(TNodePtr *proot, NodeFunction tnf);

void tDisplay(TNodePtr *proot, NodeFunction tnf);











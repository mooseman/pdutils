/******************************************************************************/
/*                                                                            */
/*  Name:        btree.c                                                      */
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

#include <stdio.h>
#include <stdlib.h>

#ifndef __BTREE_H
#include "btree.h"
#endif

static void AuxDisplay(TNodePtr *p, NodeFunction tnf, int depth);

void tFree(TNodePtr *proot)
{
        if (*proot == NULL)
                return;
        tFree(&((*proot)->left));
        tFree(&((*proot)->right));
        free(*proot);
        *proot = NULL;
        return;
}

int tInsert(TNodePtr *proot, void *data, CompareFunction tcf)
{
        TNodePtr        *NewNodePtr;
        TNodePtr        pNewNode;

        NewNodePtr = proot;

        while (*NewNodePtr != NULL)
        {
                int             comparison;
                TNodePtr        pNode;

                pNode = *NewNodePtr;
                comparison = tcf(data, pNode->data);
                if (comparison == 0)
                        return(0);
                else
                if (comparison < 0)
                        NewNodePtr = &pNode->left;
                else
                        NewNodePtr = &pNode->right;
        }

        pNewNode = (TNodePtr) malloc (sizeof(TNode));
                
        if (pNewNode == NULL)
        {
                return(-1);
        }
        
        pNewNode->left = NULL;
        pNewNode->right = NULL;
        pNewNode->data = data;

        *NewNodePtr = pNewNode;

        return(1);
}

void *tDelete(TNodePtr *proot, void *data, CompareFunction tcf)
{
        TNodePtr        *DelNodePtr;
        TNodePtr        pTemp;
        void            *deletion;

        DelNodePtr = proot;

        while (*DelNodePtr != NULL)
        {
                int             comparison;
                TNodePtr        pNode;

                pNode = *DelNodePtr;

                comparison = tcf(data, pNode->data);

                if (comparison == 0)
                        break;
                else
                if (comparison < 0)
                        DelNodePtr = &pNode->left;
                else
                        DelNodePtr = &pNode->right;
        }

        if (DelNodePtr == NULL)
                return NULL;

        deletion = (*DelNodePtr)->data;

        if (((*DelNodePtr)->left) == NULL)
        {
                pTemp = *DelNodePtr;
                *DelNodePtr = pTemp->right;
                free(pTemp);
        } else
        if (((*DelNodePtr)->right) == NULL)
        {
                pTemp = *DelNodePtr;
                *DelNodePtr = pTemp->left;
                free(pTemp);
        }
        else
        {
                TNodePtr        *ItemPtr;

                ItemPtr = &((*DelNodePtr)->right);

                while (((*ItemPtr)->left) != NULL)
                        ItemPtr = &((*ItemPtr)->left);

                pTemp = *ItemPtr;

                (*DelNodePtr)->data = pTemp->data;

                *ItemPtr = pTemp->right;

                free(pTemp);
        }
        return(deletion);
}

void *tSearch(TNodePtr *proot, void *data, CompareFunction tcf)
{
        TNodePtr        *SearchNodePtr;

        SearchNodePtr = proot;

        while (*SearchNodePtr != NULL)
        {
                int             comparison;
                TNodePtr        pNode;

                pNode = *SearchNodePtr;
                comparison = tcf(data, pNode->data);

                if (comparison == 0)
                        return(pNode->data);
                else
                if (comparison < 0)
                        SearchNodePtr = &pNode->left;
                else
                        SearchNodePtr = &pNode->right;
        }

        return(NULL);
}

void tTraverse(TNodePtr *proot, NodeFunction tnf)
{
        TNodePtr        *CurNodePtr;

        CurNodePtr = proot;

        while (*CurNodePtr != NULL)
        {
                TNodePtr        tNode;

                tNode = *CurNodePtr;
                tTraverse(&tNode->left, tnf);
                tnf(tNode->data);
                tTraverse(&tNode->right, tnf);
             
        }
        return;
}

void tDisplay(TNodePtr *proot, NodeFunction tnf)
{
        AuxDisplay(proot, tnf, 0);
        return;
}

static void AuxDisplay(TNodePtr *p, NodeFunction tnf, int depth)
{
        int i;

        if (*p == NULL)
                return;
        AuxDisplay(&((*p)->right), tnf, depth+1);
        for(i = 0; i < 4 * depth; i++)
                putchar(' ');
        printf("| ");
        tnf((*p)->data);
        printf("\n\n");
        AuxDisplay(&((*p)->left), tnf, depth+1);
}

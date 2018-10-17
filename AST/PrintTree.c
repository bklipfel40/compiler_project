#include <stdio.h>

#include "PrintTree.h"

void
PrintIndent(int indent) {
  for (int i = 0; i < indent; i++) {
    printf(" ");
  }
}

void
PrintAST(struct node * tree, int indent) {
  switch (tree->type) {
    case BLOCK:
    PrintIndent(indent);
    printf("BLOCK\n");
    PrintAST(tree->block.stmt,indent+2);
    if (tree->block.block) {
      PrintAST(tree->block.block,indent+2);
    }
    break;
    case ASSIGN:
    PrintIndent(indent);
    printf("ASSIGN \n");
    PrintAST(tree->assign.lhs,indent+2);
    PrintAST(tree->assign.expr,indent+2);
    break;
    case UNOP:
    PrintIndent(indent);
    printf("UNOP ");
    printf("%s \n",tree->unop.op);
    PrintAST(tree->unop.oprnd,indent+2);
    break;
    case BIOP:
    PrintIndent(indent);
    printf("BIOP ");
    printf("%s \n",tree->biop.op);
    PrintAST(tree->biop.oprnd1,indent+2);
    PrintAST(tree->biop.oprnd2,indent+2);
    break;
    case IDENT:
    PrintIndent(indent);
    printf("IDENT ");
    printf("%s \n",tree->ident.ident);
    break;
    case LITERAL:
    PrintIndent(indent);
    printf("LITERAL ");
    printf("%s \n",tree->literal.literal);
    break;
  }
}

#define MIN(x,y) ( (x) < (y) ? (x) : (y) )

int
LayoutAST(struct node * tree,int col) {
  switch (tree->type) {
    case BLOCK: {
      int bCol = LayoutAST(tree->block.stmt,col);
      if (tree->block.block) {
        int eCol = LayoutAST(tree->block.block,bCol);
        tree->pos[0] = (tree->block.stmt->pos[0] + tree->block.block->pos[0]) / 2.0;
        tree->pos[1] = MIN(tree->block.stmt->pos[1],tree->block.block->pos[1]) - 1;
        return eCol;
      }
      else {
        tree->pos[0] = tree->block.stmt->pos[0];
        tree->pos[1] = tree->block.stmt->pos[1] - 1;
        return bCol+1;
      }
    } break;
    case ASSIGN: {
      int lCol = LayoutAST(tree->assign.lhs,col);
      int eCol = LayoutAST(tree->assign.expr,lCol);
      tree->pos[0] = (tree->assign.lhs->pos[0] + tree->assign.expr->pos[0]) / 2.0;
      tree->pos[1] = MIN(tree->assign.lhs->pos[1],tree->assign.expr->pos[1]) - 1;
      return eCol;
    } break;
    case UNOP: {
      int uCol = LayoutAST(tree->unop.oprnd,col);
      tree->pos[0] = tree->unop.oprnd->pos[0];
      tree->pos[1] = tree->unop.oprnd->pos[1] - 1;
      return uCol;
    } break;
    case BIOP: {
      int o1Col = LayoutAST(tree->biop.oprnd1,col);
      int o2Col = LayoutAST(tree->biop.oprnd2,o1Col);
      tree->pos[0] = (tree->biop.oprnd1->pos[0] + tree->biop.oprnd2->pos[0]) / 2.0;
      tree->pos[1] = MIN(tree->biop.oprnd1->pos[1],tree->biop.oprnd2->pos[1]) - 1;
      return o2Col;
    } break;
    case IDENT: {
      tree->pos[0] = (float) col;
      tree->pos[1] = 0.0;
      return col+1;
    } break;
    case LITERAL: {
      tree->pos[0] = (float) col;
      tree->pos[1] = 0.0;
      return col+1;
    } break;
  }
  return -1;
}

#define TEXT_STYLE "font-family=\"sans-serif\" font-size=\"0.4px\" text-anchor=\"middle\" fill=\"rgb(0,0,0)\""
#define LINE_STYLE "style=\"stroke:rgb(0,0,0);stroke-width:0.1\""
#define NODE_STYLE "r=\"0.3\" stroke=\"black\" stroke-width=\"0.1\" fill=\"rgb(200,200,255)\""
#define IDENT_STYLE "r=\"0.3\" stroke=\"black\" stroke-width=\"0.1\" fill=\"rgb(200,255,200)\""
#define LITERAL_STYLE "r=\"0.3\" stroke=\"black\" stroke-width=\"0.1\" fill=\"yellow\""

void
PrintSVG(struct node * tree) {
  switch (tree->type) {
    case BLOCK: {
      printf("<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" %s/>\n",tree->pos[0],tree->pos[1],tree->block.stmt->pos[0],tree->block.stmt->pos[1],LINE_STYLE);
      PrintSVG(tree->block.stmt);
      if (tree->block.block) {
        printf("<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" %s/>\n",tree->pos[0],tree->pos[1],tree->block.block->pos[0],tree->block.block->pos[1],LINE_STYLE);
        PrintSVG(tree->block.block);
      }
      printf("<circle cx=\"%f\" cy=\"%f\" %s />\n",tree->pos[0],tree->pos[1],NODE_STYLE);
    } break;
    case ASSIGN: {
      printf("<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" %s/>\n",tree->pos[0],tree->pos[1],tree->assign.lhs->pos[0],tree->assign.lhs->pos[1],LINE_STYLE);
      printf("<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" %s/>\n",tree->pos[0],tree->pos[1],tree->assign.expr->pos[0],tree->assign.expr->pos[1],LINE_STYLE);
      PrintSVG(tree->assign.lhs);
      PrintSVG(tree->assign.expr);
      printf("<circle cx=\"%f\" cy=\"%f\" %s />\n",tree->pos[0],tree->pos[1],NODE_STYLE);
      printf("<text x=\"%f\" y=\"%f\" %s> %s </text>",tree->pos[0],tree->pos[1] + 0.1,TEXT_STYLE,":=");
    } break;
    case UNOP: {
      printf("<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" %s/>\n",tree->pos[0],tree->pos[1],tree->unop.oprnd->pos[0],tree->unop.oprnd->pos[1],LINE_STYLE);
      PrintSVG(tree->unop.oprnd);
      printf("<circle cx=\"%f\" cy=\"%f\" %s />\n",tree->pos[0],tree->pos[1],NODE_STYLE);
      printf("<text x=\"%f\" y=\"%f\" %s> %s </text>",tree->pos[0],tree->pos[1] + 0.1,TEXT_STYLE,tree->unop.op);
    } break;
    case BIOP: {
      printf("<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" %s/>\n",tree->pos[0],tree->pos[1],tree->biop.oprnd1->pos[0],tree->biop.oprnd1->pos[1],LINE_STYLE);
      printf("<line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" %s/>\n",tree->pos[0],tree->pos[1],tree->biop.oprnd2->pos[0],tree->biop.oprnd2->pos[1],LINE_STYLE);
      PrintSVG(tree->biop.oprnd1);
      PrintSVG(tree->biop.oprnd2);
      printf("<circle cx=\"%f\" cy=\"%f\" %s />\n",tree->pos[0],tree->pos[1],NODE_STYLE);
      printf("<text x=\"%f\" y=\"%f\" %s> %s </text>",tree->pos[0],tree->pos[1] + 0.1,TEXT_STYLE,tree->biop.op);
    } break;
    case IDENT: {
      printf("<circle cx=\"%f\" cy=\"%f\" %s />\n",tree->pos[0],tree->pos[1],IDENT_STYLE);
      printf("<text x=\"%f\" y=\"%f\" %s> %s </text>",tree->pos[0],tree->pos[1] + 0.1,TEXT_STYLE,tree->ident.ident);
    } break;
    case LITERAL: {
      printf("<circle cx=\"%f\" cy=\"%f\" %s />\n",tree->pos[0],tree->pos[1],LITERAL_STYLE);
      printf("<text x=\"%f\" y=\"%f\" %s> %s </text>",tree->pos[0],tree->pos[1] + 0.15,TEXT_STYLE,tree->literal.literal);
    } break;
  }
}

void
PrintSVGAST(struct node * tree) {
  
  float nextCol = LayoutAST(tree,0);
  float depth = tree->pos[1];
  
  printf("<html> <body> <svg width=\"750\" height=\"750\" viewbox=\"-1 %f %f 1\" >\n",depth,nextCol);
  
  PrintSVG(tree);
  
  printf("</svg> </body> </html>\n");
}


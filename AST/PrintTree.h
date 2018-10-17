#ifndef PRINT_TREE_H
#define PRINT_TREE_H

struct block {
  struct node * stmt;
  struct node * block;
};

struct assign {
  struct node * lhs;
  struct node * expr;
};

struct unop {
  char * op;
  struct node * oprnd;
};

struct biop {
  char * op;
  struct node * oprnd1;
  struct node * oprnd2;
};

struct ident {
  char * ident;
};

struct literal {
  char * literal;
};

#define BLOCK 0
#define ASSIGN 1
#define UNOP 2
#define BIOP 3
#define IDENT 4
#define LITERAL 5

struct node {
  float pos[2];
  int type;
  union {
    struct block block;
    struct assign assign;
    struct unop unop;
    struct biop biop;
    struct ident ident;
    struct literal literal;
  };
};

void PrintAST(struct node * tree, int indent);

void PrintSVGAST(struct node * tree);

#endif

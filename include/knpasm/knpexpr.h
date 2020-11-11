#ifndef KNP_EXPR_H
#define KNP_EXPR_H

#include <stdint.h>

// simple map for identifiers
typedef struct
{
	size_t   n;
	size_t   size;
	char*    pkeys;
	int64_t* pvals;
}
idnmap_t;

void    idnmap_init     (idnmap_t* pmap);
void    idnmap_free     (idnmap_t* pmap);

void    idnmap_insert   (idnmap_t* pmap, char key[], int64_t val);
void    idnmap_replace  (idnmap_t* pmap, char key[], int64_t val);
int64_t idnmap_get      (idnmap_t* pmap, char key[]);

typedef enum
{
	IDN,
	ECONST,
	UNARY,
	BINARY
}
expr_type_t;

// base expr_t fields
typedef struct
{
	expr_type_t ty;
	int64_t     cval;
	bool        cval_init;
}
expr_t;

// expr_const_t.ty = ECONST
typedef struct
{
	expr_t base;
}
expr_const_t;

// expr_idn_t.ty = IDN
typedef struct
{
	expr_t base;
	char idn[32];
}
expr_idn_t;


typedef enum
{
	OP_ADD = -128,	// +
	OP_SUB,			// -
	OP_MUL,			// *
	OP_DIV,			// /
	OP_AND,			// &&
	OP_OR,			// ||
	OP_EQ,			// ==
	OP_NE,			// !=
	OP_GE,			// >=
	OP_LE,			// <=
	OP_GT,			// >
	OP_LT,			// <
	OP_NOT,			// !
}
expr_op_type_t;

// expr_opu_t.ty = UNARY
typedef struct
{
	expr_t base;
	expr_op_type_t opty;
	expr_t* val;
}
expr_opu_t;

// expr_opb_t.ty = BINARY
typedef struct
{
	expr_t base;
	expr_op_type_t opty;
	expr_t* lval;
	expr_t* rval;
}
expr_opb_t;


//      creates an expression from a string representation
expr_t* make_expr(const char* pcstr, char** valid_idn, size_t nidn);

//      evaluates an expression given an identifier map
int64_t eval_expr(expr_t* pexpr, idnmap_t* idnmap);

void    print_expr(expr_t* pexpr);

//      releases the resources allocated for an expr_t
void    free_expr(expr_t* pexpr);

#endif

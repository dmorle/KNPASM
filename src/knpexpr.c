#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <knpasm/knpexpr.h>

#define    IDN_EXPR(pexpr) ((expr_idn_t   *)pexpr)
#define  CONST_EXPR(pexpr) ((expr_const_t *)pexpr)
#define  UNARY_EXPR(pexpr) ((expr_opu_t   *)pexpr)
#define BINARY_EXPR(pexpr) ((expr_opb_t   *)pexpr)

const static size_t idnmap_keylen = 32;

void idnmap_init(idnmap_t* pmap)
{
	pmap->n = 0;
	pmap->size = 10;
	pmap->pkeys =    (char*) malloc(sizeof(char)    * pmap->size * idnmap_keylen);
	pmap->pvals = (int64_t*) malloc(sizeof(int64_t) * pmap->size);
}

void idnmap_free(idnmap_t* pmap)
{
	free(pmap->pkeys);
	free(pmap->pvals);
}

void idnmap_insert(idnmap_t* pmap, char key[], int64_t val)
{
	if (pmap->size == pmap->n)
	{
		void* tmp;
		pmap->size *= 2;
		tmp = realloc(pmap->pkeys, sizeof(char) * pmap->size * idnmap_keylen);
		if (!tmp)
		{
			fprintf(stderr, "Memory overflow\n");
			exit(1);
		}
		pmap->pkeys = tmp;

		tmp = realloc(pmap->pvals, sizeof(int64_t) * pmap->size);
		if (!tmp)
		{
			fprintf(stderr, "Memory overflow\n");
			exit(1);
		}
		pmap->pvals = tmp;
	}

	strcpy(pmap->pkeys + pmap->n * idnmap_keylen, key);
	pmap->pvals[pmap->n] = val;
	pmap->n++;
}

void idnmap_replace(idnmap_t* pmap, char key[], int64_t val)
{
	size_t idx, i;
	for (i = 0; i < pmap->n; i++)
		if (!strcmp(pmap->pkeys + i * idnmap_keylen, key))
		{
			idx = i;
			break;
		}

	if (i == pmap->n)
		return;

	pmap->pvals[idx] = val;
}

int64_t idnmap_get(idnmap_t* pmap, char key[])
{
	size_t idx, i;
	for (i = 0; i < pmap->n; i++)
		if (!strcmp(pmap->pkeys + i * idnmap_keylen, key))
		{
			idx = i;
			break;
		}

	if (i == pmap->n)
		return 0;

	return pmap->pvals[idx];
}



typedef struct
{
	bool isop;	   // isop ? c : pexpr->*;
	expr_op_type_t op;
	expr_t* pexpr;
}
expr_str_elem_t;

static bool isopbch(char c)
{
	return OP_ADD <= c && c <= OP_NOT;
}

static expr_t* _make_expr(const char* pcstr, size_t len, char** valid_idn, size_t nidn)
{
	// check for constants and identifiers (base case)
	{
		size_t i;
		for (i = 0; i < len; i++)
			if (isopbch(pcstr[i]))
				break;
		if (i == len)
		{
			// check for a constant
			for (i = 0; i < len; i++)
				if (pcstr[i] < '0' || '9' < pcstr[i])
					break;
			if (i == len)
			{
				// its a constant value
				expr_const_t* npexpr = malloc(sizeof(expr_const_t));
				if (!npexpr)
				{
					fprintf(stderr, "Memory overflow\n");
					exit(1);
				}

				int64_t cval = 0;
				for (i = 0; i < len; i++)
				{
					cval *= 10;
					cval += (int64_t)pcstr[i] - '0';
				}

				npexpr->base.ty = ECONST;
				npexpr->base.cval_init = true;
				npexpr->base.cval = cval;
				return npexpr;
			}

			// check for an identifier
			for (i = 0; i < nidn; i++)
				if (!strncmp(pcstr, valid_idn[i], len))
					break;
			if (i == nidn)
			{
				printf("Identifier %s not found", pcstr);
				return NULL;
			}

			// its an identifier
			if (len >= 31)
			{
				printf("Identifier buffer overflow\n");
				return NULL;
			}

			expr_idn_t* npexpr = malloc(sizeof(expr_idn_t));
			if (!npexpr)
			{
				fprintf(stderr, "Memory overflow\n");
				exit(1);
			}
			npexpr->base.ty = IDN;
			npexpr->base.cval_init = false;
			for (i = 0; i < len; i++)
				npexpr->idn[i] = pcstr[i];
			npexpr->idn[i] = 0;
			return npexpr;
		}
	}

	// more complex expression (recursive case)

#define EXPR_ARR_SIZE 32
#define EXPR_BUF_SIZE 256

	expr_str_elem_t pEA[EXPR_ARR_SIZE];
	uint8_t eai = 0;

	char exprbuf[EXPR_BUF_SIZE];
	char* pexprstr = NULL;
	uint8_t bdepth = 0;

	// creating the expression element array
	for (char* pC = pcstr; pC != pcstr + len; pC++)
	{
		// check if in a bracket
		if (bdepth)
		{
			if (*pC == ')' && --bdepth == 0)
			{
				// find exprbuf len and do recursive call with insertion
				expr_t* npexpr = _make_expr(exprbuf, pexprstr - exprbuf, valid_idn, nidn);
				if (!npexpr)
					goto RET_NULL;

				pEA[eai].isop = false;
				pEA[eai].pexpr = npexpr;
				pexprstr = NULL;
				eai++;
				continue;
			}
			if (*pC == '(')
				bdepth++;

			*pexprstr = *pC;
			pexprstr++;
			continue;
		}

		// check for a new bracket
		if (*pC == '(')
		{
			pexprstr = exprbuf;
			bdepth = 1;
			continue;
		}

		// check for operator
		if (isopbch(*pC))
		{
			if (pexprstr)
			{
				// find exprbuf len and do recursive call with insertion
				expr_t* npexpr = _make_expr(exprbuf, pexprstr - exprbuf, valid_idn, nidn);
				if (!npexpr)
				{
					// TODO: free allocated resources
					return NULL;
				}

				pEA[eai].isop = false;
				pEA[eai].pexpr = npexpr;
				pexprstr = NULL;
				eai++;
			}

			if (eai == EXPR_ARR_SIZE)
			{
				// TODO: free allocated resources
				printf("expression array buffer overflow\n");
				return NULL;
			}
			pEA[eai].isop = true;
			pEA[eai].op = *pC;
			eai++;
			continue;
		}

		// check for new expression string
		if (!pexprstr)
			pexprstr = exprbuf;

		// within a constant or identifier
		// continue constructing exprbuf
		*pexprstr = *pC;
		pexprstr++;
	}

	if (pexprstr)
	{
		// find exprbuf len and do recursive call with insertion
		expr_t* npexpr = _make_expr(exprbuf, pexprstr - exprbuf, valid_idn, nidn);
		if (!npexpr)
			goto RET_NULL;

		pEA[eai].isop = false;
		pEA[eai].pexpr = npexpr;
		pexprstr = NULL;
		eai++;
	}

	// constructing an expr_t from the expr element array

	// collapsing unary operators
	for (size_t i = 0; i < eai; i++)
		if (
			pEA[i].isop &&
			i < eai - 1 &&
			!pEA[i + 1].isop &&
			(
				(pEA[i].op == OP_SUB && (!i || pEA[i - 1].isop)) ||
				pEA[i].op == OP_NOT
			)
		)
		{
			expr_opu_t* npexpr = (expr_opu_t*)malloc(sizeof(expr_opu_t));
			if (!npexpr)
			{
				fprintf(stderr, "Memory overflow\n");
				exit(1);
			}

			npexpr->base.ty = UNARY;
			npexpr->base.cval_init = false;
			npexpr->opty = pEA[i].op;
			npexpr->val = pEA[i + 1].pexpr;

			pEA[i].isop = false;
			pEA[i].pexpr = npexpr;

			// shifting expr elements
			eai--;
			for (size_t j = i + 1; j < eai; j++)
				pEA[j] = pEA[j + 1];
		}

	// collapsing multiplication and division
	for (size_t i = 0; i < eai; i++)
		if (
			pEA[i].isop &&
			i && i < eai - 1 &&
			!pEA[i - 1].isop && !pEA[i + 1].isop &&
			(
				pEA[i].op == OP_MUL ||
				pEA[i].op == OP_DIV
			)
		)
		{
			expr_opb_t* npexpr = (expr_opb_t*)malloc(sizeof(expr_opb_t));
			if (!npexpr)
			{
				fprintf(stderr, "Memory overflow\n");
				exit(1);
			}

			npexpr->base.ty = BINARY;
			npexpr->base.cval_init = false;
			npexpr->opty = pEA[i].op;
			npexpr->lval = pEA[i - 1].pexpr;
			npexpr->rval = pEA[i + 1].pexpr;

			pEA[i - 1].pexpr = npexpr;

			// shifting expr elements by 2
			eai -= 2;
			for (size_t j = i; j < eai; j++)
				pEA[j] = pEA[j + 2];
			i--;
		}

	// collapsing addition and subtraction
	for (size_t i = 0; i < eai; i++)
		if (
			pEA[i].isop &&
			i && i < eai - 1 &&
			!pEA[i - 1].isop && !pEA[i + 1].isop &&
			(
				pEA[i].op == OP_ADD ||
				pEA[i].op == OP_SUB
			)
		)
		{
			expr_opb_t* npexpr = (expr_opb_t*)malloc(sizeof(expr_opb_t));
			if (!npexpr)
			{
				fprintf(stderr, "Memory overflow\n");
				exit(1);
			}

			npexpr->base.ty = BINARY;
			npexpr->base.cval_init = false;
			npexpr->opty = pEA[i].op;
			npexpr->lval = pEA[i - 1].pexpr;
			npexpr->rval = pEA[i + 1].pexpr;

			pEA[i - 1].pexpr = npexpr;

			// shifting expr elements by 2
			eai -= 2;
			for (size_t j = i; j < eai; j++)
				pEA[j] = pEA[j + 2];
			i--;
		}

	// collapsing logical operators
	for (size_t i = 0; i < eai; i++)
		if (
			pEA[i].isop &&
			i && i < eai - 1 &&
			!pEA[i - 1].isop && !pEA[i + 1].isop &&
			(
				pEA[i].op == OP_EQ ||
				pEA[i].op == OP_NE ||
				pEA[i].op == OP_GE ||
				pEA[i].op == OP_LE ||
				pEA[i].op == OP_GT ||
				pEA[i].op == OP_LT
			)
		)
		{
			expr_opb_t* npexpr = (expr_opb_t*)malloc(sizeof(expr_opb_t));
			if (!npexpr)
			{
				fprintf(stderr, "Memory overflow\n");
				exit(1);
			}

			npexpr->base.ty = BINARY;
			npexpr->base.cval_init = false;
			npexpr->opty = pEA[i].op;
			npexpr->lval = pEA[i - 1].pexpr;
			npexpr->rval = pEA[i + 1].pexpr;

			pEA[i - 1].pexpr = npexpr;

			// shifting expr elements by 2
			eai -= 2;
			for (size_t j = i; j < eai; j++)
				pEA[j] = pEA[j + 2];
			i--;
		}

	if (eai != 1)
	{
		printf("Unable to collapse the given expression\n");
		goto RET_NULL;
	}

	return pEA[0].pexpr;

RET_NULL:
	for (size_t i = 0; i < eai; i++)
		free_expr(pEA[i].pexpr);
	return NULL;
}

void simplify_expr(expr_t* pexpr)
{
	if (pexpr->cval_init)
		return;
	
	switch (pexpr->ty)
	{
	case IDN:
		return;
	case ECONST:
		printf("Recieved const expr with uninitialized fields\n");
		return;
	case UNARY:
		simplify_expr(UNARY_EXPR(pexpr)->val);
		if (UNARY_EXPR(pexpr)->val->cval_init)
		{
			switch (UNARY_EXPR(pexpr)->opty)
			{
			case OP_SUB:
				pexpr->cval = -UNARY_EXPR(pexpr)->val->cval;
				break;
			case OP_NOT:
				pexpr->cval = !UNARY_EXPR(pexpr)->val->cval;
				break;
			default:
				printf("Unintialized unary expression type\n");
				return;
			}
			pexpr->cval_init = true;
		}
		return;
	case BINARY:
		simplify_expr(BINARY_EXPR(pexpr)->lval);
		simplify_expr(BINARY_EXPR(pexpr)->rval);
		if (BINARY_EXPR(pexpr)->lval->cval_init && BINARY_EXPR(pexpr)->rval->cval_init)
		{
			switch (BINARY_EXPR(pexpr)->opty)
			{
			case OP_ADD:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval + BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_SUB:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval - BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_MUL:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval * BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_DIV:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval / BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_AND:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval && BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_OR:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval || BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_EQ:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval == BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_NE:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval != BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_GE:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval >= BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_LE:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval <= BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_GT:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval > BINARY_EXPR(pexpr)->rval->cval;
				break;
			case OP_LT:
				pexpr->cval = BINARY_EXPR(pexpr)->lval->cval < BINARY_EXPR(pexpr)->rval->cval;
				break;
			default:
				printf("Unintialized binary expression type\n");
				return;
			}
			pexpr->cval_init = true;
		}
		return;
	default:
		printf("Uninitialized expression type\n");
		return;
	}
}

expr_t* make_expr(const char* pcstr, char** valid_idn, size_t nidn)
{
	// making a mutable string without spaces
	size_t len = 0;
	for (char* pC = pcstr; *pC; pC++)
		if (*pC != ' ' &&
			*pC != '\t' &&
			strncmp(pC, "&&", 2) &&
			strncmp(pC, "||", 2) &&
			strncmp(pC, "==", 2) &&
			strncmp(pC, "!=", 2) &&
			strncmp(pC, ">=", 2) &&
			strncmp(pC, "<=", 2)
			)
			len++;
	char* pstr = (char*)malloc(sizeof(char) * len);
	if (!pstr)
	{
		fprintf(stderr, "Memory overflow\n");
		exit(1);
	}

	char* pstrc = pstr;
	for (char* pC = pcstr; *pC; pC++)
	{
		if (*pC == ' ')
			continue;
		if (*pC == '\t')
			continue;

		if (*pC == '+')
			*pstrc = OP_ADD;
		else if (*pC == '-')
			*pstrc = OP_SUB;
		else if (*pC == '*')
			*pstrc = OP_MUL;
		else if (*pC == '/')
			*pstrc = OP_DIV;
		else if (!strncmp(pC, "&&", 2))
		{
			*pstrc = OP_AND;
			pC++;
		}
		else if (!strncmp(pC, "||", 2))
		{
			*pstrc = OP_OR;
			pC++;
		}
		else if (!strncmp(pC, "==", 2))
		{
			*pstrc = OP_EQ;
			pC++;
		}
		else if (!strncmp(pC, "!=", 2))
		{
			*pstrc = OP_NE;
			pC++;
		}
		else if (!strncmp(pC, ">=", 2))
		{
			*pstrc = OP_GE;
			pC++;
		}
		else if (!strncmp(pC, "<=", 2))
		{
			*pstrc = OP_LE;
			pC++;
		}
		else if (*pC == '>')
			*pstrc = OP_GT;
		else if (*pC == '<')
			*pstrc = OP_LT;
		else if (*pC == '!')
			*pstrc = OP_NOT;
		else
			*pstrc = *pC;
		pstrc++;
	}

	expr_t* pexpr = _make_expr(pstr, len, valid_idn, nidn);
	free(pstr);
	simplify_expr(pexpr);
	return pexpr;
}

int64_t eval_expr(expr_t* pexpr, idnmap_t* pmap)
{
	if (pexpr->cval_init)
		return pexpr->cval;

	switch (pexpr->ty)
	{
	case IDN:
		return idnmap_get(pmap, IDN_EXPR(pexpr)->idn);
	case ECONST:
		return CONST_EXPR(pexpr->cval)->base.cval;
	case UNARY:
	{
		int64_t val = eval_expr(UNARY_EXPR(pexpr)->val, pmap);
		switch (UNARY_EXPR(pexpr)->opty)
		{
		case OP_SUB:
			return -val;
		case OP_NOT:
			return !val;
		default:
			printf("Unintialized unary expression type\n");
			return 0;
		}
	}
	case BINARY:
	{
		int64_t lval = eval_expr(BINARY_EXPR(pexpr)->lval, pmap);
		int64_t rval = eval_expr(BINARY_EXPR(pexpr)->rval, pmap);
		switch (BINARY_EXPR(pexpr)->opty)
		{
		case OP_ADD:
			return lval + rval;
		case OP_SUB:
			return lval - rval;
		case OP_MUL:
			return lval * rval;
		case OP_DIV:
			return lval / rval;
		case OP_AND:
			return lval && rval;
		case OP_OR:
			return lval || rval;
		case OP_EQ:
			return lval == rval;
		case OP_NE:
			return lval != rval;
		case OP_GE:
			return lval >= rval;
		case OP_LE:
			return lval <= rval;
		case OP_GT:
			return lval > rval;
		case OP_LT:
			return lval < rval;
		default:
			printf("Unintialized binary expression type\n");
			return 0;
		}
	}
	default:
		printf("Uninitialized expression type\n");
		return 0;
	}
}

void _print_expr(expr_t* pexpr)
{
	if (pexpr->cval_init)
	{
		printf("%lld", pexpr->cval);
		return;
	}
	
	switch (pexpr->ty)
	{
	case IDN:
		printf("%s", IDN_EXPR(pexpr)->idn);
		return;
	case ECONST:
		printf("%lld", pexpr->cval);
		return;
	case UNARY:
		putc('(', stdout);
		switch (UNARY_EXPR(pexpr)->opty)
		{
		case OP_SUB:
			putc('-', stdout);
			break;
		case OP_NOT:
			putc('!', stdout);
			break;
		}
		_print_expr(UNARY_EXPR(pexpr)->val);
		putc(')', stdout);
		return;
	case BINARY:
		putc('(', stdout);
		_print_expr(BINARY_EXPR(pexpr)->lval);
		switch (BINARY_EXPR(pexpr)->opty)
		{
		case OP_ADD:
			printf("+");
			break;
		case OP_SUB:
			printf("-");
			break;
		case OP_MUL:
			printf("*");
			break;
		case OP_DIV:
			printf("/");
			break;
		case OP_AND:
			printf("&&");
			break;
		case OP_OR:
			printf("||");
			break;
		case OP_EQ:
			printf("==");
			break;
		case OP_NE:
			printf("!=");
			break;
		case OP_GE:
			printf(">=");
			break;
		case OP_LE:
			printf("<=");
			break;
		case OP_GT:
			printf(">");
			break;
		case OP_LT:
			printf("<");
			break;
		}
		_print_expr(BINARY_EXPR(pexpr)->rval);
		putc(')', stdout);
		return;
		return;
	}
}

void print_expr(expr_t* pexpr)
{
	_print_expr(pexpr);
	putc('\n', stdout);
}

void free_expr(expr_t* pexpr)
{
	switch (pexpr->ty)
	{
	case UNARY:
		free_expr(UNARY_EXPR(pexpr)->val);
		break;
	case BINARY:
		free_expr(BINARY_EXPR(pexpr)->lval);
		free_expr(BINARY_EXPR(pexpr)->rval);
		break;
	}
	free(pexpr);
}

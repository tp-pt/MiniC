/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
#pragma warning(disable:4996)
#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */



void printToken(TokenType token, const char* tokenString)
{
    switch (token)
    {
        /*��ӡ�ؼ��� void int*/
    case IF:
    case ELSE:
    case INPUT:
    case OUTPUT:
    case INT:
    case VOID:
    case WHILE:
    case RETURN:
        fprintf(listing,
            "reserved word: %s\n", tokenString);
        break;
    case ASSIGN: fprintf(listing, "=\n"); break;
    case LESS: fprintf(listing, "<\n"); break;
    case GREATER: fprintf(listing, ">\n"); break;
    case EQUAL: fprintf(listing, "==\n"); break;
    case LBRACKET:fprintf(listing, "{\n"); break;
    case RBRACKET:fprintf(listing, "}\n"); break;
    case LPAREN: fprintf(listing, "(\n"); break;
    case RPAREN: fprintf(listing, ")\n"); break;
    case LSQUARE:fprintf(listing, "[\n"); break;
    case RSQUARE:fprintf(listing, "]\n"); break;
    case LESSEQUAL:fprintf(listing, "<=\n"); break;
    case GREATEREQUAL:fprintf(listing, ">=\n"); break;
    case NONEQUAL:fprintf(listing, "!=\n");break;
    case SEMI: fprintf(listing, ";\n"); break;
    case PLUS: fprintf(listing, "+\n"); break;
    case MINUS: fprintf(listing, "-\n"); break;
    case TIMES: fprintf(listing, "*\n"); break;
    case DIVIDE: fprintf(listing, "/\n"); break;
    case COMMA:fprintf(listing, ",\n"); break;
    case ENDFILE: fprintf(listing, "EOF\n"); break;
    case NUM:
        fprintf(listing,
            "NUM, val= %s\n", tokenString);
        break;
    case ID:
        fprintf(listing,
            "ID, name= %s\n", tokenString);
        break;
    case ERROR:
        fprintf(listing,
            "ERROR: %s\n", tokenString);
        break;
    default: /* should never happen */
        fprintf(listing, "Unknown token: %d\n", token);
    }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode* newStmtNode(StmtKind kind)
{
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else {
        for (i = 0;i < MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        t->lineno = lineno;
        t->Arraysize = 0;
        t->index = -1;//�����飬�±�Ĭ��Ϊ-1
        t->mark = 0;//������
    }
    return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode* newExpNode(ExpKind kind)
{
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else {
        for (i = 0;i < MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = ExpK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;
        t->Arraysize = 0;
        t->index = -1;//�����飬�±�Ĭ��Ϊ-1
        t->mark = 0;//������
    }
    return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode* newDecNode(DecKind kind)
{
    TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else {
        for (i = 0;i < MAXCHILDREN;i++) t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = DecK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = Void;//Ĭ������Ϊvoid 
        t->Arraysize = 0;
        t->index = -1;//�����飬�±�Ĭ��Ϊ-1
        t->mark = 0;//������
    }
    return t;
}



/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char* copyString(char* s)
{
    int n;
    char* t;
    if (s == NULL) return NULL;
    n = strlen(s) + 1;
    t = malloc(n);
    if (t == NULL)
        fprintf(listing, "Out of memory error at line %d\n", lineno);
    else strcpy(t, s);
    return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
    int i;
    for (i = 0;i < indentno;i++)
        fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode* tree)
{
    int i;
    INDENT;
    while (tree != NULL) {
        printSpaces();
        //fprintf(listing, "start print tree!\n");
        if (tree->nodekind == StmtK)
        {

            switch (tree->kind.stmt) {
            case IfK:
                fprintf(listing, "If\n");
                break;
            case WhileK:
                fprintf(listing, "While\n");
                break;
            case ReturnK:
                fprintf(listing, "Return value: %d\n", tree->attr.val);
                break;
            case InputK:
                fprintf(listing, "Read: %s\n", tree->attr.name);
                break;
            case OutputK:
                fprintf(listing, "Write\n");
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind<<<<<<\n");
                break;
            }
        }
        else if (tree->nodekind == ExpK)
        {
            switch (tree->kind.exp) {
            case OpK:
                fprintf(listing, "Op: ");
                printToken(tree->attr.op, "\0");
                break;
            case ConstK:
                fprintf(listing, "Const: %d\n", tree->attr.val);
                break;
            case IdK:
                if (tree->mark == 1) {
                    fprintf(listing, "Array Id : %s,index = %d\n", tree->attr.name, tree->index);
                }
                else {
                    fprintf(listing, "Id : %s\n", tree->attr.name);
                }
                break;
            case AssignK:
                if (tree->mark == 1) {
                    fprintf(listing, "Assign to Array : %s,index = %d\n", tree->attr.name, tree->index);
                }
                else {
                    fprintf(listing, "Assign to Id: %s\n", tree->attr.name);
                }
                break;
            case CallK:
                fprintf(listing, "Call function : %s\n", tree->attr.name);
                break;
            case ArgsK:
                fprintf(listing, "Args list as follow : \n");
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind------\n");
                break;
            }
        }
        else if (tree->nodekind == DecK) {
            switch (tree->kind.decs) {
            case VarK:
                if (tree->mark == 1) {
                    fprintf(listing, "Array Dec : %s,size = %d\n", tree->attr.name, tree->Arraysize);
                }
                else {
                    fprintf(listing, "Var Dec: %s\n", tree->attr.name);
                }
                break;
            case FuncK:
                fprintf(listing, "Function Dec: %s\n", tree->attr.name);
                break;
            case ParamK:
                fprintf(listing, "param Id: %s\n", tree->attr.name);
                break;
            case CompoundK:
                fprintf(listing, "Compound stament as follow: \n");
                break;
            default:
                fprintf(listing, "Unknown ExpNode kind>>>>>>\n");
                break;
            }
        }
        else fprintf(listing, "Unknown node kind\n");
        for (i = 0;i < MAXCHILDREN;i++)
            printTree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}

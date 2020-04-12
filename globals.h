/****************************************************/
/* File: globals.h                                  */
/* Global types and vars for TINY compiler          */
/* must come before other include files             */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#define _CRT_SECURE_NO_WARNING

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8

typedef enum
/* book-keeping tokens */
{
    ENDFILE, ERROR,
    /* reserved words --
    ���ӹؼ������� ���Ĭ�Ϸ� �������� ���ر�ʶ*/

    IF, ELSE, WHILE, INPUT, OUTPUT, INT, VOID, RETURN,
    /* multicharacter tokens */
    ID, NUM,

    /* special symbols --�����������*/

    /*����˵��  + - * / < > <= >= == != = ; , ( ) [ ] { } /*ע�� */
    PLUS, MINUS, TIMES, DIVIDE, LESS, GREATER, LESSEQUAL, GREATEREQUAL, EQUAL, NONEQUAL,
    ASSIGN, SEMI, COMMA, LPAREN, RPAREN, LSQUARE, RSQUARE, LBRACKET, RBRACKET, LCOMMENT, RCOMMNET,


} TokenType;

extern FILE* source; /* source code text file ��ȡ�ļ�*/
extern FILE* listing; /* listing output text file ����ļ�*/
extern FILE* code; /* code text file for TM simulator �ֽ��ļ�*/

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

/*���ڵ㣬���ʽ�ڵ� */
typedef enum { StmtK, ExpK, DecK } NodeKind;
typedef enum {
    RepeatK,
    WhileK, IfK, InputK, OutputK, ReturnK,
} StmtKind;
typedef enum { OpK, ConstK, IdK, CallK, AssignK, ArgsK } ExpKind;

/*���������ڵ�*/
//������������������
typedef enum { VarK, FuncK, ParamK, CompoundK }DecKind;

/* ExpType is used for type checking */
typedef enum { Void, Integer, Boolean } ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
{
    struct treeNode* child[MAXCHILDREN];
    struct treeNode* sibling;
    int lineno;
    NodeKind nodekind;
    union { StmtKind stmt; ExpKind exp; DecKind decs; } kind;
    union {
        TokenType op;
        int val;
        char* name;
    } attr;
    ExpType type; /* for type checking of exps */

    int Arraysize;//����ڵ� ��С
    int index;//����ƫ����
    int mark;//������

} TreeNode;

/**************************************************/
/***********   Flags for tracing       ************/
/**************************************************/

/* EchoSource = TRUE causes the source program to
 * be echoed to the listing file with line numbers
 * during parsing
 */
extern int EchoSource;

/* TraceScan = TRUE causes token information to be
 * printed to the listing file as each token is
 * recognized by the scanner
 */
extern int TraceScan;

/* TraceParse = TRUE causes the syntax tree to be
 * printed to the listing file in linearized form
 * (using indents for children)
 */
extern int TraceParse;

/* TraceAnalyze = TRUE causes symbol table inserts
 * and lookups to be reported to the listing file
 */
extern int TraceAnalyze;

/* TraceCode = TRUE causes comments to be written
 * to the TM code file as code is generated
 */
extern int TraceCode;

/* Error = TRUE prevents further passes if an error occurs */
extern int Error;
#endif

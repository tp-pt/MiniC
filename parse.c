/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
#pragma warning(disable:4996)
#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls
static TreeNode* stmt_sequence(void);
static TreeNode* statement(void);
static TreeNode* if_stmt(void);
static TreeNode* repeat_stmt(void);
static TreeNode* assign_stmt(void);
static TreeNode* read_stmt(void);
static TreeNode* write_stmt(void);
*/

//dec --�������
static TreeNode* input_stmt(void);
static TreeNode* output_stmt(void);

static TreeNode* dec_list(void);//�����б�
static TreeNode* dec(void);//����
static TreeNode* var_dec(char* Type, char* id);//��������
static TreeNode* func_dec(char* Type, char* id);//��������

static TreeNode* param(void);//����

static TreeNode* params_list(void);//�����б�

static TreeNode* compound_stmt(void);//�������


static TreeNode* local_dec(void);//�ֲ�����
static TreeNode* stmt_list(void);//�����б�


static TreeNode* stmt(void);//��������

static TreeNode* exp_stmt(void);//���ʽ���
static TreeNode* exp(void);//���ʽ
static TreeNode* simple_exp(char* id);//�򵥱��ʽ
static TreeNode* var_exp(char* tempid, int tempindex, int arr);//����


static TreeNode* select_stmt(void);//�������
static TreeNode* iter_stmt(void);//�������
static TreeNode* return_stmt(void);//�������

static TreeNode* add_exp(char* id);//��ϵ����
static TreeNode* term(char* id);//��������+-
static TreeNode* factor(char* id);//��������*
static TreeNode* call_exp(char* tempid);//����
static TreeNode* args_list(void);//�����б�
static TreeNode* args(void);//����


static void syntaxError(char* message)
{
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

static void match(TokenType expected)
{
    if (token == expected) token = getToken();
    else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        fprintf(listing, "      ");
    }
}

//��һ��program ��ʼparse 
//dec_list -> dec {dec}
TreeNode* dec_list(void) {
    TreeNode* t = dec();

    TreeNode* p = t;
    while ((token != ENDFILE) &&
        (token != ELSE))
    {
        TreeNode* q;
        //match(SEMI);
        q = dec();
        //fprintf(listing, "ʶ��һ��dec!\n");
        if (q != NULL) {
            if (t == NULL) t = p = q;
            else /* now p cannot be NULL either */
            {
                p->sibling = q;//��һ������ ��Ϊ�ֵܽڵ�
                p = q;//�Ƶ���һ�������ڵ㣬����ѭ��
            }
        }

    }

    //if (t == NULL) fprintf(listing, "no of dec list returns!!\n");
    return t;
}

//�ж���������
TreeNode* dec(void) {
    TreeNode* t = NULL;
    char* tempType = NULL;//��ȡ����
    char* tempID = NULL;//��ȡid����

    if (token == VOID)
    {
        tempType = copyString(tokenString);
        //strcpy(tempType, copyString(tokenString));//void ���ʹ��ݸ� func_dec()
        match(VOID);

        tempID = copyString(tokenString);//ID���ݸ� ��һ��dec()
        match(ID);

        t = func_dec(tempType, tempID);

    }

    else if (token == INT) {

        tempType = copyString(tokenString);
        //fprintf(listing, "here is tempType%s\n",tokenString);
        //strcpy(tempType, copyString(tokenString));//void ���ʹ��ݸ� func_dec()
        match(INT);
        //int a; �� int a(params)
         //strcpy(tempID, copyString(tokenString));//ID���ݸ� ��һ��dec()
        tempID = copyString(tokenString);
        match(ID);

        if (token == SEMI || token == LSQUARE) {
            t = var_dec(tempType, tempID);//var-dec�ڵ�
        }

        else if (token == LPAREN) {
            t = func_dec(tempType, tempID);
        }
    }

    //getToken();
    //if (t == NULL) fprintf(listing, "no of dec returns!!\n");
    else {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();

    }
    /* end case */

    return t;
}

//var_dec -> type id ; | type id [NUM];
//var_dec ��Ϊ�����Ľڵ㣬û���ӽڵ�
TreeNode* var_dec(char* Type, char* id) {
    TreeNode* t = newDecNode(VarK);

    if ((t != NULL) && (id != NULL)) {
        t->attr.name = id;//����
        t->type = Integer;//�������ͼ�¼
    }
    //�ж��Ƿ�����
    if (t != NULL && token == LSQUARE) {
        match(LSQUARE);//int a [num]
        t->mark = 1;
        //��¼size 
       // t->Arraysize = atoi(copyString(tokenString));
        if (token == NUM) {
            t->Arraysize = atoi(copyString(tokenString));
            match(NUM);
        }
        else {
            syntaxError("unexpected token ->>>>>>>>>> ");
            printToken(token, tokenString);
            token = getToken();
        }
        match(RSQUARE);

    }

    //if (t!=NULL) fprintf(listing, "var name!\n");
   // if (t == NULL) fprintf(listing, "no of var dec returns!!\n");
    match(SEMI);

    //no children 
    return t;
}

TreeNode* func_dec(char* Type, char* id) {
    TreeNode* t = newDecNode(FuncK);
    //Dec_node

    if (t != NULL) {
        if (id != NULL) {
            t->attr.name = id;//����
            if (strcmp(Type, "int") == 0) {
                t->type = Integer;
                //Ĭ�ϳ�ʼ��Ϊ Void�����޸�ΪInteger
            }
        }
        match(LPAREN);//ƥ�� (


        if (token == VOID) {
            match(VOID);
            //t->child[0] = NULL;//�޲����б�
        }

        else if (token != VOID) {
            // fprintf(listing, "��ʼ��ӡ�����б�\n");
            t->child[0] = params_list();
        }

        match(RPAREN);

        t->child[1] = compound_stmt();
        //���ͼ�¼
    }
    //�����ӽڵ�
    //t->child[0]->params_node
    //t->child[1]->compound_ssmt
   // if (t != NULL) fprintf(listing, "func name!\n");
   // if (t == NULL) fprintf(listing, "no of func dec returns!!\n");
    return t;

}

//param-list -> param {,param} 
TreeNode* params_list(void) {
    TreeNode* t = param();
    TreeNode* p = t;
    //д�ɵݹ���ʽ ����while
  // while (token != ENDFILE&&token!=ELSE) {
    TreeNode* q;

    //��ֹһ������ �����б�Ϊ�ֵܽڵ�

    if (token == COMMA) {
        match(COMMA);
        q = param();

        if (q != NULL) {
            if (t == NULL) {
                t = p = q;
            }
            else {
                p->sibling = q;
                p = q;
            }
        }
    }
    // }
     //if (t == NULL) fprintf(listing, "no of param list returns!!\n");
    return t;
}

//param -> type ID | type ID []
TreeNode* param(void) {
    TreeNode* t = newDecNode(ParamK);
    if (t != NULL) {
        t->type = Integer;
        match(INT);
        t->attr.name = copyString(tokenString);
        match(ID);

        //ƥ������
        if (token == LSQUARE) {
            match(LSQUARE);
            match(RSQUARE);

            //arraysize ��ʱ����ȷ��
        }
    }

    //if (t == NULL) fprintf(listing, "no of param returns!!\n");
    return t;

}

//compound_stmt -> {local_dec stmt_list}
TreeNode* compound_stmt(void) {
    TreeNode* t = newDecNode(CompoundK);
    if (t != NULL) {
        match(LBRACKET);
        //�����ӽڵ�
        //child[0] --local_dec
        //child[1] --stmt_list
        TreeNode* tc0 = local_dec();
        TreeNode* tc1 = stmt_list();
        if (tc0 != NULL) {
            t->child[0] = tc0;

            if (tc1 != NULL) {
                t->child[1] = tc1;
            }
        }
        else {
            //�սڵ����
            if (tc1 != NULL) {
                t->child[0] = tc1;
            }
        }

        match(RBRACKET);//ƥ�� }
    }
    //if (t == NULL) fprintf(listing, "no of compund stmt returns!!\n");
    return t;
}

//local_dec -> {var_dec} |empty
TreeNode* local_dec(void) {

    TreeNode* t = NULL;
    TreeNode* p = NULL;
    /*if (token == INT) {
        char* tempType = copyString(tokenString);
        match(INT);
        char* tempID = copyString(tokenString);
        match(ID);
        t = var_dec(tempType, tempID);


    }
    TreeNode* p = t;
    */
    int head = 0;

    while (token == INT) {

        TreeNode* q = NULL;
        char* tempType = copyString(tokenString);
        match(INT);
        char* tempID = copyString(tokenString);
        match(ID);
        if (head == 0) {
            t = var_dec(tempType, tempID);
            p = t;
            head = 1;
        }
        else {
            q = var_dec(tempType, tempID);
        }

        if (q != NULL) {
            if (t == NULL) t = p = q;
            else
            {
                p->sibling = q;
                p = q;
            }
        }

    }


    //��ӡerrors 


//1�����߶��var_dec ���
/*
TreeNode* t = dec();//�����ж� var_dec
if (t == NULL) {
    fprintf(listing, "no of local dec returns!!\n");
    return t;   //empty�����

}
TreeNode* p = t;
//while ((token != ENDFILE) && (token != ELSE) )
//{
    TreeNode* q;

    q = dec();
    if (q != NULL) {
        if (t == NULL) t = p = q;
        else
        {
            p->sibling = q;
            p = q;
        }
    }

//}

if (t == NULL) fprintf(listing, "no of local dec returns!!\n");
*/
//if (t == NULL) fprintf(listing, "no of local dec returns!!\n");
    return t;
}

//stmt_list -> {stmt} | empty
TreeNode* stmt_list(void) {

    TreeNode* t = NULL;
    TreeNode* p = NULL;
    int head = 0;
    while (token == IF || token == LBRACKET || token == WHILE || token == ID || token == SEMI || token == LPAREN || token == RETURN) {
        TreeNode* q = NULL;

        if (head == 0) {
            t = stmt();
            p = t;
            head = 1;
        }
        else {
            q = stmt();
        }

        if (q != NULL) {
            if (t == NULL) t = p = q;
            else
            {
                p->sibling = q;
                p = q;
            }
        }
    }

    /*
    TreeNode* t = stmt();
    if (t == NULL)
        return t; //empty �����

    TreeNode* p = t;
    //while ((token != ENDFILE)  &&
   //     (token != ELSE) )
    //{
        TreeNode* q;
       // match(SEMI);
        q = stmt();
        if (q != NULL) {
            if (t == NULL) t = p = q;
            else
            {
                p->sibling = q;
                p = q;
            }
        }
      //  else {
      //      break;
       // }
   // }
   */
   // if (t == NULL) fprintf(listing, "no of stmt_list returns!!\n");
    return t;
}


//stmt -> exp_stmt| compound_stmt| select_stmt | iter_stmt| return_stmt
//stmt �����ж�
TreeNode* stmt(void) {
    TreeNode* t = NULL;
    switch (token)
    {
        //exp ���ֿ�ͷ���ƥ�� exp; (ID ��ͷ) | ; (���Էֺſ�ͷƥ��) 
    case ID:
    case SEMI:
    case LPAREN:
        t = exp_stmt();
        break;

    case LBRACKET:
        t = compound_stmt();
        break;
    case IF:
        t = select_stmt();
        break;

    case WHILE:
        t = iter_stmt();
        break;

    case RETURN:
        t = return_stmt();
        break;
    default:
        syntaxError("unexpected token ->>>>>>>>>> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }

    //if (t == NULL) fprintf(listing, "no of stmt returns!!\n");
    return t;
}


TreeNode* exp_stmt(void) {
    TreeNode* t = NULL;
    if (token == SEMI) {
        match(SEMI);
    }
    else {
        t = exp();
        match(SEMI);
    }

    // if (t == NULL) fprintf(listing, "no of exp_stmt returns!!\n");
    return t;

}

//���ּ���exp ��ֵ | simple-exp
//exp -> var = exp | simple_exp
//ID NUM ( ;

TreeNode* exp(void) {

    TreeNode* t = NULL;
    char* tempID = NULL;
    int arr = 0;//����������
    int  tempindex = 0;//���������±�Ĭ��ֵ 


    if (token == NUM) {
        t = simple_exp(tempID);
        // t->attr.val = 2;//ȡNUM ��ֵ
    }
    else if (token == ID) {
        //�ж���û�� = ���� 
        tempID = copyString(tokenString);
        match(ID);
        //����  Ҫ�����±� 
        if (token == LSQUARE) {
            arr = 1;//���Ϊ����
            match(LSQUARE);
            tempindex = exp()->attr.val;//��ȡ���ʽ��ֵ
            match(RSQUARE);

            // fprintf(listing, "match array<<< get here>>>>>>>>>>>>>\n");
        }

        //if (tempID==NULL) fprintf(listing, "no id get here>>>>>>>>>>>>>\n");
        //else {
        //    fprintf(listing, "tempid =%s\n", tempID);
        //}
        //�޸Ľڵ� Ϊ simple-eexp�� call_func() 
        if (token == LPAREN) {
            t = call_exp(tempID);
        }

        else if (token != PLUS && token != MINUS && token != LESS && token != GREATER && token != TIMES
            && token != DIVIDE && token != LESSEQUAL && token != GREATEREQUAL)
        {
            t = var_exp(tempID, tempindex, arr);
        }

        /*else  {
           // t = var_exp(tempID);
            if (token == LSQUARE) {
                match(LSQUARE);
                if (token == ID ||token == NUM) {
                    match(token);
                    //��¼����
                }
                else {
                    fprintf(listing, "it is wrong index of an array!\n");
                    getToken();
                }
                match(RSQUARE);
            }
        }*/

        /*
        if (token == LPAREN) {
            t =simple_exp(tempID);

            //no child in this layer
            //���ñ��ʽ
        }
        else if (token == LSQUARE){//������һ�� simple_exp()
            t = var_exp(tempID);
        }
       */
       //var | NUM ƥ�����
    }


    if (token == ASSIGN) {
        //t = var_exp(tempID);//��ǰƥ����
        t = newExpNode(AssignK);
        t->attr.name = tempID;
        if (arr == 1) {
            t->mark = 1;
            t->index = tempindex;
        }
        match(ASSIGN);
        // fprintf(listing, "Assign here>>>>>>>>>>>>>\n");
        if (t != NULL) t->child[0] = exp();
        // if (t != NULL) fprintf(listing, "no of exp returns----\n");
    }
    else if (token == LESS || token == LESSEQUAL || token == GREATER || token == GREATEREQUAL || token == EQUAL ||
        token == NONEQUAL || token == PLUS || token == MINUS || token == DIVIDE || token == TIMES) {
        /*if(tempID==NULL)
            fprintf(listing, "No temp Id HERE>>>>>>>>>>>>>\n");
         */

        t = simple_exp(tempID);
    }
    /*else if (token == LESS || token == LESSEQUAL || token == GREATER || token == GREATEREQUAL || token == EQUAL ||
        token == NONEQUAL||token==PLUS||token==MINUS||token==DIVIDE||token ==TIMES)
    {
        //token��û��Match
        t=simple_exp();
    }*/

    /*else{
        syntaxError("unexpected token -<<<<<<<<<<< ");
        printToken(token, tokenString);
        token = getToken();

    }
    */
    // if (t == NULL) fprintf(listing, "no of exp returns!!\n");
    return t;

}

//index ����Ĭ��ֵ
TreeNode* var_exp(char* tempId, int tempindex, int arr) {
    TreeNode* t = NULL;

    //id �Ѿ�����һ��match��
    //������һ������Ԫ��
    if (arr == 1) {
        // match(LSQUARE);
        t = newExpNode(IdK);
        if (t != NULL && tempId != NULL) {
            t->attr.name = tempId;
            t->index = tempindex;
            t->mark = 1;
            //fprintf(listing, "index of array is %d\n",tempindex);
        }
        // t->child[0] = exp();
        // t->index = exp()->attr.val;//�����С
        // match(RSQUARE);
    }
    else {
        t = newExpNode(IdK);
        if (t != NULL && tempId != NULL)
            t->attr.name = tempId;
    }
    //if (t == NULL) fprintf(listing, "no of var exp returns!!\n");
    return t;
}


TreeNode* simple_exp(char* id) {
    TreeNode* t = add_exp(id);
    /*if (id == NULL) fprintf(listing, "no id get here>>>>>>>>>>>>>\n");
    else {
        fprintf(listing, "id =%s>>>>>>>>>>\n", id);
    }*/

    /* TreeNode* r = NULL;
     if (left!=NULL)
         t = left;
     else {
         t = add_exp(r);
     }*/

    while ((token == LESSEQUAL) || (token == GREATEREQUAL) || (token == LESS) || (token == GREATER)
        || (token == EQUAL) || (token == NONEQUAL))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            // fprintf(listing, "match tokens here!!\n");
            char* second = NULL;
            t->child[1] = add_exp(second);
        }
    }
    //if (t == NULL) fprintf(listing, "no of simple exp returns!!\n");
    return t;
}

TreeNode* add_exp(char* id) {
    /*if (id == NULL) fprintf(listing, "no id get here>>>>>>>>>>>>>\n");
    else {
        fprintf(listing, "id =%s>>>>>>>>>>\n", id);
    }*/
    TreeNode* t = term(id);
    /* if (left != NULL) {
         t = left;
     }
     else {
         t = term();
     }
     */
    while ((token == PLUS) || (token == MINUS))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            char* second = NULL;
            t->child[1] = term(second);
        }
    }

    //if (t == NULL) fprintf(listing, "no of add exp returns!!\n");
    return t;
}

TreeNode* term(char* id) {
    /* if (id == NULL) fprintf(listing, "no id get here>>>>>>>>>>>>>\n");
     else {
         fprintf(listing, "id =%s>>>>>>>>>>term\n", id);
     }
     */
    TreeNode* t = factor(id);
    while ((token == TIMES) || (token == DIVIDE))
    {
        TreeNode* p = newExpNode(OpK);
        if (p != NULL) {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            char* second = NULL;
            p->child[1] = factor(second);
        }
    }
    if (t == NULL) fprintf(listing, "no of term returns!!\n");
    return t;
}

TreeNode* call_exp(char* tempid) {
    TreeNode* t = newExpNode(CallK);
    if (t != NULL) {
        t->attr.name = tempid;
        match(LPAREN);
        t->child[0] = args_list();
        match(RPAREN);
    }
    //if (t == NULL) fprintf(listing, "no of call exp returns!!\n");
    return t;
}

TreeNode* args_list(void) {
    TreeNode* t = newExpNode(ArgsK);
    if (t == NULL)
        return NULL; //empty �����
    t->child[0] = args();
    return t;
}

TreeNode* args(void) {


    TreeNode* t = NULL;
    if (token == ID || token == NUM || token == LPAREN) {
        t = exp();
        // fprintf(listing, "args match exp>>>\n");
    }
    if (token == COMMA) {
        match(COMMA);
    }

    TreeNode* p = t;

    while (token == ID || token == NUM || token == LPAREN) {

        TreeNode* q = exp();
        if (q != NULL) {
            //fprintf(listing, "next exps>>>>>>>\n");
            if (t == NULL) t = p = q;
            else
            {
                p->sibling = q;
                p = q;
            }
        }
        if (token == COMMA) match(COMMA);

    }
    //}
       // if (t == NULL) fprintf(listing, "no of args returns!!\n");
    return t;
}

TreeNode* factor(char* id) {
    TreeNode* t = NULL;
    //char* tempid = NULL;
    //û�н�����һ��Ĳ��� ����ȡ��ǰ�Ĳ���
    if (id == NULL) {
        id = copyString(tokenString);

        // fprintf(listing, "id =%s>>>>>>>>factor>>\n", id)
        switch (token) {
        case NUM:
            t = newExpNode(ConstK);
            if (t != NULL)
                t->attr.val = atoi(tokenString);
            match(NUM);
            //fprintf(listing, "match NUM here>>>>>>>>>>>>\n");
            break;
        case ID:
            match(ID);
            //fprintf(listing, "match ID here>>>>>>>>>>>>\n");
            if (token == LPAREN) {
                t = call_exp(id);
                //no child in this layer
                //���ñ��ʽ
            }
            else {

                t = var_exp(id, 0, 1);
            }
            break;
        case LPAREN:
            match(LPAREN);
            t = exp();
            match(RPAREN);
            break;
        default:
            syntaxError("unexpected token -> ");
            printToken(token, tokenString);
            token = getToken();
            break;
        }
    }
    else {
        t = var_exp(id, 0, 1);

    }
    //if(t==NULL) fprintf(listing, "no of factor returns!!\n");
    return t;
}

TreeNode* select_stmt(void) {
    TreeNode* t = newStmtNode(IfK);
    match(IF);

    if (t != NULL)

    {
        match(LPAREN);

        t->child[0] = exp();

        match(RPAREN);

    }
    if (t != NULL)
    {
        t->child[1] = stmt();
    }
    if (token == ELSE)
    {
        match(ELSE);

        if (t != NULL)

        {
            t->child[2] = stmt();
        }
    }
    return t;

}

TreeNode* iter_stmt(void) {
    TreeNode* t = newStmtNode(WhileK);
    match(WHILE);
    match(LPAREN);
    if (t != NULL)

    {

        t->child[0] = exp();

    }
    match(RPAREN);

    if (t != NULL)

    {

        t->child[1] = stmt();

    }

    return t;
}

TreeNode* return_stmt(void) {
    TreeNode* t = newStmtNode(ReturnK);
    match(RETURN);
    if (t != NULL && token == SEMI) {
        match(SEMI);
    }
    else {
        if (t != NULL) {
            t->child[0] = exp();
            match(SEMI);
        }
    }
    return t;
}

TreeNode* input_stmt(void)

{

    TreeNode* t = newStmtNode(InputK);

    if (token == INPUT) {
        match(INPUT);
        match(LPAREN);
        match(RPAREN);

    }


    return t;

}



TreeNode* output_stmt(void)

{

    TreeNode* t = newStmtNode(OutputK);

    if (token == OUTPUT) {
        match(OUTPUT);
        match(LPAREN);
        //t->attr.val = simple_exp()->attr.val;
        match(RPAREN);

    }
    // if (t != NULL) t->child[0] = exp();

    return t;

}



/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode* parse(void)
{
    TreeNode* t;
    token = getToken();

    //fprintf(listing, "this is token string%s\n",tokenString);
    t = dec_list();
    if (t == NULL) fprintf(listing, "no tree of start!!\n");
    if (token != ENDFILE)
        syntaxError("Code ends before file\n");
    return t;
}

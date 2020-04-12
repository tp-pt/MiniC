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

//dec --声明语句
static TreeNode* input_stmt(void);
static TreeNode* output_stmt(void);

static TreeNode* dec_list(void);//声明列表
static TreeNode* dec(void);//声明
static TreeNode* var_dec(char* Type, char* id);//变量声明
static TreeNode* func_dec(char* Type, char* id);//函数声明

static TreeNode* param(void);//参数

static TreeNode* params_list(void);//参数列表

static TreeNode* compound_stmt(void);//复合语句


static TreeNode* local_dec(void);//局部声明
static TreeNode* stmt_list(void);//声明列表


static TreeNode* stmt(void);//声明种类

static TreeNode* exp_stmt(void);//表达式语句
static TreeNode* exp(void);//表达式
static TreeNode* simple_exp(char* id);//简单表达式
static TreeNode* var_exp(char* tempid, int tempindex, int arr);//变量


static TreeNode* select_stmt(void);//条件语句
static TreeNode* iter_stmt(void);//迭代语句
static TreeNode* return_stmt(void);//返回语句

static TreeNode* add_exp(char* id);//关系运算
static TreeNode* term(char* id);//算术运算+-
static TreeNode* factor(char* id);//算术运算*
static TreeNode* call_exp(char* tempid);//调用
static TreeNode* args_list(void);//参数列表
static TreeNode* args(void);//参数


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

//加一个program 开始parse 
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
        //fprintf(listing, "识别一个dec!\n");
        if (q != NULL) {
            if (t == NULL) t = p = q;
            else /* now p cannot be NULL either */
            {
                p->sibling = q;//下一个声明 作为兄弟节点
                p = q;//移到下一个声明节点，继续循环
            }
        }

    }

    //if (t == NULL) fprintf(listing, "no of dec list returns!!\n");
    return t;
}

//判断声明类型
TreeNode* dec(void) {
    TreeNode* t = NULL;
    char* tempType = NULL;//获取类型
    char* tempID = NULL;//获取id名称

    if (token == VOID)
    {
        tempType = copyString(tokenString);
        //strcpy(tempType, copyString(tokenString));//void 类型传递给 func_dec()
        match(VOID);

        tempID = copyString(tokenString);//ID传递给 下一个dec()
        match(ID);

        t = func_dec(tempType, tempID);

    }

    else if (token == INT) {

        tempType = copyString(tokenString);
        //fprintf(listing, "here is tempType%s\n",tokenString);
        //strcpy(tempType, copyString(tokenString));//void 类型传递给 func_dec()
        match(INT);
        //int a; 或 int a(params)
         //strcpy(tempID, copyString(tokenString));//ID传递给 下一个dec()
        tempID = copyString(tokenString);
        match(ID);

        if (token == SEMI || token == LSQUARE) {
            t = var_dec(tempType, tempID);//var-dec节点
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
//var_dec 作为单独的节点，没有子节点
TreeNode* var_dec(char* Type, char* id) {
    TreeNode* t = newDecNode(VarK);

    if ((t != NULL) && (id != NULL)) {
        t->attr.name = id;//名称
        t->type = Integer;//整型类型记录
    }
    //判断是否数组
    if (t != NULL && token == LSQUARE) {
        match(LSQUARE);//int a [num]
        t->mark = 1;
        //记录size 
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
            t->attr.name = id;//名称
            if (strcmp(Type, "int") == 0) {
                t->type = Integer;
                //默认初始化为 Void，可修改为Integer
            }
        }
        match(LPAREN);//匹配 (


        if (token == VOID) {
            match(VOID);
            //t->child[0] = NULL;//无参数列表
        }

        else if (token != VOID) {
            // fprintf(listing, "开始打印参数列表！\n");
            t->child[0] = params_list();
        }

        match(RPAREN);

        t->child[1] = compound_stmt();
        //类型记录
    }
    //两个子节点
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
    //写成递归形式 不用while
  // while (token != ENDFILE&&token!=ELSE) {
    TreeNode* q;

    //不止一个参数 参数列表为兄弟节点

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

        //匹配数组
        if (token == LSQUARE) {
            match(LSQUARE);
            match(RSQUARE);

            //arraysize 暂时不能确定
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
        //两个子节点
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
            //空节点情况
            if (tc1 != NULL) {
                t->child[0] = tc1;
            }
        }

        match(RBRACKET);//匹配 }
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


    //打印errors 


//1个或者多个var_dec 组成
/*
TreeNode* t = dec();//自行判断 var_dec
if (t == NULL) {
    fprintf(listing, "no of local dec returns!!\n");
    return t;   //empty的情况

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
        return t; //empty 的情况

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
//stmt 类型判断
TreeNode* stmt(void) {
    TreeNode* t = NULL;
    switch (token)
    {
        //exp 两种开头情况匹配 exp; (ID 开头) | ; (仅以分号开头匹配) 
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

//区分几种exp 赋值 | simple-exp
//exp -> var = exp | simple_exp
//ID NUM ( ;

TreeNode* exp(void) {

    TreeNode* t = NULL;
    char* tempID = NULL;
    int arr = 0;//设置数组标记
    int  tempindex = 0;//设置数组下标默认值 


    if (token == NUM) {
        t = simple_exp(tempID);
        // t->attr.val = 2;//取NUM 的值
    }
    else if (token == ID) {
        //判断有没有 = 符号 
        tempID = copyString(tokenString);
        match(ID);
        //数组  要求常量下标 
        if (token == LSQUARE) {
            arr = 1;//标记为数组
            match(LSQUARE);
            tempindex = exp()->attr.val;//获取表达式的值
            match(RSQUARE);

            // fprintf(listing, "match array<<< get here>>>>>>>>>>>>>\n");
        }

        //if (tempID==NULL) fprintf(listing, "no id get here>>>>>>>>>>>>>\n");
        //else {
        //    fprintf(listing, "tempid =%s\n", tempID);
        //}
        //修改节点 为 simple-eexp的 call_func() 
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
                    //记录数字
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
            //调用表达式
        }
        else if (token == LSQUARE){//考虑另一种 simple_exp()
            t = var_exp(tempID);
        }
       */
       //var | NUM 匹配完成
    }


    if (token == ASSIGN) {
        //t = var_exp(tempID);//提前匹配完
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
        //token还没有Match
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

//index 设置默认值
TreeNode* var_exp(char* tempId, int tempindex, int arr) {
    TreeNode* t = NULL;

    //id 已经在上一层match了
    //变量是一个数组元素
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
        // t->index = exp()->attr.val;//数组大小
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
        return NULL; //empty 的情况
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
    //没有接受上一层的参数 ，获取当前的参数
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
                //调用表达式
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

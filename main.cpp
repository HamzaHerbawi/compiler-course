
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include<string.h>

#include <fstream>

#define BSIZE 128
#define NONE -1
#define EOS '\0'

#define NUM 256
#define DIV 257
#define MOD 258
#define ID 259
#define DONE 260

#define PROGRAM 261
#define INFIX 262
#define POSTFIX 263
#define BEGIN 264
#define END 265

#define STRMAX 999 //size of lexe array
#define SYMMAX 100 //size symtable





int lookahead=0;


int tokenval =NONE ;
int lineno =1 ;


char lexbuf [BSIZE];
char lexemes[STRMAX];
int lastchar = -1;

int lastentry =0;

FILE* fp ;
FILE* fa ;

struct entry
{
    char *lexptr ;
    int token ;

};


struct entry symtable [SYMMAX];

struct entry keywords[] = {
        (char*)"program",PROGRAM,
        (char*)"infix",INFIX,
        (char*)"postfix",POSTFIX,
        (char*)"begin",BEGIN,
        (char*)"end",END,
        (char*)"div",DIV,
        (char*)"mod",MOD,
        0,0
};

int lookup(char s[]);
int insert (char s[],int tok);
void init();
void parse ();
void expr () ;
void term();
void factor();
void match(int t);
void emit (int t,int tval);
int lexan ();
void error(char *m);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]){
//    ifstream MyReadFile("new.txt");

    fp = fopen(argv[1],"r");
    fa = fopen(argv[2],"a");
    if(fp == NULL) {
        fprintf(fa,"Error in opening file");
        return (-1);
    }
   init();
    parse();
    fclose(fp);
    fclose(fa);

    exit(0);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void init(){
    struct entry *p;
    for (p = keywords ; p->token ; p++ ) {
       // printf ("C D: %c %d \n", lexbuf[1],p->token);
        insert(p->lexptr, p->token);
     //   printf ("Characters: %c ,Decimals: %d \n", p->lexptr,p->token);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int lookup(char s[]){


    int p;
    for ( p = lastentry ; p > 0 ; p=p-1) {
        if (strcmp(symtable[p].lexptr , s) == 0 )
            return p;
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int insert (char s[],int tok){

    int len;
    len = strlen(s);
    if(lastentry + 1 >= SYMMAX)
        error((char*)"symbol table full");
    if(lastchar + len + 1 >= STRMAX)
        error((char*)"lexemes array full");
    lastentry +=1 ;

    symtable[lastentry].token = tok;
    symtable[lastentry].lexptr = &lexemes[lastchar + 1];
    lastchar = lastchar + len +1;
    strcpy(symtable[lastentry].lexptr, s);

    return lastentry;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void parse () // parses and translates expression list
{
    lookahead = lexan () ;
    if(lookahead == PROGRAM){
        match(PROGRAM);
       emit(PROGRAM, tokenval);
    }
    else error((char*)"format file error");
    if(lookahead == ID) {

        match(ID);
        fprintf(fa," test");
        //emit(ID, tokenval);
    }
        else error((char*)"format file error");
    if (lookahead == '(') {
        match('(');
        fprintf(fa,"(");
       // emit('(', tokenval);
    }
        else error((char*)"format file error");
    if(lookahead == INFIX) {
        match(INFIX);
        emit(INFIX, tokenval);
    }
        else error((char*)"format file error");
    if(lookahead == ',') {
        match(',');
        fprintf(fa,",");
        //emit(',', tokenval);

    }
    else error((char*)"format file error");
    if (lookahead == POSTFIX) {
        match(POSTFIX);
        emit(POSTFIX, tokenval);
    }
        else error((char*)"format file error");
    if(lookahead == ')') {
        match(')');
        fprintf(fa,")");
        //emit(')', tokenval);
    }
    else error((char*)"format file error");
    if(lookahead == BEGIN) {
        match(BEGIN);
        fprintf(fa,"\n");
        emit(BEGIN, tokenval);
        fprintf(fa,"\n");
    }
        else error((char*)"format file erorr");

    while (lookahead != DONE) {


        /*   match(PROGRAM);
           match(ID);
           match('(');
           match(INFIX);
           match(',');
           match(POSTFIX);
           match(')');*/
        if(lookahead == END) {
            lookahead = DONE;
            emit(END, tokenval);
        }
       else{
                expr(); //?

                match(';');

            fprintf(fa,";\n");
       }

//        emit(PROGRAM, tokenval);

    }

   // emit(PROGRAM, tokenval);
}
/*
char opt_statements(){
    char * tempvar;
    if( match(END)) {
        return tempvar;
    }
  //  else return stmt_list();
}

/*
char stmt_list(){
    while(!match(END)){
        //advance();
        statements();
        /*	if( !match(SEMI)){
            advance();
                break;
            }*/
//    }
//    return NULL;
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void match(int t){

    if (lookahead == t) {
        lookahead = lexan();
    }
        else error((char*)"syntax error");
    }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void expr () {
    int t;
    term();
    while (1)
        switch (lookahead) {
            case '+' : case '-':
                t = lookahead;
                match(lookahead);
                term();
                emit(t, NONE);
                continue;
            default :
                return;
        }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void term() {

    int t;
    factor();
    while (1)
        switch (lookahead) {
            case '*' :
            case '/':
            case '%' :
            case '\\':
            case DIV :
            case MOD :
                t = lookahead;
                match(lookahead);
                factor();
                emit(t, NONE);
                continue;
            default:
                return;
        }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void factor(){

    switch (lookahead) {
        case '(':
            match('('); expr(); match(')'); break;
        case NUM :
            emit(NUM,tokenval); match(NUM); break;
        case ID:
            emit(ID, tokenval); match(ID); break;
//        case PROGRAM:
//            emit(PROGRAM, tokenval); match(PROGRAM); break;
        default:
            error((char*)"syntax error");
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void emit (int t,int tval){

    switch(t){
        case '+' :
        case '-' :
        case '*' :
        case '/' :
        case '%' :
        case '\\' :
            fprintf(fa,"%c ",t); break;
        case DIV :
            fprintf(fa,"DIV "); break;
        case MOD :
            fprintf(fa,"MOD "); break;
        case NUM :
            fprintf(fa,"%d ",tval); break;
        case ID :
            fprintf(fa,"%s ",symtable[tval].lexptr); break;
        case PROGRAM :
            fprintf(fa,"program"); break;
        case INFIX :
            fprintf(fa,"infix"); break;
        case POSTFIX :
            fprintf(fa,"postfix"); break;
        case BEGIN :
            fprintf(fa,"begin"); break;
        case END :
            fprintf(fa,"end"); break;
//       case PROGRAM :
//           printf("%s ",symtable[tval].lexptr); break;
//        case INFIX :
//            printf("infix "); break;
//        case POSTFIX :
//            printf("postfix "); break;
        default :
            fprintf(fa,"token %d, tokenval %d ", t ,tval);
    }

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void error(char *m){

    //printf("line %d: %s\n", lineno,m);

   fprintf(fa, "line %d: %s\n", lineno,m);
    exit(1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int lexan ()  // lexical analyzer
{


    int t ;

    while (1) {
        t = fgetc (fp);

        if (t== ' ' || t=='\t');

         // strip out white space

        else if (t=='\n')
            lineno = lineno + 1 ;

        else if (t=='#'){
            while (t != '\n' && t != DONE ){
               t= getchar();
            }
        }
        else if (isdigit(t))  {//t is a digit
            ungetc (t,stdin);
            scanf ("%d",&tokenval);
            return NUM;

        }

        else if (isalpha(t)) {// t is a letter

            int p, b = 0;
            while (isalnum(t)) {//t is alphanumeric
                lexbuf[b]=t;

                t = getchar();

                b = b + 1;
                if (b >= BSIZE)
                    error("compiler error");
            }

            lexbuf[b] = EOS;


            if (t != EOF)
                ungetc(t, stdin);

            p = lookup(lexbuf);

            if (p == 0)
                p = insert(lexbuf, ID);


            tokenval = p;

            return symtable[p].token;


        }

        else if (t == EOF) {
            return DONE;
        }
        else{
        tokenval = NONE;
        return t;
                }

    }
}

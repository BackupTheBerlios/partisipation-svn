/*
 * --blank-lines-after-declarations
 * -bad / -nbad
 */



char *foovar;
char *barvar;
/* This separates blocks of declarations.  */
int baz;



/*
 * --blank-lines-after-procedures
 * -bap / -nbap
 */



int
foo ()
{
  puts("Hi");
}
/* The procedure bar is even less interesting.  */
char *
bar ()
{
  puts("Hello");
}



/*
 * Comments
 * --format-all-comments
 * -fca / -nfca
 * 
 * --format-first-column-comments
 * -fc1 / -nfc1
 *
 * --comment-indentation
 * -c
 *
 * --declaration-comment-column
 * -cd
 * 
 * --else-endif-column
 * -cp 
 */



#if defined(_WIN32)
#include <windows.h>
#else // maybe linux
#include <string.h>
#endif // defined(_WIN32)

int l; // comment following declaration

void
some_proc ()
{
// comment in column 1
  foo(); // comment following short statement
  puts("This is just filling - ignore it!"); // comment following long line
  puts("This is middle sized"); // comment following line
  puts("Short line"); // comment following
  puts("Short line"); // extraordinary long comment to test line breaking and so on
}



/*
 * --blank-lines-before-block-comments
 * -bbb / -nbbb
 */



int x;
/**********************
 * Comment in a box!! *
 **********************/



/*
 * --swallow-optional-blank-lines
 * -sob / -nsob
 */



int y;


int z;



int zz;



/*
 * --comment-line-length
 * -lc 
 */



// comment: this is a very long line to provide a test for line breaking whitin comments



/*
 * --line-comments-indentation
 * -d
 */
 
 
 
char ch;
// comment after code
 
void
another_proc ()
{
  ch = 'a';
  // comment after code
  ch = 'b';
  if (ch == 'a') {
    ch = 'b';
    // some comment
  }
}



/*
 * --comment-delimiters-on-blank-lines
 * -cdb / -ncdb
 *
 * --start-left-side-of-comments
 * -sc / -nsc 
 */



/* Loving hug */



/*
 * --braces-after-if-line
 * -bl
 *
 * --braces-on-if-line
 * -br
 *
 * --brace-indent
 * -bli
 *
 * --cuddle-else
 * -ce / -nce
 */



void brace() { if (x > 0) { x--; } else { x++; } }



/*
 * --cuddle-do-while
 * -cdw / -ncdw
 */



void dowhile() { do { x--; } while (x); }



/*
 * --case-indentation
 * -cli
 * 
 * --case-brace-indentation
 * -cbi
 */



void cssw() { 
  int i; switch (i) { 
  case 0: break; 
  case 1: {  ++i; } 
  default: break; }
}



/*
 * --space-special-semicolon
 * -ss / -nss
 */



void forsem() {
  int i;
  for(i=0;i<10;i++);
  for(;;);
  while(i-->0);
}



/*
 * --space-after-procedure-calls
 * -pcs / -npcs
 */



void call() {
  puts
  ("Hi");
}



/*
 * --space-after-cast
 * -cs / -ncs
 */



void cast() {
  x=(int)1;
}



/*
 * --blank-before-sizeof
 * -bs
 */



void size() {
   x=sizeof(int);
}



/*
 * --space-after-for
 * -saf / -nsaf
 *
 * --space-after-if
 * -sai / -nsai
 *
 * --space-after-while
 * -saw / -nsaw
 */



void stdsp() {
  for(;;);
  if(1);
  while(1);
}



/*
 * --space-after-parentheses
 * -prs / -nprs
 */



void paren() {
  int e_code,s_code,dec_ind,bb_dec_ind;
  while((e_code-s_code)<(dec_ind-1)){
    set_buf_break(bb_dec_ind);
  }
}



/*
 * --declaration-indentation
 * -di
 *
 * --blank-lines-after-commas
 * -bc / -nbc
 */
 


int flag;
char *chr;
int
newline;
int a,b,c;



/*
 * --break-function-decl-args
 * -bfda / -nbfda
 *
 * --break-function-decl-args-end
 * -bfde / -nbfde
 *
 * --procnames-start-lines
 * -psl / -npsl
 */



void fool(int arg1, char arg2, int *arg3, long arg4, char arg5);

void fool(int arg1, char arg2, int *arg3, long arg4, char arg5) 
{
}



/*
 * --braces-on-struct-decl-line
 * -brs
 *
 * --braces-after-struct-decl-line
 * -bls
 *
 * --struct-brace-indentation
 * -sbi
 */



struct strct{int x;};



/*
 * --indent-level
 * -i
 */



void indent() {
  if (a != b) {
    if (b != c) {
      if (c != a) {
      }
    }
  }
}



/*
 * --continuation-indentation
 * -ci
 *
 * --continue-at-parentheses
 * -lp / -nlp
 */



void contind() {
  int p1, p2, p3, p4, p5, p6;
  p1 = first_procedure(second_procedure(p2, p3),third_procedure(p4, p5), fourth_procedure(p6));
}



/*
 * 
 * 
 */



void exoff() {
  int i, k, p, q, n;
  if (1) { if (1) { if (1) { if (1) { if (1) { if (1) { if (1) {
  if ((((i < 2 &&
  k > 0) || p == 0) &&
  q == 1) ||
  n - p - q - i - k + i + k + p == 0);}}}}}}}
}



/*
 * --parameter-indentation
 * -ip
 */



char * create_world (x, y, scale)
int x;
int y;
float scale;
{
}



/*
 * --use-tabs
 * -ut / -nut
 *
 * --tab-size
 * -t
 */



void tabs() {
	if (1) {
		puts("hi");
	}
}



/*
 * --preprocessor-indentation
 * -ppi
 */



#if X
#if Y
#define Z 1
#else
#define Z 0
#endif
#endif



/*
 * --leave-preprocessor-space
 * -lps / -nlps
 */



# if X
#  endif



/*
 * --break-before-boolean-operator
 * -bbo / -nbbo
 *
 * --honour-newlines
 * -hnl / -nhnl
 */



void breaking() {
  char* mask;
  if (1) { if (1) { if (1) { if (1) {
  if (mask && ((mask[0] == '\0') || (mask[1] == '\0' && ((mask[0] == '0') || (mask[0] == '*')))));
  if (mask
      && ((mask[0] == '\0')
      || (mask[1] == '\0' && ((mask[0] == '0') || (mask[0] == '*')))));
  }}}}
}

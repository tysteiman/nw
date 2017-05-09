#ifndef NW_LIB
#define NW_LIB

/* CONSTANTS */
#define TRUE 1
#define FALSE 0

#define NOCOLOR "\033[0m"
#define YELLOW "\033[93m"

/* DECLS */
void err        (char   * msg);
void logint     (char   * name, int    var);
void logstr     (char   * name, char * str);
int  fileExists (char   * path);
void colorStr (char * str, char * color);

#endif

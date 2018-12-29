
//////////////////////////////////////////
//////////////////////////////////////////
// NDESK for FreeBSD (GNU)              //
// Author: Spartrekus                   //
//////////////////////////////////////////
// Main //
//////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> 
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>  
#include <time.h>

//toupper
//tolower

////////////////////////////////////////////////
#if defined(__linux__) // LINUX
#define MYOS 1
#include <ncurses.h>

#elif defined(_WIN32)
#define MYOS 2
#define PATH_MAX 2500
#include <curses.h>
#elif defined(_WIN64)
#define MYOS 3
#define PATH_MAX 2500
#include <curses.h>
#include <dirent.h>  

#elif defined(__unix__) // BSD
#define MYOS 4 
#define PATH_MAX 2500
#include <ncurses.h>
#include <dirent.h>  

#else
#define MYOS 0
#endif



///  norm
#define KRED  "\x1B[31m"
#define KGRE  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KNRM  "\x1B[0m"
///  bright
#define KBGRE  "\x1B[92m"
#define KBYEL  "\x1B[93m"
/*
   printf("%s", KBYEL );
   printf( " => L%d: Detected section.\n", linecounter );
   printf("%s", KNRM );
*/


void fseek_filesize(const char *filename)
{
    FILE *fp = NULL;
    long off;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("failed to fopen %s\n", filename);
        exit(EXIT_FAILURE);
    }

    if (fseek(fp, 0, SEEK_END) == -1)
    {
        printf("failed to fseek %s\n", filename);
        exit(EXIT_FAILURE);
    }

    off = ftell( fp );
    if (off == (long)-1)
    {
        printf("failed to ftell %s\n", filename);
        exit(EXIT_FAILURE);
    }

    //printf("[*] fseek_filesize - file: %s, size: %ld\n", filename, off);
    printf("%ld\n", off);

    if (fclose(fp) != 0)
    {
        printf("failed to fclose %s\n", filename);
        exit(EXIT_FAILURE);
    }
}




int rows,cols; 
int ndesktop_gameover = 0;
int scr_x0,scr_y0,scr_x1,scr_y1;
int colorscheme = 1; 
int rows, cols ;
int  ndesk_statusbar_mode = 1;
char ndesk_statusbar[PATH_MAX];
char ndesk_status_message[PATH_MAX];
int nwin_clip_app = 1; 
int nwin_show[25];
int nwin_app[25];
char ndesk_clipboard[PATH_MAX];
char nwin_currentfile[PATH_MAX];
char nwin_currentpath[PATH_MAX];
int ndesk_show_statusbar = 1;
int ndesk_show_status_path = 0;
int ndesk_tinygrep_count = 0; 
int config_sound_card = 0;

char nwin_pile_file[20][PATH_MAX];
int   nwin_pile_app[20];
char nwin_pile_path[255][PATH_MAX];


///////////////////////////////////////////
#include "../libc/libc-key.c"
#include "../libc/libc-str.c"
#include "../libc/libc-string.c"
///////////////////////////////////////////







void listdirsearchfile( const char *name, int indent, char *searchitem )
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_type == DT_DIR) 
	{
            char path[1024];

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            snprintf( path, sizeof(path), "%s/%s", name, entry->d_name);

            listdirsearchfile( path, indent + 2, searchitem );
        } 
	else 
	{
	    if ( strstr( entry->d_name , searchitem ) != 0 ) 
	    {
              printf("%s/%s\n", name , entry->d_name );
	    }
        }
    }
    closedir(dir);
}








int fexist(char *a_option)
{
  char dir1[PATH_MAX]; 
  char *dir2;
  DIR *dip;
  strncpy( dir1 , "",  PATH_MAX  );
  strncpy( dir1 , a_option,  PATH_MAX  );

  struct stat st_buf; 
  int status; 
  int fileordir = 0 ; 

  status = stat ( dir1 , &st_buf);
  if (status != 0) {
    fileordir = 0;
  }

  // this is compatible to check if a file exists
  FILE *fp2check = fopen( dir1  ,"r");
  if( fp2check ) {
  // exists
  fileordir = 1; 
  fclose(fp2check);
  } 

  if (S_ISDIR (st_buf.st_mode)) {
    fileordir = 2; 
  }
return fileordir;
/////////////////////////////
}






///////////////////////////////////////////
char *strreadfile( char *thefile )
{
  FILE *source; 
  int ch ; 
  char ptrout[PATH_MAX]; 
  char ptrexport[PATH_MAX]; 
  if ( fexist( thefile ) == 1 )
  {  
   source = fopen( thefile , "r");
   while(  !feof(source) )
   {
       if (  !feof(source) )
           fgets( ptrout , PATH_MAX , source ); 
   }
   fclose(source);
  }
  strncpy( ptrexport , strrlf( ptrout ), PATH_MAX ); 
  size_t siz = sizeof ptrexport ; 
  char *r = malloc( sizeof ptrexport );
  return r ? memcpy(r, ptrexport , siz ) : NULL;
}




////////////////////////////////////////////////////////////////////
char *strremovelf(char *str) 
{     // copyleft, C function made by Spartrekus 
      char ptr[strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if (str[i] != '\n' && str[i] != '\n') 
        ptr[j++]=str[i];
      } 
      ptr[j]='\0';
      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
      //char *base = ptr;
      //return (base);
}




//////////////////
void ncp( char *filetarget,  char *  filesource )
{
  // fread
  char            buffer[1];
  size_t          n;
  size_t          m;
  FILE *fp;
  FILE *fp1; 
  FILE *fp2;
  int counter = 0 ; 
  int freader = 1 ; 
  int i , j ,posy, posx ; 
  if ( fexist( filesource ) == 1 )
  {
        fp = fopen( filesource, "rb");
        fp2 = fopen( filetarget, "wb");
        counter = 0; 
        while(  !feof(fp) && ( freader == 1)   ) {
           if (  feof(fp)   ) {
                freader = 0 ; 
            }
            n = fread(  buffer, sizeof(char), 1 , fp);
            m = fwrite( buffer, sizeof(char), 1,  fp2);
        }
        fclose(fp2);
        fclose(fp);
      }
}


void mvclear( )
{
    int j, i; 
    for ( j = 0 ; ( j <= cols -1  ) ; j++) 
     for ( i = 0; ( i <= rows -1  ) ; i++) 
        mvaddch( i , j , ' ');
}



void mvcenter( int myposypass, char *mytext )
{
      mvprintw( myposypass, cols/2 - strlen( mytext )/2  , "%s", mytext );
}





void gfxhline( int y1, int x1, int x2 )
{
    int foox ;
    for( foox = x1 ; foox <= x2 ; foox++) 
        mvaddch( y1 , foox , ' ' );
}




/////////////
void gfx_cls()  // workaround to clean up the screen in Win32
{
    getmaxyx( stdscr, rows, cols);
    attroff( A_REVERSE );
    attroff( A_BOLD );
    color_set( 0, NULL );
    int foo, fooy , foox ;
    int y1 = 0; int x1=0; 
    int y2 = rows-1; int x2= cols -1 ;
    foo = x1;
    for( fooy = y1 ; fooy <= y2 ; fooy++) 
    for( foox = x1 ; foox <= x2 ; foox++) 
    {
      attron( A_REVERSE );
      color_set( 7, NULL );
      mvprintw( fooy , foox , "A" );
      refresh();
      attroff( A_REVERSE );
      color_set( 0, NULL );
      mvprintw( fooy , foox , " " );
    }
}




void gfxrectangle( int y1, int x1, int y2, int x2 )
{
    int foo, fooy , foox ;
    foo = x1;
    for( fooy = y1 ; fooy <= y2 ; fooy++) 
     for( foox = x1 ; foox <= x2 ; foox++) 
      mvaddch( fooy , foox , ' ' );
}





///////////////////
void gfxbox( int y1, int x1, int y2, int x2 )
{
    int fooy, foox; 
    for( fooy = y1 ; fooy <= y2 ; fooy++) 
    for( foox = x1 ; foox <= x2 ; foox++) 
    {
         mvprintw( fooy , foox , " " );
    }
}





void gfxframe( int y1, int x1, int y2, int x2 )
{
    int foo, fooy , foox ;
    foo = x1;
    for( fooy = y1 ; fooy <= y2 ; fooy++) 
        mvaddch( fooy , foo , ACS_VLINE );
    foo = x2;
    for( fooy = y1 ; fooy <= y2 ; fooy++) 
         mvaddch( fooy , foo , ACS_VLINE );
    foo = y1;
    for( foox = x1 ; foox <= x2 ; foox++) 
         mvaddch( foo , foox , ACS_HLINE );
    foo = y2;
    for( foox = x1 ; foox <= x2 ; foox++) 
         mvaddch( foo , foox , ACS_HLINE );
    mvaddch( y1 , x1 , ACS_ULCORNER );
    mvaddch( y1 , x2 , ACS_URCORNER );
    mvaddch( y2 , x1 , ACS_LLCORNER );
    mvaddch( y2 , x2 , ACS_LRCORNER );
}






#include "editor.c"



char *binstrcut( char *str , int myposstart, int myposend )
{  
      char ptr[strlen(str)+1];
      int i,j=0;
      for(i=0; str[i]!='\0'; i++)
      {
        if ( ( str[i] != '\0' ) && ( str[i] != '\0') )
         if ( ( i >=  myposstart-1 ) && (  i <= myposend-1 ) )
           ptr[j++]=str[i];
      } 
      ptr[j]='\0';
      size_t siz = sizeof ptr ; 
      char *r = malloc( sizeof ptr );
      return r ? memcpy(r, ptr, siz ) : NULL;
}





static int compare_fun( const void *p, const void *q){
  const char *l = p ; 
  const char *r = q ; 
  int cmp; 
  cmp = strcmp( l, r );
  return cmp; 
}





///////////////// new
char *fextension(char *str)
{ 
    char ptr[strlen(str)+1];
    int i,j=0;
    //char ptrout[strlen(ptr)+1];  
    char ptrout[25];

    if ( strstr( str, "." ) != 0 )
    {
      for(i=strlen(str)-1 ; str[i] !='.' ; i--)
      {
        if ( str[i] != '.' ) 
            ptr[j++]=str[i];
      } 
      ptr[j]='\0';

      j = 0; 
      for( i=strlen(ptr)-1 ;  i >= 0 ; i--)
            ptrout[j++]=ptr[i];
      ptrout[j]='\0';
    }
    else
     ptrout[0]='\0';

    size_t siz = sizeof ptrout ; 
    char *r = malloc( sizeof ptrout );
    return r ? memcpy(r, ptrout, siz ) : NULL;
}




char *fbasename(char *name)
{
  char *base = name;
  while (*name)
    {
      if (*name++ == '/')
	{
	  base = name;
	}
    }
  return (base);
}





///////////////////////////////
///////////////////////////////
///////////////////////////////
char *strtimenow()
{
      long t;
      struct tm *ltime;
      time(&t);
      ltime=localtime(&t);
      char charo[50];  int fooi ; 
      fooi = snprintf( charo, 50 , 
	"%02d:%02d:%02d",
	ltime->tm_hour, ltime->tm_min, ltime->tm_sec 
	);

    size_t siz = sizeof charo ; 
    char *r = malloc( sizeof charo );
    return r ? memcpy(r, charo, siz ) : NULL;
}






///////////////////////////////
///////////////////////////////
///////////////////////////////
char *strdatenow()
{
    long t;
    struct tm *ltime;
    time(&t);
    ltime=localtime(&t);
    char charo[50];  int fooi ; 
    fooi = snprintf( charo, 50 , "%04d/%02d/%02d", 1900 + ltime->tm_year, ltime->tm_mon +1 , ltime->tm_mday );
    size_t siz = sizeof charo ; 
    char *r = malloc( sizeof charo );
    return r ? memcpy(r, charo, siz ) : NULL;
}



///////////////////////////////
///////////////////////////////
///////////////////////////////
char *strtimestamp()
{
      long t;
      struct tm *ltime;
      time(&t);
      ltime=localtime(&t);
      char charo[50];  int fooi ; 
      fooi = snprintf( charo, 50 , "%04d%02d%02d%02d%02d%02d",
	1900 + ltime->tm_year, ltime->tm_mon +1 , ltime->tm_mday, 
	ltime->tm_hour, ltime->tm_min, ltime->tm_sec 
	);
    size_t siz = sizeof charo ; 
    char *r = malloc( sizeof charo );
    return r ? memcpy(r, charo, siz ) : NULL;
}




///////////////////////////////
///////////////////////////////
///////////////////////////////
char *strfiletimestamp()
{
	long t;
	struct tm *ltime;
	time(&t);
        ltime=localtime(&t);
        char charo[50];  int fooi ; 
        fooi = snprintf( charo, 50 , 
	"%04d%02d%02d-%02d%02d%02d.iex",
	1900 + ltime->tm_year, ltime->tm_mon +1 , ltime->tm_mday, 
	ltime->tm_hour, ltime->tm_min, ltime->tm_sec 
	);
    size_t siz = sizeof charo ; 
    char *r = malloc( sizeof charo );
    return r ? memcpy(r, charo, siz ) : NULL;
}





//////////////////////////////
//////////////////////////////
//////////////////////////////
//////////////////////////////
char *fbasenoext(char* mystr)
{
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
         return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
}













//////////////////////////////
//////////////////////////////
//////////////////////////////
void makeunimark( char *thefile )
{
       char targetfile[PATH_MAX];
       char cmdi[PATH_MAX];
       def_prog_mode();
       endwin();

       strncpy( targetfile, fbasenoext( thefile ) , PATH_MAX );
       strncat( targetfile , ".tex" , PATH_MAX - strlen( targetfile ) -1 );

       strncpy( cmdi , " unibeam " , PATH_MAX );
       strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , thefile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , targetfile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
       system( cmdi );

       // strncpy( cmdi , " pdflatex -shell-escape --interaction=nonstopmode " , PATH_MAX );
       strncpy( cmdi , " pdflatex -shell-escape --interaction=nonstopmode " , PATH_MAX );
       strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , targetfile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       system( cmdi );

       strncpy( targetfile, fbasenoext( thefile ) , PATH_MAX );
       strncat( targetfile , ".pdf" , PATH_MAX - strlen( targetfile ) -1 );
       strncpy( cmdi , " screen -d -m  mupdf  " , PATH_MAX );
       strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , targetfile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       system( cmdi );
       reset_prog_mode();
}













//////////////////
//////////////////
//  TC FILECOPY
//////////////////
//////////////////
void tcfilecopy( char *filetarget,  char *  filesource )
{
  // fread
  char            buffer[1];
  size_t          n;
  size_t          m;
  FILE *fp;
  FILE *fp1; 
  FILE *fp2;
  int counter = 0 ; 
  int freader = 1 ; 
  int i , j ,posy, posx ; 
  if ( fexist( filesource ) == 1 )
  {
        fp = fopen( filesource, "rb");
        fp2 = fopen( filetarget, "wb");
        counter = 0; 
        while(  !feof(fp) && ( freader == 1)   ) {
           if (  feof(fp)   ) {
                freader = 0 ; 
            }
            n = fread(  buffer, sizeof(char), 1 , fp);
            m = fwrite( buffer, sizeof(char), 1,  fp2);
        }
        fclose(fp2);
        fclose(fp);
  }
}








////////////////////////////////
void nruncmd( char *thecmd   )
{
       char cmdi[PATH_MAX];
       def_prog_mode();
       endwin();
       strncpy( cmdi , "  " , PATH_MAX );
       strncat( cmdi , thecmd , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       system( cmdi );
       reset_prog_mode();
}




    int nwin_x1[25];
    int nwin_x2[25];
    int nwin_y1[25];
    int nwin_y2[25];
    int winsel = 1;






////////////////////////////////
////////////////////////////////
////////////////////////////////
////////////////////////////////
void ncurses_runwait( char *thecmd , char *thestrfile  ) 
{
       char cmdi[PATH_MAX];
       def_prog_mode();
       endwin();
       printf( "<APP-CMD: Command...>\n" );
       strncpy( cmdi , "  " , PATH_MAX );
       strncat( cmdi , thecmd , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , thestrfile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\" " , PATH_MAX - strlen( cmdi ) -1 );
       //strncat( cmdi , " ; read keypr " , PATH_MAX - strlen( cmdi ) -1 );
       printf( "<APP-CMD: %s>\n", cmdi );
       system( cmdi );

       printf( "<Press Return to Continue>\n" ); 
       getchar();

       reset_prog_mode();
}





////////////////////////////////
void ncurses_runcmd_keypress( char *thecmd   )
{
       char rncmdi[PATH_MAX];
       def_prog_mode();
       endwin();

       strncpy( rncmdi , "  " , PATH_MAX );
       strncat( rncmdi , thecmd , PATH_MAX - strlen( rncmdi ) -1 );
       strncat( rncmdi , " " , PATH_MAX - strlen( rncmdi ) -1 );
       printf( "<CMD START:%s>\n", rncmdi );
       system( rncmdi );

       printf( "<CMD DONE:%s>\n",  rncmdi );
       printf("%s", KBYEL );
       printf( "<Press Return Key To Continue>\n" );
       printf("%s", KNRM );

       getchar();
       reset_prog_mode();
}









////////////////////////////////
void ncurses_runcmd( char *thecmd   )
{
       char rncmdi[PATH_MAX];
       def_prog_mode();
       endwin();

       strncpy( rncmdi , "  " , PATH_MAX );
       strncat( rncmdi , thecmd , PATH_MAX - strlen( rncmdi ) -1 );
       strncat( rncmdi , " " , PATH_MAX - strlen( rncmdi ) -1 );
       printf( "<CMD START:%s>\n", rncmdi );
       system(  rncmdi );
       printf( "<CMD DONE:%s>\n",  rncmdi );
       reset_prog_mode();
}



////////////////////////////////
void ncurses_runwith( char *thecmd , char *thestrfile  ) //thecmd first
{
       char cmdi[PATH_MAX];
       def_prog_mode();
       endwin();
       printf( "<NDESK CMD: Command...>\n" );
       strncpy( cmdi , "  " , PATH_MAX );
       strncat( cmdi , thecmd , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , thestrfile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\" " , PATH_MAX - strlen( cmdi ) -1 );
       printf( "<NDESk CMD: %s>\n", cmdi );
       system( cmdi );
       reset_prog_mode();
}






void nstartncurses_color_2(  )
{
  initscr();	
  curs_set( 0 );
  noecho();            
  keypad( stdscr, TRUE );  // for F...
  start_color();
  init_pair(0,  COLOR_WHITE,     COLOR_BLACK);
  init_pair(1,  COLOR_RED,     COLOR_BLACK);
  init_pair(2,  COLOR_GREEN,   COLOR_BLACK);
  init_pair(3,  COLOR_YELLOW,  COLOR_BLACK);
  init_pair(4,  COLOR_BLUE,    COLOR_BLACK);
  init_pair(5,  COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6,  COLOR_CYAN,    COLOR_BLACK);
  init_pair(7,  COLOR_BLUE,    COLOR_WHITE);
  init_pair(8,  COLOR_WHITE,   COLOR_RED);
  init_pair(9,  COLOR_BLACK,   COLOR_GREEN);
  init_pair(11, COLOR_WHITE,   COLOR_BLUE);
  init_pair(12, COLOR_YELLOW,   COLOR_BLUE);
  init_pair(13, COLOR_BLACK,   COLOR_CYAN);
  init_pair(14, COLOR_BLUE,   COLOR_GREEN);
  init_pair(15, COLOR_BLUE, COLOR_CYAN );
  init_pair(16, COLOR_CYAN, COLOR_WHITE );
  init_pair(17, COLOR_CYAN, COLOR_YELLOW );
  init_pair(18, COLOR_CYAN, COLOR_BLUE);

  init_pair(10, COLOR_BLUE,  COLOR_YELLOW   );
  init_pair( 19, COLOR_MAGENTA , COLOR_BLUE);
  init_pair( 20 , COLOR_RED , COLOR_BLUE);
  init_pair( 21 , COLOR_BLUE , COLOR_YELLOW);
  init_pair( 22, COLOR_YELLOW,  COLOR_CYAN);
  init_pair( 23, COLOR_WHITE,   COLOR_BLUE);

  init_pair( 24, COLOR_GREEN,   COLOR_BLUE);
  init_pair( 25, COLOR_MAGENTA, COLOR_CYAN);
  init_pair( 26, COLOR_WHITE,   COLOR_GREEN);
}


void ncolor_reverse(  )  { attron( A_REVERSE ); }
void ncolor_white(  )    { color_set( COLOR_BLACK, NULL );}
void ncolor_cyan(  )     { color_set( COLOR_CYAN, NULL );}
void ncolor_blue(  )     { color_set( COLOR_BLUE, NULL );}
void ncolor_yellow(  )   { color_set( COLOR_YELLOW, NULL );}
void nccol_blue( ){ attron(A_REVERSE ); attroff(A_REVERSE ); color_set( 18 , NULL ); attron( A_BOLD ); }
void nc_color_default( ){ 
   attron(A_REVERSE ); attroff(A_REVERSE ); color_set( 18 , NULL ); attron( A_BOLD );
}
void nc_color_white( )
{ 
   attron(A_REVERSE ); attroff(A_REVERSE ); color_set( 18 , NULL ); attron( A_BOLD ); color_set( 7, NULL ); attroff( A_REVERSE );

   attron(A_REVERSE ); attroff(A_REVERSE ); color_set( 18 , NULL ); attron( A_BOLD ); color_set( 23, NULL );
    // 19 purple
}
void nccolorblack(){ color_set( 0 , NULL ); attroff( A_BOLD ); attroff( A_REVERSE ); }
void nccolorcyan(){ color_set( 6 , NULL ); attroff( A_BOLD ); attron( A_REVERSE ); }
void nccoloryellow(){ color_set( 22 , NULL ); attron( A_BOLD ); attroff( A_REVERSE ); }




void ncmenubar( int myposy  )
{
       int rows,i,cols; 
       getmaxyx( stdscr, rows, cols);
       nccolorcyan(); 
       for( i = 0; i <= cols-1;  i++)
         mvaddch( myposy, i ,  ' ' );

       nccoloryellow(); mvprintw( 0 , 0 , " F" );
       nccolorcyan(); printw( "ILE  " );
       nccoloryellow(); printw( " E" );
       nccolorcyan(); printw( "DIT  " );
       nccoloryellow(); printw( " S" );
       nccolorcyan(); printw( "CREEN  " );
       nccoloryellow(); printw( " A" );
       nccolorcyan(); printw( "PPLI  " );
       nccoloryellow(); printw( " T" );
       nccolorcyan(); printw( "OOL  " );

       nccoloryellow(); printw( "O" );
       nccolorcyan(); printw( "PTION" );

       nccoloryellow(); printw(" ?" );
       nccolorcyan(); printw( "HELP  " );
       nccolorblack();
}




#include "../libnc/libnc-graphic.c"
#include "../libnc/libnc-menu-msgbox.c"
#include "../libnc/libnc-time.c"
#include "../libnc/libnc-bin.c"



void colornorm(void)
{
    attroff( A_REVERSE ); attroff( A_BOLD ); color_set( 0, NULL );
}




//////////////////////////
//////////////////////////
//////////////////////////
//////////////////////////
void win_keypress()
{
  int win_key_gameover = 0;
  int ch = 0;
  while( win_key_gameover == 0)
  {
       erase();
       mvprintw( 0, 0, "Key Win Content" );
       mvprintw( 5,5, "%d", ch );
       ch = getch();
       if ( ch == 'i' ) 
           win_key_gameover = 1 ;
       else if ( ch == 'q' ) 
           win_key_gameover = 1 ;
  }
}




void crossgraphvga_colors(void)
{
  color_set( 7 , NULL ); attroff( A_REVERSE ); mvprintw( 2,2, "Darker  ;  Brighter (bold)" );
  
  getmaxyx( stdscr, rows, cols );

  int x = 5; int i = 1;
  for( i = 0; i <= rows-1 ; i++)  //ncurses as by default 8 colors
  { 
    mvprintw( x, 2 , "%d ", i );

    attron( A_REVERSE );
    attroff( A_BOLD );
    color_set( i , NULL );
    printw( "%d", i );
    colornorm(); printw( " " );

    attroff( A_REVERSE );
    attron(  A_BOLD );
    color_set( i , NULL );
    printw( " ; %d", i );
    colornorm(); printw( " " );

    // reverse and bold
    color_set( i , NULL );
    attron( A_REVERSE );
    attroff( A_BOLD ); //for the console
    attron(  A_BLINK ); //for the console
    printw( " R%d", i );
    colornorm(); printw( " " );


    color_set( i , NULL );
    attron(  A_REVERSE );
    attron(  A_BOLD ); //for the console
    attroff( A_BLINK ); //for the console
    printw( " Rev,Bold%d", i );
    colornorm(); printw( " " );

    color_set( i , NULL );
    attron(  A_REVERSE );
    attron(  A_BOLD ); //for the console
    attron(  A_BLINK ); //for the console
    printw( " Rev,Bold,Blink%d", i );
    colornorm(); printw( " " );

    x += 1; 
    colornorm(); printw( " " );
    color_set( 7 , NULL ); printw( " (#.%d)", i );

    mvprintw( rows-1, cols-8, "|%d,%d|" , rows, cols );
  } 

  refresh();
  x += 2; 
  attroff( A_REVERSE );
  color_set( 7, NULL ); mvprintw(  x, 5 , "<Press Key>" );

  // clean up
  attroff( A_REVERSE ); 
  attroff( A_BOLD ); 
  attroff( A_BLINK ); 
  color_set( 0, NULL ); 
}






////////////////////////
////////////////////////
////////////////////////
////////////////////////
char *strninput( int inputposyt , char *myinitstring )
{
   int strninput_gameover = 0; 
   char strmsg[PATH_MAX];
   char charo[PATH_MAX];
   strncpy( strmsg, myinitstring , PATH_MAX );

   int ch ;  int foo ; 
   int fooj; 
   //int inputposy = rows-1;
   int inputposy = inputposyt; 
   if ( inputposy == -1 ) inputposy = rows-1;
   while ( strninput_gameover == 0 )
   {
                  getmaxyx( stdscr, rows, cols);
                  attroff( A_REVERSE );

                  for ( fooj = 1 ; fooj <= cols-2;  fooj++) mvaddch( inputposy, fooj , ' ' );
                  mvprintw( inputposy, 1, ":> %s" , strrlf( strmsg ) );

                  attron( A_REVERSE );
                  printw( " " );
                  attroff( A_REVERSE );
                  attroff( A_REVERSE );
                  attroff( A_BOLD );
                  refresh() ; 

                  curs_set( 0 );
                  ch = getch();

  if ( ch == 8 ) 
    strncpy( strmsg, strcut( strmsg, 1 , strlen( strmsg ) -1 )  ,  PATH_MAX );

   else if ( ch == 20 ) strncpy( strmsg, strtimestamp() ,  PATH_MAX );  //ctrl+t


   else if ( ch == 22 ) // paste ctrl+v the clipboard - linux
     //strncat( strmsg ,  strtimestamp()  , PATH_MAX - strlen( strmsg ) -1 );  //ctrl+v for dateit
     strncat( strmsg ,  ndesk_clipboard  , PATH_MAX - strlen( strmsg ) -1 );  //ctrl+v for dateit

   else if ( ch == 4 ) 
   {
     //strncpy( strmsg, strtimestamp() ,  PATH_MAX );
     strncat( strmsg ,  strtimestamp()  , PATH_MAX - strlen( strmsg ) -1 );  //ctrl+d for dateit
   }

  else if ( ch == 2 ) strncpy( strmsg,  ""  ,  PATH_MAX );  // bsd

  else if ( ch == 27 )  
  {
    strncpy( strmsg, ""  ,  PATH_MAX );
    strninput_gameover = 1;
  }

  else if ( ch == 274 )  
  {
    strncpy( strmsg, ""  ,  PATH_MAX );
    strninput_gameover = 1;
  }

  else if ( ch == 263 )  
    strncpy( strmsg, strcut( strmsg, 1 , strlen( strmsg ) -1 )  ,  PATH_MAX );



		  else if ( ch == 4 ) 
		  {
			   strncpy( strmsg, ""  ,  PATH_MAX );
		  }
		  else if ( ch == 27 ) 
		  {
			   strncpy( strmsg, ""  ,  PATH_MAX );
		  }
	          else if (
			(( ch >= 'a' ) && ( ch <= 'z' ) ) 
		        || (( ch >= 'A' ) && ( ch <= 'Z' ) ) 
		        || (( ch >= '1' ) && ( ch <= '9' ) ) 
		        || (( ch == '0' ) ) 
		        || (( ch == '~' ) ) 
		        || (( ch == '!' ) ) 
		        || (( ch == '&' ) ) 
		        || (( ch == '=' ) ) 
		        || (( ch == ':' ) ) 
		        || (( ch == ';' ) ) 
		        || (( ch == '<' ) ) 
		        || (( ch == '>' ) ) 
		        || (( ch == ' ' ) ) 
		        || (( ch == '|' ) ) 
		        || (( ch == '#' ) ) 
		        || (( ch == '?' ) ) 
		        || (( ch == '+' ) ) 
		        || (( ch == '/' ) ) 
		        || (( ch == '\\' ) ) 
		        || (( ch == '.' ) ) 
		        || (( ch == '$' ) ) 
		        || (( ch == '%' ) ) 
		        || (( ch == '-' ) ) 
		        || (( ch == ',' ) ) 
		        || (( ch == '{' ) ) 
		        || (( ch == '}' ) ) 
		        || (( ch == '(' ) ) 
		        || (( ch == ')' ) ) 
		        || (( ch == ']' ) ) 
		        || (( ch == '[' ) ) 
		        || (( ch == '*' ) ) 
		        || (( ch == '"' ) ) 
		        || (( ch == '@' ) ) 
		        || (( ch == '-' ) ) 
		        || (( ch == '_' ) ) 
		        || (( ch == '^' ) ) 
		        || (( ch == '\'' ) ) 
	             ) 
		  {
                           foo = snprintf( charo, PATH_MAX , "%s%c",  strmsg, ch );
			   strncpy( strmsg,  charo ,  PATH_MAX );
		  }
		  else if ( ch == 10 ) 
		  {
                        strninput_gameover = 1;
		  }
     }
     char ptr[PATH_MAX];
     strncpy( ptr, strmsg, PATH_MAX );
     size_t siz = sizeof ptr ; 
     char *r = malloc( sizeof ptr );
     return r ? memcpy(r, ptr, siz ) : NULL;
}






////////////////////////
////////////////////////
////////////////////////
////////////////////////
char *strninputbox( int mystarty, char *myinitstring )
{
   int strninput_gameover = 0; 
   char strmsg[PATH_MAX];
   char charo[PATH_MAX];
   strncpy( strmsg, myinitstring , PATH_MAX );
   int ch ;  int foo ; 
   int fooj; 
   while ( strninput_gameover == 0 )
   {
                  getmaxyx( stdscr, rows, cols);
                  attroff( A_REVERSE );

                  gfxrectangle( mystarty-2 , 0 , mystarty+2, cols -1 );
                  gfxframe(     mystarty-2 , 0 , mystarty+2, cols -1 );

                  for ( fooj = 1 ; fooj <= cols-2;  fooj++)
                    mvaddch( mystarty , fooj , ' ' );

                  mvprintw( mystarty , 5, ":> %s" , strrlf( strmsg ) );
                  //attron( A_REVERSE );
                  //printw( " " );
                  //attroff( A_REVERSE );
                  //attroff( A_REVERSE );
                  //attroff( A_BOLD );
                  refresh() ; 
                  //curs_set( 0 );
                  ch = getch();
                  curs_set( 1 );
   
		  if ( ch == 8 )  // for ssh on linux
		  {
		      strncpy( strmsg, strcut( strmsg, 1 , strlen( strmsg ) -1 )  ,  PATH_MAX );
	 	  } 

		  else if ( ch == 263 )  
		  {
		      strncpy( strmsg, strcut( strmsg, 1 , strlen( strmsg ) -1 )  ,  PATH_MAX );
	 	  } 

	          else if (
			(( ch >= 'a' ) && ( ch <= 'z' ) ) 
		        || (( ch >= 'A' ) && ( ch <= 'Z' ) ) 
		        || (( ch >= '1' ) && ( ch <= '9' ) ) 
		        || (( ch == '0' ) ) 
		        || (( ch == '~' ) ) 
		        || (( ch == '!' ) ) 
		        || (( ch == '&' ) ) 
		        || (( ch == '=' ) ) 
		        || (( ch == ':' ) ) 
		        || (( ch == ';' ) ) 
		        || (( ch == '<' ) ) 
		        || (( ch == '>' ) ) 
		        || (( ch == ' ' ) ) 
		        || (( ch == '|' ) ) 
		        || (( ch == '#' ) ) 
		        || (( ch == '?' ) ) 
		        || (( ch == '+' ) ) 
		        || (( ch == '/' ) ) 
		        || (( ch == '\\' ) ) 
		        || (( ch == '.' ) ) 
		        || (( ch == '$' ) ) 
		        || (( ch == '%' ) ) 
		        || (( ch == '-' ) ) 
		        || (( ch == ',' ) ) 
		        || (( ch == '{' ) ) 
		        || (( ch == '}' ) ) 
		        || (( ch == '(' ) ) 
		        || (( ch == ')' ) ) 
		        || (( ch == ']' ) ) 
		        || (( ch == '[' ) ) 
		        || (( ch == '*' ) ) 
		        || (( ch == '"' ) ) 
		        || (( ch == '@' ) ) 
		        || (( ch == '-' ) ) 
		        || (( ch == '_' ) ) 
		        || (( ch == '^' ) ) 
		        || (( ch == '\'' ) ) 
	             ) 
		  {
                           foo = snprintf( charo, PATH_MAX , "%s%c",  strmsg, ch );
			   strncpy( strmsg,  charo ,  PATH_MAX );
		  }
		  else if ( ch == 10 ) 
		  {
                        strninput_gameover = 1;
		  }
     }
     char ptr[PATH_MAX];
     strncpy( ptr, strmsg, PATH_MAX );
     size_t siz = sizeof ptr ; 
     char *r = malloc( sizeof ptr );
     return r ? memcpy(r, ptr, siz ) : NULL;
}






/////////////////////
void tinynrclock()
{
   int tinynrclock_gameover = 0;
   int foo = winsel ;  int nrch; 
   while ( tinynrclock_gameover == 0)
   {
       curs_set( 0 );
       nodelay( stdscr, TRUE);
       usleep( 1e5 );
       color_set( 12, NULL );
       ncrectangle(   nwin_y1[foo], nwin_x1[foo], nwin_y2[foo], nwin_x2[foo] );
       ncframe( nwin_y1[foo], nwin_x1[foo], nwin_y2[foo], nwin_x2[foo] );
       mvprintw( nwin_y1[foo]+2, nwin_x1[foo]+2, "%s",  strtimenow());
       nrch = getch();
       if ( nrch == 'i')      tinynrclock_gameover = 1;
   }
   curs_set( 1 );
   nodelay( stdscr, FALSE);
}



void ndesk_screenlock_text()
{
     int fooch ;  int ch; 
     int posy = 5; int posx = 0; int posi = 0;
     char strmsg[PATH_MAX];  char charo[PATH_MAX];
     strncpy( strmsg, "", PATH_MAX );
     ch = 0; 
     int i;  int ctheme = 1;
     while( ( ch != KEY_F(10) )  && ( ch != 27 )) 
     {
        curs_set( 1 );
        if ( ctheme == 1)
        {   attroff( A_BOLD ); attron( A_REVERSE ); color_set( 0, NULL ); }
        else if ( ctheme == 2)
        {   attron( A_BOLD ); attron( A_REVERSE ); color_set( 10, NULL ); }
        else if ( ctheme == 3)
        {   attron( A_BOLD ); attron( A_REVERSE ); color_set( 15, NULL ); }
        erase();
        gfxbox( 0 , 0 , rows-1 , cols-1 ); 
        mvprintw( 0, 0, "=======" );
        mvprintw( 1, 0, "NOTEPAD" );
        mvprintw( 2, 0, "=======" );
        posy = 4; posx=0;
        for(i=0; strmsg[i]!='\0'; i++)
        {
          if( strmsg[i] == 10 )
          {
            posy++; posx=0;
            mvprintw( posy, posx, "");
          }
          else
          {
            mvprintw( posy, posx, "%c", strmsg[i] );
            posx++;
          }
        }
        move( posy, posx ); 
        ch = getch();
        attron( A_REVERSE ); color_set( 0, NULL );
        if ( ch == 263 )  
         strncpy( strmsg, strcut( strmsg, 1 , strlen( strmsg ) -1 )  ,  PATH_MAX );
        else if ( ch == KEY_F(8))
        {
           if      ( ctheme == 1 ) ctheme = 2; 
           else if ( ctheme == 2 ) ctheme = 3; 
           else if ( ctheme == 3 ) ctheme = 1; 
           else ctheme = 1;
        }
        else 
        {
          snprintf( charo, PATH_MAX , "%s%c",  strmsg, ch );
	  strncpy(  strmsg,  charo ,  PATH_MAX );
        }
     }
     curs_set( 1 );
     color_set( 0 , NULL );
     attroff( A_BOLD ); attron( A_REVERSE ); color_set( 0, NULL ); 
     strncpy( ndesk_clipboard, strmsg, PATH_MAX );
}


      ////////////////
        int ndesk_menu_select( char *p1, char *p2, char *p3, char *p4 , char *p5 , char *p6, char *p7)
        {
               int ndesk_menu_select_gameover = 0; 
               int fooch = 0; int foxiy = 2;  int itemnav = 1;

               while( ndesk_menu_select_gameover == 0)
               { foxiy = 2;
               if ( itemnav <= 1 ) itemnav = 1;
               color_set( 13, NULL ); attroff( A_REVERSE );
               gfxrectangle( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               gfxframe( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               color_set( 17, NULL ); attron( A_REVERSE );
               mvcenter( rows*25/100, "| Menu File |" );
               color_set( 13, NULL ); attroff( A_REVERSE );
               if ( itemnav == foxiy-1 ) attron( A_REVERSE ); else attroff( A_REVERSE );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "%s", p1); 
               if ( itemnav == foxiy-1 ) attron( A_REVERSE ); else attroff( A_REVERSE );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "%s", p2); 
               if ( itemnav == foxiy-1 ) attron( A_REVERSE ); else attroff( A_REVERSE );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "%s", p3); 
               if ( itemnav == foxiy-1 ) attron( A_REVERSE ); else attroff( A_REVERSE );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "%s", p4); 
               if ( itemnav == foxiy-1 ) attron( A_REVERSE ); else attroff( A_REVERSE );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "%s", p5); 
               if ( itemnav == foxiy-1 ) attron( A_REVERSE ); else attroff( A_REVERSE );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "%s", p6); 
               if ( itemnav == foxiy-1 ) attron( A_REVERSE ); else attroff( A_REVERSE );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "%s", p7); 
               color_set( 5, NULL ); attron( A_REVERSE );
               mvcenter( rows*75/100 -1, "[ Esc: Cancel ]" );
               fooch = getch();
               if      ( fooch == 'j' ) itemnav++;
               else if ( fooch == KEY_DOWN ) itemnav++;

               else if ( fooch == 'g' ) itemnav = 1;

               else if ( fooch == 'k' ) itemnav--;
               else if ( fooch == KEY_UP ) itemnav--;

               else if ( ( fooch == 'l' ) || ( fooch == 10 ) )
               {    ndesk_menu_select_gameover = 1; 
                    fooch = itemnav + 48;  
               }
               else 
                 ndesk_menu_select_gameover = 1;
 
               }
               return fooch ;  
        }
     





     ////////////////////////////
     ////////////////////////////
     ////////////////////////////
     void ndesk_help()
     {
             int foo;
             getmaxyx( stdscr, rows, cols);
             erase();
             color_set( 12, NULL ); attroff( A_REVERSE ); 
             color_set( 12, NULL ); attron( A_BOLD ); 
             ncrectangle( 0, 0, rows-1 , cols -1 );
             ncframe(     0, 0, rows-1 , cols -1 );
             foo = 1;
             mvprintw( foo++, 2, "======================" ); 
             mvprintw( foo++, 2, "-*  HELP INFO PAGE  *- " );
             mvprintw( foo++, 2, "======================" );
             mvprintw( foo++, 2, "Key Right,Left,...h,j,k,l,d,u: the Usual Arrows/PgUp/PgDn!");
             mvprintw( foo++, 2, "Key F10 or Q: Quit this NDESK application " );
             mvprintw( foo++, 2, "  "  );
             mvprintw( foo++, 2, "Key i: close selected frames");
             mvprintw( foo++, 2, "Key 1,2...9: active Window Frames");
             mvprintw( foo++, 2, "Key ':': the ndesk internal command line");
             mvprintw( foo++, 2, "Key o: open Dir (next winframe)");
             mvprintw( foo++, 2, "Key r: display File Content [Reader]");
             mvprintw( rows-1, 2, "<Press Key>");
             getch();
     }





///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
///////////////////////////////////////////////////
void ndesk_menu_beon( char *tmcurfile ) // super BEON User GUI menu
{
        int ndesk_menu_gameover = 0;
        int ch = 0; int fooquestion ; int i;  int kittux;
        while( ndesk_menu_gameover == 0)
        {
               attroff( A_BOLD ); attron( A_REVERSE ); color_set( 5, NULL);
               mvprintw( 0, 0, "FILE %s", tmcurfile );

               attroff( A_BOLD ); attroff( A_REVERSE ); 
               curs_set( 1 );
               color_set( 13, NULL ); attroff( A_REVERSE );
               gfxrectangle( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               gfxframe( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               color_set( 17, NULL ); attron( A_REVERSE );
               mvcenter( rows*25/100, "| Menu X11 BeON |" );
               color_set( 13, NULL ); attroff( A_REVERSE ); 
               kittux = 2;
               mvprintw( rows*25/100 +kittux++, cols*25/100+2, "?: HELP! " );
               mvprintw( rows*25/100 +kittux++, cols*25/100+2, "t: Unimark Grep file! " );
               mvprintw( rows*25/100 +kittux++, cols*25/100+2, "p or l: mplayer single/all files" );
               mvprintw( rows*25/100 +kittux++, cols*25/100+2, "x,0: Export to X11 :0 " );
               mvprintw( rows*25/100 +kittux++, cols*25/100+2, "1:   Export to X11 :1 " );
               mvprintw( rows*25/100 +kittux++, cols*25/100+2, "m:   nmenu (SVN)" );
               mvprintw( rows*25/100 +kittux++, cols*25/100+2, "i: Cancel" );
               color_set( 5, NULL ); attron( A_REVERSE );
               mvcenter( rows*75/100 -1, "[ Esc: Cancel ]" );

               ch = getch();
               if ( ch == 27 ) { ndesk_menu_gameover = 1; nwin_show[ winsel ]  = 1; }

               else if ( ch == 'l')
                  ncurses_runcmd( "   mplayer -fs -zoom * " );

               else if ( ch == 'm')
                  ncurses_runcmd( "   nmenu " );

               else if ( ch == 'p')
                  ncurses_runwith( "   mplayer -fs -zoom ",  tmcurfile );

               else if ( ch == 'v')
               {
                  ndesk_menu_gameover = 1; 
                  ncurses_runcmd(" mplayer -fs -zoom *" );
               }

               else if ( ch == 'g' )
               {   ndesk_menu_gameover = 1; nwin_show[ winsel ]  = 1;
                   nwin_app[winsel] = 23; 
               }

               else if ( ch == 't' )  
               {
                   nwin_app[winsel] = 14; 
                   ndesk_menu_gameover = 1;  
               }

               else if ( ch == 'i' ) ndesk_menu_gameover = 1;

               else if ( ch == '0' )
               { putenv( "DISPLAY=:0" );  ndesk_menu_gameover = 1; strncpy( ndesk_statusbar, "X11 Enabled on DISPLAY :0",  PATH_MAX);  }

               else if ( ch == '1' ) 
               { putenv( "DISPLAY=:1" );  ndesk_menu_gameover = 1; strncpy( ndesk_statusbar, "X11 Enabled on DISPLAY :1",  PATH_MAX);  }

               else if ( ch == 'n' ) ncurses_runcmd( " naclock " );
               else if ( ch == 'f' ) ncurses_runcmd( " iceweasel google.com " );
               else if ( ch == 'p' ) ncurses_runcmd( " fp " );

         }
}



/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
void ndesk_main_menu()
{
               attroff( A_BOLD ); attroff( A_REVERSE ); curs_set( 1 );
               color_set( 13, NULL ); attroff( A_REVERSE );
               gfxrectangle( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               gfxframe( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               color_set( 17, NULL ); attron( A_REVERSE );
               mvcenter( rows*25/100, "| Menu |" );
               color_set( 13, NULL ); attroff( A_REVERSE );
               mvprintw( rows*25/100 +3, cols*25/100+2, "<key>: Exit Menu" );
               color_set( 5, NULL ); attron( A_REVERSE );
               mvcenter( rows*75/100 -1, "[ Esc: Cancel ]" );
               getch();
}


void ndesk_menu_parameters()
{
        int ndesk_menu_gameover = 0;
        int ch = 0; int fooquestion ; int i; 
        while( ndesk_menu_gameover == 0)
        {
               attroff( A_BOLD ); attroff( A_REVERSE ); curs_set( 1 );
               color_set( 13, NULL ); attroff( A_REVERSE );
               gfxrectangle( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               gfxframe( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               color_set( 17, NULL ); attron( A_REVERSE );
               mvcenter( rows*25/100, "| Menu File |" );
               color_set( 13, NULL ); attroff( A_REVERSE );
               mvprintw( rows*25/100 +2, cols*25/100+2, "=: NDESK Bash Command" );
               mvprintw( rows*25/100 +3, cols*25/100+2, "q: NDESK Quit!" );
               color_set( 5, NULL ); attron( A_REVERSE );
               mvcenter( rows*75/100 -1, "[ Esc: Cancel ]" );

               ch = getch();
               if ( ch == 'i' )
                  ndesk_menu_gameover = 1;

               else if ( ch == '#' )
                  ndesk_menu_gameover = 1;

               else if ( ch == '=' )
               {
                    color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                    for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
                    for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
                    mvprintw( rows-2, 0, "[System Command Line]" ); 
                    ncurses_runcmd( strninput( -1, "" ) );
                    ndesk_menu_gameover = 1;
               }

               else if ( ch == 'q' )
               {
                  ncolor_white(  ); attroff( A_BOLD ); 
                  fooquestion = ncwin_question( "Really quit NDESKTOP?" );
                  if ( fooquestion == 1 ) ndesktop_gameover = 1;
                  if ( fooquestion == 1 ) ndesk_menu_gameover = 1;
               }
        }
}






void ndesk_menu_actions_compiler()
{
        int ndesk_menu_actions_compiler_gameover = 0;
        int ch = 0; int foxiy = 0; char targetfile[PATH_MAX];
        while( ndesk_menu_actions_compiler_gameover == 0)
        {
               foxiy = 2;
               color_set( 13, NULL ); attroff( A_REVERSE );
               gfxrectangle( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               gfxframe( rows*25/100, cols*25/100 , rows*75/100, cols*75/100 );
               color_set( 17, NULL ); attron( A_REVERSE );
               mvcenter( rows*25/100, "| Menu File |" );
               color_set( 13, NULL ); attroff( A_REVERSE );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "[Space]: Compile BMR (doc)" );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "[X]: tcxdotool (x11)" );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "[p]: Open PDF" );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "[w]: Close PDFs" );
               mvprintw( rows*25/100 +foxiy++, cols*25/100+2, "[c]: Compile BMR" );
               color_set( 5, NULL ); attron( A_REVERSE );
               mvcenter( rows*75/100 -1, "[ Esc: Cancel ]" );

               ch = getch();
               if ( ch == 27 )
                  ndesk_menu_actions_compiler_gameover = 1;

               else if ( ch == 'X' )
                    ncurses_runcmd( " tcxdotool  " ); 
               else if ( ch == 'x' )
                    ncurses_runcmd( "  tcxdotool   " ); 

               else if ( ch == 'i' )
                  ndesk_menu_actions_compiler_gameover = 1;

               else if ( ch == 'q' )
                  ndesk_menu_actions_compiler_gameover = 1;

               else if ( ch == 'w' )
                    ncurses_runcmd( "  pkill mupdf  " ); 

               else if ( ch == 'P' )
               {
                    if ( fexist( nwin_currentfile ) == 1 )
                    {
                      strncpy( targetfile, fbasenoext( nwin_currentfile ) , PATH_MAX );
                      strncat( targetfile , ".pdf" , PATH_MAX - strlen( targetfile ) -1 );
                      ncurses_runwith( "  mupdf ", targetfile );
                    }
               }

               else if ( ch == 'p' )
               {
                    if ( fexist( nwin_currentfile ) == 1 )
                    {
                      strncpy( targetfile, fbasenoext( nwin_currentfile ) , PATH_MAX );
                      strncat( targetfile , ".pdf" , PATH_MAX - strlen( targetfile ) -1 );
                      ncurses_runwith( "  screen -d -m mupdf ", targetfile );
                    }
               }

               else if ( ch == 'c' )
               {
	         if ( winsel >= 1 )
                 if ( fexist( nwin_currentfile ) == 1 )
                 if ( ( strcmp( fextension( nwin_currentfile ) , "bmr" ) == 0 ) || ( strcmp( fextension( nwin_currentfile ) ,  "mrk" ) == 0 )
                     || ( strcmp( fextension( nwin_currentfile ) ,  "txt" ) == 0 )
                     )
                    if ( fexist( nwin_currentfile ) == 1 ) 
                    {
                        ncurses_runcmd( " pkill mupdf " );
                        makeunimark( nwin_currentfile );
                    }
               }

               else if ( ch == 32 )
               {
	         if ( winsel >= 1 )
                 if ( fexist( nwin_currentfile ) == 1 )
                 if ( ( strcmp( fextension( nwin_currentfile ) , "bmr" ) == 0 ) || ( strcmp( fextension( nwin_currentfile ) ,  "mrk" ) == 0 )
                     || ( strcmp( fextension( nwin_currentfile ) ,  "txt" ) == 0 )
                     )
                    if ( fexist( nwin_currentfile ) == 1 ) 
                    {
                        ncurses_runcmd( " pkill mupdf " );
                        char mytargetfile[PATH_MAX];
                        strncpy( mytargetfile, getenv( "HOME" ) , PATH_MAX );
                        strncat( mytargetfile , "/" , PATH_MAX - strlen( mytargetfile ) - 1);
                        strncat( mytargetfile , "doc.mrk" , PATH_MAX - strlen( mytargetfile ) - 1);
                        ncp( mytargetfile , nwin_currentfile );
    	                nruncmd( " cd ; unibeam doc.mrk ; pdflatex   -shell-escape --interaction=nonstopmode    doc ; bibtex doc ; pdflatex   -shell-escape --interaction=nonstopmode     doc ; pdflatex    -shell-escape --interaction=nonstopmode     doc ; screen -d -m mupdf doc.pdf  " );
                        // strncpy( cmdi , " pdflatex    -shell-escape --interaction=nonstopmode    " , PATH_MAX );
                    }
               }
        }
}





void ndesk_move_frame( int mywin )
{
   int ndesk_move_frame_gameover = 0;  int fooch ; 
   while( ndesk_move_frame_gameover == 0 )
   { 
         getmaxyx( stdscr, rows, cols);
         erase();
         attroff( A_REVERSE ); attroff( A_BOLD ); color_set( 0, NULL );
         mvprintw( 0, 0, "|Move Frame: %d-%d-%d-%d|",  nwin_x1[mywin], nwin_x2[mywin], nwin_y1[mywin], nwin_y2[mywin] );
         attroff( A_REVERSE ); color_set( 11, NULL );
         gfxrectangle( nwin_y1[mywin], nwin_x1[mywin], nwin_y2[mywin], nwin_x2[mywin] );
         gfxframe(     nwin_y1[mywin], nwin_x1[mywin], nwin_y2[mywin], nwin_x2[mywin] );

         refresh();
         fooch = getch(); 
         switch( fooch ) 
         {
           case 'q':
           case 'w':
           case 'm':
           case 'i':
           case KEY_F(10):
           case 27:
            ndesk_move_frame_gameover = 1;
            break; 

           case 'd':
              nwin_y1[mywin]+=6;
              nwin_y2[mywin]+=6;
	      break;
           case 'u':
              nwin_y1[mywin]-=6;
              nwin_y2[mywin]-=6;
	      break;

           case 'L':
              nwin_x2[mywin]++;
	      break;
           case 'H':
              nwin_x2[mywin]--;
	      break;
           case 'J':
              nwin_y2[mywin]++;
	      break;
           case 'K':
              nwin_y2[mywin]--;
	      break;


           case 'l':
              nwin_x1[mywin]++;
              nwin_x2[mywin]++;
	      break;
           case 'h':
              nwin_x1[mywin]--;
              nwin_x2[mywin]--;
	      break;

           case 'j':
              nwin_y1[mywin]++;
              nwin_y2[mywin]++;
	      break;
           case 'k':
              nwin_y1[mywin]--;
              nwin_y2[mywin]--;
	      break;
        }
    }
}







///////////////////////////////////////////
void ncdisplaygrep( char *thefile )
{
  attroff( A_REVERSE );
  attroff( A_BOLD );
  color_set( 0, NULL );
  FILE *source; 
  FILE *ftarget; 
  int ch ; 
  char ptrout[PATH_MAX]; 
  char ptrexport[PATH_MAX]; 
  int posy = 0; 
  erase(); 
  if ( fexist( thefile ) == 1 )
  {  
   ftarget = fopen( "/tmp/grep" , "wb+" );
   fclose( ftarget );
   source = fopen( thefile , "r");
   mvprintw( posy++, 0, "|===FILE: %s===|", thefile );
   while(  !feof(source) )
   {
       fgets( ptrout , PATH_MAX , source ); 
       if (  !feof(source) )
       {
          if ( ptrout[0] == '!' ) 
          if ( ptrout[1] == '=' ) 
          if ( ptrout[2] == ' ' ) 
          {
             mvprintw( posy++, 0, "%s", strrlf( ptrout ) );
             ftarget = fopen( "/tmp/grep" , "ab+" );
             fputs( ptrout  , ftarget );
             fclose( ftarget );
          }
       }
   }
   fclose(source);
  }
  getch();
}






///////////////////////////////////////////
void ncdisplayfile( char *thefile )
{
  attroff( A_REVERSE );
  attroff( A_BOLD );
  color_set( 0, NULL );
  FILE *source; 
  int ch ; 
  char ptrout[PATH_MAX]; 
  char ptrexport[PATH_MAX]; 
  int posy = 0; 
  erase(); 
  if ( fexist( thefile ) == 1 )
  {  
   source = fopen( thefile , "r");
   mvprintw( posy++, 0, "|===FILE: %s===|", thefile );
   while(  !feof(source) )
   {
       fgets( ptrout , PATH_MAX , source ); 
       if (  !feof(source) )
       {
           mvprintw( posy++, 0, "%s", strrlf( ptrout ) );
       }
   }
   fclose(source);
  }
  getch();
}





void nappend_text( char *toutputfile , char *mytmpclip )
{
   FILE *fpoutput;
   fpoutput = fopen( toutputfile, "ab+" );
    fputs( "\n", fpoutput );
    fputs( fbasename( mytmpclip ) , fpoutput );
    fputs( "\n", fpoutput );
   fclose( fpoutput );
   strncpy( ndesk_status_message, "Appended clip to file.", PATH_MAX );
}




void vim_piles()
{
                   char cmdi[PATH_MAX];
                   strncpy( cmdi, " vim  -p " , PATH_MAX );

                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_pile_path[ 1 ]  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "/" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_pile_file[ 1 ]  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);

                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_pile_path[ 2 ]  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "/" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_pile_file[ 2 ]  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);

                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_pile_path[ 3 ]  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "/" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_pile_file[ 3 ]  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);

                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_pile_path[ 4 ]  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "/" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_pile_file[ 4 ]  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);

                   ncurses_runcmd(  cmdi );
}










/////////////////////////
/////////////////////////
int  nexp_user_sel = 1;
int  nexp_user_scrolly =0 ;
char nexp_user_fileselection[PATH_MAX]; 
int  tc_det_dir_type = 0;
/////////////////////////
void printdir()
{

   DIR *dirp; int posy = 0;  int posx, chr ; 
   int fooselection = 0;
   posy = 1; posx = cols/2;
   char cwd[PATH_MAX];
   struct dirent *dp;
   dirp = opendir( "." );
   attron( A_REVERSE );

   color_set( 7, NULL ); attron( A_REVERSE );
   gfxbox( 0, cols/2-2, rows-1, cols-1 );
   gfxframe( 0, cols/2-2, rows-1, cols-1 );

   mvprintw( 0, cols/2, "[DIR: %s]", getcwd( cwd, PATH_MAX ) );
   strncpy( nexp_user_fileselection, "", PATH_MAX );
   color_set( 11, NULL ); attron( A_REVERSE );
   int entrycounter = 0;
   while  ((dp = readdir( dirp )) != NULL ) 
   if ( posy <= rows-3 )
   {
        entrycounter++;
        color_set( 7, NULL ); attron( A_REVERSE ); attroff( A_BOLD );

        if ( entrycounter <= nexp_user_scrolly )
              continue;

        if (  dp->d_name[0] !=  '.' ) 
        if (  strcmp( dp->d_name, "." ) != 0 )
        if (  strcmp( dp->d_name, ".." ) != 0 )
        {
            posy++;  fooselection++;

            if ( dp->d_type == DT_DIR ) 
            {
                 color_set( 12 , NULL );
                 mvaddch( posy, posx++ , '/' );
            }
            else if ( dp->d_type == 0 )
            {
               if ( tc_det_dir_type == 1 )
               if ( fexist( dp->d_name ) == 2 )
               {
                 color_set( 12 , NULL );
                 mvaddch( posy, posx++ , '/' );
               }
            }

            if ( nexp_user_sel == fooselection ) 
            {
                  strncpy( nexp_user_fileselection, dp->d_name , PATH_MAX );
                  color_set( 6, NULL );
            }

            for ( chr = 0 ;  chr <= strlen(dp->d_name) ; chr++) 
            {
              if  ( dp->d_name[chr] == '\n' )
                  posx = cols/2;
              else if  ( dp->d_name[chr] == '\0' )
                  posx = cols/2;
              else
                 mvaddch( posy, posx++ , dp->d_name[chr] );
            }
        }
   }
   closedir( dirp );

   color_set( 7, NULL ); attroff( A_REVERSE );
   mvprintw( rows-1, cols/2, "[FILE: %s]", nexp_user_fileselection );
}








   ////////////////////////////////////
   void printfile( char *ttuxfile )
   {
       int posyy = 2 ;  
       int zui = 0 ;  FILE *fpsource;  char ptrout[PATH_MAX];
       color_set( 7, NULL );  attron( A_REVERSE );
       gfxbox( 0, cols/2-2, rows-1, cols-1 );
       gfxframe( 0, cols/2-2, rows-1, cols-1 );
           if ( fexist( ttuxfile ) == 1 )
           {  
             mvprintw( 0, cols/2, "[FILE: %s]", fbasename( ttuxfile ) );
             posyy = 2;
             fpsource = fopen( ttuxfile , "r");
             while(  !feof(fpsource) )
             {
                 if ( !feof(fpsource) )
                 {
                   fgets( ptrout , PATH_MAX , fpsource ); 
                   if ( posyy <= rows-3 )
                    for(zui=0; ptrout[zui]!='\0'; zui++)
                     if ( ptrout[zui] != '\n' )
                      if ( zui <=  cols/2 -2 )
                        mvprintw( posyy, cols/2 + zui , "%c", ptrout[zui] );
                   posyy++;
                 }
             }
             fclose(fpsource);
           }
   }


/////////////////////////////////////
void printfile_viewer( char *filex )
{
    char foostr[PATH_MAX];
    char foocwd[PATH_MAX];
    int ch ; 
                      strncpy( foostr , getcwd( foocwd, PATH_MAX ), PATH_MAX );
                      if ( fexist( filex ) == 1 )
                      {
                        printfile( filex ); 
                        ch = getch(); 
                        if       ( ch == 'v' )  ncurses_runwith( " vim " , filex );
                        else if  ( ch == 'n' )  ncurses_runwith( " screen -d -m nedit  " , filex );
                        else if  ( ch == 'r' )  ncurses_runwith( " tcview  " , filex );
                      }
                      chdir( foostr );
}





void ndesk_show_current_pwd( char *fileselection  )
{
     int i ; char foocwd[PATH_MAX];
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
                mvprintw( rows-2, 0, "[Application Command]" ); 
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                color_set( 11, NULL ); attroff( A_REVERSE );
                gfxrectangle( 0 , 0, 3, cols-1);
                gfxframe(     0 , 0, 3, cols-1);
     	        mvprintw( 1, 2, "PATH: %s", getcwd( foocwd, PATH_MAX ) );
     	        mvprintw( 2, 2, "FILE: %s", fileselection );
                color_set( 10, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                gfxhline( 0 , 0, cols-1);
                mvcenter( 0, "|Application Command|");
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                gfxhline( rows-1 , 0, cols-1);
}


////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
int main( int argc, char *argv[])
{

   int foochg = 0; 
   char thefile[PATH_MAX];
   char targetfile[PATH_MAX];
   char foolinetmp[PATH_MAX];
   int tutox = 0 ;
   for ( tutox = 1 ; tutox <= 19 ; tutox++)
   { 
      strncpy( nwin_pile_path[ tutox ]  , "/" , PATH_MAX);
      strncpy( nwin_pile_file[ tutox ]  , "/" , PATH_MAX);
      nwin_pile_app[ tutox ] = 1;
   } 

      ////////////////////////////////////////////////////////
      if ( argc == 2)
      if ( strcmp( argv[1] , "--time" ) ==  0 )
      {
           printf("%s\n", strtimestamp() );
           return 0;
      }



    DIR *curdir;
    struct dirent *entry;
    
    int targetwinframe = 0;

    strncpy( ndesk_statusbar, "", PATH_MAX );
    char pathbefore[PATH_MAX];
    char fileselection[PATH_MAX];
    char cwd[PATH_MAX];
    char foostr[PATH_MAX];
    int move_gameover = 0;
    char tcsys_filetarget[PATH_MAX];
    char tcsys_filesource[PATH_MAX];

    strncpy( ndesk_clipboard, "" , PATH_MAX);

    char clipboard_filecopy[PATH_MAX];
    char clipboard_filepath[PATH_MAX];
    strncpy( clipboard_filecopy, "", PATH_MAX );
    strncpy( clipboard_filepath, "", PATH_MAX );

    int foo; 
    char quickpath[17][PATH_MAX];
    for( foo = 1 ; foo <= 15 ; foo++)
       strncpy( quickpath[ foo ], "" , PATH_MAX );

    int tempscroll[20];
    int tempsel[20];
    for( foo = 1 ; foo <= 19 ; foo++) tempscroll[foo] = 0;
    for( foo = 1 ; foo <= 19 ; foo++) tempsel[foo] = 0;


    ////////////////////////////////////////////////////////
    if ( argc == 2)
      if ( strcmp( argv[1] , "" ) !=  0 )
       if ( fexist( argv[1] ) ==  2 )
       {
          chdir( argv[ 1 ] );
          strncpy( pathbefore , getcwd( cwd, PATH_MAX ) , PATH_MAX );
       }


    int ch; 
    int fooquestion; 
    int i, j , k ; 
    int boo;  int boosffilter = 0;
    int posx; 
    int posy; 

    char foocwd[PATH_MAX];
    char nwin_pathclip[PATH_MAX];
    char nwin_clip_filter[PATH_MAX];
    strncpy( nwin_pathclip, getcwd( foocwd , PATH_MAX),  PATH_MAX );
    strncpy( nwin_clip_filter, "", PATH_MAX );

    int foostrlength;
    char nc_file_filter[25][PATH_MAX];

    int nwin_file_sort[25];
    int nwin_fullscreen[25];
    int nwin_sel[25];
    int nwin_colorscheme[25];
    int nwin_sel_before[25];
    int nwin_scrolly[25];
    char nwin_file[25][PATH_MAX];
    char nwin_filter[25][PATH_MAX];
    char nwin_path[25][PATH_MAX];
    int nwinmode = 0; //fly
    int nwin_show_winpath = 0;
    strncpy( nwin_currentfile, "" , PATH_MAX);

    void ndesktop_tile_wins( ) 
    {
      for( foo = 1 ; foo <= 20 ; foo++)
      {
        nwin_x1[foo]= 2+foo;
        nwin_x2[foo]= 30+foo;
        nwin_y1[foo]= 2+foo;
        nwin_y2[foo]= rows-4;
      }
    }
    ndesktop_tile_wins( );


    for( foo = 1 ; foo <= 20 ; foo++)
    {
        nwin_show[foo]=0;
        nwin_fullscreen[foo]=0;
        nwin_app[foo] = 1; 
        nwin_file_sort[foo] = 1; 
        nwin_sel[foo]=1;
        nwin_colorscheme[foo]=1;
        nwin_sel_before[foo]=1;
        nwin_scrolly[foo]=0;
	strncpy( nwin_path[foo], getcwd( foocwd, PATH_MAX) , PATH_MAX);
	strncpy( nwin_file[foo], "", PATH_MAX);
	strncpy( nc_file_filter[foo], "", PATH_MAX);
	strncpy( nwin_filter[ foo ], "", PATH_MAX);
    }
        


    char line[PATH_MAX];
    char linetmp[PATH_MAX];
    char cmdi[PATH_MAX];
    char fooline[PATH_MAX];
    char filetarget[250];
    char idata[1240][250];
    strncpy( pathbefore , getcwd( cwd, PATH_MAX ) , PATH_MAX );
    unsigned filemax = 0;
    unsigned n=0;
    DIR *dirp;
    struct dirent *dp;
    int selection = 0 ; 
    int selectionmax = 0 ; 
    int scrolly=0; 
    int showhiddenfiles = 0;




   //////////////////////////
   //////////////////////////
   void ndesk_load_config()
   {
      FILE *fpread;
      char strread[PATH_MAX];
      char strreadt[PATH_MAX];
      char foochar[PATH_MAX];
      chdir( getenv("HOME" ) );
      fpread = fopen( ".ndesk.ini", "rb");
      for( foo = 1 ; foo <= 10 ; foo++)
      {
          fgets(strreadt, PATH_MAX , fpread ); 
          strncpy( strread, strrlf( strreadt ) , PATH_MAX );
          if (!feof(fpread))
          {
             strncpy( foochar , strsplit( strread , '\t', 1 ) , PATH_MAX);
             nwin_x1[foo] = atoi( foochar );
             strncpy( foochar , strsplit( strread , '\t', 2 ) , PATH_MAX);
             nwin_x2[foo] = atoi( foochar );
             strncpy( foochar , strsplit( strread , '\t', 3 ) , PATH_MAX);
             nwin_y1[foo] = atoi( foochar );
             strncpy( foochar , strsplit( strread , '\t', 4 ) , PATH_MAX);
             nwin_y2[foo] = atoi( foochar );
             strncpy( foochar , strsplit( strread , '\t', 5 ) , PATH_MAX);
             nwin_show[foo] = atoi( foochar );
             strncpy( foochar , strsplit( strread , '\t', 6 ) , PATH_MAX);
             nwin_app[foo] = atoi( foochar );
             strncpy( foochar , strsplit( strread , '\t', 7 ) , PATH_MAX);
             nwin_sel[foo] = atoi( foochar );
             strncpy( foochar , strsplit( strread , '\t', 8 ) , PATH_MAX);
             nwin_scrolly[foo] = atoi( foochar );

             strncpy( foochar , strsplit( strread , '\t', 9 ) , PATH_MAX);
             strncpy( nwin_path[foo] , foochar , PATH_MAX );

             strncpy( foochar , strsplit( strread ,'\t' , 10 ) , PATH_MAX);
             strncpy( nwin_file[foo] , foochar , PATH_MAX );

             strncpy( foochar , strsplit( strread ,'\t' , 11 ) , PATH_MAX);
             strncpy( nwin_filter[foo] , foochar , PATH_MAX );
         }
      }

      for( foo = 1 ; foo <= 10 ; foo++)
      {
          fgets(strreadt, PATH_MAX , fpread ); 
          strncpy( strread, strrlf( strreadt ) , PATH_MAX );
          if (!feof(fpread))
          {
             strncpy( foochar , strread , PATH_MAX);
             strncpy( quickpath[foo] , foochar , PATH_MAX );
          }
      }
      fclose( fpread );
   }





   //////////////////////////
   //////////////////////////
   void ndesk_save_config()
   {
    FILE *fpsaveout;
    char strcharo[PATH_MAX]; int tofu;
    chdir( getenv("HOME" ) );
    fpsaveout = fopen( ".ndesk.ini", "wb");
    for( foo = 1 ; foo <= 10 ; foo++)
    {

        i = snprintf( strcharo , 250 , "%d", nwin_x1[foo]); 
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , 250 , "%d", nwin_x2[foo]);
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , 250 , "%d", nwin_y1[foo]);
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , 250 , "%d", nwin_y2[foo]);
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , 250 , "%d", nwin_show[foo]); 
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , 250 , "%d", nwin_app[foo]);
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , 250 , "%d", nwin_sel[foo]);
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , 250 , "%d", nwin_scrolly[foo]); 
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , PATH_MAX , "%s", nwin_path[foo]); 
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , PATH_MAX , "%s", nwin_file[foo]); 
        fputs( strcharo, fpsaveout );
        fputs( "\t" , fpsaveout );

        i = snprintf( strcharo , PATH_MAX , "%s", nwin_filter[foo]); 
        fputs( strcharo, fpsaveout );
        fputs( "\n" , fpsaveout );
    }


    for( foo = 1 ; foo <= 10 ; foo++)
    {
        i = snprintf( strcharo , PATH_MAX , "%s", quickpath[foo]); 
        fputs( strcharo, fpsaveout );
        fputs( "\n" , fpsaveout );
    }

    fclose(fpsaveout);
   }





   ///////////////////////////////
   void ndesk_msgbox( char *msgstr1, char *msgstr2 )
   {
              color_set( 13, NULL ); attroff( A_REVERSE );
              gfxrectangle( rows/2-4 , 0 , rows/2+4, cols -1 );
              gfxframe( rows/2-4 , 0 , rows/2+4, cols -1 );
              mvprintw( rows/2-1 , 2, "%s", msgstr1 ); 
              mvprintw( rows/2   , 2, "" );
              mvprintw( rows/2+2 , 2, "%s", msgstr2 );
              curs_set( 0 ) ;
   }





   int ndesk_menu_choice( char *choice1, char *choice2)
   {
                   int ndesk_menu_choice_return; 
                   int ndesk_menu_gameover = 0; 
                   int ndesk_menu_mysel = 0; 
                   while( ndesk_menu_gameover == 0)
                   {
                   if ( ndesk_menu_mysel <= 1 ) ndesk_menu_mysel = 1;
                   if ( ndesk_menu_mysel >= 2 ) ndesk_menu_mysel = 2;
                   color_set( 13, NULL ); attroff( A_REVERSE );
                   gfxrectangle( rows/2-4 , 0 , rows/2+4, cols -1 );
                   gfxframe( rows/2-4 , 0 , rows/2+4, cols -1 );
                   if ( ndesk_menu_mysel == 1 ) attron( A_REVERSE );
                     mvprintw( rows/2-1 , 2, "%s" ,  choice1 );
                   color_set( 13, NULL ); attroff( A_REVERSE );
                   if ( ndesk_menu_mysel == 2 ) attron( A_REVERSE );
                     mvprintw( rows/2   , 2, "%s" ,  choice2 );
                   color_set( 13, NULL ); attroff( A_REVERSE );
                   mvprintw( rows/2+2 , 2, "-Choice?-" );
                   curs_set( 0 ) ; 
                   ch = getch();
                    if (( ch >= '1' )  && ( ch <= '9' ))
                    {
                       ndesk_menu_choice_return = ch; 
                       ndesk_menu_gameover = 1;
                    }
                    else if ( ch == 'j' )
                       ndesk_menu_mysel += 1;
                    else if ( ch == 'k' )
                       ndesk_menu_mysel -= 1;
                    else if ( ch == 'i' )
                       ndesk_menu_gameover = 1;
                    else if ( ch == 27 )
                       ndesk_menu_gameover = 1;
                    else if ( ch == 10 )
                    {
                      ndesk_menu_gameover = 1;
                      if ( ndesk_menu_mysel == 2 )  ch = '2';
                      if ( ndesk_menu_mysel == 1 )  ch = '1';
                      ndesk_menu_choice_return = ch ; 
                    }
                    else if ( ch == 'l' )
                    {
                      ndesk_menu_gameover = 1;
                      if ( ndesk_menu_mysel == 2 )  ch = '2';
                      if ( ndesk_menu_mysel == 1 )  ch = '1';
                      ndesk_menu_choice_return = ch ; 
                    }
                    else if ( ch == 13 )
                    {
                      ndesk_menu_gameover = 1;
                      if ( ndesk_menu_mysel == 2 )  ch = '2';
                      if ( ndesk_menu_mysel == 1 )  ch = '1';
                      ndesk_menu_choice_return = ch ; 
                    }
                   }
                   return ndesk_menu_choice_return; 
   }








   ///////////////////////////
   ///////////////////////////
   void ndesk_menu_quickpath()
   {
                   ch = ndesk_menu_choice( "1: Load Path", "2: Save Path" );
                   if ( ch == '2') 
                   { 
                     ndesk_msgbox( "Press Key (1...9)" , "[Choice?]");
                     ch = getch();
                     strncpy( quickpath[ ch -47-1], getcwd( cwd, PATH_MAX ), PATH_MAX );
                   }
                   else if ( ch == '1') 
                   { 
                      ndesk_msgbox( "Press Key (1...9)", "[Choice?]" );
                      ch = getch();  
                      chdir( quickpath[ ch -47-1]); 
        	      strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                      nwin_sel[winsel] = 1;
        	      nwin_scrolly[winsel] = 0;
        	      strncpy( nwin_filter[ winsel ], "", PATH_MAX);
                   }
   }





   ///////////////////////////
   ///////////////////////////
   ///////////////////////////
   ///////////////////////////
   ///////////////////////////
   ///////////////////////////
   void ndesk_menu_desktop()
   {
                   ch = ndesk_menu_choice( "1: Load Desktop", "2: Save Desktop" );
                   if ( ch == '2') 
                     ndesk_save_config();
                   else if ( ch == '1') 
                     ndesk_load_config(); 
   }







   //////////////////////////////////
   void loadlist( int thefootab )
   {
    int n = 0 ; 
    filemax = 0; 
    dirp = opendir( "." );
    strncpy( idata[ n++ ] , ".." , 250 );

    while  ((dp = readdir( dirp )) != NULL  &&  n < sizeof idata / sizeof idata[ 0 ]) {
            if ( strcmp( dp->d_name , "." ) != 0 )
            if ( ( showhiddenfiles == 0 ) && ( dp->d_name[0] != '.' ) )
            {
                if ( strcmp( nwin_filter[ thefootab  ] , "" ) == 0 )
                   strncpy( idata[ n++ ] , dp->d_name , 250 );
                else 
                 if ( strcmp( nwin_filter[ thefootab ] , "" ) != 0 )
                  if ( strstr( dp->d_name, nwin_filter[ thefootab ] ) != 0 )
                    strncpy( idata[ n++ ] , dp->d_name , 250 );

            }
    }
    filemax = n-1 ; 
    closedir( dirp );
    
    if ( nwin_file_sort[ thefootab ] == 1 )
     if ( n > 1 )
      qsort( idata, n , sizeof idata[0], compare_fun );

    //for( n = 0 ; n <= filemax ; n++)
       //printf( "%s\n" , idata[ n ] );

    selectionmax = filemax ;
    if ( selection >= selectionmax ) selection = 0;
   }







  int gamescrolly =0 ; int tc_show_hidden = 0;
  ///////////////////  LIST FILES (MVPRINTW)
  void mvlistprint(const char *name, int indent, char *searchitem )
  {
    /*
    int posy = nwin_y1[ foo ];
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    int entryselnb = 0;  int dircondition ; 
    unsigned int entrycounter = 0;
    while ((entry = readdir(dir)) != NULL) 
    {
        attron(  A_REVERSE ); //changed
        attroff( A_BOLD ); //changed

        entrycounter++;
        if ( entrycounter <= gamescrolly )
              continue;

        dircondition = 0; 
        if ( entry->d_type == DT_DIR ) 
           dircondition = 1; 

        else if ( entry->d_type == 0 ) 
          if ( posy <= rows-3 )
           if ( fexist( entry->d_name ) == 2 )
               dircondition = 1;

        if ( dircondition == 1 )
	{
            char path[1024];

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            if ( tc_show_hidden == 0 )
             if ( entry->d_name[0] ==  '.' ) 
                continue;

            //snprintf( path, sizeof(path), "%s/%s", name, entry->d_name);
            mvprintw( posy++, nwin_x1[ foo ]+2, "[%d] [%s]", entry->d_type, entry->d_name );

	    if ( strstr( entry->d_name , searchitem ) != 0 ) 
            if ( posy <= rows-3 )
            {
              entryselnb++;
              if ( nwin_sel[ foo ] == entryselnb ) attroff( A_REVERSE );
              //color_set( 6, NULL );
              //mvprintw( posy++, 0 +1, "[%d] [%s]", entry->d_type, entry->d_name );
              mvprintw( posy++, nwin_x1[ foo ]+2, "[%d] [%s]", entry->d_type, entry->d_name );
              if ( nwin_sel[ foo ] == entryselnb ) 
                  if ( strcmp( entry->d_name , "" ) != 0 )
                   strncpy( fileselection, entry->d_name , PATH_MAX );
             }

        } 
	else 
	{

            if ( tc_show_hidden == 0 )
              if ( entry->d_name[0] ==  '.' ) 
                continue;

	    if ( strstr( entry->d_name , searchitem ) != 0 ) 
	    {
              if ( posy <= rows-3 )
              {
                 entryselnb++;
                 if ( nwin_sel[ foo ] == entryselnb ) attron( A_REVERSE );
                 // color_set( 4, NULL );
                 //mvprintw( posy++, 0 +1, "[%d] %s", entry->d_type, entry->d_name );
                 mvprintw( posy++, nwin_x1[ foo ]+2, "[%d] [%s]", entry->d_type, entry->d_name );
                 if ( nwin_sel[ foo ] == entryselnb ) 
                  if ( strcmp( entry->d_name , "" ) != 0 )
                   strncpy( fileselection, entry->d_name , PATH_MAX );
              }
	    }
        }

    }
    closedir(dir);
    */
}







    nstartncurses_color_2();   
    getmaxyx( stdscr, rows , cols);
    ndesktop_tile_wins( ); // since rows might help
    
    void colorblack(){ color_set( 0 , NULL ); attroff( A_BOLD ); attroff( A_REVERSE ); }
    void colorcyan(){ color_set( 6 , NULL ); attroff( A_BOLD ); attron( A_REVERSE ); }
    void colorjaune(){    colorcyan(); color_set( 3, NULL ); }
    void colorblue(){ attroff( A_BOLD ); attroff( A_REVERSE); color_set( 11 , NULL ); }
    void colorbluebold(){ attron( A_BOLD ); attroff( A_REVERSE); color_set( 11 , NULL ); }
    void colorred(){ attron( A_BOLD ); attron( A_REVERSE); color_set( 20 , NULL ); }
    void coloryellow(){ color_set( 12 , NULL ); attron( A_BOLD ); attroff( A_REVERSE ); }
    void colormenuyellow(){ 
         color_set( 22 , NULL ); attron( A_BOLD ); attroff( A_REVERSE ); 
    }
    void colordefault(){ attron(A_REVERSE ); attroff(A_REVERSE ); color_set( 18 , NULL ); attron( A_BOLD ); }
    void colordefaultrev(){ attron(A_REVERSE ); attroff(A_REVERSE ); color_set( 18 , NULL ); attroff( A_BOLD ); }
//]]]

    ////////////////////////////////////////////////////////
    if ( argc == 2)
    if ( strcmp( argv[1] , "--os" ) ==  0 )
    {
          erase();
          mvprintw( 1, 1 , "MY OS: %d\n", MYOS );
          getch();
          curs_set( 1 ) ;
          attroff( A_BOLD ); attroff( A_REVERSE ); curs_set( 1 );
          endwin();		/* End curses mode  */
          return 0;
    }




//[[[ ndesktoppager
  ///////// 
  char getlinestring[PATH_MAX];
  char currentline[PATH_MAX];
  int ndesktoppager_show_basename = 0 ;
  int x1, x2, y1, y2;
  int ndesktoppager_view_tabselect = 0;
  char ndesktoppager_filesource[PATH_MAX];
  int ndesktoppager_linesel = 0;
  int ndesktoppager_scrolly = 0;
  int posxcounter = 0 ;
  int ndesktoppager_linemax = 0;
  int ndesktoppager_view_scrollx = 0;
  int ndesktoppager_view_selectline = 0;
  FILE *fp;


  void ndesktoppager_draw( char winselfoo )
  {
     strncpy( getlinestring, "", PATH_MAX );
     strncpy( currentline , "" , PATH_MAX );

     if ( ndesktoppager_view_tabselect < 0 )
        ndesktoppager_view_tabselect = 0 ;

     if ( ndesktoppager_view_selectline < 0 ) ndesktoppager_view_selectline = 0 ; 
     if ( ndesktoppager_scrolly <= 0 ) ndesktoppager_scrolly = 0;
     if ( ndesktoppager_linesel <= 0 ) ndesktoppager_linesel = 0;

         posxcounter = 0 ;
	 colorblue();
         ndesktoppager_linemax = 0;

          
	 posx = x1 ;
         if (  fexist( ndesktoppager_filesource ) == 1) 
         {
         fp = fopen( ndesktoppager_filesource , "rb" ) ; 
	    posy = y1 ; 
            posxcounter = 0 ;
            while (( posy <= y2 ) && (!feof(fp)) )  
            {

	       // gets full line normally
               fgets(linetmp, PATH_MAX , fp ); 
	       if ( ndesktoppager_show_basename == 1 )
	          strncpy( line , fbasename(linetmp),  PATH_MAX );
	       else
	          strncpy( line , linetmp,  PATH_MAX );
               ndesktoppager_linemax++;

               colorblue();
               if( nwin_colorscheme[ winselfoo ] == 2 )
	          nc_color_default();

           ///////////////////////
           if ( ndesktoppager_linemax >= ndesktoppager_scrolly ){
	    
            if ( ndesktoppager_view_selectline >= 1 )
            if (  ndesktoppager_linemax ==  ndesktoppager_view_selectline )
            if (!feof(fp))
	    {
	       strncpy( getlinestring, strrlf(line), PATH_MAX );
	       attron( A_REVERSE );
	    }

             if (!feof(fp))
	     for ( i = 0 ; ( i <= strlen( line )); i++)
	          { 
		    if ( posx - ndesktoppager_view_scrollx <= x2-1 )
		      if ( line[i] != '\r' ) 
		      if ( line[i] != '\n' ) 
		      {
                       if (  posxcounter >= ndesktoppager_view_scrollx )   

                       if ( nwin_colorscheme[winsel] == 2  ) 
                          if ( line[0] == '!' ) color_set( 12, NULL ); 

	               mvprintw( posy , posx - ndesktoppager_view_scrollx , "%c" , line[i] );
		       posx++;
                       posxcounter++;
		      }
		  }
		  posy++;
		  posx = x1;
                  posxcounter = 0 ;


            }
	   }
         fclose( fp );
	 }
         attroff( A_REVERSE );
         attroff( A_BOLD );
    }
//]]]



//[[[ NDEDIT
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
//
///////////////
void ndedit( int aoption , char *ndedit_filesourcetmp )
{
    char ndedit_filesource[PATH_MAX];
    strncpy( ndedit_filesource  , ndedit_filesourcetmp , PATH_MAX );
    int rows, cols ;
    int ndedit_file_changed = 0;
    int ndedit_view_selectline = 0 ;
    int ndedit_linemax = 0 ;
    int ndedit_view_scrollx = 0;
    int i , j , k , foo ; 
    getmaxyx( stdscr, rows , cols);
   
    if ( aoption == 2 ){
       i = rows;  
       rows = i *  2 / 3 -1;
    }

    FILE *fp;
    int found = 0 ;
    char cncfileopen[PATH_MAX];
    int linecounter = 0;
    int ndedit_gameover = 0;
    char cwd[PATH_MAX];
    char ndedit_pathbefore[PATH_MAX];
    char line[PATH_MAX];
    char ndedit_statusbar[PATH_MAX];
    strncpy( ndedit_statusbar, "", PATH_MAX);
    int ndedit_visualmode = 0;
    int ndedit_visualmode_keyfound = 0;
    int  ndedit_linesel = 0;
    int ndedit_color_cursor = 5;
    int posy = 0 ;
    int ndedit_colorscheme = 7;  // it was set to one
    int chlast = 0;
    curs_set( 0 ) ;
    int ndedit_viewonly = 0;
    int  posx = 1; 
    char ndedit_data[8000];
    int ndedit_datamax=0;
    int ndedit_datacursor=0;
    int freader ;

    int ndedit_scrolly = 0;



        //
        // fread
        //
        char            buffer[1];
        size_t          n;
        void ndedit_fileload( void ){
          freader = 1 ; 
          int counter ; 
            if ( fexist( ndedit_filesource ) == 1 ){
              fp = fopen( ndedit_filesource, "rb");
              ndedit_datamax = 1 ; 
              counter = 0 ; 
              while( !feof(fp) && ( freader == 1 ) ) {
                 n = fread(buffer, sizeof(char), 1 , fp);
                 counter = ndedit_datamax; 
                 ndedit_data[ ndedit_datamax++ ] = buffer[0]; 
              }
              fclose(fp);
            }
        }





    
    if ( ndedit_viewonly == 0 )
     if ( fexist( ndedit_filesource ) == 1 )
            ndedit_fileload( );









   /////////// draw
    void ndedit_draw( ){
         ndedit_linemax = 0 ;
         linecounter = 0;

         if ( ndedit_linesel < 0 )
            ndedit_linesel = 0;

         // background
         nc_color_default();
         for ( j = 0 ; ( j <= cols -1  ) ; j++) 
            for ( i = 0; ( i <= rows -1  ) ; i++) 
                mvaddch( i , j , ' ' ); 
		

         colormenuyellow();
	 nchspace( 0, 0, cols-1 );
         //if ( ndedit_visualmode == 1)
	 mvprintw( 0,0, "|NDESKTOP EDIT| " );
	 //else
	 //  mvprintw( 0,0, "ndedit: " );
	 printw( "%s", strcut( fbasename( ndedit_filesource ), 1,   cols-1 -10 ) );
	 mvprintw( 0, cols/2,  "Line %d", ndedit_datacursor );
	 printw( "   Sel %d", ndedit_view_selectline ); 
	 mvprintw( 0,cols - 4, "EOF " );

         colormenuyellow();
	 nchspace( rows-1, 0, cols-1 );
	 mvprintw( rows-1, 0, "|%s|", strcut( fbasename( ndedit_filesource ), 1,   cols-1 -10 ) );


         nc_color_default();

         ndedit_linemax = 0;
	 posx = 0;


        if ( ndedit_viewonly == 1 )   
         if (  fexist( ndedit_filesource ) == 1)
         {
            fp = fopen( ndedit_filesource , "rb" ) ; 
	    posy = 1 ; 
            while ( ( posy <= rows -2 ) && 
	    (!feof(fp) ) ) {
               fgets(line, PATH_MAX , fp ); 
               ndedit_linemax++;

	       if ( posy <= rows -2 )
	       if ( ndedit_linemax >= ndedit_linesel )
	       {
	        // foreground + text
	        nc_color_default();

                if ( posy <= rows )
		  for ( i = 0 ; ( i <= strlen( line )); i++)
	          { 
		     if ( posx <= cols -2 )
		     if ( line[i] != '\r' ) 
		       if ( line[i] != '\n' ) 
		       {

		        if ( posy <= rows -2 )
		         mvprintw( posy , posx , "%c" , line[i] );

			 posx++;
		       }
		  }
		  posy++;
		  posx = 0;
	       }
            }
            fclose( fp );
         }






       if ( ndedit_viewonly == 0 ){
          posy = 1;
          posx = 1; 
          ndedit_linemax = 0 ;

          for ( i = 1 ; ( i <= ndedit_datamax ) ; i++){


                  /*
                  if ( nwim_scrolly <= linecounter )
                  if ( ( nwim_data[ i ] == '\n' ) || ( nwim_data[ i ] == '\r' ))
                  {
		  */


                    if ( ( ndedit_data[ i ] == '\n' ) || ( ndedit_data[ i ] == '\r' ))
                        linecounter++;



                    if ( ( ndedit_data[ i ] == '\n' ) || ( ndedit_data[ i ] == '\r' ))
                    {

                          if ( i == ndedit_datacursor ){
                              //attroff( A_REVERSE); attron( A_BOLD); color_set( 7 , NULL );

         //              attron( A_REVERSE); attroff( A_BOLD); color_set( 12 , NULL );

         nc_color_default();

                              if ( ( ndedit_data[ i ] = '\n' ) || ( ndedit_data[ i ] = '\n' ) ){

                       if ( posy <= rows -2 )
                             mvprintw( posy , posx , "%c", ' ' );

                              }
                              else

		           {
			   

/////mvprint-it here !!
///// ndedit

       // select line
       if ( ndedit_view_selectline >= 1 )
       if (  ndedit_linemax ==  ndedit_view_selectline ) {
       }
			   
			   if ( posy <= rows -2 )
                               mvprintw( posy , posx+1 , "%c", ndedit_data[ i ] );
			   }
                          }

                          posy++;
                          posx = 1; 
                          ndedit_linemax++;

                     }
                    else 
                    {

                    if ( i == ndedit_datacursor ){
        //               attron( A_REVERSE); attroff( A_BOLD); color_set( 12 , NULL );
         nc_color_default();
                     }


       // select line - begin line color
       if ( ndedit_view_selectline >= 1 )
       if (  ndedit_linemax ==  ndedit_view_selectline ) {
       }  // end line color

                     if ( posy <= rows -2 )
                          mvprintw( posy , posx , "%c", ndedit_data[ i ] );
                          posx++;
                    }

           }


         }
    }






    void ndedit_addingchar( int chartoadd ){
             ndedit_datamax++;
             for ( j = ndedit_datamax ; ( j >= ndedit_datacursor  ) ; j--){
                ndedit_data[ j ] = ndedit_data[ j -1 ];
             }
             ndedit_data[ ndedit_datacursor ] = chartoadd ; 
             ndedit_datacursor++;
    }




    void ndedit_addsome( char *sometext ){
                          int foox; 
                          char cmdipaste[PATH_MAX];
                          char cmdipastetmp[PATH_MAX];
                          int foo2;
                          int k ; 
                          strncpy( cmdipaste, "" , PATH_MAX);
                          strncpy( cmdipastetmp, "" , PATH_MAX);
			  nc_color_default();
 

                          strncpy( cmdipaste, "" , PATH_MAX);
                       for ( foox = 0 ;  ( foox<= strlen( sometext ) -1 )  ;  foox++ ){
                                ndedit_addingchar(  sometext[foox] );
                       }
   }











    // while
    //nerase();
    for ( j = 0 ; ( j <= cols -1  ) ; j++) 
     for ( i = 0; ( i <= rows -1  ) ; i++) 
        mvaddch( i , j , ' ');

    while ( ndedit_gameover == 0){
       ndedit_draw( );

       curs_set( 0 );
       ch = getch();



      if ( ch != -1 )
      {
       curs_set( 1 ) ;

       // if visual mode is there or not - set to 0 before
       ndedit_visualmode_keyfound = 0;

       //VISUAL MODE
       if ( ndedit_visualmode == 1)
       switch( ch ){
          case 'k':
                 found = 0 ; 
                 for ( j = ndedit_datacursor-1 ;  ( j >= 0  ) ; j-- ){
                   if ( ndedit_data[ j ] == '\n' ) {
                     found = 1 ; 
                     ndedit_datacursor = j-1 ; 
                     break ; 
                    }
                 }
                 ndedit_visualmode_keyfound = 1;
                 break;

          case 'j':
                 found = 0 ; 
                 for ( j = ndedit_datacursor+1 ;  ( j <= ndedit_datamax  ) ; j++){
                   if ( ndedit_data[ j ] == '\n' ) {
                     found = 1 ; 
                     ndedit_datacursor = j+1 ; 
                     break ; 
                    }
                 }
                 ndedit_visualmode_keyfound = 1;
                 break;

            case KEY_GPAGEDOWN:
	      /*
              if ( nwim_scrolly <= linecounter )
              if ( ( nwim_data[ i ] == '\n' ) || ( nwim_data[ i ] == '\r' ))
              {
	      */
	      ndedit_scrolly += 1;
	      break; 
         /*
	 /// mega scrolling
         //// !!from here to here : scrolly 
	 //cnc_selection_activepanel_posy = posy ; 
	 if ( posy >= rows -1 ) ntodo2_scrolly +=1 ;
	 if ( posy <= 2 ) ntodo2_scrolly -=1 ;
	 ///// to here!!
	 */


	  case 'l':
            ndedit_datacursor += 1 ;
            ndedit_visualmode_keyfound = 1;
	    break;

	  case 'h':
            ndedit_datacursor -= 1 ;
            ndedit_visualmode_keyfound = 1;
	    break;


          case 'w':
                 found = 0 ; 
                 for ( j = ndedit_datacursor+1 ;  ( j <= ndedit_datamax  ) ; j++){
                   if (
		   ( ndedit_data[ j ] == ' ' )
                   || ( ndedit_data[ j ] == '.' )
                   || ( ndedit_data[ j ] == '\n' )
                   || ( ndedit_data[ j ] == ';' )
		   )
		   {
                     found = 1 ; 
                     ndedit_datacursor = j+1 ; 
                     break ; 
                    }
                 }
                 ndedit_visualmode_keyfound = 1;
                 break;

       }






       //EDIT MODE
       if ( (( ndedit_visualmode_keyfound == 0 ) && ( ndedit_visualmode == 1)) 
        || ( ndedit_visualmode == 0) )
       switch( ch ){

         case KEY_GFUNCF10:
            ncolor_white(  ); attroff( A_BOLD ); 
            fooquestion = ncwin_question( "Exit NDEDIT?" );
            if ( fooquestion == 1 ) 
	       ndedit_gameover = 1;
	       break;



          case KEY_UP:
                 found = 0 ; 
                 for ( j = ndedit_datacursor-1 ;  ( j >= 0  ) ; j-- ){
                   if ( ndedit_data[ j ] == '\n' ) {
                     found = 1 ; 
                     ndedit_datacursor = j-1 ; 
                     break ; 
                    }
                 }
                 break;
              
	    case KEY_DOWN:
                 found = 0 ; 
                 for ( j = ndedit_datacursor+1 ;  ( j <= ndedit_datamax  ) ; j++){
                   if ( ndedit_data[ j ] == '\n' ) {
                     found = 1 ; 
                     ndedit_datacursor = j+1 ; 
                     break ; 
                    }
                 }
                 break;


            case KEY_GCTRLB:
                 found = 0 ; 
                 for ( j = ndedit_datacursor+1 ;  ( j <= ndedit_datamax  ) ; j++){
                   if (
		   ( ndedit_data[ j ] == ' ' )
                   || ( ndedit_data[ j ] == '.' )
                   || ( ndedit_data[ j ] == '\n' )
                   || ( ndedit_data[ j ] == ';' )
		   )
		   {
                     found = 1 ; 
                     ndedit_datacursor = j+1 ; 
                     break ; 
                    }
                 }
                 break;






	 // case 'l':
	  case KEY_RIGHT:
            ndedit_datacursor += 1 ;
	    break;

	  //case 'h':
	  case KEY_LEFT:
            ndedit_datacursor -= 1 ;
	    break;




	  case KEY_GPAGEDOWN:
            ndedit_linesel += 4 ;
	    break;
	  case KEY_GPAGEUP:
            ndedit_linesel -= 4 ;
	    break;


        default:
              ndedit_file_changed =1 ;
              if ( 1 == 1 ){
                        {
                            ndedit_datamax++;
                            for ( j = ndedit_datamax ; ( j >= ndedit_datacursor  ) ; j--){
                               ndedit_data[ j ] = ndedit_data[ j -1 ];
                            }

                            switch ( ch ){
                              case 10:
                                 ndedit_data[ ndedit_datacursor ] = '\n' ; 
                                 break;
                              default:
                                 ndedit_data[ ndedit_datacursor ] = ch ; 
                                 break;
                            }
                            ndedit_datacursor++;
                        }
                        chlast = ch ; 
                }
              break; 


            case KEY_GBACKSPACE:
              if ( ndedit_datacursor >= 1 ){
                   ndedit_datacursor--;
                   for ( j = ndedit_datacursor ;  ( j <= ndedit_datamax  ) ; j++){
                      ndedit_data[ j ] = ndedit_data[ j +1 ] ;
                   }
                   ndedit_datamax--;
               }
               break;


         }
      }
    }

    // done
    // clean up and restore the colors
    curs_set( 1 ) ;
    attroff( A_BOLD );
    attroff( A_REVERSE );
}





 int mycursor = 0 ; 
 char charo[PATH_MAX];
 char foochar[PATH_MAX];
 int mycolor = 2 ; 
 int scrollcurx=0;
 int mycolorreverse=0;

 void loadfile(  char *filein ){
  int fetchi;
  FILE *fp5;
  FILE *fp6;
  char fetchline[PATH_MAX];
  char fetchlinetmp[PATH_MAX];
  int posread = 0;

  int i ; 
  for(i=0; i <= PATH_MAX-1; i++)
    cwd[ i ] = ' ';

  /////////////////////////////////////////////////
  if ( fexist( filein ) == 1 )
  {
    fp6 = fopen( filein , "rb");
    while( !feof(fp6) ) 
    {
          strncpy( fetchline, "" , PATH_MAX );
          fgets(fetchlinetmp, PATH_MAX, fp6); 

          if ( !feof(fp6) ) 
           for( fetchi = 0 ; ( fetchi <= strlen( fetchlinetmp ) ); fetchi++ )
            //if ( fetchlinetmp[ fetchi ] != '\n' )
            if ( fetchlinetmp[ fetchi ] != '\0' )
	    {
              //fetchline[fetchi]= fetchlinetmp[fetchi];
	      cwd[ posread++ ] = fetchlinetmp[ fetchi ];
	      mycursor = posread ; 
	    }

     }
     fclose( fp6 );
     mycursor = 0 ; 
   }
  }







void ndesktop_ncateditor(  int caty1, int catx1, int caty2, int catx2, char *myeditfile ){
    int thegameover = 0;
    strncpy( cwd , "  " , PATH_MAX );
    void nedit_strtofile( char *str )
    {  
      FILE *fpout;
      char txtout[40];
      fpout = fopen( myeditfile , "wb+");
      fclose( fpout );
      int return_strcount = 0;
      char ptr[strlen(str)+1];
      int i,j = 0;
      fpout = fopen( myeditfile , "ab+");
      for(i=0;  i<= 400 ; i++)
      {
         snprintf( txtout  , 10 , "%c", str[i] );
         fputs(    txtout  , fpout );
      } 
      fclose( fpout );
    }

    loadfile( myeditfile );
    mycursor = 0; 
    attroff( A_BOLD );
    while( thegameover == 0 ) 
    {	
                if ( mycolorreverse <= 0 ) mycolorreverse = 0 ; 
                if ( mycolor <= 0 ) mycolor = 0 ; 
                if ( mycursor <= 0 ) mycursor = 0 ; 
                if ( scrollcurx <= 0 ) scrollcurx = 0 ; 

                attron( A_REVERSE ); color_set( COLOR_BLUE, NULL ); 
                attron( A_REVERSE ); color_set( COLOR_BLACK, NULL ); 
                attron( A_REVERSE ); color_set( COLOR_BLACK, NULL ); 
                attron( A_REVERSE ); color_set( COLOR_BLACK, NULL ); 
                attroff( A_REVERSE ); color_set( COLOR_BLACK, NULL ); 
                attron( A_REVERSE ); color_set( COLOR_BLUE, NULL ); 
                color_set( mycolor, NULL );
                if ( mycolorreverse ==1 ) attron( A_REVERSE ); else attroff( A_REVERSE ); 
                //ncerase();
                ncolor_white(  );
                attroff( A_REVERSE ); 

                colorblue();
                ncrectangle( caty1, catx1, caty2, catx2 );

		posy = caty1+1;  posx = catx1+1; 
                for(i= scrollcurx ; cwd[i]!='\0'; i++)
                {
		    attroff( A_REVERSE );
                    color_set( mycolor, NULL );
                    colorblue();

		    if ( mycursor == i  ) attron( A_REVERSE );
		    if ( posx <= catx2  ) 
		     if ( posy <= caty2-1 ) 
		       if ( cwd[ i ] != '\n' ) 
		        mvaddch( posy, posx, cwd[i] );

		    if ( posx <= catx2  ) if ( posy <= caty2-1 ) 
		    if ( cwd[ i ] == '\n' ) 
		    {  
		      //mvaddch(posy,posx,' ');  
		      posy++; posx = catx1+1; }
		    posx++;
                } 
                
                // bottom bar
		attroff( A_BOLD ); attron( A_REVERSE ); color_set( 0, NULL);
                for(i=0; i <= cols-1; i++)
		  mvaddch( rows-1, i , ' ');
		mvprintw( rows-1, 0, "|F2:SAVE" );
		printw( " F10:ABORT|" );

                ncmenubar(  0 );

		attroff( A_REVERSE );
	        ch = getch();
                switch( ch )
		{
                 default:
                 case 10:
                  if ( ch == 10 ) ch = '\n';
                  strncpy( foochar ,  "" , PATH_MAX);
                  strncpy( foochar ,  string_cut( cwd , 1 , mycursor ) , PATH_MAX);
                  i = snprintf( charo , 3 , "%c", ch );
                  strncat( foochar , charo , PATH_MAX - strlen(  foochar ) - 1);
                  strncat( foochar , string_cut( cwd , mycursor+1  , strlen( cwd ) ) , PATH_MAX - strlen( foochar ) - 1);
                  strncpy( cwd ,  foochar , PATH_MAX);
                  mycursor++;
                  break;

      case KEY_BACKSPACE:
         strncpy( foochar ,  "" , PATH_MAX);
         strncpy( foochar ,  string_cut( cwd , 1 , mycursor -1 ) , PATH_MAX);
         strncat( foochar , string_cut( cwd , mycursor+1  , strlen( cwd ) ) , PATH_MAX - strlen( foochar ) - 1);
         strncpy( cwd , foochar , PATH_MAX);
         mycursor--;
         break; 

                 	case KEY_GFUNCF10:
                         thegameover = 1 ;
			 break;


                 	/* case KEY_GFUNCF5:
			 attroff( A_REVERSE );
			 color_set( 0, NULL );
			 ncerase();
                         nedit_strtofile( cwd );
			 ncruncmd( " gcc  -lm -o file  file.c   ;   ./file  ; ncpresskey  " );
			 break; */

                    case KEY_GFUNCF1:
                         loadfile( myeditfile );
			 break;

                     case KEY_GCTRLG:
                         mycursor = 1;
                         scrollcurx = 0;
			 break;

                     case KEY_GFUNCF2:
                         fooquestion = ncwin_questionbar( "Save file (unstable,beta)?" );
			 if ( fooquestion ==  1 )   nedit_strtofile( cwd );
			 break;

                 	case KEY_GRIGHT:
                         mycursor++;
			 break;


            case KEY_GPAGEUP:
            posy = strcharseekback( cwd , '\n' , mycursor-1 );
	    mycursor = posy-1;
            scrollcurx = mycursor;
            posy = strcharseekback( cwd , '\n' , mycursor-1 );
	    mycursor = posy-1;
            scrollcurx = mycursor;
            posy = strcharseekback( cwd , '\n' , mycursor-1 );
	    mycursor = posy-1;
            scrollcurx = mycursor;
	    break;

                   case KEY_GPAGEDOWN:
                    i = strcharseek( cwd , '\n' , mycursor );
                    mycursor = ++i;
                    scrollcurx = mycursor;
                    i = strcharseek( cwd , '\n' , mycursor );
                    mycursor = ++i;
                    scrollcurx = mycursor;
                    i = strcharseek( cwd , '\n' , mycursor );
                    mycursor = ++i;
                    scrollcurx = mycursor;
		    break;

              case KEY_GCTRLL:
                color_set( mycolor, NULL );
                attroff( A_REVERSE ); 
                getmaxyx( stdscr, rows, cols);
                ncerase();
		break;


            case KEY_GUP:
            posy = strcharseekback( cwd , '\n' , mycursor-1 );
	    mycursor = posy-1;
	    break;

                   case KEY_GDOWN:
                    i = strcharseek( cwd , '\n' , mycursor );
                    mycursor = ++i;
		    break;

                 	case KEY_GLEFT:
                         mycursor--;
			 break;


                 	case KEY_GFUNCF11:
                         scrollcurx--;
			 break;
                 	case KEY_GFUNCF12:
                         scrollcurx++;
			 break;


		}
	}
   }
   //]]]


 



  ///////////////////
  void ndesktop_onscreen()
  {
    if ( ndesk_show_statusbar == 1 )
    {
       colormenuyellow();
       nchspace( 0, 0, cols-1 );
       mvprintw( 0, 0, "|NDESK (GNU)|FreeBSD|" );
    }

    color_set( 12, NULL );
    mvaddch( rows-1, cols-1, ACS_CKBOARD );
    move( rows-1, cols-1 );
    attroff( A_REVERSE );
    attroff( A_BLINK );
    attroff( A_BOLD );
    attroff( A_UNDERLINE );
    color_set( 0, NULL);

    ///////////////////////////////////////////////////////////
    if ( ndesk_statusbar_mode == 0 )
    {
      strncpy( foostr , "", PATH_MAX);
    }
    else if ( ndesk_statusbar_mode == 1 )
    {
      strncpy( foostr , getcwd( cwd , PATH_MAX) , PATH_MAX);
      strncat( foostr , " (Path)" , PATH_MAX - strlen(foostr) - 1);
    }
    else if ( ndesk_statusbar_mode == 2 )
    {
      strncpy( foostr , nwin_currentfile , PATH_MAX);
      strncat( foostr , " (File)" , PATH_MAX - strlen(foostr) - 1);
    }
    else if ( ndesk_statusbar_mode == 3 )
    {
      strncpy( foostr , fextension( nwin_currentfile ) , PATH_MAX);
      strncat( foostr , " (Ext.)" , PATH_MAX - strlen(foostr) - 1);
    }
    else if ( ndesk_statusbar_mode == 4 )
    {
      foo = snprintf( foostr , PATH_MAX , "%d",  fexist( nwin_currentfile ) );
      strncat( foostr , " (fexist)" , PATH_MAX - strlen(foostr) - 1);
    }
    else if ( ndesk_statusbar_mode == 5 )
    {
      foo = snprintf( foostr , PATH_MAX , "%d",  fexist( nwin_currentfile ) );
      strncat( foostr , " (fexist)" , PATH_MAX - strlen(foostr) - 1);
    }
    //////////////////////////////

    for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i, ' ');
      mvprintw( rows-2, 0, "%s",  foostr ) ;
    move( rows-1, cols-1 );
  }





  void ndesktop_showcurfile()
  {
     	        chdir( nwin_path[ winsel ] );
                strncpy( fileselection, nwin_currentfile , PATH_MAX );
                color_set( 11, NULL ); attroff( A_REVERSE );
     	        mvprintw( 0, 0, "PATH: %s", getcwd( foocwd, PATH_MAX ) );
     	        mvprintw( 1, 0, "FILE: %s", fileselection );
  }








  /////////////////////////////////////////
  void ndesktop_statusbar()
  {
    int dodux = 0;
    colormenuyellow();
    for (dodux = 0; dodux<=cols-1 ; dodux++) 
         mvaddch( rows-1, dodux, ' ' );

    mvprintw( rows-1, cols-1 -10, "%s", strtimenow() );
    mvprintw( rows-1,0, "|%d|" , winsel );
    printw(  "[%d]" , nwin_app[winsel]);

    if ( nwin_app[winsel] == 1 )
       printw( "[Tree]" );
    else if ( nwin_app[winsel] == 2 )
       printw( "[Reader]" );
    else if ( nwin_app[winsel] == 23 )
       printw( "[Listdir]" );

    color_set( 22 , NULL ); attron( A_BOLD ); attroff( A_REVERSE ); 
    printw( "[%s]", strcut( fbasename( getcwd( cwd, PATH_MAX) ),  1 , cols-1 -15 ) );
    printw( "  ");

  /*
    ////////////////
    color_set( 22 , NULL ); 
    printw( "[B:" );
    color_set( 25 , NULL ); 
    printw( "B" );
    color_set( 22 , NULL ); 
    printw( "oom]" );
  */

    ////////////////
    color_set( 22 , NULL ); 
    printw( "[F9:" );
    color_set( 25 , NULL ); 
    printw( "M" );
    color_set( 22 , NULL ); 
    printw( "enu]" );
    ////////////////
    color_set( 22 , NULL ); 
    printw( "[F10:" );
    color_set( 25 , NULL ); 
    printw( "Q" );
    color_set( 22 , NULL ); 
    printw( "uit]" );

    //////////////////////////////////////
    color_set( 22 , NULL ); attron( A_BOLD ); attroff( A_REVERSE ); 
    if ( winsel >= 1 )
    if ( nwin_app[ winsel ] == 2 ) 
    if ( strcmp( nwin_file[ winsel ], "" ) != 0 ) 
      printw( "[%s]", strcut( fbasename( nwin_file[ winsel ] ),  1 , cols-1 -15 ) );

    //////////////////////////////////////
    if ( nwin_show_winpath == 2 ) { color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD ); }

    if ( nwin_show_winpath >= 1 )      nchspace( rows-2, 0, cols-1 );

    if      ( nwin_show_winpath == 1 )    mvprintw( rows-2, 0, "|%d|[%s]", winsel, nwin_currentfile ) ;
    else if ( nwin_show_winpath == 2 )    mvprintw( rows-2, 0, "|%d|[%s]", winsel, getcwd(cwd, PATH_MAX) ) ;
  }






   void ndesktop_dirdefault()
   {
       nwin_scrolly[winsel] = 0;
       nwin_sel[winsel] = 1;
       if ( winsel >= 1 )
       {
	  strncpy( nc_file_filter[ winsel ] , "", PATH_MAX );      
	  strncpy( nwin_filter[ winsel ]    , "", PATH_MAX );      
       }
    }




   void ndesktop_quickdir()
   {
                colormenuyellow();
                if ( ndesk_show_statusbar == 1 ) 
                mvprintw( rows-1, cols-3, "[o]" );
                mvprintw( rows-1, 0, "                           " );
                mvprintw( rows-1, 0, "[Quickdir]" );
	        ch = getch();
                if      ( ch == 32 ) chdir( getenv("HOME" ));
                else if ( ch == 's' ) {  chdir( getenv("HOME" )); chdir( "sshfs" ); }
                else if ( ch == 'd' ) {  chdir( getenv("HOME" )); chdir( "Documents" ); }
                else if ( ch == 't' ) {  chdir( getenv("HOME" )); chdir( ".ndesk" ); chdir( "quickdir"); chdir( "hdd" ); chdir( "docs" );  chdir( "teaching" ); }
                //else if ( ch == 'w' ) {  chdir( getenv("HOME" )); chdir( ".ndesk" ); chdir( "quickdir"); chdir( "workspace" ); }
                else if ( ch == 'w' ) {  chdir( getenv("HOME" ));   chdir( "workspace" ); }

                else if ( ch == 'm' ) {  chdir( getenv("HOME" )); chdir( ".ndesk" ); chdir( "quickdir"); chdir( "malette" ); }
                else if ( ch == 'p' ) {  chdir( getenv("HOME" )); chdir( ".ndesk" ); chdir( "quickdir"); chdir( "pim" ); }

                //else if ( ch == 'p' ) {  chdir( getenv("HOME" )); chdir( ".ndesk" ); chdir( "quickdir"); chdir( "hdd" ); chdir( "docs" );  chdir( "publishing" ); }
                else if ( ch == 'l' ) {  chdir( getenv("HOME" )); chdir( ".ndesk" ); chdir( "quickdir"); chdir( "hdd" ); chdir( "docs" );  chdir( "library" ); }
                strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                ndesktop_dirdefault();
   }










   void sys_create_file( char *foofile )
   {
          FILE *fpnew;
          //           colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
          //           mvprintw( 1, 0 , "%s",  nwin_currentfile );
          //           strncpy( fooline , ncwin_inputboxim( "MKFILE", strtimestamp() ), PATH_MAX );
          if ( strcmp(  foofile  , "" ) != 0 )
          {
                 fpnew = fopen( foofile , "ab+" );
                     fputs( "\n", fpnew );
                 fclose( fpnew );
         }
    }


   void nc_mkfile()
   {
          FILE *fpnew;
	  {

                     colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
                     mvprintw( 1, 0 , "%s",  nwin_currentfile );
                     strncpy( fooline , ncwin_inputboxim( "MKFILE", strtimestamp() ), PATH_MAX );
                     if ( strcmp(  fooline  , "" ) != 0 )
                     {
                          fpnew = fopen( fooline, "ab+" );
                          fputs( "\n", fpnew );
                          fclose( fpnew );
                     }
          }
    }



   //////////////////
   void nc_mkdir()
   {
	  {

                     colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
                     mvprintw( 1, 0 , "%s",  nwin_currentfile );
                     strncpy( fooline , ncwin_inputboxim( "MKDIR", "" ), PATH_MAX );
                     if ( strcmp(  fooline  , "" ) != 0 )
                     {
                    strncpy( cmdi, " mkdir -p " , PATH_MAX );
                    strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                    strncat( cmdi ,  fooline , PATH_MAX - strlen(cmdi) - 1);
                    strncat( cmdi , "\"  " , PATH_MAX - strlen(cmdi) - 1);
                    strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                      ncruncmd( cmdi );
                     }
          }
    }





   //////////////////
   //////////////////
   //////////////////
   //////////////////////////////////////////////
   //////////////////////////////////////////////
   void tc_file_copy( char *dirtrg, char *dirsrc )
   {
                      strncpy( cmdi, " cp -r " , PATH_MAX );
                      strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi ,  dirsrc , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "\"  " , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , dirtrg  , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "\"  " , PATH_MAX - strlen(cmdi) - 1);
                      ncurses_runcmd( cmdi );
    }




    ////////////
    ////////////
    ////////////
    ////////////
    void tinygrep(  int ty1, int tx1, int ty2, int tx2, char *tmyfile , char *tmyfilter )
    {
                  ndesk_tinygrep_count = 0;
                  FILE *fpreadviewer; int fooline; 
                  char strreadt[PATH_MAX];
                  char strread[PATH_MAX]; 
                  int tposy;
                  strncpy( strread, "" , PATH_MAX );
                  strncpy( strreadt , "" , PATH_MAX ); 
                  if ( fexist( tmyfile ) == 1 )
                  {
                     fpreadviewer = fopen( tmyfile  , "rb");
                     tposy = ty1;
                     while (( tposy <= ty2 ) && (!feof(fpreadviewer)) )  
                     {
                      if (!feof(fpreadviewer))
                        fgets(strreadt, PATH_MAX , fpreadviewer ); 
                      if (!feof(fpreadviewer))
                        strncpy( strread, strrlf( strreadt ) , PATH_MAX );
                      if (!feof(fpreadviewer))
                      {
                         if ( strread[0] != '/' )
                         if ( strread[1] != '/' )
                         {
                           if ( strcmp( tmyfilter, "" ) == 0 ) 
                              mvprintw( tposy++ , tx1,  "%s", strcut( strread, 1 , tx2-tx1-2 ) );
                           else if ( strcmp( tmyfilter, "" ) != 0 ) 
                            if ( strstr( strread , tmyfilter ) != 0 ) 
                            {
                              mvprintw( tposy++ , tx1,  "%s", strcut( strread, 1 , tx2-tx1-2 ) );
                              ndesk_tinygrep_count++;
                            }
                         }
                      }
                     }
                     fclose( fpreadviewer );
                  }
   }









    ////////////
    void tinyviewer(  int ty1, int tx1, int ty2, int tx2, char *tmyfile , char *tmyfilter )
    {
                  FILE *fpreadviewer; int fooline; 
                  char strreadt[PATH_MAX];
                  char strread[PATH_MAX]; 
                  int tposy;
                  strncpy( strread, "" , PATH_MAX );
                  strncpy( strreadt , "" , PATH_MAX ); 
                  if ( fexist( tmyfile ) == 1 )
                  {
                     fpreadviewer = fopen( tmyfile  , "rb");
                     //for( fooline = ty1 ; fooline <= ty2 ; fooline++)
                     tposy = ty1;
                     while (( tposy <= ty2 ) && (!feof(fpreadviewer)) )  
                     {
                      if (!feof(fpreadviewer))
                        fgets(strreadt, PATH_MAX , fpreadviewer ); 
                      if (!feof(fpreadviewer))
                        strncpy( strread, strrlf( strreadt ) , PATH_MAX );
                      if (!feof(fpreadviewer))
                      {
                         if ( strcmp( tmyfilter, "" ) == 0 ) 
                            mvprintw( tposy++ , tx1,  "%s", strcut( strread, 1 , tx2-tx1-2 ) );
                         else if ( strcmp( tmyfilter, "" ) != 0 ) 
                           if ( strstr( strread , tmyfilter ) != 0 ) 
                             mvprintw( tposy++ , tx1,  "%s", strcut( strread, 1 , tx2-tx1-2 ) );
                      }
                     }
                     fclose( fpreadviewer );
                  }
   }


   void nc_file_unlink()
   {
          if ( ( fexist( nwin_currentfile ) == 2 ) || ( fexist( nwin_currentfile   ) == 1 ) )
	  {
                    colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
                    mvprintw( 1, 0 , "unlink %s?",  nwin_currentfile );
                    strncpy( fooline , strninput( -1, nwin_currentfile ) , PATH_MAX );
                    if ( strcmp(  fooline  , "" ) != 0 )
                    {
                        unlink( fooline );
                    }
          }
    }



   void nc_file_dateit()
   {
          if ( ( fexist( nwin_currentfile ) == 2 ) || ( fexist( nwin_currentfile   ) == 1 ) )
	  {
                    colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
                    mvprintw( 1, 0 , "%s",  nwin_currentfile );
                    //strncpy( fooline , strninput( -1, nwin_currentfile ) , PATH_MAX );
                    strncpy( fooline , nwin_currentfile , PATH_MAX );
                    if ( strcmp(  fooline  , "" ) != 0 )
                    {
                      strncpy( cmdi, " mv   " , PATH_MAX );
                      strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi ,  nwin_currentfile , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "\"  " , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , strtimestamp()  , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "-"  , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , fooline  , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "\"  " , PATH_MAX - strlen(cmdi) - 1);
                      ncruncmd( cmdi );
                    }
          }
   }


   //////////////////
   //////////////////
   //////////////////
   void nc_file_rename( char *myfiletoren )
   {
          //if ( ( fexist( nwin_currentfile ) == 2 ) || ( fexist( nwin_currentfile   ) == 1 ) )
                    //colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
                    //mvprintw( 1, 0 , "%s",  nwin_currentfile );

                    strncpy( fooline , strninput( -1, nwin_currentfile ) , PATH_MAX );
                    if ( strcmp(  fooline  , "" ) != 0 )
                    {
                      strncpy( cmdi, " mv   " , PATH_MAX );
                      strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi ,  myfiletoren , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "\"  " , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , fooline  , PATH_MAX - strlen(cmdi) - 1);
                      strncat( cmdi , "\"  " , PATH_MAX - strlen(cmdi) - 1);
                      ncruncmd( cmdi ); 
                    }
    }














//////
  void ndesktop_draw( int drawmode ) //mode 1: norm, mode 2: move frames
  {
    strncpy( nwin_currentfile, "" , PATH_MAX );
    if ( nwin_sel[winsel] <= 0 ) nwin_sel[winsel] = 0;
    if ( nwin_scrolly[winsel] <= 0 ) nwin_scrolly[winsel] = 0;

    colordefaultrev();
    attroff( A_REVERSE);
    color_set( 0, NULL );
    erase();

    attroff( A_BOLD );
    for( foo = 1 ; foo <= 20 ; foo++)
    {
	strncpy( ndesktoppager_filesource, "" , PATH_MAX );

        if ( nwin_show[foo] == 1 )
	{
              attron( A_REVERSE); color_set( 0, NULL );
	      if ( colorscheme == 1 ) colorblue();

	      ncrectangle(   nwin_y1[foo], nwin_x1[foo], nwin_y2[foo], nwin_x2[foo] );

              if ( foo == winsel ) if ( drawmode == 1) { color_set( 23 , NULL );  nc_color_white(); }  
              if ( foo == winsel ) if ( drawmode == 2) { color_set( 23 , NULL );  color_set( 20, NULL); }  

	      ncframe( nwin_y1[foo], nwin_x1[foo], nwin_y2[foo], nwin_x2[foo] );
	      if ( colorscheme == 1 ) colorblue();

	     if( nwin_y1[foo] >= 0 )
	     {
	      mvprintw( nwin_y1[foo], nwin_x1[foo]+2, "[" );
               
	      if ( foo == winsel ) 
               if ( nwinmode == 0 )     //fly 
	       { color_set( 12 , NULL); addch( '!' ); colorblue(); }

               else if ( nwinmode == 1 ) //active tree
	          addch( ACS_DIAMOND ); 

	       else if ( foo != winsel ) addch( ' ' ); 
	          printw( "%d]", foo );
	     }






             /// app 5, time BSD
              if ( nwin_app[ foo ] == 5 )
	      {
                    attroff( A_REVERSE ); attroff( A_BOLD );
   	            boo = 0; posx = nwin_x1[foo]+1;
   		    posy = nwin_y1[foo]+boo - nwin_scrolly[ foo ] +1 ; 
                    mvprintw( posy++, posx, "" );
                    mvprintw( posy++, posx,  "BSD/X-20M(GNU) Version 1.0  " );
                    mvprintw(  posy++, posx, "Copyleft (C) 1978 GNU Research, Inc.  ");
                    mvprintw(  posy++, posx, "" );
                    mvprintw(  posy++, posx, "[DEC-VT100][%s] A> ls ", strtimenow());
	      }



              /// app 6, simple time
              else if ( nwin_app[ foo ] == 6 )
	      {
                    attroff( A_REVERSE ); attroff( A_BOLD );
   	            boo = 0; posx = nwin_x1[foo]+1;
   		    posy = nwin_y1[foo]+boo - nwin_scrolly[ foo ] +1 ; 
                    mvprintw(  posy++, posx, "[%s]", strtimenow());
                    mvprintw(  posy++, posx, "[%s]", strdatenow() );
	      }

              /// app 7, show current file 
              if ( nwin_app[ foo ] == 7 )
	      {
                    attroff( A_REVERSE ); attroff( A_BOLD );
   	            boo = 0; posx = nwin_x1[foo]+1;
   		    posy = nwin_y1[foo]+boo - nwin_scrolly[ foo ] +1 ; 
                    mvprintw( posy++, posx, "" );
                    mvprintw( posy++, posx, "[Win:%d]", winsel ); 
                    mvprintw( posy++, posx, "[%s]", nwin_path[ winsel ] ); 
                    mvprintw( posy++, posx, "[%s]", nwin_file[ winsel ] ); 
	      }





              /// app 12, !section viewer VIEWER, activated with 't'
              else if ( nwin_app[ foo ] == 12 )
	      {
	          chdir( nwin_path[ foo ] );
	          colorblue();
		  attroff( A_BOLD );
                  if( nwin_colorscheme[foo] == 2 ) nc_color_default();
	          gfxrectangle( nwin_y1[foo]+1, nwin_x1[foo]+1, nwin_y2[foo]-1, nwin_x2[foo]-1);
	          strncpy( ndesktoppager_filesource, nwin_file[foo] , PATH_MAX );
                  x1 = nwin_x1[foo]+1 ; 
                  y1 = nwin_y1[foo]+1 ; 
                  x2 = nwin_x2[foo]-1 ; 
                  y2 = nwin_y2[foo]-1 ; 
		  if ( foo == winsel ) strncpy( nwin_currentfile, nwin_file[foo] , PATH_MAX );
                  tinyviewer( y1, x1, y2, x2, nwin_file[ foo ] , nwin_filter[ foo ] );
		  attroff( A_BOLD );
	      } ///////////////



              /// app 14, !section viewer VIEWER, activated with 'T' of menu
              else if ( nwin_app[ foo ] == 14 )
	      {
	          chdir( nwin_path[ foo ] );
	          colorblue();
		  attroff( A_BOLD );
                  if( nwin_colorscheme[foo] == 2 ) nc_color_default();
	          gfxrectangle( nwin_y1[foo]+1, nwin_x1[foo]+1, nwin_y2[foo]-1, nwin_x2[foo]-1);
	          strncpy( ndesktoppager_filesource, nwin_file[foo] , PATH_MAX );
                  x1 = nwin_x1[foo]+1 ; 
                  y1 = nwin_y1[foo]+1 ; 
                  x2 = nwin_x2[foo]-1 ; 
                  y2 = nwin_y2[foo]-1 ; 
		  if ( foo == winsel ) strncpy( nwin_currentfile, nwin_file[foo] , PATH_MAX );
                  tinygrep( y1, x1, y2, x2, nwin_file[ foo ] , nwin_filter[ foo ] );
		  attroff( A_BOLD );
	      } ///////////////


              // dir application, which is based on tiny dirlist
	      else if ( nwin_app[ foo ] == 23 )
	      {
                /*
	        chdir( nwin_path[ foo ] );
		color_set( 0, NULL ); attron( A_REVERSE );
	        if ( colorscheme == 1 ) nc_color_default();

	        foostrlength = nwin_x2[foo] - nwin_x1[foo] -1 ;
   	        posx = nwin_x1[foo]+1;
                posy = nwin_y1[foo]+1 -nwin_scrolly[ foo ];

                boo=0;
                curdir = opendir( "." ); 
      	        while ((entry = readdir(curdir)) != NULL) 
      	        {
                  boosffilter = 1;
                  if ( strcmp( nwin_filter[ foo ] , "" ) != 0 )
                  {
                     if ( strstr( entry->d_name, nwin_filter[ foo ] ) != 0 )
                        boosffilter = 1; 
                     else
                        boosffilter = 0; //donjon naheulbeuk
                  }
                  if ( entry->d_name[0] == '.' ) boosffilter = 0;

                 if ( boosffilter == 1 ) boo++;

                 if ( boosffilter == 1 )
                 { 
		   if ( nwin_sel[ foo ] == boo ) attron(A_REVERSE); else attroff(A_REVERSE);
      		   if ( entry->d_type == DT_DIR ) 
                   {
		     if ( nwin_sel[ foo ] == boo ) { strncpy( nwin_currentfile , entry->d_name, PATH_MAX ); } 
                     color_set( 12, NULL );
                     if ( posy <= nwin_y2[foo] -1 ) if ( posy >= nwin_y1[foo]+1 )
                     mvprintw( posy, nwin_x1[foo]+1, "[%s]", strcut( entry->d_name , 1, foostrlength-2 )  );
                     posy++;  

		     if ( nwin_sel[foo] == boo ) strncpy( nwin_currentfile, entry->d_name    , PATH_MAX );
		     if ( nwin_sel[foo] == boo ) strncpy( nwin_file[ foo ], entry->d_name , PATH_MAX );
                   }
                   else
                   {
		     if ( nwin_sel[ foo ] == boo ) { strncpy( nwin_currentfile , entry->d_name, PATH_MAX ); } 
                     color_set( 11, NULL );
                     if ( posy <= nwin_y2[foo] -1 ) if ( posy >= nwin_y1[foo]+1 )
                     mvprintw( posy , nwin_x1[foo]+1, "%s", strcut( entry->d_name , 1,  foostrlength )  );
                     posy++;  
		     if ( nwin_sel[foo] == boo ) strncpy( nwin_currentfile, entry->d_name    , PATH_MAX );
		     if ( nwin_sel[foo] == boo ) strncpy( nwin_file[ foo ], entry->d_name , PATH_MAX );
                   }
                  } 

      	        }
      	        closedir(curdir);
                */
              }







	      else if ( nwin_app[ foo ] == 24 )
	      {
	        chdir( nwin_path[ foo ] );
		color_set( 0, NULL ); attron( A_REVERSE );
	        if ( colorscheme == 1 ) nc_color_default();
                posy = nwin_y1[foo]+1 -nwin_scrolly[ foo ];
                color_set( 11, NULL );
	        foostrlength = nwin_x2[foo] - nwin_x1[foo] -1 ;
                posy = nwin_y1[foo]+1 -nwin_scrolly[ foo ];
                curdir = opendir( "." ); 
      	        while ((entry = readdir(curdir)) != NULL) 
      	        {
                     if ( posy <= nwin_y2[foo] -1 ) 
                     if ( posy >= nwin_y1[foo]+1 )
                     {
                       mvprintw( posy , nwin_x1[foo]+1, "%s", strcut( entry->d_name , 1,  foostrlength )  );
                       posy++;  
                     }
      	        }
      	        closedir(curdir);
              }






              // dir application, which is based on TC turbo commander
	      else if ( nwin_app[ foo ] == 22 )
	      {
	        chdir( nwin_path[ foo ] );
		color_set( 0, NULL ); attron( A_REVERSE );
	        if ( colorscheme == 1 ) nc_color_default();
                //mvlistprint( ".", 0 , "" ) ;
              }




              /// app 30, 
              else if ( nwin_app[ foo ] == 30 )
	      {
                  attroff( A_REVERSE ); attroff( A_BOLD );
   	          boo = 0; posx = nwin_x1[foo]+1;
   		  posy = nwin_y1[foo]+boo - nwin_scrolly[ foo ] +1 ; 
                  mvprintw( posy++, posx, "-------" );
                  mvprintw( posy++, posx, "[Win:%d]", winsel ); 
                  mvprintw( posy++, posx, "[%s]", nwin_path[ winsel ] ); 
                  mvprintw( posy++, posx, "[%s]", nwin_file[ winsel ] ); 
                  mvprintw( posy++, posx, "-------" );
                  for( boo = 1 ; boo <= 9 ; boo++)
		  {
                    mvprintw( posy++, posx, "%d.App: %d", boo, nwin_app[ boo ] ); 
                    mvprintw( posy++, posx, "%d.File:%s", boo, nwin_file[ boo ] ); 
                    mvprintw( posy++, posx, "%d.Path:%s", boo, nwin_path[ boo ] ); 
                  }
	      } ///////////////






              // dir application  (main)
	      else if ( nwin_app[ foo ] == 1 )
	      {
	        chdir( nwin_path[ foo ] );
                loadlist( foo );
	        attron( A_REVERSE );
                for( boo = 0 ; boo <= filemax ; boo++)
		{
		  color_set( 0, NULL ); attron( A_REVERSE );
	          if ( colorscheme == 1 ) nc_color_default();

		  if ( boo >= nwin_scrolly[ foo ] )
		    posy = nwin_y1[foo]+boo - nwin_scrolly[ foo ] +1  ; 

		   if ( fexist( idata[ boo ] ) == 2 )  
		   {
		         color_set( 23 , NULL ); 
	                 if ( colorscheme == 1 ) nc_color_white();
		   }

		  foostrlength = nwin_x2[foo] - nwin_x1[foo] -1 ;
		  if ( nwin_sel[ foo ] == boo  ) 
		  {  
		    attroff( A_REVERSE );
	            if ( colorscheme == 1 ) attron( A_REVERSE );

		    if ( foo == winsel ) 
	              strncpy( nwin_currentfile, idata[boo] , PATH_MAX );
		    if ( foo == winsel ) 
	              strncpy( nwin_file[ winsel ], idata[boo] , PATH_MAX );
		  }

		  if ( boo >= nwin_scrolly[ foo ] )
		   if ( posy <= nwin_y2[foo] -1 )
		    if ( posy >= nwin_y1[ foo ]+1 )
		    {
	             //mvprintw( posy,  nwin_x1[foo]+1, "%d%s",boo, strcut( idata[ boo ], 1,  foostrlength )  );
	             mvprintw( posy,  nwin_x1[foo]+1, "%s", strcut( idata[ boo ], 1,  foostrlength )  );
		    }
		}
	      }








              // viewer (reader) 
	      else if ( nwin_app[ foo ] == 2 )
	      {
	          chdir( nwin_path[ foo ] );
	          colorblue();
		  attroff( A_BOLD );

                  if( nwin_colorscheme[foo] == 2 )
		     nc_color_default();

	          ncrectangle( nwin_y1[foo]+1, nwin_x1[foo]+1, nwin_y2[foo]-1, nwin_x2[foo]-1);
		  ndesktoppager_scrolly = nwin_sel[foo] ;
	          strncpy( ndesktoppager_filesource, nwin_file[foo] , PATH_MAX );
                  x1 = nwin_x1[foo]+1 ; 
                  y1 = nwin_y1[foo]+1 ; 
                  x2 = nwin_x2[foo]-1 ; 
                  y2 = nwin_y2[foo]-1 ; 
		  if ( foo == winsel )
	             strncpy( nwin_currentfile, nwin_file[foo] , PATH_MAX );
		  if ( strcmp( ndesktoppager_filesource, "" ) != 0 )
	              ndesktoppager_draw( foo );
		  attroff( A_BOLD );
	      }
	}
    }
   }




   void pixel(int toy, int tox )
   {
     mvaddch( toy+2, tox, ACS_CKBOARD );
   }









   /////////////////////////////////////////////////////
   /////////////////////////////////////////////////////
   //////////////////////////// tinywall ///////////////
   /////////////////////////////////////////////////////
   /////////////////////////////////////////////////////
   //    1:ghost,  2:just the time (clock) 
   /////////////////////////////////////////////////////
   /////////////////////////////////////////////////////
   void tinynrwall( int ghostmode ) 
   {
     int tinynrclock_gameover = 0;
     int foo = winsel ;  int nrch; 
     int myposx = 0;
     attroff( A_REVERSE ); attroff( A_BOLD );
     while ( tinynrclock_gameover == 0)
     {
      curs_set( 0 );
      nodelay( stdscr, TRUE);

      color_set( 0 , NULL );
      ndesktop_draw(1);                // refresh draw

      color_set( 7 , NULL );
      colormenuyellow();

      if ( ghostmode == 1 ) // with ghost 
      {
         mvprintw( rows/2+1, myposx,     "The time is now : %s",  strtimenow());
      }

      else if ( ghostmode == 2 ) //just the clock 
      {
         mvprintw( rows-3 , myposx,     "The time is now : %s, Date: %s",  strtimenow(), strdatenow() );
      }

      if ( ghostmode == 1 )
      {
         myposx++;
         if ( myposx >= cols-1 ) myposx = 0;
         pixel(3,myposx + 1);
         pixel(4,myposx +1);
         pixel(5,myposx + 1);
         pixel(6,myposx + 1);
         pixel(7,myposx + 1);
         pixel(8,myposx + 1);
   
         pixel(2,myposx + 2);
         pixel(9,myposx + 2);
   
         pixel(1,myposx + 3);
         pixel(8,myposx + 3);
   
         pixel(1,myposx + 4);
         pixel(5,myposx + 4);
         pixel(8,myposx + 4);
         pixel(6,myposx + 4);
   
         pixel(1,myposx + 5);
         pixel(9,myposx + 5);
   
         pixel(1,myposx + 6);
         pixel(5,myposx + 6);
         pixel(6,myposx + 6);
         pixel(9,myposx + 6);
   
         pixel(2,myposx + 7);
         pixel(8,myposx + 7);
   
   
         pixel(2,myposx + 8);
         pixel(3,myposx + 8);
         pixel(4,myposx + 8);
         pixel(5,myposx + 8);
         pixel(6,myposx + 8);
         pixel(7,myposx + 8);
      }

      usleep( 1e5 * 10);
      nrch = getch();
      if ( nrch == 'Q')        tinynrclock_gameover = 1;
      else if ( nrch == 'q')   tinynrclock_gameover = 1;
      else if ( nrch == 'i')   tinynrclock_gameover = 1;
      else if ( nrch == 'W')   tinynrclock_gameover = 1;
   }
   curs_set( 1 );
   nodelay( stdscr, FALSE);
  }







  ///////////////////////
  void ndesktop_sendtopanel( int dop )
  {
      int tofoudou = 0;
      switch( dop ) 
      {
	   case ')':
	      foo = nwin_sel[winsel]; 
	      if ( nwin_app[ winsel ] == 1 )
	      if ( fexist( nwin_currentfile ) == 2 )
	      {
                  chdir( nwin_currentfile );
	          strncpy( nwin_path[winsel+1], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                  nwin_app[winsel+1] = 1; 
                  nwin_show[winsel+1] = 1; 
	          nwin_scrolly[winsel+1] = 0;
	          nwin_sel[winsel+1] = 1;
		  ////////
	      }
	      break;


	   case '>':
	      foo = nwin_sel[winsel]; 
	      if ( nwin_app[ winsel ] == 1 )
	      if ( fexist( nwin_currentfile ) == 2 )
	      {
                  chdir( nwin_currentfile );
	          strncpy( nwin_path[winsel+1], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                  nwin_app[winsel+1] = 1; 
                  nwin_show[winsel+1] = 1; 
	          nwin_scrolly[winsel+1] = 0;
	          nwin_sel[winsel+1] = 1;
		  ////////
	      }
	      break;


	   case '<':
	   case '(':
	      foo = nwin_sel[winsel]; 
	      if ( nwin_app[ winsel ] == 1 )
	      if ( fexist( nwin_currentfile ) == 2 )
	      {
                  chdir( nwin_currentfile );
	          strncpy( nwin_path[winsel-1], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                  nwin_app[winsel-1] = 1; 
                  nwin_show[winsel-1] = 1; 
	          nwin_scrolly[winsel-1] = 0;
	          nwin_sel[winsel-1] = 1;
	      }
	      break;

	     }
   }







      ////////////////////////////////////////////////////////
      if ( argc == 2)
      if ( ( strcmp( argv[1] , "--ghost" ) ==  0 )
      || ( strcmp( argv[1] , "--clock" ) ==  0 ))
      {
           printf(" NDESKTOP: Hello from Ghost! \n" );
           erase();
           if ( strcmp( argv[1] , "--clock" ) ==  0 )
               tinynrwall( 2 );
           else
               tinynrwall( 1 );
           attroff( A_BOLD ); attroff( A_REVERSE ); curs_set( 1 );
           endwin();
           printf(" NDESKTOP: Bye from Ghost! \n" );
           return 0;
      }





    void ndesk_file_info()
    {
                 char foofilepath[PATH_MAX];
                 foo = 1; 
                 erase();
                 erase();
           	      mvprintw( foo++, 1 , "App (winsel): %d",  nwin_app[ winsel ] );
           	      mvprintw( foo++, 1 , "Winsel: %d", winsel );
           	      mvprintw( foo++, 1 , "File (winsel): %s", nwin_file[ winsel ] );
           	      mvprintw( foo++, 1 , "Filter (winsel): %s", nwin_filter[ winsel ] );
           	      mvprintw( foo++, 1 , "Path (winsel): %s", nwin_path[ winsel ] );
           	      mvprintw( foo++, 1 , "NWIN Filter: %s", nwin_filter[ winsel ] );
           	      mvprintw( foo++, 1 , "Crt File: %s", nwin_currentfile );
           	      mvprintw( foo++, 1 , "Crt Path: %s", getcwd( foofilepath, PATH_MAX ) );
           	      mvprintw( foo++, 1 , "Crt Basename File: %s", fbasename( nwin_currentfile ) );
           	      mvprintw( foo++, 1 , "Crt Fexist File: %d", fexist( nwin_currentfile ) );
           	      mvprintw( foo++, 1 , "ndesk Clip: %s",             ndesk_clipboard );
           	      mvprintw( foo++, 1 , "ndesk Clip (basename): %s",  fbasename( ndesk_clipboard ));
           	      mvprintw( foo++, 1 , "ndesk Clip (clip): %s",      ndesk_clipboard );
           	      mvprintw( foo++, 1 , "Clip: %s",        clipboard_filecopy );
           	      mvprintw( foo++, 1 , "Clip (basename): %s",        fbasename( clipboard_filecopy ));
           	      mvprintw( foo++, 1 , "Clip (Path): %s", clipboard_filepath );
                 i = foo;
                 for( foo = 1 ; foo <= 10 ; foo++) 
    	            mvprintw( i++, 1 , "Quickpath[%d]: %s", foo, quickpath[foo] );

                 ch = getch();  
     }





   foo = 1;
   nwin_x1[foo]= 2; nwin_x2[foo]= cols/2-1;
   nwin_y1[foo]= 3; nwin_y2[foo]= rows-4;
   foo = 2;
   nwin_x1[foo]= cols/2+1; nwin_x2[foo]= cols-2;
   nwin_y1[foo]= 3; nwin_y2[foo]= rows-4;

   // sel the first winframe #1
   nwin_show[ 1 ] = 1; nwin_app[ 1 ] = 1; winsel = 1;

   foo = 3;
   nwin_x1[foo]= cols*25/100;
   nwin_x2[foo]= cols*50/100;
   nwin_y1[foo]= rows/2-1;
   nwin_y2[foo]= rows-5;

   while( ndesktop_gameover == 0 )
   {
    // main, and clean
    // WHILE
    // MAIN WHILE 
    // MAIN LOOP
    getmaxyx( stdscr, rows, cols);
    attroff( A_REVERSE ); attroff( A_BOLD ); color_set( 0 , NULL );

    ndesktop_draw(1);

    strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
    strncpy( nwin_currentpath, nwin_path[ winsel ] , PATH_MAX );

    chdir( nwin_currentpath );
    if ( ndesk_show_statusbar == 1 )  
    {
        ndesktop_onscreen();
        ndesktop_statusbar();
    }

    if ( ndesk_show_status_path == 1 )
    {
          color_set( 0 , NULL ); 
          attroff( A_BOLD );
          attroff( A_UNDERLINE );
          attroff( A_REVERSE );
          gfxhline( rows-3, 0, cols -1 );
          mvprintw( rows-3 , 0 , "|%s|", getcwd( cwd, PATH_MAX ) );
    }

    if ( ndesk_show_statusbar == 1 )
    if ( strcmp( ndesk_statusbar, "" ) != 0 )
    {
          color_set( 11, NULL ); attroff( A_REVERSE );
          mvprintw( 0 , 0 , "|%s|", ndesk_statusbar );
          strncpy( ndesk_statusbar, "", PATH_MAX );
    }

    if ( strcmp( ndesk_status_message, "" ) != 0 ) 
    {
        attroff( A_BOLD ); 
        attroff( A_BOLD ); attron( A_REVERSE ); color_set( 5, NULL);
        mvprintw( rows-1, 0, "|%s|", ndesk_status_message );
        strncpy( ndesk_status_message, "", PATH_MAX );
    }

    refresh();
    /// MAIN CH 
    ch = getch();
      if ( ch == '|' ) ch = '~'; // <- strange
    attroff( A_REVERSE ); attroff( A_BOLD ); color_set( 0 , NULL );
    curs_set( 1 );


    /////////////////////////
    /////////////////////////
    /////////////////////////
    if ( ch == 'w' ) 
    {
        move_gameover = 0;
        while ( move_gameover == 0)
        {
           ndesktop_draw(2); //mode 2 for move
           colormenuyellow();
           if ( ndesk_show_statusbar == 1 ) 
           {
              mvprintw( rows-1, cols-3, "[w]" );
              gfxhline( rows-1, 0, cols -1 );
              mvprintw( rows-1, 0, "[Move Frames]" );
           }
           refresh();
           ch = getch(); 
           switch( ch ) 
           {
           case 'w':
           case 'm':
           case 'i':
           case KEY_F(10):
           case 27:
            move_gameover = 1;
            break; 

           case '>':
              nwin_x1[winsel]+=6;
              nwin_x2[winsel]+=6;
	      break;
           case '<':
              nwin_x1[winsel]-=6;
              nwin_x2[winsel]-=6;
	      break;

           case 'd':
              nwin_y1[winsel]+=6;
              nwin_y2[winsel]+=6;
	      break;
           case 'u':
              nwin_y1[winsel]-=6;
              nwin_y2[winsel]-=6;
	      break;
           case 'L':
              nwin_x2[winsel]++;
	      break;
           case 'H':
              nwin_x2[winsel]--;
	      break;
           case 'J':
              nwin_y2[winsel]++;
	      break;
           case 'K':
              nwin_y2[winsel]--;
	      break;


           case 'l':
              nwin_x1[winsel]++;
              nwin_x2[winsel]++;
	      break;
           case 'h':
              nwin_x1[winsel]--;
              nwin_x2[winsel]--;
	      break;

           case 'j':
              nwin_y1[winsel]++;
              nwin_y2[winsel]++;
	      break;
           case 'k':
              nwin_y1[winsel]--;
              nwin_y2[winsel]--;
	      break;


           case '#':
              foo = winsel;
              mvprintw( 0, 0, "%d %d %d %d", nwin_x1[foo], nwin_x2[foo], nwin_y1[foo], nwin_y2[foo]);
              mvprintw( 1, 0, "%d %d", rows, cols );
              getch();
	      break;

           }
           }
           ch = 0;
       }






      if ( nwinmode == 1 )         //active edit
      if ( nwin_app[winsel] == 2 ) //tree
      switch( ch ){
           case 'j':
              nwin_sel[winsel]++;
	      break;
           case 'k':
              nwin_sel[winsel]--;
	      break;
           case 'i':
              nwinmode = 0; 
	      break;
           case 'g':
              nwin_sel[winsel] = 1;
	      break;
           case KEY_GBACKSPACE:
              chdir( ".." );
	      strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
	      break;
      }



      if ( nwinmode == 1 )         //active edit
      if ( nwin_app[winsel] == 1 ) //tree
      switch( ch ){
           case 'j':
              nwin_sel[winsel]++;
	      break;
           case 'k':
              nwin_sel[winsel]--;
	      break;
           case 'i':
              nwinmode = 0; 
	      break;
           case 'g':
              nwin_sel[winsel] = 1;
	      break;
           case KEY_GBACKSPACE:
              chdir( ".." );
	      strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
	      break;
      }



     


      if ( nwinmode == 0 ) // fly
      switch( ch )
      {

           case KEY_F(10):
              ncolor_white(  ); attroff( A_BOLD ); 
              fooquestion = ncwin_question( "Really quit NDESKTOP?" );
              if ( fooquestion == 1 ) 
	        ndesktop_gameover = 1;
	        break;



    case 'B':
 	 chdir( nwin_path[ winsel ] );
         strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
         printfile_viewer( fileselection  ); 
         break;




    case 'b':
              foochg = 0; 
              nexp_user_sel = 1;
              nexp_user_scrolly =0 ;
              while ( ( foochg != 'q' ) && ( foochg != 'i' ) &&  ( foochg != 27 ) &&  ( foochg != 'b' )) 
              { 
                color_set( 0, NULL); attroff( A_REVERSE ); attroff( A_BOLD );
                gfxhline( rows-1 , 0, cols-1);
                printdir(); 
                color_set( 0, NULL); attroff( A_REVERSE ); attroff( A_BOLD );
                foochg = getch();
                if ( foochg == 'j' )      nexp_user_sel++; 
                else if ( foochg == 'k' ) nexp_user_sel--;
                else if ( foochg == 'g' ) { nexp_user_sel = 1; nexp_user_scrolly = 0; }

                else if ( foochg == 'h' ) {  chdir( ".." ); nexp_user_sel = 1; nexp_user_scrolly = 0; } 
                else if ( foochg == 'l' ) {  chdir( nexp_user_fileselection ); nexp_user_sel = 1; nexp_user_scrolly = 0; } 
                else if ( foochg == '~' ) {  chdir( getenv( "HOME") ); nexp_user_sel = 1; nexp_user_scrolly = 0; } 

                else if ( foochg == 'y' ) 
                {
	           strncpy( nwin_pathclip,   getcwd( foocwd, PATH_MAX) , PATH_MAX);
	           strncpy( ndesk_clipboard, getcwd( foocwd, PATH_MAX) , PATH_MAX);
                }

                else if  ( foochg == 'V' )  ncurses_runwith( " vim.tiny  " , nexp_user_fileselection );
                else if  ( foochg == 'v' )  ncurses_runwith( " vim " , nexp_user_fileselection );
                else if  ( foochg == 'n' )  ncurses_runwith( " screen -d -m nedit  " , nexp_user_fileselection );
                else if  ( foochg == 'r' )  ncurses_runwith( " tcview  " , nexp_user_fileselection );
                else if  ( foochg == '!' )  ncurses_runwith( strninput( -1 , "") , nexp_user_fileselection );
                else if  ( foochg == '$' )  ncurses_runcmd( strninput( -1 , "") );

                else if ( foochg == 'u' ) nexp_user_scrolly-=4;
                else if ( foochg == 'd' ) nexp_user_scrolly+=4;
                else if ( foochg == 'n' ) nexp_user_scrolly+=4;
                else if ( foochg == 'r' ) ncurses_runwith( " tcview " , nexp_user_fileselection );
                else if ( foochg == 'v' ) ncurses_runwith( " vim " , nexp_user_fileselection );
                else if ( foochg == 's' ) ncurses_runwith( " clisheet " , nexp_user_fileselection );
                else if ( foochg == 'w' ) ncurses_runwith( " cliword " , nexp_user_fileselection );
                else if ( foochg == 'o' ) { printfile( nexp_user_fileselection ); getch();  }

                else if ( foochg == 'c' ) 
                {
                      strncpy( foostr , getcwd( foocwd, PATH_MAX ), PATH_MAX );
                      chdir( getenv( "HOME" ) );
                      if ( fexist( ".clipboard" ) == 1 )
                      {
                        printfile( ".clipboard" ); 
                        ch = getch(); 
                        if       ( ch == 'v' )  ncurses_runwith( " vim " , ".clipboard" );
                        else if  ( ch == 'n' )  ncurses_runwith( " screen -d -m nedit  " , ".clipboard" );
                      }
                      chdir( foostr );
                }

              }
              ch = 0;
              break;






           case '1':
           case '2':
           case '3':
           case '4':
           case '5':
           case '6':
           case '7':
           case '8':
           case '9':
              nwin_show[ch - 48] = 1;
              winsel = ch - 48;
	      break;


           case KEY_GPAGEUP:
           case 'U':
              nwin_scrolly[winsel]--;
	      break;
           case KEY_GPAGEDOWN:
           case 'D':
              nwin_scrolly[winsel]++;
	      break;




           case KEY_GCTRLU:
	   case 'u':
              if ( nwin_app[winsel] >= 1 )
	      if ( winsel >= 1 )
	      {
               nwin_scrolly[winsel]-=4;
               nwin_sel[winsel]-=4;
	      }
	      break;


	   case 'd':
	   case 'n':
           case KEY_GCTRLD:
	   case 32:
              if ( nwin_app[winsel] >= 1 )
	      if ( winsel >= 1 )
	      {
               nwin_scrolly[winsel]+=4;
               nwin_sel[winsel]+=4;
	      }
	      break;



	   case '.':
	      winsel++;
	      if ( winsel >= 10 ) winsel = 1;
              foo = snprintf( charo, PATH_MAX , "Windesk #%d",  winsel );
              strncpy( ndesk_statusbar, charo , PATH_MAX );
              //nwin_show[winsel]=1;
	      break;
           case ',':
	      winsel--;
	      if ( winsel <= 0 ) winsel = 9;
              foo = snprintf( charo, PATH_MAX , "Windesk #%d",  winsel );
              strncpy( ndesk_statusbar, charo , PATH_MAX );
              //nwin_show[winsel]=1;
	      break;


          /// to adjacent pan (1 or 2)
	  case 'e':
	  case 'o':
	      if ( nwin_app[ winsel ] == 0 ) nwin_app[ winsel ] = 1;

	      foo = nwin_sel[winsel]; 
	      if ( ( nwin_app[ winsel ] == 1 ) || ( nwin_app[ winsel ] == 23 ))
	      {
	         if ( fexist( nwin_currentfile ) == 2 )
                 {
                    chdir( nwin_currentfile );
	            strncpy( nwin_path[winsel+1], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                    nwin_app[winsel+1] = 1; 
                 } 
	         else if ( fexist( nwin_currentfile ) == 1 )
                 {
                   if ( nwin_app[winsel+1] == 1 ) nwin_app[winsel+1] = 2; 
     	           strncpy( nwin_path[ winsel+1], nwin_path[winsel], PATH_MAX );
                 } 
                 else
                 {
     	           strncpy( nwin_path[ winsel+1], nwin_path[winsel], PATH_MAX );
                 } 
                 nwin_show[winsel+1] = 1; 
	         nwin_scrolly[winsel+1] = 0;
	         nwin_sel[winsel+1] = 1;
		 ////////
     	         strncpy( nwin_file[ winsel+1], nwin_currentfile, PATH_MAX );
	      }
	      break;

          /// to third pan
	  case 'z':
	      if ( nwin_app[ winsel ] == 0 ) nwin_app[ winsel ] = 1;
	      foo = nwin_sel[winsel]; 
	      if ( ( nwin_app[ winsel ] == 1 ) || ( nwin_app[ winsel ] == 23 ))
	      {
	         if ( fexist( nwin_currentfile ) == 2 )
                 {
                    chdir( nwin_currentfile );
	            strncpy( nwin_path[ 3 ], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                    nwin_app[ 3 ] = 1; 
                 } 
	         else if ( fexist( nwin_currentfile ) == 1 )
                 {
                   if ( nwin_app[ 3 ] == 1 ) nwin_app[ 3 ] = 2; 
     	           strncpy( nwin_path[ 3 ], nwin_path[ winsel ], PATH_MAX );
                 } 
                 else
                 {
     	           strncpy( nwin_path[ 3 ], nwin_path[ winsel ], PATH_MAX );
                 } 
                 nwin_show[3] = 1; 
	         nwin_scrolly[3] = 0;
	         nwin_sel[3] = 1;
		 ////////
     	         strncpy( nwin_file[ 3 ], nwin_currentfile, PATH_MAX );
                 nwin_sel[3] = 1; nwin_scrolly[3] = 0;
	      }
	      break;


           case 'S':
              nwin_app[winsel] = 1; 
              nwin_file_sort[winsel] = 1; 
              strncpy( ndesk_statusbar, "Sort File Explorer" , PATH_MAX );
	      break;

           case 'g':
                colormenuyellow();
                if ( ndesk_show_statusbar == 1 ) 
                   mvprintw( rows-1, cols-3, "[g]" );
  	        ch = getch();
	        if ( ch == 'g' )
	        {
                  nwin_sel[winsel] = 1; nwin_scrolly[winsel] = 0;
	        }
	        else if ( ch == 'v')
		{
	          foo = winsel; 
                  nwin_y1[foo]=1;
                  nwin_y2[foo]=rows-2;
	        }
	        else if ( ch == 'h')
		{
	          foo = winsel; 
                  nwin_x1[foo]=0;
                  nwin_x2[foo]=cols-1;
	        }
	        break;

           case '0':
              winsel = 0;
              for( foo = 1 ; foo <= 10 ; foo++) 
                nwin_show[foo] = 0;
	      break;

           case 27:
	      //winsel--;
	      foo = winsel; 
              nwin_show[foo]=0;
              nwin_show[ winsel -1 ] = 1;
              winsel--;
	      if ( winsel <= 0 ) winsel = 1;
	      break;


           case 'C':
              nwin_app[winsel] = 6; 
	      nwin_scrolly[winsel] = 0;
              nwinmode = 0;
	      break;

           case 'W':
              tinynrwall( 1 ); // nice ghost
	      break;

           //case 'T':
              //tinynrwall( 2 ); // tiny
	    //  break;


           case '+':
              nwin_app[winsel]++;
	      break;

           case '-':
              nwin_app[winsel]--;
	      break;



	 case 'a':
              ndesktop_showcurfile();
              ndesk_menu_actions_compiler();
	      break;


           case 'Q':
              ncolor_white(  ); attroff( A_BOLD ); 
              mvprintw( 0,0, "Q" );
	      ndesktop_gameover = 1;
	      break;

           case 'q':
                ncolor_white(  ); attroff( A_BOLD ); 
                mvprintw( 0,0, "Q" );
                nwin_app[winsel] = 1; 
                nwin_sel[winsel] = 1;
	        ndesktop_quickdir();
                nwin_sel[winsel] = 1;
	        nwin_scrolly[winsel] = 0;
                nwin_app[winsel] = 1; 
	        nwin_scrolly[winsel] = 0;
                nwinmode = 0;
		break;


           case KEY_GTAB:
                 if ( winsel == 1 )
                    winsel = 2;
                 else
                    winsel = 1;
                 chdir( nwin_path[winsel] );
                 nwin_show[winsel] = 1;
	      break;



	   case 10:
              ndesktop_showcurfile();
	      if ( winsel >= 1 )
              if ( fexist( nwin_currentfile ) == 1 )
              {
                 if ( strcmp( fextension( nwin_currentfile ) , "pdf" ) == 0 )
                 {
                   if ( strcmp( getenv( "TERM" ) , "linux" ) == 0 ) 
                        ncurses_runwith( " fbgs -r 270  " , nwin_currentfile ); 
                   else
                        ncurses_runwith( " screen -d -m mupdf " , nwin_currentfile ); 
                 }
                 else if ( strcmp( fextension( nwin_currentfile ) , "PDF" ) == 0 )
                   ncurses_runwith( " screen -d -m mupdf " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "html" ) == 0 )
                   ncurses_runwith( " links " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "htm" ) == 0 )
                   ncurses_runwith( " links " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "png" ) == 0 )
                 {
                   //ncurses_runwith( " feh -FZ " , nwin_currentfile ); 
                   if ( strcmp( getenv( "TERM" ) , "linux" ) == 0 ) 
                        ncurses_runwith( " fbi   " , nwin_currentfile ); 
                   else
                        ncurses_runwith( " feh -FZ  " , nwin_currentfile ); 
                 }

                 else if ( strcmp( fextension( nwin_currentfile ) , "eps" ) == 0 )
                   ncurses_runwith( " epsview  " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "sms" ) == 0 )
                   ncurses_runwith( " mednafen " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "gen" ) == 0 )
                   ncurses_runwith( " mednafen " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "nes" ) == 0 )
                   ncurses_runwith( " mednafen " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "wad" ) == 0 )
                   ncurses_runwith( " prboom-plus " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "WAD" ) == 0 )
                   ncurses_runwith( " prboom-plus " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "wav" ) == 0 )
                   ncurses_runwith( " mplayer " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "WAV" ) == 0 )
                   ncurses_runwith( " mplayer " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "MP3" ) == 0 )
                   ncurses_runwith( " mplayer " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "mp3" ) == 0 )
                   ncurses_runwith( " mplayer " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "rec0" ) == 0 )
                   ncurses_runwith( " mplayer -af volume=10.1:0  "  , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "rec" ) == 0 )
                 {
                   if ( config_sound_card == 0 ) 
                     ncurses_runwith( " mplayer -ao alsa:device=hw=0,0   -af volume=10.1:0  "  , nwin_currentfile ); 
                   else if ( config_sound_card == 1 ) 
                     ncurses_runwith( " mplayer -ao alsa:device=hw=1,0   -af volume=10.1:0  "  , nwin_currentfile ); 
                   else if ( config_sound_card == 2 ) 
                     ncurses_runwith( " mplayer -ao alsa:device=hw=0,0   -af volume=10.1:0     "  , nwin_currentfile ); 
                   else if ( config_sound_card == 3 ) 
                     ncurses_runwith( " mplayer -ao alsa:device=hw=1,0    "  , nwin_currentfile ); 
                 }

                 else if ( strcmp( fextension( nwin_currentfile ) , "avi" ) == 0 )
                   ncurses_runwith( " mplayer -fs -zoom " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "mp4" ) == 0 )
                   ncurses_runwith( " mplayer -fs -zoom " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "MP4" ) == 0 )
                   ncurses_runwith( " mplayer -fs -zoom " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "mpeg" ) == 0 )
                   ncurses_runwith( " mplayer -fs -zoom " , nwin_currentfile ); 
                 else if ( strcmp( fextension( nwin_currentfile ) , "mpg" ) == 0 )
                   ncurses_runwith( " mplayer -fs -zoom " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "jpg" ) == 0 )
                 {
                   //ncurses_runwith( " feh -FZ " , nwin_currentfile ); 
                   if ( strcmp( getenv( "TERM" ) , "linux" ) == 0 ) 
                        ncurses_runwith( " fbi   " , nwin_currentfile ); 
                   else
                        ncurses_runwith( " feh -FZ  " , nwin_currentfile ); 
                 }

                 else if ( strcmp( fextension( nwin_currentfile ) , "nbf" ) == 0 )
                   ncurses_runwith( " ndata " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "tex" ) == 0 )
                   ncurses_runwith( " tdcedit " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "csn" ) == 0 )
                   ncurses_runwith( " nspread " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "CSV" ) == 0 )
                   ncurses_runwith( " clisheet " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "wrd" ) == 0 )
                   ncurses_runwith( " cliword " , nwin_currentfile ); //cli office suite
                 else if ( strcmp( fextension( nwin_currentfile ) , "csx" ) == 0 )
                   ncurses_runwith( " clisheet " , nwin_currentfile );//cli office suite 

                 else if ( strcmp( fextension( nwin_currentfile ) , "csv" ) == 0 )
                   ncurses_runwith( " clisheet " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "ws1" ) == 0 )
                   ncurses_runwith( " freelotus123  " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "wrd" ) == 0 )
                   ncurses_runwith( " cliword  " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "txt" ) == 0 )
                   ncurses_runwith( " cliword  " , nwin_currentfile ); 

                 else if ( strcmp( fextension( nwin_currentfile ) , "uni" ) == 0 )
                   ncurses_runwith( " cliword  " , nwin_currentfile ); 
                   // to view only


                 else
                   ncurses_runwith( " feh -FZ "   , nwin_currentfile ); 
              }
	      break;




	   case 'Y':
     	    chdir( nwin_path[ winsel ] );
	    if ( nwin_app[ winsel ] >= 1 )
	    {
                color_set( 11, NULL ); attroff( A_REVERSE );
                mvprintw( 0 , 0 , "FILE SEL: %s", nwin_file[ winsel ] );

		chdir( getenv( "HOME" ));
                fp = fopen( ".clipboard" , "wb" );
		  fputs( "!fig{" , fp ); 
                  fputs(  strrlf( nwin_file[ winsel ] ),  fp ) ; 
		  fputs( "}" , fp ); 
		  fputs( "\n" , fp ); 
		 fclose( fp );
	    }
	    break;



	   case 'y':
              if ( winsel >= 1 )
	      {
	        chdir( nwin_path[ winsel ] );
	        strncpy( nwin_pathclip, getcwd( foocwd, PATH_MAX) , PATH_MAX);
	        strncpy( nwin_clip_filter, nwin_filter[ winsel ] , PATH_MAX);
	        nwin_clip_app = nwin_app[ winsel ];

                if ( fexist( nwin_file[ winsel  ] ) == 1 ) 
	          strncpy( ndesk_clipboard,  nwin_file[ winsel ] , PATH_MAX);
              }
	      break;


	   case 'p':
	      if ( strcmp( nwin_pathclip, "" ) != 0 )
              if ( winsel >= 1 )
	      {
	        chdir( nwin_pathclip );
	        strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                nwin_app[winsel] = 1; 
	        nwin_scrolly[winsel] = 0;
	        strncpy( nc_file_filter[ winsel ] , "" , PATH_MAX );
	        strncpy( nwin_filter[ winsel ] , "" , PATH_MAX );
	        strncpy( nwin_filter[ winsel ] , nwin_clip_filter , PATH_MAX ); //new
                nwin_scrolly[winsel] = 0;
                nwin_sel[winsel] = 1;
	        nwin_app[ winsel ] = nwin_clip_app ; 

	        strncpy( nwin_filter[ winsel ] , nwin_clip_filter , PATH_MAX);
              }
	      break;


	   case KEY_GCTRLN:
              if ( winsel >= 1 )
	      if ( strcmp( nwin_path[ winsel ] , "" ) != 0 )
	      {
	        chdir( nwin_path[ winsel ] );
	        strncpy( nwin_pathclip, getcwd( foocwd, PATH_MAX) , PATH_MAX);
	        if ( strcmp( nwin_pathclip, "" ) != 0 )
		{
		   winsel++;
   	           chdir( nwin_pathclip );
   	           strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                   nwin_app[winsel] = 1; 
                   nwin_show[winsel] = 1; 
   	           nwin_scrolly[winsel] = 0;
		}
              }
	      break;


           case KEY_F(2):
              getmaxyx( stdscr, rows, cols);
     	      chdir( nwin_path[ winsel ] );
              strncpy( fileselection, nwin_currentfile , PATH_MAX );
              color_set( 11, NULL ); attroff( A_REVERSE );
              color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
              mvprintw( 0, 0, "[Rename][%s]", fileselection ); 
              if ( ( fexist( fileselection ) == 1 ) || ( fexist( fileselection ) == 2 ) )
                nc_file_rename( fileselection );
	      break;

        case '/':
          chdir( "/" );
	  strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
          nwin_sel[winsel] = 1;
	  nwin_scrolly[winsel] = 0;
	  strncpy( nc_file_filter[ winsel ], "", PATH_MAX);
 	  strncpy( nwin_filter[ winsel ], "", PATH_MAX);
          break;

        case '~':
        //case '>':
          strncpy( ndesk_status_message, "HOME (chdir)", PATH_MAX );
          chdir( getenv("HOME") );
	  strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
          nwin_sel[winsel] = 1;
	  nwin_scrolly[winsel] = 0;
	  strncpy( nc_file_filter[ winsel ], "", PATH_MAX);
 	  strncpy( nwin_filter[ winsel ], "", PATH_MAX);
          break;


       case '\'':
          color_set( 5, NULL ); attron( A_REVERSE );
          mvprintw( rows-1, cols-1, "R" );
          mvprintw( 0, 0, "R" );
          ch = getch();

          if ( ( ch >= 'a' ) && ( ch <= 'z' ) )
          {
	      strncpy( nwin_path[ winsel ] ,  nwin_pile_path[ ch ]  ,  PATH_MAX);
              nwin_sel[winsel] = 1; nwin_scrolly[winsel] = 0;  // <- top of line sel
              strncpy( ndesk_status_message, "Path Restored.", PATH_MAX );
          }
          else
          switch( ch )
          { 
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
	      strncpy( nwin_file[winsel]   , nwin_pile_file[ ch -48 ]  , PATH_MAX);
	      nwin_app[winsel] = nwin_pile_app[ ch -48 ]; 
	      strncpy( nwin_path[ winsel ] ,  nwin_pile_path[ ch -48 ]  ,  PATH_MAX);
              nwin_sel[winsel] = 1; nwin_scrolly[winsel] = 0;  // <- top of line sel
              strncpy( ndesk_status_message, "Path Restored.", PATH_MAX );
              break;
           }
           break;




    case KEY_F(9):
         foo = ndesk_menu_select( "1: Run with...", "2: Run system command...", "3: Quick Notepad", "4: nmenu (SVN)", "5 or v: Load vim with mksession" , "6: nexplorer" , "Q: Quit!" );
         if ( ( foo == '1' ) ||  ( foo == '!' ) )
	      {
     	        chdir( nwin_path[ winsel ] );
                strncpy( fileselection, nwin_currentfile , PATH_MAX );
                color_set( 11, NULL ); attroff( A_REVERSE );
     	        mvprintw( 0, 0, "PATH: %s", getcwd( foocwd, PATH_MAX ) );
     	        mvprintw( 1, 0, "FILE: %s", fileselection );
     	        mvprintw( 2, 0, "Run with given command line." );
                strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                ncurses_runwith( strninput( -1, "" ) , fileselection  ); 
	      }

         else if ( ( foo == '2' ) ||  ( foo == '$' ) )
         {
             color_set( 11, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
             for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
             for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
             mvprintw( rows-2, 0, "[System Command Line]" ); 
             strncpy( fooline , strninput( -1, "" ), PATH_MAX );
             if ( strcmp( fooline , "" ) != 0 ) ncurses_runcmd(  fooline  );
         }
         else if  ( foo == '3' ) ndesk_screenlock_text();
         //else if  ( foo == '4' ) nruncmd( "box" );
         else if  ( foo == '4' ) nruncmd( "nmenu" );
         else if  ( foo == '5' ) nruncmd( " vim -S Session.vim " );
         else if  ( foo == 'v' ) nruncmd( " vim -S Session.vim " );
         else if  ( foo == 'V' ) vim_piles();
         else if  ( foo == 'c' ) nruncmd( " bash " );
         else if  ( foo == '6' ) nruncmd( " nexplorer " );
         else if  ( foo == 'h' ) ndesk_help();
         else if  ( foo == 'H' ) ndesk_help();
         else if  ( foo == '?' ) 
         {
             color_set( 11, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
             for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
             for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
             mvprintw( rows-2, 0, "[Test Input Line]" ); 
             strncpy( fooline , strninput( -1, "" ), PATH_MAX );
         }
         else if  ( foo == 'Q' ) ndesktop_gameover = 1;
         else if  ( foo == 'q' ) ndesktop_gameover = 1;
	 break;


       case '"':
       case 'm':
          getmaxyx( stdscr, rows, cols);
          attron( A_REVERSE ) ; 
          color_set( 3, NULL ) ;  
          mvprintw( 0,0,"|SELECT A PILE (Path)|");
          ch = getch();
          if ( ( ch >= 'a' ) && ( ch <= 'z' ) )
          {
              strncpy( nwin_pile_path[ ch ]  , nwin_path[ winsel ] , PATH_MAX);
              strncpy( ndesk_status_message, "Path Stored.", PATH_MAX );
          }
          else
          switch( ch )
          { 
              case 'V':
              case 'v':
                vim_piles();
                break;

              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
              if ( winsel >= 1 )
	      {
	          strncpy( nwin_pile_file[ ch -48 ]  , nwin_file[ winsel ] , PATH_MAX);
	          nwin_pile_app[ ch -48 ] = nwin_app[ winsel ];
	          strncpy( nwin_pile_path[ ch -48 ]  , nwin_path[ winsel ] , PATH_MAX);
                  strncpy( ndesk_status_message, "Path Stored.", PATH_MAX );
              }
	      break;
           }
         break;

        case 'M':
          getmaxyx( stdscr, rows, cols);
          attron( A_REVERSE ) ; 
          color_set( 3, NULL ) ; mvprintw( rows-1, cols-7+4 , "[m]" );
          mvprintw( 0,0,"[m]");
          ch = getch();
          if      ( ch == 'c' ) ncurses_runcmd( " bash " );
          else if ( ch == 'm' ) ndesk_move_frame( winsel );
          else if ( ch == 'w' ) ndesk_move_frame( winsel );

          else switch( ch )
          { 
              case 'V':
              case 'v':
                vim_piles();
                break;

              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
              if ( winsel >= 1 )
	      {
	          strncpy( nwin_pile_path[ ch -48 ]  , nwin_path[ winsel ] , PATH_MAX);
	          strncpy( nwin_pile_file[ ch -48 ]  , nwin_file[ winsel ] , PATH_MAX);
	          nwin_pile_app[ ch -48 ] = nwin_app[ winsel ];
                  strncpy( ndesk_status_message, "Path Stored.", PATH_MAX );
              }
	      break;
            }
	    break;


           case KEY_F(3):
              getmaxyx( stdscr, rows, cols);
              ndesk_menu_desktop(); 
	      break;




           //case KEY_F(4):
              //getmaxyx( stdscr, rows, cols);
	      //if ( fexist( nwin_file[winsel] ) == 1 ) ncurses_runwith( " tcedit ", nwin_file[ winsel ]  ); 
	      //break;


           //case 'S':
              //ndesk_menu_quickpath();
	    //  break;




	   case '#':
              getmaxyx( stdscr, rows, cols);
              getmaxyx( stdscr, rows, cols);
              erase();     

              color_set( 10, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
              gfxhline( 0 , 0, cols-1);
              mvcenter( 0, "|Internal Application Command|");

              i = 2; color_set( 0, NULL );
              gfxframe( 1, 0, rows-1, cols-1 );
              mvprintw( i++, 2, "-: Current Win Sel: %d", winsel );
              mvprintw( i++, 2, "1: File Info           ; 2: Menu Parameters" );
              mvprintw( i++, 2, "3: Show/Hide Statusbar ; 4: Show/Hide Path Info" );
              mvprintw( i++, 2, "5: Show Grep Count %d",   ndesk_tinygrep_count );
              mvprintw( i++, 2, "-: Show Grep Filter: %s",  nwin_filter[ winsel  ] );
              mvprintw( i++, 2, "6: Show Hidden files %d", showhiddenfiles );
              mvprintw( i++, 2, "7: Config Sound Card %d", config_sound_card );
              mvprintw( i++, 2, "8: Alsamixer App." ); 
              mvprintw( i++, 2, "9: Hostname [%s]", strreadfile( "/etc/hostname" ) ); 
              mvprintw( i++, 2, "Info [term %s][user %s][home %s]", getenv( "TERM" ), getenv( "USER" ), getenv( "HOME" ) );
              /////////////////////////////////
              mvprintw( i++, 2, "" );
              foo = 1; mvprintw( i++, 2, "Pile Path/file (Pile #1):"); 
              mvprintw( i++, 2, "[%s/%s]", nwin_pile_path[foo] , nwin_pile_file[foo] );
              mvprintw( i++, 2, "" );
              mvprintw( i++, 2, "" );
              /////////////////////////////////
              for ( foo = 1 ; foo <= 5 ; foo++)
              { 
                if ( i <= rows-2 )
                { 
                   mvprintw( i++, 2, "Pile Path %d [%s]", foo, nwin_pile_path[foo] );
                   mvprintw( i++, 2, "Pile File %d [%s]", foo, nwin_pile_file[foo] );
                   mvprintw( i++, 2, "Pile App  %d [%d]", foo, nwin_pile_app[foo] );
                } 
              }

              ch = getch();  

              if ( ch == '1' ) 
                   ndesk_file_info();

              else if ( ch == '8' ) 
                  ncurses_runcmd( " alsamixer "); 

              else if ( ch == '9' ) 
                  ncdisplayfile( "/etc/hostname" );

              else if ( ch == '2' ) 
                   ndesk_menu_parameters();

              else if ( ch == '3' )
              {  
                 if ( ndesk_show_statusbar == 1 ) ndesk_show_statusbar = 0;
                 else ndesk_show_statusbar = 1;
              }
              else if ( ch == '4' )
              {  
                 if ( ndesk_show_status_path == 1 ) ndesk_show_status_path = 0;
                 else ndesk_show_status_path = 1;
              }
              else if ( ch == '5' )
              {   erase(); 
                  mvprintw( 2, 2, "Show Grep Count %d", ndesk_tinygrep_count );
                  getch();
              }
              else if ( ch == '6' )
              {   erase(); 
                  if(  showhiddenfiles == 0 ) 
                       showhiddenfiles = 1 ; 
                  else 
                       showhiddenfiles = 0 ; 
                  mvprintw( 2, 2, "Show hidden files %d", showhiddenfiles );
                  getch();
              }
              else if ( ch == '7' )
              {
                  if      (  config_sound_card == 0 ) config_sound_card = 1 ; 
                  else if (  config_sound_card == 1 ) config_sound_card = 2 ; 
                  else if (  config_sound_card == 2 ) config_sound_card = 3 ; 
                  else if (  config_sound_card >= 3 ) config_sound_card = 0 ; 
                  erase(); 
                  mvprintw( 2, 2, "Config Sound Card (ALSA) %d", config_sound_card );
                  getch();
              }
              break;


         case '?':
         case KEY_F(1):
              getmaxyx( stdscr, rows, cols);
              ndesk_help();
              break;




          case '*':
            if      ( ndesk_statusbar_mode == 0 ) ndesk_statusbar_mode = 1;
            else if ( ndesk_statusbar_mode == 1 ) ndesk_statusbar_mode = 2;
            else if ( ndesk_statusbar_mode == 2 ) ndesk_statusbar_mode = 3;
            else if ( ndesk_statusbar_mode == 3 ) ndesk_statusbar_mode = 4;
            else if ( ndesk_statusbar_mode == 4 ) ndesk_statusbar_mode = 5;
            else if ( ndesk_statusbar_mode == 5 ) ndesk_statusbar_mode = 0;
            break;


          //////////////////
          case KEY_F(4):
              getmaxyx( stdscr, rows, cols);
              ch = ndesk_menu_choice( "1: Copy File", "2: Paste File" );
              if ( ch == '1') 
	      {
                color_set( 11, NULL ); attroff( A_REVERSE );
  	        if ( nwin_app[ winsel ] != 2 )
  	        {
     	        chdir( nwin_path[ winsel ] );
                strncpy( fileselection, nwin_currentfile , PATH_MAX );
     	        mvprintw( 0, 0, "FILE: %s", fileselection );
		{ 
                   strncpy( clipboard_filepath, "", PATH_MAX );
                   strncpy( clipboard_filecopy, "", PATH_MAX );
                   /// 
                   strncpy( clipboard_filepath , nwin_path[winsel] , PATH_MAX );
                   strncpy( fileselection, nwin_currentfile , PATH_MAX );
                   /// 
                   strncpy( cmdi , nwin_path[winsel] , PATH_MAX );
                   strncat( cmdi , "/" , PATH_MAX - strlen( cmdi ) -1 );
                   strncat( cmdi , fileselection , PATH_MAX - strlen( cmdi ) -1 );
                   /// 
                   mvprintw( 0 , 0 , "FILE SEL: %s", cmdi );
                   strncpy( clipboard_filecopy, cmdi , PATH_MAX );
                   mvprintw( 1 , 0 , "FILE CLIP: %s", clipboard_filecopy );
                   strncpy( ndesk_statusbar, "FILE COPIED", PATH_MAX );
		}
	        }
	      }


              else if ( ch == '2') 
	      {
                   strncpy( cmdi, fbasename( clipboard_filecopy ) , PATH_MAX );
                   strncpy( foostr , getcwd( foocwd, PATH_MAX ), PATH_MAX );
                   if ( strcmp( foostr, clipboard_filepath ) == 0 )
                   {
                       strncat( cmdi , "-" , PATH_MAX - strlen(cmdi) - 1);
                       strncat( cmdi , strtimestamp() , PATH_MAX - strlen(cmdi) - 1);
                   }
                   curs_set( 0 ) ;
                   color_set( 11, NULL ); attroff( A_REVERSE );
                   strncpy( tcsys_filesource , clipboard_filecopy, PATH_MAX );
                   //strncpy( tcsys_filetarget , strninputbox( rows/2, cmdi ), PATH_MAX );
                   // strncpy( fooline , strninput( -1, nwin_currentfile ) , PATH_MAX );
                   strncpy( foolinetmp, cmdi, PATH_MAX );
                   strncpy( tcsys_filetarget , strninput( -1, foolinetmp ) , PATH_MAX );
                   ndesktop_draw(1);  
                   color_set( 11, NULL ); attroff( A_REVERSE );
                   gfxrectangle( rows/2-4 , 0 , rows/2+4, cols -1 );
                   gfxframe(    rows/2-4  , 0 , rows/2+4, cols -1 );
                   mvprintw( rows/2-1, 2, "SRC: %s", tcsys_filesource );
                   mvprintw( rows/2  , 2, "TRG: %s", tcsys_filetarget );
                   mvprintw( rows/2+2, 2, "COPY [y/N]? " );
                   curs_set( 1 ) ;

     	           chdir( nwin_path[ winsel ] );
                   mvprintw( 0,0, "PATH: %s" , getcwd( cwd, PATH_MAX ) );
                   ch = getch();

                   if ( ( ch == '1') || ( ch == 'y') )
                    if ( fexist(tcsys_filesource) == 1 )
                      tcfilecopy( tcsys_filetarget,   tcsys_filesource ); 
	      }
              ch = 0;
              break;



	   case KEY_F(5):
	   case 'c':
               strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
               strncpy( nwin_currentpath, nwin_path[ winsel ] , PATH_MAX );
               strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
               if ( nwin_show[ 1 ] == 1 )
               if ( nwin_show[ 2 ] == 1 )
               if (( winsel == 1 ) || ( winsel == 2 ))
	       if ( ( fexist( nwin_currentfile ) == 2 ) || ( fexist( nwin_currentfile ) == 1 ))
               {
                   chdir( nwin_currentpath );
                   strncpy( cmdi, " cp -r   " , PATH_MAX );
                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi ,   nwin_currentfile  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                   if ( winsel == 2 ) foo = 1; else if ( winsel == 1 ) foo = 2; 
                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_path[ foo ] ,  PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   color_set( 0, NULL ); attron( A_REVERSE ); 
                   mvprintw( 1,0," PATH:%s", getcwd( cwd, PATH_MAX) );
                   mvprintw( 3,0," CMD:%s", cmdi );
                   foo = ncwin_question( "copy this file ?" );
                   if ( foo == 1 )   ncurses_runcmd( cmdi );
               }
               break;


	   case KEY_F(6):
               strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
               strncpy( nwin_currentpath, nwin_path[ winsel ] , PATH_MAX );
               strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
               if ( nwin_show[ 1 ] == 1 )
               if ( nwin_show[ 2 ] == 1 )
               if (( winsel == 1 ) || ( winsel == 2 ))
	       if ( ( fexist( nwin_currentfile ) == 2 )
	       || ( fexist( nwin_currentfile ) == 1 ))
               {
                   chdir( nwin_currentpath );
                   strncpy( cmdi, " mv   " , PATH_MAX );
                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi ,   nwin_currentfile  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                   if ( winsel == 2 ) foo = 1; else if ( winsel == 1 ) foo = 2; 
                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_path[ foo ] ,  PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   color_set( 0, NULL ); attron( A_REVERSE ); 
                   mvprintw( 1,0," PATH:%s", getcwd( cwd, PATH_MAX) );
                   mvprintw( 3,0," CMD:%s", cmdi );
                   foo = ncwin_question( "copy this file ?" );
                   if ( foo == 1 ) 
                   {
                      foo = ncwin_question( "copy this file (sure)?" );
                      if ( foo == 1 ) 
                        ncurses_runcmd( cmdi );
                   }
               }
               break;


	  case KEY_F(7):
                color_set( 10, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                color_set( 22 , NULL ); 
                gfxhline( 0 , 0, cols-1);
                mvcenter( 0, "| MKDIR |");
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
                color_set( 0 , NULL ); mvprintw( rows-2, 0, "PATH: %s", getcwd( cwd, PATH_MAX ) ); 
                //////
                color_set( 23, NULL ); attron( A_REVERSE );
                gfxrectangle( rows/2-2 , 0 , rows/2+2  , cols-1);
                gfxframe(     rows/2-2 , 0 , rows/2+2  , cols-1);
                mvcenter( rows/2-2, "|New Dir Name|" ); 
                //////
                strncpy( foostr, strninput( rows/2 , "" ), PATH_MAX );
                if ( strcmp( foostr , "" ) != 0 ) 
                     ncurses_runwith( " mkdir -p " , foostr  ); 
		break;

	  case KEY_F(8):
                color_set( 10, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                color_set( 22 , NULL ); 
                gfxhline( 0 , 0, cols-1);
                mvcenter( 0, "| MKDIR |");
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
                color_set( 0 , NULL ); mvprintw( rows-2, 0, "PATH: %s", getcwd( cwd, PATH_MAX ) ); 
                //////
                color_set( 23, NULL ); attron( A_REVERSE );
                gfxrectangle( rows/2-2 , 0 , rows/2+2  , cols-1);
                gfxframe(     rows/2-2 , 0 , rows/2+2  , cols-1);
                mvcenter( rows/2-2, "|New File Name|" ); 
                //////
                strncpy( foostr, strninput( rows/2 , strtimestamp() ), PATH_MAX );
                if ( strcmp( foostr , "" ) != 0 ) 
                    sys_create_file( foostr ); 
		break;





#if defined(__linux__)
   /*
	   case 'M':
               color_set( 0, NULL ); attron( A_REVERSE ); 
	       gfxrectangle( rows/2 -3 , 0 , rows/2 +3 , cols-1 ); 
	       gfxframe( rows/2 -3 , 0 , rows/2 +3 , cols-1 ); 
               mvprintw( rows/2, 3, "1 - COPY " );
               color_set( 0, NULL ); attroff( A_REVERSE ); 
               ch = getch();
               if ( ch == '1' ) 
               { 
               strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
               strncpy( nwin_currentpath, nwin_path[ winsel ] , PATH_MAX );
               strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
               if ( nwin_show[ 1 ] == 1 )
               if ( nwin_show[ 2 ] == 1 )
               if (( winsel == 1 ) || ( winsel == 2 ))
	       if ( ( fexist( nwin_currentfile ) == 2 )
	       || ( fexist( nwin_currentfile ) == 1 ))
               {
                   chdir( nwin_currentpath );
                   strncpy( cmdi, " cp -r -v  " , PATH_MAX );
                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi ,   nwin_currentfile  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                   if ( winsel == 2 ) foo = 1; else if ( winsel == 1 ) foo = 2; 
                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , nwin_path[ foo ] ,  PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   color_set( 0, NULL ); attron( A_REVERSE ); 
                   mvprintw( 1,0," PATH:%s", getcwd( cwd, PATH_MAX) );
                   mvprintw( 3,0," CMD:%s", cmdi );
                   foo = ncwin_question( "copy this file ?" );
                   if ( foo == 1 )   ncurses_runcmd( cmdi );
               }
               }
               break;
   */


	   case 281:
               strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
               strncpy( nwin_currentpath, nwin_path[ winsel ] , PATH_MAX );
               strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
               //if ( nwin_show[ 2 ] == 1 )
	       //if ( ( fexist( nwin_currentfile ) == 2 )
	       //|| ( fexist( nwin_currentfile ) == 1 ))
               if ( nwin_show[ 1 ] == 1 )
               if (( winsel == 1 ) || ( winsel == 2 ))
	       if ( ( fexist( nwin_currentfile ) == 1 ) ||( fexist( nwin_currentfile ) == 2 ))
               {
                   chdir( nwin_currentpath );
                   strncpy( cmdi, " cp -r   " , PATH_MAX );
                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi ,   nwin_currentfile  , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                   if ( winsel == 2 ) foo = 1; else if ( winsel == 1 ) foo = 2; 
                   strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , strtimestamp() ,  PATH_MAX - strlen(cmdi) - 1);
                   //strncat( cmdi , nwin_path[ foo ] ,  PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                   color_set( 0, NULL ); attron( A_REVERSE ); 
                   mvprintw( 1,0," PATH:%s", getcwd( cwd, PATH_MAX) );
                   mvprintw( 3,0," CMD:%s", cmdi );
                   foo = ncwin_question( "clone copy this file ?" );
                   if ( foo == 1 )   ncurses_runcmd( cmdi );
               }
               break;


         // was f5 f6 and f7
         // f8

#endif








	   case 't':
	       if ( fexist( nwin_currentfile ) == 1 )
               {
                   nwin_sel_before[winsel] = nwin_sel[winsel];
     	           chdir( nwin_path[ winsel ] );
                   nwin_app[winsel] = 12; 
                   nwin_sel[winsel] = 1; 
                   nwinmode = 0;
                   nwin_colorscheme[winsel] = 2; 
     	           foo = nwin_sel[winsel]; 
     	           strncpy( nwin_file[ winsel ], nwin_currentfile , PATH_MAX );
                   strncpy( ndesk_status_message, "Tinyview (app:12)", PATH_MAX );
               }
	      break;

	   case 'G':
	       if ( fexist( nwin_currentfile ) == 1 )
               {
                   nwin_sel_before[winsel] = nwin_sel[winsel];
     	           chdir( nwin_path[ winsel ] );
                   nwin_app[winsel] = 14; 
                   //nwin_sel[winsel] = 1; 
                   //nwin_sel[winsel] = 1; nwin_scrolly[winsel] = 0;  // <- top of line sel
                   nwinmode = 0;
                   nwin_colorscheme[winsel] = 2; 
     	           //foo = nwin_sel[winsel]; 
     	           strncpy( nwin_file[ winsel ], nwin_currentfile , PATH_MAX );
                   strncpy( ndesk_status_message, "Tinygrep (app:14) (filter uni sections)", PATH_MAX );
	           strncpy( nwin_filter[ winsel ] , "!= ", PATH_MAX ); 
               }
	      break;

	   case 'T':
	       if ( fexist( nwin_currentfile ) == 1 )
               {
                   nwin_sel_before[winsel] = nwin_sel[winsel];
     	           chdir( nwin_path[ winsel ] );
                   nwin_app[winsel] = 14; 
                   nwin_sel[winsel] = 1; 
                   nwinmode = 0;
                   nwin_colorscheme[winsel] = 2; 
     	           foo = nwin_sel[winsel]; 
     	           strncpy( nwin_file[ winsel ], nwin_currentfile , PATH_MAX );
                   strncpy( ndesk_status_message, "Tinygrep (app:14)", PATH_MAX );
               }
	      break;



	   case 'r':
	      if ( ( nwin_app[ winsel ]  == 2 ) 
              &&  ( nwin_show[ winsel ] == 1 ))
              {
                nwin_sel[winsel] = tempsel[winsel] ; 
                nwin_scrolly[winsel] = tempscroll[winsel] ; 
                nwin_app[winsel] = 1;
              }
	      else
              if ( nwin_app[ winsel ] != 2 )
	      {
	           tempscroll[winsel] = nwin_scrolly[winsel];
	           tempsel[winsel] = nwin_sel[winsel];
                   nwin_app[winsel] = 2; 
                   nwin_sel[winsel] = 1;
	           nwin_scrolly[winsel] = 0;
                   nwinmode = 0;
                   nwin_sel_before[winsel] = nwin_sel[winsel];
     	           chdir( nwin_path[ winsel ] );
                   nwin_app[winsel] = 2; 
                   nwinmode = 0;
                   nwin_colorscheme[winsel] = 2; 
     	           foo = nwin_sel[winsel]; 
     	           strncpy( nwin_file[ winsel ], nwin_currentfile , PATH_MAX );
                   nwin_sel[winsel] = 1; 
	      }
	     //else
	     // {
             //  nwin_app[winsel] = 1; 
             //  nwin_sel[winsel] = nwin_sel_before[winsel];
	     // }
	      break;


           /// old blue
	   case 'R':
	      if ( nwin_app[ winsel ] != 2 )
	      {
	        if ( fexist( nwin_currentfile ) == 1 )
		{
                   nwin_sel_before[winsel] = nwin_sel[winsel];
     	           chdir( nwin_path[ winsel ] );
                   nwin_app[winsel] = 2; 
                   nwinmode = 0;
     	           foo = nwin_sel[winsel]; 
                   nwin_colorscheme[winsel] = 1; 
     	           strncpy(nwin_file[ winsel ], nwin_currentfile , PATH_MAX );
                   nwin_sel[winsel] = 1; 
		}
	      }
	      else
	      {
                nwin_app[winsel] = 1; 
                nwin_sel[winsel] = nwin_sel_before[winsel];
	      }
	      break;


           case KEY_GBACKSPACE:
	      chdir( nwin_path[ winsel ] );
              chdir( ".." );
	      strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
	      break;

           case 'F':
	     ndesktop_dirdefault();
	     break;

           case 'f':
             colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
             strncpy( foolinetmp,  nc_file_filter[ winsel ]  , PATH_MAX );
             strncpy( fooline , strninput( -1, "" ), PATH_MAX );
	     if ( winsel >= 1 )
	     {
	        strncpy( nc_file_filter[ winsel ] , fooline, PATH_MAX );
	        strncpy( nwin_filter[ winsel ]    , fooline, PATH_MAX ); // for tinyviewer
                nwin_scrolly[winsel] = 0;
                nwin_sel[winsel] = 1;
	     }
	     break;

           case 6: //ctrlf
             /*
             colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
             strncpy( foolinetmp,  nc_file_filter[ winsel ]  , PATH_MAX );
             strncpy( fooline , strninput( -1, foolinetmp ), PATH_MAX );
	     if ( winsel >= 1 )
	     {
	        strncpy( nc_file_filter[ winsel ] , fooline, PATH_MAX );
	        strncpy( nwin_filter[ winsel ]    , fooline, PATH_MAX ); // for tinyviewer
                nwin_scrolly[winsel] = 0;
                nwin_sel[winsel] = 1;
	     } */
                       color_set( 7, NULL ); attron( A_REVERSE ); attroff( A_BOLD ); mvclear( ); 
                       foo = 3;
                       mvcenter( 0, "== NFIND ==" ); 
                       mvcenter( 1, "===========" ); 
                       mvprintw( foo++, 0, "DISPLAY: %d %d", rows, cols );
                       mvprintw( foo++, 0, "HOME:    %s ", getenv( "HOME" ));
                       mvprintw( foo++, 0, "USER:    %s ", getenv( "USER" ));
                       mvprintw( foo++, 0, "PATH:    %s ", getcwd( foostr, PATH_MAX ));
                       strncpy( foostr, strninput( rows/2 , "" ), PATH_MAX );
                       if ( strcmp( foostr, "" ) != 0 )  
                       {
                         def_prog_mode();
                         endwin();
                          listdirsearchfile( ".", 0, foostr );
                          printf( "<Press Key To Continue>\n" );
                          getchar();
                         reset_prog_mode();
                       }
	     break;


	   case '$':
             color_set( 22, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
             color_set( 11, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
             for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
             for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
             mvprintw( rows-2, 0, "[System Command Line]" ); 
     	     chdir( nwin_path[ winsel ] ); ndesk_show_current_pwd( nwin_currentfile );
             strncpy( fooline , strninput( -1, "" ), PATH_MAX );
             if ( strcmp( fooline , "" ) != 0 ) ncurses_runcmd(  fooline  );
	     break;

	   case '%':
             color_set( 22, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
             color_set( 11, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
             for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
             for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
             mvprintw( rows-2, 0, "[System Command Line (screen)]" ); 
     	     chdir( nwin_path[ winsel ] ); ndesk_show_current_pwd( nwin_currentfile );
             strncpy( fooline , strninput( -1, " screen -d -m " ), PATH_MAX );
             if ( strcmp( fooline , "" ) != 0 ) ncurses_runcmd(  fooline  );
	     break;



	   case ':':
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
                mvprintw( rows-2, 0, "[Internal Command]" ); 
     	        chdir( nwin_path[ winsel ] );
                strncpy( fileselection, nwin_currentfile , PATH_MAX );
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                color_set( 11, NULL ); attroff( A_REVERSE );
                gfxrectangle( 0 , 0, 3, cols-1);
                gfxframe(     0 , 0, 3, cols-1);
     	        mvprintw( 1, 2, "PATH: %s", getcwd( foocwd, PATH_MAX ) );
     	        mvprintw( 2, 2, "FILE: %s", fileselection );
                color_set( 10, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                gfxhline( 0 , 0, cols-1);
                mvcenter( 0, "|Internal Application Command|");
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                gfxhline( rows-1 , 0, cols-1);

                 strncpy( cmdi, strninput( -1, "" ), PATH_MAX );
                 color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                 if ( strcmp( cmdi , "" ) != 0 ) 
                 {

                   if ( strcmp( cmdi , "nc" ) == 0 ) ncurses_runcmd( "nc" );


                   else if ( strcmp( cmdi , "mpaa" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " mplayer -vo aa " , fileselection  ); 
                   }

                   else if ( strcmp( cmdi , "mp" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " mplayer -fs -zoom " , fileselection  ); 
                   }

                   else if ( strcmp( cmdi , "mpx" ) == 0 ) 
                        ncurses_runwith( " mplayer -fs -zoom -idx " , nwin_file[ winsel ] ); 

                   else if ( strcmp( cmdi , "view?" ) == 0 ) 
                   {
                       strncpy( fileselection, strninput( -1,"noname.c") , PATH_MAX );
                       ncdisplayfile( fileselection );
                   }

                   else if ( strcmp( cmdi , "grep" ) == 0 ) 
                   {
                       strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                       if ( fexist( fileselection ) == 1 )
                          ncdisplaygrep( fileselection );
                   }

                   else if ( strcmp( cmdi , "view" ) == 0 ) 
                   {
                       strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                       if ( fexist( fileselection ) == 1 )
                          ncdisplayfile( fileselection );
                   }

                   else if ( strcmp( cmdi , "ip" ) == 0 ) ncurses_runcmd( "              ip addr ; read key " );


                   else if ( strcmp( cmdi , "hostname" ) == 0 ) ncurses_runcmd( "  hostname ; read key " );

                   ////////////////////////////////////////////
                   else if ( strcmp( cmdi , "fileview" ) == 0 )  // basic retro blue ncurses apps
                       ncurses_runwith( " fileview " , nwin_file[ winsel ] ); 
                   ////////////////////////////////////////////
                   else if ( strcmp( cmdi , "dirview" ) == 0 )  // basic retro blue ncurses apps
                       ncurses_runwith( " dirview " , nwin_file[ winsel ] ); 
                   else if ( strcmp( cmdi , "menu" ) == 0 )
                         ndesk_main_menu();
                   ////////////////////////////////////////////

                   else if ( strcmp( cmdi , "mpall" ) == 0 ) ncurses_runcmd( " mplayer -fs -zoom * " );
                   else if ( strcmp( cmdi , "mpa" ) == 0 ) ncurses_runcmd( "   mplayer -fs -zoom * " );


                   else if ( strcmp( cmdi , "google" ) == 0 ) ncurses_runcmd( "    links google.com     " );

                   else if ( strcmp( cmdi , "tex" ) == 0 )    ncurses_runcmd( "nconsole2" );
                   else if ( strcmp( cmdi , "vimrc" ) == 0 )  ncurses_runcmd( "vim ~/.vimrc " );
                   else if ( strcmp( cmdi , "links" ) == 0 )  ncurses_runcmd( "links  http://duckduckgo.com  " );

                   else if ( ( strcmp( cmdi , "snake" ) == 0 )
                   || ( strcmp( cmdi , "snakedraw" ) == 0 ))
                   {
                         int nxy = 5; int nxx = 5;
                         int nxc = 0;
                         while ( nxc == 0 )
                         { 
                          if ( strcmp( cmdi , "snake" ) == 0 ) erase(); 
                          mvprintw( nxy, nxx , "O" );
                          ch = getch();  
                          if      ( ch == 'l' ) nxx++;
                          else if ( ch == 'h' ) nxx--;
                          else if ( ch == 'j' ) nxy++;
                          else if ( ch == 'k' ) nxy--;
                          else if ( ch == 'g' ) {nxy = 0; nxx = 0;}
                          else if ( ch == 'q' ) nxc = 1;
                         } 
                   }
                   else if ( strcmp( cmdi , "os" ) == 0 )
                   {
                        erase();
                        mvprintw( 1, 1 , "MY OS: %d\n", MYOS );
                        getch();
                   }
                   else if ( ( strcmp( cmdi , "erase" ) == 0 ) || ( strcmp( cmdi , "cls" ) == 0 ))
                   {
                        erase();
                        gfx_cls();
                        erase();
                   }

                   else if ( strcmp( cmdi , "elinks" ) == 0 )  ncurses_runcmd( "links  http://duckduckgo.com  " );
                   else if ( strcmp( cmdi , "noteman" ) == 0 ) ncurses_runcmd( "noteman" );
                   else if ( strcmp( cmdi , "ngg" ) == 0 )     ncurses_runwith( " ngg " , strninput( -1, "" ) );
                   else if ( strcmp( cmdi , "rox" ) == 0 )   ncurses_runcmd( "rox" );
                   else if ( strcmp( cmdi , "bash" ) == 0 )   ncurses_runcmd( "bash" );

                   else if ( strcmp( cmdi , "make" ) == 0 )   ncurses_runcmd( "make" );
                   else if ( strcmp( cmdi , "run" ) == 0 )   ncurses_runcmd( " bash run.sh " );

                   else if ( strcmp( cmdi , "mutt" ) == 0 )   

                         ncurses_runcmd( "mutt" );


                   else if ( strcmp( cmdi , "tcps" ) == 0 ) ncurses_runcmd( " tcps  " );
                   else if ( strcmp( cmdi , "xterm" ) == 0 ) ncurses_runcmd( "xterm -bg black -fg green " );
                   else if ( strcmp( cmdi , "sxterm" ) == 0 ) ncurses_runcmd( " screen -d -m xterm -bg black -fg green " );


                   else if ( strcmp( cmdi , "test menu" ) == 0 ) 
                   { foo = ndesk_menu_select( "1: naclock...", "2: ...", "3: three", "4: ...", "5: ..." , "6: ..." , "i: Cancel" ); if ( foo == '1' ) ncurses_runcmd( "naclock" );      }

                   else if ( strcmp( cmdi , "naclock" ) == 0 ) ncurses_runcmd( " naclock " );

                   else if ( strcmp( cmdi , "nmenu" ) == 0 ) ncurses_runcmd( " nmenu " );
                   else if ( strcmp( cmdi , "vim" ) == 0 ) ncurses_runcmd( " vim " );

                   else if ( strcmp( cmdi , "scr" ) == 0 ) ncurses_runcmd( " tcscreenshot " );

                   else if ( strcmp( cmdi , "webcam" ) == 0 ) 
                        ncurses_runcmd( " mplayer -ao null -tv device=/dev/video0 tv://  ");
                   else if ( strcmp( cmdi , "nwebcam" ) == 0 ) 
                        ncurses_runcmd( " mplayer -ao null -hardframedrop  -tv device=/dev/video0 tv://  ");

                   else if ( strcmp( cmdi , "calc" ) == 0 ) ncurses_runcmd( "         nrpn  " );
                   else if ( strcmp( cmdi , "nrpn" ) == 0 ) ncurses_runcmd( "         nrpn  " );

                   else if ( strcmp( cmdi , "tcsp" ) == 0 ) ncurses_runcmd( "tcsp" );
                   else if ( strcmp( cmdi , "tcs" ) == 0 ) ncurses_runcmd(  "tcs" );
                   else if ( strcmp( cmdi , "tcs" ) == 0 ) ncurses_runcmd( "tcs" );
                   else if ( strcmp( cmdi , "tc" ) == 0 ) ncurses_runcmd( "tc" );

                   else if ( strcmp( cmdi , "vim all" ) == 0 ) ncurses_runcmd( " vim -p *" );
                   else if ( strcmp( cmdi , "vimall" ) == 0 ) ncurses_runcmd( " vim -p *" );


                   else if ( strcmp( cmdi , "cliview" ) == 0 ) 
                       ncurses_runwith( " cliview  " , nwin_file[ winsel ] ); 

                   else if ( strcmp( cmdi , "mpvoloop" ) == 0 ) 
                       ncurses_runwith( " mplayer -vo null -loop 0 " , nwin_file[ winsel ] ); 

                   else if ( strcmp( cmdi , "mpvo" ) == 0 ) 
                   {
                       strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                       ncurses_runwith( " mplayer -vo null " , fileselection  ); 
                   }


    else if ( strcmp( cmdi , "compile" ) == 0 ) 
    {
      foo = ncwin_question( "Compile this file (sure)?" );
      if ( foo == 1 ) 
      {
       strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
       def_prog_mode();
       endwin();
       strncpy( thefile, fileselection , PATH_MAX );
       strncpy( cmdi , " unibeam " , PATH_MAX );
       strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , thefile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       system( cmdi );
       strncpy( targetfile, fbasenoext( thefile ) , PATH_MAX );
       strncat( targetfile , ".tex" , PATH_MAX - strlen( targetfile ) -1 );
       strncpy( cmdi , " pdflatex -shell-escape --interaction=nonstopmode " , PATH_MAX );
       strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , targetfile , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
       strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
       system( cmdi );
        strncpy( targetfile, fbasenoext( thefile ) , PATH_MAX );
        strncat( targetfile , ".pdf" , PATH_MAX - strlen( targetfile ) -1 );
        strncpy( cmdi , " screen -d -m  mupdf  " , PATH_MAX );
        strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
        strncat( cmdi , targetfile , PATH_MAX - strlen( cmdi ) -1 );
        strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
        strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
        system( cmdi );
        reset_prog_mode();
       }
      }

                   else if ( strcmp( cmdi , "close" ) == 0 ) 
                   {
                       nwin_show[winsel]=0;
                       winsel--;
                       if ( nwin_sel[winsel] == 0 ) nwin_sel[winsel] = 1;
       	               if ( winsel <= 0 ) winsel = 1;
                     }


                   else if ( strcmp( cmdi , "mpao" ) == 0 ) 
                   {
                       strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                       ncurses_runwith( " mplayer -ao null " , fileselection  ); 
                   }

    else if ( strcmp( cmdi , "acx" ) == 0 ) 
       ncurses_runcmd( " xterm -bg black -fg green -fs 28 -fa 'DejaVu Sans Mono' " );

                   else if ( strcmp( cmdi , "snedit" ) == 0 ) 
                       ncurses_runwith( " screen -d -m nedit  " , nwin_file[ winsel ] ); 

                   else if ( strcmp( cmdi , "less" ) == 0 ) 
                       ncurses_runwith( " less " , nwin_file[ winsel ] ); 

                   else if ( strcmp( cmdi , "sfledit" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " screen -d -m fledit " , fileselection  ); 
                   }


                   else if ( strcmp( cmdi , "fledit" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " fledit " , fileselection  ); 
                   }


                   else if (( strcmp( cmdi , "ssh" ) == 0 ) 
                   || ( strcmp( cmdi , "SSH" ) == 0 ))
                   {
                    attron( A_REVERSE ); 
                    mvprintw( 0, 0 , "|SSH|" );
                    strncpy( fooline , strninput( -1, "192.168.1.10" ) , PATH_MAX );
                    colornorm();
                    if ( strcmp(  fooline  , "" ) != 0 )
                    {
                        ncurses_runwith( " ssh -C " , fooline  ); 
                    }
                   }

                   else if ( strcmp( cmdi , "nblue" ) == 0 ) 
                       ncurses_runcmd( "nblue " ); 

                   else if ( strcmp( cmdi , "ls2file" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " ls -1  >> " , fileselection  ); 
                        color_set( 7, NULL ); attron( A_REVERSE ); attroff( A_BOLD ); 
                        mvclear( ); 
                        mvcenter( rows*50/100 -1, "[ ls command Completed ]" );
                        getch();
                   }

                   else if ( strcmp( cmdi , "xclip2file" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " xclip -o >> " , fileselection  ); 
                        color_set( 7, NULL ); attron( A_REVERSE ); attroff( A_BOLD ); 
                        mvclear( ); 
                        mvcenter( rows*50/100 -1, "[ xclip command Completed ]" );
                        getch();
                   }

                   else if ( strcmp( cmdi , "mpup" ) == 0 ) 
                        ncurses_runwith( "   mplayer -af volume=10.1:0   " , nwin_file[ winsel ] ); 

                   else if ( strcmp( cmdi , "lc" ) == 0 ) ncurses_runcmd( " lc " );

                   else if ( strcmp( cmdi , "nfileman" ) == 0 ) ncurses_runcmd( " nfileman " );
                   else if ( strcmp( cmdi , "tcs" ) == 0 ) ncurses_runcmd( " tcs " );
                   else if ( strcmp( cmdi , "tc" ) == 0 ) ncurses_runcmd( " tc " );
                   else if ( strcmp( cmdi , "tcsp" ) == 0 ) ncurses_runcmd( " tcsp " );
                   else if ( strcmp( cmdi , "mc" ) == 0 ) ncurses_runcmd( " mc " );
                   else if ( strcmp( cmdi , "ndesk" ) == 0 ) ncurses_runcmd( " ndesk " );
                   else if ( strcmp( cmdi , "nc" ) == 0 ) ncurses_runcmd( " nc " );
                   else if ( strcmp( cmdi , "nbash" ) == 0 ) ncurses_runcmd( " nbash " );
                   else if ( strcmp( cmdi , "nexplorer" ) == 0 ) ncurses_runcmd( " nexplorer " );

                   else if ( strcmp( cmdi , "lotus" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " freelotus123 " , fileselection  ); 
                   }

                   else if ( strcmp( cmdi , "size" ) == 0 )  
                   { 
                         def_prog_mode();
                         endwin();

                         strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                         if ( fexist( fileselection ) == 1 ) 
                            fseek_filesize( fileselection );

                         printf("%s", KBYEL );
                         printf( "<Press Return Key To Continue>\n" );
                         printf("%s", KNRM );

                         getchar();
                         reset_prog_mode();
                   } 


                   else if ( ( strcmp( cmdi , "find" ) == 0 )  
                   ||  ( strcmp( cmdi , "nfind" ) == 0 )  
                   ||  ( strcmp( cmdi , "search" ) == 0 )  )
                   { 
                       color_set( 7, NULL ); attron( A_REVERSE ); attroff( A_BOLD ); mvclear( ); 
                       foo = 3;
                       mvcenter( 0, "== NFIND ==" ); 
                       mvcenter( 1, "===========" ); 
                       mvprintw( foo++, 0, "DISPLAY: %d %d", rows, cols );
                       mvprintw( foo++, 0, "HOME:    %s ", getenv( "HOME" ));
                       mvprintw( foo++, 0, "USER:    %s ", getenv( "USER" ));
                       mvprintw( foo++, 0, "PATH:    %s ", getcwd( foostr, PATH_MAX ));
                       strncpy( foostr, strninput( rows/2 , "" ), PATH_MAX );
                       if ( strcmp( foostr, "" ) != 0 )  
                       {
                         def_prog_mode();
                         endwin();
                          listdirsearchfile( ".", 0, foostr );
                          printf( "<Press Key To Continue>\n" );
                          getchar();
                         reset_prog_mode();
                       }
                   } 


                   else if ( ( strcmp( cmdi , "display" ) == 0 )  
                   ||  ( strcmp( cmdi , "infos" ) == 0 )  
                   ||  ( strcmp( cmdi , "info" ) == 0 )  )
                   { 
                       color_set( 7, NULL ); attron( A_REVERSE ); attroff( A_BOLD ); mvclear( ); 
                       foo = 2;
                       mvcenter( 0, "INFORMATION" ); 
                       mvprintw( foo++, 0, "DISPLAY: %d %d", rows, cols );
                       mvprintw( foo++, 0, "HOME:    %s ", getenv( "HOME" ));
                       mvprintw( foo++, 0, "USER:    %s ", getenv( "USER" ));
                       mvprintw( foo++, 0, "PATH:    %s ", getcwd( foostr, PATH_MAX ));
                       mvprintw( foo++, 0, "HOME:    %s ", getenv( "HOME" ));
                       getch(); 
                   } 


                   else if ( strcmp( cmdi , "printfile" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        printfile_viewer( fileselection  ); 
                   }

                   else if ( strcmp( cmdi , "cp2tmp" ) == 0 ) 
                   {
                            strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                            strncpy( cmdi , " cp -r    " , PATH_MAX );
                            strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , fileselection , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , "  /tmp  " , PATH_MAX - strlen( cmdi ) -1 );
                            nruncmd( cmdi );
                   }

                   else if ( strcmp( cmdi , "odt2txt" ) == 0 ) 
                   {
                            strncpy( targetfile, fbasenoext( nwin_file[ winsel ] ) , PATH_MAX );
                            strncat( targetfile , ".txt" , PATH_MAX - strlen( targetfile ) -1 );
                            strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                            strncpy( cmdi , " odt2txt  " , PATH_MAX );
                            strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , fileselection , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , " > " , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , "  \"" , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , targetfile , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , "\"  " , PATH_MAX - strlen( cmdi ) -1 );
                            nruncmd( cmdi );
                   }

                   else if ( strcmp( cmdi , "du" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwait( " du -hs " ,  fileselection );
                   }

                   else if ( strcmp( cmdi , "test echo" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwait( " echo test " ,  fileselection );
                   }

                   else if ( strcmp( cmdi , "mount" ) == 0 ) 
                        ncurses_runcmd_keypress( " mount  " );

                   else if ( strcmp( cmdi , "df" ) == 0 ) 
                   {
                        //strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        //ncurses_runcmd( " df -h  ; read keypress " );
                        //ncurses_runcmd( " watch -e  df -h " );
                        ncurses_runcmd_keypress( "  df -h "  );
                   }

                   else if ( strcmp( cmdi , "copy" ) == 0 ) 
                   {
                     strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
                     strncpy( nwin_currentpath, nwin_path[ winsel ] , PATH_MAX );
                     strncpy( nwin_currentfile, nwin_file[ winsel ] , PATH_MAX );
                     if ( nwin_show[ 1 ] == 1 )
                     if ( nwin_show[ 2 ] == 1 )
                     if (( winsel == 1 ) || ( winsel == 2 ))
     	             ////if ( ( fexist( nwin_currentfile ) == 2 ) || ( fexist( nwin_currentfile ) == 1 ))
                     {
                        chdir( nwin_currentpath );
                        strncpy( cmdi, " cp -r  " , PATH_MAX );
                        strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                        strncat( cmdi ,   nwin_currentfile  , PATH_MAX - strlen(cmdi) - 1);
                        strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                        strncat( cmdi , "  " , PATH_MAX - strlen(cmdi) - 1);
                        if ( winsel == 2 ) foo = 1; else if ( winsel == 1 ) foo = 2; 
                        strncat( cmdi , " \"" , PATH_MAX - strlen(cmdi) - 1);
                        strncat( cmdi , nwin_path[ foo ] ,  PATH_MAX - strlen(cmdi) - 1);
                        strncat( cmdi , "\" " , PATH_MAX - strlen(cmdi) - 1);
                        color_set( 0, NULL ); attron( A_REVERSE ); 
                        mvprintw( 1,0," PATH:%s", getcwd( cwd, PATH_MAX) );
                        mvprintw( 3,0," CMD:%s", cmdi );
                        foo = ncwin_question( "copy this file ?" );
                        if ( foo == 1 )   ncurses_runcmd( cmdi );
                     }
               }

                   else if ( strcmp( cmdi , "lbo" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " libreoffice " , fileselection  ); 
                   }


                   else if ( strcmp( cmdi , "gnumeric" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        ncurses_runwith( " gnumeric " , fileselection  ); 
                   }


               else if ( strcmp( cmdi , "input" ) == 0 ) 
               {
                //  nc_mkdir();
                color_set( 10, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                color_set( 22 , NULL ); 
                gfxhline( 0 , 0, cols-1);
                mvcenter( 0, "| Text Input |");
                color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
                for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
                color_set( 0 , NULL ); mvprintw( rows-2, 0, "PATH: %s", getcwd( cwd, PATH_MAX ) ); 
                //////
                color_set( 23, NULL ); attron( A_REVERSE );
                gfxrectangle( rows/2-2 , 0 , rows/2+2  , cols-1);
                gfxframe(     rows/2-2 , 0 , rows/2+2  , cols-1);
                mvcenter( rows/2-2, "|Text Input Test|" ); 
                //////
                strncpy( foostr, strninput( rows/2 , "" ), PATH_MAX );
               }

            else if ( strcmp( cmdi , "svn" ) == 0 ) 
            {
                     strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
           	     chdir( nwin_path[ winsel ] );
                     strncpy( fileselection, nwin_currentfile , PATH_MAX );
      	             chdir(   nwin_path[ winsel ] );
                     attroff( A_REVERSE ); color_set( 0, NULL );
                     gfxhline( rows-2, 0, cols-1);
                     gfxhline( rows-1, 0, cols-1);
                     ////////
                     color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                     for( i = 0; i <= cols-1 ; i++) mvaddch( rows-2, i , ' ');
                     for( i = 0; i <= cols-1 ; i++) mvaddch( rows-1, i , ' ');
                     color_set( 10, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                     gfxhline( 0 , 0, cols-1);
                     mvcenter( 0, "|Internal Application Command|");
                     ////////
                     attroff( A_REVERSE ); attroff( A_BOLD ); color_set( 11, NULL ); 
                     gfxrectangle( 0+1 , 0, 3+1, cols-1);
                     gfxframe(     0+1 , 0, 3+1, cols-1);
                     mvprintw( 1+1, 2, "FILE: %s", fileselection );
                     mvprintw( 2+1, 2, "PATH: %s", getcwd( foostr  , PATH_MAX) );
                     foo = ndesk_menu_select( "1: SVN nmenu", "2: ", "3: ", "4: ", "5: " , "6: Delete Current File on SVN" , "9: Cancel" );
                     if ( foo == '1' ) 
                     {
                           ncurses_runcmd( " nmenu " );
                     }
                     else if ( foo == '6' ) 
                     {
                            strncpy( cmdi , "  svn delete  " , PATH_MAX );
                            strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi ,  fileselection , PATH_MAX - strlen( cmdi ) -1 );
                            strncat( cmdi , "\" --force " , PATH_MAX - strlen( cmdi ) -1 );
                            if ( fexist( fileselection ) == 1 ) 
                               ncurses_runcmd( cmdi );
                     }
            }





//////////////////////////////////////////////
//////////////////////////////////////////////
//////////////////////////////////////////////
else if ( strcmp( cmdi , "boom" ) == 0 ) 
{
         attroff( A_REVERSE ); color_set( 0, NULL );
         gfxhline( rows-2, 0, cols-1);
         gfxhline( rows-1, 0, cols-1);
         foo = ndesk_menu_select( "1: prboom solo mode", "2: File explorer", "3: prboom multiplayer netgame", "4: prboom alien", "5: eureka (wad editor)" , "6: Boom Game Server" , "Q: Quit!" );

         if ( foo == '1' ) 
         {
                strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                snprintf( foostr, PATH_MAX , " prboom-plus -solo-net -warp %s -file ", strninput( -1, "30"  ) );
                ncurses_runwith( foostr , fileselection  ); 
         }

         else if ( foo == '2' ) 
         {
         }

         else if  ( foo == '3' ) 
         {
                strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                ncurses_runwith(  strninput( -1 , " prboom-plus -net 192.168.52.11  -file " ) , fileselection  ); 
         }

         else if  ( foo == '4' ) 
         {
                strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                ncurses_runwith( " prboom-plus -iwad doom2.wad -file aaliens.wad  " , fileselection  ); 
         }

         else if  ( foo == '5' ) 
         {
                strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                ncurses_runwith( " eureka -iwad doom2.wad  " , fileselection  ); 
         }

         else if  ( foo == '6' ) 
         {
                strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                ncurses_runcmd( " xterm -e prboom-plus-game-server -N2 & " );
         }

         else if  ( foo == 'h' ) ndesk_help();
         else if  ( foo == 'H' ) ndesk_help();
         else if  ( foo == 'Q' ) ndesktop_gameover = 1;
         else if  ( foo == 'q' ) ndesktop_gameover = 1;
}


                   else if ( strcmp( cmdi , "clip2file" ) == 0 ) 
                   {
                        strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                        if ( fexist( fileselection ) == 1 )
                          nappend_text( fileselection  , ndesk_clipboard ); 
                   }


                   else if ( strcmp( cmdi , "colors" ) == 0 )
                   { crossgraphvga_colors( ); getch( ); }


                   else if ( strcmp( cmdi , "keys" ) == 0 )    win_keypress();
                   else if ( strcmp( cmdi , "key" ) == 0 )     win_keypress();

                   else if ( strcmp( cmdi , "keypress" ) == 0 )     win_keypress();

                   else if ( strcmp( cmdi , "nconsole" ) == 0 )   ncurses_runcmd( "nconsole" );
                   else if ( strcmp( cmdi , "nconsole2" ) == 0 )  ncurses_runcmd( "nconsole2" );


                   else if ( strcmp( cmdi , "unlink" ) == 0 ) nc_file_unlink();

                   else if ( strcmp( cmdi , "dateit" ) == 0 ) nc_file_dateit();

                   else if ( strcmp( cmdi , "unibeam" ) == 0 )
                       makeunimark( fileselection );

                   else if ( strcmp( cmdi , "unimark" ) == 0 )
                       makeunimark( fileselection );

                   else if ( strcmp( cmdi , "rename" ) == 0 )
                       nc_file_rename( fileselection );

                 }
                 break;

           case 'V':
                 ncurses_runcmd( " vim " );
                 break;


           case 'v':
	    foo = nwin_sel[winsel]; 
     	    chdir( nwin_path[ winsel ] );
            strncpy( fileselection, nwin_currentfile , PATH_MAX );
	    chdir(   nwin_path[ winsel ] );
            strncpy( cmdi , "  vim -p " , PATH_MAX );
            strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
            strncat( cmdi ,  nwin_file[ winsel ] , PATH_MAX - strlen( cmdi ) -1 );
            strncat( cmdi , "\"" , PATH_MAX - strlen( cmdi ) -1 );
	    if ( nwin_app[ winsel ] >= 1 )
            ncurses_runcmd( cmdi );
	    break;

	   case '!':
	      if ( ( nwin_app[ winsel ] == 1 ) || ( nwin_app[ winsel ] == 2 ) 
	      || ( nwin_app[ winsel ] == 22 ) || ( nwin_app[ winsel ] == 23 )
	      || ( nwin_app[ winsel ] >= 1 ) )
	      {
     	        chdir( nwin_path[ winsel ] );
                strncpy( fileselection, nwin_currentfile , PATH_MAX );
                color_set( 11, NULL ); attroff( A_REVERSE );
     	        mvprintw( 0, 0, "PATH: %s", getcwd( foocwd, PATH_MAX ) );
     	        mvprintw( 1, 0, "FILE: %s", fileselection );
	        //if ( fexist( nwin_currentfile ) == 1 )
		{ 
                   //strncpy( fileselection, nwin_currentfile , PATH_MAX );
                   strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
                   ncurses_runwith( strninput( -1, "" ) , fileselection  ); 
		}
	      }
           break;


	   case ';':
	        if ( fexist( nwin_currentfile ) == 1 )
		{
                 color_set( 0, NULL ); attroff( A_REVERSE ); attroff( A_BOLD );
                 strncpy( cwd, ncwin_inputbox( "LAUNCHER",  "" ), PATH_MAX );
                 if ( strcmp( cwd , "" ) != 0 ) 
                 {
                   strncpy( cmdi, " nclauncher  " , PATH_MAX );
                   strncat( cmdi , cwd , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "  \"" , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi ,   nwin_currentfile   , PATH_MAX - strlen(cmdi) - 1);
                   strncat( cmdi , "\"  " , PATH_MAX - strlen(cmdi) - 1);
                   ncruncmd( cmdi );
                 }
		}
	        ncerase(); 
                break;


           case KEY_GCTRLO:
	     chdir( nwin_path[ winsel ] );
             colorblack(); attron( A_REVERSE ); nchspace(  1, 0 , cols-1 );
             mvprintw( 1, 0 , "%s" , "CHDIR" );
             //strncpy( fooline , ncwin_inputbox( "CHDIR", ""  ), PATH_MAX );
             strncpy( fooline , strninput( -1, "" ), PATH_MAX );
	     if ( fexist( fooline ) == 2 )
	     {
                  chdir( fooline );
	          strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                  nwin_sel[winsel] = 1;
	     }
	     break;

	     case 'i':
               if ( ( winsel == 1 ) || ( winsel ==  2 ) )
               {
                 if ( winsel == 1 )
                    winsel = 2;
                 else
                    winsel = 1;
                 chdir( nwin_path[winsel] );
                 nwin_show[winsel] = 1;
               }
               else
               {
                 nwin_show[winsel]=0;
                 winsel--;
                 if ( nwin_sel[winsel] == 0 ) nwin_sel[winsel] = 1;
	         if ( winsel <= 0 ) winsel = 1;
               }
	       break;


	   case 'X':
              ndesk_menu_beon( nwin_file[ winsel ] );
              nwin_sel[winsel] = 1;
	      nwin_scrolly[winsel] = 0;
	      break;


	     case 'x':
	      if ( ( nwin_app[ winsel ] == 2 ) &&  ( nwin_show[ winsel ] == 1 ))
              {
                nwin_sel[winsel] = tempsel[winsel] ; 
                nwin_scrolly[winsel] = tempscroll[winsel] ; 
                nwin_app[winsel] = 1;
              }
             else if (( nwin_app[ winsel ] == 1 ) && ( nwin_show[ winsel ] == 1 ))
             {
                //nwin_sel[winsel] = tempsel[winsel] ; 
                //nwin_scrolly[winsel] = tempscroll[winsel] ; 
                nwin_app[winsel] = 1;
                attroff( A_BOLD );
                strncpy( ndesk_status_message, "Explore (view)", PATH_MAX );

	        strncpy( nc_file_filter[ winsel ] , "", PATH_MAX );
	        strncpy( nwin_filter[ winsel ]    , "", PATH_MAX ); // for tinyviewer
                nwin_scrolly[winsel] = 0;
                nwin_sel[winsel] = 1;
             }
             else
             {
              //nwin_sel[winsel] = tempsel[winsel];
              // nwin_scrolly[winsel] = tempscroll[winsel];
                 nwin_app[winsel] = 1;
                 nwin_app[ winsel ]  = 1; 
                 nwin_show[ winsel ]  = 1; 
                 showhiddenfiles = 0;
                 //nwin_sel[winsel] = 1; nwin_scrolly[winsel] = 0;  // <- top of line sel
                 attroff( A_BOLD );
                 strncpy( ndesk_status_message, "File e[X]plorer (no filter)", PATH_MAX );
	              strncpy( nwin_filter[ winsel ] , "", PATH_MAX ); 
                      nwin_sel[winsel] = 1;
	              nwin_scrolly[winsel] = 0;
                 }
	         break;



                       case '&':
	                 if ( colorscheme == 1 ) 
			 colorscheme = 0; 
			 else
			 colorscheme = 1; 
			 break;



           case '>':
           case ')':
              nwin_x1[winsel]+=6;
              nwin_x2[winsel]+=6;
	      break;
           case '<':
           case '(':
              nwin_x1[winsel]-=6;
              nwin_x2[winsel]-=6;
	      break;




           /*
           case KEY_GCTRLT:
	       ncruncmd( " pwd ; nctoolmenu " );
	       break;

           case 'L':
              nwin_x2[winsel]++;
	      break;
           case 'H':
              nwin_x2[winsel]--;
	      break;
           case 'J':
              nwin_y2[winsel]++;
	      break;
           case 'K':
              nwin_y2[winsel]--;
	      break; */


         case 234234: // it was 'b'
	      if ( nwin_app[ winsel ] == 0 ) nwin_app[ winsel ] = 1;
	      foo = nwin_sel[winsel]; 
	      if ( ( nwin_app[ winsel ] == 1 ) || ( nwin_app[ winsel ] == 23 ))
	      {
	         if ( fexist( nwin_currentfile ) == 2 )
                 {
                    chdir( nwin_currentfile );
	            strncpy( nwin_path[winsel+1], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                    nwin_app[winsel+1] = 1; 

                    nwin_show[ winsel+1  ] = 1;
                    nwin_show[winsel+1] = 1; 
	            nwin_scrolly[winsel+1] = 0;
	            nwin_sel[winsel+1] = 1;
                    winsel = winsel +1;
                 } 
	         else if ( fexist( nwin_currentfile ) == 1 )
                 {
                   if ( nwin_app[winsel+1] == 1 ) nwin_app[winsel+1] = 2; 
     	           strncpy( nwin_path[ winsel+1], nwin_path[winsel], PATH_MAX );
                   nwin_show[winsel+1] = 1; 
	           nwin_scrolly[winsel+1] = 0;
	           nwin_sel[winsel+1] = 1;
                 } 
                 else
                 {
     	           strncpy( nwin_path[ winsel+1], nwin_path[winsel], PATH_MAX );
                   nwin_show[winsel+1] = 1; 
	           nwin_scrolly[winsel+1] = 0;
	           nwin_sel[winsel+1] = 1;
                 } 
		 ////////
     	         strncpy( nwin_file[ winsel+1], nwin_currentfile, PATH_MAX );
	      }
	      break;






           case 'A':
           case 1:
              ///////////////////////////////////
              if ( nwin_fullscreen[winsel] == 0 )
               nwin_fullscreen[winsel] = 1;
              else 
               nwin_fullscreen[winsel] = 0;
               ///////////////////////////////////
               if( nwin_fullscreen[winsel] == 1)
               {
                  nwin_x1[winsel]=0;
                  nwin_x2[winsel]=cols-1;
                  nwin_y1[winsel]=1;
                  nwin_y2[winsel]=rows-3;
               }
               else if( nwin_fullscreen[winsel] == 0)
               {
                  foo = winsel;
                  nwin_x1[foo]= 2+foo;
                  nwin_x2[foo]= 30+foo;
                  nwin_y1[foo]= 2+foo;
                  nwin_y2[foo]= rows-4;
               }
	      break;




    case 28323:
              strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
              if ( fexist( fileselection ) == 1 )
              {
                ////////////////////////////////////// 
                char mytargetfile[PATH_MAX];
                strncpy( mytargetfile, getenv( "HOME" ) , PATH_MAX );
                strncat( mytargetfile , "/" , PATH_MAX - strlen( mytargetfile ) - 1);
                strncat( mytargetfile , "doc.mrk" , PATH_MAX - strlen( mytargetfile ) - 1);
                ncp( mytargetfile , fileselection );
    	        nruncmd( " cd ; unibeam doc.mrk ; pdflatex    -shell-escape --interaction=nonstopmode    doc ; bibtex doc ; pdflatex   -shell-escape --interaction=nonstopmode     doc ; pdflatex   -shell-escape --interaction=nonstopmode     doc ; screen -d -m mupdf doc.pdf  " );
                ////////////////////////////////////// 
              }
              break;



    case KEY_F(11):
      foo = ncwin_question( "Compile this file (local)?" );
      if ( foo == 1 ) 
      {
              strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
              if ( fexist( fileselection ) == 1 )
              {
                char mytargetfile[PATH_MAX];
                ////////////////////////////////////// 
                strncpy( mytargetfile, getenv( "HOME" ) , PATH_MAX );
                strncat( mytargetfile , "/" , PATH_MAX - strlen( mytargetfile ) - 1);
                strncat( mytargetfile , "doc.mrk" , PATH_MAX - strlen( mytargetfile ) - 1);
                //ncp( mytargetfile , fileselection );
    	        //nruncmd( " cd ; unibeam doc.mrk ; pdflatex   -shell-escape --interaction=nonstopmode     doc.tex ; screen -d -m mupdf doc.pdf " );
                /////////////////////////////////// 
                makeunimark( fileselection );
                ////////////////////////////////////// 
              }
       }
       break;


    case KEY_F(12):
              strncpy( fileselection, nwin_file[ winsel ] , PATH_MAX );
              if ( fexist( fileselection ) == 1 )
              {
                ////////////////////////////////////// 
                char mytargetfile[PATH_MAX];
                strncpy( mytargetfile, getenv( "HOME" ) , PATH_MAX );
                strncat( mytargetfile , "/" , PATH_MAX - strlen( mytargetfile ) - 1);
                strncat( mytargetfile , "doc.mrk" , PATH_MAX - strlen( mytargetfile ) - 1);
                ncp( mytargetfile , fileselection );
    	        //nruncmd( " cd ; unibeam doc.mrk ; pdflatex    -shell-escape --interaction=nonstopmode    doc ; bibtex doc ; pdflatex   -shell-escape --interaction=nonstopmode    doc ; pdflatex    -shell-escape --interaction=nonstopmode    doc ; screen -d -m mupdf doc.pdf  " );
    	        nruncmd( " cd ; unibeam doc.mrk ; pdflatex    -shell-escape --interaction=nonstopmode    doc ; bibtex doc ; pdflatex   -shell-escape --interaction=nonstopmode    doc ; pdflatex    -shell-escape --interaction=nonstopmode    doc  " );
                if ( strcmp( getenv( "TERM" ) , "linux" ) == 0 ) 
                   nruncmd( " cd ; fbgs -r 270  doc.pdf " );
                else
                   nruncmd( " cd ; screen -d -m mupdf  doc.pdf " );
                ////////////////////////////////////// 
              }
              break;




           ///////////////// main
           case 'k':
	   case KEY_UP:
              nwin_sel[winsel]--;
	      break;

           case 'j':
	   case KEY_DOWN:
              nwin_sel[winsel]++;
	      break;

	   case 'h':
	   case KEY_LEFT:
              chdir( ".." );
	      strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
              nwin_sel[winsel] = 1;
	      nwin_scrolly[winsel] = 0;
	      strncpy( nc_file_filter[ winsel ], "", PATH_MAX);
	      strncpy( nwin_filter[ winsel ], "", PATH_MAX);
	      break;

           case 'L':
              nwin_x2[winsel]++;
	      break;
           case 'H':
              nwin_x2[winsel]--;
	      break;
           case 'J':
              nwin_y2[winsel]++;
	      break;
           case 'K':
              nwin_y2[winsel]--;
	      break;

	   case 'l':
	   case KEY_RIGHT:
	      foo = nwin_sel[winsel]; 
	      if ( fexist( nwin_currentfile ) == 2 )
	      {
                  chdir( nwin_currentfile );
	          strncpy( nwin_path[winsel], getcwd( foocwd, PATH_MAX) , PATH_MAX);
                  nwin_sel[winsel] = 1;
	          nwin_scrolly[winsel] = 0;
	          strncpy( nc_file_filter[ winsel ], "", PATH_MAX);
	          strncpy( nwin_filter[ winsel ], "", PATH_MAX);
	      }
	      break;

           case 's':
              nwin_scrolly[winsel]++;
	      break;


      }

    }

    curs_set( 1 ) ;
    attroff( A_BOLD ); attroff( A_REVERSE ); curs_set( 1 );
    endwin();		/* End curses mode  */
    printf(" NDESKTOP: Bye ! \n" );
    
    return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dbgout.h"

#define DBG_OUT_TO stderr
#define MAX_STR_LEN 256

size_t strlcat(char *dst,const char *src,size_t siz);

/* script for check colour
 *
 * for i in {0..255} ; do
 * printf "\x1b[48;5;%sm%3d\e[0m " "$i" "$i"
 * if (( i == 15 )) || (( i > 15 )) && (( (i-15) % 6 == 0 )); then
 * printf "\n";
 * fi
 * done
 *
 * https://askubuntu.com/questions/821157/print-a-256-color-test-pattern-in-the-terminal
 */

static const char *colstr[DBG_COLOUR_NUM] = {
  "\x1b[31m", /* red */
  "\x1b[32m", /* green */
  "\x1b[33m", /* yellow */
  "\x1b[34m", /* blue */
  "\x1b[35m", /* magenta */
  "\x1b[36m", /* cyan */
  "\x1b[37m", /* white */
  "\x1b[39m", /* default */
};
static const char *colstr_bg[DBG_BG_COLOUR_NUM] = {
  "\x1b[49m", /* default */
  "\x1b[41m", /* red */
  "\x1b[42m", /* green */
  "\x1b[43m", /* yellow */
  "\x1b[44m", /* blue */
  "\x1b[45m", /* magenta */
  "\x1b[46m", /* cyan */
  "\x1b[48;5;8m", /* gray */
  "\x1b[48;5;17m", /* navy-blue */
  "\x1b[48;5;53m", /* cotton-pink */
};

/* cl
 *  0-7  bits: str_colour
 *  8-15 bits: bg_colour
 */
void dbgf_(int cl,const char *fmt,...){
  va_list lst;
  char msg[MAX_STR_LEN] = {'\0',};
  char buf[MAX_STR_LEN];

  int colour = cl & 0xFF;
  int colour_bg = cl >> 8;

  if(colour < 0 || colour >= DBG_COLOUR_NUM){
    fprintf(DBG_OUT_TO,"Invalid colour\n");
    colour = DBG_COLOUR_DEFAULT;
  }
  if(colour_bg < 0 || colour_bg >= DBG_BG_COLOUR_NUM){
    fprintf(DBG_OUT_TO,"Invalid bg-colour\n");
    colour_bg = DBG_BG_COLOUR_DEFAULT;
  }
    
  
  va_start(lst,fmt);

  const char *s;
  const char *ss,*es;
  int i;
  for(s=fmt,i=0; *s != '\0'; s++,i++){
    if(*s == '%'){
      
      switch(*++s){
      case 'd':
	sprintf(buf,"%d",va_arg(lst,int));
	strlcat(msg,buf,sizeof(msg));
	continue;

      case 'u':
	sprintf(buf,"%u",va_arg(lst,unsigned int));
	strlcat(msg,buf,sizeof(msg));
	continue;

      case 'f':
	sprintf(buf,"%f",va_arg(lst,double));
	strlcat(msg,buf,sizeof(msg));
	continue;
	
      case 'c':
	sprintf(buf,"%c",va_arg(lst,int));
	strlcat(msg,buf,sizeof(msg));
	continue;
	
      case 's':
	sprintf(buf,"%s",va_arg(lst,char *));
	strlcat(msg,buf,sizeof(msg));
	continue;

      default:
	fprintf(DBG_OUT_TO,"Invalid format string\n");
	break;
      }
    }

    ss = es = s;
    while( (*es != '\0') &&
	   (*es !=  '%') )
      es++;

    if(*es == '\0'){
      sprintf(buf,"%s",ss);
      strlcat(msg,buf,sizeof(msg));
      break;
    }
    if(*es == '%'){

      if((size_t)(es - ss) >= sizeof(buf)){
	fprintf(DBG_OUT_TO,"Too long string\n");
	break;
      }
      strncpy(buf,ss,es-ss);
      buf[es-ss] = '\0';
      strlcat(msg,buf,sizeof(msg));

      s += (es-ss)-1;
    }
    
  }

  int last = strlen(msg) - 1;
  char lc = msg[last];
  if(lc == '\n' || lc == ' '){
    msg[last] = '\0';
    sprintf(buf,"\x1b[0m\x1b[K%c",lc);
    strlcat(msg,buf,sizeof(msg));
  }else{
    strlcat(msg,"\x1b[0m\x1b[K",sizeof(msg));
  }
  
  fprintf(DBG_OUT_TO,"%s%s%s",
	  colstr_bg[colour_bg],
	  colstr[colour],
	  msg);
    
  //fflush(DBG_OUT_TO);
  
  va_end(lst);
}

/* strlcat */
/* derived from FreeBSD kernel */
size_t strlcat(char *dst,const char *src,size_t siz){
  char *d = dst;
  const char *s = src;
  size_t n = siz;
  size_t dlen;

  /* Find the end of dst and adjust bytes left but don't go past end */
  while (n-- != 0 && *d != '\0')
    d++;
  dlen = d - dst;
  n = siz - dlen;

  if (n == 0)
    return(dlen + strlen(s));
  while (*s != '\0') {
    if (n != 1) {
      *d++ = *s;
      n--;
    }
    s++;
  }
  *d = '\0';

  return(dlen + (s - src));       /* count does not include NUL */
}

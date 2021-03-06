#include <stdio.h>
#include "mandelbro.h"
#include <glib.h>
#include <uuid/uuid.h>
#include <QThread>
#include <QObject>
#include "dbgout.h"
#include "task_spawner.h"
#include <string>

#ifdef CPPLINDA
#include <linda/server.hpp>
#include <linda/datatypes/base_type.hpp>
extern LINDA::TUPLESPACE *linda_server;
#else
#include "tgroup.h"
#endif

#define DEFAULT_MAX_ITERATE 1024
static size_t strlcpy(char *dst,const char *src,size_t siz);

mandelbro::mandelbro(){
}

mandelbro::mandelbro(tile *parent,struct area_coord *crd,int w,int h,int de){
  dbgf(DBG_CL(BLUE,WHITE),"MANDELBRO NEWED\n");
  ptile = parent;

  div_h = de & 0xff;
  div_v = de >> 8;

  dw = w/div_h;
  dh = h/div_v;

  this->w = w;
  this->h = h;

  uuid_t uuid;
  uuid_generate(uuid);
  uuid_unparse_upper(uuid,uuid_s);
#ifndef CPPLINDA
  std::string tgid(uuid_s);
  add_tgid(tgid);
#endif
  
  texture_data = new GLubyte*[div_v*div_h];
  for(int i=0;i<(div_v * div_h);i++){
    //texture_data[i] = new GLubyte[3 * dw * dh];
    texture_data[i] = NULL;
  }

  struct tsp_task_info ti;
  ti.crd = *crd;
  ti.dw = dw;
  ti.dh = dh;
  ti.div_h = div_h;
  ti.div_v = div_v;
  strlcpy(ti.uuid_s,uuid_s,sizeof(uuid_s));
  
  QThread *thread_sl = new QThread;
  task_spawner *tsp = new task_spawner(&ti);
  tsp->moveToThread(thread_sl);
  QObject::connect(thread_sl,SIGNAL(started()),tsp,SLOT(process()));
  QObject::connect(tsp,SIGNAL(finished()),thread_sl,SLOT(quit()));
  QObject::connect(tsp,SIGNAL(finished()),tsp,SLOT(deleteLater()));
  QObject::connect(thread_sl,SIGNAL(finished()),thread_sl,SLOT(deleteLater()));
  thread_sl->start();
  
  x_pix_num = w;
  y_pix_num = h;
  max_iterate = DEFAULT_MAX_ITERATE;

//  x_pix_length = (x_e - x_s)/(double)x_pix_num;
 // y_pix_length = (y_e - y_s)/(double)y_pix_num;
  half_x_pix_length = x_pix_length / 2.0;
  half_y_pix_length = y_pix_length / 2.0;

  res = new int[x_pix_num * y_pix_num];
  printf("size is %dx%d\n", x_pix_num, y_pix_num);	
  if(res == NULL)
    fprintf(stderr,"new fail\n");
}

mandelbro::~mandelbro(){
  dbgf(DBG_CL(RED,CYAN),"mandelbro Deleted\n");
  delete[] res;

#if 0
  for(int i=0;i<div_v;i++){
    for(int j=0;j<div_h;j++){
      delete[] texture_data[i][j];
    }
    delete[] texture_data[i];
  }
  delete[] texture_data;
#endif
  
  for(int i=0;i<(div_v * div_h);i++){
    if(texture_data[i] != NULL)
      delete[] texture_data[i];
  }
  delete[] texture_data;
}

void mandelbro::update_rectangle(int x,int y,unsigned char *ud){
  int *res = this->res;

  res += y * w * dh;
  res += x * dw;
  for(int i=0;i<dh;i++){
    memcpy(res,ud,sizeof(int)*dw);
    res += w;
    ud += sizeof(int)*dw;
  }
}

GLubyte *mandelbro::get_texture_data(int x,int y){
  return texture_data[div_h*y+x];
}

void mandelbro::create_subtile_tex(int x,int y,unsigned char *ud){
  texture_data[div_h*y+x] = new GLubyte[3 * dw * dh];
  GLubyte *d = texture_data[div_h*y+x];
  unsigned int *uud = (unsigned int *)ud;

  for(int i=0;i<dh;i++){
    for(int j=0;j<dw;j++){
      d[3*(dw*i+j)+0] = uud[dw*i+j]/4;
      d[3*(dw*i+j)+1] = uud[dw*i+j]/2;
      d[3*(dw*i+j)+2] = uud[dw*i+j]/4;
    }
  }
  
}

int mandelbro::calc(){
#ifdef CPPLINDA
  LINDA::TYPE::String id(uuid_s);
  LINDA::TYPE::Integer pos;
  LINDA::TYPE::String result;
  LINDA::TYPE::Double x_s,y_s;
  gsize out_len;

  for(int i=0;i<div_v;i++)
    for(int j=0;j<div_h;j++){
      pos.toFormal();
      result.toFormal();
      
      //  printf("\t RECV-id is %s\n",id().c_str());
      // fprintf(stdout,"waiting for res\n");
      //  linda_server->in(&x_s,&y_s,&result);
      linda_server->in(&id,&pos,&result);
      // fprintf(stderr,"\t@@@ @@@ got res\n");
      //  memset(res,1,1024*1024*4);
      // printf("\t pos:%dx%d\n",pos()&0xff,pos()>>8);
      guchar *res_data;
      res_data = g_base64_decode(result().c_str(),&out_len);
      //      if( ((pos()&0xFF) == 0) &&
      //	  ((pos()>>8)   == 1))
      update_rectangle(pos()&0xFF,pos()>>8,res_data);
      create_subtile_tex(pos()&0xFF,pos()>>8,res_data);
      ptile->query_subtile_update(pos()&0xFF,pos()>>8);
      ptile->notify_subtile_update();
      
      // printf("out_len is %ld\n", out_len);
      //memcpy(res,res_data,out_len);
      g_free(res_data);
    }
#else /* CPPLINDA */
  std::string tgid(uuid_s);
  struct entry *e;
  struct dlst *rsltlst;
  int pos;
  unsigned char *data;
  
  for(int i=0;i<div_v;i++)
    for(int j=0;j<div_h;j++){
      rsltlst = get_result_list(tgid);
      e = dlst_deQ(rsltlst);
      pos = ((struct mandel_task_result *)(&e->data))->pos;
      data = (unsigned char *)((struct mandel_task_result *)(&e->data))->data;
      update_rectangle(pos&0xFF,pos>>8,data);
      create_subtile_tex(pos&0xFF,pos>>8,data);
      ptile->query_subtile_update(pos&0xFF,pos>>8);
      ptile->notify_subtile_update();
      dlst_free_entry(e);
    }
  rm_tgid(tgid);
  printf("rm tgid\n");
  
#endif
  

#if 0
  int i,j,k;
  double c_x,c_y;
  double a,b,_a,_b;

  c_x = x_s + half_x_pix_length;
  c_y = y_s + half_y_pix_length;

  for(i=0;i<y_pix_num;i++){
    for(j=0;j<x_pix_num;j++){
      a=0;
      b=0;  
      for(k=0;k<max_iterate;k++){
	_a = a * a - b * b + c_x;
	_b = 2 * a * b + c_y;
	if(a*a + b*b > 4.0)
	  break;
	a = _a;
	b = _b;
      }
      *(res + y_pix_num*i + j) = k;
      c_x += x_pix_length;
    }
    c_x = x_s + half_x_pix_length;
    c_y += y_pix_length;
  }
#endif
  
  return 0;
}

int *mandelbro::get_mandel_res(){
  return res;
}

void mandelbro::show(){
  printf("x_s: %f\n",x_s_);
  printf("y_s: %f\n",y_s_);
  printf("x_e: %f\n",x_e_);
  printf("y_e: %f\n",y_e_);
  printf("x_pix_num: %d\n",x_pix_num);
  printf("y_pix_num: %d\n",y_pix_num);
  printf("max_iterate: %d\n",max_iterate);
  printf("x_pix_length: %f\n",x_pix_length);
  printf("y_pix_length: %f\n",y_pix_length);
  printf("half_x_pix_length: %f\n",half_x_pix_length);
  printf("half_y_pix_length: %f\n",half_y_pix_length);
}

// strlcpy()
// derived from FreeBSD kernel
static size_t strlcpy(char *dst,const char *src,size_t siz){
  char *d = dst;
  const char *s = src;
  size_t n = siz;

  /* Copy as many bytes as will fit */
  if (n != 0) {
    while (--n != 0) {
      if ((*d++ = *s++) == '\0')
	break;
    }
  }
  
  /* Not enough room in dst, add NUL and traverse rest of src */
  if (n == 0) {
    if (siz != 0)
      *d = '\0';              /* NUL-terminate dst */
    while (*s++)
      ;
  }

  return(s - src - 1); /* count does not include NUL */
}

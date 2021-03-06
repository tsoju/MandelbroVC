#include "calc.h"
#include <glib.h>
#include <stdio.h>
#include <string>

#ifdef CPPLINDA
#include <linda/server.hpp>
#include <linda/datatypes/base_type.hpp>
extern LINDA::TUPLESPACE *linda_server;
#else /* CPPLINDA */
#include "sp_server.h"
#include "tgroup.h"
#include "dlst.h"
#include "task_spawner.h"
#endif

#define DEFAULT_MAX_ITERATE 511

calc::calc(){
  for(int i=0;i<THREAD_NUM;i++){
    pthread_create(thread_handle+i,NULL,calc::worker,(void *)(long)i);
  }

}

calc::~calc(){
  printf("calc is deleting\n");
#ifdef CPPLINDA
  LINDA::TYPE::Integer pos;
  LINDA::TYPE::Double x_s,x_e;
  LINDA::TYPE::Double y_s,y_e;
  LINDA::TYPE::Integer _w,_h;
  LINDA::TYPE::String id("end");

  for(int i=0;i<THREAD_NUM;i++){
    linda_server->out(&id,&pos,&x_s,&y_s,&x_e,&y_e,&_w,&_h);
  }
#endif /* CPPLINDA */
}

void *calc::worker(void *arg){
  (void)arg;
#ifdef CPPLINDA
  std::string *result_string;
  LINDA::TYPE::String result;
  LINDA::TYPE::String id;
  LINDA::TYPE::Integer pos;
  LINDA::TYPE::Double x_s,x_e;
  LINDA::TYPE::Double y_s,y_e;
  LINDA::TYPE::Integer _w,_h;

  double _x_s,_x_e;
  double _y_s,_y_e;
  int __w,__h;
  
  int i,j,k;
  double c_x,c_y;
  double a,b,_a,_b;
  double x_pix_length,y_pix_length;
  double half_x_pix_length,half_y_pix_length;

  int *res;
  

  while(1){
    // printf("$$$ worker# %ld is waiting in\n",(long)arg);
    id.toFormal();
    pos.toFormal();
    x_s.toFormal();
    y_s.toFormal();
    x_e.toFormal();
    y_e.toFormal();
    _w.toFormal();
    _h.toFormal();
    linda_server->in(&id,&pos,&x_s,&y_s,&x_e,&y_e,&_w,&_h);

    if(id().compare("end") == 0){
      printf("end!!!!\n");
      break;
    }
    
    
    // printf("worker #%ld is running\n",(long)arg);

    _x_s = x_s();
    _y_s = y_s();
    _x_e = x_e();
    _y_e = y_e();
    __w = _w();
    __h = _h();

    x_pix_length = (_x_e - _x_s)/(double)__w;
    y_pix_length = (_y_e - _y_s)/(double)__h;
    half_x_pix_length = x_pix_length /  2.0;
    half_y_pix_length = y_pix_length /  2.0;

    c_x = _x_s + half_x_pix_length;
    c_y = _y_s + half_y_pix_length;
    //res = new int[__w * __h];
    res = new int[__w * __h];
    
    for(i=0;i<__h;i++){
      for(j=0;j<__w;j++){
	a=0;
	b=0;
	for(k=0;k<DEFAULT_MAX_ITERATE;k++){
	  _a = a * a - b * b + c_x;
	  _b = 2 * a * b + c_y;
	  if(a*a + b*b > 4.0)
	    break;
	  a = _a;
	  b = _b;
	}
	*(res + __h*i + j) = k;
	c_x += x_pix_length;
      }
      c_x = _x_s + half_x_pix_length;
      c_y += y_pix_length;
    }
    gchar *base64;
    base64 = g_base64_encode((unsigned char *)res,sizeof(int) * __w * __h);

    //printf("%s",base64);

    // printf("length :%ld\n",strlen(base64));
    result_string = new std::string(base64);
    result = *result_string;
    delete result_string;

    // printf("@@@ result OUT, worker# %ld\n",(long)arg);
    // printf("x_s: %15.14f\n",x_s());

    /* for debug */
    //int rand_time = rand() % 1000000;
    //usleep(rand_time);
      
    linda_server->out(&id,&pos,&result);
    
    //    delete result_string;

    g_free(base64);
    delete[] res;
  }

#else

  std::string tgid;
  double x_pix_length,y_pix_length;
  double half_x_pix_length,half_y_pix_length;
  double c_x,c_y;
  struct mandel_task_result *ri;
  int *res;
  int i,j,k;
  double a,b,_a,_b;
  struct entry *e;
  struct mandel_task_info mti;
  std::string tname;

  while(1){
    printf("worker #%ld is running\n",(long)arg);
    // /* int retval = */get_first_tgid(&tgid);
    //    printf("worker got work: %s\n",tgid.c_str());
    //struct dlst *tsklst = get_task_list(tgid);
    //e = dlst_deQ(tsklst);
    get_task(&tname,&mti);
    
    //mti = (struct mandel_task_info *)dlst_getdata_void(e);
    
    x_pix_length = (mti.x_e - mti.x_s)/(double)mti.w;
    y_pix_length = (mti.y_e - mti.y_s)/(double)mti.h;
    half_x_pix_length = x_pix_length /  2.0;
    half_y_pix_length = y_pix_length /  2.0;
    
    c_x = mti.x_s + half_x_pix_length;
    c_y = mti.y_s + half_y_pix_length;
    //    res = new int[mti.w * mti.h];

    int risize =
      sizeof(mandel_task_result) -
      sizeof(unsigned char) +
      sizeof(int) * mti.w * mti.h;
    ri = (struct mandel_task_result *)malloc(risize);
    res = ri->data;

    ri->pos = mti.pos;
    
    for(i=0;i<mti.h;i++){
      for(j=0;j<mti.w;j++){
	a=0;
	b=0;
	for(k=0;k<DEFAULT_MAX_ITERATE;k++){
	  _a = a * a - b * b + c_x;
	  _b = 2 * a * b + c_y;
	  if(a*a + b*b > 4.0)
	    break;
	  a = _a;
	  b = _b;
	}
	*(res + mti.h*i + j) = k;
	c_x += x_pix_length;
      }
      c_x = mti.x_s + half_x_pix_length;
      c_y += y_pix_length;
    }
    
    struct dlst *rsltlst = get_result_list(tname);
    // printf("enQ before\n");
    dlst_enQ(rsltlst,risize,ri);
    // printf("enQ after\n");
    
    //dlst_free_entry(e);
    free(ri);
  }
#endif
    
  return NULL;
}

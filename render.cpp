#include <stdio.h>
#include <math.h>
#include "render.h"
#include "tile.h"
#include <GL/glu.h>

#include <omp.h>
#include "sslib.h"

#include "dbgout.h"

//#define WIDTH 4096
//#define HEIGHT 4096

#define WIDTH 1024
#define HEIGHT 1024

#define CAMERA_MV_UNIT 1.0
#define CAMERA_ZOOM_UNIT 8.0
#define CAMERA_PAN_UNIT (1.0/8.0)
#define COUNT_NONE 0
#define COUNT_MV 82
#define COUNT_ZOOM 96
#define COUNT_PAN COUNT_ZOOM
#define COUNT_MVZM 96
#define COUNT_SZOOM 42
#define COUNT_SPAN COUNT_SZOOM
#define COUNT_GOTO 96
#define SZOOM_MAG 1.25
#define SZOOM_MAG_INCREMENT 1.125
#define SPAN_MAG 0.8
#define SPAN_MAG_INCREMENT 0.8888

#define TL_NUM 5 /* 中心のタイルを除いた片方向のtile数 */

int render::vpt_wh = INITIAL_VPORT_WIDTH;
int render::vpt_ht = INITIAL_VPORT_HEIGHT;

//#define pos_to_crdx(pos) (2.0*pos/(mag_x * base_mag))
//#define pos_to_crdy(pos) (2.0*pos/(mag_y * base_mag))
#define pos_to_crdx(pos) (center_crd_x + (2.0*(pos))/base_mag)
#define pos_to_crdy(pos) (center_crd_y + (2.0*(pos))/base_mag)
#define  pos_to_len(pos) (-(2.0*(pos))/base_mag)
#define crd_to_posx(crd) (((crd)-center_crd_x)*base_mag*0.5)
#define crd_to_posy(crd) (((crd)-center_crd_y)*base_mag*0.5)

render::render(){
}

static struct scene type[] = {
  /*                  First,Anime,prepare,cnt_max,    cnt */
  {RENDER_MODE_NORMAL,1,    0,    1,      COUNT_NONE, 0,
   &render::render_normal,&render::pre_normal,NULL},
  {RENDER_MODE_ZOOM,  1,    1,    1,      COUNT_ZOOM, 0,
   &render::render_zoom,  &render::pre_zoom,  &render::fin_zoom},
  {RENDER_MODE_PAN,   1,    1,    1,      COUNT_PAN,  0,
   &render::render_pan,   &render::pre_pan,   &render::fin_pan},
  {RENDER_MODE_MV,    1,    1,    1,      COUNT_MV,   0,
   &render::render_mv,    &render::pre_mv,    &render::fin_mv},
  {RENDER_MODE_MV_POS,1,    1,    1,      COUNT_MV,   0,
   &render::render_mvps,  &render::pre_mvps,  &render::fin_mvps},
  {RENDER_MODE_MVZM,  1,    1,    1,      COUNT_MVZM, 0,
   &render::render_mvzm,  &render::pre_mvzm,  &render::fin_mvzm},
  {RENDER_MODE_SZOOM, 1,    1,    1,      COUNT_SZOOM,0,
   &render::render_szoom, &render::pre_szoom, &render::fin_szoom},
  {RENDER_MODE_SPAN,  1,    1,    1,      COUNT_SPAN, 0,
   &render::render_span,  &render::pre_span,  &render::fin_span},
  {RENDER_MODE_GOTO,  1,    1,    1,      COUNT_GOTO, 0,
   &render::render_goto,  &render::pre_goto,  &render::fin_goto},
};

void render::init_render_mode(){
  for(int i=0;i<RENDER_MODE_NUM;i++)
    mtype[i] = type[i];
}

void render::tl_null_fill(){
  int i;
  for(i=0;i<tl_num_all;i++)
    tl_array[i] = NULL;
}

void render::update(){
  glw->update();
}

render::render(GLWidget *widget,struct render_info *ri,lupe *lp){
  lh = lp;
  if(widget != NULL)
    glw = widget;

  init_render_mode();
  
  center_crd_x = ri->cent_crd_x;
  center_crd_y = ri->cent_crd_y;

  printf("cent_crd_x:%f, y:%f\n",center_crd_x,center_crd_y);
  
  mag_x = 1.0;
  mag_y = 1.0;
  width = WIDTH;
  height = HEIGHT;
  base_mag = ri->base_mag;

  render_mode= RENDER_MODE_NORMAL;
  timer = new class QTimer();
  QObject::connect(timer,SIGNAL(timeout()),glw,SLOT(update()));
  
  camera_pos_step_array = new GLdouble[COUNT_MV];
  camera_pos_step_array_0 = new GLdouble[COUNT_MV];
  camera_pos_step_array_goto = new GLdouble[COUNT_GOTO];
  
  memset(camera_pos_step_array_0,0,sizeof(GLdouble)*COUNT_MV);

  camera_zoom_step_array = new GLdouble[COUNT_ZOOM];
  camera_zoom_step_array_s = new GLdouble[COUNT_SZOOM];

  camera_mvzm_step_array_z = new GLdouble[COUNT_MVZM];
  //  camera_mvzm_step_array_z_sum = new GLdouble[COUNT_MVZM];
  camera_mvzm_step_array_z_sum = new GLdouble*[lh->ocx_num()];
  for(int i=0;i<lh->ocx_num();i++)
    camera_mvzm_step_array_z_sum[i] = new GLdouble[COUNT_MVZM];

  camera_mvzm_step_array_m = new GLdouble[COUNT_MVZM];

  //mv_camra_drag_unit_x = 2.0/(GLdouble)VIEWPORT_WIDTH;
  //mv_camra_drag_unit_y = 2.0/(GLdouble)VIEWPORT_HEIGHT;
  
  tl_num_all = 1+4*(1+TL_NUM)*TL_NUM-1+1;
  tl_array = (tile **)malloc(sizeof(tile *) * tl_num_all);
    
  if(tl_array != NULL)
    tl_null_fill();
  
  create_step_array_mv();
  create_step_array_mv_goto();

  create_step_array_mvzm();
  //  create_step_array_zoom();
  create_step_array_zoom_s();
  
  camera_pos_x = 0;
  camera_pos_y = 0;
  set_render_tile_index();
#if 0
  //  glGenTextures(1,&texture);
  glGenFramebuffers(1,&fbo);
  glGenRenderbuffers(1,&colorBuffer);

  // bind the FBO
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  // color render buffer
  glBindRenderbuffer(GL_RENDERBUFFER,colorBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER,GL_RGB,width,height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,colorBuffer);
#endif

}

render::render(GLWidget *widget,lupe *lp){
  lh = lp;
  if(widget != NULL)
    glw = widget;

  init_render_mode();

  center_crd_x = 0;
  center_crd_y = 0;

  mag_x = 1.0;
  mag_y = 1.0;
  width = WIDTH;
  height = HEIGHT;
  base_mag = 1.0;

  render_mode= RENDER_MODE_NORMAL;
  timer = new class QTimer();
  QObject::connect(timer,SIGNAL(timeout()),glw,SLOT(update()));
  
  camera_pos_step_array = new GLdouble[COUNT_MV];
  camera_pos_step_array_0 = new GLdouble[COUNT_MV];
  camera_pos_step_array_goto = new GLdouble[COUNT_GOTO];
    
  memset(camera_pos_step_array_0,0,sizeof(GLdouble)*COUNT_MV);

  camera_zoom_step_array = new GLdouble[COUNT_ZOOM];
  camera_zoom_step_array_s = new GLdouble[COUNT_SZOOM];

  camera_mvzm_step_array_z = new GLdouble[COUNT_MVZM];
  //  camera_mvzm_step_array_z_sum = new GLdouble[COUNT_MVZM];
  camera_mvzm_step_array_z_sum = new GLdouble*[lh->ocx_num()];
  for(int i=0;i<lh->ocx_num();i++)
    camera_mvzm_step_array_z_sum[i] = new GLdouble[COUNT_MVZM];
  
  camera_mvzm_step_array_m = new GLdouble[COUNT_MVZM];
  
  //mv_camra_drag_unit_x = 2.0/(GLdouble)VIEWPORT_WIDTH;
  //mv_camra_drag_unit_y = 2.0/(GLdouble)VIEWPORT_HEIGHT;
  
  tl_num_all = 1+4*(1+TL_NUM)*TL_NUM-1+1;
  tl_array = (tile **)malloc(sizeof(tile *) * tl_num_all);
  
  if(tl_array != NULL)
    tl_null_fill();
  
  create_step_array_mv();
  create_step_array_mv_goto();

  create_step_array_mvzm();
  //create_step_array_zoom();
  create_step_array_zoom_s();

  camera_pos_x = 0.0;
  camera_pos_y = 0.0;
  set_render_tile_index();
}


void render::render_init_glew(){
  glGenFramebuffers(1,&fbo);
  glGenRenderbuffers(1,&colorBuffer);

  // bind the FBO
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  // color render buffer
  glBindRenderbuffer(GL_RENDERBUFFER,colorBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER,GL_RGB,width,height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,colorBuffer);

  glw->initializeGL_post();
  //  glGenTextures(1,&texture);
}


render::~render(){
  delete timer;
  delete[] camera_pos_step_array;
  delete[] camera_pos_step_array_0;
  delete[] camera_pos_step_array_goto;
  delete[] camera_zoom_step_array;
  delete[] camera_zoom_step_array_s;

  delete[] camera_mvzm_step_array_z;
  for(int i=0;i<lh->ocx_num();i++)
    delete[] camera_mvzm_step_array_z_sum[i];
  delete[] camera_mvzm_step_array_z_sum;
  delete[] camera_mvzm_step_array_m;


  for(int i=0;i<tl_num_all;i++)
    if(tl_array[i] != NULL)
      delete tl_array[i];
  free(tl_array);
  
  printf("tl_array deleted\n");
  dbgf(DBG_CL(RED,WHITE),"render Deleted\n");

}

void render::initial_wndset(){
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
  wnd->set_mag(base_mag * mag_x);
}

void render::create_step_array_zoom_s(){
  double x;
  unsigned long long max = (unsigned long long)COUNT_SZOOM;
  double a,b,c;
  double s;

  s = dgl48(0,(double)max,this,&render::_f_clzoom);
  a =  30.0/(double)(max * max * max * max * max);
  b = -60.0/(double)(max * max * max * max);
  c =  30.0/(double)(max * max * max);

#pragma omp parallel for schedule(static)
  for(int i=0;i<COUNT_SZOOM;i++){
    x = 0.5 + (double)i;
    camera_zoom_step_array_s[i] = (x*x * (a*x*x + b*x + c))/s;
  }
}

void render::create_step_array_zoom(GLdouble diff){
  int i;
  double x;

  unsigned long long max = (unsigned long long)COUNT_ZOOM;
  double a,b,c;
#if 0
  a = 30.0/(double)(max * max * max * max * max);
  b = -60.0/(double)(max * max * max * max);
  c = 30.0/(double)(max * max * max);

  x = 0.5;
  for(i=0;i<COUNT_ZOOM;i++){
    camera_zoom_step_array[i] = diff * (x*x) * (a * x*x + b * x + c);
    x += 1.0;
  }
#else
  a = 60.0/(double)(max * max * max * max * max * max);
  b = -120.0/(double)(max * max * max * max * max);
  c = 60.0/(double)(max * max * max * max);
  
#pragma omp parallel for schedule(static)
  for(i=0;i<COUNT_ZOOM;i++){
    x = 0.5 + (double)i;
    camera_zoom_step_array[i] = diff * x*x*x * (a*x*x + b*x + c);
  }
#endif
}


double render::_f_init_(double x,unsigned long long max){
  (void)x;
  (void)max;
  fprintf(stderr,"_f has not been set.\n");
  return 0;
}

double render::_f_clzoom(double x,unsigned long long max){
  double a = 30.0/(double)(max * max * max * max * max);
  double b = -60.0/(double)(max * max * max * max);
  double c = 30.0/(double)(max * max * max);
  //return /*(x*x * (a*x*x + b*x + c)) */ (dest_mag/((-(dest_mag)/(double)max)*((destmag-1.0)/dest_mag)*x+(double)max/dest_mag) + dest_mag + 1.0);
  return (x*x * (a*x*x + b*x + c));

  //double ct = ((double)lh->get_x()*(double)max)/((double)(lh->get_x()-1));
  // return ((dest_mag/((-(dest_mag-1.0)/(double)max) * x + dest_mag)));
  // return (1.0/(x-ct)) * (x/(x-ct)-1.0) * (x*x) * (a * x*x + b * x + c);
  // return (1.0/(x-ct)) * ((x/(x-ct))-1.0);
}

double render::_f_chzoom(double x,unsigned long long max){
  double ch = (((double)lh->get_x()-1.0)/(double)max) * x + 1.0;
  return (double)lh->get_x()/(((double)(lh->get_x()-1)) * ch * ch);
}

void render::create_step_array_mvzm(){
  double x;
  unsigned long long max = (unsigned long long)COUNT_MVZM;
  double a,b,c;
  double s;

  s = dgl48(0,(double)max,this,&render::_f_clzoom);
  // 2.11451181490247 s = dgl3(0,(double)max,this,&render::_f_chzoom);
  // 4.12903225806411 s = dgl48(0,(double)max,this,&render::_f_chzoom);
  //  s = dgl48(0,(double)max,this,&render::_f_chzoom);
  // 4.08298517385763 s = nc8(0,(double)max,100,this,&render::_f_chzoom);
  //s = 4.110;
  // 3.07525686042171 s = hardy(0,(double)max,100,this,&render::_f_chzoom);

  /* Integral [0:max] of _f_chzoom(x) */
  // s = (double)max/(double)(lh->get_x()-1.0);
  
  // printf("integral [0,max] %15.14f\n",s);

  a =  30.0/(double)(max * max * max * max * max);
  b = -60.0/(double)(max * max * max * max);
  c =  30.0/(double)(max * max * max);
  
  // double dest_mag = (double)lh->get_x();
  // double tmp = 1.0/(double)max;
  
  //#pragma omp parallel for schedule(static) private(i)
  //double ct = ((double)(lh->get_x()*max))/((double)(lh->get_x()-1));



#pragma omp parallel for schedule(static)
  for(int i=0;i<COUNT_MVZM;i++){
    x = 0.5 + (double)i;
    camera_mvzm_step_array_z[i] = (x*x * (a*x*x + b*x + c))/s;
    // alternative zoom, constant zoom slope
    // camera_mvzm_step_array_z[i] = tmp;
  }

  /* 10kb when MAX=256,ocxnum=5 */
  double sum;
  for(int i=0;i<lh->ocx_num();i++){
    sum = 1.0;
    for(int j=0;j<COUNT_MVZM;j++){
      sum += camera_mvzm_step_array_z[j] * (double)(lh->get_xidx(i)-1);
      camera_mvzm_step_array_z_sum[i][j] = sum;
    }
  }

#if 0
  /* (x*x * (a*x*x + b*x + c)) */
  // (dest_mag/((-(dest_mag-1.0)/(double)max)*x+dest_mag+1.0)) * (1.0/s);
  // ((dest_mag/((-(dest_mag-1.0)/(double)max) * x + dest_mag))) * (1.0/s);
  // (1.0/(x-ct)) * (x/(x-ct)-1.0) *(1.0/s);
  // (1.0/(x-ct)) * (x/(x-ct)-1.0) *(1.0/s) * (x*x) * (a*x*x + b*x + c);
  // double ch = (((double)(lh->get_x()-1))/(double)max) * x + 1.0;
  // (double)lh->get_x()/(((double)(lh->get_x()-1)) * ch * ch * s);
  //(1.0/(x-ct)) * (x/(x-ct)-1.0) * (1.0/s) * (x*x) * (a * x*x + b * x + c);
  //    camera_mvzm_step_array_z[i] = (x*x) * (a * x*x + b * x + c);
#endif

#if 0
  sum=0;
  for(i=0;i<COUNT_MVZM;i++){
    sum += camera_mvzm_step_array_z[i];
    printf("%3d: %f\n", i,camera_mvzm_step_array_z[i]);
  }
  printf("sum: %f\n", sum);
#endif  
}

void render::create_step_array_mv(){
  int i;
  double x;

  unsigned long long max = (unsigned long long)COUNT_MV;
  double a,b,c;
  /* Definite integral between x:[0, max] should be 1.0 */
  a = 30.0/(double)(max * max * max * max * max);
  b = -60.0/(double)(max * max * max * max);
  c = 30.0/(double)(max * max * max);
  
  x = 0.5;
  for(i=0;i<COUNT_MV;i++){
    camera_pos_step_array[i] = (x*x) * (a * x*x + b * x + c);
    x += 1.0;
  }

#if 0
  a = 6.0 / (double)(COUNT_MV * COUNT_MV * COUNT_MV);

  x = 0.5;
  for(i=0;i<COUNT_MV;i++){
    camera_pos_step_array[i] =
      a * x * (-x +(double)COUNT_MV);
      
    x += 1.0;
  }
#endif
  
#if 0
  double sum=0;
  for(i=0;i<COUNT_MV;i++){
    sum += camera_pos_step_array[i];
    printf("%3d: %f\n", i,camera_pos_step_array[i]);
  }
  printf("sum: %f\n", sum);
#endif

}

void render::create_step_array_mv_goto(){
  int i;
  double x;

  unsigned long long max = (unsigned long long)COUNT_GOTO;
  double a,b,c;
  /* Definite integral between x:[0, max] should be 1.0 */
  a = 30.0/(double)(max * max * max * max * max);
  b = -60.0/(double)(max * max * max * max);
  c = 30.0/(double)(max * max * max);
  
  x = 0.5;
  for(i=0;i<COUNT_GOTO;i++){
    camera_pos_step_array_goto[i] = (x*x) * (a * x*x + b * x + c);
    x += 1.0;
  }

#if 0
  double sum=0;
  for(i=0;i<COUNT_GOTO;i++){
    sum += camera_pos_step_array_goto[i];
    printf("%15.14f\n", camera_pos_step_array_goto[i]);
  }
  printf("sum: %f\n", sum);
#endif
}

#define IMG_X 256
#define IMG_Y 256
unsigned char image[IMG_Y][IMG_X][3];

void drawImage(void){
  int x,y;

  for(y=0;y<IMG_Y;y++){
    for(x=0;x<IMG_X;x++){
      image[y][x][0] = y % 256;
      image[y][x][1] = x % 256;
      image[y][x][2] = x / 2;
    }
  }
}

void render::change_render_mode(RENDER_MODE mode){
  timer->stop();
  mtype[render_mode].isFirst = 1;
  render_mode = mode;
  glw->update();
}

int render::get_render_mode(){
  return render_mode;
}

int render::get_render_mode_flg(){
  return 1 << render_mode;
}

#define TIMER_INTERVAL 16 /* 15ms - about 60fps */
int render::draw(){
  struct scene *scene;
  scene = &mtype[render_mode];

  if(scene->isFirst){
    if(scene->need_prepare){
      (this->*(scene->prepare))();
    }
    if(scene->isAnimation){
      scene->count = 0;
      timer->start(TIMER_INTERVAL);
    }
    scene->isFirst = 0;
  }

  (this->*(scene->rend))();

  if(lh->isShow())
    lh->draw_lupe();
  if(lh->isScaleShow())
    lh->draw_scale();
  if(lh->isShow_stored_lupe)
    lh->draw_stored_lupe();
      
  if(scene->isAnimation){
    if(++scene->count >= scene->count_max){
      (this->*(scene->fin))();
      scene->isFirst = 1;
      render_mode = RENDER_MODE_NORMAL;
      timer->stop();
      // glw->updateGL();
      // printf("updated\n");
    }
  }

  return 0;
}


int render::calc_tile_index(int x_idx,int y_idx){
  int n;
  int st;
  
  if(x_idx == 0){
    if(y_idx == 0)
      return 0; /* center */
    else
      n = abs(y_idx);
  }

  if(abs(y_idx) <= abs(x_idx))
    n = abs(x_idx);
  else
    n = abs(y_idx);

  st = 4*n*n - 4*n + 1;

  if(y_idx >= 0 && x_idx == n){
    return st + y_idx;
  }
  st += n;
  if(y_idx == n){
    return st + (n - x_idx);
  }
  st += 2*n;
  if(x_idx == -n){
    return st + (n - y_idx);
  }
  st += 2*n;
  if(y_idx == -n){
    return st + (x_idx + n);
  }
  st += 2*n;
  return st + (y_idx + n);
}

int render::create_tile_in_view(){
  int i,j;
  int x_idx,y_idx;
  int array_index;
  struct area_coord crd;

  double area_crd_width = 4.0/base_mag;
  double area_crd_width_hf = area_crd_width/2.0;

  y_idx = render_need_st_y;
  crd.y_s = center_crd_y + (area_crd_width * y_idx) - area_crd_width_hf;
  crd.y_e = crd.y_s + area_crd_width;
  
  for(i=0;i<render_need_y_num;i++){
    x_idx = render_need_st_x;
    crd.x_s = center_crd_x + (area_crd_width * x_idx) - area_crd_width_hf;
    crd.x_e = crd.x_s + area_crd_width;

    for(j=0;j<render_need_x_num;j++){
      array_index = calc_tile_index(x_idx,y_idx);
      printf("\tarray_index = %d\n",array_index);
      if(tl_array[array_index] == NULL){
	tl_array[array_index] = new tile(this,&crd,width,height,x_idx,y_idx);
	if(tl_array[array_index] == NULL)printf("ALERT new failed 8\n");

#if 0
	tl_array[array_index]->calc_me();
	tl_array[array_index]->make_texture_data();
	tl_array[array_index]->make_texture();
#endif
	tl_array[array_index]->prepare_tile_in_new_thread();
      }
      x_idx++;
      crd.x_s = crd.x_e;
      crd.x_e += area_crd_width;
    }
    y_idx++;
    crd.y_s = crd.y_e;
    crd.y_e += area_crd_width;
  }
  
  return 0;
}

int render::pre_normal(){
  printf("prepare normal called\n");

#if 0
  tl = new tile(width,height);
  tl->calc_me();
  tl->make_texture_data();
  tl->make_texture();
#endif
  set_render_tile_index();
  create_tile_in_view();
  
  //  glBindTexture(GL_TEXTURE_2D,texture);
  //  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,tl->get_texture_data());
  //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  //  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  return 0;
}

int render::draw_tile(tile *tl){
  int pos_x,pos_y;
  pos_x = -1 + 2*(tl->get_x_idx()+1 - 1);
  pos_y = -1 + 2*(tl->get_y_idx()+1 - 1);

  glBindTexture(GL_TEXTURE_2D,tl->get_texture());

  glBegin(GL_TRIANGLE_STRIP);
  	glTexCoord2f(0,0);
  	glVertex3i(pos_x,pos_y,0);
	glTexCoord2f(0,1);
	glVertex3i(pos_x,pos_y + 2,0);
	glTexCoord2f(1,0);
	glVertex3i(pos_x + 2,pos_y,0);
	glTexCoord2f(1,1);
	glVertex3i(pos_x + 2,pos_y + 2,0);
  glEnd();

  return 0;
}

int render::render_current_tile_in_view(){
  int i,j;
  int x_idx,y_idx;
  int array_index;

  y_idx = render_need_st_y;
  for(i=0;i<render_need_y_num;i++,y_idx++){
    x_idx = render_need_st_x;
    for(j=0;j<render_need_x_num;j++,x_idx++){
      array_index = calc_tile_index(x_idx,y_idx);
      
      if(tl_array[array_index] == NULL){
	create_tile_in_view();
      }
      
      int tl_sts = tl_array[array_index]->get_status();
      if(tl_sts & TILE_PREPARED){
	dbgf(DBG_CL(NAVY_BLUE,MAGENTA),"pR");
	draw_tile(tl_array[array_index]);
	continue;
      }
      if(tl_sts & TILE_CALCULATED){
	dbgf(DBG_CL(NAVY_BLUE,CYAN),"cA");
	tl_array[array_index]->make_texture();
	draw_tile(tl_array[array_index]);
	continue;
      }
      if(tl_sts & TILE_SUBTILE_UPDATED){
	dbgf(DBG_CL(NAVY_BLUE,RED),"uP");
	tl_array[array_index]->update_subtile();
	draw_tile(tl_array[array_index]);
	continue;
      }
      dbgf(DBG_CL(NAVY_BLUE,GREEN),"oT");
      tl_array[array_index]->make_texture();
      draw_tile(tl_array[array_index]);
    }
  }
  
  return 0;
}

int render::render_normal(){
  // printf("render normal called\n");
  
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  //printf("\t\t render_normal,vp %d x %d\n",VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
  // camera_pos_x,camera_pos_y,0, /* lookat */
  // 0,1,0);  /* vector up */
  glScalef(mag_x,mag_y,1.f);
  
  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  //glScalef(mag_x,mag_y,1.f);

  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  set_render_tile_index();
  render_current_tile_in_view();
#if 0
  glBegin(GL_TRIANGLE_STRIP);
  	glTexCoord2f(0,0);
  	glVertex3i(-1,-1,0);
	glTexCoord2f(0,1);
	glVertex3i(-1,1,0);
	glTexCoord2f(1,0);
	glVertex3i(1,-1,0);
	glTexCoord2f(1,1);
	glVertex3i(1,1,0);
  glEnd();
#endif
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);

  return 0;  
}

int render::pre_pan(){
  camera_zoom_x_to = mag_x * CAMERA_PAN_UNIT;
  camera_zoom_y_to = mag_y * CAMERA_PAN_UNIT;

  camera_zoom_diff_x = camera_zoom_x_to - mag_x;
  camera_zoom_diff_y = camera_zoom_y_to - mag_y;
  create_step_array_zoom(camera_zoom_diff_x);

  camera_zoom_step_x = camera_zoom_step_array;
  camera_zoom_step_y = camera_zoom_step_array;

  set_render_tile_index_all();
    
  return 0;
}

int render::fin_pan(){
  camera_zoom_to(camera_zoom_x_to,camera_zoom_y_to);
  wnd->set_mag(base_mag * mag_x);
  flip_to_new_render();
  glw->update();
  return 0;
}

int render::pre_zoom(){
  // printf("render smooth called\n");
  camera_zoom_x_to = mag_x * CAMERA_ZOOM_UNIT;
  camera_zoom_y_to = mag_y * CAMERA_ZOOM_UNIT;
  
  //  camera_zoom_x_to = mag_x * (float)lh->get_x();
  //  camera_zoom_y_to = mag_y * (float)lh->get_x();
  
  camera_zoom_diff_x = camera_zoom_x_to - mag_x;
  camera_zoom_diff_y = camera_zoom_y_to - mag_y;
  create_step_array_zoom(camera_zoom_diff_x);
  
  camera_zoom_step_x = camera_zoom_step_array;
  camera_zoom_step_y = camera_zoom_step_array;
  return 0;
}

void render::szoom_increment(){
  int start_cnt;
  int now_cnt;
  is_szoom_incremented = true;

  now_cnt = mtype[RENDER_MODE_SZOOM].count;
  if(now_cnt <= COUNT_SZOOM/2){
    start_cnt = now_cnt;
  }else{
    start_cnt = COUNT_SZOOM/2 - (now_cnt - COUNT_SZOOM/2);
  }
  mtype[RENDER_MODE_SZOOM].count = start_cnt;
  camera_zoom_step_x -= now_cnt -start_cnt;
  camera_zoom_step_y -= now_cnt -start_cnt;

  camera_zoom_diff_x *= SZOOM_MAG_INCREMENT;
  camera_zoom_diff_y *= SZOOM_MAG_INCREMENT;

#if 0
  camera_zoom_x_to = (mag_x - stored_mag_x) +
    dgl48((double)start_cnt,(double)COUNT_SZOOM,this,&render::_f_clzoom)*camera_zoom_diff_x;

  camera_zoom_y_to = (mag_y - stored_mag_y) +
    dgl48((double)start_cnt,(double)COUNT_SZOOM,this,&render::_f_clzoom)*camera_zoom_diff_y;
#endif

#if 0
  camera_zoom_x_to *= SZOOM_MAG_INCREMENT;
  camera_zoom_y_to *= SZOOM_MAG_INCREMENT;
  
  double sx = mag_x - stored_mag_x;
  double sy = mag_y - stored_mag_y;

  camera_zoom_diff_x *= SZOOM_MAG_INCREMENT;
  camera_zoom_diff_y *= SZOOM_MAG_INCREMENT;

  printf("sx : %15.14f\n",sx);
  printf("camera_zoom_x_to - sx : %15.14f\n",camera_zoom_x_to - sx);
  printf("count is %d\n",mtype[RENDER_MODE_SZOOM].count);
  printf("integral is %15.14f\n",dgl48((double)mtype[RENDER_MODE_SZOOM].count,(double)COUNT_SZOOM,this,&render::_f_clzoom));

  camera_zoom_diff_x = (camera_zoom_x_to - sx)/dgl48((double)mtype[RENDER_MODE_SZOOM].count-3.0,(double)COUNT_SZOOM,this,&render::_f_clzoom);
  camera_zoom_diff_y = (camera_zoom_y_to - sy)/dgl48((double)mtype[RENDER_MODE_SZOOM].count-3.0,(double)COUNT_SZOOM,this,&render::_f_clzoom);
#endif
}

int render::pre_szoom(){
  camera_zoom_x_to = mag_x * SZOOM_MAG;
  camera_zoom_y_to = mag_y * SZOOM_MAG;

  camera_zoom_diff_x = camera_zoom_x_to - mag_x;
  camera_zoom_diff_y = camera_zoom_y_to - mag_y;

  camera_zoom_step_x = camera_zoom_step_array_s;
  camera_zoom_step_y = camera_zoom_step_array_s;

  stored_mag_x = mag_x;
  stored_mag_y = mag_y;

  is_szoom_incremented = false;
  
  return 0;
}
int render::render_szoom(){
  camera_zoom_smooth_l();
      
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
  //	    camera_pos_x,camera_pos_y,0, /* lookat */
  //	    0,1,0);  /* vector up */
  glScalef(mag_x,mag_y,1.f);
  
  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  //glScalef(mag_x,mag_y,1.f);

  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  render_current_tile_in_view();
#if 0
  glBegin(GL_TRIANGLE_STRIP);
  	glTexCoord2f(0,0);
  	glVertex3i(-1,-1,0);
	glTexCoord2f(0,1);
	glVertex3i(-1,1,0);
	glTexCoord2f(1,0);
	glVertex3i(1,-1,0);
	glTexCoord2f(1,1);
	glVertex3i(1,1,0);
  glEnd();
#endif
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
    
  return 0;
}

int render::fin_szoom(){
  if(is_szoom_incremented)
    return 0;

  camera_zoom_to(camera_zoom_x_to,camera_zoom_y_to);
  wnd->set_mag(base_mag * mag_x);
  glw->update();

  return 0;
}

void render::span_increment(){
  int start_cnt;
  int now_cnt;
  is_span_incremented = true;

  now_cnt = mtype[RENDER_MODE_SPAN].count;
  if(now_cnt <= COUNT_SPAN/2){
    start_cnt = now_cnt;
  }else{
    start_cnt = COUNT_SPAN/2 - (now_cnt - COUNT_SPAN/2);
  }
  mtype[RENDER_MODE_SPAN].count = start_cnt;
  camera_zoom_step_x -= now_cnt -start_cnt;
  camera_zoom_step_y -= now_cnt -start_cnt;

  stored_mag_x *= SPAN_MAG_INCREMENT;
  stored_mag_y *= SPAN_MAG_INCREMENT;
  
  camera_zoom_diff_x = stored_mag_x - mag_x;
  camera_zoom_diff_y = stored_mag_y - mag_x;
}

int render::pre_span(){
  camera_zoom_x_to = mag_x * SPAN_MAG;
  camera_zoom_y_to = mag_y * SPAN_MAG;

  camera_zoom_diff_x = camera_zoom_x_to - mag_x;
  camera_zoom_diff_y = camera_zoom_y_to - mag_y;

  camera_zoom_step_x = camera_zoom_step_array_s;
  camera_zoom_step_y = camera_zoom_step_array_s;

  stored_mag_x = camera_zoom_x_to;
  stored_mag_y = camera_zoom_y_to;

  is_span_incremented = false;
  
  return 0;
}

int render::render_span(){
  camera_zoom_smooth_l();
      
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
  //	    camera_pos_x,camera_pos_y,0, /* lookat */
  //	    0,1,0);  /* vector up */
  glScalef(mag_x,mag_y,1.f);
  
  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  //glScalef(mag_x,mag_y,1.f);

  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  render_current_tile_in_view();
#if 0
  glBegin(GL_TRIANGLE_STRIP);
  	glTexCoord2f(0,0);
  	glVertex3i(-1,-1,0);
	glTexCoord2f(0,1);
	glVertex3i(-1,1,0);
	glTexCoord2f(1,0);
	glVertex3i(1,-1,0);
	glTexCoord2f(1,1);
	glVertex3i(1,1,0);
  glEnd();
#endif
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
    
  return 0;
}
int render::fin_span(){
  if(is_span_incremented)
    return 0;
  
  camera_zoom_to(camera_zoom_x_to,camera_zoom_y_to);
  wnd->set_mag(base_mag * mag_x);
  glw->update();
  
  return 0;
}

int render::pre_goto(){
  QString *s_x,*s_y;
  glw->get_goto_pos(&s_x,&s_y);

  // printf("crd_to_posx(): %15.14f\n",crd_to_posx(s_x->toDouble()));
  // printf("camera_pos: %15.14f\n",camera_pos_x);

  mv_pos_len_x = crd_to_posx(s_x->toDouble()) - camera_pos_x;
  mv_pos_len_y = crd_to_posy(s_y->toDouble()) - camera_pos_y;

  printf("\t\t\tlength is %15.14f\n",mv_pos_len_x);

  camera_pos_x_to = camera_pos_x + mv_pos_len_x;
  camera_pos_y_to = camera_pos_y + mv_pos_len_y;

  camera_pos_step_x = camera_pos_step_array_goto;
  camera_pos_step_y = camera_pos_step_array_goto;

  stored_mag_x = mag_x;
  stored_mag_y = mag_y;

  printf("%15.14f \n",  s_x->toDouble());
  return 0;
}

int render::render_goto(){
  camera_mv_smooth_topos_goto();
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
    
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glScalef(mag_x,mag_y,1.f);

  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  set_render_tile_index();
  render_current_tile_in_view();
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
  return 0;
}

int render::fin_goto(){
  camera_mv_to(camera_pos_x_to,camera_pos_y_to);
  
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
  printf("x:%f, y:%f\n",camera_pos_x,camera_pos_y);
  set_render_tile_index();

  glw->update();
  return 0;
}

int render::fin_zoom(){
  camera_zoom_to(camera_zoom_x_to,camera_zoom_y_to);
  wnd->set_mag(base_mag * mag_x);
  flip_to_new_render();
  glw->update();
  return 0;
}

void render::camera_zoom_to(GLdouble x,GLdouble y){
  mag_x = x;
  mag_y = y;
}

void render::camera_zoom(GLdouble addmag_x,GLdouble addmag_y){
  mag_x += addmag_x;
  mag_y += addmag_y;
}

void render::camera_zoom_smooth_l(){
  camera_zoom(*camera_zoom_step_x * camera_zoom_diff_x,*camera_zoom_step_y * camera_zoom_diff_y);

  wnd->set_mag(base_mag * mag_x);
  
  camera_zoom_step_x++;
  camera_zoom_step_y++;
}

void render::camera_zoom_smooth(){
  camera_zoom(*camera_zoom_step_x,*camera_zoom_step_y);

  wnd->set_mag(base_mag * mag_x);
  
  camera_zoom_step_x++;
  camera_zoom_step_y++;
}

void render::camera_pan_smooth(){
  camera_zoom(*camera_zoom_step_x,*camera_zoom_step_y);
  
  wnd->set_mag(base_mag * mag_x);
  
  camera_zoom_step_x++;
  camera_zoom_step_y++;
}

int render::render_zoom(){
  camera_zoom_smooth();
      
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
  //	    camera_pos_x,camera_pos_y,0, /* lookat */
  //	    0,1,0);  /* vector up */
  glScalef(mag_x,mag_y,1.f);
  
  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  //glScalef(mag_x,mag_y,1.f);

  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  render_current_tile_in_view();
#if 0
  glBegin(GL_TRIANGLE_STRIP);
  	glTexCoord2f(0,0);
  	glVertex3i(-1,-1,0);
	glTexCoord2f(0,1);
	glVertex3i(-1,1,0);
	glTexCoord2f(1,0);
	glVertex3i(1,-1,0);
	glTexCoord2f(1,1);
	glVertex3i(1,1,0);
  glEnd();
#endif
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
    
  return 0;
}


int render::render_pan(){
  camera_pan_smooth();
      
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
  //	    camera_pos_x,camera_pos_y,0, /* lookat */
  //	    0,1,0);  /* vector up */
  glScalef(mag_x,mag_y,1.f);
  
  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  //glScalef(mag_x,mag_y,1.f);

  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  render_current_tile_in_view();
#if 0
  glBegin(GL_TRIANGLE_STRIP);
  	glTexCoord2f(0,0);
  	glVertex3i(-1,-1,0);
	glTexCoord2f(0,1);
	glVertex3i(-1,1,0);
	glTexCoord2f(1,0);
	glVertex3i(1,-1,0);
	glTexCoord2f(1,1);
	glVertex3i(1,1,0);
  glEnd();
#endif
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
  
  return 0;
}

void render::camera_mv(GLdouble x,GLdouble y){
  camera_pos_x += x;
  camera_pos_y += y;
}

void render::camera_mv_to(GLdouble x,GLdouble y){
  camera_pos_x = x;
  camera_pos_y = y;
}

int render::fin_mv(){
  camera_mv_to(camera_pos_x_to,camera_pos_y_to);
  
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
  printf("x:%f, y:%f\n",camera_pos_x,camera_pos_y);
  set_render_tile_index();

  glw->update();

  return 0;
}

#define STEP_UNIT 0.1
void render::move_camera_step(){
  switch(mv_direction){
  case RENDER_DIR_RIGHT:
    camera_mv(STEP_UNIT/mag_x,0);
    wnd->set_center_crd(LINEEDIT_CRD_X,pos_to_crdx(camera_pos_x));
    break;
  case RENDER_DIR_LEFT:
    camera_mv(-STEP_UNIT/mag_x,0);
    wnd->set_center_crd(LINEEDIT_CRD_X,pos_to_crdx(camera_pos_x));
    break;
  case RENDER_DIR_UP:
    camera_mv(0,STEP_UNIT/mag_y);
    wnd->set_center_crd(LINEEDIT_CRD_Y,pos_to_crdy(camera_pos_y));
    break;
  case RENDER_DIR_DOWN:
    camera_mv(0,-STEP_UNIT/mag_y);
    wnd->set_center_crd(LINEEDIT_CRD_Y,pos_to_crdy(camera_pos_y));
    break;
  }
  mtype[RENDER_MODE_NORMAL].isFirst = 1;
  glw->update();
}

int render::pre_mvps(){
  mv_pos_len_x =  (GLfloat)(stored_mvpos_x - VIEWPORT_WIDTH /2)/(GLfloat)(VIEWPORT_WIDTH /2);
  mv_pos_len_y = -(GLfloat)(stored_mvpos_y - VIEWPORT_HEIGHT/2)/(GLfloat)(VIEWPORT_HEIGHT/2);
  
  camera_pos_x_to = camera_pos_x + mv_pos_len_x/mag_x;
  camera_pos_y_to = camera_pos_y + mv_pos_len_y/mag_y;
    
  camera_pos_step_x = camera_pos_step_array;
  camera_pos_step_y = camera_pos_step_array;

  stored_mag_x = mag_x;
  stored_mag_y = mag_y;
  
  return 0;
}

void render::store_mv_pos(int x,int y){
  stored_mvpos_x = x;
  stored_mvpos_y = y;
}

void render::camera_mv_smooth_topos_f_mag(){
  GLdouble a;
  a = ((double)lh->get_x()/(double)(lh->get_x()-1)) *
    (1.0- 1.0/(*camera_pos_step_x));

  //  printf("stored_camera_pos_x: %15.14f\n",stored_camera_pos_x);
  //  printf("stored_camera_pos_y: %15.14f\n",stored_camera_pos_y);
  //  camera_mv(*camera_pos_step_x * mv_pos_len_x/stored_mag_x,*camera_pos_step_y * mv_pos_len_y/stored_mag_y);
  camera_mv_to(stored_camera_pos_x + a * mv_pos_len_x/stored_mag_x,stored_camera_pos_y + a * mv_pos_len_y/stored_mag_y);
  
  camera_pos_step_x++;
  camera_pos_step_y++;
}

void render::camera_mv_smooth_topos(){
  //  printf("%f %f\n", mv_pos_len_x,mv_pos_len_y);
  //  printf("%15.10f %15.10f\n", *camera_pos_step_x,*camera_pos_step_y);
  //  printf("\n");
  
  camera_mv(*camera_pos_step_x * mv_pos_len_x/stored_mag_x,*camera_pos_step_y * mv_pos_len_y/stored_mag_y);
  
  camera_pos_step_x++;
  camera_pos_step_y++;
}

void render::camera_mv_smooth_topos_goto(){
  camera_mv(*camera_pos_step_x * mv_pos_len_x,*camera_pos_step_y * mv_pos_len_y);
  camera_pos_step_x++;
  camera_pos_step_y++;
}

int render::render_mvps(){
  camera_mv_smooth_topos();
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
    
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glScalef(mag_x,mag_y,1.f);

  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  set_render_tile_index();
  render_current_tile_in_view();
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
    
  return 0;
}

int render::fin_mvps(){
  camera_mv_to(camera_pos_x_to,camera_pos_y_to);
  
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
  printf("x:%f, y:%f\n",camera_pos_x,camera_pos_y);
  set_render_tile_index();

  glw->update_pressed_xy();
  store_camera_pos();
  drag_unit_update();

  glw->update();
  
  return 0;
}

int render::pre_mvzm(){
  // move
  mv_pos_len_x =  (GLfloat)(stored_mvpos_x - VIEWPORT_WIDTH/2)/(GLfloat)(VIEWPORT_WIDTH/2);
  mv_pos_len_y = -(GLfloat)(stored_mvpos_y - VIEWPORT_HEIGHT/2)/(GLfloat)(VIEWPORT_HEIGHT/2);
  
  camera_pos_x_to = camera_pos_x + mv_pos_len_x/mag_x;
  camera_pos_y_to = camera_pos_y + mv_pos_len_y/mag_y;

  camera_pos_step_x = camera_mvzm_step_array_z_sum[lh->get_idx()];
  //  camera_pos_step_y = camera_mvzm_step_array_z_sum;

  store_camera_pos();

  stored_mag_x = mag_x;
  stored_mag_y = mag_y;
  
  // zoom
  // camera_zoom_x_to = mag_x * CAMERA_ZOOM_UNIT;
  // camera_zoom_y_to = mag_y * CAMERA_ZOOM_UNIT;
  camera_zoom_x_to = mag_x * (float)lh->get_x();
  camera_zoom_y_to = mag_y * (float)lh->get_x();
  
  camera_zoom_diff_x = camera_zoom_x_to - mag_x;
  camera_zoom_diff_y = camera_zoom_y_to - mag_y;
  
  camera_zoom_step_x = camera_mvzm_step_array_z;
  camera_zoom_step_y = camera_mvzm_step_array_z;

  lh->store_current_lp_pos();
  lh->show_stored_lupe();

  return 0;
}

int render::render_mvzm(){
  //camera_mv_smooth_topos();
  camera_mv_smooth_topos_f_mag();
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
  camera_zoom_smooth_l();
  
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glScalef(mag_x,mag_y,1.f);

  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  set_render_tile_index();
  render_current_tile_in_view();
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
    
  return 0;
}

int render::fin_mvzm(){
  // move
  camera_mv_to(camera_pos_x_to,camera_pos_y_to);
  
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
  printf("x:%f, y:%f\n",camera_pos_x,camera_pos_y);
  // set_render_tile_index();

  // zoom
  camera_zoom_to(camera_zoom_x_to,camera_zoom_y_to);
  wnd->set_mag(base_mag * mag_x);

  // lh->set_visible();
  lh->set_enable();

  lh->hide_stored_lupe();

  flip_to_new_render();
  glw->update();

  return 0;
}

int render::pre_mv(){
  switch(mv_direction){
  case RENDER_DIR_RIGHT:
    camera_pos_x_to = camera_pos_x + CAMERA_MV_UNIT/mag_x;
    camera_pos_y_to = camera_pos_y;
    camera_pos_step_x = camera_pos_step_array;
    camera_pos_step_y = camera_pos_step_array_0;
    break;
  case RENDER_DIR_LEFT:
    camera_pos_x_to = camera_pos_x - CAMERA_MV_UNIT/mag_x;
    camera_pos_y_to = camera_pos_y;
    camera_pos_step_x = camera_pos_step_array;
    camera_pos_step_y = camera_pos_step_array_0;
    break;
  case RENDER_DIR_UP:
    camera_pos_x_to = camera_pos_x;
    camera_pos_y_to = camera_pos_y + CAMERA_MV_UNIT/mag_y;
    camera_pos_step_x = camera_pos_step_array_0;
    camera_pos_step_y = camera_pos_step_array;
    break;
  case RENDER_DIR_DOWN:
    camera_pos_x_to = camera_pos_x;
    camera_pos_y_to = camera_pos_y - CAMERA_MV_UNIT/mag_y;
    camera_pos_step_x = camera_pos_step_array_0;
    camera_pos_step_y = camera_pos_step_array;
    break;
  }

  return 0;
}

void render::store_camera_pos(){
  stored_camera_pos_x = camera_pos_x;
  stored_camera_pos_y = camera_pos_y;
}

void render::drag_unit_update(){
  mv_camera_drag_unit_x = 2.0/((GLdouble)VIEWPORT_WIDTH * mag_x);
  printf("drag_unit:%f, mag_x:%f, base_mag:%f\n",mv_camera_drag_unit_x,mag_x,base_mag);
  mv_camera_drag_unit_y = 2.0/((GLdouble)VIEWPORT_HEIGHT * mag_y);
}

void render::mv_camera_from_drag(int mv_x,int mv_y){
  camera_mv_to(stored_camera_pos_x + (double)-mv_x * mv_camera_drag_unit_x,stored_camera_pos_y + (double)-mv_y * mv_camera_drag_unit_y);

  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
  
  // printf("camera pos x: %f, y: %f\n",camera_pos_x,camera_pos_y);
  glw->update();
}

void render::camera_mv_smooth(){
  if( (mv_direction == RENDER_DIR_RIGHT) ||
      (mv_direction == RENDER_DIR_UP) ){
    camera_mv(*camera_pos_step_x/mag_x,*camera_pos_step_y/mag_y);
  }else{
    camera_mv(-*camera_pos_step_x/mag_x,-*camera_pos_step_y/mag_y);
  }
  camera_pos_step_x++;
  camera_pos_step_y++;
}

int render::render_mv(){
  camera_mv_smooth();
  wnd->set_center_crd(pos_to_crdx(camera_pos_x),pos_to_crdy(camera_pos_y));
  
  glViewport(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */


  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glScalef(mag_x,mag_y,1.f);

  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera
 */
	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */
  
  glEnable(GL_TEXTURE_2D);
  //  glBindTexture(GL_TEXTURE_2D,texture);
  glBindFramebuffer(GL_FRAMEBUFFER,fbo);

  set_render_tile_index();
  render_current_tile_in_view();
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER,fbo);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
  glBlitFramebuffer(0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,0,0,VIEWPORT_WIDTH,VIEWPORT_HEIGHT,GL_COLOR_BUFFER_BIT,GL_NEAREST);
    
  return 0;
}

#if 0
#define MAG_STEP 0.005
void render::magnify(){
  mag_x += MAG_STEP;
  mag_y += MAG_STEP;
  printf("mag is %f now\n",mag_x);
}
#define WIDE_STEP 0.1
void render::wide(){
  mag_x -= WIDE_STEP;
  mag_y -= WIDE_STEP;
  printf("mag is %f now\n",mag_x);
}
#endif

void render::set_direction(int dir){
  mv_direction = dir;
}

void render::set_render_tile_index_all(){
  render_need_st_x = -TL_NUM;
  render_need_x_num = TL_NUM + TL_NUM + 1;
  render_need_st_y = -TL_NUM;
  render_need_y_num = TL_NUM + TL_NUM + 1;
}

void render::set_render_tile_index(){
  int st_index;
  int tl_num;

  if(mag_x < 1.0){
    tl_num = set_render_tile_index_calc_u1(camera_pos_x,&st_index);
    render_need_st_x = st_index;
    render_need_x_num = tl_num;

    tl_num = set_render_tile_index_calc_u1(camera_pos_y,&st_index);
    render_need_st_y = st_index;
    render_need_y_num = tl_num;
  }else{
    tl_num = set_render_tile_index_calc(camera_pos_x,&st_index);
    render_need_st_x = st_index;
    render_need_x_num = tl_num;
    //  printf("!!! render_need_st_x: %d, num:%d\n",render_need_st_x,render_need_x_num);
    tl_num = set_render_tile_index_calc(camera_pos_y,&st_index);
    render_need_st_y = st_index;
    render_need_y_num = tl_num;
  }
  //  printf("!!! render_need_st_y: %d, num:%d\n",render_need_st_y,render_need_y_num);
}

int render::set_render_tile_index_calc(GLdouble camera_pos,int *st){
  const GLdouble eps = 0.000000000000001;
  int truncate;
  int t;
  
  if(camera_pos > 0)
    truncate = (int)(camera_pos + 0.5);
  else
    truncate = (int)(camera_pos - 0.5);

  if(fabs(camera_pos - (double)truncate) < eps){
    if(truncate & 1){
      *st = (truncate+1)/2-1;
      return 2;
    }
    *st = truncate/2;
    return 1;
  }
  
  if(camera_pos < 0)
    t = (int)(camera_pos - 1.0);
  else
    t = (int)camera_pos;
  
  if(t & 1){
    *st = (t+1)/2-1;
    return 2;
  }
  *st = t/2;
  return 2;
  
#if 0
  if(truncate & 1){
    *st = (truncate+1)/2-1;
    return 2;
    //printf("need pic x: %d and %d\n", (truncate+1)/2-1,(truncate+1)/2);
  }else{
    if(fabs(camera_pos - (double)truncate) < eps){
      *st = truncate/2;
      //printf("need pic x: %d\n", truncate/2);
      return 1;
    }else{
      *st = truncate/2-1;
      //printf("need pic x: %d and %d\n", truncate/2-1,truncate/2);
      return 2;
    }
  }
#endif
}

/* todo */
int render::set_render_tile_index_calc_u1(GLdouble camera_pos,int *st){
  const GLdouble eps = 0.000000000000001;
  int truncate;
  int t;
  
  if(camera_pos > 0)
    truncate = (int)(camera_pos + 0.5);
  else
    truncate = (int)(camera_pos - 0.5);

  if(fabs(camera_pos - (double)truncate) < eps){
    if(truncate & 1){
      *st = (truncate+1)/2-1;
      return 2;
    }
    *st = truncate/2;
    return 1;
  }
  
  if(camera_pos < 0)
    t = (int)(camera_pos - 1.0);
  else
    t = (int)camera_pos;
  
  if(t & 1){
    *st = (t+1)/2-1;
    return 2;
  }
  *st = t/2;
  return 2;
}

void render::register_mainwindow(MainWindow *h){
  wnd = h;
}
MainWindow *render::get_mainwindow(){
  return wnd;
}

void render::flip_to_new_render(){
  glw->create_next_layer();
  //  initialize_scene();
  
  glw->flip_next_layer();
}

void render::initialize_scene(){
  for(unsigned int i=0;i<sizeof(mtype)/sizeof(mtype[0]);i++){
    mtype[i].isFirst = 1;
    mtype[i].count = 0;
  }
}

void render::get_render_info(struct render_info *ri){
  ri->cent_crd_x = pos_to_crdx(camera_pos_x);
  ri->cent_crd_y = pos_to_crdy(camera_pos_y);
  printf("get_render_info x:%f, y:%f\n",ri->cent_crd_x,ri->cent_crd_y);
    
  ri->base_mag = base_mag * mag_x;
}

void render::cp_gl_object(render *to){
  to->fbo = fbo;
  to->colorBuffer = colorBuffer;
}

void render::mag_mag(GLdouble mag){
  mag_x *= mag;
  mag_y *= mag;
}

void render::get_mag(GLdouble *_mag_x,GLdouble *_mag_y){
  *_mag_x = mag_x;
  *_mag_y = mag_y;
}

void render::get_camera_pos(GLdouble *x,GLdouble *y){
  *x = camera_pos_x;
  *y = camera_pos_y;
}

void render::d_print_camera_pos(){
  printf("-- camera_pos -----\n");
  printf("x: %15.14f\n",camera_pos_x);
  printf("y: %15.14f\n",camera_pos_y);
}

void render::update_cursor_pos(int cur_x,int cur_y){
  char str[64];
  sprintf(str,"% 15.14f,  % 15.14f",
	  pos_to_crdx(camera_pos_x + (double)cur_x/(mag_x*(double)(VIEWPORT_WIDTH>>1))),
	  pos_to_crdy(camera_pos_y + (double)cur_y/(mag_y*(double)(VIEWPORT_HEIGHT>>1)))
	  );
  wnd->show_status_msg(str);
}

void render::update_mv_length(int mv_x,int mv_y){
  char str[64];
  sprintf(str,"% 15.14f,  % 15.14f (move length)",
	  pos_to_len((double)mv_x/(double)(VIEWPORT_WIDTH>>1)),
	  pos_to_len((double)mv_y/(double)(VIEWPORT_HEIGHT>>1))
	  );
  wnd->show_status_msg(str);
}

void render::redraw(){
  flip_to_new_render();
  printf("flipped \n");
  glw->update();
}

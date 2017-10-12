#include <stdio.h>
#include "tile.h"
#include "dbgout.h"

tile::tile(){
  update_cnt_sum = 0;

  tile_man = new class mandelbro;

  if(tile_man == NULL){
    fprintf(stderr,"new fail\n");
  }
  status = TILE_NONE;
}

#define BUNKATSU_NUM 16

#define LAREA_WIDTH  INITIAL_VPORT_WIDTH
#define LAREA_HEIGHT INITIAL_VPORT_HEIGHT

inline int tile::find_le(int width,int height){
  for(int i=0;i<LAREA_LE;i++){
    if( ((LAREA_WIDTH << i) == width) &&
	((LAREA_HEIGHT << i) == height) )
      return i;
  }
  return -1;
}
 
tile::tile(render *parent,struct area_coord *crd,int w,int h,int x,int y){
  dbgf(DBG_CL(NAVY_BLUE,GREEN),"TILE WAS CREATED\n");
  parent_render = parent;
  tile_man = new class mandelbro(this,crd,w,h,(BUNKATSU_NUM<<8)|BUNKATSU_NUM);
  if(tile_man == NULL){
    fprintf(stderr,"new fail\n");
  }
  width = w;
  height = h;
  x_idx = x;
  if(x_idx > 1000)
    printf("$$$ ALERT\n");

  y_idx = y;
  
  texture_data = new GLubyte[width*height*3];
  int le = find_le(width,height);
  if(le < 0)
    return;
  memcpy(texture_data,tile::larea_texdt[le],(LAREA_WIDTH<<le) * (LAREA_HEIGHT<<le) * 3);
  
  area = *crd;
  glGenTextures(2,texture);

  tex_sel = TEXTURE_SEL_0;
  status = TILE_NONE;

  this->make_texture_loading();

  update_cnt_sum = 0;
  // gettimeofday(&s,NULL);
}

GLubyte* tile::larea_texdt[LAREA_LE];
GLuint   tile::larea_tex[LAREA_LE];

void tile::mk_larea_data(GLubyte *dt,int le){
#define LAREA_COLOUR   0x99
#define LAREA_COLOUR_L 0xAA
  le -= 1;
  int width = LAREA_WIDTH<<le;
    
  for(int i=0;i<LAREA_HEIGHT<<le;i++){
    for(int j=0;j<LAREA_WIDTH<<le;j++){
      dt[3*(width*i+j) + 0] = LAREA_COLOUR;
      dt[3*(width*i+j) + 1] = LAREA_COLOUR;
      dt[3*(width*i+j) + 2] = LAREA_COLOUR;
    }
  }

  int ds = (LAREA_HEIGHT<<le) / BUNKATSU_NUM;
  int k;
  
  for(int i=0;i<LAREA_HEIGHT<<le;i++){
    
    for(k=1;k>(1-(2<<le));k--){
      if((i+k)%ds == 0)
	break;
    }
    
    if(k<=(1-(2<<le)))
      continue;
    
    for(int j=0;j<LAREA_WIDTH<<le;j++){
      dt[3*(width*i+j) + 0] = LAREA_COLOUR_L;
      dt[3*(width*i+j) + 1] = LAREA_COLOUR_L;
      dt[3*(width*i+j) + 2] = LAREA_COLOUR_L;
    }
  }

  for(int i=0;i<LAREA_WIDTH<<le;i++){

    for(k=1;k>(1-(2<<le));k--){
      if((i+k)%ds == 0)
	break;
    }
    
    if(k<=(1-(2<<le)))
      continue;

    for(int j=0;j<LAREA_HEIGHT<<le;j++){
      dt[3*(width*j+i) + 0] = LAREA_COLOUR_L;
      dt[3*(width*j+i) + 1] = LAREA_COLOUR_L;
      dt[3*(width*j+i) + 2] = LAREA_COLOUR_L;
    }
  }
}

int tile::init(){
  dbgf(DBG_CL(BLUE,RED),"INIT");
  for(int i=0;i<LAREA_LE;i++)
    larea_texdt[i] = new GLubyte[(LAREA_WIDTH<<i) * (LAREA_HEIGHT<<i) * 3];
  
  glGenTextures(LAREA_LE,larea_tex);

  for(int i=0;i<LAREA_LE;i++)
    mk_larea_data(larea_texdt[i],i+1);

  for(int i=0;i<LAREA_LE;i++){
    glBindTexture(GL_TEXTURE_2D,tile::larea_tex[i]);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,LAREA_WIDTH<<i,LAREA_HEIGHT<<i,0,GL_RGB,GL_UNSIGNED_BYTE,tile::larea_texdt[i]);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  }
  
  return 0;
}

#if 0
tile::tile(struct area_coord *crd,int w,int h){
  tile_man = new class mandelbro(crd,w,h);
  if(tile_man == NULL){
    fprintf(stderr,"new fail\n");
  }
  width = w;
  height = h;
  texture_data = new GLubyte[width*height*3];
  area = *crd;
  glGenTextures(1,&texture);
}
#endif

tile::~tile(){
  delete tile_man;
  delete[] texture_data;
 
  dbgf(DBG_CL(NAVY_BLUE,GREEN),"TILE WAS DELETED!!!");
  glDeleteTextures(1,&texture[0]);
  
  if(glIsTexture(texture[TEXTURE_SEL_1]))
    glDeleteTextures(1,&texture[TEXTURE_SEL_1]);
}

int tile::prepare_tile_in_new_thread(){
  pthread_t pt;
  
  pthread_create(&pt,NULL,tile::launch_thread,this);
  return 0;
}

int tile::prepare_thread_local(){
  calc_me();
  // make_texture_data();
  // status |= TILE_CALCULATED;

  // note:
  //   texture should be updated in same OpenGL thread
  // make_texture();

  printf("tile prepared\n");
  parent_render->update();
  return 0;
}

int tile::calc_me(){
  tile_man->calc();

  return 0;
}

int tile::make_texture_data(){
  int *res;
  int i,j;
  
  res = tile_man->get_mandel_res();
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      texture_data[3*(width*i+j) + 0] = res[width*i+j]/4;
      texture_data[3*(width*i+j) + 1] = res[width*i+j]/2;
      texture_data[3*(width*i+j) + 2] = res[width*i+j]/4;
    }
  }

  return 0;
}

int tile::make_texture_loading(){
  glBindTexture(GL_TEXTURE_2D,texture[tex_sel]);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,texture_data);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  status |= TILE_INITIALIZED;
  return 0;
}

int tile::get_status(){
  return status;
}

int tile::make_texture(){
  return 0;
  glBindTexture(GL_TEXTURE_2D,texture[tex_sel]);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,texture_data);
  //  glTexSubImage2D(GL_TEXTURE_2D,0,/*xoffset*/0,/*yoffset*/0,/*width*/1024,/*height*/1024,/*format*/GL_RGB,/*type*/GL_UNSIGNED_BYTE,texture_data);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  printf("make texture called \n");
  status |= TILE_PREPARED;
  return 0;
}

int tile::make_texture_subtile_update(int x,int y){
  int dw = width / BUNKATSU_NUM;
  int dh = height / BUNKATSU_NUM;
  GLubyte *rd = tile_man->get_texture_data(x,y);
  
  glBindTexture(GL_TEXTURE_2D,texture[tex_sel]);
  glTexSubImage2D(GL_TEXTURE_2D,0,dw*x/*xoffset*/,dh*y/*yoffset*/,/*width*/dw,/*height*/dh,/*format*/GL_RGB,/*type*/GL_UNSIGNED_BYTE,rd);
  //  glTexSubImage2D(GL_TEXTURE_2D,0,/*xoffset*/0,/*yoffset*/0,/*width*/1024,/*height*/1024,/*format*/GL_RGB,/*type*/GL_UNSIGNED_BYTE,texture_data);
  //glTexSubImage2D(GL_TEXTURE_2D,0,0/*xoffset*/,0/*yoffset*/,/*width*/1024,/*height*/1024,/*format*/GL_RGB,/*type*/GL_UNSIGNED_BYTE,texture_data);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  // printf("update subtile texture\n");
  // status |= TILE_PREPARED;
  return 0;
}

GLubyte *tile::get_texture_data(){
  return texture_data;
}

GLuint tile::get_texture(){
  return texture[tex_sel];

  int i = 0;
  glBindTexture(GL_TEXTURE_2D,tile::larea_tex[i]);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,LAREA_WIDTH<<i,LAREA_HEIGHT<<i,0,GL_RGB,GL_UNSIGNED_BYTE,tile::larea_texdt[i]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  return tile::larea_tex[i];
}

void tile::show(){
  printf("width: %d\n",width);
  printf("height: %d\n",height);
  printf("texture_data_p: %p\n",texture_data);
  printf("tile_man---------------\n");
  tile_man->show();
}

struct area_coord *tile::get_area_crd(){
  return &area;
}

int tile::get_x_idx(){
  //  printf("### x_idx: %d\n",x_idx);
  return x_idx;
}
int tile::get_y_idx(){
  return y_idx;
}

void tile::query_subtile_update(int x,int y){
  struct subtile_pos ps = {x,y};
  
  mutex_uppos_lst.lock();
    q_update_pos << ps;
  mutex_uppos_lst.unlock();
  
  // printf("\t list-size:%d\n",q_update_pos.size());

  
}

void tile::notify_subtile_update(){
  status |= TILE_SUBTILE_UPDATED;
#if 0
    // note:
    //   texture should be updated in same OpenGL thread
    // make_texture_subtile_update(ps.x,ps.y);
#endif
}

void tile::update_subtile(){
  struct subtile_pos ps;
  bool loop;
  int cnt = 0;
  int cnt_max;

  mutex_uppos_lst.lock();
    cnt_max = q_update_pos.size();
  mutex_uppos_lst.unlock();

  if(cnt_max > BUNKATSU_NUM>>1)
    cnt_max = BUNKATSU_NUM>>1;
  
#if 1

  do{
    mutex_uppos_lst.lock();
    if( (loop = ! q_update_pos.isEmpty()) )
      ps = q_update_pos.takeFirst();
    else{
      status &= ~TILE_SUBTILE_UPDATED;
      mutex_uppos_lst.unlock();
      break;
    }
    mutex_uppos_lst.unlock();
    
    make_texture_subtile_update(ps.x,ps.y);
    if(++cnt >= cnt_max)
      break;
  }while(loop);

  if(cnt > 0)
    parent_render->update();

  update_cnt_sum += cnt;

  if(update_cnt_sum >= BUNKATSU_NUM*BUNKATSU_NUM){
    // printf("\t\t $$$ ENDED!\n");
    // gettimeofday(&e,NULL);
    // printf("time = %lf\n", (e.tv_sec - s.tv_sec) + (e.tv_usec - s.tv_usec)*1.0E-6);
  }
#endif
  
#if 0
  while(! q_update_pos.isEmpty()){
     ps = q_update_pos.takeFirst();
     make_texture_subtile_update(ps.x,ps.y);
     parent_render->update();
  }
  status &= ~TILE_SUBTILE_UPDATED;
#endif
#if 0
  if(! q_update_pos.isEmpty()){
    ps = q_update_pos.takeFirst();
    // printf("\t ## ps.x: %d\n",ps.x);
    // printf("\t ## ps.y: %d\n",ps.y);
    make_texture_subtile_update(ps.x,ps.y);
    parent_render->update();
  }
#endif
}


#include "lupe.h"
#include "dbgout.h"

int ocx[] = {2,4,8,16,32,64};

int lupe::ocx_num(){
  return sizeof(ocx)/sizeof(ocx[0]);
}

lupe::lupe(GLWidget *parent){
  printf("lupe is created: %p\n", this);
  enable = true;
  flg_show = true;
  flg_scale_show = true;
  isShow_stored_lupe = false;
  x_idx = 2;
  ratio_x = 1.0f;
  ratio_y = 1.0f;
  glw = parent;
}

lupe::lupe(){
  printf("lupe is created: %p\n", this);
  enable = true;
  flg_show = true;
  flg_scale_show = true;
  isShow_stored_lupe = false;
  x_idx = 2;
  ratio_x = 1.0f;
  ratio_y = 1.0f;
  // printf("\t\t\t\t BBBBB\n");
}

int lupe::get_x(){
  return ocx[x_idx];
}

int lupe::get_xidx(int idx){
  return ocx[idx];
}

int lupe::get_idx(){
  return x_idx;
}

void lupe::cycle_ocx(){
  if(++x_idx >= sizeof(ocx)/sizeof(ocx[0]))
    x_idx = 0;
}

void lupe::up_ocx(){
  if(x_idx+1 < sizeof(ocx)/sizeof(ocx[0]))
    x_idx++;
}

void lupe::down_ocx(){
  if(x_idx > 0)
    x_idx--;
}

void lupe::init(){
  dbgf(DBG_CL(NAVY_BLUE,GREEN),"LUPE WAS INITED!!!");
  glGenTextures(2,texture);
  texture_data[0] = mk_lupe_window_rgba(256,2);

  glBindTexture(GL_TEXTURE_2D,texture[0]);
  
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,texture_data[0]);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  center_x = 0;
  center_y = 0;
  set_hide();
}

lupe::~lupe(){
}

void lupe::set_visible(){
  flg_show = true;
}
void lupe::set_hide(){
  flg_show = false;
}

void lupe::set_enable(){
  enable = true;
}
void lupe::set_disable(){
  enable = false;
}

bool lupe::isShow(){
  return flg_show;
}

void lupe::set_scale_visible(){
  flg_scale_show = true;
}
void lupe::set_scale_hide(){
  flg_scale_show = false;
}
bool lupe::isScaleShow(){
  return flg_scale_show;
}


GLubyte *lupe::mk_lupe_window_rgba(int sides,int waku){
  int i;
  int j;
  GLubyte *p,*mp;
  
  p = new GLubyte[sides * sides * 4];
  if(p == NULL)
    return NULL;

  mp = p;

  for(i=0;i<waku;i++){
    for(j=0;j<sides;j++){
      *mp++ = 255;
      *mp++ = 0;
      *mp++ = 0;
      *mp++ = 255;
    }
  }

  for(i=0;i<(sides-2*waku);i++){
    for(j=0;j<waku;j++){
      *mp++ = 255;
      *mp++ = 0;
      *mp++ = 0;
      *mp++ = 255;
    }
    for(j=0;j<(sides-2*waku);j++){
      *mp++ = 55;
      *mp++ = 255;
      *mp++ = 255;
      *mp++ = 0;
    }
    for(j=0;j<waku;j++){
      *mp++ = 255;
      *mp++ = 0;
      *mp++ = 0;
      *mp++ = 255;
    }
  }

  for(i=0;i<waku;i++){
    for(j=0;j<sides;j++){
      *mp++ = 255;
      *mp++ = 0;
      *mp++ = 0;
      *mp++ = 255;
    }
  }

  return p;
}
#if 0
void lupe::draw_lupe(){
  glPushMatrix();
  
  glScalef(0.125,0.125,0.125);
  
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  glBegin(GL_TRIANGLE_STRIP);
	  glTexCoord2f(0,0);
	  glVertex3f(-1,-1,0.5);
	  
	  glTexCoord2f(0,1);
	  glVertex3f(-1,1,0.5);
	  
	  glTexCoord2f(1,0);
	  glVertex3f(1,-1,0.5);
	  
	  glTexCoord2f(1,1);
	  glVertex3f(1,1,0.5);
  glEnd();
  
  glPopMatrix();

#if 0
  GLint a;
  glGetIntegerv(GL_MATRIX_MODE,&a);
  switch(a){
  case GL_MODELVIEW:
    printf("MODEL\n");
    break;
  case GL_PROJECTION:
    printf("PROJECTION\n");
    break;
  case GL_TEXTURE:
    printf("TEXTURE\n");
    break;
  case GL_COLOR:
    printf("COLOR\n");
    break;
  }
#endif

}
#endif

void lupe::store_current_lp_pos(){
  float _LUPE_HW = 1.0f/(float)get_x();
  GLdouble mag_x,mag_y;
  GLdouble camera_pos_x,camera_pos_y;

  glw->get_rend()->get_mag(&mag_x,&mag_y);
  glw->get_rend()->get_camera_pos(&camera_pos_x,&camera_pos_y);
  
  stored_lppos_x[0] = (center_x-_LUPE_HW)/mag_x*ratio_x + camera_pos_x;
  //  printf("center_x is %15.14f\n",center_x);
  stored_lppos_x[1] = (center_x+_LUPE_HW)/mag_x*ratio_x + camera_pos_x;
  stored_lppos_y[0] = (center_y-_LUPE_HW)/mag_y*ratio_y + camera_pos_y;
  stored_lppos_y[1] = (center_y+_LUPE_HW)/mag_y*ratio_y + camera_pos_y;
  

}

void lupe::draw_stored_lupe(){
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0,1.0, /* left, right */
	  -1.0,1.0, /* bottom, top */
	  1.0,-1.0); /*near, far */
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLdouble mag_x,mag_y;
  glw->get_rend()->get_mag(&mag_x,&mag_y);
  glScalef(mag_x,mag_y,1.f);
  
  GLdouble camera_pos_x,camera_pos_y;
  glw->get_rend()->get_camera_pos(&camera_pos_x,&camera_pos_y);
  //  camera_pos_x = -camera_pos_x;
  //  camera_pos_y = -camera_pos_y;

  //  printf("\t\tpos_x: %15.14f, y:%15.14f\n",camera_pos_x,camera_pos_y);
  gluLookAt(camera_pos_x,camera_pos_y,0.5, /* camera */
  	    camera_pos_x,camera_pos_y,0, /* lookat */
	    0,1,0);  /* vector up */

  //  gluLookAt(0,0,0.5, /* camera */
  //  	    0,0,0, /* lookat */
  //  	    0,1,0);  /* vector up */

  
  float c[16];

  // glPushMatrix();
  
  glLineWidth(1);

  glGetFloatv(GL_CURRENT_COLOR,c);
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if 0
  glBegin(GL_QUADS);
  	glColor4f(0.1f,0.2f,0.4f,0.1f);
	glVertex3f(stored_lppos_x[0],stored_lppos_y[0],0.2);
	glVertex3f(stored_lppos_x[0],stored_lppos_y[1],0.2);
	glVertex3f(stored_lppos_x[1],stored_lppos_y[1],0.2);
	glVertex3f(stored_lppos_x[1],stored_lppos_y[0],0.2);
  glEnd();
#endif

  glLineWidth(2);
  glBegin(GL_LINE_LOOP);
  	glColor4f(0.7f,0.5f,0.3f,0.7f);
	glVertex3f(stored_lppos_x[0],stored_lppos_y[0],0.2);
	glVertex3f(stored_lppos_x[0],stored_lppos_y[1],0.2);
	glVertex3f(stored_lppos_x[1],stored_lppos_y[1],0.2);
	glVertex3f(stored_lppos_x[1],stored_lppos_y[0],0.2);
  glEnd();

  // glPopMatrix();
  glColor3f(c[0],c[1],c[2]);
}

void lupe::draw_lupe(){
  if( ! enable)
    return;
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  float c[16];

  glPushMatrix();
  
  glLineWidth(1);

  glGetFloatv(GL_CURRENT_COLOR,c);
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //#define _LUPE_HW 0.125f

  float _LUPE_HW = 1.0f/(float)get_x();

#define _GL_VERTEX_C glVertex3f((center_x-_LUPE_HW)*ratio_x,(center_y-_LUPE_HW)*ratio_y,0.2); \
  glVertex3f((center_x-_LUPE_HW)*ratio_x,(center_y+_LUPE_HW)*ratio_y,0.2);	\
  glVertex3f((center_x+_LUPE_HW)*ratio_x,(center_y+_LUPE_HW)*ratio_y,0.2);	\
  glVertex3f((center_x+_LUPE_HW)*ratio_x,(center_y-_LUPE_HW)*ratio_y,0.2);
  
  glBegin(GL_QUADS);
  	glColor4f(0.9f,0.2f,0.4f,0.10f);
	_GL_VERTEX_C
  glEnd();

	glLineWidth(2);
  glBegin(GL_LINE_LOOP);
    	glColor4f(0.9f,0.3f,0.3f,0.76f);
	_GL_VERTEX_C
  glEnd();

  glPopMatrix();
  glColor3f(c[0],c[1],c[2]);
}


#define H_SC_L 0.01

static struct{
  float step;
  int repeat;
}tp_cl[] = {
  {0.1f,19},
  {0.2f,9},
  {0.25f,7},
};

static struct{
  float c[3];
}sc_cl[] = {
  {0.0f,0.0f,0.0f},
  {1.0f,1.0f,1.0f},
};

void lupe::draw_scale(){
  float h;
  int id = 2;
  int repeat = tp_cl[id].repeat;
  float step = tp_cl[id].step;

  float c[16];
  int cid = 1;

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glPushMatrix();
  glLineWidth(1.5);
  glBindTexture(GL_TEXTURE_2D, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glGetFloatv(GL_CURRENT_COLOR,c);
  
  glColor4f(sc_cl[cid].c[0],sc_cl[cid].c[1],sc_cl[cid].c[2],0.20f);
  
  glBegin(GL_LINES);
  	h=-1.0f;
	
  	for(int i=0;i<repeat;i++){
	  h += step;
	  glVertex3f(-H_SC_L,h,0.2);
	  glVertex3f( H_SC_L,h,0.2);
	}

	h=-1.0f;
  	for(int i=0;i<repeat;i++){
	  h += step;
	  glVertex3f(h,-H_SC_L,0.2);
	  glVertex3f(h, H_SC_L,0.2);
	}
	glVertex3f(0,-1.0f,0.2);
	glVertex3f(0, 1.0f,0.2);

	glVertex3f(-1.0f,0,0.2);
	glVertex3f( 1.0f,0,0.2);
  glEnd();

  glPopMatrix();
  glColor3f(c[0],c[1],c[2]);
}

void lupe::update_lupe_pos(int x,int y){
  // printf("%d %d@\n",x,y);
  center_x = -1.0f + (2.0f/VIEWPORT_WIDTH) * x;
  center_y = -1.0f + (2.0f/VIEWPORT_HEIGHT) * (VIEWPORT_HEIGHT-y);
  center_x *= 1.0f/ratio_x;
  center_y *= 1.0f/ratio_y;
}

void lupe::set_size(int width,int height){
  if(width > VIEWPORT_WIDTH)
    width = VIEWPORT_WIDTH;
  if(height > VIEWPORT_HEIGHT)
    height = VIEWPORT_HEIGHT;
  
  ratio_x = ((GLfloat)width/(GLfloat)VIEWPORT_WIDTH);
  ratio_y = ((GLfloat)height/(GLfloat)VIEWPORT_HEIGHT);
}

void lupe::show_stored_lupe(){
  isShow_stored_lupe = true;
}

void lupe::hide_stored_lupe(){
  isShow_stored_lupe = false;
}

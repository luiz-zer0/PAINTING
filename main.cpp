#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "freeglut_static.lib")
#include <GL/glut.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>

enum Tool { POINT_TOOL, LINE_TOOL, CIRCLE_TOOL, RECTANGLE_TOOL, TRIANGLE_TOOL, ERASER_TOOL };
struct Color { float r, g, b; };
struct Shape { Tool tool; int x1, y1, x2, y2; Color color; float width; };
const int TOOLBAR_HEIGHT=84, FOOTER_HEIGHT=24, TOOL_X=12, TOOL_Y=12, TOOL_W=96, TOOL_H=27, TOOL_GAP=7;
const int COLOR_Y=48, SWATCH_W=26, SWATCH_H=22, SWATCH_GAP=9;
const Color palette[]={{.12f,.14f,.18f},{.86f,.20f,.20f},{.10f,.55f,.25f},{.12f,.38f,.82f},{.95f,.65f,.08f},{.60f,.22f,.75f}};
const char* toolLabels[]={"1  Point","2  Line","3  Circle","4  Rectangle","5  Triangle","6  Eraser"};
int canvasWidth=900, canvasHeight=650;
Tool currentTool=POINT_TOOL;
int currentColorIndex=0;
float lineWidth=3.0f;
bool dragging=false;
int startX=0,startY=0,mouseX=0,mouseY=0;
std::vector<Shape> shapes;
Color currentColor(){return palette[currentColorIndex];}
void pixel(int x,int y){glVertex2i(x,canvasHeight-1-y);}
void drawRect(int x,int y,int w,int h,Color c){glColor3f(c.r,c.g,c.b);glBegin(GL_QUADS);glVertex2i(x,canvasHeight-1-y);glVertex2i(x+w,canvasHeight-1-y);glVertex2i(x+w,canvasHeight-1-(y+h));glVertex2i(x,canvasHeight-1-(y+h));glEnd();}
void drawOutline(int x,int y,int w,int h,Color c){glColor3f(c.r,c.g,c.b);glBegin(GL_LINE_LOOP);glVertex2i(x,canvasHeight-1-y);glVertex2i(x+w,canvasHeight-1-y);glVertex2i(x+w,canvasHeight-1-(y+h));glVertex2i(x,canvasHeight-1-(y+h));glEnd();}
void drawRasterLine(int x0,int y0,int x1,int y1){int dx=std::abs(x1-x0),sx=x0<x1?1:-1,dy=-std::abs(y1-y0),sy=y0<y1?1:-1,error=dx+dy;glBegin(GL_POINTS);for(;;){pixel(x0,y0);if(x0==x1&&y0==y1)break;int e2=2*error;if(e2>=dy){error+=dy;x0+=sx;}if(e2<=dx){error+=dx;y0+=sy;}}glEnd();}
void drawRasterCircle(int cx,int cy,int r){int x=r,y=0,d=1-r;glBegin(GL_POINTS);while(x>=y){pixel(cx+x,cy+y);pixel(cx+y,cy+x);pixel(cx-y,cy+x);pixel(cx-x,cy+y);pixel(cx-x,cy-y);pixel(cx-y,cy-x);pixel(cx+y,cy-x);pixel(cx+x,cy-y);++y;if(d<=0)d+=2*y+1;else{--x;d+=2*(y-x)+1;}}glEnd();}
void drawShape(const Shape&s){glColor3f(s.color.r,s.color.g,s.color.b);glPointSize(s.width);int l=std::min(s.x1,s.x2),r=std::max(s.x1,s.x2),t=std::min(s.y1,s.y2),b=std::max(s.y1,s.y2);switch(s.tool){case POINT_TOOL:case ERASER_TOOL:glBegin(GL_POINTS);pixel(s.x1,s.y1);glEnd();break;case LINE_TOOL:drawRasterLine(s.x1,s.y1,s.x2,s.y2);break;case CIRCLE_TOOL:{int dx=s.x2-s.x1,dy=s.y2-s.y1;drawRasterCircle(s.x1,s.y1,int(std::sqrt(double(dx*dx+dy*dy))));break;}case RECTANGLE_TOOL:drawRasterLine(l,t,r,t);drawRasterLine(r,t,r,b);drawRasterLine(r,b,l,b);drawRasterLine(l,b,l,t);break;case TRIANGLE_TOOL:drawRasterLine(s.x1,s.y2,s.x2,s.y2);drawRasterLine(s.x2,s.y2,(s.x1+s.x2)/2,s.y1);drawRasterLine((s.x1+s.x2)/2,s.y1,s.x1,s.y2);break;}}
void drawText(int x,int y,const std::string&s){glRasterPos2i(x,canvasHeight-1-y);for(unsigned char c:s)glutBitmapCharacter(GLUT_BITMAP_8_BY_13,c);}
const char* toolName(){switch(currentTool){case POINT_TOOL:return"Point";case LINE_TOOL:return"Line";case CIRCLE_TOOL:return"Circle";case RECTANGLE_TOOL:return"Rectangle";case TRIANGLE_TOOL:return"Triangle";case ERASER_TOOL:return"Eraser";}return"Point";}
void drawToolbar(){drawRect(0,0,canvasWidth,TOOLBAR_HEIGHT,{.94f,.95f,.97f});for(int i=0;i<6;++i){int x=TOOL_X+i*(TOOL_W+TOOL_GAP);bool on=int(currentTool)==i;drawRect(x,TOOL_Y,TOOL_W,TOOL_H,on?Color{.82f,.88f,.98f}:Color{1,1,1});drawOutline(x,TOOL_Y,TOOL_W,TOOL_H,on?Color{.16f,.38f,.72f}:Color{.72f,.75f,.79f});glColor3f(.12f,.14f,.18f);drawText(x+8,TOOL_Y+18,toolLabels[i]);}glColor3f(.22f,.24f,.28f);drawText(14,COLOR_Y+16,"Color");int sx0=62;for(int i=0;i<6;++i){int x=sx0+i*(SWATCH_W+SWATCH_GAP);drawRect(x,COLOR_Y,SWATCH_W,SWATCH_H,palette[i]);drawOutline(x-2,COLOR_Y-2,SWATCH_W+4,SWATCH_H+4,i==currentColorIndex?Color{.10f,.12f,.16f}:Color{.72f,.75f,.79f});}drawText(292,COLOR_Y+16,"Thickness");drawRect(372,COLOR_Y-1,28,25,{1,1,1});drawOutline(372,COLOR_Y-1,28,25,{.65f,.68f,.72f});drawRect(467,COLOR_Y-1,28,25,{1,1,1});drawOutline(467,COLOR_Y-1,28,25,{.65f,.68f,.72f});glColor3f(.12f,.14f,.18f);drawText(381,COLOR_Y+16,"-");drawText(476,COLOR_Y+16,"+");drawText(408,COLOR_Y+16,std::to_string(int(lineWidth))+" px");drawRect(535,COLOR_Y-1,76,25,{1,1,1});drawOutline(535,COLOR_Y-1,76,25,{.65f,.68f,.72f});drawText(547,COLOR_Y+16,"Clear");drawRect(0,TOOLBAR_HEIGHT-1,canvasWidth,1,{.82f,.84f,.87f});}
void display(){glClear(GL_COLOR_BUFFER_BIT);for(const Shape&s:shapes)drawShape(s);if(dragging&&currentTool!=POINT_TOOL&&currentTool!=ERASER_TOOL)drawShape({currentTool,startX,startY,mouseX,mouseY,currentColor(),lineWidth});drawToolbar();drawRect(0,canvasHeight-FOOTER_HEIGHT,canvasWidth,FOOTER_HEIGHT,{.94f,.95f,.97f});glColor3f(.30f,.32f,.36f);drawText(12,canvasHeight-7,"Selected: "+std::string(toolName())+" | Click swatches and +/- above | C clears | Esc exits");glFlush();}
void reshape(int w,int h){canvasWidth=std::max(1,w);canvasHeight=std::max(1,h);glViewport(0,0,canvasWidth,canvasHeight);glMatrixMode(GL_PROJECTION);glLoadIdentity();gluOrtho2D(0,canvasWidth,0,canvasHeight);glMatrixMode(GL_MODELVIEW);glLoadIdentity();glutPostRedisplay();}
bool inside(int x,int y,int l,int t,int w,int h){return x>=l&&x<l+w&&y>=t&&y<t+h;}
int clampY(int y){return std::max(TOOLBAR_HEIGHT+1,std::min(canvasHeight-FOOTER_HEIGHT-1,y));}
void mouse(int button,int state,int x,int y){if(button!=GLUT_LEFT_BUTTON)return;if(state==GLUT_DOWN){if(y<TOOLBAR_HEIGHT){for(int i=0;i<6;++i){int bx=TOOL_X+i*(TOOL_W+TOOL_GAP);if(inside(x,y,bx,TOOL_Y,TOOL_W,TOOL_H))currentTool=Tool(i);}for(int i=0;i<6;++i){int sx=62+i*(SWATCH_W+SWATCH_GAP);if(inside(x,y,sx-3,COLOR_Y-3,SWATCH_W+6,SWATCH_H+6))currentColorIndex=i;}if(inside(x,y,372,COLOR_Y-1,28,25))lineWidth=std::max(1.0f,lineWidth-1);if(inside(x,y,467,COLOR_Y-1,28,25))lineWidth=std::min(30.0f,lineWidth+1);if(inside(x,y,535,COLOR_Y-1,76,25))shapes.clear();glutPostRedisplay();return;}if(y>=canvasHeight-FOOTER_HEIGHT)return;startX=mouseX=x;startY=mouseY=clampY(y);dragging=true;if(currentTool==POINT_TOOL||currentTool==ERASER_TOOL){Color c=currentTool==ERASER_TOOL?Color{1,1,1}:currentColor();shapes.push_back({currentTool,x,startY,x,startY,c,currentTool==ERASER_TOOL?std::max(12.0f,lineWidth*4):lineWidth});}}else if(state==GLUT_UP&&dragging){mouseX=x;mouseY=clampY(y);if(currentTool!=POINT_TOOL&&currentTool!=ERASER_TOOL)shapes.push_back({currentTool,startX,startY,x,mouseY,currentColor(),lineWidth});dragging=false;}glutPostRedisplay();}
void motion(int x,int y){if(!dragging)return;mouseX=x;mouseY=clampY(y);if(currentTool==POINT_TOOL||currentTool==ERASER_TOOL){Color c=currentTool==ERASER_TOOL?Color{1,1,1}:currentColor();shapes.push_back({currentTool,x,mouseY,x,mouseY,c,currentTool==ERASER_TOOL?std::max(12.0f,lineWidth*4):lineWidth});}glutPostRedisplay();}
void chooseColor(int i){if(i>=0&&i<6)currentColorIndex=i;}
void keyboard(unsigned char key,int,int){switch(key){case'1':currentTool=POINT_TOOL;break;case'2':currentTool=LINE_TOOL;break;case'3':currentTool=CIRCLE_TOOL;break;case'4':currentTool=RECTANGLE_TOOL;break;case'5':currentTool=TRIANGLE_TOOL;break;case'6':currentTool=ERASER_TOOL;break;case'q':case'Q':chooseColor(0);break;case'w':case'W':chooseColor(1);break;case'e':case'E':chooseColor(2);break;case'r':case'R':chooseColor(3);break;case't':case'T':chooseColor(4);break;case'y':case'Y':chooseColor(5);break;case'+':case'=':lineWidth=std::min(30.0f,lineWidth+1);break;case'-':case'_':lineWidth=std::max(1.0f,lineWidth-1);break;case'c':case'C':shapes.clear();break;case 27:std::exit(0);}glutPostRedisplay();}
void init(){glClearColor(1,1,1,1);glDisable(GL_DEPTH_TEST);glMatrixMode(GL_PROJECTION);glLoadIdentity();gluOrtho2D(0,canvasWidth,0,canvasHeight);glMatrixMode(GL_MODELVIEW);glLoadIdentity();}
int main(int argc,char**argv){glutInit(&argc,argv);glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);glutInitWindowSize(canvasWidth,canvasHeight);glutInitWindowPosition(100,100);glutCreateWindow("2D Paint Application");init();glutDisplayFunc(display);glutReshapeFunc(reshape);glutMouseFunc(mouse);glutMotionFunc(motion);glutKeyboardFunc(keyboard);glutMainLoop();return 0;}

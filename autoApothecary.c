/*
 *  Shaders - OpenGL2 Style
 *
 *  Demonstrate shaders:
 *    Simple shaders
 *    Vertex lighting
 *    Procedural textures
 *    Toon shader
 *    Pixel lighting
 *    Texture lookup
 *    Pixel lighting with texture
 *
 *  Key bindings:
 *  m/M        Cycle through shaders
 *  x/X        Increase/decrease mandelbrot X-value
 *  y/Y        Increase/decrease mandelbrot Y-value
 *  z/Z        Zoom in/out of mandelbrot
 *  o/O        Cycle through objects
 *  p/P        Toggle between orthogonal & perspective projections
 *  s/S        Start/stop light movement
 *  -/+        Decrease/increase light elevation
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx229.h"
int axes=1;       //  Display axes
int move=1;       //  Move light
int roll=1;       //  Rolling brick texture
int proj=1;       //  Projection type
int obj=1;        //  Object
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int pi=0;         //  Pi texture
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
int zh=90;        //  Light azimuth
int suza=0;       //  Object
float Ylight=2;   //  Light elevation
int shader[]  = {0,0,0,0,0,0,0,0,0,0}; //  Shader programs
const char* text[] = {"Fixed Pipeline","Constant Color","Lighting","Brick","Mandelbrot Set","Mandelbrot Hole","Toon Shader","Pixel Lighting","Textures","Pixel Lighting and Texture"};
float X=0,Y=0,Z=1; //  Mandelbrot X,Y,Z
//  Light colors
const float Emission[]  = {0.0,0.0,0.0,1.0};
const float Ambient[]   = {0.3,0.3,0.3,1.0};
const float Diffuse[]   = {1.0,1.0,1.0,1.0};
const float Specular[]  = {1.0,1.0,1.0,1.0};
const float Shinyness[] = {16};
//  Transformation matrixes
float ProjectionMatrix[16];
float ViewMatrix[16];
//  Set lighting parameters using uniforms
float Position[4];
int light     =   1;  // Lighting
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
float ylight  =   0;  // Elevation of light

/*

  _    _
   ).\  (o)
     \`,/~/\___
     ,/        \,
 .-./ |   ...    \._   /|
| o |_|   |__}    ._III |
 '-'  |         ,/     \|
       `-------'
          ||
          ||
          ||
          ||
         //\\
        //||\\
       // || \\
      //  ||  \\
     //   ||   \\
    //    ||    \\   jgs
 ====    ====   ====

*/
// Camera stuff
int lastMouseX = -1;
int lastMouseY = -1;
float cameraX = 0.0f;
float cameraY = -5.0f;  // Fixed Y position
float cameraZ = 0.0f;
float cameraPitch = 0.0f;  // Pitch (looking up and down)
float cameraYaw = 0.0f;    // Yaw (looking left and right)
float sensitivity = 0.1;
float movementSpeed = 0.1;
int screenWidth = 800;
int screenHeight = 600;
float forwardX = 0;
float forwardZ = -1;

void updateCamera() {
    float pitchRad = cameraPitch * (3.14159 / 180.0f);
    float yawRad = cameraYaw * (3.14159 / 180.0f);

    forwardX = cos(yawRad);
    forwardZ = -sin(yawRad);

    float lookAtX = cameraX + forwardX;
    float lookAtY = cameraY + sin(pitchRad);
    float lookAtZ = cameraZ + forwardZ;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, lookAtX, lookAtY, lookAtZ, 0.0f, 1.0f, 0.0f);
}
void passiveMouseMotion(int x, int y) {
    // Calculate the change in mouse position
    int deltaX = x - screenWidth / 2;
    int deltaY = y - screenHeight / 2;
    // Invert the sign of deltaX to ensure correct camera movement
    deltaX = -deltaX;
   //printf("Mouse Position: (%d, %d)\n", x, y);
    // Adjust the camera yaw and pitch based on mouse movement
    cameraYaw += deltaX * sensitivity;
    cameraPitch -= deltaY * sensitivity;  // Reverse pitch calculation for natural movement

    // Clamp pitch to avoid flipping the camera
    if (cameraPitch > 89.0f) {
        cameraPitch = 89.0f;
    }
    if (cameraPitch < -89.0f) {
        cameraPitch = -89.0f;
    }
    // Set the mouse position back to the center to create a virtual "lock"
    glutWarpPointer(screenWidth / 2, screenHeight / 2);

    updateCamera();
    // Update the view matrix (camera)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glutPostRedisplay();  // Request a redraw
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void moveCamera(char ch) 
{ 
   if(ch == 27){
      exit(0);
   }
   switch (ch) {
        case 'w':
            cameraX += forwardX * movementSpeed;
            cameraZ += forwardZ * movementSpeed;
            break;
        case 's':
            cameraX -= forwardX * movementSpeed;
            cameraZ -= forwardZ * movementSpeed;
            break;
        case 'a':
           cameraX += forwardZ * movementSpeed;
            cameraZ -= forwardX * movementSpeed;
            break;
        case 'd':
            cameraX -= forwardZ * movementSpeed;
            cameraZ += forwardX * movementSpeed;
            break;
         case 27:
            exit(0);
            break;
   }

}
void keyboard(unsigned char key, int x, int y) {
    moveCamera(key);
    updateCamera();
    glutPostRedisplay(); // Request a redraw
}
/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   screenWidth = width;
   screenHeight = height;
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Set projection
   Project(fov,asp,dim);
}

struct point
{
   float x;
   float y;
   float z;
};


struct point* createPolygon(float Cx, float Cy, float Cz, float radius, int sides){
   struct point* points = calloc(sides, sizeof(struct point)); 
    for(int i = 0; i < sides; i++){
         float theta = 2.0f * 3.14159 * i / sides;
         float x = radius * cos(theta);
         float z = radius * sin(theta);
         points[i].x = x + Cx;
         points[i].y = Cy;
         points[i].z = z + Cz; 
     }
    return points;
}
/*
 * Set color
 */
void SetColor(float R,float G,float B)
{
   float color[] = {R,G,B,1.0};
   glColor3f(R,G,B);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
}

/*
   .+------+     +------+     +------+     +------+     +------+.
 .' |    .'|    /|     /|     |      |     |\     |\    |`.    | `.
+---+--+'  |   +-+----+ |     +------+     | +----+-+   |  `+--+---+
|   |  |   |   | |    | |     |      |     | |    | |   |   |  |   |
|  ,+--+---+   | +----+-+     +------+     +-+----+ |   +---+--+   |
|.'    | .'    |/     |/      |      |      \|     \|    `. |   `. |
+------+'      +------+       +------+       +------+      `+------+
*/

static void roundRoof(float Cx, float Cy, float Cz, float radius, int sides,float height){
      struct point* topCircle = createPolygon(Cx,(Cy + (height/2)),Cz,radius,sides);
      struct point* bottomCircle = createPolygon(Cx,(Cy - (height/2)),Cz,radius,sides);
      glBegin(GL_TRIANGLE_FAN);
            glVertex3f(Cx,(Cy + (height/2)),Cz);
            for(int i = 0; i < sides/2; i++){
                glVertex3f(topCircle[i].x,topCircle[i].y,topCircle[i].z);
                glNormal3f( topCircle[i].x,Cy+height,topCircle[i].z);
            }
            glVertex3f(topCircle[0].x,topCircle[0].y,topCircle[0].z);
            glNormal3f( topCircle[0].x,Cy+height,topCircle[0].z);
      glEnd();
      glBegin(GL_TRIANGLE_FAN);
            glVertex3f(Cx,(Cy - (height/2)),Cz);
            for(int i = 0; i < sides/2; i++){
                glVertex3f(bottomCircle[i].x,bottomCircle[i].y,bottomCircle[i].z);
                glNormal3f( bottomCircle[i].x,Cy+height,bottomCircle[i].z);
            }
            glVertex3f(bottomCircle[0].x,bottomCircle[0].y,bottomCircle[0].z);
            glNormal3f( bottomCircle[0].x,Cy+height,bottomCircle[0].z);
      glEnd();
      glBegin(GL_QUAD_STRIP);
            for(int i = 0; i < sides/2; i++){
                topCircle[i];
                bottomCircle[i];
                glVertex3f(topCircle[i].x,topCircle[i].y,topCircle[i].z);
                glVertex3f(bottomCircle[i].x,bottomCircle[i].y,bottomCircle[i].z);
                
                glNormal3f( bottomCircle[i].x,Cy, bottomCircle[i].z);
            }
            glVertex3f(topCircle[0].x,topCircle[0].y,topCircle[0].z);
            glVertex3f(bottomCircle[0].x,bottomCircle[0].y,bottomCircle[0].z);
            glEnd();
      free(topCircle);
      free(bottomCircle);
}
static void Roof(float x, float y, float z,float length, float width, float height){

   //TODO normals?? how??
   //draw roof
            //parameters height,width,length
            //bottom
            glBegin(GL_QUADS);
            glVertex3f(-width/2,-height/2,length/2);
            glNormal3f(-width/2,-height/2,length/2);

            glVertex3f(-width/2,-height/2,-length/2);
            glNormal3f(-width/2,-height/2,-length/2);

            glVertex3f(width/2,-height/2,-length/2);
            glNormal3f(width/2,-height/2,-length/2);

            glVertex3f(width/2,-height/2,length/2);
            glNormal3f(width/2,-height/2,length/2);

            glEnd();
            //east
            glBegin(GL_QUADS);
            glVertex3f(0,height/2,-length/2);
            glNormal3f(0,height/2,-length/2);

            glVertex3f(0,height/2,length/2);
            glNormal3f(0,height/2,length/2);

            glVertex3f(-width/2,-height/2,length/2);
            glNormal3f(-width/2,-height/2,length/2);

            glVertex3f(-width/2,-height/2,-length/2);
            glNormal3f(-width/2,-height/2,-length/2);

            glEnd();
            //west
            glBegin(GL_QUADS);
            glVertex3f(0,height/2,-length/2);
            glVertex3f(0,height/2,length/2);
            glVertex3f(width/2,-height/2,length/2);
            glVertex3f(width/2,-height/2,-length/2);
            glEnd();
            //north
            glBegin(GL_TRIANGLES);
            glVertex3f(-width/2,-height/2,-length/2);
            glVertex3f(width/2,-height/2,-length/2);
            glVertex3f(0,height/2,-length/2);
            glEnd();
            //south
            glBegin(GL_TRIANGLES);
            glVertex3f(-width/2,-height/2,length/2);
            glVertex3f(width/2,-height/2,length/2);
            glVertex3f(0,height/2,length/2);
            glEnd();
}
static void Cone(float Cx, float Cy, float Cz, float radius, int sides,float height){
   struct point* topCircle = createPolygon(Cx,(Cy + (height/2)),Cz,0.01,sides);
      struct point* bottomCircle = createPolygon(Cx,(Cy - (height/2)),Cz,radius,sides);
      glBegin(GL_TRIANGLE_FAN);
            glVertex3f(Cx,(Cy + (height/2)),Cz);
            for(int i = 0; i < sides; i++){
                glVertex3f(topCircle[i].x,topCircle[i].y,topCircle[i].z);
                glNormal3f( topCircle[i].x,Cy+height,topCircle[i].z);
            }
            glVertex3f(topCircle[0].x,topCircle[0].y,topCircle[0].z);
            glNormal3f( topCircle[0].x,Cy+height,topCircle[0].z);
      glEnd();
      glBegin(GL_TRIANGLE_FAN);
            glVertex3f(Cx,(Cy - (height/2)),Cz);
            for(int i = 0; i < sides; i++){
                glVertex3f(bottomCircle[i].x,bottomCircle[i].y,bottomCircle[i].z);
                glNormal3f( bottomCircle[i].x,Cy+height,bottomCircle[i].z);
            }
            glVertex3f(bottomCircle[0].x,bottomCircle[0].y,bottomCircle[0].z);
            glNormal3f( bottomCircle[0].x,Cy+height,bottomCircle[0].z);
      glEnd();
      glBegin(GL_QUAD_STRIP);
            for(int i = 0; i < sides; i++){
                topCircle[i];
                bottomCircle[i];
                glVertex3f(topCircle[i].x,topCircle[i].y,topCircle[i].z);
                glVertex3f(bottomCircle[i].x,bottomCircle[i].y,bottomCircle[i].z);
                
                glNormal3f( bottomCircle[i].x,Cy, bottomCircle[i].z);
            }
            glVertex3f(topCircle[0].x,topCircle[0].y,topCircle[0].z);
            glVertex3f(bottomCircle[0].x,bottomCircle[0].y,bottomCircle[0].z);
            glEnd();
      free(topCircle);
      free(bottomCircle);
}

static void Cylinder(float Cx, float Cy, float Cz, float radius, int sides,float height){
      struct point* topCircle = createPolygon(Cx,(Cy + (height/2)),Cz,radius,sides);
      struct point* bottomCircle = createPolygon(Cx,(Cy - (height/2)),Cz,radius,sides);
      glBegin(GL_TRIANGLE_FAN);
            glVertex3f(Cx,(Cy + (height/2)),Cz);
            for(int i = 0; i < sides; i++){
                glVertex3f(topCircle[i].x,topCircle[i].y,topCircle[i].z);
                glNormal3f( topCircle[i].x,Cy+height,topCircle[i].z);
            }
            glVertex3f(topCircle[0].x,topCircle[0].y,topCircle[0].z);
            glNormal3f( topCircle[0].x,Cy+height,topCircle[0].z);
      glEnd();
      glBegin(GL_TRIANGLE_FAN);
            glVertex3f(Cx,(Cy - (height/2)),Cz);
            for(int i = 0; i < sides; i++){
                glVertex3f(bottomCircle[i].x,bottomCircle[i].y,bottomCircle[i].z);
                glNormal3f( bottomCircle[i].x,Cy+height,bottomCircle[i].z);
            }
            glVertex3f(bottomCircle[0].x,bottomCircle[0].y,bottomCircle[0].z);
            glNormal3f( bottomCircle[0].x,Cy+height,bottomCircle[0].z);
      glEnd();
      glBegin(GL_QUAD_STRIP);
            for(int i = 0; i < sides; i++){
                topCircle[i];
                bottomCircle[i];
                glVertex3f(topCircle[i].x,topCircle[i].y,topCircle[i].z);
                glVertex3f(bottomCircle[i].x,bottomCircle[i].y,bottomCircle[i].z);
                
                glNormal3f( bottomCircle[i].x,Cy, bottomCircle[i].z);
            }
            glVertex3f(topCircle[0].x,topCircle[0].y,topCircle[0].z);
            glVertex3f(bottomCircle[0].x,bottomCircle[0].y,bottomCircle[0].z);
            glEnd();
      free(topCircle);
      free(bottomCircle);
}
/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glVertex3d(x,y,z);
}

static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball with yellow specular
   float yellow[]   = {1.0,1.0,0.0,1.0};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   glColor3f(1,1,1);
   glMaterialf(GL_FRONT,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
   //  Bands of latitude
   for (int ph=-90;ph<90;ph+=inc)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=2*inc)
      {
         Vertex(th,ph);
         Vertex(th,ph+inc);
      }
      glEnd();
   }
   //  Undo transofrmations
   glPopMatrix();
}
/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void Cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   //  Front
   SetColor(0.41,0.27,0.27);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,+1);
   glTexCoord2f(0,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,+1);
   glEnd();
   //  Back
   
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}
static void sideTable(float Cx, float Cy, float Cz, float height, float radius){
   //base
   Cylinder(Cx,Cy - height/2,Cz,radius/2,15,0.2);
   //leg
   Cylinder(Cx,Cy,Cz,0.2,5,height);
   //tabletop
   Cylinder(Cx,Cy + height/2,Cz,radius,20,0.2);
}
static void shelf(float Cx, float Cy, float Cz, float rotationY, float height, float depth, float width, int numShelves){
   float heightPerShelf = height/numShelves;
   float shelfthickness = 0.8;
   float currheight = Cy - height/2;
   for(int i = 0; i < numShelves+1; i++){
      Cube(Cx,currheight,Cz,depth,shelfthickness,width,rotationY);
      currheight += heightPerShelf;
   }
   //back
   Cube(Cx-depth,Cy,Cz,shelfthickness,height/2,width,rotationY);
   //left
   Cube(Cx,Cy,Cz+width,depth,height/2,shelfthickness,rotationY);
   //right
   Cube(Cx,Cy,Cz-width,depth,height/2,shelfthickness,rotationY);

}
/*
//     __       __          .--.
// (  ""--__(  ""-_    ,' .-.\        *
//  "-_ __  ""--__ "-_(  (^_^))      /
//     (  """--___""--__" )-'(      /
//      "-__      ""---/ ,(., )__o-/,  
//          """----___(.'.   /--"--'
//                    ("-_"/(    /
//                     \   \ \
//                      `.  \ |
//                        \  \/
//                        ||  \
//                      ,-'/`. \
//                      ) /   ) \  Ojo '98
//                      |/    `-.\
    
*/  
/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //moveCamera('b');
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();

   updateCamera();
   //  Draw light position as sphere (still no lighting here)
   float Position[]  = {4*Cos(zh),Ylight,4*Sin(zh),1.0};
   glColor3f(1,1,1);
   glPushMatrix();
   glTranslated(Position[0],Position[1],Position[2]);
   glutSolidSphere(0.03,10,10);
   glPopMatrix();

   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  Enable light 0
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
   //  Set materials
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,Shinyness);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,Specular);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Bind Pi texture
   glBindTexture(GL_TEXTURE_2D,pi);

   //  Select shader (0 => no shader)
   //glUseProgram(shader[mode]);

           
   //  Draw the objects
   SetColor(0,1,1);
   //glEnable(GL_TEXTURE_2D);
   for(int i = -10; i < 10; i++){
      for(int j = -10; j < 10; j ++){
         for(int k = -10; k < 10; k++){
            Cube(i,j,k,0.1,0.1,0.1,0);
         }
      }
   }


   Cube(0,-10,0,10,0.2,10,0);

   //ball(0,0,0,1);

   //glDisable(GL_TEXTURE_2D);

   //  Revert to fixed pipeline
   //glUseProgram(0);

   // sideTable(4,-7,2,3,3);
   // glColor3f(1,1,1);
   // shelf(0,-10,0,0,5,2,2,2);
   
   // glBegin(GL_LINE_LOOP);
   // for(int i = 0; i < 8; i++){
   //    glColor3f(1/i,1,1/i);
   //    glVertex3f(Octogon[i].x,Octogon[i].y,Octogon[i].z);
      
      
   // }
   // glEnd();
   // free(Octogon);
   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
  
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   if (move) zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
// void special(int key,int x,int y)
// {
//    //  Right arrow key - increase angle by 5 degrees
//    if (key == GLUT_KEY_RIGHT)
//       th += 5;
//    //  Left arrow key - decrease angle by 5 degrees
//    else if (key == GLUT_KEY_LEFT)
//       th -= 5;
//    //  Up arrow key - increase elevation by 5 degrees
//    else if (key == GLUT_KEY_UP)
//       ph += 5;
//    //  Down arrow key - decrease elevation by 5 degrees
//    else if (key == GLUT_KEY_DOWN)
//       ph -= 5;
//    //  PageUp key - increase dim
//    else if (key == GLUT_KEY_PAGE_DOWN)
//       dim += 0.1;
//    //  PageDown key - decrease dim
//    else if (key == GLUT_KEY_PAGE_UP && dim>1)
//       dim -= 0.1;
//    //  Keep angles to +/-360 degrees
//    th %= 360;
//    ph %= 360;
//    //  Update projection
//    Project(proj?fov:0,asp,dim);
//    //  Tell GLUT it is necessary to redisplay the scene
//    glutPostRedisplay();
// }



/*
 *  Read text file
 */
char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
// void PrintShaderLog(int obj,char* file)
// {
//    int len=0;
//    glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
//    if (len>1)
//    {
//       int n=0;
//       char* buffer = (char *)malloc(len);
//       if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
//       glGetShaderInfoLog(obj,len,&n,buffer);
//       fprintf(stderr,"%s:\n%s\n",file,buffer);
//       free(buffer);
//    }
//    glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
//    if (!len) Fatal("Error compiling %s\n",file);
// }

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

/*
 *  Create Shader
 */
// int CreateShader(GLenum type,char* file)
// {
//    //  Create the shader
//    int shader = glCreateShader(type);
//    //  Load source code from file
//    char* source = ReadText(file);
//    glShaderSource(shader,1,(const char**)&source,NULL);
//    free(source);
//    //  Compile the shader
//    fprintf(stderr,"Compile %s\n",file);
//    glCompileShader(shader);
//    //  Check for errors
//    PrintShaderLog(shader,file);
//    //  Return name
//    return shader;
// }

/*
 *  Create Shader Program
 */
// int CreateShaderProg(char* VertFile,char* FragFile)
// {
//    //  Create program
//    int prog = glCreateProgram();
//    //  Create and compile vertex shader
//    int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
//    //  Create and compile fragment shader
//    int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
//    //  Attach vertex shader
//    glAttachShader(prog,vert);
//    //  Attach fragment shader
//    glAttachShader(prog,frag);
//    //  Link program
//    glLinkProgram(prog);
//    //  Check for errors
//    PrintProgramLog(prog);
//    //  Return name
//    return prog;
// }

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(screenWidth,screenHeight);
   glutCreateWindow("AutoApothecary");

      //glutMotionFunc(mouseMotion);
    // Register the passiveMouseMotion function
    glutPassiveMotionFunc(passiveMouseMotion);
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   // Set cursor to be invisible
   glutSetCursor(GLUT_CURSOR_NONE);
   //glutSpecialFunc(special);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(idle);
   ErrCheck("init");
   glutMainLoop();
   return 0;
}

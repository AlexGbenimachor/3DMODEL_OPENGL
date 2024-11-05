


// STD LIBRARY
#include <math.h>
#include <stdio.h>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <stack>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// OPENGL LIBRARY

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "loadShaderfolder/loadShader.hpp"
//#include "loadShaderfolder/myCompute.hpp"


using namespace std;
using namespace glm;


//################################################ALL YOUR CLASSES GOES HERE######################################################

//build Ray class
#define RAYTRACE_RENDER_WIDTH   1000 // also set window width & height to these values
#define RAYTRACE_RENDER_HEIGHT  1000




//float t = INFINITY;
class Ray{
      public:
            Ray(){}
            Ray(const vec3& origin, const vec3& direction){
                this->On = origin;
                this->dir = direction;
            }
            vec3 origin(){return On;}
            vec3 direction(){return dir;}
            
            vec3 P(const float& t){return On + t * dir;}
            
       private:
            vec3 On;
            vec3 dir;

};

class u_object{
      public:
            u_object(){}
            u_object(vector<vec3> position, const vec3& obSize, vector<vec3> Objcolor, const int& objtype){
                   this->Pos = position;//position
                   this->Sz = obSize;//obj size
                   this->type  = objtype;//type
                   this->obColor = Objcolor;//object color
           
           };
           vector<vec3> position(){return Pos;}
           vec3 obSize(){return Sz;}
           int objtype(){return type;}
           vector<vec3> objColors(){return obColor;}
      private:
             vector<vec3> Pos;
             vec3 Sz;
             int type;
             vector<vec3> obColor;







};

//create sphere class here...
class Sphere{

    private:
     int numVertices;
     int numIndices;
     vector<int> indices;
     vector<vec2> texCoords;
     vector<vec3> vertices;
     vector<vec3> normals;
     //void init(int);
     //float toRadians(float degrees);

    public:
      

      Sphere(){//declare a default value Sphere precision
        init(48);
      }
      Sphere(int prec){
        init(prec);
      };
      
      int getNumVertices();
      int getNumIndices();
      vector<int> getIndices();
      vector<vec3> getVertices();
      vector<vec2> getTexCoords();
      vector<vec3> getNormals();

      float toRadians(int degrees){return (degrees * 2.0f * 3.14159f) / 360.0f;}

      void init(int prec){
           numVertices = (prec+1)*(prec+1);//initialize the number of vertices required to render Sphere
           numIndices  = prec * prec * 6;  //initialization of the number of indices to render the triangle needed to render sphere..
           for(int i=0; i<numVertices; i++){vertices.push_back(vec3());};
           for(int i=0; i<numVertices; i++){texCoords.push_back(vec2());};
           for(int i=0; i<numVertices; i++){normals.push_back(vec3());};
           for(int i=0; i<numIndices; i++){indices.push_back(0);};

           //calculate the number triangles per vertices...
           for(int i=0; i<=prec; i++){
            for(int j=0; j<=prec; j++){
               float y = (float)cos(toRadians(180.0f - i * 180.0f / prec));
               float x =  -(float)cos(toRadians(j*360.0f / prec)) * (float)abs(cos(asin(y)));
               float z = (float)sin(toRadians(j*360.0f / prec)) * (float)abs(cos(asin(y)));
               vertices[i*(prec + 1) + j] = glm::vec3(x, y, z);
			   texCoords[i*(prec + 1) + j] = glm::vec2(((float)j / prec), ((float)i / prec));
			   normals[i*(prec + 1) + j] = glm::vec3(x,y,z);

            }
           }
           //compute the indicies of the each triangle
           for (int i = 0; i<prec; i++) {
               for (int j = 0; j<prec; j++) {
                indices[6 * (i*prec + j) + 0] = i*(prec + 1) + j;
                indices[6 * (i*prec + j) + 1] = i*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 2] = (i + 1)*(prec + 1) + j;
                indices[6 * (i*prec + j) + 3] = i*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 4] = (i + 1)*(prec + 1) + j + 1;
                indices[6 * (i*prec + j) + 5] = (i + 1)*(prec + 1) + j;
             }
            }


           

        }//void()

        


};

//define the accessors of the Sphere...
int Sphere::getNumVertices() { return numVertices; }
int Sphere::getNumIndices() { return numIndices; }
vector<int> Sphere::getIndices() { return indices; }
vector<vec3> Sphere::getVertices() { return vertices; }
vector<vec2> Sphere::getTexCoords() { return texCoords; }
vector<vec3> Sphere::getNormals() { return normals; }


// declare your classes here
class triangleFaces
{

public:
    vector<vec3> triangleVertex; // triangle vertices
    vector<vec3> colorVertex;    // color vertices
    vector<vec2> textCoord;
    vec3 objPos; // triangle position

    triangleFaces(){}; // empty constructor...

    triangleFaces(vector<vec3> triangleVertex, vector<vec3> colorVertex, vec3 objPos, vector<vec2> textCoord);
};

triangleFaces::triangleFaces(vector<vec3> triangleVertexs, vector<vec3> colors, vec3 objpos, vector<vec2> texTCoord)
{
    this->triangleVertex = triangleVertexs;
    this->colorVertex = colors;
    this->objPos = objpos;
    this->textCoord = texTCoord;
}
class rectangle
{

public:
    vector<vec3> vertexPos;
    vector<vec3> colorPos;
    vector<vec2> texturePos;
    rectangle(){}; // empty constructor
    rectangle(vector<vec3> vertexPos, vector<vec3> colorPos, vector<vec2> texturePos);
};

rectangle::rectangle(vector<vec3> vertexpos, vector<vec3> colorpos, vector<vec2> texturePos)
{
    this->vertexPos = vertexpos;
    this->colorPos = colorpos;
    this->texturePos=texturePos;
}



//############################################################ALL WINDOWS SETTING GOES HERE#####################################################
int width = 1920;//800;////1366;//1366;//
int height= 1078;//600;;//768; //768;//

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = width / 2.0f;
float lastY = height / 2.0f;
bool firstMouse = true;

// TIMING

float deltaTime = 0.0f;
float lastFrame = 0.0f;
// lighting properties...
//1. lightPos
glm::vec3 lightDirection(-0.2f, -1.0f, -0.3f);//(1.2f, 1.0f, 2.0f);//(-0.2f, -1.0f, -0.3f);
vec3 lightPos(1.2f, 1.0f, 2.0f);//
vec3 lightPos2(-1.5f, 1.0f, 2.0f);
//2. lightPos
vec3 lightPos3(-8.0, 10.0, 0.0);
vec3 objColor(0.62f, 0.31f, 0.1f);
vec3 objColor2(0.71f, 0.40f, 0.11f);
vec3 lightColor( 1.0f, 1.0f, 1.0f);

vec3 viewPos(-1.5f, 1.5f, 4.0);


//camera properties

vec3 cameraPosition = camera.Position;
vec3 cameraDirection = camera.Front;

//defined vertex array object and buffer object...
int flect_FBO, ReflectTexture, depth_fbo, DepthMap;
unsigned int BaseVAO, BaseVBO, cubeVAO, cubeVBO, PlaneVAO, PlaneVBO, sphereVAO, sphereVBO, depthRenderBuffer,CubeMapShadowMap;
unsigned int QuadVAO, QuadVBO, SkyBoxVAO, SkyBoxVBO, depthMapFBO,  depthMap, reflectMap, reflectVBO,FBOCubeMapShadow, PYRVAO, PYRVBO, ConeVAO, ConeVBO,ConeEBO;//, rbo, framebuffer;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
GLuint tileTexture, cubeTexture, sphereTexture, tennisTexture,  tennisTexture2, textureColorbuffer, specularTexture;
//#################################################CALLBACK METHODS GOES HERE######################################################
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLuint loadcubemapimages(vector<string> faces);
GLuint loadTextures2Object(char const *imPath);

string sphereColorStr;
void ObjInput(int ProgramID, u_object obj);
void renderScene(int ProgramID);
void renderSphere(int ProgramID);
void renderCube(int ProgramID);
void renderQuad(int ProgramID);
void renderQuad2(int ProgramID);
void PlaneObject(int programID);
void renderCone(int ProgramID);
void renderSkyBox();
GLuint  depthFBO();
void renderPathtracerScene(int ProgramID);
vec3 computeNormal(vec3 a, vec3 b, vec3 c);
void renderCone(float ConHeight, int programID);
void renderPyramind(int programID);

GLuint renderRenderCubeFBO();
unsigned int DynamicCubemap();
void renderObject(int programID);
double random_double(double min, double max);
//void ObjectProperty(int ProgramID, u_object Xobject);
//void renderScene(int ProgramID, u_object BxObject, u_object spObject);//render Scene
void renderScene(int ProgramID);//render Scene
//void pointLightMaterial(int ProgramID, vector<vec3> pointLightVec, u_object pointObject);//render point lights
//void MatProperty(int ProgramID,vector<vec3> objectAmb, vector<vec3> objectDiff, vector<vec3> objSpec, vector<float> shininess);
void renderFramebufferTextureID(int ProgramID, int FBO, int TextureID, u_object bxObject, u_object spObject);
float intersectSphere(glm::vec3 position, float radius, Ray r);
float intersectBox(glm::vec3 position, glm::vec3 boxSize, Ray r);
//#####################################################NOTHING GOES HERE##########################################################
vector<vec3> pointlights,  cubePosition,  spherePosition;
u_object SphereObject, BoxObject, lightObject;
GLuint TexCubeMapID, FBOID, RBOID;
const int CUBEMAP_SIZE = 1024;

//define the workgroup size...
int workGroupsX = RAYTRACE_RENDER_WIDTH;
int workGroupsY = RAYTRACE_RENDER_HEIGHT;
int workGroupsZ = 1;

const unsigned int TEXTURE_WIDTH = 1000, TEXTURE_HEIGHT = 1000;

float twirl_amount = 0;

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
float pyrLocX, pyrLocY, pyrLocZ;

vec3 cameraPos = camera.Position;
// allocate variables used in display() function, so that they won’t need to be allocated during rendering
GLuint mvLoc, projLoc, tfLoc, vLoc, tf;
//int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, tMat, rMat;
GLuint PROGRAMID, sunTexture, EarthTexture, MarsTexture, MoonTexture;
stack<glm::mat4> MvStack; //stack
void init(GLFWwindow* window) {

     //sun texture
     string sunfilepath = "2k_sun.jpg";
     sunTexture = loadTextures2Object(sunfilepath.c_str());
     //earth texture
     string Earthfilepath = "ojwD8.jpg";
     EarthTexture = loadTextures2Object(Earthfilepath.c_str());
     //moon texture...
     string Moonfilepath = "lroc_color_poles_1k.jpg";
     MoonTexture = loadTextures2Object(Moonfilepath.c_str());
     
     //Mars textures
     string Marsfilepath = "8k_mars.jpg";
     MarsTexture = loadTextures2Object(Marsfilepath.c_str());
     
     
     PROGRAMID = MyShader("vertexShader.glsl","fragmentShader.glsl");
     cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f; //cameraZ = 420.0f;// cameraZ = 8.0f;
     cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f; // shift down Y to reveal perspective
     //renderCube(PROGRAMID);
}

void display(GLFWwindow* window, double currentTime) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//cyan background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(PROGRAMID);
    // get the uniform variables for the MV and projection matrices
    mvLoc = glGetUniformLocation(PROGRAMID, "mv_matrix");
    projLoc = glGetUniformLocation(PROGRAMID, "proj_matrix");
    // build perspective matrix
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(radians(60.0f), aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees
    // build view matrix, model matrix, and model-view matrix
    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));// CAMERA MATRIX
    MvStack.push(vMat);//ADD CAMERA MATRIX TO THE STACK...
    
    //PYRAMID => SUN...
    
    MvStack.push(MvStack.top());
    MvStack.top()*=translate(mat4(1.0), vec3(0.0, 0.0, 0.0));
    MvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
    MvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    GLuint pyrloc  = glGetUniformLocation(PROGRAMID, "mv_matrix"); 
    GLuint Pyr_projLoc = glGetUniformLocation(PROGRAMID, "proj_matrix");   
    // copy perspective and MV matrices to corresponding uniform variables
    glUniformMatrix4fv(pyrloc , 1, GL_FALSE, glm::value_ptr(MvStack.top()));
    glUniformMatrix4fv(Pyr_projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
    // associate VBO with the corresponding vertex attribute in the vertex shader
   

    // adjust OpenGL settings and draw model
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(PROGRAMID, "samp"), 0);
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    renderSphere(PROGRAMID);
    MvStack.pop();
    
    
    //-----------------------------------planets-----------------------------------------------------
    //cout<<"planets------------------"<<endl;
    //-----------------------------------------------------------------------------------------------
    
    MvStack.push(MvStack.top());//empty top...
    MvStack.top()*= translate(mat4(1.0), vec3(sin((float)glfwGetTime())*-3.0f,0.0f, cos((float)glfwGetTime())*-2.0f));
    MvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
    MvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
    GLuint MVlocC = glGetUniformLocation(PROGRAMID, "mv_matrix");
    GLuint Planeta_projLoc = glGetUniformLocation(PROGRAMID, "proj_matrix"); 
    glUniformMatrix4fv(MVlocC, 1, GL_FALSE, glm::value_ptr(MvStack.top())); 
    glUniformMatrix4fv(Planeta_projLoc, 1, GL_FALSE, glm::value_ptr(pMat));  
    
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(PROGRAMID, "samp"), 0);
    glBindTexture(GL_TEXTURE_2D, EarthTexture);  
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);              // send matrix data to the
    renderSphere(PROGRAMID);
    MvStack.pop();
    
    
    //small moon cube...
    MvStack.push(MvStack.top());//empty top...  //sin((float)glfwGetTime())* //cos((float)glfwGetTime())*
    MvStack.top() *= translate(mat4(1.0), vec3(0.0f, sin((float)glfwGetTime())*-1.5f, cos((float)glfwGetTime())*-1.5f));
    MvStack.top() *= rotate(mat4(1.0), (float)glfwGetTime(),  glm::vec3(0.0, 0.0, 1.0));
    MvStack.top() *= glm::scale(glm::mat4(1.0f), glm::vec3(0.25f, 0.25f, 0.25f));
    GLuint MVlocCm = glGetUniformLocation(PROGRAMID, "mv_matrix");
    GLuint MOON_projLoc = glGetUniformLocation(PROGRAMID, "proj_matrix"); 
    glUniformMatrix4fv(MVlocCm, 1, GL_FALSE, glm::value_ptr(MvStack.top())); 
    glUniformMatrix4fv(MOON_projLoc, 1, GL_FALSE, glm::value_ptr(pMat));   
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(PROGRAMID, "samp"), 0);
    glBindTexture(GL_TEXTURE_2D, MoonTexture);                 // send matrix data to the
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);              // send matrix data to the
    renderSphere(PROGRAMID);
    MvStack.pop();
    
    
    //second planet cube Mars Planet...
    MvStack.push(MvStack.top());//empty top...
    //vec3(sin((float)glfwGetTime())*
    //cos((float)glfwGetTime())*
    MvStack.top()*= translate(mat4(1.0), glm::vec3(-1.0f,-3.0f, -2.0f));
    MvStack.top() *= glm::rotate(glm::mat4(1.0f), 2.0f*(float)glfwGetTime(), glm::vec3(0.0, 1.0, 0.0));
    GLuint MVloc2p = glGetUniformLocation(PROGRAMID, "mv_matrix");
    GLuint pLN2_projLoc = glGetUniformLocation(PROGRAMID, "proj_matrix"); 
    glUniformMatrix4fv(MVloc2p, 1, GL_FALSE, glm::value_ptr(MvStack.top())); 
    glUniformMatrix4fv(pLN2_projLoc, 1, GL_FALSE, glm::value_ptr(pMat)); 
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(PROGRAMID, "samp"), 0);
    glBindTexture(GL_TEXTURE_2D, MarsTexture);     
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);                  // send matrix data to the
    renderSphere(PROGRAMID);
    //MvStack.pop();
    
    
    

    
    
    
}

int main(void){

  cout<<"SOLAR STACK IS WORKING"<<endl;
  if (!glfwInit()) { exit(EXIT_FAILURE); }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  GLFWwindow* window = glfwCreateWindow(width, height, "<<Chapter 4 - solar stacking>>", NULL, NULL);
  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
  glfwSwapInterval(1);
  init(window);
  while (!glfwWindowShouldClose(window)) {
		 display(window, glfwGetTime());
		 glfwSwapBuffers(window);
		 glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
  
  

}



//RANDOM FUNCTION
double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}


double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

//CAMERA MOVEMENT FUNCTION...
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime); // move up
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime); // move DOWN
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}


//load texture 

GLuint loadTextures2Object(char const *imPath){

       GLuint textureID;
       unsigned char* image;
       glGenTextures(1, &textureID);
       int width,height, nrChannels;
       
       image = stbi_load(imPath , &width, &height, &nrChannels, 0);
       
       // Assign texture to ID
       glBindTexture(GL_TEXTURE_2D, textureID);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
       glGenerateMipmap(GL_TEXTURE_2D);	
 
       // Parameters
       
       if(image){
        GLenum imgFormat;
        if(nrChannels == 1)
          imgFormat = GL_RED;
        else if(nrChannels == 3)
          imgFormat = GL_RGB;
        else if(nrChannels == 4)
           imgFormat = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, imgFormat, width, height, 0, imgFormat, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(image);
        stbi_set_flip_vertically_on_load(true);

        }else{
            cout << "Texture failed to load at path: " << imPath << std::endl;
            stbi_image_free(image);
        };
       
       return textureID;




}
//for loading cubemaps

GLuint loadcubemapimages(vector<string> faces){
    /* 
    TextureID-> return textureID
    ImageVars-> include the width, height and image array itself
    
    */
    GLuint TextureID;
    glGenTextures(1, &TextureID);
    glActiveTexture(GL_TEXTURE0);
    
    int width, height, nrChannels;
    unsigned char* image;
    glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
    
    for(int i =0; i<faces.size(); i++){
    
       image = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
       stbi_set_flip_vertically_on_load(true);
      
       
       if (image){
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
          stbi_image_free(image);
         
       }else{
          std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
          stbi_image_free(image);
         
       }
    
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    
    
    return TextureID;

}

/*


PRIMITIVE SHAPES OBJECTS...

*/




//PLANE OBJECTS...

void PlaneObject(int programID){
     vector<vec3> PlaneVertices;
     vector<vec3> PlaneColorVertices;
     vector<vec2> PtextureCoord;
     rectangle planeFaces(vector<vec3>{vec3(25.0f, -0.5f,  25.0f),//plane vertex position...
                                       vec3(-25.0f, -0.5f,  25.0f),
                                       vec3(-25.0f, -0.5f, -25.0f),
                                       vec3( 25.0f, -0.5f,  25.0f),
                                       vec3(-25.0f, -0.5f, -25.0f),
                                       vec3( 25.0f, -0.5f, -25.0f)}, 
                          vector<vec3>{vec3(0.0f, 1.0f, 0.0f),//Normal...
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f),
                                       vec3(0.0f, 1.0f, 0.0f), 
                                       vec3( 0.0f, 1.0f, 0.0f)}, 
                          vector<vec2>{vec2(25.0f,  0.0f),//Texture Coordinate..
                                       vec2(0.0f, 0.0f), 
                                       vec2( 0.0f, 25.0f),
                                       vec2(25.0f,  0.0f),
                                       vec2(0.0f,  25.0f),
                                       vec2(25.0f, 25.0f)}); 
                                       
     //plane properties
    for(int ix = 0; ix<planeFaces.vertexPos.size(); ix++){
        PlaneVertices.push_back(planeFaces.vertexPos[ix]);
        PlaneColorVertices.push_back(planeFaces.colorPos[ix]);
        PtextureCoord.push_back(planeFaces.texturePos[ix]);
    }
    
       //plane buffer VAO, VBO;
        glGenBuffers(1, &PlaneVBO);
        glGenVertexArrays(1, &PlaneVAO);
        //plane vertex
        //plane  vertex position to BUFFER
        glBindVertexArray(PlaneVAO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER, PlaneVertices.size() * sizeof(vec3), & PlaneVertices[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        
        
        //bind plane vertex to Normal

        glBindVertexArray(PlaneVAO);
        glGenBuffers(1, &PlaneVBO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER,  PlaneColorVertices.size()*sizeof(vec3), &PlaneColorVertices[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);

        
        
        
        
        //Plane texCoord
        glBindVertexArray(PlaneVAO);
        glGenBuffers(1, &PlaneVBO);
        glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
        glBufferData(GL_ARRAY_BUFFER,   PtextureCoord.size()*sizeof(vec2), &PtextureCoord[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        
        
        
        
        glBindVertexArray(PlaneVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);





}

//CUBE OBJECT

void renderCube(int programID){

    
            //cube vertex
    vector<vec3> cubeVertices;
    vector<vec3> cubeColorVertices;
    vector<vec2> textureCoord;
    rectangle cubeFaces(vector<vec3>{//TOP SIDE 1
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f, -1.0f), 
                                        vec3(1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f),

                                        //RIGHT SIDE 2
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f), 
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),

                                        //BOTTOM SIDE 3
                                        vec3(-1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f), 
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f),

                                        //LEFT SIDE 4
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f, -1.0f, -1.0f), 
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(1.0f, -1.0f, -1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f),

                                        //FRONT SIDE 5
                                        vec3(-1.0f, -1.0f, -1.0f),
                                        vec3(1.0f, -1.0f, -1.0f), 
                                        vec3(1.0f, -1.0f,  1.0f),
                                        vec3(1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f,  1.0f),
                                        vec3(-1.0f, -1.0f, -1.0f),

                                        //BACK SIDE 6
                                        vec3(-1.0f,  1.0f, -1.0f),
                                        vec3(1.0f,  1.0f , 1.0f), 
                                        vec3(1.0f,  1.0f, -1.0f),
                                        vec3(1.0f,  1.0f,  1.0f),
                                        vec3(-1.0f,  1.0f, -1.0f),
                                        vec3(-1.0f,  1.0f,  1.0f)
    }, vector<vec3>{
            //NORMAL 01 / COLOR 01

            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            vec3(0.0f,  0.0f, -1.0f),
            //NORMAL 02 / COLOR 02
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            vec3(0.0f,  0.0f, 1.0f),
            //NORMAL 03 / COLOR 03
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            vec3(-1.0f,  0.0f,  0.0f),
            //NORMAL 04 / COLOR 04
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            vec3(1.0f,  0.0f,  0.0f),
            //NORMAL 05/COLOR 05

            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            vec3(0.0f, -1.0f,  0.0f),
            //NORMAL 06/COLOR 06
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f),
            vec3(0.0f,  1.0f,  0.0f)
    }, vector<vec2>{
                //texture 01
                vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    //texture 02

                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    //texture 03

                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    //texture 04

                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 1.0f),
                    vec2(0.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    //texture 05

                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f),
                    //texture 06

                    vec2(0.0f, 1.0f),
                    vec2(1.0f, 1.0f),
                    vec2(1.0f, 0.0f),
                    vec2(1.0f, 0.0f),
                    vec2(0.0f, 0.0f),
                    vec2(0.0f, 1.0f)
                        
            });
    //cube properties
    for(int ix = 0; ix<cubeFaces.vertexPos.size(); ix++){
        cubeVertices.push_back(cubeFaces.vertexPos[ix]);
        cubeColorVertices.push_back(cubeFaces.colorPos[ix]);
        textureCoord.push_back(cubeFaces.texturePos[ix]);
    }

    //cube VAO, VBO
//cube buffer VAO, VBO;
glGenBuffers(1, &cubeVBO);
glGenVertexArrays(1, &cubeVAO);
//cube vertex
//bind cube vertex to BUFFER
glBindVertexArray(cubeVAO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER, cubeVertices.size() * sizeof(vec3), & cubeVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);


//bind cube vertex to Normal

glBindVertexArray(cubeVAO);
glGenBuffers(1, &cubeVBO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER,  cubeColorVertices.size()*sizeof(vec3), &cubeColorVertices[0], GL_STATIC_DRAW);   
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(1);

//cube texCoord
glBindVertexArray(cubeVAO);
glGenBuffers(1, &cubeVBO);
glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
glBufferData(GL_ARRAY_BUFFER,   textureCoord.size()*sizeof(vec2), &textureCoord[0], GL_STATIC_DRAW);   
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);




glBindVertexArray(cubeVAO);
glDrawArrays(GL_TRIANGLES, 0, 36);
//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100000);
glBindVertexArray(0);



}
//THE SPHERE OBJECTS...
Sphere mySphere(48);
void renderSphere(int programID){
     
     
    //sphere VAO and VBO 
   //let's create a sphere 
    
        vector<int> indx = mySphere.getIndices();
        vector<vec3> vert = mySphere.getVertices();
        vector<vec2> tex = mySphere.getTexCoords();
        vector<vec3> norm = mySphere.getNormals();

        vector<float> pvalues;// vertex positions		
        vector<float> tvalues;// texture coordinates		
        vector<float> nvalues;// normal vectors		

        int numIndices = mySphere.getNumIndices();
        for (int i = 0; i<numIndices; i++ ){
            //vertex position
            pvalues.push_back((vert[indx[i]]).x);
            pvalues.push_back((vert[indx[i]]).y);
            pvalues.push_back((vert[indx[i]]).z);

            //texture position
            tvalues.push_back((tex[indx[i]]).s);
            tvalues.push_back((tex[indx[i]]).t);

            //normal vectors
            nvalues.push_back((norm[indx[i]]).x);
            nvalues.push_back((norm[indx[i]]).y);
            nvalues.push_back((norm[indx[i]]).z);
        }


    
        glGenVertexArrays(1, &sphereVAO);
        glGenBuffers(1, &sphereVBO);

        //Sphere position...
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0,4, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        
        //Texture vertex position buffer
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER,  tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(1);


        //Normal vertex position buffer... based on the last failed codes, because the VBO(vertex buffer object) wasn't well initialized and binded to the array vertex
        glGenBuffers(1, &sphereVBO);
        glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
        glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);   
        //glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);


      



    glBindVertexArray(sphereVAO);
    glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
    glBindVertexArray(0);

    



}
//QUAD OBJECT...
void renderQuad(int ProgramID){
    vector<vec3> quadVertices;
    vector<vec3> quadNormal;
    vector<vec2> quadTexcoord;
    rectangle Quad(vector<vec3>{
              vec3(-1.0f,  1.0f, 0.0f),
              vec3(-1.0f, -1.0f, 0.0f), 
              vec3(1.0f,  1.0f, 0.0f),
              vec3(1.0f, -1.0f, 0.0f)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    }, vector<vec3>{
             



    }, vector<vec2>{
        vec2(0.0f, 1.0f), 
        vec2(0.0f, 0.0f), 
        vec2(1.0f, 1.0f), 
        vec2(1.0f, 0.0f)
    });



     for(int ix = 0; ix<Quad.vertexPos.size(); ix++){
        quadVertices.push_back(Quad.vertexPos[ix]);
        //quadNormal.push_back(Quad.colorPos[ix]);
        quadTexcoord.push_back(Quad.texturePos[ix]);
    }



    //cube buffer VAO, VBO;
glGenBuffers(1, &QuadVBO);
glGenVertexArrays(1, &QuadVAO);
//cube vertex
//bind cube vertex to BUFFER
glBindVertexArray(QuadVAO);
glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(vec3), &quadVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);




//cube texCoord
glBindVertexArray(QuadVAO);
glGenBuffers(1, &QuadVBO);
glBindBuffer(GL_ARRAY_BUFFER,QuadVBO);
glBufferData(GL_ARRAY_BUFFER,   quadTexcoord.size()*sizeof(vec2), &quadTexcoord[0], GL_STATIC_DRAW);   
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);


glBindVertexArray(QuadVAO);
glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
glBindVertexArray(0);



}

//ADVANCE QUAD2...
void renderQuad2(int ProgramID){
    vector<vec3> quadVertices;
    vector<vec3> quadNormal;
    vector<vec2> quadTexcoord;
    rectangle Quad(vector<vec3>{
        vec3(-1.0f,  1.0f, 0.3f),
        vec3(-1.0f, -1.0f, 0.3f), 
        vec3( 1.0f,  1.0f,  0.3f),
        vec3(1.0f,  -1.0f, 0.3f),
        vec3(1.0f, 1.0f, 0.3f), 
        vec3(-1.0, 1.0f,0.3f)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
            }, vector<vec3>{
             



    }, vector<vec2>{
        vec2(0.0f, 1.0f), 
        vec2(0.0f, 0.0f), 
        vec2(1.0f, 1.0f), 
        vec2(1.0f, 0.0f),
        vec2(1.0f, 0.0f),
        vec2(0.0f, 1.0f)
    });



     for(int ix = 0; ix<Quad.vertexPos.size(); ix++){
        quadVertices.push_back(Quad.vertexPos[ix]);
        //quadNormal.push_back(Quad.colorPos[ix]);
        quadTexcoord.push_back(Quad.texturePos[ix]);
    }



    //cube buffer VAO, VBO;
glGenBuffers(1, &QuadVBO);
glGenVertexArrays(1, &QuadVAO);
//cube vertex
//bind cube vertex to BUFFER
glBindVertexArray(QuadVAO);
glBindBuffer(GL_ARRAY_BUFFER, QuadVBO);
glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(vec3), &quadVertices[0], GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
glEnableVertexAttribArray(0);



//cube texCoord
glBindVertexArray(QuadVAO);
glGenBuffers(1, &QuadVBO);
glBindBuffer(GL_ARRAY_BUFFER,QuadVBO);
glBufferData(GL_ARRAY_BUFFER,   quadTexcoord.size()*sizeof(vec2), &quadTexcoord[0], GL_STATIC_DRAW);   
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
glEnableVertexAttribArray(2);


glBindVertexArray(QuadVAO);
glDrawArrays(GL_TRIANGLES, 0, 6);
glBindVertexArray(0);



}
//SKYBOX...
void renderSkyBox(){

     //SKYBOX
    vector<vec3>  SkyBoxVertices;
   vector<vec3>  SkyBoxColorVertices;
   rectangle SkyBoxFaces(vector<vec3>{//TOP SIDE 1
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f, -1.0f), 
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),

                                    //RIGHT SIDE 2
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),

                                     //BOTTOM SIDE 3
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),


                                     //LEFT SIDE 4
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f, -1.0f,  1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),

                                    //FRONT SIDE 5
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    vec3(1.0f,  1.0f, -1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f,  1.0f),
                                    vec3(-1.0f,  1.0f, -1.0f),
                                    //FRONT SIDE 6
                                    vec3(-1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(1.0f, -1.0f, -1.0f),
                                    vec3(-1.0f, -1.0f,  1.0f),
                                    vec3(1.0f, -1.0f,  1.0f)

   }, vector<vec3>{}, vector<vec2>{});
   //SKYBOX properties
   for(int ix = 0; ix< SkyBoxFaces.vertexPos.size(); ix++){
      SkyBoxVertices.push_back( SkyBoxFaces.vertexPos[ix]);
   }

    //cube VAO, VBO
    //cube buffer VAO, VBO;
    glGenBuffers(1, &SkyBoxVBO);
    glGenVertexArrays(1, &SkyBoxVAO);
    //cube vertex
    //bind cube vertex to BUFFER
    glBindVertexArray(SkyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, SkyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, SkyBoxVertices.size() * sizeof(vec3), &SkyBoxVertices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glEnableVertexAttribArray(0);




}




//Pyramid  vertices..
void renderPyramind(int programID){
     
   // pyramid vertex
    triangleFaces triangleFace1(vector<vec3>{
                                    // FRONT FACE
                                    vec3(-0.5, -0.5, -0.5), // left
                                    vec3(0.5, -0.5, -0.5),  // right
                                    vec3(0.0, 0.5, 0.0),    // top
                                    // RIGHT FACE
                                    vec3(-0.5, -0.5, 0.5), // LEFT
                                    vec3(0.5, -0.5, 0.5),  // RIGHT
                                    vec3(0.0, 0.5, 0.0),   // TOP
                                    // BACK FACE
                                    vec3(-0.5, -0.5, -0.5), // LEFT
                                    vec3(-0.5, -0.5, 0.5),  // RIGHT
                                    vec3(0.0, 0.5, 0.0),    // TOP
                                    // LEFT FACE
                                    vec3(0.5, -0.5, -0.5), // LEFT
                                    vec3(0.5, -0.5, 0.5),  // RIGHT
                                    vec3(0.0, 0.5, 0.0),
                                    // BASE LEFT
                                    vec3(-0.5f, -0.5f, -0.5f),
                                    vec3(0.5f, -0.5f, 0.5f),
                                    vec3(-0.5f, -0.5f, 0.5f),
                                    // BASE RIGHT
                                    vec3(0.5f, -0.5f, 0.5f),
                                    vec3(-0.5f, -0.5f, -0.5f),
                                    vec3(0.5f, -0.5f, -0.5f)

                                }, // top

                                // color
                                vector<vec3>{
                                    // FRONT FACE COLOR//NORMAL
                                    vec3(0.0, 0.0, -1.0), // blue
                                    vec3(0.0, 0.0, -1.0), // green
                                    vec3(0.0, 0.0, -1.0), // red
                                    // RIGHT FACE COLOR
                                    vec3(0.0, 0.0, 1.0), // blue
                                    vec3(0.0, 0.0, 1.0), // green
                                    vec3(0.0, 0.0, 1.0), // red
                                    // BACK FACE COLOR
                                    vec3(-1.0, 0.0, 0.0), // blue
                                    vec3(-1.0, 0.0, 0.0), // green
                                    vec3(-1.0, 0.0, 0.0), // red
                                    // LEFT FACE COLOR
                                    vec3(1.0, 0.0, 0.0), // blue
                                    vec3(1.0, 0.0, 0.0), // green
                                    vec3(1.0, 0.0, 0.0), // red

                                    vec3(0.0, -1.0, 0.0), // blue
                                    vec3(0.0, -1.0, 0.0), // green
                                    vec3(0.0, -1.0, 0.0), // red

                                    vec3(1.0, 0.0, 0.0), // blue
                                    vec3(1.0, 0.0, 0.0), // green
                                    vec3(1.0, 0.0, 0.0), // red

                                },
                                vec3(-0.5, 0.0, 0.0), // triangle default position
                                vector<vec2>{         // the front face texture
                                             vec2(0.0, 0.0), 
                                             vec2(1.0, 0.0f), 
                                             vec2(0.5f, 1.0),
                                             // right face...
                                             vec2(0.0f, 0.0f), 
                                             vec2(1.0f, 0.0f), 
                                             vec2(0.5f, 1.0f),
                                             // Back face texture
                                             vec2(0.0f, 1.0f),
                                             vec2(0.0f, 0.0f), 
                                             vec2(0.5f, 1.0f),

                                             vec2(0.0f, 1.0f), 
                                             vec2(0.0f, 0.0f), 
                                             vec2(0.5f, 1.0f),

                                             vec2(1.0f, 1.0f), 
                                             vec2(1.0f, 0.0f), 
                                             vec2(0.5f, 1.0f),

                                             vec2(0.0f, 0.0f), 
                                             vec2(0.0f, 1.0f), 
                                             vec2(0.5f, 1.0f)

                                });

    vector<vec3> pyravertices;      // vertex
    vector<vec3> pyraColorVertices; // colors
    vector<vec2> pyraTextureCoords; // textures

    // pyramid properties
    for (unsigned int i = 0; i < triangleFace1.triangleVertex.size(); i++)
    {
        pyravertices.push_back(triangleFace1.triangleVertex[i]);
        pyraColorVertices.push_back(triangleFace1.colorVertex[i]);
        pyraTextureCoords.push_back(triangleFace1.textCoord[i]);
    }
    //cout<<"pyramid vertices: "<<pyravertices.size()<<endl;
    //cout<<"pyramid texture coords: "<<pyraTextureCoords.size()<<endl;
    //cout<<"pyramid Normals Color: "<<pyraColorVertices.size()<<endl;
    //PYRVAO, PYRVBO 
    
     //base buffer VAO, VBO;
    glGenBuffers(1, &PYRVBO);
    glGenVertexArrays(1, &PYRVAO);
    //light buffer VAO, VBO
    
   //bind Base vertex to BUFFER
   glBindVertexArray(PYRVAO);
   glGenBuffers(1, &PYRVBO);
   glBindBuffer(GL_ARRAY_BUFFER, PYRVBO );
   glBufferData(GL_ARRAY_BUFFER, pyravertices.size() * sizeof(vec3), &pyravertices[0], GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(0);

   //bind Base vertex to Normal
  
   glBindVertexArray(PYRVAO);
   glGenBuffers(1, &PYRVBO);
   glBindBuffer(GL_ARRAY_BUFFER, PYRVBO);
   glBufferData(GL_ARRAY_BUFFER,   pyraColorVertices.size()*sizeof(vec3), &pyraColorVertices[0], GL_STATIC_DRAW);   
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
   glEnableVertexAttribArray(1);

   //Base TexCoord
   glBindVertexArray(PYRVAO);
   glGenBuffers(1, &PYRVBO);
   glBindBuffer(GL_ARRAY_BUFFER, PYRVBO);
   glBufferData(GL_ARRAY_BUFFER,   pyraTextureCoords.size()*sizeof(vec2), &pyraTextureCoords[0], GL_STATIC_DRAW);   
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
   glEnableVertexAttribArray(2);
  
   glBindVertexArray(PYRVAO);
   glDrawArrays(GL_TRIANGLES,0, 18);
   glEnableVertexAttribArray(0);
   

                 //renderSphere(ProgramID);
                 //renderPyramind(ProgramID);

}

//this compute the normal of the cone
vec3 computeNormal(vec3 a, vec3 b, vec3 c){
      
      vec3 x = vec3(b.x - a.x, b.y - a.y, b.z - a.z);
      vec3 y = vec3(c.x - a.x, c.y - a.x, c.z - a.z);
      vec3 result = vec3(((x.y*y.z)-(y.y*x.z)), ((x.x*y.z)-(y.x*x.z)),((x.x*y.y)-(y.x*x.y)));
      return result;
 };
//renders the cone object
void renderCone(float ConHeight,int ProgramID){
     vector<vec3> pts; //vertices
     vector<vec3> nts;// normals
     vector<vec2> tex;//text coordinates

     vec3 a = vec3(0.0, ConHeight, 0.0);
     vec3 b, c;
     b.y = c.y = 0.0;
     float t,s;
     float i = 0.05;
     int iSectorCount = 0;
     float fReciprocalPrecisition = (float)(10 / 2*360);
     for (double angle = 0; angle<=360; angle+=10){
          c.x = b.x;
          c.z = b.z;
          b.x = cos(radians(angle));
          b.z = sin(radians(angle));
             
             // relative texture coordinates
          float fTextureOffsetS1 = iSectorCount       * fReciprocalPrecisition;
          float fTextureOffsetS2 = (iSectorCount + 1) * fReciprocalPrecisition;
          float sa =  a.x/ (2*360);
          float ta =  a.z/ (2*360);
            
          float sb =  b.x/ (2*360);
          float tb =  b.z/ (2*360);
          //cout<<"fTextureOffsetS1:\t"<<fTextureOffsetS1/2*360<<"fTextureOffsetS2:\t"<<fTextureOffsetS2/2*360<<endl;
          float sc =  c.x/ (2*360);
          float tc =  c.z/ (2*360);

          if(angle!=0){
                //compute the Normal of the cone...
            vec3 normal =  computeNormal(a,  b, c);
            nts.push_back(normal);
                //compute the vertex of the cone...
                
            pts.push_back(a);
            tex.push_back(vec2(0.5*(fTextureOffsetS1/(2*360)), 1.0));
            pts.push_back(b);
            tex.push_back(vec2(0.5*(fTextureOffsetS2/(2*360)), 0.0));
            pts.push_back(c);
            tex.push_back(vec2(0.5*(fTextureOffsetS2/(2*360)), 0.0));
               
                
                //get texture...
                /*
                 tex.push_back(vec2(1-a.x/angle, 1-a.y/angle));
                 tex.push_back(vec2(1-b.x/angle, 1-b.y/angle));
                 
                 
                 tex.push_back(vec2(1-c.x/angle, 1-c.y/angle));
             */
                 iSectorCount++;
             }
             
            
            
           }
           //draw the circle below
           for (double angle = 0; angle<=360; angle+=10){
                a = vec3(0.0);
                tex.push_back(vec2(0.0));
                tex.push_back(vec2((b.x/cos(2*M_PI)), 1-b.z/1));
                //tex.push_back(vec2(1+(b.x/sin(angle)), b.z));
                c.x = b.x;
                c.z = b.z;
                b.x = cos(radians(angle));
                b.z = sin(radians(angle));
                
               double cs = cos(angle);
               double sn = sin(angle);

               float fTextureOffsetS = 0.5F + (float)(0.5F * cs);
               float fTextureOffsetT = 0.5F + (float)(0.5F * sn);
               float sa =  a.x/ (2*360);
               float ta =  a.z/ (2*360);
                
               float  sb =  b.x/ (2*360);
               float  tb =  b.z/ (2*360);
                
               float  sc =  c.x/ (2*360);
               float  tc =  c.z/ (2*360);
                
                vec3 normal =  computeNormal(a,  b, c);
                pts.push_back(a);
                tex.push_back(vec2(fTextureOffsetS/(2*M_PI), fTextureOffsetT/(2*M_PI)));
                pts.push_back(b);
                //tex.push_back(vec2(1*sb, 1.0));
                pts.push_back(c);
                //tex.push_back(vec2(1*sc,  1.0));
                nts.push_back(normal);
             
             }
             
             

        //cout<<"size of points:"<<pts.size()<<endl;

        //plane buffer VAO, VBO;
        glGenBuffers(1, &ConeVBO);
        glGenVertexArrays(1, &ConeVAO);
        //cone vertex
        //cone vertex position to BUFFER
        glBindVertexArray(ConeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, ConeVBO);
        glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(vec3), &pts[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        //bind cone vertex to Normal

        glBindVertexArray(ConeVAO);
        glGenBuffers(1, &ConeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, ConeVBO);
        glBufferData(GL_ARRAY_BUFFER,  nts.size()*sizeof(vec3), &nts[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);


        //cone texCoord
        glBindVertexArray(ConeVAO);
        glGenBuffers(1, &ConeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, ConeVBO);
        glBufferData(GL_ARRAY_BUFFER,   tex.size()*sizeof(vec2), &tex[0], GL_STATIC_DRAW);   
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(2);
        
        glBindVertexArray(ConeVAO);
                //(sizeof(array)/sizeof(array[0])
                //int arrayObj = (sizeof(array)/sizeof(array[0]);
        glDrawArrays(GL_TRIANGLES, 0, 2*360);
           
}



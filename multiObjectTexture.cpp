const char* INSTRUCTIONS = 
"***************\n"
"OOPSIES Game.\n"
"\n"
"Enter - Advance the turn to the next step.\n"
"1, 2, and 3 - Select corresponding piece to move.\n\n"
"On your turn you will press 'Enter' to draw a card,\n"
"The card will have a amount to move or variation on movement,\n"
"You will then select the piece 1-3 you would like to move,\n"
"If it is a special card you may be prompted to input more selections,\n"
"Your piece will move, and you will press 'Enter' to end your turn.\n\n"
"Cards:\n"
"The cards 1, 2, 3, 5, 8, 11, and 12 all just move their specified amounts\n"
"4 moves backwards 4\n"
"7 allows the player to choose any amount 1-7 to move forward\n"
"10 allows the player to either move forward 10 or backwards 1\n"
"OOPSIES moves forward a random amount 1-13\n"
"***************\n"
;

#include "glad/glad.h"  //Include order can matter here
#if defined(__APPLE__) || defined(__linux__)
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_opengl.h>
#else
 #include <SDL.h>
 #include <SDL_opengl.h>
#endif
#include <cstdio>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>  
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "gameLogic.h"

using namespace std;

int screenWidth = 1000; 
int screenHeight = 800;  
float timePast = 0;
float lastUpdated = 0;

float objx=5, objy=5, objz=0;
float colR=1, colG=1, colB=1;
int textest = -1;

bool DEBUG_ON = false;
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName);
bool fullscreen = false;
void Win2PPM(int width, int height);

int testNum = 0;

//srand(time(NULL));
float rand01(){
  return rand()/(float)RAND_MAX;
}

void drawGeometry(int shaderProgram, int model1_start, int model1_numVerts, int model2_start, int model2_numVerts, int model3_start, int model3_numVerts);
void loadLights(int shaderProgram, vector<Light> lights);
void renderQuad(void);

void SetLightUniform(int shaderProgram, size_t lightIndex, Light light) {
  std::ostringstream ss;
  std::string uniformName;
  GLint uniLight;

  // position
  ss << "inLights[" << lightIndex << "]." << "position";
    uniformName = ss.str();
  uniLight = glGetUniformLocation(shaderProgram, uniformName.c_str());
  glUniform4fv(uniLight, 1, glm::value_ptr(light.position));
  ss.str("");
  ss.clear();

  // color
  ss << "inLights[" << lightIndex << "]." << "color";
    uniformName = ss.str();
  uniLight = glGetUniformLocation(shaderProgram, uniformName.c_str());
  glUniform3fv(uniLight, 1, glm::value_ptr(light.color));
  ss.str("");
  ss.clear();

  // ambient
  ss << "inLights[" << lightIndex << "]." << "ambient";
    uniformName = ss.str();
  uniLight = glGetUniformLocation(shaderProgram, uniformName.c_str());
  glUniform1f(uniLight, light.ambient);
  ss.str("");
  ss.clear();

  // attemuation
  ss << "inLights[" << lightIndex << "]." << "attenuation";
    uniformName = ss.str();
  uniLight = glGetUniformLocation(shaderProgram, uniformName.c_str());
  glUniform1f(uniLight, light.attenuation);
  ss.str("");
  ss.clear();

  // ambient_only
  ss << "inLights[" << lightIndex << "]." << "ambient_only";
    uniformName = ss.str();
  uniLight = glGetUniformLocation(shaderProgram, uniformName.c_str());
  glUniform1f(uniLight, light.ambient_only);
  ss.str("");
  ss.clear();
}

void loadLights(int shaderProgram, vector<Light> lights)
{
  for (int i = 0; i < lights.size(); i++)
  {
    SetLightUniform(shaderProgram, i, lights[i]);
  }
}


int main(int argc, char *argv[]){
  // Initialize sliders and pieces
  slides.push_back({1, 4});
  slides.push_back({9, 13});
  slides.push_back({16, 19});
  slides.push_back({24, 28});
  slides.push_back({31, 34});
  slides.push_back({39, 43});
  slides.push_back({46, 49});
  slides.push_back({54, 58});

  pieces.push_back({gPlayer, 1, start, -1});
  pieces.push_back({gPlayer, 2, start, -1});
  pieces.push_back({gPlayer, 3, start, -1});
  pieces.push_back({rPlayer, 1, start, -1});
  pieces.push_back({rPlayer, 2, start, -1});
  pieces.push_back({rPlayer, 3, start, -1});
  pieces.push_back({bPlayer, 1, start, -1});
  pieces.push_back({bPlayer, 2, start, -1});
  pieces.push_back({bPlayer, 3, start, -1});
  pieces.push_back({yPlayer, 1, start, -1});
  pieces.push_back({yPlayer, 2, start, -1});
  pieces.push_back({yPlayer, 3, start, -1});  // 11

  // Seed rng
  srand(time(NULL));

  // Initalize lights
  lights.push_back({glm::vec4(0.f,-3.f,0.f,0.9f), glm::vec3(0.f,0.f,1.f), 0.0, 4, false}); // blue
  lights.push_back({glm::vec4(0.f,3.f,0.f,0.9f), glm::vec3(1.f,0.f,0.f), 0.0, 2, false}); // red
  lights.push_back({glm::vec4(0.f,-3.f,0.f,0.9f), glm::vec3(1.f,1.f,1.f), 0.2, 1.0, true}); // ambient


  SDL_Init(SDL_INIT_VIDEO);  //Initialize Graphics (for OpenGL)

  //Ask SDL to get a recent version of OpenGL (3.2 or greater)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

  //Create a window (offsetx, offsety, width, height, flags)
  SDL_Window* window = SDL_CreateWindow("My OpenGL Program", 100, 100, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

  //Create a context to draw in
  SDL_GLContext context = SDL_GL_CreateContext(window);
  
  //Load OpenGL extentions with GLAD
  if (gladLoadGLLoader(SDL_GL_GetProcAddress)){
    printf("\nOpenGL loaded\n");
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n\n", glGetString(GL_VERSION));
  }
  else {
    printf("ERROR: Failed to initialize OpenGL context.\n");
    return -1;
  }
  
  //Load Model 1
  ifstream modelFile;
  modelFile.open("models/testing4.txt");
  int numLines = 0;
  modelFile >> numLines;
  float maxheight = 0;
  float* model1 = new float[numLines];
  for (int i = 0; i < numLines; i+=24) {
    modelFile >> model1[i+21];
    modelFile >> model1[i+23];
    modelFile >> model1[i+22];
    modelFile >> model1[i+19];
    modelFile >> model1[i+20];
    modelFile >> model1[i+16];
    model1[i+16]--;
    modelFile >> model1[i+18];
    modelFile >> model1[i+17];
    model1[i+17]--;

    modelFile >> model1[i+13];
    modelFile >> model1[i+15];
    modelFile >> model1[i+14];
    modelFile >> model1[i+11];
    modelFile >> model1[i+12];
    modelFile >> model1[i+8];
    model1[i+8]--;
    modelFile >> model1[i+10];
    modelFile >> model1[i+9];
    model1[i+9]--;

    modelFile >> model1[i+5];
    modelFile >> model1[i+7];
    modelFile >> model1[i+6];
    modelFile >> model1[i+3];
    modelFile >> model1[i+4];
    modelFile >> model1[i];
    model1[i]--;
    modelFile >> model1[i+2];
    modelFile >> model1[i+1];
    model1[i+1]--;
  }
  for (int i = 0; i < numLines; i++) { //THIS LOOP IS FOR TEXTURING THE GAME PIECE
    if (i % 8 == 4) {
      model1[i] = model1[i-2] / 2.5;
    }
    if (i % 8 == 3) {
      float readx = model1[i-3];
      float ready = model1[i-2];
      float deg = atan2(ready,readx) * 180 / M_PI;
      deg += 180;
      model1[i] = deg / 360;
    }
  }
  printf("blah %d %f\n",numLines,maxheight);
  int numVertsTeapot = numLines/8;
  modelFile.close();
  
  //Load Model 2
  modelFile.open("models/table.txt");
  numLines = 0;
  modelFile >> numLines;
  float* model2 = new float[numLines];
  for (int i = 0; i < numLines; i+=8) {
    modelFile >> model2[i+5];
    model2[i+5]*=-1;
    modelFile >> model2[i+7];
    modelFile >> model2[i+6];
    model2[i+6]*=-1;
    modelFile >> model2[i+3];
    modelFile >> model2[i+4];
    modelFile >> model2[i];
    model2[i]-=11.4641016151;
    model2[i]*=-1;
    modelFile >> model2[i+2];
    modelFile >> model2[i+1];
    model2[i+1]-=11.4641016151;
  }
  for (int i = 0; i < numLines; i++) { //THIS LOOP IS FOR TEXTURING THE table
    if (i % 8 == 4 || i % 8 == 3) {
      model2[i] = (model2[i-3] + 11.4641016151) / 22.92820323;
    }
  }
  //printf("%d\n",numLines);
  int numVertsKnot = numLines/8;
  modelFile.close();

  modelFile.open("models/cube.txt");
  numLines = 0;
  modelFile >> numLines;
  float* model3 = new float[numLines];
  for (int i = 0; i < numLines; i++){
    modelFile >> model3[i];
  }
  for (int i = 0; i < numLines; i++) { //THIS LOOP IS FOR TEXTURING THE table
    if (i % 8 == 4 || i % 8 == 3) {
      model3[i] = (model3[i-3] + 0.5);
    }
  }
  int numVertsCube = numLines/8;
  modelFile.close();
  
  //Concatenate model arrays
  float* modelData = new float[(numVertsTeapot+numVertsKnot+numVertsCube)*8];
  copy(model1, model1+numVertsTeapot*8, modelData);
  copy(model2, model2+numVertsKnot*8, modelData+numVertsTeapot*8);
  copy(model3, model3+numVertsCube*8, modelData+(numVertsTeapot+numVertsKnot)*8);
  int totalNumVerts = numVertsTeapot+numVertsKnot+numVertsCube;
  int startVertTeapot = 0;  //The teapot is the first model in the VBO
  int startVertKnot = numVertsTeapot; //The knot starts right after the taepot
  int startVertCube = numVertsTeapot+numVertsKnot;
  
  
  //// Allocate Texture 0 board ///////
  SDL_Surface* surface = SDL_LoadBMP("sorryboardgrey.bmp");
  if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex0;
    glGenTextures(1, &tex0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface);
    //// End Allocate Texture ///////


  //// Allocate Texture 1 cardback ///////
  SDL_Surface* surface1 = SDL_LoadBMP("cardback.bmp");
  if (surface==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
    GLuint tex1;
    glGenTextures(1, &tex1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface1->w,surface1->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface1->pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(surface1);
  //// End Allocate Texture ///////

  //Allocate tex 2 1card
  SDL_Surface* surface2 = SDL_LoadBMP("1card.bmp");
  if (surface==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex2;
  glGenTextures(1, &tex2);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, tex2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface2->w,surface2->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface2->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface2);

  //Allocate tex 3 2card
  SDL_Surface* surface3 = SDL_LoadBMP("2card.bmp");
  if (surface3==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex3;
  glGenTextures(1, &tex3);
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, tex3);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface3->w,surface3->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface3->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface3);

  //Allocate tex 4 3card
  SDL_Surface* surface4 = SDL_LoadBMP("3card.bmp");
  if (surface4==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex4;
  glGenTextures(1, &tex4);
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, tex4);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface4->w,surface4->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface4->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface4);

  //Allocate tex 5 4card
  SDL_Surface* surface5 = SDL_LoadBMP("4card.bmp");
  if (surface5==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex5;
  glGenTextures(1, &tex5);
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, tex5);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface5->w,surface5->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface5->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface5);

  //Allocate tex 6 5card
  SDL_Surface* surface6 = SDL_LoadBMP("5card.bmp");
  if (surface6==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex6;
  glGenTextures(1, &tex6);
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, tex6);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface6->w,surface6->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface6->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface6);

  //Allocate tex 7 7card
  SDL_Surface* surface7 = SDL_LoadBMP("7card.bmp");
  if (surface7==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex7;
  glGenTextures(1, &tex7);
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, tex7);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface7->w,surface7->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface7->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface7);

  //Allocate tex 8 8card
  SDL_Surface* surface8 = SDL_LoadBMP("8card.bmp");
  if (surface8==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex8;
  glGenTextures(1, &tex8);
  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D, tex8);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface8->w,surface8->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface8->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface8);

  //Allocate tex 9 10card
  SDL_Surface* surface9 = SDL_LoadBMP("10card.bmp");
  if (surface9==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex9;
  glGenTextures(1, &tex9);
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D, tex9);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface9->w,surface9->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface9->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface9);

  //Allocate tex 10 11card
  SDL_Surface* surface10 = SDL_LoadBMP("11card.bmp");
  if (surface10==NULL){ //If it failed, print the error
    printf("Error: \"%s\"\n",SDL_GetError());
    return 1;
  }
  GLuint tex10;
  glGenTextures(1, &tex10);
  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_2D, tex10);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface10->w,surface10->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface10->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface10);

  //Allocate tex 11 12card
  SDL_Surface* surface11 = SDL_LoadBMP("12card.bmp");
  if (surface11==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
  GLuint tex11;
  glGenTextures(1, &tex11);
  glActiveTexture(GL_TEXTURE11);
  glBindTexture(GL_TEXTURE_2D, tex11);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface11->w,surface11->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface11->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface11);

  //Allocate tex 12 oopscard
  SDL_Surface* surface12 = SDL_LoadBMP("oopscard.bmp");
  if (surface12==NULL){ //If it failed, print the error
        printf("Error: \"%s\"\n",SDL_GetError()); return 1;
    }
  GLuint tex12;
  glGenTextures(1, &tex12);
  glActiveTexture(GL_TEXTURE12);
  glBindTexture(GL_TEXTURE_2D, tex12);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface12->w,surface12->h, 0, GL_BGR,GL_UNSIGNED_BYTE,surface12->pixels);
  glGenerateMipmap(GL_TEXTURE_2D);
  SDL_FreeSurface(surface12);
  
  //Build a Vertex Array Object (VAO) to store mapping of shader attributse to VBO
  GLuint vao;
  glGenVertexArrays(1, &vao); //Create a VAO
  glBindVertexArray(vao); //Bind the above created VAO to the current context

  //Allocate memory on the graphics card to store geometry (vertex buffer object)
  GLuint vbo[1];
  glGenBuffers(1, vbo);  //Create 1 buffer called vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
  glBufferData(GL_ARRAY_BUFFER, totalNumVerts*8*sizeof(float), modelData, GL_STATIC_DRAW); //upload vertices to vbo
  
  int texturedShader = InitShader("textured-Vertex.glsl", "textured-Fragment.glsl");  
  int blurShader = InitShader("blur_v.glsl", "blur_f.glsl");  
  int bloomShader = InitShader("bloom_v.glsl", "bloom_f.glsl");  
  
  //Tell OpenGL how to set fragment shader input 
  GLint posAttrib = glGetAttribLocation(texturedShader, "position");
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
  //Attribute, vals/attrib., type, isNormalized, stride, offset
  glEnableVertexAttribArray(posAttrib);
  
  //GLint colAttrib = glGetAttribLocation(phongShader, "inColor");
  //glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
  //glEnableVertexAttribArray(colAttrib);
  
  GLint normAttrib = glGetAttribLocation(texturedShader, "inNormal");
  glVertexAttribPointer(normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
  glEnableVertexAttribArray(normAttrib);
  
  GLint texAttrib = glGetAttribLocation(texturedShader, "inTexcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

  GLint uniView = glGetUniformLocation(texturedShader, "view");
  GLint uniProj = glGetUniformLocation(texturedShader, "proj");

  glBindVertexArray(0); //Unbind the VAO in case we want to create a new one  
                       
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);  //Be default: CCW are front faces, CW are back ffaces
  glCullFace(GL_BACK);  //Don't draw an CW (back) faces  

  printf("%s\n",INSTRUCTIONS);

  // BLOOM
  unsigned int hdrFBO;
  glGenFramebuffers(1, &hdrFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  unsigned int colorBuffers[2];
  glGenTextures(2, colorBuffers);
  for (unsigned int i = 0; i < 2; i++)
  {
      glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
      glTexImage2D(
          GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL
      );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      // attach texture to framebuffer
      glFramebufferTexture2D(
          GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
      );
  }
  // create and attach depth buffer (renderbuffer)
  unsigned int rboDepth;
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
  // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glDrawBuffers(2, attachments);
  // finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      std::cout << "Framebuffer not complete!" << std::endl;
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // ping-pong-framebuffer for blurring
  unsigned int pingpongFBO[2];
  unsigned int pingpongColorbuffers[2];
  glGenFramebuffers(2, pingpongFBO);
  glGenTextures(2, pingpongColorbuffers);
  for (unsigned int i = 0; i < 2; i++)
  {
      glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
      glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
      // also check if framebuffers are complete (no need for depth buffer)
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
          std::cout << "Framebuffer not complete!" << std::endl;
  }
  // BLOOM END



  /********** Event Loop (Loop forever processing each event as fast as possible) **********/
  SDL_Event windowEvent;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&windowEvent)){  //inspect all events in the queue
      if (windowEvent.type == SDL_QUIT) quit = true;
      //List of keycodes: https://wiki.libsdl.org/SDL_Keycode - You can catch many special keys
      //Scancode referes to a keyboard position, keycode referes to the letter (e.g., EU keyboards)
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE) 
        quit = true; //Exit event loop
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_f){ //If "f" is pressed
        fullscreen = !fullscreen;
        SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0); //Toggle fullscreen 
      }
      //DEBUG KEYS
      /*if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_UP){ //If "up key" is pressed
        if (windowEvent.key.keysym.mod & KMOD_SHIFT) objx -= .1; //Is shift pressed?
        else objy -= .01;
        printf("x: %f | y: %f\n",objx,objy);
      }
      if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_DOWN){ //If "down key" is pressed
        if (windowEvent.key.keysym.mod & KMOD_SHIFT) objx += .1; //Is shift pressed?
        else objy += .01;
        printf("x: %f | y: %f\n",objx,objy);
      }
        if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_LEFT){ //If "up key" is pressed
        objx += .01;
        printf("x: %f | y: %f\n",objx,objy);
      }
      if (windowEvent.type == SDL_KEYDOWN && windowEvent.key.keysym.sym == SDLK_RIGHT){ //If "down key" is pressed
        objx -= .01;
        printf("x: %f | y: %f\n",objx,objy);
      }*/
      /*if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_c){ // If "c" is pressed
        colR = rand01();
        colG = rand01();
        colB = rand01();
        if (textest == 12)
          textest = -1;
        else
          textest++;
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_m) { // If "m" is pressed
        movePiece(&pieces.at(testNum), 1);
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_n) { // If "n" is pressed
        movePiece(&pieces.at(testNum), 4);
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_b) { // If "b" is pressed
        testNum = (testNum + 1) % 12;
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_r) { // If "r" is pressed
        cardposition = 0.0;
        state = turnBegin;
      }*/
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_RETURN) { // If "enter" is pressed
        waiting = false;
      }
      /*if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_z) { // If "z" is pressed
        printf("Current state: %d\n", state);
      }*/
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_1) { // If "1" is pressed
        playerInput = 1;
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_2) { // If "2" is pressed
        playerInput = 2;
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_3) { // If "3" is pressed
        playerInput = 3;
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_4) { // If "4" is pressed
        playerInput = 4;
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_5) { // If "5" is pressed
        playerInput = 5;
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_6) { // If "6" is pressed
        playerInput = 6;
      }
      if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_7) { // If "7" is pressed
        playerInput = 7;
      }
    }
      
    // Clear the screen to default color
    // glClearColor(.2f, 0.4f, 0.8f, 1.0f);
    glClearColor(.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  // BLOOM
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //ENDBLOOM

  glUseProgram(texturedShader);

	lastUpdated = timePast;
    timePast = SDL_GetTicks()/1000.f; 

    if (state == movingCamera) {
      if (cameraposition < 1.0) {
        cameraposition += timePast - lastUpdated;
        if (cameraposition >= 1.0) {
          cameraposition = 1.0;
          state = turnBegin;
          //waiting = false;
        }
      }
    }
    glm::vec3 translatecamera = camPos[currTurn] - camPos[(currTurn+3)%4];
    glm::mat4 view = glm::lookAt(
      camPos[(currTurn+3)%4] + 
      glm::vec3(translatecamera.x * cameraposition, translatecamera.y * cameraposition, translatecamera.z * cameraposition),  //Cam Position
      glm::vec3(0.0f, 0.f, -5.0f),  //Look at point
      glm::vec3(0.0f, 0, 1)); //Up
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

    glm::mat4 proj = glm::perspective(3.14f/4, screenWidth / (float) screenHeight, 1.0f, 40.0f); //FOV, aspect, near, far
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

    GLint uniLightDir = glGetUniformLocation(texturedShader, "inLightDir");
    // glUniform3fv(uniLightDir, 1, glm::value_ptr(lightDirs[(currTurn)%4])); 
    glm::vec3 temp = normalize(glm::vec3(0.5,1,-5));
    glUniform3fv(uniLightDir, 1, glm::value_ptr(temp)); 

    GLint uniLightCol = glGetUniformLocation(texturedShader, "lightColor");
    glm::vec3 weakColor = pawnColors[((currTurn+3)%4)*3+1];
    weakColor /= 16.0;
    weakColor = glm::vec3(1.0) - weakColor;
    glUniform3fv(uniLightCol, 1, glm::value_ptr(weakColor)); 

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0);
    glUniform1i(glGetUniformLocation(texturedShader, "tex0"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1);
    glUniform1i(glGetUniformLocation(texturedShader, "tex1"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2);
    glUniform1i(glGetUniformLocation(texturedShader, "tex2"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, tex3);
    glUniform1i(glGetUniformLocation(texturedShader, "tex3"), 3);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, tex4);
    glUniform1i(glGetUniformLocation(texturedShader, "tex4"), 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, tex5);
    glUniform1i(glGetUniformLocation(texturedShader, "tex5"), 5);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, tex6);
    glUniform1i(glGetUniformLocation(texturedShader, "tex6"), 6);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, tex7);
    glUniform1i(glGetUniformLocation(texturedShader, "tex7"), 7);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, tex8);
    glUniform1i(glGetUniformLocation(texturedShader, "tex8"), 8);

    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, tex9);
    glUniform1i(glGetUniformLocation(texturedShader, "tex9"), 9);

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, tex10);
    glUniform1i(glGetUniformLocation(texturedShader, "tex10"), 10);

    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, tex11);
    glUniform1i(glGetUniformLocation(texturedShader, "tex11"), 11);

    glActiveTexture(GL_TEXTURE12);
    glBindTexture(GL_TEXTURE_2D, tex12);
    glUniform1i(glGetUniformLocation(texturedShader, "tex12"), 12);

    glBindVertexArray(vao);

    takeTurn();
    drawGeometry(texturedShader, startVertTeapot, numVertsTeapot, startVertKnot, numVertsKnot, startVertCube, numVertsCube);

    // BLOOM
    bool horizontal = true, first_iteration = true;
    unsigned int amount = 10;
    glUseProgram(blurShader);
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        GLint uniBlur = glGetUniformLocation(blurShader, "horizontal");
        glUniform1i(uniBlur, horizontal); 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? colorBuffers[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        renderQuad();
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(bloomShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glUniform1i(glGetUniformLocation(bloomShader, "scene"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
    glUniform1i(glGetUniformLocation(bloomShader, "bloomBlur"), 1);
    renderQuad();
    // ENDBLOOM

    SDL_GL_SwapWindow(window); //Double buffering
  }
  
  //Clean Up
  glDeleteProgram(texturedShader);
    glDeleteBuffers(1, vbo);
    glDeleteVertexArrays(1, &vao);

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


void drawGeometry(int shaderProgram, int model1_start, int model1_numVerts, int model2_start, int model2_numVerts,
                  int model3_start, int model3_numVerts) {
  // set color                  
  GLint uniColor = glGetUniformLocation(shaderProgram, "inColor");
  glm::vec3 colVec(colR,colG,colB);
  glUniform3fv(uniColor, 1, glm::value_ptr(colVec));
  // get texture id loc
  GLint uniTexID = glGetUniformLocation(shaderProgram, "texID");
  // get model loc
  glm::mat4 model = glm::mat4(1);
  GLint uniModel = glGetUniformLocation(shaderProgram, "model");
  // get toon toggle loc
  GLint uniToon = glGetUniformLocation(shaderProgram, "toon");
  // get bloom toggle loc
  GLint uniBloom = glGetUniformLocation(shaderProgram, "bloom");
   

  loadLights(shaderProgram, lights); // Load Lights

  // DRAW PIECES
  glUniform1i(uniToon, 1.0); // use toon for pieces
  
  for (int i = 0; i < pieces.size(); i++) {
    Piece currPiece = pieces.at(i);
    glUniform1i(uniBloom, 0.0);

    if (state == movingPiece ) {
      if (currPiece.num == movedpiece && movedplayer == currPiece.player.num) {glUniform1i(uniBloom, 1.0); }
      else {glUniform1i(uniBloom, 0.0); }
    }


    if (currPiece.num == movedpiece && currPiece.player.num == movedplayer && movepath.size() != 0) {
      //printf("%d modementindex\n",movementindex);
      glm::vec3 translatevector = movepath.at(movementindex) - movepath.at(movementindex-1);
      model = glm::mat4(1);
      float zmov;
      if (pieceposition <= 0.5)
        zmov = pieceposition * 2;
      else
        zmov = 2-(pieceposition * 2);
      model = glm::translate(model,movepath.at(movementindex-1) +
              glm::vec3(translatevector.x * pieceposition, translatevector.y * pieceposition, zmov));
      model = glm::scale(model,glm::vec3(0.25,0.25,0.25));
      glUniform3f(uniColor, pawnColors[i][0], pawnColors[i][1], pawnColors[i][2]);
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
      glUniform1i(uniTexID, -1); 
      glDrawArrays(GL_TRIANGLES, model1_start, model1_numVerts);
      if (state == movingPiece) {
        if (pieceposition < 1.0) {
          pieceposition += (timePast - lastUpdated) * 2;
          if (pieceposition >= 1.0 && movementindex == movepath.size()-1) {
            pieceposition = 0.0;
            movementindex = 1;
            //movedpiece = -1;
            //movedplayer = -1;
            if (slidCheck)
              slidStart = glm::vec3(movepath.at(movepath.size()-1));
            movepath.clear();
            //state = turnEnd;
            //waiting = false;
          }
          else if (pieceposition >= 1.0) {
            pieceposition = 1.0;
          }
        }
        else {
          pieceposition = 0;
          movementindex++;
        }
      }
    }
    else if (currPiece.num == movedpiece && currPiece.player.num == movedplayer && movepath.size() == 0) {
      if (slidCheck) {
        glm::vec3 translatevector = slidPos - slidStart;
        model = glm::mat4(1);
        model = glm::translate(model,slidStart +
                glm::vec3(translatevector.x * pieceposition, translatevector.y * pieceposition, 0));
        model = glm::scale(model,glm::vec3(0.25,0.25,0.25));
        glUniform3f(uniColor, pawnColors[i][0], pawnColors[i][1], pawnColors[i][2]);
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(uniTexID, -1); 
        glDrawArrays(GL_TRIANGLES, model1_start, model1_numVerts);
        if (state == movingPiece) {
          if (pieceposition < 1.0) {
            pieceposition += (timePast - lastUpdated) * 2;
            if (pieceposition >= 1.0) {
              pieceposition = 1.0;
              slidCheck = false;
            }
          }
        }
      }
      else {
        movedpiece = -1;
        movedplayer = -1;
        pieceposition = 0;
        if (!oopsCheck) {
          state = turnEnd;
        }
      }
    }
    else if (currPiece.num == oopspiece && currPiece.player.num == oopsplayer) {
      float checkx, checky;
      altgetXY(start, -1, oopspiece, oopsplayer, &checkx, &checky);
      glm::vec3 translatevector = glm::vec3(checkx,checky,0) - oopsPos;
      float zmov;
      if (pieceposition <= 0.5)
        zmov = (pieceposition * 2)*2;
      else
        zmov = (2-(pieceposition * 2))*2;
      model = glm::mat4(1);
      if (movedpiece == -1 && movedplayer == -1) {
        model = glm::translate(model,oopsPos +
                glm::vec3(translatevector.x * pieceposition, translatevector.y * pieceposition, zmov));
      }
      else {
        model = glm::translate(model,oopsPos);
      }
      model = glm::scale(model,glm::vec3(0.25,0.25,0.25));
      glUniform3f(uniColor, pawnColors[i][0], pawnColors[i][1], pawnColors[i][2]);
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
      glUniform1i(uniTexID, -1); 
      glDrawArrays(GL_TRIANGLES, model1_start, model1_numVerts);
        
      if (oopsCheck) {
        if (state == movingPiece && movedpiece == -1 && movedplayer == -1) {
          if (pieceposition < 1.0) {
            pieceposition += (timePast - lastUpdated) * 2;
            if (pieceposition >= 1.0) {
              pieceposition = 1.0;
              oopsCheck = false;
              oopspiece = -1;
              oopsplayer = -1;
              pieceposition = 0;
              state = turnEnd;
            }
          }
        }
      }
    }
    else {
      float x;
      float y;
      getXY(currPiece, &x, &y);
      model = glm::mat4(1);
      model = glm::translate(model,glm::vec3(x, y, 0.01));
      model = glm::scale(model,glm::vec3(0.25,0.25,0.25));
      glUniform3f(uniColor, pawnColors[i][0], pawnColors[i][1], pawnColors[i][2]);
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
      glUniform1i(uniTexID, -1); 
      glDrawArrays(GL_TRIANGLES, model1_start, model1_numVerts);
    }
  }

  //DRAW GAME BOARD
  model = glm::mat4(1);
  model = glm::translate(model,glm::vec3(0,0,-0.05));
  model = glm::scale(model,glm::vec3(11,11,0.1));
  glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
  glUniform1i(uniTexID, 0);
  glUniform1i(uniToon, 0.0); // don't use toon for pieces
  glUniform1i(uniBloom, 0.0); // don't use bloom
  glDrawArrays(GL_TRIANGLES, model3_start, model3_numVerts);
  
    
  //DRAW CARD DECK
  model = glm::mat4(1);
  model = glm::translate(model,glm::vec3(-1.03,1.75,0.5));
  model = glm::rotate(model,-3.14f/4,glm::vec3(0.0f, 0.0f, 1.0f));
  model = glm::scale(model,glm::vec3(2.0,3.0,1.f));
  glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
  glUniform1i(uniTexID, 1); 
  glUniform1i(uniToon, 1.0); // use toon for pieces
  glUniform1i(uniBloom, 0.0); // don't use bloom
  glDrawArrays(GL_TRIANGLES, model3_start, model3_numVerts);

  //DRAW CARD
  if (state == drawingCard) {
    if (cardposition < 1.0) {
      cardposition += timePast - lastUpdated;
      if (cardposition >= 1.0) {
        cardposition = 1.0;
        state = choosePiece;
        waiting = false;
        printf("Choose Piece\n");
      }
    } 
  }
  glm::vec3 translatevector = glm::vec3(1.13,-1.93,0.05) - glm::vec3(-1.03,1.75,0.9);
  model = glm::mat4(1);
  model = glm::translate(model,glm::vec3(-1.03,1.75,0.9) +
          glm::vec3(translatevector.x * cardposition, translatevector.y * cardposition, translatevector.z * cardposition));
  model = glm::rotate(model,-3.14f/4,glm::vec3(0.0f, 0.0f, 1.f));
  model = glm::scale(model,glm::vec3(1.9,2.9,0.1f));
  glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
  glUniform1i(uniTexID, displayCard); 
  glDrawArrays(GL_TRIANGLES, model3_start, model3_numVerts);

  //DRAW TABLE
  glUniform3fv(uniColor, 1, glm::value_ptr(glm::vec3(0.43,0.34,0.17)));
  model = glm::mat4(1);
  model = glm::translate(model,glm::vec3(0,0,-13.2));
  //model = glm::rotate(model,-3.14f/4,glm::vec3(0.0f, 0.0f, 1.f));
  //model = glm::scale(model,glm::vec3(0.5,0.5,0.5));
  glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
  glUniform1i(uniTexID, -1); 
  glUniform1i(uniToon, 0.0); // don't use toon for table
  glUniform1i(uniBloom, 0.0); // don't use bloom
  glDrawArrays(GL_TRIANGLES, model2_start, model2_numVerts);

  //DRAW UI
  for (int i = 0; i < 4; i++) {
    model = glm::mat4(1);
    if (i==0) {
      model = glm::translate(model,glm::vec3(4.56,7.63,-1.4));
      model = glm::rotate(model,-3.14f/6,glm::vec3(1.0f, 0.0f, 0.f));
    }
    else if (i==1) {
      model = glm::translate(model,glm::vec3(7.63,-4.56,-1.4));
      model = glm::rotate(model,3.14f/6,glm::vec3(0.0f, 1.0f, 0.f));
      model = glm::rotate(model,-3.14f/2,glm::vec3(0.0f, 0.f, 1.f));
    }
    else if (i==2) {
      model = glm::translate(model,glm::vec3(-4.56,-7.63,-1.4));
      model = glm::rotate(model,3.14f/6,glm::vec3(1.0f, 0.0f, 0.f));
      model = glm::rotate(model,3.14f,glm::vec3(0.0f, 0.f, 1.f));
    }
    else {
      model = glm::translate(model,glm::vec3(-7.63,4.56,-1.4));
      model = glm::rotate(model,-3.14f/6,glm::vec3(0.0f, 1.0f, 0.f));
      model = glm::rotate(model,3.14f/2,glm::vec3(0.0f, 0.f, 1.f));
    }
    model = glm::scale(model,glm::vec3(2.0,3.0,0.1f));
    glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(uniTexID, displayCard); 
    glUniform1i(uniBloom, 0.0); // don't use bloom
    glDrawArrays(GL_TRIANGLES, model3_start, model3_numVerts);
  }
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < 3; i++) {
      glUniform3fv(uniColor, 1, glm::value_ptr(pawnColors[(j*3)+i]));
      model = glm::mat4(1);
      if (j == 0) {
        model = glm::translate(model,glm::vec3(1.58-(i*3),9.0,-2.0));
        model = glm::rotate(model,3.14f/3,glm::vec3(1.0f, 0.0f, 0.f));
      }
      else if (j == 1) {
        model = glm::translate(model,glm::vec3(9.0,-1.58+(i*3),-2.0));
        model = glm::rotate(model,-3.14f/3,glm::vec3(0.0f, 1.0f, 0.f));
      }
      else if (j == 2) {
        model = glm::translate(model,glm::vec3(-1.58+(i*3),-9.0,-2.0));
        model = glm::rotate(model,-3.14f/3,glm::vec3(1.0f, 0.0f, 0.f));
      }
      else {
        model = glm::translate(model,glm::vec3(-9.0,1.58-(i*3),-2.0));
        model = glm::rotate(model,3.14f/3,glm::vec3(0.0f, 1.0f, 0.f));
      }
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(model));
      glUniform1i(uniTexID, -1); 
      glUniform1i(uniToon, 1.0); // use toon for UI pieces
      glUniform1i(uniBloom, 0.0); // don't use bloom
      glDrawArrays(GL_TRIANGLES, model1_start, model1_numVerts);
    }
  }

}

// Create a NULL-terminated string by reading the provided file
static char* readShaderSource(const char* shaderFile){
  FILE *fp;
  long length;
  char *buffer;

  // open the file containing the text of the shader code
  fp = fopen(shaderFile, "r");

  // check for errors in opening the file
  if (fp == NULL) {
    printf("can't open shader source file %s\n", shaderFile);
    return NULL;
  }

  // determine the file size
  fseek(fp, 0, SEEK_END); // move position indicator to the end of the file;
  length = ftell(fp);  // return the value of the current position

  // allocate a buffer with the indicated number of bytes, plus one
  buffer = new char[length + 1];

  // read the appropriate number of bytes from the file
  fseek(fp, 0, SEEK_SET);  // move position indicator to the start of the file
  fread(buffer, 1, length, fp); // read all of the bytes

  // append a NULL character to indicate the end of the string
  buffer[length] = '\0';

  // close the file
  fclose(fp);

  // return the string
  return buffer;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint InitShader(const char* vShaderFileName, const char* fShaderFileName){
  GLuint vertex_shader, fragment_shader;
  GLchar *vs_text, *fs_text;
  GLuint program;

  // check GLSL version
  printf("GLSL version: %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Create shader handlers
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  // Read source code from shader files
  vs_text = readShaderSource(vShaderFileName);
  fs_text = readShaderSource(fShaderFileName);

  // error check
  if (vs_text == NULL) {
    printf("Failed to read from vertex shader file %s\n", vShaderFileName);
    exit(1);
  } else if (DEBUG_ON) {
    printf("Vertex Shader:\n=====================\n");
    printf("%s\n", vs_text);
    printf("=====================\n\n");
  }
  if (fs_text == NULL) {
    printf("Failed to read from fragent shader file %s\n", fShaderFileName);
    exit(1);
  } else if (DEBUG_ON) {
    printf("\nFragment Shader:\n=====================\n");
    printf("%s\n", fs_text);
    printf("=====================\n\n");
  }

  // Load Vertex Shader
  const char *vv = vs_text;
  glShaderSource(vertex_shader, 1, &vv, NULL);  //Read source
  glCompileShader(vertex_shader); // Compile shaders
  
  // Check for errors
  GLint  compiled;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    printf("Vertex shader failed to compile:\n");
    if (DEBUG_ON) {
      GLint logMaxSize, logLength;
      glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
      printf("printing error message of %d bytes\n", logMaxSize);
      char* logMsg = new char[logMaxSize];
      glGetShaderInfoLog(vertex_shader, logMaxSize, &logLength, logMsg);
      printf("%d bytes retrieved\n", logLength);
      printf("error message: %s\n", logMsg);
      delete[] logMsg;
    }
    exit(1);
  }
  
  // Load Fragment Shader
  const char *ff = fs_text;
  glShaderSource(fragment_shader, 1, &ff, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compiled);
  
  //Check for Errors
  if (!compiled) {
    printf("Fragment shader failed to compile\n");
    if (DEBUG_ON) {
      GLint logMaxSize, logLength;
      glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &logMaxSize);
      printf("printing error message of %d bytes\n", logMaxSize);
      char* logMsg = new char[logMaxSize];
      glGetShaderInfoLog(fragment_shader, logMaxSize, &logLength, logMsg);
      printf("%d bytes retrieved\n", logLength);
      printf("error message: %s\n", logMsg);
      delete[] logMsg;
    }
    exit(1);
  }

  // Create the program
  program = glCreateProgram();

  // Attach shaders to program
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);

  // Link and set program to use
  glLinkProgram(program);

  return program;
}

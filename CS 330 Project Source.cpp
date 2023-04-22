#include <iostream>         // cout, cerr
#include <cstdlib>          // EXIT_FAILURE
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#include <math.h>           // For trig functions
#include <vector>           // For generating arrays

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Camera class
#include <camera.h> 

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions


using namespace std; // Declares the implied namespace for the file

/*Shader program Macro*/
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// Unnamed namespace
namespace
{
    const char* const WINDOW_TITLE = "3D Scene"; // Macro for window title

    // Variables for window width and height
    const int WINDOW_WIDTH = 2400;
    const int WINDOW_HEIGHT = 1800;

    // Stores the GL data relative to a given mesh
    struct GLMesh
    {
        GLuint vao;         // Handle for the vertex array object
        GLuint vbo;         // Handles for the vertex buffer objects
        GLuint ibo;          // Index buffer object for drawing by index
        GLuint nVertices;
        GLuint size;
        GLuint indSize = 0;
        GLuint nIndices = 0; // init as 0 for logic in render function.
    };
    //mesh objects
    GLMesh cookieSheet, batteryCharger, ball, battery, hairTie;
    // Here we define how our arrays are set up
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;
    // Main GLFW window
    GLFWwindow* gWindow = nullptr;
    // Shader program
    GLuint gProgramId;
    // light shader program
    GLuint gLightProgramId;

    glm::vec2 gUVScale(1.0f, 1.0f);

    // Object texture IDS
    GLuint cookieSheetTexture, batteryChargerTexture, ballTexture, batteryTexture, canTexture, hairTieTexture;

    // camera
    Camera gCamera(glm::vec3(0.0f, 0.0f, 3.0f));
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // timing
    float gDeltaTime = 0.0f; // time between current frame and last frame
    float gLastFrame = 0.0f;

    //view type
    bool orthoView = false;

    //for my circles :)
    float pi = 3.14159265358979323846;

    //used to pass objects with no index data to render
    GLuint blankIndices[];

    GLuint baseCenterIndex;
    GLuint topCenterIndex;
}

/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UProcessInput(GLFWwindow* window);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void UCreateMesh(GLMesh& mesh, GLfloat verts[], GLuint indices[]);
vector<GLfloat> UGenerateSphereVerts(GLMesh mesh);
vector<GLuint> UGenerateSphereIndices(GLMesh mesh);
vector<GLfloat> UGenerateCylinderVerts(GLMesh &mesh);
vector<GLuint> UGenerateCylinderIndices(GLMesh mesh);
vector<GLfloat> UGenerateHairTieVerts(GLMesh mesh);
vector<GLuint> UGenerateHairTieIndices(GLMesh mesh);
void UDestroyMesh(GLMesh& mesh);
bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);
void URenderTriangles(glm::mat4 model, GLMesh mesh, GLuint indices[]);
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId);
void UDestroyShaderProgram(GLuint programId);
void renderLight();


/* Vertex Shader Source Code*/
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position;
    layout(location = 1) in vec3 normal;
    layout(location = 2) in vec2 textureCoordinate;

    out vec3 vertexNormal; // For outgoing normals to fragment shader
    out vec3 vertexFragmentPos; // For outgoing color / pixels to fragment shader
    out vec2 vertexTextureCoordinate; // variable to transfer color data to the fragment shader

    //Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
        vertexFragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)
        vertexNormal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
        vertexTextureCoordinate = textureCoordinate; // references incoming texture data
    }
);



/* Fragment Shader Source Code*/
const GLchar* fragmentShaderSource = GLSL(440,
    in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate;
out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPosition;

uniform vec3 objectColorTwo;
uniform vec3 lightColorTwo;
uniform vec3 lightPosTwo;
uniform vec3 viewPositionTwo;

uniform sampler2D uTexture; // Useful when working with multiple textures
uniform vec2 uvScale;

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.001f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    vec3 lightDirection = normalize(lightPos - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact = max(dot(norm, lightDirection), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    vec3 diffuse = impact * lightColor; // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.005f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    vec3 reflectDir = reflect(-lightDirection, norm);// Calculate reflection vector
    //Calculate specular component
    float specularComponent = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
    vec3 specular = specularIntensity * specularComponent * lightColor;

    //Caluclate second light
    float secondAmbientStrength = 0.001f;
    vec3 secondAmbient = secondAmbientStrength * lightColorTwo;

    vec3 secondLightDirection = normalize(lightPosTwo - vertexFragmentPos);
    float secondImpact = max(dot(norm, secondLightDirection), 0.0);
    vec3 secondDiffuse = secondImpact * lightColorTwo;

    float secondSpecularIntensity = 0.005f;
    vec3 secondViewDir = normalize(viewPositionTwo - vertexFragmentPos);
    vec3 secondReflectDir = reflect(-secondLightDirection, norm);
    float secondSpecularComponent = pow(max(dot(secondViewDir, secondReflectDir), 0.0), highlightSize);
    vec3 secondSpecular = secondSpecularIntensity * secondSpecularComponent * lightColorTwo;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate * uvScale);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular + secondSpecular + secondAmbient + secondDiffuse) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}
);


/* Lamp Shader Source Code*/
const GLchar* lampVertexShaderSource = GLSL(440,

    layout(location = 0) in vec3 position; // VAP position 0 for vertex position data

    //Uniform / Global variables for the  transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // Transforms vertices into clip coordinates
    }
);


/* Fragment Shader Source Code*/
const GLchar* lampFragmentShaderSource = GLSL(440,

    out vec4 fragmentColor; // For outgoing lamp color (smaller cube) to the GPU

    void main()
    {
        fragmentColor = vec4(1.0f); // Set color to white (1.0f,1.0f,1.0f) with alpha 1.0
    }
);


//COOKIE SHEET OBJECT
GLfloat cookieSheetVerts[]
{
    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
    0.0f, 0.75f,0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f,
                                    
    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,    1.0f, 0.0f,
    1.0f, 0.75f,0.0f,   0.0f, 0.0f, 1.0f,    1.0f, 1.0f,
    0.0f, 0.75f,0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 1.0f
};
glm::mat4 cookieSheetModel = glm::scale(glm::vec3(6.5f, 6.5f, 6.5f));


//BATTERY CHARGER OBJECT
GLfloat batteryChargerVerts[]
{
    //top cube
    0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.2f,//0
    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.2f,//1
    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,//3
                                    
    1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.2f,//1
    1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,//2
    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,//3

    0.0f, 0.0f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,//5
    0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.2f, 0.0f,//0
    0.0f, 1.0f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 0.2f,//6

    0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.2f, 0.0f,//0
    0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  0.2f, 0.2f,//3
    0.0f, 1.0f, -0.5f, -1.0f, 0.0f, 0.0f,  0.0f, 0.2f,//6
                       
    1.0f, 0.0f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f,//4
    0.0f, 0.0f, -0.5f,  0.0f, 0.0f,-1.0f,  0.2f, 0.0f,//5
    1.0f, 1.0f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 0.2f,//7

    0.0f, 0.0f, -0.5f,  0.0f, 0.0f,-1.0f,  0.2f, 0.0f,//5
    0.0f, 1.0f, -0.5f,  0.0f, 0.0f,-1.0f,  0.2f, 0.2f,//6
    1.0f, 1.0f, -0.5f,  0.0f, 0.0f,-1.0f,  0.0f, 0.2f,//7 
                                   
    1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.0f,//1
    1.0f, 0.0f, -0.5f,  1.0f, 0.0f, 0.0f,  0.2f, 0.0f,//4
    1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.2f,//2

    1.0f, 0.0f, -0.5f,  1.0f, 0.0f, 0.0f,  0.2f, 0.0f,//4
    1.0f, 1.0f, -0.5f,  1.0f, 0.0f, 0.0f,  0.2f, 0.2f,//7 
    1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,  0.0f, 0.2f,//2
                        
    0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,  0.0f, 0.0f,//3
    1.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,  0.2f, 0.0f,//2
    0.0f, 1.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.2f,//6
                              
    1.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,  0.2f, 0.0f,//2
    1.0f, 1.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.2f, 0.2f,//7 
    0.0f, 1.0f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.2f,//6

    0.0f, 0.0f, -0.5f,  0.0f,-1.0f, 0.0f,  0.0f, 0.0f,//5
    1.0f, 0.0f, -0.5f,  0.0f,-1.0f, 0.0f,  0.2f, 0.0f,//4
    0.0f, 0.0f, 0.0f,   0.0f,-1.0f, 0.0f,  0.0f, 0.2f,//0
                             
    1.0f, 0.0f, -0.5f,  0.0f,-1.0f, 0.0f,  0.2f, 0.0f,//4
    1.0f, 0.0f, 0.0f,   0.0f,-1.0f, 0.0f,  0.2f, 0.2f,//1
    0.0f, 0.0f, 0.0f,   0.0f,-1.0f, 0.0f,  0.0f, 0.2f,//0

    //bottom cube
    0.0f, -1.0f, -0.25f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,//8
    1.0f, -1.0f, -0.25f,    0.0f, 0.0f, 1.0f,   0.2f, 0.0f,//9
    0.0f, 0.0f, -0.25f,     0.0f, 0.0f, 1.0f,   0.0f, 0.2f,//11
                                        
    1.0f, -1.0f, -0.25f,    0.0f, 0.0f, 1.0f,   0.2f, 0.0f,//9
    1.0f, 0.0f, -0.25f,     0.0f, 0.0f, 1.0f,   0.2f, 0.2f,//10
    0.0f, 0.0f, -0.25f,     0.0f, 0.0f, 1.0f,   0.0f, 0.2f,//11

    0.0f, -1.0f, -0.5f,    -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,//13
    0.0f, -1.0f, -0.25f,   -1.0f, 0.0f, 0.0f,   0.2f, 0.0f,//8
    0.0f, 0.0f, -0.5f,     -1.0f, 0.0f, 0.0f,   0.0f, 0.2f,//5
                           
    0.0f, -1.0f, -0.25f,   -1.0f, 0.0f, 0.0f,   0.2f, 0.0f,//8
    0.0f, 0.0f, -0.25f,    -1.0f, 0.0f, 0.0f,   0.2f, 0.2f,//11
    0.0f, 0.0f, -0.5f,     -1.0f, 0.0f, 0.0f,   0.0f, 0.2f,//5

    1.0f, -1.0f, -0.5f,     0.0f, 0.0f,-1.0f,   0.0f, 0.0f,//12
    0.0f, -1.0f, -0.5f,     0.0f, 0.0f,-1.0f,   0.2f, 0.0f,//13
    1.0f, 0.0f, -0.5f,      0.0f, 0.0f,-1.0f,   0.0f, 0.2f,//4
                                       
    0.0f, -1.0f, -0.5f,     0.0f, 0.0f,-1.0f,   0.2f, 0.0f,//13
    0.0f, 0.0f, -0.5f,      0.0f, 0.0f,-1.0f,   0.2f, 0.2f,//5
    1.0f, 0.0f, -0.5f,      0.0f, 0.0f,-1.0f,   0.0f, 0.5f,//4

    1.0f, -1.0f, -0.25f,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,//9
    1.0f, -1.0f, -0.5f,     1.0f, 0.0f, 0.0f,   0.2f, 0.0f,//12
    1.0f, 0.0f, -0.25f,     1.0f, 0.0f, 0.0f,   0.0f, 0.2f,//10
                            
    1.0f, -1.0f, -0.5f,     1.0f, 0.0f, 0.0f,   0.2f, 0.0f,//12
    1.0f, 0.0f, -0.5f,      1.0f, 0.0f, 0.0f,   0.2f, 0.2f,//14
    1.0f, 0.0f, -0.25f,     1.0f, 0.0f, 0.0f,   0.0f, 0.2f,//10

    0.0f, -1.0f, -0.5f,     0.0f,-1.0f, 0.0f,   0.0f, 0.0f,//13
    1.0f, -1.0f, -0.5f,     0.0f,-1.0f, 0.0f,   0.2f, 0.0f,//12
    0.0f, -1.0f, -0.25f,    0.0f,-1.0f, 0.0f,   0.0f, 0.2f,//8

    1.0f, -1.0f, -0.5f,     0.0f,-1.0f, 0.0f,   0.2f, 0.0f,//12
    1.0f, -1.0f, -0.25f,    0.0f,-1.0f, 0.0f,   0.2f, 0.2f,//9
    0.0f, -1.0f, -0.25f,    0.0f,-1.0f, 0.0f,   0.0f, 0.2f //8
};
glm::mat4 BCscale = glm::scale(glm::vec3(0.7f, 0.7f, 0.7f));
glm::mat4 BCtranslate = glm::translate(glm::vec3(3.3f, 3.2f, 0.351f));
glm::mat4 batteryChargerModel = BCtranslate * BCscale;

//LIGHT OBJECTS
glm::vec3 gObjectColor(1.0f, 1.0f, 1.0f);
glm::vec3 gLightColor(1.0f, 1.0f, 1.0f);
glm::vec3 gLightPosition(3.0f, 2.5f, 5.0f);
glm::vec3 gLightScale(0.3f);
glm::vec3 keyLightColor(1.0f, 1.0f, 0.6f);
glm::vec3 keyLightPosition(3.0f, 8.0f, 3.0f);


//BALL OBJECT
vector<GLfloat> ballVerts;
vector<GLuint> ballIndices;
glm::mat4 ballScale = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
glm::mat4 ballTranslate = glm::translate(glm::vec3(5.3f, 1.2f, 0.5f));
glm::mat4 ballModel = ballTranslate * ballScale;


//BATTERY OBJECTS
glm::mat4 batteryScale = glm::scale(glm::vec3(0.08f, 0.08f, 0.7f));
glm::mat4 batteryRotate = glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
glm::mat4 batteryOneTranslate = glm::translate(glm::vec3(3.39f, 2.87f, 0.25f));
glm::mat4 batteryTwoTranslate = glm::translate(glm::vec3(3.56f, 2.87f, 0.25f));
glm::mat4 batteryThreeTranslate = glm::translate(glm::vec3(3.74f, 2.87f, 0.25f));
glm::mat4 batteryFourTranslate = glm::translate(glm::vec3(3.91f, 2.87f, 0.25f));
glm::mat4 batteryOneModel = batteryOneTranslate * batteryRotate * batteryScale;
glm::mat4 batteryTwoModel = batteryTwoTranslate * batteryRotate * batteryScale;
glm::mat4 batteryThreeModel = batteryThreeTranslate * batteryRotate * batteryScale;
glm::mat4 batteryFourModel = batteryFourTranslate * batteryRotate * batteryScale;


//CAN OBJECT
glm::mat4 canScale = glm::scale(glm::vec3(0.6f, 0.6f, 3.0f));
glm::mat4 canTranslate = glm::translate(glm::vec3(1.4f, 2.5f, 0.6f));
glm::mat4 canXRotate = glm::rotate(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
glm::mat4 canYRotate = glm::rotate(glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 canZRotate = glm::rotate(glm::radians(35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
glm::mat4 canModel = canTranslate * canZRotate * canYRotate * canXRotate * canScale;

//HAIRTIE OBJECT
glm::mat4 hairTieScale = glm::scale(glm::vec3(0.3f, 0.3f, 0.3f));
glm::mat4 hairTieTranslate = glm::translate(glm::vec3(5.5f, 3.5f, 0.1f));
glm::mat4 hairTieModel = hairTieTranslate * hairTieScale;



// The one and only int main()
int main(int argc, char* argv[])
{
    // Trying to initialize our OpenGL window. The function returns false when the window fails to initialize
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE;


    // Generating our cookie sheet mesh object
    cookieSheet.size = sizeof(cookieSheetVerts);
    cookieSheet.nVertices = sizeof(cookieSheetVerts) / (sizeof(cookieSheetVerts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));
    UCreateMesh(cookieSheet, cookieSheetVerts, blankIndices);

    // Generating the battery charger mesh object
    batteryCharger.size = sizeof(batteryChargerVerts);
    batteryCharger.nVertices = sizeof(batteryChargerVerts) / (sizeof(batteryChargerVerts[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));
    UCreateMesh(batteryCharger, batteryChargerVerts, blankIndices);

    // Generating our ball mesh object
    ballVerts = UGenerateSphereVerts(ball);
    ballIndices = UGenerateSphereIndices(ball);
    // Converting to arrays for OpenGL
    GLfloat ballVertArray[22568];
    GLuint ballIndArray[16020];
    std::copy(ballVerts.begin(), ballVerts.end(), ballVertArray);
    std::copy(ballIndices.begin(), ballIndices.end(), ballIndArray);
    ball.size = sizeof(ballVertArray);
    ball.indSize = sizeof(ballIndArray);
    ball.nVertices = sizeof(ballVertArray) / (sizeof(ballVertArray[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));
    ball.nIndices = 16020;
    UCreateMesh(ball, ballVertArray, ballIndArray);


    // Generating our Battery mesh object
    vector<GLfloat> batteryVerts = UGenerateCylinderVerts(battery);
    vector<GLuint> batteryIndices = UGenerateCylinderIndices(battery);
    GLfloat batteryVertArray[11552];
    GLuint batteryIndArray[4320];
    std::copy(batteryVerts.begin(), batteryVerts.end(), batteryVertArray);
    std::copy(batteryIndices.begin(), batteryIndices.end(), batteryIndArray);
    battery.size = sizeof(batteryVertArray);
    battery.indSize = sizeof(batteryIndArray);
    battery.nVertices = sizeof(batteryIndArray) / (sizeof(batteryIndArray[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));
    battery.nIndices = 4320;
    UCreateMesh(battery, batteryVertArray, batteryIndArray);

    //Generating our HairTie object
    vector<GLfloat> hairTieVerts = UGenerateHairTieVerts(hairTie);
    vector<GLuint> hairTieIndices = UGenerateHairTieIndices(hairTie);
    cout << hairTieIndices.size() << "  --  " << hairTieVerts.size() << endl;
    GLfloat hairTieVertArray[5776];
    GLuint hairTieIndicesArray[2166];
    std::copy(hairTieVerts.begin(), hairTieVerts.end(), hairTieVertArray);
    std::copy(hairTieIndices.begin(), hairTieIndices.end(), hairTieIndicesArray);
    hairTie.size = sizeof(hairTieVertArray);
    hairTie.indSize = sizeof(hairTieIndicesArray);
    hairTie.nVertices = sizeof(hairTieVertArray) / (sizeof(hairTieVertArray[0]) * (floatsPerVertex + floatsPerUV + floatsPerNormal));
    hairTie.nIndices = 2166;
    UCreateMesh(hairTie, hairTieVertArray, hairTieIndicesArray);

    // Here we create the shader program, and we exit if it returns false
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE;

    if (!UCreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource, gLightProgramId))
        return EXIT_FAILURE;

    //Here we will load our textures
    const char* cookieSheetFile = "cookie sheet.png";
    if (!UCreateTexture(cookieSheetFile, cookieSheetTexture))
    {
        cout << "Failed to load texture " << cookieSheetFile << endl;
        return EXIT_FAILURE;
    }

    const char* BCtextFile = "BatteryCharger.png";
    if (!UCreateTexture(BCtextFile, batteryChargerTexture))
    {
        cout << "Failed to load texture " << BCtextFile << endl;
        return EXIT_FAILURE;
    }

    const char* ballFile = "Ball.png";
    if (!UCreateTexture(ballFile, ballTexture))
    {
        cout << "Failed to load texture " << ballFile << endl;
        return EXIT_FAILURE;
    }

    const char* batteryFile = "Battery.png";
    if (!UCreateTexture(batteryFile, batteryTexture))
    {
        cout << "Failed to load texture " << batteryFile << endl;
        return EXIT_FAILURE;
    }

    const char* canFile = "can texture.png";
    if (!UCreateTexture(canFile, canTexture))
    {
        cout << "Failed to load texture " << canFile << endl;
        return EXIT_FAILURE;
    }
    const char* hairTieFile = "HairTie.png";
    if (!UCreateTexture(hairTieFile, hairTieTexture))
    {
        cout << "Failed to load texture " << hairTieFile << endl;
        return EXIT_FAILURE;
    }

    // Here we set the background to black (0 in each color an 1 in opacity)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // render loop, runs until the window is closed
    while (!glfwWindowShouldClose(gWindow))
    {

        //vars for timing frames
        float currentFrame = glfwGetTime();
        gDeltaTime = currentFrame - gLastFrame;
        gLastFrame = currentFrame;

        // Enable z-depth
        glEnable(GL_DEPTH_TEST);

        // Clear the frame and z buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // input (only checks for esc key to close window)
        UProcessInput(gWindow);

        // Set the shader to be used
        glUseProgram(gProgramId);

        //Render cookie sheet
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cookieSheetTexture);
        URenderTriangles(cookieSheetModel, cookieSheet, blankIndices);

        //Render Battery Charger
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, batteryChargerTexture);
        URenderTriangles(batteryChargerModel, batteryCharger, blankIndices);

        //Render Ball
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ballTexture);
        URenderTriangles(ballModel, ball, ballIndArray);

        //Render Batteries
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, batteryTexture);
        URenderTriangles(batteryOneModel, battery, batteryIndArray);
        URenderTriangles(batteryTwoModel, battery, batteryIndArray);
        URenderTriangles(batteryThreeModel, battery, batteryIndArray);
        URenderTriangles(batteryFourModel, battery, batteryIndArray);

        //Render Can (battery object is just a generic cylinder, so I'm just reusing it)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, canTexture);
        URenderTriangles(canModel, battery, batteryIndArray);

        //Render Hair Tie, the hair tie is just black so we'll just bound the only black texture we have
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hairTieTexture);
        URenderTriangles(hairTieModel, hairTie, hairTieIndicesArray);
        

        // Renders my light objects
        glUseProgram(gLightProgramId);
        renderLight();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(gWindow);    // Flips the the back buffer with the front buffer every frame.

        // Handles any events, such as closing the window when we hit escape
        glfwPollEvents();
    }

    // when the window is closed, we destroy our mesh VBOs and textures to clear the memory
    UDestroyMesh(cookieSheet);
    UDestroyTexture(cookieSheetTexture);
    UDestroyMesh(batteryCharger);
    UDestroyTexture(batteryChargerTexture);
    UDestroyMesh(ball);
    UDestroyTexture(ballTexture);
    UDestroyMesh(battery);
    UDestroyTexture(batteryTexture);
    UDestroyTexture(canTexture);
    UDestroyMesh(hairTie);
    UDestroyTexture(hairTieTexture);


    // and we do the same with our shader program
    UDestroyShaderProgram(gProgramId);
    UDestroyShaderProgram(gLightProgramId);

    exit(EXIT_SUCCESS);
}


// Initialize GLFW, GLEW, and create a window
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // Here we initialize the glfw library and give it the version range that should work
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    //Here we're telling GLFW which profile to create the context for
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //this checks if we're using a mac
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //Here we create the glfw window and pass it our predetermined sizes and title
    //the two nulls are the monitor address and other windows the glfw window should share resources with
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);

    //here we're checking if the openGL window object we're using actually exists
    if (*window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    //here we're setting the application window to the one we just created
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);

    //telling our window to watch for mouse inputs
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);
    glfwSetMouseButtonCallback(*window, UMouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //Here we're checking our version and initializing the GLEW library
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    //lastly we'll check that we're using the right version of GLEW and that it initialized correctly
    if (GLEW_OK != GlewInitResult)
    {
        std::cerr << glewGetErrorString(GlewInitResult) << std::endl;
        return false;
    }

    // Displays GPU OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    //if we got here everything ran great so we'll return true so the program can will continue
    return true;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void UProcessInput(GLFWwindow* window)
{
    //exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //handling movement input (camera class handles all but up and down)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gCamera.ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gCamera.ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gCamera.ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gCamera.ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        gCamera.Position += gCamera.Up * gCamera.MovementSpeed * gDeltaTime;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gCamera.Position -= gCamera.Up * gCamera.MovementSpeed * gDeltaTime;

    //changes the view type
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        orthoView = !orthoView;

}


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (gFirstMouse)
    {
        gLastX = xpos;
        gLastY = ypos;
        gFirstMouse = false;
    }

    float xoffset = xpos - gLastX;
    float yoffset = gLastY - ypos; // reversed since y-coordinates go from bottom to top

    gLastX = xpos;
    gLastY = ypos;

    gCamera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    //prevents the movement speed from becoming less than 1, cause reversed controls
    //TODO find a way to normalize y offset so scrolling the wheel always changes the speed within range
    if (gCamera.MovementSpeed < 1.0)
        return;
    gCamera.MovementSpeed += yoffset;

}

// glfw: handle mouse button events (right now, it just prints debug statements to the console)
// --------------------------------------------------------------------------------------------
void UMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT:
    {
        if (action == GLFW_PRESS)
            cout << "Left mouse button pressed" << endl;
        else
            cout << "Left mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_MIDDLE:
    {
        if (action == GLFW_PRESS)
            cout << "Middle mouse button pressed" << endl;
        else
            cout << "Middle mouse button released" << endl;
    }
    break;

    case GLFW_MOUSE_BUTTON_RIGHT:
    {
        if (action == GLFW_PRESS)
            cout << "Right mouse button pressed" << endl;
        else
            cout << "Right mouse button released" << endl;
    }
    break;

    default:
        cout << "Unhandled mouse button event" << endl;
        break;
    }
}

// Functioned called to render a frame                                                                            
void URenderTriangles(glm::mat4 model, GLMesh mesh, GLuint indices[])
{
    // We'll also show it where we have the triangles' meshes
    glBindVertexArray(mesh.vao);

    glm::mat4 view;
    glm::mat4 projection;

    //TODO fix the ortho view so it's more aesthetic
    if (orthoView) {
        // Transforms the camera to a uniform location
        view = glm::translate(glm::vec3(1.0f, 1.0f, -15.0f));

        // Creates a orthographic projection
        projection = glm::ortho(-1.0f, 10.0f, -2.0f, 10.0f, 0.1f, 100.0f);
    }

    else {
        // Gets our camera transforms from the camera object
        view = gCamera.GetViewMatrix();

        // Creates a perspective projection based on our camera
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }


    // Retrieves and passes transform matrices to the Shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Here we're referencing the shader program for the object color and the light color and position and the camera position
    GLint objectColorLoc = glGetUniformLocation(gProgramId, "objectColor");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "lightColor");
    GLint lightPositionLoc = glGetUniformLocation(gProgramId, "lightPos");
    GLint viewPositionLoc = glGetUniformLocation(gProgramId, "viewPosition");

    // Here we're passing that information to the shader program's uniforms
    glUniform3f(objectColorLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorLoc, gLightColor.r, gLightColor.g, gLightColor.b);
    glUniform3f(lightPositionLoc, gLightPosition.x, gLightPosition.y, gLightPosition.z);


    // Here we'll define our second light object
    GLint objectColorTwoLoc = glGetUniformLocation(gProgramId, "objectColorTwo");
    GLint lightColorTwoLoc = glGetUniformLocation(gProgramId, "lightColorTwo");
    GLint lightPositionTwoLoc = glGetUniformLocation(gProgramId, "lightPosTwo");
    GLint viewPositionTwoLoc = glGetUniformLocation(gProgramId, "viewPositionTwo");

    // Here we're passing that information to the shader program's uniforms
    glUniform3f(objectColorTwoLoc, gObjectColor.r, gObjectColor.g, gObjectColor.b);
    glUniform3f(lightColorTwoLoc, keyLightColor.r, keyLightColor.g, keyLightColor.b);
    glUniform3f(lightPositionTwoLoc, keyLightPosition.x, keyLightPosition.y, keyLightPosition.z);

    const glm::vec3 cameraPosition = gCamera.Position;
    glUniform3f(viewPositionLoc, cameraPosition.x, cameraPosition.y, cameraPosition.z);


    GLint UVScaleLoc = glGetUniformLocation(gProgramId, "uvScale");
    glUniform2fv(UVScaleLoc, 1, glm::value_ptr(gUVScale));

    // This function will take the information we gave it and draw it by the vertex information stored in the mesh
    if (mesh.nIndices == 0){
        glDrawArrays(GL_TRIANGLES, 0, mesh.nVertices);
    }
    else {
        glDrawElements(GL_TRIANGLES, mesh.nIndices, GL_UNSIGNED_INT, (void*)0);
    }
    

    // Deactivate the VAO
    glBindVertexArray(0);
}

void renderLight()
{
    glBindVertexArray(cookieSheet.vao);
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 model = glm::translate(gLightPosition) * glm::scale(gLightScale);

    if (orthoView) {
        // Transforms the camera to a uniform location
        view = glm::translate(glm::vec3(1.0f, 1.0f, -15.0f));

        // Creates a orthographic projection
        projection = glm::ortho(-1.0f, 10.0f, -2.0f, 10.0f, 0.1f, 100.0f);
    }

    else {
        // Gets our camera transforms from the camera object
        view = gCamera.GetViewMatrix();

        // Creates a perspective projection based on our camera
        projection = glm::perspective(glm::radians(gCamera.Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }

    // Reference matrix uniforms from the Lamp Shader program
    GLuint modelLoc = glGetUniformLocation(gLightProgramId, "model");
    GLuint viewLoc = glGetUniformLocation(gLightProgramId, "view");
    GLuint projLoc = glGetUniformLocation(gLightProgramId, "projection");

    // Pass matrix data to the Lamp Shader program's matrix uniforms
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glDrawArrays(GL_TRIANGLES, 0, cookieSheet.nVertices);

    //Changing the model matrix and drawing the accent light
    model = glm::translate(keyLightPosition) * glm::scale(gLightScale);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, cookieSheet.nVertices);
    //unbind our program and vao

    glUseProgram(0);
    glBindVertexArray(0);

}


// Implements the UCreateMesh function
void UCreateMesh(GLMesh& mesh, GLfloat verts[], GLuint indices[])
{
    //here we'll give the mesh the vertex array we created above and activate it
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Generating our VBO
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
    glBufferData(GL_ARRAY_BUFFER, mesh.size, verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

    // Generating an IBO 
    glGenBuffers(1, &mesh.ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indSize, indices, GL_STATIC_DRAW);


    //The Stride is the number of floats that belong to each vertex. This is how OpenGL segregates the data for each vertex
    // Strides between vertex coordinates is 6 (x, y, r, g, b, a).
     GLint stride = sizeof(float) * (floatsPerVertex + floatsPerUV + floatsPerNormal);

    // Creates the Vertex Attribute Pointer
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);
}

/* Function for generating a sphere's coordinate array using the lat-long method
   This function generates a vector of GLfloats in the standard format for this program (x, y, z, nx, ny, nz, tx, ty), 3 positions, 3 normals and 2 texture coordinates
   This function and the following 3 functions were adapted from : http://www.songho.ca/opengl/index.html, big thanks to them for their very helpful tutorial
 */
vector<GLfloat> UGenerateSphereVerts(GLMesh mesh) {
    vector<GLfloat> verts;
    GLfloat radius = 1.0f;
    GLfloat x, y, z, xy;                    // vars for mesh coords
    GLfloat nx, ny, nz, lengthInv = 1.0f;   // vars for normals coords
    GLfloat tx, ty;                         // vars for texture coords

    GLfloat sectorCount = 30, stackCount = 90; // sets the number of lat and long lines, essentially
    GLfloat sectorStep = 2 * pi / sectorCount; // Step vars track the angle to the center point
    GLfloat stackStep = pi / stackCount;
    GLfloat sectorAngle, stackAngle;           // For storing angles

    for (int i = 0; i <= stackCount; ++i) {
        // We'll start at pi/2 and generate to -pi/2
        stackAngle = pi / 2 - i * stackStep;
        xy = radius * cosf(stackAngle);
        z = radius * sinf(stackAngle);

        //Here we'll add another vertex per stack 
        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;

            //here we'll calculate the position of each x and y coord
            x = xy * cosf(sectorAngle);
            y = xy * sinf(sectorAngle);
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);

            //Generating our normals
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            verts.push_back(nx);
            verts.push_back(ny);
            verts.push_back(nz);

            //Generating our texture coordintates
            tx = (float)j / sectorCount;
            ty = (float)i / stackCount;
            verts.push_back(tx);
            verts.push_back(ty);
        }

    }
    return verts;
}

/* Generates a list of indices based on the vertices generated for a sphere using lat - long method
* To modularize this, you would need to add parameters for stack and sector count, allowing you to update the resolution of the sphere
*/
vector<GLuint> UGenerateSphereIndices(GLMesh mesh) {
    vector<GLuint> indices;
    GLuint k1, k2, sectorCount = 30, stackCount = 90; // placeholder vars
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1); //Start of the stack
        k2 = k1 + sectorCount + 1; // Start of the next stack
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            //We need 2 triangles per rectangle on the sphere's surface
            //But the first and last stack are made of triangles, so we'll skip them
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

    }
    return indices;
}

// Like the UGenerateSphereVerts function, this function generates a vector of vertices in the standard format for this program (x, y, z, nx, ny, nz, tx, ty), 3 positions, 3 normals and 2 texture coordinates
vector<GLfloat> UGenerateCylinderVerts(GLMesh &mesh) {
    vector<GLfloat> verts, unitCircle;
    GLfloat sectorCount = 360;
    GLfloat sectorStep = 2 * pi / sectorCount;
    GLfloat sectorAngle;
    GLfloat ux, uy, uz;     //Unit circle coordinates
    GLfloat height = 1.0f, radius = 1.0f;
    
    //We'll calcualte the unit cirlce coordinates first, so we don't have to redo it for each sector
    for (int i = 0; i <= sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;
        unitCircle.push_back(cos(sectorAngle)); // x
        unitCircle.push_back(sin(sectorAngle)); // y
        unitCircle.push_back(0);                // z
    }
    //Next we'll translate the unit circle coordinates to each sector
    for (int i = 0; i < 2; ++i)
    {
        float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
        float t = 1.0f - i;                              // vertical tex coord; 1 to 0

        for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
        {
            float ux = unitCircle[k];
            float uy = unitCircle[k + 1];
            float uz = unitCircle[k + 2];
            // position vector
            verts.push_back(ux * radius);             // vx
            verts.push_back(uy * radius);             // vy
            verts.push_back(h);                       // vz
            // normal vector
            verts.push_back(ux);                       // nx
            verts.push_back(uy);                       // ny
            verts.push_back(uz);                       // nz
            // texture coordinate
            verts.push_back((float)j / sectorCount); // s
            verts.push_back(t);                      // t
        }
    }

    //we're gonna store these away later for generating our indices
    baseCenterIndex = (int)verts.size() / 8;
    topCenterIndex = baseCenterIndex + sectorCount + 1;

    // put base and top vertices to arrays
    for (int i = 0; i < 2; ++i)
    {
        float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
        float nz = -1 + i * 2;                           // z value of normal; -1 to 1

        // center point
        verts.push_back(0);      verts.push_back(0);     verts.push_back(h);
        verts.push_back(0);      verts.push_back(0);      verts.push_back(nz);
        verts.push_back(0.5f);   verts.push_back(0.5f);

        for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
        {
            float ux = unitCircle[k];
            float uy = unitCircle[k + 1];
            // position vertices
            verts.push_back(ux * radius);             // vx
            verts.push_back(uy * radius);             // vy
            verts.push_back(h);                       // vz
            // normal vertices
            verts.push_back(0);                        // nx
            verts.push_back(0);                        // ny
            verts.push_back(nz);                       // nz
            // texture coordinates
            verts.push_back(0.0f);      // s
            verts.push_back(0.0f);      // t
        }
    }

    return verts;
}

//This function generates a list of indices for our draw elements function
vector<GLuint> UGenerateCylinderIndices(GLMesh mesh) {
    vector<GLuint> indices;
    GLfloat sectorCount = 360;

    GLuint k1 = 0;                         // 1st vertex index at base
    GLuint k2 = sectorCount + 1;           // 1st vertex index at top

    // Start by adding the indices for each side, each side needs two triangles
    for (int i = 0; i < sectorCount; ++i, ++k1, ++k2)
    {
        indices.push_back(k1);
        indices.push_back(k1 + 1);
        indices.push_back(k2);

        
        indices.push_back(k2);
        indices.push_back(k1 + 1);
        indices.push_back(k2 + 1);
    }

    // Next we'll add the base triangles
     for (GLuint i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < sectorCount - 1)
        {
            indices.push_back(baseCenterIndex);
            indices.push_back(k + 1);
            indices.push_back(k);
        }
        else // last triangle
        {
            indices.push_back(baseCenterIndex);
            indices.push_back(baseCenterIndex + 1);
            indices.push_back(k);
        }
    }

    // Lastly the top triangles
    for (GLuint i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
    {
        if (i < sectorCount - 1)
        {
            indices.push_back(topCenterIndex);
            indices.push_back(k);
            indices.push_back(k + 1);
        }
        else // last triangle
        {
            indices.push_back(topCenterIndex);
            indices.push_back(k);
            indices.push_back(topCenterIndex + 1);
        }
    }

    return indices;
}

vector<GLfloat> UGenerateHairTieVerts(GLMesh mesh) {
    vector<GLfloat> verts;
    GLfloat sectorCount = 360;
    GLfloat sectorStep = 2 * pi / sectorCount;
    GLfloat sectorAngle;
    GLfloat outerRadius = 1.0f, innerRadius = 0.9f;

    for (int i = 0; i <= sectorCount; ++i) {
        sectorAngle = i * sectorStep;
        //inner circle
        //Vertex info
        verts.push_back((innerRadius * cos(sectorAngle))); // x
        verts.push_back((innerRadius * sin(sectorAngle))); // y
        verts.push_back(0.0);                            // z
        //Normals
        verts.push_back(0.0);
        verts.push_back(0.0);
        verts.push_back(1.0);
        //Texuture
        verts.push_back(0.0);
        verts.push_back(0.0);
        //Outer Circle
        verts.push_back(outerRadius * cos(sectorAngle)); // x
        verts.push_back(outerRadius * sin(sectorAngle)); // y
        verts.push_back(0.0);                            // z
        //Normals
        verts.push_back(0.0);
        verts.push_back(0.0);
        verts.push_back(1.0);
        //Texuture
        verts.push_back(0.0);
        verts.push_back(0.0);
    }

    return verts;
}

vector<GLuint> UGenerateHairTieIndices(GLMesh mesh) {
    vector<GLuint> indices;
    int sectorCount = 360;
    //adds two triangles starting at each inner circle point
    for (int i = 0; i <= sectorCount*2; i += 2) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 1);
        indices.push_back(i + 3);
        indices.push_back(i + 2);
    }
    return indices;
}

//used to clear memory when window is closed
void UDestroyMesh(GLMesh& mesh)
{
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(2, &mesh.vbo);
}

//Creates a texture object from an image using the STBI library
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (channels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (channels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else
        {
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(image);
        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

        return true;
    }

    // Error loading the image
    return false;
}

//Destroys the texture upon window close
void UDestroyTexture(GLuint textureId) {
    glGenTextures(1, &textureId);
}


// Implements the UCreateShaders function
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // used to track whether the the shader program and objects were compiled correctly
    int success = 0;

    //log for storing error messaging
    char infoLog[512];

    // Creates the shader program and assigns it to our global variable (since it's & in the parameter)
    programId = glCreateProgram();

    // Creates the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Retrive the shader source
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compile the vertex shader, and print compilation errors (if any)
    glCompileShader(vertexShaderId); // compile the vertex shader
    // check for shader compile errors
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

    // if we're given an error code, we retrieve and print the error log
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // compile the fragment shader
    glCompileShader(fragmentShaderId);

    // check for shader compile errors
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    // if we're given an error code, we retrieve and print the error log
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Here we attach the shader objects to the shaderprogram object
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Then we link the shader program
    glLinkProgram(programId);

    // check for linking errors
    glGetProgramiv(programId, GL_LINK_STATUS, &success);

    // if we're given an error code, we retrieve and print the error log
    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;

        return false;
    }

    // Then we tell OpenGL to use the object we just finished creating
    glUseProgram(programId);

    // And we pass true, because the shader object was created correctly
    return true;
}


// used to clear memory when the window is closed
void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}
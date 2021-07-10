#include <glad/glad.h>
#include <GLFW/glfw3.h>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "../Tools/shader_m.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

#include "../GLCamera/GLCamera.h"
//using namespace viewer;
//using namespace viewer::geometry;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// testCamera

viewer::Viewer camera;


// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

bool firstMouse = true;
//float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.

float yaw   = 0.0f;
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

typedef std::numeric_limits<float> dfl;
void Printmat4(const glm::mat4& m)
{
    std::cout.precision(dfl::max_digits10);
    std::cout << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n";
    std::cout << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n";
    std::cout << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n";
    std::cout << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << "\n";
}

int main()
{

    std::string texture1_path = "../Camera/assets/container.jpg";
    std::string texture2_path = "../Camera/assets/awesomeface.png";
    std::string vshader_path = "../Camera/src/Shaders/vertex.vs";
    std::string fshader_path = "../Camera/src/Shaders/fragment.fs";

    camera.LookAt(cameraPos, cameraFront, cameraUp);

    // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        // glfw window creation
        // --------------------
        GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
//        // tell GLFW to capture our mouse and hide cursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return -1;
        }

        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);


        // build and compile our shader zprogram
        // ------------------------------------
        Shader ourShader(vshader_path.c_str(), fshader_path.c_str());

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        // set up vertex data (and buffer(s)) and configure vertex attributes
           // ------------------------------------------------------------------
       float vertices[] = {
           -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
           -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

           -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
           -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

           -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
           -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
           -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
           -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
           -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
           -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

           -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
           -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
           -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

           -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
           -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
           -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
       };
       // world space positions of our cubes
        glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };



        // set up VBO, VAO and concerned attributes
        // ------------------------------------------------------------------
        unsigned int VBO, VAO;
        // VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        //  v1,     v2,     v3
        // xyzst, xyzst, xyzst,
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);


        // load and create a texture
      // -------------------------
      unsigned int texture1, texture2;
      // texture 1
      // ---------
      glGenTextures(1, &texture1);
      glBindTexture(GL_TEXTURE_2D, texture1);
      // set the texture wrapping parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      // set texture filtering parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // load image, create texture and generate mipmaps
      int width, height, nrChannels;
      stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
      unsigned char *data = stbi_load(texture1_path.c_str(), &width, &height, &nrChannels, 0);

      if (data)
      {
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
          glGenerateMipmap(GL_TEXTURE_2D);
      }
      else
      {
          std::cout << "Failed to load texture" << std::endl;
      }
      stbi_image_free(data);
      // texture 2
      // ---------
      glGenTextures(1, &texture2);
      glBindTexture(GL_TEXTURE_2D, texture2);
      // set the texture wrapping parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      // set texture filtering parameters
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // load image, create texture and generate mipmaps
      data = stbi_load(texture2_path.c_str(), &width, &height, &nrChannels, 0);
      if (data)
      {
          // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
          glGenerateMipmap(GL_TEXTURE_2D);
      }
      else
      {
          std::cout << "Failed to load texture" << std::endl;
      }
      stbi_image_free(data);

      // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
       // -------------------------------------------------------------------------------------------
       ourShader.use();
       ourShader.setInt("texture1", 0);
       ourShader.setInt("texture2", 1);






       // render loop
       // -----------
       while (!glfwWindowShouldClose(window))
       {
           // per-frame time logic
           // --------------------
           float currentFrame = glfwGetTime();
           deltaTime = currentFrame - lastFrame;
           lastFrame = currentFrame;

           // input
           // -----
           processInput(window);

           // render
           // ------

           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // perform by state

           // bind textures on corresponding texture units
           glActiveTexture(GL_TEXTURE0);
           glBindTexture(GL_TEXTURE_2D, texture1);
           glActiveTexture(GL_TEXTURE1);
           glBindTexture(GL_TEXTURE_2D, texture2);

           // activate shader
           ourShader.use();

           // pass projection matrix to shader (as projection matrix rarely changes there's no need to do this per frame)
           // -----------------------------------------------------------------------------------------------------------
           camera.Perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
           glm::mat4 projection = camera.GetProjectionMatrix();

           ourShader.setMat4("projection", projection);

           // camera/view transformation
//           glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
           glm::mat4 view = camera.GetViewMatirx();
           ourShader.setMat4("view", view);


           // render container
           glBindVertexArray(VAO);
           for(unsigned int i = 0; i < 10; i++)
           {
               // calculate the model matrix for each object and pass it to shader before drawing
               glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
               model = glm::translate(model, cubePositions[i]);
               float angle = 20.0f * i;
               model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
               ourShader.setMat4("model", model);

               glDrawArrays(GL_TRIANGLES, 0, 36);
           }

           // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
           // -------------------------------------------------------------------------------
           glfwSwapBuffers(window);
           glfwPollEvents();
       }

       // optional: de-allocate all resources once they've outlived their purpose:
       // ------------------------------------------------------------------------
       glDeleteVertexArrays(1, &VAO);
       glDeleteBuffers(1, &VBO);


       // glfw: terminate, clearing all previously allocated GLFW resources.
       // ------------------------------------------------------------------
       glfwTerminate();
       return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        cameraPos += cameraSpeed * cameraFront;
        camera.ShiftForward(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        cameraPos -= cameraSpeed * cameraFront;
        camera.ShiftBackward(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        camera.ShiftLeft(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        camera.ShiftRight(cameraSpeed);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ShiftUp(cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ShiftDown(cameraSpeed);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

//    yaw += xoffset;
//    pitch += yoffset;

    yaw = xoffset;
    pitch = yoffset;


    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

//    glm::vec3 front;
//    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//    front.y = sin(glm::radians(pitch));
//    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//    cameraFront = glm::normalize(front);

    camera.Pitch(glm::radians(pitch));
    camera.Yaw(glm::radians(yaw));
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

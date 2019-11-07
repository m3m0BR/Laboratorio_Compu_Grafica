/*---------------------------------------------------------*/
/* ----------------   Práctica 8 --------------------------*/
/*-----------------    2020-1   ---------------------------*/
/*------------- Alumno:---------------*/
#define STB_IMAGE_IMPLEMENTATION
#include "esfera.h"
#include "camera.h"

Esfera my_sphere(1.0f);

void resize(GLFWwindow* window, int width, int height);
void my_input(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
// Window size
int SCR_WIDTH = 3800;
int SCR_HEIGHT = 7600;

GLFWmonitor *monitors;
GLuint VBO, VAO, EBO;

//Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
double	lastX = 0.0f,
lastY = 0.0f;
bool firstMouse = true;

//Timing
double	deltaTime = 0.0f,
lastFrame = 0.0f;

//Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

void myData(void);
void display(Shader);
void getResolution(void);
void animate(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool);

//For Keyboard
float	movX = 0.0f,
movY = 0.0f,
movZ = -5.0f,
rotX = 0.0f,
rotY = 0.0f,
rotZ = 0.0f,
i = 0;

//Texture
unsigned int	t_pared_A,
t_pared_B,
t_pared_C,
t_pared_AlterC,
t_pilar,
t_piso,
t_techo,
t_aire,
t_pizarron,
t_cartel,
t_cartel2,
t_sismo,
t_placa,
t_lampara,
t_puerta;

//For model
bool animacion = false;
bool avanza = true;
bool abre = true;
float mov_puerta = 5.2;


unsigned int generateTextures(const char* filename, bool alfa)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.

	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;

	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;

}

void LoadTextures()
{
	t_pared_A = generateTextures("Texturas/Textura_Pared.png", 1);
	t_pared_B = generateTextures("Texturas/Textura_pared_B.png", 1);
	t_pared_C = generateTextures("Texturas/Cortina.png", 1);
	t_pared_AlterC = generateTextures("Texturas/Cortina.jpg", 0);
	t_pilar = generateTextures("Texturas/pilar.jpg", 0);
	t_piso = generateTextures("Texturas/Piso.jpg", 0);
	t_techo = generateTextures("Texturas/techo.jpg", 0);
	t_aire = generateTextures("Texturas/Salida_aire.png", 1);
	t_pizarron = generateTextures("Texturas/pizarron.png", 1);
	t_cartel = generateTextures("Texturas/No_cartel.png", 1);
	t_cartel2 = generateTextures("Texturas/No_cartel2.png", 1);
	t_sismo = generateTextures("Texturas/Sismo_cartel.png", 1);
	t_placa = generateTextures("Texturas/placa_metal_piso.jpg", 0);
	t_lampara = generateTextures("Texturas/Lamparas.png", 1);
	t_puerta = generateTextures("Texturas/Puerta.png", 1);
}

void myData()
{
	float vertices[] = {
		//Cuadrado utilizado para realizar todos los modelos del escenario 
		// positions          // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 1.0f, //0
		 1.0f,  0.0f, 0.0f,   1.0f, 0.0f, //1
		 0.0f,  0.0f, 0.0f,   0.0f, 0.0f, //2
		 0.0f,  1.0f, 0.0f,   0.0f, 1.0f, //3

	};
	unsigned int indices[] = {
		//Frontal
		0, 1, 3, // first triangle
		1, 2, 3,  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

}

void animate(void)
{
	if (animacion) {
		if (avanza) {
			if (abre) {
				mov_puerta -= 0.01f;
				if (mov_puerta <= 4.44f) {
					avanza = false;
					animacion = false;
					abre = false;
				}
			}
			else
			{
			mov_puerta += 0.01f;
			if (mov_puerta >= 5.2f) {
				avanza = false;
				animacion = false;
				abre = true;
			}
			}
		}
	}
}

void display(Shader shader)
{
	//Enable Shader
	shader.use();
	// create transformations and Projection
	glm::mat4 temp = glm::mat4(1.0f);
	glm::mat4 temp2 = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 view = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection
	
	//Use "projection" to include Camera
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	view = camera.GetViewMatrix();
	
	// pass them to the shaders
	shader.setVec3("viewPos", camera.Position);
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	shader.setMat4("projection", projection);


	glBindVertexArray(VAO);
	//Colocar código aquí
	//Pared del frente del salón
	for (i=0; i < 100; i = i + 14.0f) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(14.0f, 20.0f, 0.0f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
		glBindTexture(GL_TEXTURE_2D, t_pared_B);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	//Pared trasera del salón
	for (i = 0; i < 100; i = i + 14.0f) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.0f, 120.0f));
		model = glm::scale(model, glm::vec3(14.0f, 20.0f, 0.0f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_pared_B);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	//Pared izquierda del salón
	for (i = 0; i <= 4.375; i = i + 0.625) {
		model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(i+0.2f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.625f, 1.0f, 0.0f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_pared_A);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	//Puerta
	model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(mov_puerta, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.8f, 1.0f, 0.2f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_puerta);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Pilar
	model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 1.0f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_pilar);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Pared derecha del salón
	for (i = 0; i <= 5.6f; i = i + 0.625) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.2f+i));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.625, 1.0f, 0.0f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_pared_C);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	//Pilar derecho frontal
	model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 1.0f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_pilar);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//Pilar derecho trasero
	model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 5.8f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 1.0f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_pilar);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//Pilar izquierdo frontal
	model = glm::translate(glm::mat4(1.0f), glm::vec3(4.99f, 0.0f, 2.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 1.0f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_pilar);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Piso
	//model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	for (i = 0; i < 5.8; i = i + 0.2) {
		for (float j = 0; j < 5; j++) {
			model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(j, i, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 0.2f, 0.0f));
			shader.setMat4("model", model);
			shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, t_piso);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	//Techo
	for (float i = 0; i < 6; i = i + 0.5) {
		for (float j = 0; j < 5; j++) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(j, 1.0f, i));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 0.5f, 0.0f));
			shader.setMat4("model", model);
			shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, t_techo);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}
	//Salida de Aire
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.99f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_aire);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.99f, 0.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_aire);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.99f, 3.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_aire);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Pizarron
	model = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.01f));
	model = glm::scale(model, glm::vec3(2.0f, 1.0f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
	glBindTexture(GL_TEXTURE_2D, t_pizarron);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Letreros
	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.03f, 0.8f, 0.01f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
	glBindTexture(GL_TEXTURE_2D, t_cartel);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.75f, 0.76f, 0.01f));
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
	glBindTexture(GL_TEXTURE_2D, t_cartel2);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.714285714f, 0.3f, 0.01f));
	model = glm::scale(model, glm::vec3(0.3f, 0.2f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
	glBindTexture(GL_TEXTURE_2D, t_sismo);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Placa de metal entrada al salón
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.01f, 5.2f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.2f, 0.8f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", glm::vec3(1.0f, 1.0f, 1.0f));
	glBindTexture(GL_TEXTURE_2D, t_placa);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Lamparas
	for (float k = 1.0f; k <= 2.5f; k =  k + 1.25f) {
		for (i = 0.0f; i < 5; i = i + 2.0f) {
			model = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.99f, k));
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.0f));
			shader.setMat4("model", model);
			shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
			glBindTexture(GL_TEXTURE_2D, t_lampara);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	for (i = 0.0f; i < 5; i = i + 2.0f) {
		model = glm::translate(glm::mat4(1.0f), glm::vec3(i, 0.99f, 4.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.0f));
		shader.setMat4("model", model);
		shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
		glBindTexture(GL_TEXTURE_2D, t_lampara);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.99f, 1.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_lampara);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.99f, 1.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_lampara);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.99f, 1.0f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 0.3f, 0.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, t_lampara);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Estructuras", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 0, 30);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, resize);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//To Enable capture of our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glewInit();


	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	my_sphere.init();
	glEnable(GL_DEPTH_TEST);

	Shader projectionShader("shaders/shader_texture_color.vs", "shaders/shader_texture_color.fs");

	// render loop
	// While the windows is not closed
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		my_input(window);
		animate();

		// render
		// Backgound color
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Mi función de dibujo
		display(projectionShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		animacion = true;
		avanza = true;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void resize(GLFWwindow* window, int width, int height)
{
	// Set the Viewport to the size of the created window
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

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

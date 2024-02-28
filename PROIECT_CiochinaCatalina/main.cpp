#if defined (__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <iostream>
#include <windows.h>
#include <time.h>
#include <ctime>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"
#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "SkyBox.hpp"

gps::Window fereastraApp;

const int FERE_LATIME = 800;
const int FERE_INALTIME = 500;

const unsigned int UMBRAW = 8192;
const unsigned int UMBRAH = 8192;

GLint ziNoapte = 1; // stabilire zi pe 1 sau noapte pe 0

gps::Camera cameraUtilizator(glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
float vitezaCamera = 0.7f;

gps::Shader shaderPrincipal;
gps::Shader shaderLuminaDirectionala;
gps::Shader shaderPatratUmbra;
gps::Shader shaderSkyboxuri;
gps::Shader shaderUmbre;

gps::SkyBox skyboxSatOcean;

gps::Model3D scenaSatParasitLaOcean;
gps::Model3D patratUmbra;
gps::Model3D cubLumina;
gps::Model3D broasca;
gps::Model3D nava;

glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;
glm::mat4 lightModel;

glm::vec3 dirLumina;
glm::vec3 culoareLumina;
glm::vec3 culoareLuminaFelinare;

GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint dirLuminaLoc;
GLuint ziNoapteLoc;
GLuint culoareLuminaLoc;
GLuint culoareLuminaFelinareLoc;
GLuint shadowMapFBO;
GLuint depthMapTexture;
GLuint intensitateCeataLoc;
GLuint constantLoc;
GLuint linearLoc;
GLuint quadraticLoc;
GLuint coordonateFelinareLoc;

GLfloat constant = 1.0f;
GLfloat linear = 0.14f;
GLfloat quadratic = 0.07f;
GLfloat intensitateCeata = 0.0f;

glm::vec3 coordonateFelinare[4] = {
						  glm::vec3(59.217f, 21.0f, 21.368f),
						  glm::vec3(133.2f, 21.0f, -129.37f),
						  glm::vec3(97.576f, 21.0f, 129.78f),
						  glm::vec3(141.62f, 21.0f, -29.433f)
};

bool showDepthMap = false;
bool firstMouse = true;

bool taste[1024];

float unghiLumina = 0.0f;
float unghiVedere = 45.0f;
float unghiRotatieScena = 0.0f;

float lastX = 400, lastY = 300;
double yaw = -90.0f, pitch;

glm::vec3 broascaPozitie = glm::vec3(-60.387f, -0.5f, -40.105f);
float unghiBrasca = 0.0;
bool directieBroasca = false; //incepe din cea initiala si se modifica la fiecare parcurgere cu negarea

glm::vec3 navaPozitie = glm::vec3(-250.0f, 3.0f, -127.0f);
float unghiNava = 180.0;
bool directieNava = false; //incepe din cea initiala si se modifica la fiecare parcurgere cu negarea

bool animatieComutare = true;
int numarAnimatie = 1; //numarul care tine care animatie este in desfasurare
float unghiRotatieAnimatie = 0.0f;
double timpInceputAnimatie = glfwGetTime(); // nr sec de inceput program

GLenum glCheckError_(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {

		glfwSetWindowShouldClose(window, GLFW_TRUE); //iesire din aplicatie
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap; //harta de adancime a umbrelor

	if (key >= 0 && key < 1024) {

		if (action == GLFW_PRESS) {
			taste[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			taste[key] = false;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (!animatieComutare) {
		shaderPrincipal.utilizeazaShader();
		if (firstMouse)
		{
			lastX = xpos; //poz cursor x
			lastY = ypos; // poz cursor y
			firstMouse = false; //salvez ca nu mai sunt la prima miscare a mouse-ului
		}

		float xoffset = xpos - lastX; // calcul deplasare mouse pe x
		float yoffset = lastY - ypos; // calcul deplasare mouse pe y
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;//depl pe yaw cu offset
		pitch += yoffset;//depl pe pitch cu offset


		if (pitch > 89.0f)
			pitch = 89.0f;
		else if (pitch < -89.0f)
			pitch = -89.0f;
		else
			pitch = pitch;

		cameraUtilizator.rotesteCamera(pitch, yaw);//rotire in fct de pitch si yaw
		view = cameraUtilizator.calculeazaMatriceVizualizare();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)); //trimite catre shader matr view
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	unghiVedere -= (float)yoffset;
	if (unghiVedere < 1.0f)
		unghiVedere = 1.0f;
	if (unghiVedere > 45.0f)
		unghiVedere = 45.0f;
}

void initOpenGLWindow() {

	fereastraApp.Create(FERE_LATIME, FERE_INALTIME, "Sat parasit la ocean");

	glfwSetWindowSizeCallback(fereastraApp.getWindow(), windowResizeCallback);
	glfwSetKeyCallback(fereastraApp.getWindow(), keyboardCallback);
	glfwSetCursorPosCallback(fereastraApp.getWindow(), mouseCallback);
	glfwSetInputMode(fereastraApp.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(fereastraApp.getWindow(), scroll_callback);
}


void initializareSkybox(const std::string& perioadaZiNoapte) {
	std::vector<const GLchar*> faces;

	if (perioadaZiNoapte == "noapte")
	{
		//skybox de noapte
		faces.push_back("skybox/noapte_right.tga");
		faces.push_back("skybox/noapte_left.tga");
		faces.push_back("skybox/noapte_top.tga");
		faces.push_back("skybox/noapte_bottom.tga");
		faces.push_back("skybox/noapte_back.tga");
		faces.push_back("skybox/noapte_front.tga");
	}
	else
	{
		//skybox de zi
		faces.push_back("skybox/zi_right.tga");
		faces.push_back("skybox/zi_left.tga");
		faces.push_back("skybox/zi_top.tga");
		faces.push_back("skybox/zi_bottom.tga");
		faces.push_back("skybox/zi_back.tga");
		faces.push_back("skybox/zi_front.tga");
	}
	skyboxSatOcean.Load(faces);
}

void controleazaTaste()
{
	if (taste[GLFW_KEY_Q])
	{
		//roteste dreapta scena
		animatieComutare = false;
		unghiRotatieScena -= 1.0f;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(unghiRotatieScena), glm::vec3(0, 1, 0));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (taste[GLFW_KEY_E]) 
	{
		//roteste stanga scena
		animatieComutare = false;
		unghiRotatieScena += 1.0f;
		model = glm::rotate(glm::mat4(1.0f), glm::radians(unghiRotatieScena), glm::vec3(0, 1, 0));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (taste[GLFW_KEY_W])
	{
		animatieComutare = false;
		gps::Camera cameraAuxUtilizator(
			cameraUtilizator.getPozitieCamera(),
			cameraUtilizator.getVectorTarget(),
			cameraUtilizator.getVectorSus());
		cameraAuxUtilizator.miscaCamera(gps::INAINTE, vitezaCamera);
		//daca user ul ajunge in x < -130 inseamna ca vrea sa intre in apa si nu ii este permis
	    if (cameraAuxUtilizator.getPozitieCamera().x > -130)
		{			
			//altfel merge pe dir inainte
			cameraUtilizator.miscaCamera(gps::INAINTE, vitezaCamera);
			view = cameraUtilizator.calculeazaMatriceVizualizare();
			shaderPrincipal.utilizeazaShader();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		}
	}

	if (taste[GLFW_KEY_S])
	{
		//merge pe dir inapoi
		animatieComutare = false;
		cameraUtilizator.miscaCamera(gps::INAPOI, vitezaCamera);
		view = cameraUtilizator.calculeazaMatriceVizualizare();
		shaderPrincipal.utilizeazaShader();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (taste[GLFW_KEY_A]) 
	{
		//merge la stanga
		animatieComutare = false;
		cameraUtilizator.miscaCamera(gps::STANGA, vitezaCamera);
		view = cameraUtilizator.calculeazaMatriceVizualizare();
		shaderPrincipal.utilizeazaShader();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (taste[GLFW_KEY_D])
	{
		//merge la dreapta
		animatieComutare = false;
		cameraUtilizator.miscaCamera(gps::DREAPTA, vitezaCamera);
		view = cameraUtilizator.calculeazaMatriceVizualizare();
		shaderPrincipal.utilizeazaShader();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (taste[GLFW_KEY_F]) 
	{
		//incrementeaza coeficient ceata
		intensitateCeata += 0.001f;
		if (intensitateCeata >= 0.3f)
			intensitateCeata = 0.3f;
		shaderPrincipal.utilizeazaShader();
		glUniform1fv(intensitateCeataLoc, 1, &intensitateCeata);
	}

	if (taste[GLFW_KEY_G]) 
	{
		//decrementeaza coeficient ceata
		intensitateCeata -= 0.001f;
		if (intensitateCeata <= 0.0f)
			intensitateCeata = 0.0f;
		shaderPrincipal.utilizeazaShader();
		glUniform1fv(intensitateCeataLoc, 1, &intensitateCeata);
	}

	if (taste[GLFW_KEY_9]) 
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);// modul solid
	}

	if (taste[GLFW_KEY_8])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);// modul wireframe / muchii
	}

	if (taste[GLFW_KEY_7])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);// modul poligonal /varfuri
	}

	if (taste[GLFW_KEY_6])
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_SMOOTH);// modul smooth
	}

	if (taste[GLFW_KEY_J]) 
	{
		unghiLumina -= 1.0f; //dec unghi lumina
		model = glm::rotate(glm::mat4(1.0f), glm::radians(unghiLumina), glm::vec3(0, 1, 0));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (taste[GLFW_KEY_L])
	{
		unghiLumina += 1.0f; //inc unghi lumina
		model = glm::rotate(glm::mat4(1.0f), glm::radians(unghiLumina), glm::vec3(0, 1, 0));
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	}

	if (taste[GLFW_KEY_N])
	{
		ziNoapte = 0; //comuta pe peisaj noapte
		ziNoapteLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "ziNoapte");
		glUniform1i(ziNoapteLoc, ziNoapte);
		initializareSkybox("noapte");
	}

	if (taste[GLFW_KEY_Z])
	{
		ziNoapte = 1; //comuta pe peisaj zi
		ziNoapteLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "ziNoapte");
		glUniform1i(ziNoapteLoc, ziNoapte);
		initializareSkybox("zi");
	}
}

void initializareOpenGl()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, fereastraApp.getWindowDimensions().width, fereastraApp.getWindowDimensions().height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_FRAMEBUFFER_SRGB);
}

void initializareObjs()
{
	scenaSatParasitLaOcean.LoadModel("models/scenaProiect/scenaProiect.obj", "models/scenaProiect/");
	cubLumina.LoadModel("models/cube/cube.obj");
	patratUmbra.LoadModel("models/quad/quad.obj");
	broasca.LoadModel("models/broasca/turtle.obj");
	nava.LoadModel("models/nava/nava.obj");
}

void initializareShadere()
{
	shaderPrincipal.incarcaShader("shaders/shaderPrincipal.vert", "shaders/shaderPrincipal.frag");
	shaderPrincipal.utilizeazaShader();
	shaderLuminaDirectionala.incarcaShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	shaderLuminaDirectionala.utilizeazaShader();
	shaderPatratUmbra.incarcaShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	shaderPatratUmbra.utilizeazaShader();
	shaderUmbre.incarcaShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
	shaderUmbre.utilizeazaShader();
	shaderSkyboxuri.incarcaShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
	shaderSkyboxuri.utilizeazaShader();
}

void initializareVarsUniform()
{
	shaderPrincipal.utilizeazaShader();
	model = glm::rotate(glm::mat4(1.0f), glm::radians(unghiRotatieScena), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "model");

	view = cameraUtilizator.calculeazaMatriceVizualizare();
	viewLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(unghiVedere), (float)fereastraApp.getWindowDimensions().width / (float)fereastraApp.getWindowDimensions().height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	dirLumina = glm::vec3(0.0f, 1.0f, -1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(unghiLumina), glm::vec3(0.0f, 1.0f, 0.0f));

	dirLuminaLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "dirLumina");
	glUniform3fv(dirLuminaLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * dirLumina));

	culoareLumina = glm::vec3(1.0f, 1.0f, 1.0f);
	culoareLuminaLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "culoareLumina");
	glUniform3fv(culoareLuminaLoc, 1, glm::value_ptr(culoareLumina));

	ziNoapteLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "ziNoapte");
	glUniform1i(ziNoapteLoc, ziNoapte);

	culoareLuminaFelinare = glm::vec3(1.0f, 1.0f, 1.0f);
	culoareLuminaFelinareLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "culoareLuminaFelinare");
	glUniform3fv(culoareLuminaFelinareLoc, 1, glm::value_ptr(culoareLuminaFelinare));

	constantLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "constant");
	glUniform1f(constantLoc, constant);

	linearLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "linear");
	glUniform1f(linearLoc, linear);

	quadraticLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "quadratic");
	glUniform1f(quadraticLoc, quadratic);

	coordonateFelinareLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "coordonateFelinare");
	glUniform3fv(coordonateFelinareLoc, 4, glm::value_ptr(coordonateFelinare[0]));

	intensitateCeataLoc = glGetUniformLocation(shaderPrincipal.shaderProgram, "intensitateCeata");
	glUniform1f(intensitateCeataLoc, intensitateCeata);

	shaderLuminaDirectionala.utilizeazaShader();
	glUniformMatrix4fv(glGetUniformLocation(shaderLuminaDirectionala.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

glm::mat4 calcMatriceSpLumina()
{
	glm::mat4 lightView = glm::lookAt(dirLumina, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //calc in sp luminii
	const GLfloat near_plane = -450.0f, far_plane = 450.0f;
	glm::mat4 lightProjection = glm::ortho(-450.0f, 450.0f, -150.0f, 150.0f, near_plane, far_plane);//proiectie ortografica in sp luminii
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView; //calcul matrice lumina
	return lightSpaceTrMatrix;
}

void deseneazaBroasca(gps::Shader shader, bool depthPass)
{
	if (broascaPozitie.x <= -125.0f || broascaPozitie.x >= 10.0f)
	{
		directieBroasca = !directieBroasca; //schimba directie broasca
		unghiBrasca = unghiBrasca - 180.0f; //schimba unghi la 180 grade
	}

	if (ziNoapte == 0) 
	{
		broascaPozitie.x = broascaPozitie.x; //broasca nu se misca noaptea
	}
	else if (directieBroasca)
	{
		broascaPozitie.x = broascaPozitie.x + 0.03f; //mareste pe x ptr deplasare
	}
	else
	{
		broascaPozitie.x = broascaPozitie.x - 0.03f; //scade pe x ptr deplasare
	}

	model = glm::rotate(glm::mat4(1.0f), glm::radians(unghiRotatieScena), glm::vec3(0.0f, 1.0f, 0.0f));
	//translatie in jurul centrului
	model = glm::translate(model, broascaPozitie);
	model = glm::rotate(model, glm::radians(unghiBrasca), glm::vec3(0.0f, 1.0f, 0.0f)); //rotatia cu 180 grade
	model = glm::translate(model, -broascaPozitie);
	model = glm::translate(model, broascaPozitie);

	if (!depthPass) 
	{
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	broasca.Draw(shader);
}

void deseneazaNava(gps::Shader shader, bool depthPass)
{
	if (navaPozitie.z >= 127.0f || navaPozitie.z <= -127.0f)
	{
		directieNava = !directieNava; //schimba directie nava
		unghiNava = unghiNava + 180.0f; //schimba unghi la 180
	}

	if (directieNava)
	{
		navaPozitie.z = navaPozitie.z + 0.1f; //incrementare
	}
	else
	{
		navaPozitie.z = navaPozitie.z - 0.1f; //decrementare
	}

	model = glm::rotate(glm::mat4(1.0f), glm::radians(unghiRotatieScena), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, navaPozitie);
	model = glm::rotate(model, glm::radians(unghiNava), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, -navaPozitie);
	model = glm::translate(model, navaPozitie);

	if (!depthPass)
	{
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}

	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	nava.Draw(shader);
}

void desenareObiecteScena(gps::Shader shader, bool depthPass) {

	shader.utilizeazaShader();

	model = glm::rotate(glm::mat4(1.0f), glm::radians(unghiRotatieScena), glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

	if (!depthPass) {
		constantLoc = glGetUniformLocation(shader.shaderProgram, "constant");
		glUniform1f(constantLoc, constant);
		linearLoc = glGetUniformLocation(shader.shaderProgram, "linear");
		glUniform1f(linearLoc, linear);
		quadraticLoc = glGetUniformLocation(shader.shaderProgram, "quadratic");
		glUniform1f(quadraticLoc, quadratic);
		normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		ziNoapteLoc = glGetUniformLocation(shader.shaderProgram, "ziNoapte");
		glUniform1i(ziNoapteLoc, ziNoapte);
	}

	scenaSatParasitLaOcean.Draw(shader);
	deseneazaBroasca(shader, depthPass);
	deseneazaNava(shader, depthPass);
}


void initializareFBO() {
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, UMBRAW, UMBRAH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void animatieInceputCamera() 
{	
	if (animatieComutare)
	{
		if (numarAnimatie == 1)
		{
			unghiRotatieAnimatie += 1.0f;
			cameraUtilizator.animatie1Scena(unghiRotatieAnimatie, glm::vec3(-100.0, 50.0, 40.0));
			if ((glfwGetTime() - timpInceputAnimatie) > 14 && (glfwGetTime() - timpInceputAnimatie) < 20.0)
			{
				ziNoapte = 0;
				initializareSkybox("noapte");
			}
			else if ((glfwGetTime() - timpInceputAnimatie) > 21.0)
			{
				unghiRotatieAnimatie = 0.0f;
				ziNoapte = 1;
				initializareSkybox("zi");
				numarAnimatie = 2;
				timpInceputAnimatie = glfwGetTime();
			}
		}
		else
		{
			glm::vec3 destinatie(0.0f, 5.0f, 5.0f);			
			float viteza = 0.015f;
			cameraUtilizator.animatie2Scena(destinatie, viteza);
			if ((glfwGetTime() - timpInceputAnimatie) >= 8) {
				animatieComutare = false;
				gps::Camera cameraDupaAnimatii(glm::vec3(0.0f, 2.0f, 5.0f), glm::vec3(0.0f, 1.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				cameraUtilizator = cameraDupaAnimatii;
			}
		}
	}
}


void renderScene() {

	shaderUmbre.utilizeazaShader();
	glm::mat4 matrSpLumina = calcMatriceSpLumina();
	glUniformMatrix4fv(glGetUniformLocation(shaderUmbre.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(matrSpLumina));
	glUniformMatrix4fv(glGetUniformLocation(shaderUmbre.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glViewport(0, 0, UMBRAW, UMBRAH);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	desenareObiecteScena(shaderUmbre, true);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (showDepthMap) {

		glViewport(0, 0, fereastraApp.getWindowDimensions().width, fereastraApp.getWindowDimensions().height);

		glClear(GL_COLOR_BUFFER_BIT);

		shaderPatratUmbra.utilizeazaShader();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(shaderPatratUmbra.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		patratUmbra.Draw(shaderPatratUmbra);
		glEnable(GL_DEPTH_TEST);
	}
	else {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, fereastraApp.getWindowDimensions().width, fereastraApp.getWindowDimensions().height);

		shaderPrincipal.utilizeazaShader();

		view = cameraUtilizator.calculeazaMatriceVizualizare();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(unghiLumina), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(dirLuminaLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * dirLumina));

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(shaderPrincipal.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(shaderPrincipal.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(calcMatriceSpLumina()));

		desenareObiecteScena(shaderPrincipal, false);

		shaderLuminaDirectionala.utilizeazaShader();
		glUniformMatrix4fv(glGetUniformLocation(shaderLuminaDirectionala.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * dirLumina);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(glGetUniformLocation(shaderLuminaDirectionala.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		//cubLumina.Draw(shaderLuminaDirectionala);
		skyboxSatOcean.Draw(shaderSkyboxuri, view, projection);
	}
}

void cleanup() {
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	fereastraApp.Delete();
}

int main(int argc, const char* argv[]) {

	try {
		initOpenGLWindow();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	initializareOpenGl();
	initializareObjs();
	initializareShadere();
	initializareVarsUniform();
	initializareFBO();
	initializareSkybox("zi");
	glCheckError();

	while (!glfwWindowShouldClose(fereastraApp.getWindow())) {

		controleazaTaste();
		renderScene(); 
		animatieInceputCamera();

		glfwPollEvents();
		glfwSwapBuffers(fereastraApp.getWindow());
	}

	cleanup();

	return 0;
}

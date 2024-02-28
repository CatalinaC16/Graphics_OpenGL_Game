#include "Camera.hpp"

namespace gps {

    Camera::Camera(glm::vec3 pozitie, glm::vec3 target, glm::vec3 directieSus)
    {
        this->pozitie = pozitie;
        this->target = target;
        this->directieSus = glm::normalize(directieSus);
        this->directieInainte = glm::normalize(target - pozitie);
        this->directieDreapta = glm::normalize(glm::cross(this->directieInainte, this->directieSus));
    }

    glm::mat4 Camera::calculeazaMatriceVizualizare() 
    {  
       return glm::lookAt(pozitie, pozitie + directieInainte, directieSus);   
    }

    void Camera::miscaCamera(DIRECTIE directie, float viteza)
    { 
        switch (directie)
        {
            case INAINTE:
                this->pozitie = this->pozitie + (viteza * this->directieInainte);
                break;
            case INAPOI:
                this->pozitie = this->pozitie -  (viteza * this->directieInainte);
                break;
            case STANGA:
                this->pozitie = this->pozitie - (viteza * this->directieDreapta);
                break;
            case DREAPTA:
                this->pozitie = this->pozitie + (viteza * this->directieDreapta);
                break;            
        }
    }

    void Camera::rotesteCamera(float pitch, float yaw)
    {
        glm::vec3 direction;
     
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

        this->directieInainte = glm::normalize(direction);
        this->directieDreapta = glm::normalize(glm::cross(directieInainte, directieSus));
        this->target = pozitie + directieInainte;
    }

    glm::vec3 Camera::getPozitieCamera() 
    {
        return this->pozitie;
    }

    glm::vec3 Camera::getVectorSus()
    {
        return this->directieSus;
    }

    glm::vec3 Camera::getVectorTarget()
    {
        return this->target;
    }

    glm::vec3 Camera::getVectorInainte()
    {
        return this->directieInainte;
    }

    void Camera::animatie1Scena(float unghiRotatie, const glm::vec3 pozitieCamera)
    {     
        this->pozitie = pozitieCamera;
        glm::mat4 rotatie = glm::rotate(glm::mat4(1.0f), glm::radians(unghiRotatie), glm::vec3(0, 1, 0));
        this->pozitie = glm::vec4(rotatie * glm::vec4(this->pozitie, 1));
        this->directieInainte = glm::normalize(this->target - this->pozitie);
        this->directieDreapta = glm::normalize(glm::cross(this->directieInainte, this->directieSus));
    }

    void Camera::animatie2Scena(const glm::vec3& pozitieDestinatie, float vitezaAnimatie) {
     
        vitezaAnimatie = glm::clamp(vitezaAnimatie, 0.0f, 1.0f);
        this->pozitie = glm::mix(this->pozitie, pozitieDestinatie, vitezaAnimatie);
        this->directieInainte = glm::normalize(this->target - this->pozitie);
        this->directieDreapta = glm::normalize(glm::cross(this->directieInainte, this->directieSus));
    }

}
#ifndef Camera_hpp
#define Camera_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gps {

    enum DIRECTIE { INAINTE, INAPOI, DREAPTA, STANGA };

    class Camera {

    public:

        glm::vec3 pozitie;

        Camera(glm::vec3 pozitie, glm::vec3 target, glm::vec3 directieSus);
     
        glm::mat4 calculeazaMatriceVizualizare();

        void miscaCamera(DIRECTIE directie, float viteza);

        void rotesteCamera(float pitch, float yaw);

        glm::vec3 getPozitieCamera();

        glm::vec3 getVectorTarget();

        glm::vec3 getVectorInainte();

        glm::vec3 getVectorSus();

        void animatie1Scena(float unghiRotatie, glm::vec3 pozitieCamera);

        void animatie2Scena(const glm::vec3& pozitieDestinatie, float vitezaAnimatie);
 

    private:
    
        glm::vec3 target;
        glm::vec3 directieInainte;
        glm::vec3 directieDreapta;
        glm::vec3 directieSus;
    };
}

#endif /* Camera_hpp */

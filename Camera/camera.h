#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
    Camera();
    static Camera* getCam(){
        if(cam == NULL){
            cam = new Camera();
        }
        return cam;
    }
    glm::mat4 getView(){return view;}
    glm::mat4 getProjection(){return perspective;}
    glm::vec3 getCamPos(){return camPos;}
    glm::vec3 getUpPos(){return upPos;}
    glm::vec3 getTargetPos(){return targetPos;}

    void setView(glm::mat4 view);
    void setProjection(glm::mat4 projection);
    void setCamPos(glm::vec3 camPos);
    void setUpPos(glm::vec3 upPos);
    void setTargetPos(glm::vec3 targetPos);

private:
    static Camera* cam;
    glm::mat4 view;
    glm::mat4 perspective;
    glm::vec3 camPos;
    glm::vec3 upPos;
    glm::vec3 targetPos;
};

#endif // CAMERA_H

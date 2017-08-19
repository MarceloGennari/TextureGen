#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "GL/freeglut.h"
class Camera
{
public:
    Camera(){}
    static Camera* getCam(){
        if(cam == NULL){
            cam = new Camera();
        }
        return cam;
    }

    void static keyBoardInput(unsigned char key, int x, int y);
    void static mouseInput(int button, int state, int x, int y);


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

    int lastX, lastY;
    float yaw, pitch;

private:
    static Camera* cam;
    glm::mat4 view;
    glm::mat4 perspective;
    glm::vec3 camPos;
    glm::vec3 upPos;
    glm::vec3 targetPos;
    void updateView();
};

#endif // CAMERA_H

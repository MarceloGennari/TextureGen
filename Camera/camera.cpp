#include "camera.h"

Camera* Camera::cam;

void Camera::keyBoardInput(unsigned char key, int x, int y){
    /*
     * This is an FPS style of camera
     * wasd/zx controls your position
     * ijkl controls your angle
     * */

    glm::vec3 Direction;
    glm::vec3 Right;
    float dirAbs;
    float vel = 1;
    float angle = 1;
    switch(key){
        case 'w':
            Direction = cam->camPos - cam->targetPos;
            Direction = glm::normalize(Direction);
            cam->setCamPos(cam->camPos - Direction*vel);
            cam->setTargetPos(cam->targetPos - Direction*vel);
            break;
        case 'a':
            Direction = cam->camPos - cam->targetPos;
            Right = glm::cross(Direction, cam->upPos);
            Right = glm::normalize(Right);
            cam->setCamPos(cam->camPos + Right*vel);
            cam->setTargetPos(cam->targetPos + Right*vel);
            break;
        case 's':
            Direction = cam->camPos - cam->targetPos;
            Direction = glm::normalize(Direction);
            cam->setCamPos(cam->camPos + Direction*vel);
            cam->setTargetPos(cam->targetPos + Direction*vel);
            break;
        case 'd':
            Direction = cam->camPos - cam->targetPos;
            Right = glm::cross(Direction, cam->upPos);
            Right = glm::normalize(Right);
            cam->setCamPos(cam->camPos - Right*vel);
            cam->setTargetPos(cam->targetPos - Right*vel);
            break;
        case 'z':
            cam->setCamPos(cam->getCamPos()+glm::vec3(0.0f, vel, 0.0f));
            cam->setTargetPos(cam->getTargetPos()+glm::vec3(0.0f, vel, 0.0f));
            break;
        case 'x':
            cam->setCamPos(cam->getCamPos()+glm::vec3(0.0f, -vel, 0.0f));
            cam->setTargetPos(cam->getTargetPos()+glm::vec3(0.0f, -vel, 0.0f));
        break;
        case 'j':
            Direction = cam->camPos-cam->targetPos;
            dirAbs = glm::length(Direction);
            Direction = glm::normalize(Direction);
            Direction = glm::rotate(Direction, glm::radians(angle), cam->upPos);
            Direction = Direction*dirAbs;
            cam->setTargetPos( cam->camPos-Direction);
            break;
        case 'k':
            Direction = cam->camPos - cam->targetPos;
            Right = glm::cross(Direction, cam->upPos);
            Right = glm::normalize(Right);
            dirAbs = glm::length(Direction);

            Direction = glm::normalize(Direction);
            Direction = glm::rotate(Direction, glm::radians(angle), Right);
            Direction = Direction*dirAbs;
            cam->setTargetPos( cam->camPos-Direction);
            break;
        case 'i':
            Direction = cam->camPos - cam->targetPos;
            Right = glm::cross(Direction, cam->upPos);
            Right = glm::normalize(Right);
            dirAbs = glm::length(Direction);

            Direction = glm::normalize(Direction);
            Direction = glm::rotate(Direction, glm::radians(-angle), Right);
            Direction = Direction*dirAbs;
            cam->setTargetPos( cam->camPos-Direction);
            break;
        case 'l':
            Direction = cam->camPos-cam->targetPos;
            dirAbs = glm::length(Direction);
            Direction = glm::normalize(Direction);
            Direction = glm::rotate(Direction, glm::radians(-angle), cam->upPos);
            Direction = Direction*dirAbs;
            cam->setTargetPos( cam->camPos-Direction);
            break;
        default:
            break;
    }
}
void Camera::mouseInput(int button, int state, int x, int y){
//    if(button == GLUT_LEFT_BUTTON){
//        Camera* cam = Camera::getCam();
//        int offsetX = x - cam->lastX;
//        int offsetY = y - cam->lastY;
//        cam->lastX = x;
//        cam->lastY = y;


//        float sensitivity = 0.5;
//        offsetX *= sensitivity;
//        offsetY *= sensitivity;

//        cam->yaw   += offsetX;
//        cam->pitch += offsetY;

//        if(cam->pitch > 89.0f)
//            cam->pitch = 89.0f;
//        if(cam->pitch < -89.0f)
//            cam->pitch = -89.0f;

//        glm::vec3 front;
//        front.x = cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
//        front.y = sin(glm::radians(cam->pitch));
//        front.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
//        cam->setUpPos(glm::normalize(front));
//    }

}

void Camera::updateView(){
    this->view = glm::lookAt(this->getCamPos(), this->targetPos, this->upPos);
}

void Camera::setView(glm::mat4 view){
    this->view =view;
}

void Camera::setProjection(glm::mat4 projection){
    this->perspective =projection;
}

void Camera::setCamPos(glm::vec3 camPos){
    this->camPos = camPos;
    this->updateView();
}

void Camera::setUpPos(glm::vec3 upPos){
    this->upPos = upPos;
    this->updateView();
}

void Camera::setTargetPos(glm::vec3 targetPos){
    this->targetPos = targetPos;
    this->updateView();
}

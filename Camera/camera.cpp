#include "camera.h"

Camera* Camera::cam;

Camera::Camera()
{
//    cam->perspective = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
}

void Camera::setView(glm::mat4 view){
    this->view =view;
}

void Camera::setProjection(glm::mat4 projection){
    this->perspective =projection;
}

void Camera::setCamPos(glm::vec3 camPos){
    this->camPos = camPos;
}

void Camera::setUpPos(glm::vec3 upPos){
    this->upPos = upPos;
}

void Camera::setTargetPos(glm::vec3 targetPos){
    this->targetPos = targetPos;
}

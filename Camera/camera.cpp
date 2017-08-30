#include "camera.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

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
    glm::mat4 Temp;
    glm::vec3 newTarg;
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
        case 'n':
            Temp = glm::rotate(Temp, glm::radians(1.0f), cam->getUpPos());
            cam->setTargetPos(glm::vec3(Temp*glm::vec4(cam->getTargetPos(),1.0f)));
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

void Camera::initializeCalib(){
    std::string path = "/home/marcelo/TextureGen/Teddy/calib.txt";
    std::string line;
    std::ifstream calibFile(path.c_str());
    std::vector<std::string> array;
    if(calibFile.is_open()){
        while(getline(calibFile,line)){
          std::size_t pos = 0, found;

          while((found = line.find_first_of(' ', pos)) != std::string::npos) {
              array.push_back(line.substr(pos, found - pos));
              pos = found+1;
          }
          array.push_back(line.substr(pos));
        }
    }

    float foc[2];
        std::istringstream (array[2])>>foc[0];
        std::istringstream (array[3])>>foc[1];
    this->focal = glm::vec2(foc[0], foc[1]);

    float pp[2];
        std::istringstream (array[4])>>pp[0];
        std::istringstream (array[5])>>pp[1];
    this->PrincPoint = glm::vec2(pp[0], pp[1]);

    float calib[12];
    for(int k =0; k<12; k++)
        std::istringstream(array[k+14])>>calib[k];
    this->KMat = glm::make_mat3x4(calib);


}

void Camera::getPose(const std::string &frameNr, glm::mat3 &Rot, glm::vec3 &Tra, glm::mat4 &Pose){
      std::string line;
      std::string path = "/home/marcelo/TextureGen/Teddy/Poses/Pose"+frameNr + ".txt";
      std::ifstream pose(path.c_str());
      std::vector<std::string> array;

      if(pose.is_open()){
          while(getline(pose, line)){
            std::size_t pos = 0, found;

            while((found = line.find_first_of(' ', pos)) != std::string::npos) {
                array.push_back(line.substr(pos, found - pos));
                pos = found+1;
            }
            array.push_back(line.substr(pos));


          }
      }

    float m[9];
    float t[3];
    float p[16];
    for(int k = 0; k<9; k++)
        std::istringstream ( array[k] ) >> m[k];
    for(int k=9; k<12; k++)
        std::istringstream( array[k] ) >> t[k-9];
    for(int k = 12; k<28; k++)
        std::istringstream( array[k] ) >> p[k-12];

    // Remember that glm::mat3 is stored column-wise
    Rot = glm::make_mat3(m);
    Rot = glm::transpose(Rot);
    Pose = glm::make_mat4(p);
    Pose = glm::transpose(Pose);
    Tra = glm::vec3(t[0], t[1], t[2]);
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

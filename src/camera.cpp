#include "camera.h"
#include <iostream>


Camera::Camera(unsigned int w, unsigned int h)
    : fovy(55), aspect(w / (float)h), nearClip(0.1), farClip(1000),
      eye(0,0,10), target(0,0,0),
      forward(0,0,-1), up(0,1,0), right(glm::cross(forward, up))
{}

void Camera::recomputeAspectRatio(unsigned int w, unsigned int h) {
    aspect = w / (float)h;
}


glm::mat4 Camera::getViewProj() {
    return getProj() * getView();
}

glm::mat4 Camera::getViewProj_OrientOnly() {
    return getProj() * glm::lookAt(glm::vec3(0,0,0), target - eye, up);
}

glm::mat4 Camera::getView() {
    return glm::lookAt(eye, target, up);
}

glm::mat4 Camera::getProj() {
    return glm::perspective(glm::radians(fovy), aspect, nearClip, farClip);
}


void Camera::RotateAboutGlobalUp(float deg) {
    glm::mat4 R = glm::rotate(glm::mat4(), glm::radians(deg), glm::vec3(0,1,0));
    glm::vec4 relEye = glm::vec4(eye - target, 1.);
    relEye = R * relEye;
    eye = glm::vec3(relEye) + target;
    forward = glm::vec3(R * glm::vec4(forward, 0.));
    up = glm::vec3(R * glm::vec4(up, 0.));
    right = glm::vec3(R * glm::vec4(right, 0.));
}

void Camera::RotateAboutLocalRight(float deg) {
    glm::mat4 R = glm::rotate(glm::mat4(), glm::radians(deg), right);
    glm::vec4 relEye = glm::vec4(eye - target, 1.);
    relEye = R * relEye;
    eye = glm::vec3(relEye) + target;
    forward = glm::vec3(R * glm::vec4(forward, 0.));
    up = glm::vec3(R * glm::vec4(up, 0.));
}

void Camera::Zoom(float amt) {
    glm::vec3 translation = forward * amt;
    eye += translation;
}

void Camera::PanAlongRight(float amt) {
    glm::vec3 translation = right * amt;
    eye += translation;
    target += translation;
}

void Camera::PanAlongUp(float amt) {
    glm::vec3 translation = up * amt;
    eye += translation;
    target += translation;
}

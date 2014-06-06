#include "Camera.h"
//
// Camera.h
//
// Copyright 2014 Seed Of Andromeda
//

#include "Options.h"
#include "GlobalStructs.h"
#include <SDL.h>
#include <iostream>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>

//the way I am initializing the position and angles here is stupid. I calculated it by hand and wrote it down.
Camera::Camera() : pitchAngle(0), yawAngle(3*M_PI/2), direction(-1.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f), right(0.0f, 0.0f, -1.0f), position(2.0f, 0.0f, 0.5f)
{
	updateViewMatrix();
	updateProjectionMatrix();
}


void Camera::update()
{
	const float moveSpeed = 0.5f;
	if (Keys[SDLK_w].pr){
		position += direction*moveSpeed;
	}else if (Keys[SDLK_s].pr){
		position -= direction* moveSpeed;
	}

	if (Keys[SDLK_d].pr){
		position += right*moveSpeed;
	}
	else if (Keys[SDLK_a].pr){
		position -= right*moveSpeed;
	}

	if (Keys[SDLK_SPACE].pr){
		position += glm::vec3(0.0, 1.0, 0.0)*moveSpeed;
	}
	else if (Keys[SDLK_LSHIFT].pr){
		position -= glm::vec3(0.0, 1.0, 0.0)* moveSpeed;
	}

	updateViewMatrix();
}

//this function moves the camera direction when the player is dragging the camera
void Camera::mouseMove(int relx, int rely)
{


	yawAngle += controlsOptions.mouseSensitivity * float(-relx) * 0.005f;
	pitchAngle += controlsOptions.mouseSensitivity * float(-rely) * 0.005f;
	cout << pitchAngle << " " << yawAngle << endl;
	//bound angles
	if (pitchAngle > M_PI / 2.0f){
		pitchAngle = M_PI / 2.0f;
	}
	else if (pitchAngle < -M_PI / 2.0f){
		pitchAngle = -M_PI / 2.0f;
	}

	//wrap angles
	if (yawAngle > 2.0f*M_PI){
		yawAngle = 0.0f;
	}
	else if (yawAngle < 0.0f){
		yawAngle = 2.0f*M_PI - 0.00001f; //for some reason it gets stuck at zero without the - 0.00001f
	}

	direction = glm::vec3(
		cos(pitchAngle) * sin(yawAngle),
		sin(pitchAngle),
		cos(pitchAngle) * cos(yawAngle)
		);

	// Right vector
	right = glm::vec3(
		sin(yawAngle - 3.14f / 2.0f),
		0,
		cos(yawAngle - 3.14f / 2.0f)
		);

	up = glm::cross(right, direction);
}

void Camera::mouseZoom(float zoomScale){
	zoomScale *= 0.04f;
	position += direction * zoomScale;
}

//update the view matrix every time the camera moves, or every frame
void Camera::updateViewMatrix()
{
	glm::vec3 emptyVec(0.0f, 0.0f, 0.0f);
	// Camera matrix
	viewMatrix = glm::lookAt(
		emptyVec,           // Camera is always at the origin. We translate the world around us, we dont translate the camera
		glm::vec3(direction), // and looks here : at the same position, plus "direction"
		glm::vec3(up)                  // Head is up (set to 0,-1,0 to look upside-down)
		);
}

//update the projection matrix only when the fov changes
void Camera::updateProjectionMatrix()
{
	float znear = 0.01;
	float zfar = 10000.0f;
	projectionMatrix = glm::perspective((float)(graphicsOptions.fov), (float)graphicsOptions.screenWidth / (float)graphicsOptions.screenHeight, znear, zfar);
}

//glm::vec3 Camera::screenToWorld(glm::vec2 mouse, int width, int height)
//{
//	cout << height << " " << width << endl;
//	cout << mouse.x << " " << mouse.y << endl;
//	printf("screen space <%f,%f>\n.", (mouse.x / (float)width - 0.5f), (-mouse.y / (float)height + 0.5f));
//	glm::vec4 rayStart(0.0f, 0.0f, -1.0f, 1.0f);
//	//glm::vec4 rayEnd(mouse.x - ((float)width / 2), mouse.y - ((float)height / 2), 0.0, 1.0);
//	//glm::vec4 rayStart((mouse.x / (float)width - 0.5f) * 2.0f, (mouse.y / (float)height - 0.5f) * 2.0f, -1.0f, 1.0f);
//	glm::vec4 rayEnd((mouse.x / (float)width - 0.5f) * 2.0f, -(mouse.y / (float)height - 0.5f) * 2.0f, 0.0f, 1.0f);
//	//glm::vec4 rayEnd((2.0f * mouse.x) / width - 1.0f, 1.0f - (2.0f * mouse.y) / height, -1.0f, 1.0f);
//	glm::mat4 modelMatrix(1);
//	modelMatrix[3][0] = -position.x;
//	modelMatrix[3][1] = -position.y;
//	modelMatrix[3][2] = -position.z;
//
//
//	glm::mat4 M = glm::inverse(projectionMatrix * viewMatrix);
//	//M = M * modelMatrix;
//	//glm::mat4 M = projectionMatrix * viewMatrix;
//	glm::vec4 rayStartWorld = M * rayStart;
//	rayStartWorld /= rayStartWorld.w;
//	glm::vec4 rayEndWorld = M * rayEnd;
//	rayEndWorld /= rayEndWorld.w;
//
//	glm::vec3 rayDirWorld(rayEndWorld - rayStartWorld);
//
//	rayDirWorld = glm::normalize(rayDirWorld);
//
//	return rayDirWorld;
//	//glm::mat4 invProj = glm::inverse(projectionMatrix);
//	//glm::mat4 invView = glm::inverse(viewMatrix);
//
//	//glm::vec4 rayStartCamera = invProj * rayStart;
//	//rayStartCamera /= rayStartCamera.w;
//	//glm::vec4 rayStartWorld = invView * rayStartCamera;
//	//rayStartWorld /= rayStartWorld.w;
//	//glm::vec4 rayEndCamera = invProj * rayEnd;
//	//rayEndCamera /= rayEndCamera.w;
//	//glm::vec4 rayEndWorld = invView * rayEndCamera;
//	//rayEndWorld /= rayEndWorld.w;
//	//glm::vec3 rayDirWorld(rayEndWorld - rayStartWorld);
//
//	//rayDirWorld = glm::normalize(rayDirWorld);
//
//	//return rayDirWorld;
//}

glm::vec3 Camera::screenToWorld(glm::vec2 mouse, int width, int height)
{
	    /*cout << height << " " << width << endl;
				    cout << mouse.x << " " << mouse.y << endl;
				    printf("Screen space <%f,%f>.\n", (mouse.x / (float)width - 0.5f), -(mouse.y / (float)height - 0.5f));
				    glm::vec4 rayClip((mouse.x / (float)width - 0.5f), -(mouse.y / (float)height - 0.5f), 0.0f, 1.0f);
				    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
			
				    rayEye = glm::vec4(rayEye.x, rayEye.y, rayEye.z, 1.0);
				    glm::vec3 rayWorld = glm::normalize(glm::vec3((glm::inverse(viewMatrix) * rayEye).x, (glm::inverse(viewMatrix) * rayEye).y, (glm::inverse(viewMatrix) * rayEye).z));
			
				    printf("World space <%f,%f,%f>.\n", rayWorld.x, rayWorld.y, rayWorld.z);
				*/
			
					//followed this tutorial : http://antongerdelan.net/opengl/raycasting.html
	float x = (2.0f * mouse.x) / width - 1.0f;
	float y = 1.0f - (2.0f * mouse.y) / height;
	glm::vec4 rayClip(x, y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);
	glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
	return glm::normalize(rayWorld);
}

void Camera::findIntersect(glm::vec3 direction){
	float i = 0.1f;
	glm::vec3 base = direction, tempV;
	voxel *tempVox;

    drawDebugLine = 1;
    debugP1 = position;

	while(i < 50.0){
		tempV = direction * i + position;
		if (tempV.x >= 0 && tempV.y >= 0){
			tempVox = gameGrid->getVoxel(tempV.x, tempV.y, tempV.z);
		}
		else if (tempV.z >= 0){
			tempV = direction * (i - .01f) + position;
			tempVox = gameGrid->getVoxel(tempV.x, tempV.y, tempV.z);
		}
		else{
			tempVox = NULL;
		}
		
		if (tempVox == NULL){
			//cout << "i: " << i << endl;
			//printf("Final attempt at <%f,%f,%f>.\n", tempV.x, tempV.y, tempV.z);
			//printf("Final attempt at <%f,%f,%f>.\n", round(tempV.x), round(tempV.y), round(tempV.z));
			//break;
		}

		else if (tempVox->type != '\0'){
			if (!(Keys[SDLK_q].pr) && !(Keys[SDLK_e].pr)){
				cout << tempVox->type << endl;
				tempVox->selected = !(tempVox->selected);
				printf("Voxel at <%d,%d,%d> clicked.\n", (int)(tempV.x), (int)(tempV.y), (int)(tempV.z));

				break;
			}
			else if (Keys[SDLK_q].pr){
				cout << tempVox->type << endl;
				gameGrid->removeVoxel(tempV.x, tempV.y, tempV.z);
				printf("Voxel at <%d,%d,%d> removed.\n", (int)(tempV.x), (int)(tempV.y), (int)(tempV.z));
				break;
			}
			else if (Keys[SDLK_e].pr){
				cout << tempVox->type << endl;
				tempV = direction * (i - .01f) + position;
				if (gameGrid->getVoxel(tempV.x, tempV.y - 1.0, tempV.z)->type != '\0'){
					gameGrid->addVoxel(currentVox, tempV.x, tempV.y, tempV.z);
					break;
				}
			}
		}
		i += 0.1f;
	}
	printf("Final attempt at <%f,%f,%f>.\n", tempV.x, tempV.y, tempV.z);
	printf("Final attempt at <%d,%d,%d>.\n", (int)tempV.x, (int)tempV.y, (int)tempV.z);
    debugP2 = tempV;
}
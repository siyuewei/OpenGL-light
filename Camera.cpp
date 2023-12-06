#include "Camera.h"


Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 worldUp)
{
	this->Position = position;
	this->Front = front;
	this->WorldUp = worldUp;

	this->Right = glm::normalize(glm::cross(Front,WorldUp));
	this->Up = glm::normalize(glm::cross(Right,Front));
	
	this->Yaw = glm::degrees(atan2(Front.z, Front.x));
	this->Pitch = glm::degrees(asin(Front.y));

	//std::cout << "yaw:" << Yaw << std::endl;
	//std::cout << "pitch:" << Pitch << std::endl;
}

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch)
{
	this->Position = position;
	this->WorldUp = worldUp;
	this->Yaw = yaw;
	this->Pitch = pitch;
	
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	this->Front = glm::normalize(front);

	this->Right = glm::normalize(glm::cross(Front, WorldUp));
	this->Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::GetViewMatrix()
{
	// 1. Position = known
	// 2. Calculate cameraDirection
	//glm::vec3 target = glm::vec3(0, 0, 0);
	//glm::vec3 zaxis = glm::normalize(Position - target);
	//// 3. Get positive right axis vector
	//glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(WorldUp), zaxis));
	//// 4. Calculate camera up vector
	//glm::vec3 yaxis = glm::cross(zaxis, xaxis);

	//// Create translation and rotation matrix
	//// In glm we access elements as mat[col][row] due to column-major layout
	//glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
	//translation[3][0] = -Position.x; // Third column, first row
	//translation[3][1] = -Position.y;
	//translation[3][2] = -Position.z;
	//glm::mat4 rotation = glm::mat4(1.0f);
	//rotation[0][0] = xaxis.x; // First column, first row
	//rotation[1][0] = xaxis.y;
	//rotation[2][0] = xaxis.z;
	//rotation[0][1] = yaxis.x; // First column, second row
	//rotation[1][1] = yaxis.y;
	//rotation[2][1] = yaxis.z;
	//rotation[0][2] = zaxis.x; // First column, third row 
	//rotation[1][2] = zaxis.y;
	//rotation[2][2] = zaxis.z;

	// Return lookAt matrix as combination of translation and rotation matrix
	//return rotation * translation; // Remember to read from right to left (first translation then rotation)
	//glm::mat4 view = rotation * translation;


	//std::cout << "POSITION:" << Position[0] << " " << Position[1] << " " << Position[2] << std::endl;
	//glm::vec3 tmp = Position + Front;
	//std::cout << "POSITION+FRONT:" << tmp[0] << " " << tmp[1] << " " << tmp[2] << std::endl;
	//std::cout << "UP:" << Up[0] << " " << Up[1] << " " << Up[2] << std::endl;

	////glm::mat4 view = glm::lookAt(Position, Position + Front, Up);
	//std::cout << "rotation" << std::endl;
	//for (int i = 0; i < 4; ++i) {
	//	for (int j = 0; j < 4; ++j) {
	//		std::cout << rotation[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}

	//std::cout << "translation" << std::endl;
	//for (int i = 0; i < 4; ++i) {
	//	for (int j = 0; j < 4; ++j) {
	//		std::cout << translation[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}

	//std::cout << "view" << std::endl;
	//for (int i = 0; i < 4; ++i) {
	//	for (int j = 0; j < 4; ++j) {
	//		std::cout << view[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}


	//glm::mat2 tmp1 = glm::mat2(1.0f);
	//tmp1[0][0] = 0;
	//tmp1[0][1] = 1;
	//tmp1[1][0] = 2;
	//tmp1[1][1] = 3;
	//glm::mat2 tmp2 = glm::mat2(1.0f);
	//tmp2[0][0] = 0;
	//tmp2[0][1] = 1;
	//tmp2[1][0] = 2;
	//tmp2[1][1] = 3;

	//glm::mat2 reslut = tmp1 * tmp2;
	//std::cout << "reslut" << std::endl;
	//for (int i = 0; i < 2; ++i) {
	//	for (int j = 0; j < 2; ++j) {
	//		std::cout << reslut[i][j] << " ";
	//	}
	//	std::cout << std::endl;
	//}

	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyBoard(Camera_Movement direction, float deltaTime)
{
	float velocity = MoveSpeed * deltaTime;
	if (direction == FORWARD) {
		Position += Front * velocity;
	}
	if (direction == BACKWARD) {
		Position += -Front * velocity;
	}
	if (direction == RIGHT) {
		Position += Right * velocity;
	}
	if (direction == LEFT) {
		Position += -Right * velocity;
	}
	if (direction == UP) {
		Position.y += velocity;
	}
	if (direction == DOWN) {
		Position.y -= velocity;
	}
	//Position.y = 0.0f;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	//std::cout << "xoffset: " << xoffset << std::endl;
	//std::cout << "yoffset: " << yoffset << std::endl;

	Yaw += xoffset;
	Pitch += yoffset;

	if (constrainPitch) {
		if (Pitch >= 89.0f) {
			Pitch = 89.0f;
		}
		if (Pitch <= -1.0f) {
			Pitch = -1.0f;
		}
	}

	updateCameraVectors();
}

void Camera::ProcessScroll(float yOffset)
{
	zoom += -(float)(yOffset)*ZoomSensitivity;
	if (zoom < 1.0f)
		zoom = 1.0f;
	if (zoom > 45.0f)
		zoom = 45.0f;
}

glm::vec3 Camera::getPosition()
{
	return this->Position;
}

float Camera::getZoom()
{
	return this->zoom;
}

glm::vec3 Camera::getFront()
{
	return Front;
}

void Camera::updateCameraVectors()
{
	// calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right, Front));
}
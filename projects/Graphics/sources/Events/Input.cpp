#include "pch.h"
#include "Input.h"

#include <GLFW/glfw3.h>

namespace projectSolar
{

	bool Input::IsKeyPressed(const KeyCode key)
	{
		//auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		//auto state = glfwGetKey(window, static_cast<int32_t>(key));
		//return state == GLFW_PRESS;
		return false;
	}

	bool Input::IsMouseButtonPressed(const MouseCode button)
	{
		//auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		/*auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;*/
		return false;
	}

	glm::vec2 Input::GetMousePosition()
	{
		//auto* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		/*double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };*/
		return { 0, 0 };
	}

	float Input::GetMouseX()
	{
		return GetMousePosition().x;
	}

	float Input::GetMouseY()
	{
		return GetMousePosition().y;
	}

}
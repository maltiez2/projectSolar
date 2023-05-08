#pragma once

#include "Simulation.h"
#include "Graphics.h"
#include "Logger.h" 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#define EMIT_EVENT(dest, command, ...) auto* data = new EventsData::command({ __VA_ARGS__ });\
	m_handler.send(dest, Command::command, data)
#define PROCESS_EVENT(command, data) Events::command(m_owner, (EventsData::command*)data)


namespace projectSolar
{
	enum class Command : uint8_t
	{
		UNKNOWN_COMMAND = 0,
		// WINDOW
		WND_WINDOW_CLOSE = 101,
		// APPLICATION
		APP_SET_MAP_MODEL = 201,
		APP_SET_MAP_VIEW = 202,
		APP_SET_MAP_PROJ = 203
	};

	namespace EventsData
	{
		struct APP_SET_MAP_VIEW
		{
			size_t layer;
			float x;
			float y;
			float z;
		};
	}
	namespace Events
	{
		void APP_SET_MAP_VIEW(void* owner, EventsData::APP_SET_MAP_VIEW* data)
		{
			auto& layers = ((Application*)owner)->m_layers;
			auto* layer = layers.get<MapLayer>(data->layer);

			LOG_DEBUG("Set camera to: ", data->x, ", ", data->y, ", ", data->z);

			glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-data->x, -data->y, -data->z));

			layer->setView(view);
		}
	}
}

#pragma once
#include "EventCallbackManager.h"
#include <functional>

template<typename T>
inline void EventCallbackManager::BindEvent(T* _obj, EventCallback::eTrigger _e, void(T::* memberFunction)(void*, EventArgs*))
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	HandlerParam info;
	info.obj = _obj;
	info.handler = [_obj, memberFunction](void* sender, EventArgs* args)
	{
		(_obj->*memberFunction)(sender, args);
	};
	handlers[_e].push_back(info);
}

template<typename T>
inline void EventCallbackManager::UnbindEvent(T* _obj, EventCallback::eTrigger _e, void(T::* memberFunction)(void*, EventArgs*))
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	if (handlers.find(_e) != handlers.end())
	{
		auto& eventHandlers = handlers[_e];
		eventHandlers.erase(
			std::remove_if(eventHandlers.begin(), eventHandlers.end(), 
				[_obj](const HandlerParam& info){ return _obj == info.obj; }),
			eventHandlers.end());
	}
}

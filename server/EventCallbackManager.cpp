#include "EventCallbackManager.h"


EventCallbackManager* EventCallbackManager::_intance = nullptr;
EventCallbackManager* EventCallbackManager::instance()
{
	if (_intance == nullptr)
	{
		_intance = new EventCallbackManager();
	}
	return _intance;
}

EventCallbackManager::~EventCallbackManager()
{
	if (_intance == this)
	{
		_intance = nullptr;
	}
}

void EventCallbackManager::UnbindAllEventsForObject(void* obj)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	for (auto it = handlers.begin(); it != handlers.end(); it++)
	{
		std::vector<HandlerParam>& eventhandlers = it->second;
		eventhandlers.erase(
			std::remove_if(eventhandlers.begin(), eventhandlers.end(), 
				[obj](const HandlerParam& info){return info.obj == obj;}),
			eventhandlers.end()
		);
	}
}

void EventCallbackManager::Notify(EventCallback::eTrigger _e, void* sender, EventArgs* args)
{
	std::shared_lock<std::shared_mutex> lock(mutex);
	if (handlers.find(_e) != handlers.end())
	{
		std::vector<HandlerParam> excuteHanlder = handlers[_e];
		lock.unlock();

		for (const auto& info : excuteHanlder)
		{
			info.handler(sender, args);
		}
	}
}

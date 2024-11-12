#pragma once
// ## ������ ������Ƽ���� ����.
// ## �̺�Ʈ �� �Լ��� ���ؼ��� �۾��ڰ� �����Ͽ��� �Ѵ�.

#include"eEventCallbackTrigger.h"
#include"EventArgs.h"
#include <functional>
#include <unordered_map>
#include <shared_mutex>

class EventCallbackManager
{
private:
	static EventCallbackManager* _intance;

private:
	using EventHandler = std::function<void(void*,EventArgs*)>;
	struct HandlerParam {
		void* obj;
		EventHandler handler;
	};
	mutable std::shared_mutex  mutex;
	std::unordered_map< EventCallback::eTrigger, std::vector<HandlerParam>> handlers;
	
public:
	static EventCallbackManager* instance();
	EventCallbackManager() {};
	~EventCallbackManager();
	// ���� �����ڿ� ���� �����ڸ� �����Ͽ� �̱����� �������� ����
	EventCallbackManager(const EventCallbackManager&) = delete;
	EventCallbackManager& operator=(const EventCallbackManager&) = delete;

public:
	template<typename T>
	void BindEvent(T* _obj, EventCallback::eTrigger _e, void (T::* memberFunction)(void*, EventArgs*));
	template<typename T>
	void UnbindEvent(T* _obj, EventCallback::eTrigger _e, void (T::* memberFunction)(void*, EventArgs*));
	void UnbindAllEventsForObject(void* obj);

	void Notify(EventCallback::eTrigger _e, void* sender, EventArgs* args);
};

#include "EventCallbackManager_impl.h"
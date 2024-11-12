#pragma once
// ## 쓰레드 세이프티하지 않음.
// ## 이벤트 콜 함수에 대해서는 작업자가 관리하여야 한다.

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
	// 복사 생성자와 대입 연산자를 삭제하여 싱글톤의 고유성을 보장
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
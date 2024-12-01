#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <future>
using namespace boost::placeholders;

class ThreadPool {
public:
    ThreadPool(size_t thread_count);
    ~ThreadPool();

    template <typename Task>
    void post(Task task) {
        io_service_.post(task);
    }

    boost::asio::io_service& get_io_service();

private:
    boost::asio::io_service io_service_;
    boost::asio::executor_work_guard<boost::asio::io_service::executor_type> work_;
    boost::thread_group threads_;
};

//===========================================================================================
// 
class TaskManager {
public:
    TaskManager(size_t thread_count)
        : thread_pool_(thread_count) {}

    void addSocketTask(std::function<void()> task);
    void addFunctionTask(std::function<void()> task);
    void addTimerTask(std::function<void()> task, int milliseconds);
    std::future<std::string> addDBTask(std::function<std::string()> task);

    // 비동기 DB 쿼리 실행 함수
    template<typename Callback>
    void asyncDBQuery(const std::string& query, Callback callback) {
        thread_pool_.post([this, query, callback]() {
            std::string result = executeDBQuery(query);
            thread_pool_.get_io_service().post(std::bind(callback, result));
            });
    }

private:
    // 실제 DB 쿼리 실행을 시뮬레이션하는 함수
    std::string executeDBQuery(const std::string& query);

private:
    ThreadPool thread_pool_;
};
#include "ThreadPool.h"

void TaskManager::addSocketTask(std::function<void()> task)
{
    thread_pool_.post(task);
}


std::future<std::string> TaskManager::addDBTask(std::function<std::string()> task)
{
    auto promise = std::make_shared<std::promise<std::string>>();
    std::future<std::string> future = promise->get_future();

    thread_pool_.post([task, promise]() {
        std::string result = task();
        promise->set_value(result);
        });

    return future;
}

std::string TaskManager::executeDBQuery(const std::string& query)
{
    std::cout << "DB �����忡�� ���� ���� ��: " << query << std::endl;
    boost::this_thread::sleep(boost::posix_time::seconds(2));  // DB ���� �ùķ��̼�
    return "���� ���";  // �����δ� ���⼭ �����ͺ��̽� ������ �����ϰ� ����� ��ȯ�ؾ� �մϴ�
}

void TaskManager::addFunctionTask(std::function<void()> task)
{
    thread_pool_.post(task);
}


void TaskManager::addTimerTask(std::function<void()> task, int milliseconds)
{
    boost::shared_ptr<boost::asio::deadline_timer> timer(
        new boost::asio::deadline_timer(thread_pool_.get_io_service(),
            boost::posix_time::milliseconds(milliseconds)));

    timer->async_wait([this, task, timer](const boost::system::error_code& ec) {
        if (!ec) {
            task();
        }
        });
}

ThreadPool::ThreadPool(size_t thread_count) : work_(boost::asio::make_work_guard(io_service_)) 
{
    for (size_t i = 0; i < thread_count; ++i) {
        threads_.create_thread(boost::bind(&boost::asio::io_service::run, &io_service_));
    }
}

ThreadPool::~ThreadPool()
{
    io_service_.stop();
    threads_.join_all();
}

boost::asio::io_service& ThreadPool::get_io_service()
{
    return io_service_;
}


#ifndef __TS_RNE_THREAD_SAFE_QUEUE_HPP__
#define __TS_RNE_THREAD_SAFE_QUEUE_HPP__
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() {}
    ~ThreadSafeQueue() {}

    void push(T new_data) {
        std::lock_guard<std::mutex> lk(_mtx);

        _queue.push(std::move(new_data));

        _cv.notify_one();
    }

    void wait_and_pop(T& val) {
        std::unique_lock<std::mutex> lk(_mtx);

        while (_queue.empty()) {
            _cv.wait(lk);
        }

        val = std::move(_queue.front());
        _queue.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lk(_mtx);

        while (_queue.empty()) {
            _cv.wait(lk);
        }

        std::shared_ptr<T> val(std::make_shared<T>(std::move(_queue.front())));
        _queue.pop();

        return val;
    }

    bool try_pop(T& val) {
        std::lock_guard<std::mutex> lk(_mtx);

        if (_queue.empty())
            return false;

        val = std::move(_queue.front());
        _queue.pop();

        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::shared_ptr<T> val;
        std::lock_guard<std::mutex> lk(_mtx);

        if (_queue.empty())
            return val;

        val = std::make_shared<T>(std::move(_queue.front()));
        _queue.pop();

        return val;
    }

    bool empty() {
        std::lock_guard<std::mutex> lk(_mtx);
        return _queue.empty();
    }

private:
    std::queue<T> _queue;
    std::mutex _mtx;
    std::condition_variable _cv;
};

#endif /* __TS_RNE_THREAD_SAFE_QUEUE_HPP__ */

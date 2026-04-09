#ifndef _RNE_NN_THREAD_HPP_
#define _RNE_NN_THREAD_HPP_
#include <thread>
#include <list>
#include <mutex>
#include <algorithm>
#include "ts_rne_c_api.h"
#include "ts_rne_nn_input.h"
#include "ts_rne_thread_safe_queue.hpp"

//singleton
class RneNNThread {
public:
    static RneNNThread *get_instance() {
        static RneNNThread _it;
        return &_it;
    }

    int start() {
        _isExit = false;
        if (!_thd.joinable()) {
            _thd = std::thread(&RneNNThread::thread_proc, this);
        }
        return 0;
    }

    int end() {
        _ready.push(NULL);
        _isExit = true;
        if (_thd.joinable()) {
            _thd.join();
        }
        return 0;
    }

    int push(RNE_ASYNC_EXECUTOR *asyncEtr) {
        _ready.push(asyncEtr);
        return 0;
    }

    enum State {
        RUNNING,
        DONE,
        ERROR
    };

    State query(RNE_ASYNC_EXECUTOR *asyncEtr) {
        State st = ERROR;
        std::lock_guard<std::mutex> lck(_mtx);
        auto it = std::find(_done.begin(), _done.end(), asyncEtr);
        if (it != _done.end()) {
            _done.erase(it);
            st = DONE;
        } else {
            st = RUNNING;
        }
        return st;
    }

private:
    RneNNThread() = default;
    ~RneNNThread() = default;
    RneNNThread(RneNNThread &other) = delete;
    void operator = (RneNNThread &other) = delete;

private:
    void get_ready() {
        RNE_ASYNC_EXECUTOR *asyncEtr = NULL;

        if (_run.empty()) {
            _ready.wait_and_pop(asyncEtr);
        } else {
            _ready.try_pop(asyncEtr);
        }
        if (!asyncEtr) return;

        auto it = _run.begin();
        while (it != _run.end()) {
            if (asyncEtr->priority < (*it)->priority) {
                break;
            }
            ++it;
        }
        _run.insert(it, asyncEtr);
        asyncEtr->state = RNE_NN_FORWARD_INIT;
    }

    void push_done(RNE_ASYNC_EXECUTOR *asyncEtr) {
        std::lock_guard<std::mutex> lck(_mtx);
        _done.push_back(asyncEtr);
    }

    int thread_proc() {
        while (!_isExit) {
            get_ready();

            int times = 2;
            do {
                for (auto it = _run.begin(); it != _run.end();) {
                    int highestPriority = (*_run.begin())->priority;
                    RNE_ASYNC_EXECUTOR *asyncEtr = *it;

                    TS_MPI_TRP_RNE_ForwardAsync(asyncEtr);

                    switch (asyncEtr->state) {
                    case RNE_NN_FORWARD_FEED: {
                        asyncEtr->state = RNE_NN_FORWARD_READY;
                        TS_MPI_TRP_RNE_FillInputBlobs(asyncEtr->net,
                                                      asyncEtr->coreIdx[0],
                                                      asyncEtr->netIn);
                        break;
                    }
                    case RNE_NN_FORWARD_DONE:
                        it = _run.erase(it);
                        push_done(asyncEtr);
                        continue;
                    case RNE_NN_FORWARD_RUNNING:
                        if (asyncEtr->priority > highestPriority) {
                            asyncEtr->state = RNE_NN_FORWARD_SUSPEND;
                            continue;
                        }
                        break;
                    case RNE_NN_FORWARD_WAIT:
                        if (asyncEtr->priority == highestPriority) {
                            asyncEtr->state = RNE_NN_FORWARD_RESUME;
                            continue;
                        }
                        break;
                    default:
                        break;
                    }
                    ++it;
                }
            } while (--times);
        }
        return 0;
    }

private:
    std::thread _thd;
    ThreadSafeQueue<RNE_ASYNC_EXECUTOR *> _ready;
    std::list<RNE_ASYNC_EXECUTOR *> _run;
    std::list<RNE_ASYNC_EXECUTOR *> _done;
    std::mutex _mtx;
    volatile bool _isExit;
};

#endif /* __TS_RNE_NN_THREAD_HPP__ */

#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <thread>
#include <chrono>

#include "SyncQueue.hpp"

const int MaxTaskCount = 100;

class ThreadPool {
public:
  using Task = std::function<void()>;
  ThreadPool(int numThreads = std::thread::hardware_concurrency())
      : m_queue(MaxTaskCount) {
    cout << numThreads << " threads" << endl;
    Start(numThreads);
  }

  ~ThreadPool(void) { Stop(); }

  void Stop() {
    std::call_once(m_flag, [this] { StopThreadGroup(); });
  }

  void AddTask(Task &&task) { m_queue.Put(std::forward<Task>(task)); }

  void AddTask(const Task &task) { m_queue.Put(task); }

private:
  void Start(int numThreads) {
    m_running = true;

    for (int i = 0; i < numThreads; ++i) {
      m_threadgroup.push_back(
          std::make_shared<std::thread>(&ThreadPool::RunInThread, this));
    }
  }

  void RunInThread() {
    while (m_running) {
      std::list<Task> list;
      m_queue.Take(list);
      for (auto &task : list) {
        if (!m_running)
          return;
        task();
      }
    }
  }

  void StopThreadGroup() {
    m_queue.Stop();
    m_running = false;
    for (auto thread : m_threadgroup) {
      if (thread)
        thread->join();
    }
    m_threadgroup.clear();
  }

  std::list<std::shared_ptr<std::thread>> m_threadgroup;
  SyncQueue<Task> m_queue;
  atomic_bool m_running;
  std::once_flag m_flag;
};

void TestThreadPool()
{
  ThreadPool pool;

  std::thread thd1([&pool] {
    for (int i = 0; i < 10; i++) {
      auto thdId = this_thread::get_id();
      pool.AddTask([thdId] { cout << "sync thread 1 Id : " << thdId << endl; });
    }
  });

  std::thread thd2([&pool] {
    for (int i = 0; i < 10; i++) {
      auto thdId = this_thread::get_id();
      pool.AddTask([thdId] { cout << "sync thread 2 Id : " << thdId << endl; });
    }
  });

  this_thread::sleep_for(std::chrono::seconds(2));
  getchar();
  pool.Stop();
  thd1.join();
  thd2.join();
}

int main(){
  TestThreadPool();
  return 0;
}


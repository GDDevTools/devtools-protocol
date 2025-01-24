#include <Geode/utils/Task.hpp>
#include <queue>

template <std::move_constructible T, std::move_constructible P = std::monostate>
class [[nodiscard]] Future final {
  using Task = geode::Task<T,P>;
  Task m_task;
  geode::EventListener<Task> m_taskListener;

  std::queue<std::function<void(T*)>> m_listeners;
public:
  Future(Future::Task task) : m_task(task) {
    m_taskListener.bind([this](Task::Event* event) {
      if (T* result = event->getValue()) {
        while (m_listeners.empty()) {
          m_listeners.back()(result);
          m_listeners.pop();
        }
      }
    });
    m_taskListener.setFilter(m_task);
  };

  void addListener(decltype(m_listeners)::value_type listener) {
    m_listeners.push(listener);
  }
};
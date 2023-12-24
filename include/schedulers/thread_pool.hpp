#pragma once

#include "generator/thread_local_random_double_generator.hpp"
#include <condition_variable>
#include <deque>
#include <mutex>
#include <stdexec/execution.hpp>
#include <thread>

namespace mrl {
class thread_pool;

namespace __thread_pool_details {
struct task_base {
  void (*execute)(task_base *) noexcept;
};

class notification_queue {
  std::deque<task_base *> _q;
  std::mutex _mutex;
  bool _done{false};
  std::condition_variable _ready;

public:
  bool try_pop(task_base *&x) {
    std::unique_lock lock{_mutex, std::try_to_lock};
    if (!lock || _q.empty())
      return false;
    x = std::move(_q.front());
    _q.pop_front();
    return true;
  }

  template <typename F> bool try_push(F &&f) {
    {
      std::unique_lock lock{_mutex, std::try_to_lock};
      if (!lock)
        return false;
      _q.emplace_back(std::forward<F>(f));
    }
    _ready.notify_one();
    return true;
  }

  void done() {
    {
      std::unique_lock lock{_mutex};
      _done = true;
    }
    _ready.notify_all();
  }

  bool pop(task_base *&x) {
    std::unique_lock lock{_mutex};
    while (_q.empty() && !_done) {
      _ready.wait(lock);
    }
    if (_q.empty())
      return false;
    x = std::move(_q.front());
    _q.pop_front();
    return true;
  }

  template <typename F> void push(F &&f) {
    {
      std::unique_lock lock{_mutex};
      _q.emplace_back(std::forward<F>(f));
    }
    _ready.notify_one();
  }
};

template <typename ReceiverId> struct operation;
} // namespace __thread_pool_details

struct thread_pool_scheduler {
  using __t = thread_pool_scheduler;
  using __id = thread_pool_scheduler;
  bool operator==(thread_pool_scheduler const &) const = default;

  // TODO: Write the domain
  struct domain {};

  struct sender {
    using __t = sender;
    using __id = sender;
    using sender_concept = stdexec::sender_t;
    using completion_signatures =
        stdexec::completion_signatures<stdexec::set_value_t(),
                                       stdexec::set_stopped_t()>;
    template <typename Receiver>
    auto make_operation(Receiver r) const
        -> __thread_pool_details::operation<stdexec::__id<Receiver>> {
      return __thread_pool_details::operation<stdexec::__id<Receiver>>(
          pool, std::move(r));
    }

    template <stdexec::receiver Receiver>
    friend auto tag_invoke(stdexec::connect_t, sender s, Receiver r)
        -> __thread_pool_details::operation<stdexec::__id<Receiver>> {
      return s.make_operation(std::move(r));
    }
    struct env {
      thread_pool *pool;

      template <typename CPO>
      friend thread_pool_scheduler
      tag_invoke(stdexec::get_completion_scheduler_t<CPO>,
                 env const &self) noexcept {
        return self.make_scheduler();
      }

      auto make_scheduler() const -> thread_pool_scheduler {
        return thread_pool_scheduler{pool};
      }
    };

    friend env tag_invoke(stdexec::get_env_t, sender const &self) noexcept {
      return env{self.pool};
    }

    thread_pool *pool;
  };

  sender make_sender() const { return sender{pool}; }

  friend sender tag_invoke(stdexec::schedule_t,
                           thread_pool_scheduler const &s) noexcept {
    return s.make_sender();
  }

  friend domain tag_invoke(stdexec::get_domain_t,
                           thread_pool_scheduler) noexcept {
    return {};
  }

  friend stdexec::forward_progress_guarantee
  tag_invoke(stdexec::get_forward_progress_guarantee_t,
             thread_pool const &) noexcept {
    return stdexec::forward_progress_guarantee::parallel;
  }

  thread_pool *pool;
};

class thread_pool {
  unsigned _count;
  std::vector<std::thread> _threads;
  std::vector<__thread_pool_details::notification_queue> _q;
  std::atomic<unsigned> _index{0};
  unsigned _retry_factor;

  void run(unsigned i) {
    while (true) {
      __thread_pool_details::task_base *f = nullptr;
      for (unsigned n = 0; n != _count; ++n) {
        if (_q[(i + n) % _count].try_pop(f))
          break;
      }
      if (f == nullptr && !_q[i].pop(f))
        break;
      f->execute(f);
    }
  }

public:
  thread_pool(unsigned num_threads, unsigned retry_factor = 1)
      : _count(num_threads), _q(num_threads), _retry_factor(retry_factor) {
    for (unsigned n = 0; n != num_threads; ++n) {
      _threads.emplace_back([&, n] { run(n); });
    }
  }

  ~thread_pool() {
    for (auto &e : _q)
      e.done();
    for (auto &e : _threads)
      e.join();
  }

  template <typename F> void submit(F &&f) {
    auto i = _index++;
    for (unsigned n = 0; n != _count * _retry_factor; ++n) {
      if (_q[(i + n) % _count].try_push(std::forward<F>(f)))
        return;
    }
    _q[i % _count].push(std::forward<F>(f));
  }

  auto get_scheduler() { return thread_pool_scheduler{this}; }
};

template <typename ReceiverId>
struct __thread_pool_details::operation
    : public __thread_pool_details::task_base {
  using Receiver = stdexec::__t<ReceiverId>;
  thread_pool &pool;
  Receiver receiver;

  operation(thread_pool *pool_arg, Receiver r)
      : pool(*pool_arg), receiver(std::move(r)) {
    this->execute = [](__thread_pool_details::task_base *t) noexcept {
      auto &op = *static_cast<operation *>(t);
      auto stoken = stdexec::get_stop_token(stdexec::get_env(op.receiver));
      if constexpr (std::unstoppable_token<decltype(stoken)>) {
        stdexec::set_value(std::move(op.receiver));
      } else if (stoken.stop_requested()) {
        stdexec::set_stopped(std::move(op.receiver));
      } else {
        stdexec::set_value(std::move(op.receiver));
      }
    };
  }

  void enqueue(task_base *op) const { pool.submit(op); }

  friend void tag_invoke(stdexec::start_t, operation &op) noexcept {
    op.enqueue(&op);
  }
};

inline auto random_generator(mrl::thread_pool_scheduler) {
  return mrl::thread_local_random_double_generator{};
}
} // namespace mrl

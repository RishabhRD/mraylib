#pragma once

#include "generator/thread_local_random_double_generator.hpp"
#include <atomic>
#include <condition_variable>
#include <deque>
#include <exception>
#include <exec/async_scope.hpp>
#include <mutex>
#include <stdexec/__detail/__execution_fwd.hpp>
#include <stdexec/execution.hpp>
#include <thread>

namespace mrl {
class thread_pool;

namespace __thread_pool_details {
template <class> struct not_a_sender {
  using sender_concept = stdexec::sender_t;
};

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

namespace __thread_pool_bulk {
template <class Fun, class Shape, class... Args>
  requires stdexec::__callable<Fun, Shape, Args &...>
using bulk_non_throwing = //
    stdexec::__mbool<stdexec::__nothrow_callable<Fun, Shape, Args &...> &&
                     noexcept(stdexec::__decayed_tuple<Args...>(
                         std::declval<Args>()...))>;

template <class SenderId, class ReceiverId, std::integral Shape, class Fun>
struct op_state;

template <class SenderId, std::integral Shape, class FunId> struct sender {
  using Sender = stdexec::__t<SenderId>;
  using Fun = stdexec::__t<FunId>;
  using sender_concept = stdexec::sender_t;

  thread_pool *pool_;
  Sender sndr_;
  Shape shape_;
  Fun fun_;

  template <class Fun, class Sender, class Env>
  using with_error_invoke_t = //
      stdexec::__if_c<
          stdexec::__v<stdexec::__value_types_of_t<
              Sender, Env,
              stdexec::__mbind_front_q<bulk_non_throwing, Fun, Shape>,
              stdexec::__q<stdexec::__mand>>>,
          stdexec::completion_signatures<>, stdexec::__with_exception_ptr>;

  template <class... Tys>
  using set_value_t = stdexec::completion_signatures<stdexec::set_value_t(
      stdexec::__decay_t<Tys>...)>;

  template <class Self, class Env>
  using completion_signatures = //
      stdexec::__try_make_completion_signatures<
          stdexec::__copy_cvref_t<Self, Sender>, Env,
          with_error_invoke_t<Fun, stdexec::__copy_cvref_t<Self, Sender>, Env>,
          stdexec::__q<set_value_t>>;

  template <class Self, class Receiver>
  using bulk_op_state_t = //
      op_state<stdexec::__x<stdexec::__copy_cvref_t<Self, Sender>>,
               stdexec::__x<stdexec::__decay_t<Receiver>>, Shape, Fun>;

  template <stdexec::__decays_to<sender> Self, stdexec::receiver Receiver>
    requires stdexec::receiver_of<
        Receiver, completion_signatures<Self, stdexec::env_of_t<Receiver>>>
  friend bulk_op_state_t<Self, Receiver>                       //
  tag_invoke(stdexec::connect_t, Self &&self, Receiver &&rcvr) //
      noexcept(stdexec::__nothrow_constructible_from<
               bulk_op_state_t<Self, Receiver>, thread_pool &, Shape, Fun,
               Sender, Receiver>) {
    return bulk_op_state_t<Self, Receiver>{*self.pool_, self.shape_, self.fun_,
                                           (std::forward<Self>(self)).sndr_,
                                           std::move(rcvr)};
  }

  template <stdexec::__decays_to<sender> Self, class Env>
  friend auto tag_invoke(stdexec::get_completion_signatures_t, Self &&, Env &&)
      -> completion_signatures<Self, Env> {
    return {};
  }

  friend auto tag_invoke(stdexec::get_env_t, const sender &self) noexcept
      -> stdexec::env_of_t<const Sender &> {
    return stdexec::get_env(self.sndr_);
  }
};
template <stdexec::sender Sender, std::integral Shape, class Fun>
using bulk_sender_t = //
    sender<stdexec::__x<stdexec::__decay_t<Sender>>, Shape,
           stdexec::__x<stdexec::__decay_t<Fun>>>;
} // namespace __thread_pool_bulk

struct thread_pool_scheduler {
  using __t = thread_pool_scheduler;
  using __id = thread_pool_scheduler;
  bool operator==(thread_pool_scheduler const &) const = default;

  struct transform_bulk {
    template <class Data, class Sender>
    auto operator()(stdexec::bulk_t, Data &&data, Sender &&sndr) {
      auto [shape, fun] = std::forward<Data>(data);
      return __thread_pool_bulk::bulk_sender_t<Sender, decltype(shape),
                                               decltype(fun)>{
          &pool_, std::forward<Sender>(sndr), shape, std::move(fun)};
    }

    thread_pool &pool_;
  };

  struct domain {
    template <stdexec::sender_expr_for<stdexec::bulk_t> Sender>
    auto transform_sender(Sender &&sndr) const noexcept {
      if constexpr (stdexec::__completes_on<Sender, thread_pool_scheduler>) {
        auto sched = stdexec::get_completion_scheduler<stdexec::set_value_t>(
            stdexec::get_env(sndr));
        return stdexec::__sexpr_apply(std::forward<Sender>(sndr),
                                      transform_bulk{*sched.pool});
      } else {
        static_assert(
            stdexec::__completes_on<Sender, thread_pool_scheduler>,
            "No static_thread_pool instance can be found in the sender's "
            "environment "
            "on which to schedule bulk work.");
        return __thread_pool_details::not_a_sender<
            stdexec::__name_of<Sender>>();
      }
      STDEXEC_UNREACHABLE();
    }

    // transform the generic bulk sender into a parallel thread-pool bulk sender
    template <stdexec::sender_expr_for<stdexec::bulk_t> Sender, class Env>
    auto transform_sender(Sender &&sndr, const Env &env) const noexcept {
      if constexpr (stdexec::__completes_on<Sender, thread_pool_scheduler>) {
        auto sched = stdexec::get_completion_scheduler<stdexec::set_value_t>(
            stdexec::get_env(sndr));
        return stdexec::__sexpr_apply(std::forward<Sender>(sndr),
                                      transform_bulk{*sched.pool});
      } else if constexpr (stdexec::__starts_on<Sender, thread_pool_scheduler,
                                                Env>) {
        auto sched = stdexec::get_scheduler(env);
        return stdexec::__sexpr_apply(std::forward<Sender>(sndr),
                                      transform_bulk{*sched.pool});
      } else {
        static_assert( //
            stdexec::__starts_on<Sender, thread_pool_scheduler, Env> ||
                stdexec::__completes_on<Sender, thread_pool_scheduler>,
            "No static_thread_pool instance can be found in the sender's or "
            "receiver's "
            "environment on which to schedule bulk work.");
        return __thread_pool_details::not_a_sender<
            stdexec::__name_of<Sender>>();
      }
      STDEXEC_UNREACHABLE();
    }
  };

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

  void submit(__thread_pool_details::task_base *f) {
    auto i = _index++;
    for (unsigned n = 0; n != _count * _retry_factor; ++n) {
      if (_q[(i + n) % _count].try_push(f))
        return;
    }
    _q[i % _count].push(f);
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

namespace __thread_pool_bulk {

template <typename SenderId, typename ReceiverId, typename Shape, typename Fn,
          bool MayThrow>
struct bulk_shared_state {
  using Sender = stdexec::__t<SenderId>;
  using Receiver = stdexec::__t<ReceiverId>;
  using variant_t =
      stdexec::__value_types_of_t<Sender, stdexec::env_of_t<Receiver>,
                                  stdexec::__q<stdexec::__decayed_tuple>,
                                  stdexec::__q<stdexec::__variant>>;

  variant_t data;
  Receiver receiver;
  std::uint32_t shape;
  Fn fn;

  std::atomic<std::uint32_t> finished_tasks{0};
  std::atomic<std::uint32_t> task_with_exception{shape};
  std::exception_ptr exception;

  template <class F> void apply(F f) {
    std::visit(
        [&](auto &tupl) -> void {
          std::apply([&](auto &...args) -> void { f(args...); }, tupl);
        },
        data);
  }

  bulk_shared_state(Receiver receiver_, Shape shape_, Fn fn_)
      : receiver{std::move(receiver_)},
        shape{static_cast<std::uint32_t>(shape_)}, fn{fn_} {}

  struct bulk_task : public __thread_pool_details::task_base {
    bulk_shared_state *sh_state_;
    std::uint32_t task_id_;

    bulk_task(bulk_shared_state *sh_state_arg, std::uint32_t task_id_arg)
        : sh_state_{sh_state_arg}, task_id_{task_id_arg} {
      this->execute = [](task_base *t) noexcept {
        auto &self = *static_cast<bulk_task *>(t);
        auto &sh_state = *self.sh_state_;
        auto task_id = self.task_id_;

        auto computation = [&sh_state, task_id](auto &...args) {
          sh_state.fn(static_cast<Shape>(task_id), args...);
        };
        auto completion = [&sh_state](auto... args) {
          stdexec::set_value(std::move(sh_state.receiver), std::move(args)...);
        };

        if constexpr (MayThrow) {
          try {
            sh_state.apply(computation);
          } catch (...) {
            std::uint32_t expected = sh_state.shape;

            if (sh_state.task_with_exception.compare_exchange_strong(
                    expected, task_id, std::memory_order_relaxed,
                    std::memory_order_relaxed)) {
              sh_state.exception = std::current_exception();
            }
          }
          auto const is_last_thread =
              sh_state.finished_tasks.fetch_add(1) == (sh_state.shape - 1);
          if (is_last_thread) {
            if (sh_state.exception) {
              stdexec::set_error(std::move(sh_state.receiver),
                                 std::move(sh_state.exception));
            } else {
              sh_state.apply(completion);
            }
          }
        } else {
          sh_state.apply(computation);
          auto const is_last_thread =
              sh_state.finished_tasks.fetch_add(1) == (sh_state.shape - 1);
          if (is_last_thread)
            sh_state.apply(completion);
        }
      };
    }
  };

  std::vector<bulk_task> tasks;
};

template <class SenderId, class ReceiverId, class Shape, class Fn,
          bool MayThrow>
struct receiver {
  using receiver_concept = stdexec::receiver_t;
  using Sender = stdexec::__t<SenderId>;
  using Receiver = stdexec::__t<ReceiverId>;

  using shared_state =
      bulk_shared_state<SenderId, ReceiverId, Shape, Fn, MayThrow>;

  shared_state &shared_state_;
  thread_pool &pool_;

  void enqueue() noexcept {
    using bulk_task = typename shared_state::bulk_task;
    shared_state_.tasks.reserve(shared_state_.shape);
    for (std::uint32_t i{}; i != shared_state_.shape; ++i) {
      shared_state_.tasks.push_back(bulk_task(&shared_state_, i));
    }
    for (auto &task : shared_state_.tasks) {
      pool_.submit(&task);
    }
  }

  template <class... As>
  friend void tag_invoke(stdexec::same_as<stdexec::set_value_t> auto,
                         receiver &&self, As &&...as) noexcept {
    using tuple_t = stdexec::__decayed_tuple<As...>;
    shared_state &state = self.shared_state_;

    if constexpr (MayThrow) {
      try {
        state.data.template emplace<tuple_t>(std::forward<As>(as)...);
      } catch (...) {
        stdexec::set_error(std::move(state.receiver), std::current_exception());
      }
    } else {
      state.data.template emplace<tuple_t>(std::forward<As>(as)...);
    }

    if (state.shape) {
      self.enqueue();
    } else {
      state.apply([&](auto &...args) {
        stdexec::set_value(std::move(state.receiver), std::move(args)...);
      });
    }
  }
  template <stdexec::__one_of<stdexec::set_error_t, stdexec::set_stopped_t> Tag,
            class... As>
  friend void tag_invoke(Tag tag, receiver &&self, As &&...as) noexcept {
    shared_state &state = self.shared_state_;
    tag(std::forward<receiver>(state.receiver_), std::forward<As>(as)...);
  }

  friend auto tag_invoke(stdexec::get_env_t, receiver const &self) noexcept
      -> stdexec::env_of_t<Receiver> {
    return stdexec::get_env(self.shared_state_.receiver);
  }
};

template <class SenderId, class ReceiverId, std::integral Shape, class Fun>
struct op_state {
  using Sender = stdexec::__t<SenderId>;
  using Receiver = stdexec::__t<ReceiverId>;

  static constexpr bool may_throw = //
      !stdexec::__v<stdexec::__value_types_of_t<
          Sender, stdexec::env_of_t<Receiver>,
          stdexec::__mbind_front_q<bulk_non_throwing, Fun, Shape>,
          stdexec::__q<stdexec::__mand>>>;

  using bulk_rcvr = receiver<SenderId, ReceiverId, Shape, Fun, may_throw>;
  using shared_state =
      bulk_shared_state<SenderId, ReceiverId, Shape, Fun, may_throw>;
  using inner_op_state = stdexec::connect_result_t<Sender, bulk_rcvr>;

  shared_state shared_state_;

  inner_op_state inner_op_;

  friend void tag_invoke(stdexec::start_t, op_state &op) noexcept {
    stdexec::start(op.inner_op_);
  }

  op_state(thread_pool &pool, Shape shape, Fun fn, Sender &&sender,
           Receiver receiver)
      : shared_state_(std::move(receiver), shape, fn),
        inner_op_{stdexec::connect(std::forward<Sender>(sender),
                                   bulk_rcvr{shared_state_, pool})} {}
};

} // namespace __thread_pool_bulk
} // namespace mrl

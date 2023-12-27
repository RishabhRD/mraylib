#pragma once

#include "stdexec/execution.hpp"
#include <dispatch/dispatch.h>

namespace mrl {
namespace __libdispatch_details {
struct task_base {
  void (*execute)(task_base *) noexcept;
};

template <typename ReceiverId> struct operation;
} // namespace __libdispatch_details

struct libdispatch_queue;

struct libdispatch_scheduler {
  using __t = libdispatch_scheduler;
  using __id = libdispatch_scheduler;

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
        -> __libdispatch_details::operation<stdexec::__id<Receiver>> {
      return __libdispatch_details::operation<stdexec::__id<Receiver>>(
          pool, std::move(r));
    }

    template <stdexec::receiver Receiver>
    friend auto tag_invoke(stdexec::connect_t, sender s, Receiver r)
        -> __libdispatch_details::operation<stdexec::__id<Receiver>> {
      return s.make_operation(std::move(r));
    }
    struct env {
      libdispatch_queue *pool;

      template <typename CPO>
      friend libdispatch_scheduler
      tag_invoke(stdexec::get_completion_scheduler_t<CPO>,
                 env const &self) noexcept {
        return self.make_scheduler();
      }

      auto make_scheduler() const -> libdispatch_scheduler {
        return libdispatch_scheduler{pool};
      }
    };

    friend env tag_invoke(stdexec::get_env_t, sender const &self) noexcept {
      return env{self.pool};
    }

    libdispatch_queue *pool;
  };

  sender make_sender() const { return sender{pool}; }

  friend sender tag_invoke(stdexec::schedule_t,
                           libdispatch_scheduler const &s) noexcept {
    return s.make_sender();
  }

  friend domain tag_invoke(stdexec::get_domain_t,
                           libdispatch_scheduler) noexcept {
    return {};
  }

  friend stdexec::forward_progress_guarantee
  tag_invoke(stdexec::get_forward_progress_guarantee_t,
             libdispatch_queue const &) noexcept {
    return stdexec::forward_progress_guarantee::parallel;
  }

  libdispatch_queue *pool;
};

struct libdispatch_queue {
  bool operator==(libdispatch_queue const &) const = default;

  void submit(__libdispatch_details::task_base *f) {
    auto queue = dispatch_get_global_queue(priority, 0);
    dispatch_async_f(queue, f,
                     reinterpret_cast<void (*)(void *) noexcept>(f->execute));
  }

  auto get_scheduler() { return libdispatch_scheduler{this}; }

  int priority{DISPATCH_QUEUE_PRIORITY_DEFAULT};
};

template <typename ReceiverId>
struct __libdispatch_details::operation
    : public __libdispatch_details::task_base {
  using Receiver = stdexec::__t<ReceiverId>;
  libdispatch_queue &pool;
  Receiver receiver;

  operation(libdispatch_queue *pool_arg, Receiver r)
      : pool(*pool_arg), receiver(std::move(r)) {
    this->execute = [](__libdispatch_details::task_base *t) noexcept {
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
} // namespace mrl

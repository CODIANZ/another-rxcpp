template <typename F> auto flat_map(F f) const;
template <typename F> auto map(F f) const;
auto take(std::size_t n) const;
auto publish() const;
template <typename F> auto on_error_resume_next(F f) const;
auto retry(std::size_t n = 0) const;
auto observe_on(scheduler::creator_fn sccr) const;
auto subscribe_on(scheduler::creator_fn sccr) const;
template <typename OB> auto take_until(OB ob) const;
template <typename...ARGS> auto merge(ARGS...args) const;
template <typename...ARGS> auto amb(ARGS...args) const;
auto distinct_until_changed() const;
auto tap(
  std::function<void(value_type)>         on_next       = {},
  std::function<void(std::exception_ptr)> on_error      = {},
  std::function<void()>                   on_completed  = {}
) const;
auto timeout(std::chrono::milliseconds d) const;
auto delay(std::chrono::milliseconds d) const;
auto last() const;
auto finally(std::function<void()> on_finally) const;
auto take_last(std::size_t n) const;

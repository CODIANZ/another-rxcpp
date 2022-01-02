class scheduler;
template <typename F> auto flat_map(F f) -> decltype(f(std::declval<value_type>()));
template <typename F> auto map(F f) -> observable<decltype(f(std::declval<value_type>()))>;
auto take(std::size_t n) -> observable<value_type>;
auto publish() -> observable<value_type>;
template <typename F> auto on_error_resume_next(F f) -> observable<value_type>;
auto retry() -> observable<value_type>;
auto observe_on(scheduler scl) -> observable<value_type>;
auto subscribe_on(scheduler scl) -> observable<value_type>;

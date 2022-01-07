template <typename F, typename...ARGS> auto flat_map(F, ARGS...) const;
template <typename F, typename...ARGS> auto map(F, ARGS...) const;
template <typename F, typename...ARGS> auto tap(F, ARGS...) const;

template <typename...ARGS> auto take(ARGS...) const;
template <typename...ARGS> auto on_error_resume_next(ARGS...) const;
template <typename...ARGS> auto observe_on(ARGS...) const;
template <typename...ARGS> auto subscribe_on(ARGS...) const;
template <typename...ARGS> auto take_last(ARGS...) const;
template <typename...ARGS> auto take_until(ARGS...) const;
template <typename...ARGS> auto merge(ARGS...) const;
template <typename...ARGS> auto amb(ARGS...) const;
template <typename...ARGS> auto timeout(ARGS...) const;
template <typename...ARGS> auto delay(ARGS...) const;
template <typename...ARGS> auto finally(ARGS...) const;

auto publish() const;
auto retry() const;
auto distinct_until_changed() const;
auto last() const;

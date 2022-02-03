template <typename F, typename...ARGS> auto flat_map(F&&, ARGS&&...) const noexcept;
template <typename F, typename...ARGS> auto map(F&&, ARGS&&...) const noexcept;
template <typename F, typename...ARGS> auto tap(F&&, ARGS&&...) const noexcept;

template <typename OB, typename...ARGS, std::enable_if_t<is_observable<OB>::value, bool> = true>
      auto zip(OB, ARGS...) const noexcept;
template <typename F, typename...ARGS, std::enable_if_t<!is_observable<F>::value, bool> = true>
      auto zip(F, ARGS...) const noexcept;

template <typename...ARGS> auto take(ARGS&&...) const noexcept;
template <typename...ARGS> auto on_error_resume_next(ARGS&&...) const noexcept;
template <typename...ARGS> auto observe_on(ARGS&&...) const noexcept;
template <typename...ARGS> auto retry(ARGS&&...) const noexcept;
template <typename...ARGS> auto subscribe_on(ARGS&&...) const noexcept;
template <typename...ARGS> auto skip_until(ARGS&&...) const noexcept;
template <typename...ARGS> auto skip_while(ARGS&&...) const noexcept;
template <typename...ARGS> auto take_last(ARGS&&...) const noexcept;
template <typename...ARGS> auto take_until(ARGS&&...) const noexcept;
template <typename...ARGS> auto take_while(ARGS&&...) const noexcept;
template <typename...ARGS> auto merge(ARGS&&...) const noexcept;
template <typename...ARGS> auto amb(ARGS&&...) const noexcept;
template <typename...ARGS> auto timeout(ARGS&&...) const noexcept;
template <typename...ARGS> auto delay(ARGS&&...) const noexcept;
template <typename...ARGS> auto finally(ARGS&&...) const noexcept;
template <typename...ARGS> auto filter(ARGS&&...) const noexcept;
template <typename...ARGS> auto first(ARGS&&...) const noexcept;
      
auto publish() const noexcept;
auto as_blocking() const noexcept;
auto distinct_until_changed() const noexcept;
auto last() const noexcept;

template <typename...ARGS> static auto just(ARGS&&...args) noexcept;
template <typename T, typename...ARGS> static auto error(ARGS&&...args) noexcept;
template <typename T> static auto never() noexcept;
template <typename T> static auto empty() noexcept;
template <typename T, typename...ARGS> static auto range(T&&, ARGS&&...args) noexcept;
template <typename T = int, typename...ARGS> static auto interval(ARGS&&...args) noexcept;
template <typename...ARGS> static auto iterate(ARGS&&...args) noexcept;

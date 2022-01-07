template <typename...ARGS> static auto just(ARGS...args);
template <typename T, typename...ARGS> static auto error(ARGS...args);
template <typename T> static auto never();
template <typename T, typename...ARGS> static auto range(T, ARGS...args);
template <typename T = int, typename...ARGS> static auto interval(ARGS...args);
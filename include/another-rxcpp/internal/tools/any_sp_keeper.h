#if !defined(__another_rxcpp_h_any_sp_keeper__)
#define __another_rxcpp_h_any_sp_keeper__

#include <memory>
#include <vector>

namespace another_rxcpp { namespace internal {

class typed_sp_keeper_base {
public:
  using sp = std::shared_ptr<typed_sp_keeper_base>;
  virtual ~typed_sp_keeper_base() = default;
protected:
  typed_sp_keeper_base() = default;
};

template <typename T> class typed_sp_keeper;

class any_sp_keeper {
private:
  using sp = std::shared_ptr<any_sp_keeper>;
  using array_type = std::vector<typename typed_sp_keeper_base::sp>;
  array_type ptrs_;

  template <typename SP, typename ...ARGS> static void create_internal(array_type& ptrs, SP sp, ARGS...args) noexcept {
    using elem_type = typename SP::element_type;
    using keeper_type = typed_sp_keeper<elem_type>;
    ptrs.push_back(std::shared_ptr<keeper_type>(new keeper_type(sp)));
    create_internal(ptrs, args...);
  }

  template <typename SP> static void create_internal(array_type& ptrs, SP sp) noexcept {
    using elem_type = typename SP::element_type;
    using keeper_type = typed_sp_keeper<elem_type>;
    ptrs.push_back(std::shared_ptr<keeper_type>(new keeper_type(sp)));
  }

  static void create_internal(array_type&) noexcept {}

public:
  any_sp_keeper() = default;
  virtual ~any_sp_keeper() = default;

  operator bool () const noexcept { return ptrs_.size() != 0; }
  void clear() noexcept { ptrs_.clear(); }

  template <typename...ARGS> static any_sp_keeper create(ARGS...args) noexcept {
    any_sp_keeper keeper;
    create_internal(keeper.ptrs_, args...);
    return keeper;
  }
};

template <typename T> class typed_sp_keeper : public typed_sp_keeper_base {
friend class any_sp_keeper;
private:
  std::shared_ptr<T> p_;
  typed_sp_keeper(std::shared_ptr<T> p) noexcept : p_(p) {}
public:
  virtual ~typed_sp_keeper() = default;
};

}} /* namespace another_rxcpp::internal */

#endif /* !defined(__another_rxcpp_h_any_sp_keeper__) */

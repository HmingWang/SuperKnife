#pragma once
#include "define.h"

// 容器类型限定
template <typename T>
concept ContainerType = requires(T c) {
  typename T::value_type;
  typename T::iterator;
  typename T::const_iterator;
  // 支持range
  requires std::ranges::range<T>;
  // 必须有下列方法：
  { c.size() } -> std::convertible_to<typename T::size_type>;
  { c.empty() } -> std::convertible_to<bool>;
  { c.clear() } -> std::same_as<void>;
  { c.begin() } -> std::input_iterator;
  { c.end() } -> std::input_or_output_iterator;
};

// 序列容器概念（支持push_back）
template <typename C>
concept SequenceContainer =
    ContainerType<C> && requires(C c, typename C::value_type v) {
      { c.push_back(v) } -> std::same_as<void>;
      { c.pop_back() } -> std::same_as<void>;
    };

// 前插序列容器概念（支持push_front）
template <typename C>
concept FrontInsertionContainer =
    SequenceContainer<C> && requires(C c, typename C::value_type v) {
      { c.push_front(v) } -> std::same_as<void>;
      { c.pop_front() } -> std::same_as<void>;
    };

// 关联容器概念
template <typename C>
concept AssociativeContainer =
    ContainerType<C> && requires(C c, typename C::key_type k) {
      typename C::key_type;
      { c.find(k) } -> std::same_as<typename C::iterator>;
      { c.count(k) } -> std::convertible_to<typename C::size_type>;
    };

template <ContainerType T> class Container {
  using value_type = typename T::value_type;
  using reference = typename T::reference;
  using const_reference = typename T::const_reference;
  using iterator = typename T::iterator;
  using const_iterator = typename T::const_iterator;
  using size_type = typename T::size_type;
  // using key_type = typename T::key_type;

protected:
  T _data;

public:
  // 1. 默认构造函数
  Container() = default;
  // 2. 带参数的构造函数
  explicit Container(const T &c) : _data(c) {};
  explicit Container(T &&c) noexcept(std::is_nothrow_move_constructible_v<T>)
      : _data(std::move(c)) {}
  // 3. 从迭代器范围构造
  template <std::input_iterator InputIt>
  Container(InputIt first, InputIt last) : _data(first, last) {}
  // 4. 从range构造
  template <std::ranges::input_range R>
    requires std::convertible_to<std::ranges::range_value_t<R>, value_type>
  Container(R &&r) : _data(std::ranges::begin(r), std::ranges::end(r)) {}
  // 五/六法则实现
  // 5. 拷贝构造函数
  Container(const Container &) = default;
  // 6. 移动构造函数
  Container(Container &&) noexcept(std::is_nothrow_move_constructible_v<T>) =
      default;
  // 7. 拷贝赋值运算符
  Container &operator=(const Container &) = default;
  // 8. 移动赋值运算符
  Container &operator=(Container &&) noexcept(
      std::is_nothrow_move_assignable_v<T>) = default;
  // 9. 析构函数
  virtual ~Container() = default;
  // 10. 交换函数
  void swap(Container &other) noexcept(std::is_nothrow_swappable_v<T>) {
    using std::swap;
    swap(_data, other._data);
  }

  // 通用操作
  auto begin() { return _data.begin(); }
  auto end() { return _data.end(); }
  auto cbegin() const { return _data.cbegin(); }
  auto cend() const { return _data.cend(); }
  auto rbegin() { return _data.rbegin(); }
  auto rend() { return _data.rend(); }
  auto crbegin() const { return _data.crbegin(); }
  auto crend() const { return _data.crend(); }
  [[nodiscard]] bool empty() const { return _data.empty(); }
  [[nodiscard]] auto length() const { return _data.size(); };
  [[nodiscard]] auto size() const { return _data.size(); };
  auto resize(size_type size) { return _data.resize(size); };
  auto clear() { return _data.clear(); }
  auto reserve(size_type size) { return _data.reserve(size); }

  // 元素访问
  auto &operator[](size_type pos)
    requires requires { _data[pos]; }
  {
    return _data[pos];
  }
  const auto &operator[](size_type pos) const
    requires requires { _data[pos]; }
  {
    return _data[pos];
  }

  auto &at(size_type pos)
    requires requires { _data.at(pos); }
  {
    return _data.at(pos);
  }
  const auto &at(size_type pos) const
    requires requires { _data.at(pos); }
  {
    return _data.at(pos);
  }

  //// 获取底层容器
  const T &get() const & { return _data; }
  T &get() & { return _data; }
  T &&get() && { return std::move(_data); }
  auto data()
    requires requires { _data.data(); }
  {
    return _data.data();
  }
  // 获取c风格指针
  void *c_ptr()
    requires requires { _data.data(); }
  {
    return reinterpret_cast<void *>(_data.data());
  }
  // 获取c风格const指针
  const void *c_cptr() const
    requires requires { _data.data(); }
  {
    return reinterpret_cast<const void *>(_data.data());
  }

  // 视图操作
  auto view() & { return std::ranges::ref_view(_data); }
  auto view() const & { return std::ranges::ref_view(_data); }
  auto view() && { return std::ranges::owning_view(std::move(_data)); }

  // 比较操作
  bool operator==(const T &other) const { return _data == other._data; }
  bool operator!=(const T &other) const { return _data != other._data; }
};

// 特化 std::swap
template <ContainerType T>
void swap(Container<T> &lhs,
          Container<T> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
  lhs.swap(rhs);
}

// 使Container满足range概念
template <ContainerType T>
inline constexpr bool std::ranges::enable_borrowed_range<Container<T>> = true;

#ifndef TRIXY_LOCKER_VECTOR_HPP
#define TRIXY_LOCKER_VECTOR_HPP

#include <utility> // move, forward

#include <Trixy/Locker/Base.hpp>
#include <Trixy/Detail/TrixyMeta.hpp>

#include <Trixy/Detail/MacroScope.hpp>

namespace trixy
{

template <class Tensor>
using VectorLocker = Locker<Tensor, LockerType::vector>;

template <class Tensor>
class Locker<Tensor, LockerType::vector> : protected Tensor
{
protected:
    using require = Tensor;

public:
    using typename require::type;

    using typename require::size_type;
    using typename require::precision_type;

    using typename require::pointer;
    using typename require::const_pointer;

public:
    template <typename U = Tensor,
              typename = meta::when<std::is_constructible<U>::value>>
    Locker() : Tensor() {}

    // We MUST define explicit copy and move constructors
    // to prevent EATING args by constructor with perfect forwarding
    template <typename T, typename... Tn,
        typename = meta::when<not std::is_base_of<meta::decay_t<T>, Locker>::value>,
        typename = meta::when<std::is_constructible<Tensor, T, Tn...>::value>>
    explicit Locker(T&& t, Tn&&... tn)
    : Tensor(std::forward<T>(t), std::forward<Tn>(tn)...) {}

    // operator= for copy and move Locker object will not implicit generate
    Locker(const Locker& vector) : Tensor(vector) {}
    Locker(Locker&& vector) noexcept : Tensor(std::move(vector)) {}

    Locker(const Tensor& vector) : Tensor(vector) {}
    Locker(Tensor&& vector) noexcept : Tensor(std::move(vector)) {}

    ~Locker() = default;

    const Tensor& base() const noexcept
    { return static_cast<const Tensor&>(*this); }

public:
    using require::operator();

    using require::copy;
    using require::size;

    using require::fill;
    using require::apply;

    using require::dot;

    using require::add;
    using require::sub;
    using require::mul;
    using require::join;

    using require::data;
};

} // namespace trixy

#include <Trixy/Detail/MacroUnscope.hpp>

#endif // TRIXY_LOCKER_VECTOR_HPP

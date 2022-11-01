#ifndef TRIXY_NETWORK_FEED_FORWARD_NET_HPP
#define TRIXY_NETWORK_FEED_FORWARD_NET_HPP

#include <cstddef> // size_t
#include <cstdint> // uint8_t

#include <Trixy/Neuro/Network/Base.hpp>
#include <Trixy/Neuro/Network/Require.hpp>

#include <Trixy/Neuro/Functional/Id.hpp>

#include <Trixy/Locker/Core.hpp>

#include <Trixy/Detail/TrixyMeta.hpp>

#include <Trixy/Neuro/Detail/MacroScope.hpp>

namespace trixy
{

TRIXY_NET_TPL_DECLARATION
using FeedForwardNet = TRIXY_NET_TPL(TrixyNetType::FeedForward);

TRIXY_NET_TPL_DECLARATION
class TRIXY_NET_TPL(TrixyNetType::FeedForward)
    : public guard::TRIXY_NET_REQUIRE_TPL(TrixyNetType::FeedForward)::type
{
public:
    class InnerStruct;

    struct ActivationFunction;
    struct LossFunction;

    class Builder;

    class InnerFunctional;

public:
    template <typename T>
    using Container                 = typename TypeSet::template Container<T>;

    using Vector                    = typename TypeSet::Vector;
    using Matrix                    = typename TypeSet::Matrix;

    using precision_type            = typename TypeSet::precision_type;
    using size_type                 = typename TypeSet::size_type;

    using Linear                    = typename TypeSet::Linear;

    template <typename T>
    using XContainer                = memory::ContainerLocker<Container<T>>;

    using XVector                   = memory::VectorLocker<Vector>;
    using XMatrix                   = memory::MatrixLocker<Matrix>;

    using InnerBuffer               = XContainer<XVector>;
    using InnerTopology             = Container<size_type>;

    using ActivationId              = functional::ActivationId;
    using LossId                    = functional::LossId;

private:
    mutable InnerBuffer buff;       ///< 1D buffer for handle

public:
    InnerStruct inner;              ///< Network set of bias and weight

    InnerFunctional function;       ///< Functional object for setup inner network function
    Linear linear;                  ///< Linear class for tensor calculate

public:
    TrixyNet(const InnerTopology& topology);

    // operator= for copy and move TrixyNet object will not implicit generate
    TrixyNet(const TrixyNet&) = default;
    TrixyNet(TrixyNet&&) noexcept = default;

    const Vector& feedforward(const Vector& sample) const noexcept;
    const Vector& operator() (const Vector& sample) const noexcept;
};

TRIXY_NET_TPL_DECLARATION
class TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerStruct
{
public:
    const size_type N;              ///< Number of functional layer (same as topology_size - 1)

    XContainer<XVector> B;          ///< Container of network bias
    XContainer<XMatrix> W;          ///< Container of network weight

    const InnerTopology topology;   ///< Network topology

public:
    InnerStruct(const InnerTopology& topology);

    // operator= for copy and move InnerStruct object will not implicit generate
    InnerStruct(const InnerStruct&) = default;
    InnerStruct(InnerStruct&&) noexcept = default;

    template <class TopologyGenerator>
    void initialize(TopologyGenerator functor) noexcept;

    template <class BiasGenerator, class WeightGenerator>
    void initialize(BiasGenerator functorB,
                    WeightGenerator functorW) noexcept;

    void initialize(const Container<Vector>& bias,
                    const Container<Matrix>& weight) noexcept;
};

TRIXY_NET_TPL_DECLARATION
struct TRIXY_NET_TPL(TrixyNetType::FeedForward)::ActivationFunction
{
public:
    using Function        = void (*)(XVector&, const XVector&);
    using FunctionDerived = void (*)(XVector&, const XVector&);

public:
    Function f;           ///< void (*f)(XVector& buff, const XVector& tensor)
    FunctionDerived df;   ///< void (*df)(XVector& buff, const XVector& tensor)

    ActivationId id;      ///< id of activation function, for user's def id = activationId::undefined

public:
    ActivationFunction() noexcept : f(nullptr), df(nullptr), id(ActivationId::undefined) {}

    ActivationFunction(Function function,
                       FunctionDerived function_derived,
                       ActivationId function_id = ActivationId::undefined) noexcept
    : f(function)
    , df(function_derived)
    , id(function_id) {}
};

TRIXY_NET_TPL_DECLARATION
struct TRIXY_NET_TPL(TrixyNetType::FeedForward)::LossFunction
{
public:
    using Function        = void (*)(precision_type&, const XVector&, const XVector&);
    using FunctionDerived = void (*)(XVector&, const XVector&, const XVector&);

public:
    Function f;           ///< void (*)(Precision& result, const XVector& target, const XVector& prediction)
    FunctionDerived df;   ///< void (*)(XVector& buff, const XVector& target, const XVector& prediction)

    LossId id;            ///< id of loss function, for user's def id = LossId::undefined

public:
    LossFunction() noexcept : f(nullptr), df(nullptr), id(LossId::undefined) {}

    LossFunction(Function function,
                 FunctionDerived function_derived,
                 LossId function_id = LossId::undefined) noexcept
    : f(function)
    , df(function_derived)
    , id(function_id) {}
};

TRIXY_NET_TPL_DECLARATION
class TRIXY_NET_TPL(TrixyNetType::FeedForward)::Builder
{
public:
    template <class Ret = XContainer<XVector>,
              class Init = Container<XVector>, typename... Args>
    static Ret getlock1d(const InnerTopology& topology, Args&&... args);

    template <class Ret = Container<Vector>,
              class Init = Ret, typename... Args>
    static Ret get1d(const InnerTopology& topology, Args&&... args);

    template <class Ret = XContainer<XMatrix>,
              class Init = Container<XMatrix>, typename... Args>
    static Ret getlock2d(const InnerTopology& topology, Args&&... args);

    template <class Ret = Container<Matrix>,
              class Init = Ret, typename... Args>
    static Ret get2d(const InnerTopology& topology, Args&&... args);
};

TRIXY_NET_TPL_DECLARATION
class TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional
{
public:
    using AllActivationFunction             = Container<ActivationFunction>;
    using AllActivationId                   = Container<ActivationId>;

private:
    AllActivationFunction activation_;      ///< Network activation functions
    AllActivationId activationId_;          ///< Network activation function ids
    LossFunction loss_;                     ///< Network loss function

public:
    explicit InnerFunctional(size_type N) : activation_(N), activationId_(N), loss_() {}
    ~InnerFunctional() = default;

    // operator= for copy and move InnerFunctional object will not implicit generate
    InnerFunctional(const InnerFunctional&) = default;
    InnerFunctional(InnerFunctional&&) noexcept = default;

    void activation(const ActivationFunction&);
    void activationSet(const AllActivationFunction&);
    void normalization(const ActivationFunction&);

    void loss(const LossFunction&);

    const ActivationFunction& activation(size_type i = 0) const noexcept { return activation_[i]; }
    const ActivationId& activationId(size_type i = 0) const noexcept { return activationId_[i]; }

    const AllActivationFunction& activationSet() const noexcept { return activation_; }
    const Container<ActivationId>& activationIdSet() const noexcept { return activationId_; }

    const ActivationFunction& normalization() const noexcept { return activation_.back(); }
    const ActivationId& normalizationId() const noexcept { return activationId_.back(); }

    const LossFunction& loss() const noexcept { return loss_; }
    const LossId& lossId() const noexcept { return loss_.id; }
};

TRIXY_NET_TPL_DECLARATION
template <class TopologyGenerator>
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerStruct::initialize(
    TopologyGenerator functor) noexcept
{
    for (size_type i = 0; i < N; ++i)
    {
        B[i].fill(functor);
        W[i].fill(functor);
    }
}

TRIXY_NET_TPL_DECLARATION
template <class BiasGenerator, class WeightGenerator>
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerStruct::initialize(
    BiasGenerator functorB,
    WeightGenerator functorW) noexcept
{
    for (size_type i = 0; i < N; ++i)
    {
        B[i].fill(functorB);
        W[i].fill(functorW);
    }
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerStruct::initialize(
    const Container<Vector>& bias,
    const Container<Matrix>& weight) noexcept
{
    for (size_type i = 0; i < N; ++i)
    {
        B[i].copy(bias[i]);
        W[i].copy(weight[i]);
    }
}

TRIXY_NET_TPL_DECLARATION
template <class Ret, class Init, typename... Args>
Ret TRIXY_NET_TPL(TrixyNetType::FeedForward)::Builder::get1d(
    const Container<size_type>& topology, Args&&... args)
{
    Init data;

    data.reserve(topology.size() - 1);
    for (size_type i = 1; i < topology.size(); ++i)
        data.emplace_back(topology[i], std::forward<Args>(args)...);

    return data;
}

TRIXY_NET_TPL_DECLARATION
template <class Ret, class Init, typename... Args>
Ret TRIXY_NET_TPL(TrixyNetType::FeedForward)::Builder::getlock1d(
    const Container<size_type>& topology, Args&&... args)
{
    return get1d<Ret, Init>(topology, std::forward<Args>(args)...);
}

TRIXY_NET_TPL_DECLARATION
template <class Ret, class Init, typename... Args>
Ret TRIXY_NET_TPL(TrixyNetType::FeedForward)::Builder::get2d(
    const Container<size_type>& topology, Args&&... args)
{
    Init data;

    data.reserve(topology.size() - 1);
    for (size_type i = 1; i < topology.size(); ++i)
        data.emplace_back(topology[i - 1], topology[i], std::forward<Args>(args)...);

    return data;
}

TRIXY_NET_TPL_DECLARATION
template <class Ret, class Init, typename... Args>
Ret TRIXY_NET_TPL(TrixyNetType::FeedForward)::Builder::getlock2d(
    const Container<size_type>& topology, Args&&... args)
{
    return get2d<Ret, Init>(topology, std::forward<Args>(args)...);
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional::activation(
    const ActivationFunction& f)
{
    for (size_type i = 0; i < activation_.size() - 1; ++i)
    {
        activation_  [i] = f;
        activationId_[i] = f.id;
    }
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional::activationSet(
    const Container<ActivationFunction>& fs)
{
    for (size_type i = 0; i < activation_.size(); ++i)
    {
        activation_  [i] = fs[i];
        activationId_[i] = fs[i].id;
    }
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional::normalization(
    const ActivationFunction& f)
{
    activation_  .back() = f;
    activationId_.back() = f.id;
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional::loss(
    const LossFunction& f)
{
    loss_ = f;
}

TRIXY_NET_TPL_DECLARATION
TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerStruct::InnerStruct(
    const Container<size_type>& topology)
    : N(topology.size() - 1)
    , B(TrixyNet::Builder::getlock1d(topology))
    , W(TrixyNet::Builder::getlock2d(topology))
    , topology(topology)
{
}

TRIXY_NET_TPL_DECLARATION
TRIXY_NET_TPL(TrixyNetType::FeedForward)::TrixyNet(
    const Container<size_type>& topology)
    : buff(TrixyNet::Builder::getlock1d(topology))
    , inner(topology)
    , function(topology.size() - 1)
    , linear()
{
}

TRIXY_NET_TPL_DECLARATION
auto TRIXY_NET_TPL(TrixyNetType::FeedForward)::feedforward(
    const Vector& sample) const noexcept -> const Vector&
{
    /*
    Operations:
    . - dot
    Processing:
    buff = H' . W + B, where H' - previous
    H = activation(buff)
    */

    linear.dot(buff[0], sample, inner.W[0]);
    linear.add(buff[0], inner.B[0]);
    function.activation(0).f(buff[0], buff[0]);

    for (size_type i = 1; i < inner.N; ++i)
    {
        linear.dot(buff[i], buff[i - 1], inner.W[i]);
        linear.add(buff[i], inner.B[i]);
        function.activation(i).f(buff[i], buff[i]);
    }

    return buff.back().base();
}

TRIXY_NET_TPL_DECLARATION
auto TRIXY_NET_TPL(TrixyNetType::FeedForward)::operator() (
    const Vector& sample) const noexcept -> const Vector&
{
    return feedforward(sample);
}

} // namespace trixy

#include <Trixy/Neuro/Detail/MacroUnscope.hpp>

#endif // TRIXY_NETWORK_FEED_FORWARD_NET_HPP

#ifndef TRIXY_NETWORK_FEED_FORWARD_NET_HPP
#define TRIXY_NETWORK_FEED_FORWARD_NET_HPP

#include <cstddef> // size_t
#include <cstdint> // uint8_t

#include <Trixy/Neuro/Network/Base.hpp>
#include <Trixy/Neuro/Network/Require.hpp>

#include <Trixy/Neuro/Training/Base.hpp>
#include <Trixy/Neuro/Functional/Id.hpp>

#include <Trixy/Locker/Vector.hpp>
#include <Trixy/Locker/Matrix.hpp>
#include <Trixy/Locker/Container.hpp>

#include <Trixy/Detail/TrixyMeta.hpp>

#include <Trixy/Neuro/Detail/MacroScope.hpp>

namespace trixy
{

TRIXY_NET_TPL_DECLARATION
using FeedForwardNet = TRIXY_NET_TPL(TrixyNetType::FeedForward);

TRIXY_NET_TPL_DECLARATION
class TRIXY_NET_TPL(TrixyNetType::FeedForward) :
    public TRIXY_NET_REQUIRE_TPL(TrixyNetType::FeedForward)::type
{
public:
    struct InnerStruct;

    struct ActivationFunction;
    struct LossFunction;

    class Init;

private:
    class InnerFunctional;

public:
    template <typename T>
    using Container                 = ContainerType<T>;

    template <typename T>
    using XContainer                = ContainerLocker<ContainerType<T>>;

    using Vector                    = VectorType<PrecisionType, Pack...>;
    using Matrix                    = MatrixType<PrecisionType, Pack...>;

    using XVector                   = VectorLocker<Vector>;
    using XMatrix                   = MatrixLocker<Matrix>;

    using precision_type            = PrecisionType;
    using size_type                 = std::size_t;

    using TensorOperation           = LinearType<precision_type>;

    using InnerBuffer               = XContainer<XVector>;
    using InnerTopology             = ContainerType<size_type>;

    using ActivationId              = functional::ActivationId;
    using LossId                    = functional::LossId;

private:
    mutable InnerBuffer buff;       ///< 1D buffer for handle

public:
    InnerStruct inner;              ///< Network set of bias and weight

    InnerFunctional function;       ///< Functional object for setup inner network function
    TensorOperation linear;         ///< Linear class for tensor calculate

public:
    TrixyNet(const InnerTopology& topology);

    // operator= for copy and move TrixyNet object will not implicit generate
    TrixyNet(const TrixyNet&) = default;
    TrixyNet(TrixyNet&&) noexcept = default;

    const Vector& feedforward(const Vector& sample) const noexcept;

public:
    template <class FloatGenerator>
    void initInnerStruct(FloatGenerator generator_all) noexcept;

    template <class FloatGenerator>
    void initInnerStruct(FloatGenerator generator_bias,
                         FloatGenerator generator_weight) noexcept;

    void initInnerStruct(const Container<Vector>& bias,
                         const Container<Matrix>& weight) noexcept;

    const Container<XVector>& getInnerBias() const noexcept { return inner.B.base(); }
    const Container<XMatrix>& getInnerWeight() const noexcept { return inner.W.base(); }
    const InnerTopology& getTopology() const noexcept { return inner.topology; }

    long double accuracy(const Container<Vector>& idata,
                         const Container<Vector>& odata) const noexcept;

    long double loss(const Container<Vector>& idata,
                     const Container<Vector>& odata) const noexcept;

    bool check(const Vector& target,
               const Vector& prediction) const noexcept;
};

TRIXY_NET_TPL_DECLARATION
struct TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerStruct
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
};

TRIXY_NET_TPL_DECLARATION
TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerStruct::InnerStruct(
    const Container<size_type>& topology)
    : N(topology.size() - 1)
    , B(TrixyNet::Init::getlock1d(topology))
    , W(TrixyNet::Init::getlock2d(topology))
    , topology(topology)
{
}

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
class TRIXY_NET_TPL(TrixyNetType::FeedForward)::Init
{
public:
    template <class Ret = XContainer<XVector>,
              class InitData = Container<XVector>, typename... Args>
    static Ret getlock1d(const InnerTopology& topology, Args&&... args);

    template <class Ret = Container<Vector>,
              class InitData = Ret, typename... Args>
    static Ret get1d(const InnerTopology& topology, Args&&... args);

    template <class Ret = XContainer<XMatrix>,
              class InitData = Container<XMatrix>, typename... Args>
    static Ret getlock2d(const InnerTopology& topology, Args&&... args);

    template <class Ret = Container<Matrix>,
              class InitData = Ret, typename... Args>
    static Ret get2d(const InnerTopology& topology, Args&&... args);
};

TRIXY_NET_TPL_DECLARATION
class TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional
{
friend TrixyNet;
friend train::Training<TrixyNet>;

private:
    ~InnerFunctional() = default;

public:
    using AllActivationFunction             = Container<ActivationFunction>;
    using AllActivationId                   = Container<ActivationId>;

private:
    AllActivationFunction activation;       ///< Network activation functions
    LossFunction loss;                      ///< Network loss function

    AllActivationId activationId;           ///< Network activation function ids

public:
    explicit InnerFunctional(size_type N) : activation(N), loss(), activationId(N) {}

    // operator= for copy and move InnerFunctional object will not implicit generate
    InnerFunctional(const InnerFunctional&) = default;
    InnerFunctional(InnerFunctional&&) noexcept = default;

    void setActivation(const ActivationFunction&);
    void setAllActivation(const AllActivationFunction&);
    void setNormalization(const ActivationFunction&);

    void setLoss(const LossFunction&);

    const ActivationFunction& getActivation() const noexcept { return activation.front(); }
    const ActivationId& getActivationId() const noexcept { return activationId.front(); }

    const AllActivationFunction& getAllActivation() const noexcept { return activation; }
    const Container<ActivationId>& getAllActivationId() const noexcept { return activationId; }

    const ActivationFunction& getNormalization() const noexcept { return activation.back(); }
    const ActivationId& getNormalizationId() const noexcept { return activationId.back(); }

    const LossFunction& getLoss() const noexcept { return loss; }
    const LossId& getLossId() const noexcept { return loss.id; }
};

TRIXY_NET_TPL_DECLARATION
template <class Ret, class InitData, typename... Args>
Ret TRIXY_NET_TPL(TrixyNetType::FeedForward)::Init::get1d(
    const Container<size_type>& topology, Args&&... args)
{
    InitData data;

    data.reserve(topology.size() - 1);
    for(size_type i = 1; i < topology.size(); ++i)
        data.emplace_back(topology[i], std::forward<Args>(args)...);

    return data;
}

TRIXY_NET_TPL_DECLARATION
template <class Ret, class InitData, typename... Args>
Ret TRIXY_NET_TPL(TrixyNetType::FeedForward)::Init::getlock1d(
    const Container<size_type>& topology, Args&&... args)
{
    return get1d<Ret, InitData>(topology, std::forward<Args>(args)...);
}

TRIXY_NET_TPL_DECLARATION
template <class Ret, class InitData, typename... Args>
Ret TRIXY_NET_TPL(TrixyNetType::FeedForward)::Init::get2d(
    const Container<size_type>& topology, Args&&... args)
{
    InitData data;

    data.reserve(topology.size() - 1);
    for(size_type i = 1; i < topology.size(); ++i)
        data.emplace_back(topology[i - 1], topology[i], std::forward<Args>(args)...);

    return data;
}

TRIXY_NET_TPL_DECLARATION
template <class Ret, class InitData, typename... Args>
Ret TRIXY_NET_TPL(TrixyNetType::FeedForward)::Init::getlock2d(
    const Container<size_type>& topology, Args&&... args)
{
    return get2d<Ret, InitData>(topology, std::forward<Args>(args)...);
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional::setActivation(
    const TRIXY_NET_TPL(TrixyNetType::FeedForward)::ActivationFunction& f)
{
    for(size_type i = 0; i < activation.size() - 1; ++i)
    {
        activation  [i] = f;
        activationId[i] = f.id;
    }
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional::setAllActivation(
    const Container<ActivationFunction>& fs)
{
    for(size_type i = 0; i < activation.size(); ++i)
    {
        activation  [i] = fs[i];
        activationId[i] = fs[i].id;
    }
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional::setNormalization(
    const ActivationFunction& f)
{
    activation.  back() = f;
    activationId.back() = f.id;
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::InnerFunctional::setLoss(
    const LossFunction& f)
{
    loss = f;
}

TRIXY_NET_TPL_DECLARATION
TRIXY_NET_TPL(TrixyNetType::FeedForward)::TrixyNet(
    const Container<size_type>& topology)
    : buff(TrixyNet::Init::getlock1d(topology))
    , inner(topology)
    , function(topology.size() - 1)
{
}

TRIXY_NET_TPL_DECLARATION
template <class FloatGenerator>
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::initInnerStruct(
    FloatGenerator generator_all) noexcept
{
    for(size_type i = 0; i < inner.N; ++i)
    {
        inner.B[i].fill(generator_all);
        inner.W[i].fill(generator_all);
    }
}

TRIXY_NET_TPL_DECLARATION
template <class FloatGenerator>
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::initInnerStruct(
    FloatGenerator generator_bias,
    FloatGenerator generator_weight) noexcept
{
    for(size_type i = 0; i < inner.N; ++i)
    {
        inner.B[i].fill(generator_bias);
        inner.W[i].fill(generator_weight);
    }
}

TRIXY_NET_TPL_DECLARATION
void TRIXY_NET_TPL(TrixyNetType::FeedForward)::initInnerStruct(
    const Container<Vector>& bias,
    const Container<Matrix>& weight) noexcept
{
    for(size_type i = 0; i < inner.N; ++i)
    {
        inner.B[i].copy(bias[i]);
        inner.W[i].copy(weight[i]);
    }
}

TRIXY_NET_TPL_DECLARATION
const typename TRIXY_NET_TPL(TrixyNetType::FeedForward)::Vector&
    TRIXY_NET_TPL(TrixyNetType::FeedForward)::feedforward(
    const Vector& sample) const noexcept
{
    /*
    Operations:
    . - dot
    Processing:
    buff = H' . W + B, H' - previous
    H = activation(buff)
    */
    linear.dot(buff[0], sample, inner.W[0]);
    linear.add(buff[0], inner.B[0]);
    function.activation[0].f(buff[0], buff[0]);

    for(size_type i = 1; i < inner.N; ++i)
    {
        linear.dot(buff[i], buff[i - 1], inner.W[i]);
        linear.add(buff[i], inner.B[i]);
        function.activation[i].f(buff[i], buff[i]);
    }

    return buff.back().base();
}

TRIXY_NET_TPL_DECLARATION
long double TRIXY_NET_TPL(TrixyNetType::FeedForward)::accuracy(
    const Container<Vector>& idata,
    const Container<Vector>& odata) const noexcept
{
    size_type count = 0;

    for(size_type i = 0; i < odata.size(); ++i)
        if(check(odata[i], feedforward(idata[i])))
            ++count;

    return static_cast<long double>(count) / odata.size();
}

TRIXY_NET_TPL_DECLARATION
long double TRIXY_NET_TPL(TrixyNetType::FeedForward)::loss(
    const Container<Vector>& idata,
    const Container<Vector>& odata) const noexcept
{
    precision_type result = 0.;
    precision_type error  = 0.;

    for(size_type i = 0; i < odata.size(); ++i)
    {
        function.loss.f(error, odata[i], feedforward(idata[i]));
        result += error;
    }

    return result / static_cast<long double>(odata.size());
}

TRIXY_NET_TPL_DECLARATION
bool TRIXY_NET_TPL(TrixyNetType::FeedForward)::check(
    const Vector& target,
    const Vector& prediction) const noexcept
{
    size_type max_true_out;
    size_type max_pred_out;

    max_true_out = 0;
    max_pred_out = 0;

    for(size_type j = 1; j < target.size(); ++j)
        if(target(max_true_out) < target(j))
            max_true_out = j;

    for(size_type j = 1; j < target.size(); ++j)
        if(prediction(max_pred_out) < prediction(j))
            max_pred_out = j;

    return max_true_out == max_pred_out;
}

} // namespace trixy

#include <Trixy/Neuro/Detail/MacroUnscope.hpp>

#endif // TRIXY_NETWORK_FEED_FORWARD_NET_HPP

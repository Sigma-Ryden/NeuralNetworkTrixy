#ifndef RMS_PROP_OPTIMIZER_HPP
#define RMS_PROP_OPTIMIZER_HPP

#include "BaseOptimizer.hpp"
#include "FeedForwardNetIOptimizer.hpp"

#include "Trixy/Neuro/Functional/IdFunctional.hpp"

#include "Trixy/Neuro/Detail/FunctionDetail.hpp"
#include "Trixy/Neuro/Detail/TrixyNetMeta.hpp"

#include "Trixy/Neuro/Detail/MacroScope.hpp"

namespace trixy
{

namespace train
{

TRIXY_OPTIMIZER_TPL_DECLARATION
using RMSPropOptimizer =
    TRIXY_OPTIMIZER_TPL(meta::is_feedforward_net, OptimizerType::rms_prop);

TRIXY_OPTIMIZER_TPL_DECLARATION
class TRIXY_OPTIMIZER_TPL(meta::is_feedforward_net, OptimizerType::rms_prop)
    : public IOptimizer<RMSPropOptimizer<Optimizeriable>, Optimizeriable>
{
public:
    using Net               = Optimizeriable;

    using Base              = IOptimizer<RMSPropOptimizer<Net>, Net>;

    template <class... T>
    using Container         = typename Net::template Container<T...>;

    using Vector            = typename Net::Vector;
    using Matrix            = typename Net::Matrix;

    using NetInit           = typename Net::Init;

    using precision_type    = typename Net::precision_type;
    using size_type         = typename Net::size_type;

private:
    Net& net;

    Container<Vector> buff1;
    Container<Matrix> buff2;

    Container<Vector> optimizedB;
    Container<Matrix> optimizedW;

    precision_type learning_rate;

    precision_type beta, rbeta;

public:
    Optimizer(Net& network,
              precision_type learning_rate,
              precision_type beta = 0.9);

    void set_learning_rate(precision_type value) noexcept;

    template <class BiasGrad, class WeightGrad>
    void update(const Container<BiasGrad>& grad_bias,
                const Container<WeightGrad>& grad_weight) noexcept;

    Optimizer& reset() noexcept;

private:
    template <class Buffer, class Optimized, class Parameter, class Gradient>
    void update(Buffer& buff,
                Optimized& optimized,
                Parameter& parameter,
                const Gradient& grad) noexcept;
};

TRIXY_OPTIMIZER_TPL_DECLARATION
RMSPropOptimizer<Optimizeriable>::Optimizer(
    Net& network,
    precision_type learning_rate,
    precision_type beta)
    : Base()
    , net(network)
    , buff1(NetInit::get1D(net.inner.topology))
    , buff2(NetInit::get2D(net.inner.topology))
    , optimizedB(NetInit::get1D(net.inner.topology, 0.))
    , optimizedW(NetInit::get2D(net.inner.topology, 0.))
    , learning_rate(learning_rate)
    , beta(beta)
{
    rbeta = 1. - beta;
}

TRIXY_OPTIMIZER_TPL_DECLARATION
void RMSPropOptimizer<Optimizeriable>::set_learning_rate(
    precision_type value) noexcept
{
    learning_rate = value;
}

TRIXY_OPTIMIZER_TPL_DECLARATION
template <class BiasGrad, class WeightGrad>
void RMSPropOptimizer<Optimizeriable>::update(
    const Container<BiasGrad>& gradB,
    const Container<WeightGrad>& gradW) noexcept
{
    for(size_type i = 0; i < net.inner.N; ++i)
    {
        update(buff1[i], optimizedB[i], net.inner.B[i], gradB[i]);
        update(buff2[i], optimizedW[i], net.inner.W[i], gradW[i]);
    }
}

TRIXY_OPTIMIZER_TPL_DECLARATION
template <class Buffer, class Optimized, class Parameter, class Gradient>
void RMSPropOptimizer<Optimizeriable>::update(
    Buffer& buff,
    Optimized& optimized,
    Parameter& parameter,
    const Gradient& grad) noexcept
{
    // velocity = beta * velocity + (1 - beta) * g * g
    // w = w - learning_rate * g / sqrt(velocity)

    net.linear.join(optimized, beta);
    net.linear.mul(buff, grad, grad);
    net.linear.join(buff, rbeta);
    net.linear.add(optimized, buff);

    net.linear.apply(buff, &detail::invert_sqrt<precision_type>, optimized);
    net.linear.mul(buff, grad);
    net.linear.join(buff, learning_rate);

    net.linear.sub(parameter, buff);
}

TRIXY_OPTIMIZER_TPL_DECLARATION
RMSPropOptimizer<Optimizeriable>& RMSPropOptimizer<Optimizeriable>::reset() noexcept
{
    for(size_type i = 0; i < net.inner.N; ++i)
    {
        optimizedB[i].fill(0.);
        optimizedW[i].fill(0.);
    }

    return *this;
}

} // namespace train

} // namespace trixy

#include "Trixy/Neuro/Detail/MacroUnscope.hpp"

#endif // RMS_PROP_OPTIMIZER_HPP

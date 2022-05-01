#ifndef TRIXY_FUNCTION_LOSS_HPP
#define TRIXY_FUNCTION_LOSS_HPP

#include <cmath> // log, fabs, tanh, cosh

#include <Trixy/Detail/MacroScope.hpp>

namespace trixy
{

namespace functional
{

namespace loss
{

template <typename Precision, class Target, class Prediction>
void categorical_cross_entropy(Precision& result, const Target& y_true, const Prediction& y_pred) noexcept
{
    static constexpr Precision epsilon = 1e-9;

    auto target = y_true.data();
    auto end    = y_true.data() + y_true.size();

    auto pred   = y_pred.data();

    result = Precision{};
    while(target != end)
    {
        result -= (*target) * std::log(*pred + epsilon);

        ++target;
        ++pred;
    }
}

template <class Buffer, class Target, class Prediction>
void categorical_cross_entropy_derived(Buffer& buff, const Target& y_true, const Prediction& y_pred) noexcept
{
    static constexpr auto epsilon = 1e-9;

    auto first  = buff.data();
    auto last   = buff.data() + buff.size();

    auto target = y_true.data();
    auto pred   = y_pred.data();

    while(first != last)
    {
        *first++ = -(*target) / (*pred + epsilon);

        ++target;
        ++pred;
    }
}

template <typename Precision, class Target, class Prediction>
void mean_squared_error(Precision& result, const Target& y_true, const Prediction& y_pred) noexcept
{
    static Precision f;

    auto target = y_true.data();
    auto end    = y_true.data() + y_true.size();

    auto pred   = y_pred.data();

    result = Precision{};
    while(target != end)
    {
        f = *target - *pred;
        result += f * f;

        ++target;
        ++pred;
    }

    result *= 0.5;
}

template <class Buffer, class Target, class Prediction>
void mean_squared_error_derived(Buffer& buff, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto first  = buff.data();
    auto last   = buff.data() + buff.size();

    auto target = y_true.data();
    auto pred   = y_pred.data();

    while(first != last)
    {
        *first++ = *pred - *target;

        ++target;
        ++pred;
    }
}

template <typename Precision, class Target, class Prediction>
void mean_absolute_error(Precision& result, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto target = y_true.data();
    auto end    = y_true.data() + y_true.size();

    auto pred   = y_pred.data();

    result = Precision{};
    while(target != end)
    {
        result += std::fabs(*pred - *target);

        ++target;
        ++pred;
    }
}

template <class Buffer, class Target, class Prediction>
void mean_absolute_error_derived(Buffer& buff, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto first  = buff.data();
    auto last   = buff.data() + buff.size();

    auto target = y_true.data();
    auto pred   = y_pred.data();

    while(first != last)
    {
        *first = *target - *pred;
        *first = *first < 0.
                 ? -1.
                 : (*first > 0. ? 1. : 0.);

        ++first;
        ++target;
        ++pred;
    }
}

template <typename Precision, class Target, class Prediction>
void mean_squared_log_error(Precision& result, const Target& y_true, const Prediction& y_pred) noexcept
{
    static Precision f;

    auto target = y_true.data();
    auto end    = y_true.data() + y_true.size();

    auto pred   = y_pred.data();

    result = Precision{};
    while(target != end)
    {
        f = (*pred + 1.) / (*target + 1.);
        f = std::log(f);

        result += f * f;

        ++target;
        ++pred;
    }

    result *= 0.5;
}

template <class Buffer, class Target, class Prediction>
void mean_squared_log_error_derived(Buffer& buff, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto first  = buff.data();
    auto last   = buff.data() + buff.size();

    auto target = y_true.data();
    auto pred   = y_pred.data();

    while(first != last)
    {
        *first = *pred + 1.;
        *first = std::log(*first / (*target + 1.)) / (*first);

        ++first;
        ++target;
        ++pred;
    }
}

template <typename Precision, class Target, class Prediction>
void binary_cross_entropy(Precision& result, const Target& y_true, const Prediction& y_pred) noexcept
{
    static constexpr Precision epsilon = 1e-9;

    auto target = y_true.data();
    auto end    = y_true.data() + y_true.size();

    auto pred   = y_pred.data();

    result = Precision{};
    while(target != end)
    {
        result -= (*target) * std::log(*pred + epsilon) +  (1. - *target) * std::log1p(epsilon - *pred);

        ++target;
        ++pred;
    }
}

template <class Buffer, class Target, class Prediction>
void binary_cross_entropy_derived(Buffer& buff, const Target& y_true, const Prediction& y_pred) noexcept
{
    static constexpr auto epsilon = 1e-9;
    static constexpr auto alpha   = epsilon - 1.0;

    auto first  = buff.data();
    auto last   = buff.data() + buff.size();

    auto target = y_true.data();
    auto pred   = y_pred.data();

    while(first != last)
    {
        *first++ = (*target - 1.) / (*pred + alpha) - (*target) / (*pred + epsilon);

        ++target;
        ++pred;
    }
}

template <class Buffer, class Target, class Prediction>
void binary_cross_entropy_derived_sigmoid(Buffer& buff, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto first  = buff.data();
    auto last   = buff.data() + buff.size();

    auto target = y_true.data();
    auto pred   = y_pred.data();

    while(first != last)
    {
        *first++ = (*target) * (*pred - 1.) + (*pred) * (1. - *target);

        ++target;
        ++pred;
    }
}

template <class Precision, class Target, class Prediction>
void negative_log_likelihood(Precision& result, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto target = y_true.data();
    auto end    = y_true.data() + y_true.size();

    auto pred   = y_pred.data();

    result = Precision{};
    while(target != end)
    {
        result += (*target) * (*pred);

        ++target;
        ++pred;
    }
    // negative logarithm
    result = -std::log(result);
}

template <class Buffer, class Target, class Prediction>
void negative_log_likelihood_derived_softmax(Buffer& buff, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto first  = buff.data();
    auto last   = buff.data() + buff.size();

    auto target = y_true.data();
    auto pred   = y_pred.data();

    while(first != last)
    {
        *first++ = *pred - *target;

        ++target;
        ++pred;
    }
}

template <typename Precision, class Target, class Prediction>
void logcosh(Precision& result, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto target = y_true.data();
    auto end    = y_true.data() + y_true.size();

    auto pred   = y_pred.data();

    result = Precision{};
    while(target != end)
    {
        result += std::log(std::cosh(*pred - *target));

        ++target;
        ++pred;
    }
}

template <class Buffer, class Target, class Prediction>
void logcosh_derived(Buffer& buff, const Target& y_true, const Prediction& y_pred) noexcept
{
    auto first  = buff.data();
    auto last   = buff.data() + buff.size();

    auto target = y_true.data();
    auto pred   = y_pred.data();

    while(first != last)
    {
        *first++ = std::tanh(*pred - *target);

        ++target;
        ++pred;
    }
}

template <class LossFunction, typename CastType, typename LossId>
LossFunction get_loss_function(LossId id)
{
    auto f_id = static_cast<CastType>(id);

    switch (id)
    {
    case LossId::MSE:  return { mean_squared_error, mean_squared_error_derived, f_id };
    case LossId::MAE:  return { mean_absolute_error, mean_absolute_error_derived, f_id };
    case LossId::CCE:  return { categorical_cross_entropy, mean_squared_error_derived, f_id };

    case LossId::BCE:  return { binary_cross_entropy, binary_cross_entropy_derived_sigmoid, f_id };
    case LossId::MSLE: return { mean_squared_log_error, mean_squared_log_error_derived, f_id };
    case LossId::NLL:  return { negative_log_likelihood, negative_log_likelihood_derived_softmax, f_id };

    case LossId::LC:   return { logcosh, logcosh_derived, f_id };

    case LossId::CCE_: return { categorical_cross_entropy, categorical_cross_entropy_derived, f_id };
    case LossId::BCE_: return { binary_cross_entropy, binary_cross_entropy_derived, f_id };

    default:
        return { nullptr, nullptr, static_cast<CastType>(LossId::undefined) };
    }
}

TRIXY_FUNCTION_GENERIC_LOSS_HELPER(MSE, mean_squared_error);
TRIXY_FUNCTION_GENERIC_LOSS_HELPER(MAE, mean_absolute_error);
TRIXY_FUNCTION_GENERIC_LOSS_HELPER(CCE, categorical_cross_entropy);
TRIXY_FUNCTION_GENERIC_LOSS_HELPER(BCE, binary_cross_entropy);
TRIXY_FUNCTION_GENERIC_LOSS_HELPER(MSLE, mean_squared_log_error);
TRIXY_FUNCTION_GENERIC_LOSS_HELPER(NLL, negative_log_likelihood);
TRIXY_FUNCTION_GENERIC_LOSS_HELPER(LC, logcosh);

} // namespace loss

} // namespace functional

} // namespace trixy

#include <Trixy/Detail/MacroUnscope.hpp>

#endif // TRIXY_FUNCTION_LOSS_HPP

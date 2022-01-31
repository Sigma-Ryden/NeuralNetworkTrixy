#ifndef POLYNOMIAL_REGRESSION_HPP
#define POLYNOMIAL_REGRESSION_HPP

#include <cstddef> // size_t

#include "Trixy/Neuro/Regression/BaseRegression.hpp"
#include "Trixy/Neuro/Training/PolynomialRegressionTraining.hpp"

#include "Trixy/Neuro/Detail/TrixyNetMeta.hpp"

#include "Trixy/Neuro/Detail/MacroScope.hpp"

namespace trixy
{
TRIXY_REGRESSION_TPL_DECLARATION
using PolynomialRegression = TRIXY_REGRESSION_TPL(RegressionType::Polynomial);

TRIXY_REGRESSION_TPL_DECLARATION
class TRIXY_REGRESSION_TPL(RegressionType::Polynomial) : public RegressionType::Polynomial
{
friend train::Training<TRIXY_REGRESSION_TPL(RegressionType::Polynomial)>;

public:
    using Vector          = VectorType<PrecisionType, Args...>;
    using Matrix          = MatrixType<PrecisionType, Args...>;

    using precision_type  = PrecisionType;
    using size_type       = std::size_t;

    using TensorOperation = LinearType<PrecisionType>;

private:
    Vector  W;            ///< Inner weight
    size_type N;          ///< Size of weight vector (same as power size + 1)

    TensorOperation linear;

public:
    explicit Regression(size_type power);

    void initializeInnerStruct(Vector weight) noexcept;

    void reset(size_type new_power);

    precision_type feedforward(precision_type sample) const noexcept;
    Vector feedforward(const Vector& idata) const;

    long double loss(const Vector& idata,
                     const Vector& odata) const;

    const Vector& getInnerWeight() const noexcept { return W; }
    precision_type getInnerPower() const noexcept { return N; }
};

TRIXY_REGRESSION_TPL_DECLARATION
TRIXY_REGRESSION_TPL(RegressionType::Polynomial)::Regression(size_type power)
    : W(power + 1), N(power + 1)
{
}

TRIXY_REGRESSION_TPL_DECLARATION
void TRIXY_REGRESSION_TPL(RegressionType::Polynomial)::initializeInnerStruct(
    Vector weight) noexcept
{
    W.copy(weight);
}

TRIXY_REGRESSION_TPL_DECLARATION
void TRIXY_REGRESSION_TPL(RegressionType::Polynomial)::reset(
    size_type new_power)
{
    W.resize(new_power + 1);
    N = new_power + 1;
}

TRIXY_REGRESSION_TPL_DECLARATION
typename TRIXY_REGRESSION_TPL(RegressionType::Polynomial)::precision_type
    TRIXY_REGRESSION_TPL(RegressionType::Polynomial)::feedforward(
    precision_type sample) const noexcept
{
    precision_type result = W(0);
    precision_type power  = 1.;

    for(size_type i = 1; i < N; ++i)
    {
        power  *= sample;
        result += power * W(i);
    }

    return result;
}

TRIXY_REGRESSION_TPL_DECLARATION
typename TRIXY_REGRESSION_TPL(RegressionType::Polynomial)::Vector
    TRIXY_REGRESSION_TPL(RegressionType::Polynomial)::feedforward(
    const Vector& idata) const
{
    Matrix X(idata.size(), N);

    precision_type sample;
    precision_type power;

    for(size_type i = 0; i < idata.size(); ++i)
    {
        sample  = idata(i);
        power   = 1.;

        X(i, 0) = 1.;
        for(size_type j = 1; j < N; ++j)
        {
            power  *= sample;
            X(i, j) = power;
        }
    }

    return linear.dot(X, W);
}

TRIXY_REGRESSION_TPL_DECLARATION
long double TRIXY_REGRESSION_TPL(RegressionType::Polynomial)::loss(
    const Vector& idata,
    const Vector& odata) const
{
    Vector r = feedforward(idata);
    r.sub(odata);

    return r.dot(r) / static_cast<long double>(r.size());
}

} // namespace trixy

#include "Trixy/Neuro/Detail/MacroUnscope.hpp"

#endif // POLYNOMIAL_REGRESSION_HPP

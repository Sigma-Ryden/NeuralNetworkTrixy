#ifndef TRIXY_REGRESSION_LINEAR_HPP
#define TRIXY_REGRESSION_LINEAR_HPP

#include <cstddef> // size_t

#include "Base.hpp"
#include "Require.hpp"

#include "Trixy/Neuro/Training/Base.hpp"

#include "Trixy/Detail/TrixyMeta.hpp"

#include "Trixy/Neuro/Detail/MacroScope.hpp"

namespace trixy
{

TRIXY_REGRESSION_TPL_DECLARATION
using LinearRegression = TRIXY_REGRESSION_TPL(RegressionType::Linear);

TRIXY_REGRESSION_TPL_DECLARATION
class TRIXY_REGRESSION_TPL(RegressionType::Linear) :
    public TRIXY_REGRESSION_REQUIRE_TPL(RegressionType::Linear)::type
{
friend train::Training<TRIXY_REGRESSION_TPL(RegressionType::Linear)>;

public:
    using Vector          = typename TypeSet::Vector;
    using Matrix          = typename TypeSet::Matrix;

    using Linear          = typename TypeSet::Linear;

    using precision_type  = typename TypeSet::precision_type;
    using size_type       = typename TypeSet::size_type;

private:
    Vector W;             ///< Inner weight
    size_type N;          ///< Size of weight vector (same as sample size + 1)

    Linear linear;

public:
    explicit Regression(size_type sample_size);

    void initializeInnerStruct(Vector weight) noexcept;

    void reset(size_type new_sample_size);

    precision_type feedforwardSample(const Vector& sample) const;
    Vector feedforward(const Matrix& idata) const;

    Vector operator() (const Matrix& idata) const
    { return feedforward(idata); }

    const Vector& getInnerWeight() const noexcept { return W; }
    size_type getInnerSize() const noexcept { return N - 1; }
};

TRIXY_REGRESSION_TPL_DECLARATION
TRIXY_REGRESSION_TPL(RegressionType::Linear)::Regression(size_type sample_size)
    : W(sample_size + 1), N(sample_size + 1)
{
}

TRIXY_REGRESSION_TPL_DECLARATION
void TRIXY_REGRESSION_TPL(RegressionType::Linear)::initializeInnerStruct(Vector weight) noexcept
{
    W.copy(weight);
}

TRIXY_REGRESSION_TPL_DECLARATION
void TRIXY_REGRESSION_TPL(RegressionType::Linear)::reset(size_type new_sample_size)
{
    N = new_sample_size + 1;
    W.resize(N);
}

TRIXY_REGRESSION_TPL_DECLARATION
typename TRIXY_REGRESSION_TPL(RegressionType::Linear)::precision_type
    TRIXY_REGRESSION_TPL(RegressionType::Linear)::feedforwardSample(
    const Vector& sample) const
{
    precision_type result = W(0);

    for (size_type i = 1; i < N; ++i)
    {
        result += sample(i - 1) * W(i);
    }

    return result;
}

TRIXY_REGRESSION_TPL_DECLARATION
typename TRIXY_REGRESSION_TPL(RegressionType::Linear)::Vector
    TRIXY_REGRESSION_TPL(RegressionType::Linear)::feedforward(
    const Matrix& idata) const
{
    Matrix X(idata.shape().height, N);

    for (size_type i = 0; i < X.shape().height; ++i)
    {
        X(i, 0) = 1.;
        for (size_type j = 1; j < N; ++j)
            X(i, j) = idata(i, j - 1);
    }

    return linear.dot(X, W);
}

} // namespace trixy

#include "Trixy/Neuro/Detail/MacroUnscope.hpp"

#endif // TRIXY_REGRESSION_LINEAR_HPP

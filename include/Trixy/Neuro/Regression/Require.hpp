#ifndef TRIXY_REGRESSION_REQUIRE_HPP
#define TRIXY_REGRESSION_REQUIRE_HPP

#include <Trixy/Neuro/Regression/Base.hpp>

#include <Trixy/Require/Core.hpp>

#include <Trixy/Neuro/Detail/MacroScope.hpp>

namespace trixy
{

TRIXY_REGRESSION_TPL_DECLARATION
struct TRIXY_REGRESSION_REQUIRE_TPL(RegressionType::Linear)
{
public:
    using type = RegressionType::Linear;

protected:
    using precision_type    = typename TypeSet::precision_type;

    using Vector            = typename TypeSet::Vector;
    using Matrix            = typename TypeSet::Matrix;

    using Linear            = typename TypeSet::Linear;

protected:
    using require_precision = typename PrecisionRequire<precision_type>::type;

    using require_vector    = typename VectorRequire<Vector>::type;
    using require_matrix    = typename MatrixRequire<Matrix>::type;

    using require_linear    = typename LinearRequire<Linear>::type;
};

TRIXY_REGRESSION_TPL_DECLARATION
struct TRIXY_REGRESSION_REQUIRE_TPL(RegressionType::Polynomial)
{
public:
    using type = RegressionType::Polynomial;

protected:
    using precision_type    = typename TypeSet::precision_type;

    using Vector            = typename TypeSet::Vector;
    using Matrix            = typename TypeSet::Matrix;

    using Linear            = typename TypeSet::Linear;

protected:
    using require_precision = typename PrecisionRequire<precision_type>::type;

    using require_vector    = typename VectorRequire<Vector>::type;
    using require_matrix    = typename MatrixRequire<Matrix>::type;

    using require_linear    = typename LinearRequire<Linear>::type;
};

} // namespace trixy

#include <Trixy/Neuro/Detail/MacroUnscope.hpp>

#endif // TRIXY_REGRESSION_REQUIRE_HPP

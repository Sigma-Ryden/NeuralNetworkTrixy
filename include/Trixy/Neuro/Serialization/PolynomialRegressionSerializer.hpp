#ifndef POLYNOMIAL_REGRESSION_SERIALIZER_HPP
#define POLYNOMIAL_REGRESSION_SERIALIZER_HPP

#include <fstream> // ifstream, ofstream

#include "Trixy/Neuro/Serialization/BaseSerializer.hpp"
#include "Trixy/Neuro/Detail/TrixyNetMeta.hpp"

#include "Trixy/Neuro/Detail/MacroScope.hpp"

namespace trixy
{

TRIXY_SERIALIZER_TPL_DECLARATION
class TRIXY_SERIALIZER_TPL(meta::is_polynomial_regression)
{
public:
    using Tensor1D       = typename Serializable::Tensor1D;

    using precision_type = typename Serializable::precision_type;
    using size_type      = typename Serializable::size_type;

private:
    Tensor1D  W;         ///< Inner weight
    size_type N;         ///< Size of weight vector (same as power size + 1)

public:
    Serializer() = default;

    void prepare(const Serializable& reg);

    void serialize(std::ofstream& out) const;
    void deserialize(std::ifstream& in);

    const Tensor1D& getWeight() const noexcept { return W; };
    size_type getPower() const noexcept { return N; };
};

TRIXY_SERIALIZER_TPL_DECLARATION
void TRIXY_SERIALIZER_TPL(meta::is_polynomial_regression)::prepare(const Serializable& reg)
{
    W = reg.getInnerWeight();
    N = reg.getInnerPower();
}

TRIXY_SERIALIZER_TPL_DECLARATION
void TRIXY_SERIALIZER_TPL(meta::is_polynomial_regression)::serialize(std::ofstream& out) const
{
    out.write(reinterpret_cast<const char*>(&N), sizeof(size_type));

    for(size_type i = 0; i < W.size(); ++i)
        out.write(reinterpret_cast<const char*>(&W(i)), sizeof(precision_type));
}

TRIXY_SERIALIZER_TPL_DECLARATION
void TRIXY_SERIALIZER_TPL(meta::is_polynomial_regression)::deserialize(std::ifstream& in)
{
    in.read(reinterpret_cast<char*>(&N), sizeof(size_type));

    W.resize(N + 1);

    for(size_type i = 0; i < W.size(); ++i)
        in.read(reinterpret_cast<char*>(&W(i)), sizeof(precision_type));
}

} // namespace trixy

#include "Trixy/Neuro/Detail/MacroUnscope.hpp"

#endif // POLYNOMIAL_REGRESSION_SERIALIZER_HPP
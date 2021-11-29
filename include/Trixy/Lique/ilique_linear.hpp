#ifndef ILIQUE_LINEAR_HPP
#define ILIQUE_LINEAR_HPP

#include <cstddef> // size_t

namespace trixy
{

namespace ilique
{

template <class Tensor1D, class Tensor2D>
class ILinear
{
protected:
    virtual ~ILinear() = default;

public:
    virtual void dot(Tensor1D& buff,
                     const Tensor1D& vector,
                     const Tensor2D& matrix) const noexcept = 0;

    virtual void dottranspose(Tensor1D& buff,
                              const Tensor1D& vector,
                              const Tensor2D& matrix) const noexcept = 0;

    virtual void tensordot(Tensor2D& buff,
                           const Tensor1D& col_vector,
                           const Tensor1D& row_vector) const noexcept = 0;
};

} // namespace ilique

} // namespace trixy

#endif // ILIQUE_LINEAR_HPP

#ifndef LIQUE_BASE_TENSOR_HPP
#define LIQUE_BASE_TENSOR_HPP

namespace lique
{

struct TensorType
{
    struct vector;
    struct matrix;
};

template <typename Precision, typename tensor_type, typename enable = void>
class Tensor;

} // namespace lique

#endif // LIQUE_BASE_TENSOR_HPP

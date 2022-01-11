// This file contains all internal macro definitions
// You MUST include macro_unscope.hpp at the end of *.hpp to undef all of them

#define LIQUE_TENSOR_TPL_DECLARATION                                                                    \
    template <typename Precision>

#define LIQUE_TENSOR_TPL(tensor_type)                                                                   \
    Tensor<Precision, tensor_type,                                                                      \
        typename std::enable_if<std::is_arithmetic<Precision>::value>::type>

#define LIQUE_LINEAR_TPL_DECLARATION                                                                    \
    template <template <typename, typename...> class Vector,                                            \
              template <typename, typename...> class Matrix,                                            \
              typename Precision,                                                                       \
              typename... Args>

#define LIQUE_LINEAR_TPL                                                                                \
    Linear<Vector, Matrix, Precision,                                                                   \
           typename std::enable_if<std::is_arithmetic<Precision>::value>::type, Args...>

#define LIQUE_FUNCTION_TPL                                                                              \
    template <typename Precision,                                                                       \
              typename std::enable_if<std::is_arithmetic<Precision>::value, int>::type = 0>

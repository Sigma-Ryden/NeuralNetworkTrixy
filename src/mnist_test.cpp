#include <cstdlib> // rand, srand, size_t
#include <ctime> // time
#include <iostream> // cin, cout
#include <iomanip> // setprecision, fixed

#include "Trixy/Lique/lique_linear.hpp" // Linear
#include "Trixy/Lique/lique_matrix.hpp" // Matrix
#include "Trixy/Lique/lique_vector.hpp" // Vector

#include "Trixy/Collection/collection.hpp" // Collection

#include "Trixy/Neuro/neuro_core.hpp" // Neuro, Activation, Loss, Optimization
#include "Trixy/Neuro/neuro_functional.hpp" // get
#include "Trixy/Neuro/neuro_optimizer.hpp" // get
#include "MnistMaster/mnist_reader.hpp" // read_dataset

#include "Timer/timer.h" // Timer

namespace var_collection
{

int D  = 0;
int C  = 0;
int CC = 0;
int M  = 0;

}
namespace var_vector
{

int D  = 0;
int C  = 0;
int CC = 0;
int M  = 0;

}
namespace var_matrix
{

int D  = 0;
int C  = 0;
int CC = 0;
int M  = 0;

}

namespace tr = trixy;
namespace li = lique;

template <typename Precision>
Precision random_real()
{
    static int within = 1000;
    return static_cast<Precision>(std::rand() % (2 * within + 1) - within) / (within * within);
}

template <typename Precision>
Collection<li::Vector<Precision>> initialize_i(
    const std::vector<std::vector<unsigned char>>& data, std::size_t batch_size, std::size_t input_size)
{
    Collection<li::Vector<Precision>> input_batch(batch_size);
    for(std::size_t i = 0; i < batch_size; ++i)
        input_batch[i] = li::Vector<Precision>(input_size);

    for(std::size_t i = 0; i < batch_size; ++i)
        for(std::size_t j = 0; j < input_size; ++j)
            input_batch[i](j) = data[i][j] / 255.0;

    return input_batch;
}

template <typename Precision>
Collection<li::Vector<Precision>> initialize_o(
    const std::vector<unsigned char>& data, std::size_t batch_size, std::size_t output_size)
{
    Collection<li::Vector<Precision>> output_batch(batch_size);
    for(std::size_t i = 0; i < batch_size; ++i)
        output_batch[i] = li::Vector<Precision>(output_size);

    for(std::size_t i = 0; i < batch_size; ++i)
        for(std::size_t j = 0; j < output_size; ++j)
            if(data[i] == j) output_batch[i](j) = 1;
            else output_batch[i](j) = 0;

    return output_batch;
}

template <typename Precision>
void mnist_test()
{
//  Data preparing:
    auto dataset = mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>("C:/mnist_data/");

    std::size_t train_batch_size = 60000;
    std::size_t test_batch_size = 10000;
    std::size_t input_size = 784;
    std::size_t out_size = 10;

//  Train batch initialize:
    Collection<li::Vector<Precision>> train_in = initialize_i<Precision>(dataset.training_images, train_batch_size, input_size);
    Collection<li::Vector<Precision>> train_out = initialize_o<Precision>(dataset.training_labels, train_batch_size, out_size);

//  Test batch initialize:
    Collection<li::Vector<Precision>> test_in = initialize_i<Precision>(dataset.test_images, test_batch_size, input_size);
    Collection<li::Vector<Precision>> test_out = initialize_o<Precision>(dataset.test_labels, test_batch_size, out_size);

    var_vector::D  = 0;
    var_vector::C  = 0;
    var_vector::CC = 0;
    var_vector::M  = 0;
//  Show image:
   //show_image_batch(train_in);

//  NeuralNetwork preparing:
    using NeuralFeedForward = trixy::Neuro<li::Vector, li::Matrix, li::Linear, Collection, Precision>;

//  NeuralNetwork topology:
    NeuralFeedForward network = { input_size, 256, out_size };

    network.initializeInnerStruct(random_real);

    network.setActivationFunction(tr::get<tr::function::Activation, li::Vector, Precision>("relu"));
    network.setNormalizationFunction(tr::get<tr::function::Activation, li::Vector, Precision>("softmax"));

    network.setLossFunction(tr::get<tr::function::Loss, li::Vector, Precision>("CCE"));
    //network.setOptimizationFunction(tr::get<tr::function::Optimization, li::Vector, li::Matrix, Precision>("ada_grad"));

//  Train network:
    Timer t;
    //
    std::size_t times = 20;
    for(std::size_t i = 0; i < times; ++i)
    {
        std::cout << "start train [" << i << "]:\n";
        network.trainMiniBatch(train_in, train_out, 0.1, 10, 32, std::rand);
        //network.trainOptimize(train_in, train_out, 0.1, 20, 250, std::rand);
        //if (i % 5 == 0) std::cout << "Accuracy: " << network.accuracy(train_in, train_out) << '\n';
        //network.trainStochastic(train_in, train_out, 0.5, 1000, std::rand);
    }
    std::cout << t.elapsed() << '\n';
    t.reset();

    std::cout << "\nPROFILING >>>\n";

    std::cout << "Collection D: " << var_collection::D << '\n';
    std::cout << "Collection C: " << var_collection::C << '\n';
    std::cout << "Collection CC: " << var_collection::CC << '\n';
    std::cout << "Collection M: " << var_collection::M << '\n';

    std::cout << "Vector D: " << var_vector::D << '\n';
    std::cout << "Vector C: " << var_vector::C << '\n';
    std::cout << "Vector CC: " << var_vector::CC << '\n';
    std::cout << "Vector M: " << var_vector::M << '\n';

    std::cout << "Matrix D: " << var_matrix::D << '\n';
    std::cout << "Matrix C: " << var_matrix::C << '\n';
    std::cout << "Matrix CC: " << var_matrix::CC << '\n';
    std::cout << "Matrix M: " << var_matrix::M << '\n';

//  Test train_batch aft
    std::cout << "NNetwork train loss: " << network.loss(train_in, train_out) << '\n';
    std::cout << "NNetwork tarin normal accuracy: " << network.accuracy(train_in, train_out) << '\n';
    std::cout << "NNetwork tarin global accuracy: " << network.accuracyGlobal(train_in, train_out, 0.25) << '\n';
    std::cout << "NNetwork tarin full accuracy: " << network.accuracyFull(train_in, train_out, 0.25) << '\n';
//  Test test_batch aft
    std::cout << "NNetwork test loss: " << network.loss(test_in, test_out) << '\n';
    std::cout << "NNetwork test normal accuracy: " << network.accuracy(test_in, test_out) << '\n';
    std::cout << "NNetwork test global accuracy: " << network.accuracyGlobal(test_in, test_out, 0.25) << '\n';
    std::cout << "NNetwork test full accuracy: " << network.accuracyFull(test_in, test_out, 0.25) << '\n';
    std::cout << t.elapsed() << '\n';
}
//
int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    std::cout << std::fixed << std::setprecision(6);

    mnist_test<float>();

    std::cin.get();

    return 0;
}
//
/*
FLOAT:
4.810986
NNetwork tarin normal accuracy: 0.809367
25.023176
NNetwork test normal accuracy: 0.810200
4.186155

DOUBLE:
7.028966
NNetwork tarin normal accuracy: 0.749817
27.647358
NNetwork test normal accuracy: 0.753600
4.607892
*/
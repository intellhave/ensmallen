/**
 * @file ada_grad.hpp
 * @author Abhinav Moudgil
 *
 * Implementation of the Adagrad optimizer. Adagrad is an optimizer that
 * chooses learning rate dynamically by adapting to the data. Hence Adagrad
 * eliminates the need to manually tune the learning rate.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_CORE_OPTIMIZERS_ADAGRAD_ADA_GRAD_HPP
#define MLPACK_CORE_OPTIMIZERS_ADAGRAD_ADA_GRAD_HPP

#include <mlpack/prereqs.hpp>
#include <mlpack/core/optimizers/sgd/sgd.hpp>
#include "ada_grad_update.hpp"

namespace mlpack {
namespace optimization {

/**
 * Adagrad is a modified version of stochastic gradient descent which performs
 * larger updates for more sparse parameters and smaller updates for less sparse
 * parameters.
 *
 * For more information, see the following.
 *
 * @code
 * @article{duchi2011adaptive,
 *   author    = {Duchi, John and Hazan, Elad and Singer, Yoram},
 *   title     = {Adaptive subgradient methods for online learning and stochastic optimization},
 *   journal   = {Journal of Machine Learning Research},
 *   volume    = {12},
 *   number    = {Jul},
 *   pages     = {2121--2159},
 *   year      = {2011}
 * }
 * @endcode
 *
 * For AdaGrad to work, a DecomposableFunctionType template parameter is
 * required. This class must implement the following function:
 *
 *   size_t NumFunctions();
 *   double Evaluate(const arma::mat& coordinates, const size_t i);
 *   void Gradient(const arma::mat& coordinates,
 *                 const size_t i,
 *                 arma::mat& gradient);
 *
 * NumFunctions() should return the number of functions (\f$n\f$), and in the
 * other two functions, the parameter i refers to which individual function (or
 * gradient) is being evaluated.  So, for the case of a data-dependent function,
 * such as NCA (see mlpack::nca::NCA), NumFunctions() should return the number
 * of points in the dataset, and Evaluate(coordinates, 0) will evaluate the
 * objective function on the first point in the dataset (presumably, the dataset
 * is held internally in the DecomposableFunctionType).
 *
 * @tparam DecomposableFunctionType Decomposable objective function type to be
 *     minimized.
 */
template<typename DecomposableFunctionType>
class AdaGrad
{
 public:
  /**
   * Construct the AdaGrad optimizer with the given function and parameters.
   * The defaults here are not necessarily good for the given problem, so it is
   * suggested that the values used be tailored to the task at hand. The
   * maximum number of iterations refers to the maximum number of points that
   * are processed (i.e., one iteration equals one point; one iteration does not
   * equal one pass over the dataset).
   *
   * @param function Function to be optimized (minimized).
   * @param stepSize Step size for each iteration
   * @param epsilon Value used to initialise the squared gradient parameter.
   * @param maxIterations Maximum number of iterations allowed (0 means no
   *        limit).
   * @param tolerance Maximum absolute tolerance to terminate algorithm.
   * @param shuffle If true, the function order is shuffled; otherwise, each
   *        function is visited in linear order.
   */
  AdaGrad(DecomposableFunctionType& function,
      const double stepSize = 0.01,
      const double epsilon = 1e-8,
      const size_t maxIterations = 100000,
      const double tolerance = 1e-5,
      const bool shuffle = true)
  {
    AdaGradUpdate adagradUpdate(epsilon);
    optimizer = SGD<DecomposableFunctionType, AdaGradUpdate>(function, stepSize, maxIterations,
        tolerance, shuffle, adagradUpdate);  
  }

  /**
   * Optimize the given function using AdaGrad. The given starting point will
   * be modified to store the finishing point of the algorithm, and the final
   * objective value is returned.
   *
   * @param iterate Starting point (will be modified).
   * @return Objective value of the final point.
   */
  double Optimize(arma::mat& iterate)
  {
    return optimizer.Optimize(iterate);
  }

  //! Get the instantiated function to be optimized.
  const DecomposableFunctionType& Function() const { return function; }
  //! Modify the instantiated function.
  DecomposableFunctionType& Function() { return function; }

  //! Get the step size.
  double StepSize() const { return optimizer.StepSize(); }
  //! Modify the step size.
  double& StepSize() { return optimizer.StepSize(); }

  //! Get the value used to initialise the squared gradient parameter.
  double Epsilon() const { return optimizer.AdaGradUpdate.Epsilon(); }
  //! Modify the value used to initialise the squared gradient parameter.
  double& Epsilon() { return optimizer.AdaGradUpdate.Epsilon(); }

  //! Get the maximum number of iterations (0 indicates no limit).
  size_t MaxIterations() const { return optimizer.MaxIterations(); }
  //! Modify the maximum number of iterations (0 indicates no limit).
  size_t& MaxIterations() { return optimizer.MaxIterations(); }

  //! Get the tolerance for termination.
  double Tolerance() const { return optimizer.Tolerance(); }
  //! Modify the tolerance for termination.
  double& Tolerance() { return optimizer.Tolerance(); }

  //! Get whether or not the individual functions are shuffled.
  bool Shuffle() const { return optimizer.Shuffle(); }
  //! Modify whether or not the individual functions are shuffled.
  bool& Shuffle() { return optimizer.Shuffle(); }

 private:
  //! The instantiated function.
  DecomposableFunctionType& function;

  //! The step size for each example.
  double stepSize;

  //! The value used to initialise the squared gradient parameter.
  double epsilon;

  //! The maximum number of allowed iterations.
  size_t maxIterations;

  //! The tolerance for termination.
  double tolerance;

  //! Controls whether or not the individual functions are shuffled when
  //! iterating.
  bool shuffle;

  //! Stochastic Gradient Descent object with AdaGrad policy.
  SGD<DecomposableFunctionType, AdaGradUpdate> optimizer;
};

} // namespace optimization
} // namespace mlpack

#endif

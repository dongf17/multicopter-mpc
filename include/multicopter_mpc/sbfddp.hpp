#ifndef MULTICOPTER_MPC_SBFDDP_HPP_
#define MULTICOPTER_MPC_SBFDDP_HPP_

#include <Eigen/Dense>

#include "crocoddyl/core/solver-base.hpp"
#include "crocoddyl/core/solvers/ddp.hpp"
#include "crocoddyl/core/solvers/fddp.hpp"
#include "crocoddyl/core/optctrl/shooting.hpp"
#include "crocoddyl/core/actuation/squashing/smooth-sat.hpp"
#include "crocoddyl/core/actuation/actuation-squashing.hpp"
#include "crocoddyl/core/activations/quadratic-barrier.hpp"
#include "crocoddyl/core/activations/weighted-quadratic-barrier.hpp"
#include "crocoddyl/multibody/costs/control.hpp"
#include "crocoddyl/core/integrator/euler.hpp"
#include "crocoddyl/core/integrator/rk4.hpp"

#include "crocoddyl/multibody/actions/free-fwddyn.hpp"

#include "multicopter_mpc/utils/log.hpp"

namespace multicopter_mpc {
class SolverSbFDDP : public crocoddyl::SolverFDDP {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  explicit SolverSbFDDP(boost::shared_ptr<crocoddyl::ShootingProblem> problem,
                        boost::shared_ptr<crocoddyl::SquashingModelSmoothSat> squashing_model);
  virtual ~SolverSbFDDP();

  virtual bool solve(const std::vector<Eigen::VectorXd>& init_xs = crocoddyl::DEFAULT_VECTOR,
                     const std::vector<Eigen::VectorXd>& init_us = crocoddyl::DEFAULT_VECTOR,
                     const std::size_t& maxiter = 100, const bool& is_feasible = false, const double& reginit = 1e-9);

  const std::vector<Eigen::VectorXd>& getSquashControls() const;

 private:
  void barrierInit();
  void squashingUpdate();
  void barrierUpdate();

  bool solveFDDP(const std::size_t& maxiter, const bool& is_feasible, const double& reginit);
  bool solveDDP(const std::size_t& maxiter, const bool& is_feasible, const double& reginit);
  const Eigen::Vector2d& expectedImprovementDDP();
  double tryStepDDP(const double& steplength = 1);
  void forwardPassDDP(const double& steplength);

  boost::shared_ptr<crocoddyl::SquashingModelSmoothSat> squashing_model_;
  boost::shared_ptr<crocoddyl::ActuationSquashingModel> actuation_;
  boost::shared_ptr<crocoddyl::ActivationBounds> barrier_act_bounds_;
  boost::shared_ptr<crocoddyl::ActivationModelWeightedQuadraticBarrier> barrier_activation_;
  boost::shared_ptr<crocoddyl::CostModelControl> squash_barr_cost_;
  boost::shared_ptr<crocoddyl::SolverDDP> ddp_;

  // Variables for downcasting
  // models
  boost::shared_ptr<crocoddyl::IntegratedActionModelEuler> euler_;
  boost::shared_ptr<crocoddyl::IntegratedActionModelRK4> rk4_;
  boost::shared_ptr<crocoddyl::DifferentialActionModelFreeFwdDynamics> differential_;
  // datas
  boost::shared_ptr<crocoddyl::IntegratedActionDataEuler> euler_d_;
  boost::shared_ptr<crocoddyl::IntegratedActionDataRK4> rk4_d_;
  boost::shared_ptr<crocoddyl::DifferentialActionDataFreeFwdDynamics> differential_d_;
  boost::shared_ptr<crocoddyl::ActuationSquashingData> actuation_squashing_d_;

  double smooth_;
  double smooth_init_;
  double smooth_mult_;

  Eigen::VectorXd barrier_quad_weights_;
  Eigen::VectorXd barrier_quad_weights_aux_;
  double barrier_weight_;

  double convergence_;
  double convergence_init_;
  double convergence_stop_;
  double convergence_mult_;

  std::size_t max_iters_;
  double reg_init_;
  std::size_t total_iters_;

  std::vector<Eigen::VectorXd> us_squash_;  //!< Control trajectory


  double th_acceptnegstep_; // FDDP solver
};
}  // namespace multicopter_mpc

#endif
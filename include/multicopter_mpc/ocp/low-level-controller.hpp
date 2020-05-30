#ifndef MULTICOPTER_MPC_OCP_LOW_LEVEL_CONTROLLER_HPP_
#define MULTICOPTER_MPC_OCP_LOW_LEVEL_CONTROLLER_HPP_

#include <cassert>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "multicopter_mpc/ocp-base.hpp"
#include "multicopter_mpc/multicopter-base-params.hpp"

namespace multicopter_mpc {

class LowLevelController : public OcpAbstract {
 public:
  LowLevelController(const boost::shared_ptr<pinocchio::Model> model,
                     const boost::shared_ptr<MultiCopterBaseParams>& mc_params, const double& dt,
                     std::size_t& n_knots);
  ~LowLevelController();

  virtual void createProblem(const SolverTypes::Type& solver_type);

  void setInitialState(const Eigen::Ref<Eigen::VectorXd>& initial_state);
  void setReferenceStateTrajectory(const std::vector<Eigen::VectorXd>& state_trajectory);
  void updateReferenceStateTrajectory(const Eigen::Ref<Eigen::VectorXd>& state_new);

  virtual void solve() override;
 private:
  boost::shared_ptr<crocoddyl::DifferentialActionModelFreeFwdDynamics> createDifferentialModel(
      const unsigned int& trajectory_idx);
  boost::shared_ptr<crocoddyl::CostModelAbstract> createCostState(const unsigned int& trajectory_idx);
  boost::shared_ptr<crocoddyl::CostModelAbstract> createCostControlRegularization();

  std::vector<Eigen::VectorXd> state_ref_;
  Eigen::VectorXd state_init_;
};
}  // namespace multicopter_mpc

#endif

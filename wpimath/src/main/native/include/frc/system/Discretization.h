// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/EigenCore.h"
#include "units/time.h"
#include "unsupported/Eigen/MatrixFunctions"

namespace frc {

/**
 * Discretizes the given continuous A matrix.
 *
 * @tparam States Number of states.
 * @param contA Continuous system matrix.
 * @param dt    Discretization timestep.
 * @param discA Storage for discrete system matrix.
 */
template <int States>
void DiscretizeA(const Matrixd<States, States>& contA, units::second_t dt,
                 Matrixd<States, States>* discA) {
  *discA = (contA * dt.value()).exp();
}

/**
 * Discretizes the given continuous A and B matrices.
 *
 * @tparam States Number of states.
 * @tparam Inputs Number of inputs.
 * @param contA Continuous system matrix.
 * @param contB Continuous input matrix.
 * @param dt    Discretization timestep.
 * @param discA Storage for discrete system matrix.
 * @param discB Storage for discrete input matrix.
 */
template <int States, int Inputs>
void DiscretizeAB(const Matrixd<States, States>& contA,
                  const Matrixd<States, Inputs>& contB, units::second_t dt,
                  Matrixd<States, States>* discA,
                  Matrixd<States, Inputs>* discB) {
  // Matrices are blocked here to minimize matrix exponentiation calculations
  Matrixd<States + Inputs, States + Inputs> Mcont;
  Mcont.setZero();
  Mcont.template block<States, States>(0, 0) = contA * dt.value();
  Mcont.template block<States, Inputs>(0, States) = contB * dt.value();

  // Discretize A and B with the given timestep
  Matrixd<States + Inputs, States + Inputs> Mdisc = Mcont.exp();
  *discA = Mdisc.template block<States, States>(0, 0);
  *discB = Mdisc.template block<States, Inputs>(0, States);
}

/**
 * Discretizes the given continuous A and Q matrices.
 *
 * @tparam States Number of states.
 * @param contA Continuous system matrix.
 * @param contQ Continuous process noise covariance matrix.
 * @param dt    Discretization timestep.
 * @param discA Storage for discrete system matrix.
 * @param discQ Storage for discrete process noise covariance matrix.
 */
template <int States>
void DiscretizeAQ(const Matrixd<States, States>& contA,
                  const Matrixd<States, States>& contQ, units::second_t dt,
                  Matrixd<States, States>* discA,
                  Matrixd<States, States>* discQ) {
  // Make continuous Q symmetric if it isn't already
  Matrixd<States, States> Q = (contQ + contQ.transpose()) / 2.0;

  // Set up the matrix M = [[-A, Q], [0, A.T]]
  Matrixd<2 * States, 2 * States> M;
  M.template block<States, States>(0, 0) = -contA;
  M.template block<States, States>(0, States) = Q;
  M.template block<States, States>(States, 0).setZero();
  M.template block<States, States>(States, States) = contA.transpose();

  Matrixd<2 * States, 2 * States> phi = (M * dt.value()).exp();

  // Phi12 = phi[0:States,        States:2*States]
  // Phi22 = phi[States:2*States, States:2*States]
  Matrixd<States, States> phi12 = phi.block(0, States, States, States);
  Matrixd<States, States> phi22 = phi.block(States, States, States, States);

  *discA = phi22.transpose();

  Q = *discA * phi12;

  // Make discrete Q symmetric if it isn't already
  *discQ = (Q + Q.transpose()) / 2.0;
}

/**
 * Discretizes the given continuous A and Q matrices.
 *
 * Rather than solving a 2N x 2N matrix exponential like in DiscretizeAQ()
 * (which is expensive), we take advantage of the structure of the block matrix
 * of A and Q.
 *
 * 1) The exponential of A*t, which is only N x N, is relatively cheap.
 * 2) The upper-right quarter of the 2N x 2N matrix, which we can approximate
 *    using a taylor series to several terms and still be substantially cheaper
 *    than taking the big exponential.
 *
 * @tparam States Number of states.
 * @param contA Continuous system matrix.
 * @param contQ Continuous process noise covariance matrix.
 * @param dt    Discretization timestep.
 * @param discA Storage for discrete system matrix.
 * @param discQ Storage for discrete process noise covariance matrix.
 */
template <int States>
void DiscretizeAQTaylor(const Matrixd<States, States>& contA,
                        const Matrixd<States, States>& contQ,
                        units::second_t dt, Matrixd<States, States>* discA,
                        Matrixd<States, States>* discQ) {
  // Make continuous Q symmetric if it isn't already
  Matrixd<States, States> Q = (contQ + contQ.transpose()) / 2.0;

  Matrixd<States, States> lastTerm = Q;
  double lastCoeff = dt.value();

  // Aᵀⁿ
  Matrixd<States, States> Atn = contA.transpose();

  Matrixd<States, States> phi12 = lastTerm * lastCoeff;

  // i = 6 i.e. 5th order should be enough precision
  for (int i = 2; i < 6; ++i) {
    lastTerm = -contA * lastTerm + Q * Atn;
    lastCoeff *= dt.value() / static_cast<double>(i);

    phi12 += lastTerm * lastCoeff;

    Atn *= contA.transpose();
  }

  DiscretizeA<States>(contA, dt, discA);
  Q = *discA * phi12;

  // Make discrete Q symmetric if it isn't already
  *discQ = (Q + Q.transpose()) / 2.0;
}

/**
 * Returns a discretized version of the provided continuous measurement noise
 * covariance matrix.
 *
 * @tparam Outputs Number of outputs.
 * @param R  Continuous measurement noise covariance matrix.
 * @param dt Discretization timestep.
 */
template <int Outputs>
Matrixd<Outputs, Outputs> DiscretizeR(const Matrixd<Outputs, Outputs>& R,
                                      units::second_t dt) {
  return R / dt.value();
}

}  // namespace frc

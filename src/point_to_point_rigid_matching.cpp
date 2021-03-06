#include "point_to_point_rigid_matching.h"
#include "closest_rotation.h"
#include "vect_to_skew.h"
#include <Eigen/Dense>

void point_to_point_rigid_matching(
  const Eigen::MatrixXd & X,
  const Eigen::MatrixXd & P,
  Eigen::Matrix3d & R,
  Eigen::RowVector3d & t)
{
    int k = X.rows();
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(3*k, 6);

    //////////////////////////////////////////////////////
    // A = 
    // 0_k        x(i, 3)      -x(i, 2)      1_k 0_k 0_k
    // -(xi, 3)   0_k          x(i, 1)       0_k 1_k 0_k
    // x(i, 2)    -x(i, 1)     0_k           0_k 0_k 1_k
    //////////////////////////////////////////////////////
    // 0_k        x(i, 3)      -x(i, 2)      1_k 0_k 0_k
    A.block(0, 1, k, 1) = X.col(2);
    A.block(0, 2, k, 1) = - X.col(1);
    A.block(0, 3, k, 1) = Eigen::VectorXd::Ones(k);
    // -(xi, 3)   0_k          x(i, 1)       0_k 1_k 0_k
    A.block(k, 0, k, 1) = -1 * X.col(2);
    A.block(k, 2, k, 1) = X.col(0);
    A.block(k, 4, k, 1) = Eigen::VectorXd::Ones(k);
    // x(i, 2)    -x(i, 1)     0_k           0_k 0_k 1_k
    A.block(2 * k, 0, k, 1) = X.col(1);
    A.block(2 * k, 1, k, 1) = - X.col(0);
    A.block(2 * k, 5, k, 1) = Eigen::VectorXd::Ones(k);

    // Vectors pointing from current estimates
    Eigen::MatrixXd differences = P - X;
    Eigen::VectorXd difference_vector = Eigen::VectorXd(3*k);
    difference_vector << differences.col(0), differences.col(1), differences.col(2);

    // Closed Form solve
    difference_vector = A.transpose() * difference_vector;
    // solution_vector = [α β γ] (rotation matrix) + [t_1, t_2, t_3] (translation)
    Eigen::VectorXd solution_vector = (A.transpose() * A).inverse() * difference_vector;

    // Translation
    t << solution_vector(3), solution_vector(4), solution_vector(5);

    // Linearized rotation; find closest actual rotation
    Eigen::Matrix3d rotation_like = Eigen::Matrix3d::Identity() + vect_to_skew(solution_vector);
    closest_rotation(rotation_like.transpose(), R);
}


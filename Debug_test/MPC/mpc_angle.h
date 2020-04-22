/*
 * @Author: Yaodecheng
 * @Date: 2020-04-12 16:33:16
 * @LastEditTime: 2020-04-14 11:26:51
 * @LastEditors: Yaodecheng
 * @Description: 
 * @Adeall licence@2020
 */
/*
 * @Author: Yaodecheng
 * @Date: 2020-04-12 16:33:16
 * @LastEditTime: 2020-04-13 13:43:09
 * @LastEditors: Yaodecheng
 */
#include "Eigen/Core"
template <typename T>
T Clamp(const T value, T bound1, T bound2)
{
    if (bound1 > bound2)
    {
        std::swap(bound1, bound2);
    }

    if (value < bound1)
    {
        return bound1;
    }
    else if (value > bound2)
    {
        return bound2;
    }
    return value;
}
class MPC_Controller
{
private:
    // the following parameters are vehicle physics related.
    // control time interval
    double ts_ = 0.05;
    // corner stiffness; front
    double cf_ = 155494;
    // corner stiffness; rear
    double cr_ = 155494;
    // distance between front and rear wheel center
    double wheelbase_ = 1500.0;
    // mass of the vehicle
    double mass_ = 1300.0;
    // distance from front wheel center to COM
    double lf_ = 400.0;
    // distance from rear wheel center to COM
    double lr_ = 900.0;
    // rotational inertia
    double iz_ = 1000.0;
    // the ratio between the turn of the steering wheel and the turn of the wheels
    double steer_transmission_ratio_ = 0.0;
    // the maximum turn of steer
    double steer_single_direction_max_degree_ = 1.4;

    // limit steering to maximum theoretical lateral acceleration
    double max_lat_acc_ = 0.1;

    // number of states, includes
    // lateral error, lateral error rate, heading error, heading error rate,
    // station error, velocity error,
    const int basic_state_size_ = 6;

    const int controls_ = 2;

    const int horizon_ = 10;
    // vehicle state matrix
    Eigen::MatrixXd matrix_a_;
    // vehicle state matrix (discrete-time)
    Eigen::MatrixXd matrix_ad_;

    // control matrix
    Eigen::MatrixXd matrix_b_;
    // control matrix (discrete-time)
    Eigen::MatrixXd matrix_bd_;

    // offset matrix
    Eigen::MatrixXd matrix_c_;
    // offset matrix (discrete-time)
    Eigen::MatrixXd matrix_cd_;

    // gain matrix
    Eigen::MatrixXd matrix_k_;
    // control authority weighting matrix
    Eigen::MatrixXd matrix_r_;
    // updated control authority weighting matrix
    Eigen::MatrixXd matrix_r_updated_;
    // state weighting matrix
    Eigen::MatrixXd matrix_q_;
    // updated state weighting matrix
    Eigen::MatrixXd matrix_q_updated_;
    // vehicle state matrix coefficients
    Eigen::MatrixXd matrix_a_coeff_;
    // 4 by 1 matrix; state matrix
    Eigen::MatrixXd matrix_state_;

    // heading error of last control cycle
    double previous_heading_error_ = 0.0;
    // lateral distance to reference trajectory of last control cycle
    double previous_lateral_error_ = 0.0;

    // parameters for mpc solver; number of iterations
    int mpc_max_iteration_ = 10;
    // parameters for mpc solver; threshold for computation
    double mpc_eps_ = 0.01;

    const std::string name_;

    double standstill_acceleration_ = 0.0;

    double throttle_deadzone_ = 0.0;

    double brake_deadzone_ = 0.0;

    double max_acceleration_ = 0.1;

    double max_deceleration_ = 0.1;

public:
    MPC_Controller(/* args */);
    void UpdateMatrix(double heading_error_rate, double linear_velocity);
    void UpdateStateAnalyticalMatching(double lateral_error, double lateral_error_rate, double heading_error, double heading_error_rate, double station_error, double speed_error);
    void ComputeControlCommand(double Position_Error, double Angle_Error, double target_dis, double &speed, double &turnangle);
    void Init();
    ~MPC_Controller();
};

void MPC_angle_control(double Position_Error, double Angle_Error, double &speed, double &turnangle);

class MY_MPC_Controller
{
private:
    // the following parameters are vehicle physics related.
    // control time interval
    double ts_ = 0.05;
    double L_ = 135; //车轮轴距
    double Rr = 1;
    double w = 1;
    double vd1 = Rr * w; //% For circular trajectory，参考系统的纵向速度
    double vd2 = 0;      //%参考系统的前轮偏角

    // number of states, includes
    // lateral error, lateral error rate, heading error, heading error rate,
    // station error, velocity error,
    const int basic_state_size_ = 3;

    const int controls_ = 2;

    const int horizon_ = 10;
    // vehicle state matrix
    Eigen::MatrixXd matrix_a_;
    // vehicle state matrix (discrete-time)
    Eigen::MatrixXd matrix_ad_;

    // control matrix
    Eigen::MatrixXd matrix_b_;
    // control matrix (discrete-time)
    Eigen::MatrixXd matrix_bd_;

    // offset matrix
    Eigen::MatrixXd matrix_c_;
    // offset matrix (discrete-time)
    Eigen::MatrixXd matrix_cd_;

    // gain matrix
    Eigen::MatrixXd matrix_k_;
    // control authority weighting matrix
    Eigen::MatrixXd matrix_r_;
    // updated control authority weighting matrix
    Eigen::MatrixXd matrix_r_updated_;
    // state weighting matrix
    Eigen::MatrixXd matrix_q_;
    // updated state weighting matrix
    Eigen::MatrixXd matrix_q_updated_;
    // vehicle state matrix coefficients
    Eigen::MatrixXd matrix_a_coeff_;
    // 4 by 1 matrix; state matrix
    Eigen::MatrixXd matrix_state_;

    // heading error of last control cycle
    double previous_heading_error_ = 0.0;
    // lateral distance to reference trajectory of last control cycle
    double previous_lateral_error_ = 0.0;

    // parameters for mpc solver; number of iterations
    int mpc_max_iteration_ = 10;
    // parameters for mpc solver; threshold for computation
    double mpc_eps_ = 0.01;

    const std::string name_;
    public:
    MY_MPC_Controller(/* args */);
    void UpdateMatrix(double heading_error_rate, double linear_velocity);
    void UpdateStateAnalyticalMatching(double lateral_error, double lateral_error_rate, double heading_error, double heading_error_rate, double station_error, double speed_error);
    void ComputeControlCommand(double Position_Error, double Angle_Error, double target_dis, double &speed, double &turnangle);
    void Init();
    ~MY_MPC_Controller();
};
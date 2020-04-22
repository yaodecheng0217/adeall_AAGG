/*
 * @Author: Yaodecheng
 * @Date: 2020-04-12 16:33:35
 * @LastEditTime: 2020-04-14 21:53:33
 * @LastEditors: Yaodecheng
 */
#include "mpc_angle.h"
#include "mpc_solver.h"
#include "Eigen/LU"
#include <iostream>
#include "readconfig/readjson.h"
using namespace apollo;
using namespace common;
using namespace math;
void MPC_angle_control(double Position_Error, double Angle_Error, double &speed, double &turnangle)
{
    const int STATES = 4;
    int CONTROLS = 2;
    const int HORIZON = 10;
    const double EPS = 0.01;
    const int MAX_ITER = 100;

    //A阵状态转移矩阵
    Eigen::MatrixXd A(STATES, STATES);
    A << 1, 0, 1, 0,
        0, 1, 0, 1,
        0, 0, 1, 0,
        0, 0, 0, 1;
    //B阵控制矩阵
    Eigen::MatrixXd B(STATES, CONTROLS);
    B << 0, 1,
        0, 0,
        1, 0,
        0, 1;
    //C阵 干扰矩阵
    Eigen::MatrixXd C(STATES, 1);
    C << 0,
        0,
        0,
        0.1;
    //权重矩阵R
    Eigen::MatrixXd Q(STATES, STATES);
    Q << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0;
    //权重矩阵R
    Eigen::MatrixXd R(CONTROLS, CONTROLS);
    R << 1, 0,
        0, 1;
    //控制下界
    Eigen::MatrixXd lower_bound(CONTROLS, 1);
    lower_bound << -10, -10;
    //控制上界
    Eigen::MatrixXd upper_bound(CONTROLS, 1);
    upper_bound << 10, 10;
    //初始状态==========================================================即输入===========
    Eigen::MatrixXd initial_state(STATES, 1);
    initial_state(0, 0) = Position_Error / 100.0; //横向误差
    initial_state(0, 0) = 0;                      //横向误差变化率
    initial_state(0, 0) = -Angle_Error;           //航向误差
    initial_state(0, 0) = 0;                      //航向误差变化率

    //参考状态
    Eigen::MatrixXd reference_state(STATES, 1);
    reference_state << 0, 0, 0, 0; //参考误差为0，即无误差

    std::vector<Eigen::MatrixXd> reference(HORIZON, reference_state);

    Eigen::MatrixXd control_matrix(CONTROLS, 1);
    control_matrix << turnangle, 0; //控制量为当前车辆转角
    std::vector<Eigen::MatrixXd> control(HORIZON, control_matrix);

    for (unsigned int i = 1; i < control.size(); ++i)
    {
        control[i - 1] = control[i];
    }
    control[HORIZON - 1] = control_matrix;

    SolveLinearMPC(A, B, C, Q, R, lower_bound, upper_bound, initial_state,
                   reference, EPS, MAX_ITER, &control);
    turnangle = control[0](0, 0);
    //解mpc，输出一个vector，control内有10个control_matrix
}

MPC_Controller::MPC_Controller(/* args */)
{
    Init();
}

MPC_Controller::~MPC_Controller()
{
}
using Matrix = Eigen::MatrixXd;
void MPC_Controller::Init()
{
    printf("init mpc........\n");
    // Matrix init operations.
    matrix_a_ = Matrix::Zero(basic_state_size_, basic_state_size_);
    matrix_ad_ = Matrix::Zero(basic_state_size_, basic_state_size_);
    matrix_a_(0, 1) = 1.0;
    matrix_a_(1, 2) = (cf_ + cr_) / mass_;
    matrix_a_(2, 3) = 1.0;
    matrix_a_(3, 2) = (lf_ * cf_ - lr_ * cr_) / iz_;
    // TODO(QiL): expand the model to accomendate more combined states.

    matrix_a_coeff_ = Matrix::Zero(basic_state_size_, basic_state_size_);
    matrix_a_coeff_(1, 1) = -(cf_ + cr_) / mass_;
    matrix_a_coeff_(1, 3) = (lr_ * cr_ - lf_ * cf_) / mass_;
    matrix_a_coeff_(3, 1) = (lr_ * cr_ - lf_ * cf_) / iz_;
    matrix_a_coeff_(3, 3) = -1.0 * (lf_ * lf_ * cf_ + lr_ * lr_ * cr_) / iz_;

    matrix_b_ = Matrix::Zero(basic_state_size_, controls_);
    matrix_bd_ = Matrix::Zero(basic_state_size_, controls_);
    matrix_b_(1, 0) = cf_ / mass_;
    matrix_b_(3, 0) = lf_ * cf_ / iz_;
    matrix_bd_ = matrix_b_ * ts_;

    matrix_c_ = Matrix::Zero(basic_state_size_, 1);
    matrix_cd_ = Matrix::Zero(basic_state_size_, 1);

    matrix_state_ = Matrix::Zero(basic_state_size_, 1);
    matrix_k_ = Matrix::Zero(1, basic_state_size_);

    matrix_r_ = Matrix::Identity(controls_, controls_);

    matrix_q_ = Matrix::Zero(basic_state_size_, basic_state_size_);
    printf("init 0 is ok!\n");
    int r_param_size = controls_;

    for (int i = 0; i < r_param_size; ++i)
    {
        matrix_r_(i, i) = 1;
    }
    printf("init 1 is ok!\n");
    int q_param_size = basic_state_size_;
    for (int i = 0; i < q_param_size; ++i)
    {
        matrix_q_(i, i) = 1;
    }
    printf("init 2 is ok!\n");
    // Update matrix_q_updated_ and matrix_r_updated_
    matrix_r_updated_ = matrix_r_;
    matrix_q_updated_ = matrix_q_;
    printf("init mpc is ok!\n");
}
//更新状态矩阵A，并且进行离散化
void MPC_Controller::UpdateMatrix(double heading_error_rate, double linear_velocity)
{
    //test*******************************
    //初始化状态矩阵A
    matrix_a_(0, 1) = 1.0;
    matrix_a_(1, 2) = (cf_ + cr_) / mass_;
    matrix_a_(2, 3) = 1.0;
    matrix_a_(3, 2) = (lf_ * cf_ - lr_ * cr_) / iz_;
    //与速度相关矩阵量A
    matrix_a_coeff_(1, 1) = -(cf_ + cr_) / mass_;
    matrix_a_coeff_(1, 3) = (lr_ * cr_ - lf_ * cf_) / mass_;
    matrix_a_coeff_(3, 1) = (lr_ * cr_ - lf_ * cf_) / iz_;
    matrix_a_coeff_(3, 3) = -1.0 * (lf_ * lf_ * cf_ + lr_ * lr_ * cr_) / iz_;
    //控制矩阵B
    matrix_b_(1, 0) = cf_ / mass_;
    matrix_b_(3, 0) = lf_ * cf_ / iz_;
    matrix_bd_ = matrix_b_ * ts_;
    //test***************end******************

    const double v = linear_velocity;
    matrix_a_(1, 1) = matrix_a_coeff_(1, 1) / v;
    matrix_a_(1, 3) = matrix_a_coeff_(1, 3) / v;
    matrix_a_(3, 1) = matrix_a_coeff_(3, 1) / v;
    matrix_a_(3, 3) = matrix_a_coeff_(3, 3) / v;

    Matrix matrix_i = Matrix::Identity(matrix_a_.cols(), matrix_a_.cols());
    matrix_ad_ = (matrix_i + ts_ * 0.5 * matrix_a_) *
                 (matrix_i - ts_ * 0.5 * matrix_a_).inverse();

    matrix_c_(1, 0) = (lr_ * cr_ - lf_ * cf_) / mass_ / v - v;
    matrix_c_(3, 0) = -(lf_ * lf_ * cf_ + lr_ * lr_ * cr_) / iz_ / v;
    matrix_cd_ = matrix_c_ * heading_error_rate * ts_;
}
void MPC_Controller::UpdateStateAnalyticalMatching(double lateral_error, double lateral_error_rate, double heading_error, double heading_error_rate, double station_error, double speed_error)
{

    // State matrix update;
    matrix_state_(0, 0) = lateral_error;
    matrix_state_(1, 0) = lateral_error_rate;
    matrix_state_(2, 0) = heading_error;
    matrix_state_(3, 0) = heading_error_rate;
    matrix_state_(4, 0) = station_error;
    matrix_state_(5, 0) = speed_error;
}

void MPC_Controller::ComputeControlCommand(double Position_Error, double Angle_Error, double target_dis, double &speed, double &turnangle)
{
    static ReadSeting set("mpc.json");
    set.reload();
    double Lc = 40.0;
    double sp = 40;
    set.GetValue("linear_velocity", sp);
    set.GetValue("LC", Lc);

    //获取车辆基本参数，调试用
    set.GetValue("lf_", lf_);
    set.GetValue("lr_", lr_);
    set.GetValue("iz_", iz_);
    set.GetValue("ts_", ts_);
    set.GetValue("cf_", cf_);
    set.GetValue("cr_", cr_);
    set.GetValue("wheelbase_", wheelbase_);
    set.GetValue("mass_", mass_);
    set.GetValue("steer_transmission_ratio_", steer_transmission_ratio_);
    set.GetValue("steer_single_direction_max_degree_", steer_single_direction_max_degree_);
    set.GetValue("max_lat_acc_", max_lat_acc_);

    //***************获取结束

    //换算前叉误差
    double Ferr = (Position_Error / sin(Angle_Error) + Lc) * sin(Angle_Error);
    //输入状态
    double lateral_error = Ferr * 10;
    double lateral_error_rate = speed* sin(Angle_Error);
    double heading_error = Angle_Error;
    double heading_error_rate = (previous_heading_error_ - heading_error);
    double station_error = target_dis;
    double speed_error = 0;

    double linear_velocity = speed;

    previous_lateral_error_ = lateral_error;
    previous_heading_error_ = heading_error;
    // 更新初始状态矩阵
    UpdateStateAnalyticalMatching(lateral_error, lateral_error_rate, heading_error, heading_error_rate, station_error, speed_error);
    //更新矩阵ABC
    UpdateMatrix(heading_error_rate, linear_velocity);

    //std::cout << "==>  " << matrix_state_.transpose() * 57.3 << std::endl;

    double q1 = 0, q2 = 0, q3 = 0, q4 = 0, q5 = 0, q6 = 0;
    double r1 = 1, r2 = 1;

    set.GetValue("q1", q1);
    set.GetValue("q2", q2);
    set.GetValue("q3", q3);
    set.GetValue("q4", q4);
    set.GetValue("q5", q5);
    set.GetValue("q6", q6);
    set.GetValue("r1", r1);
    set.GetValue("r2", r2);

    matrix_q_(0, 0) = q1;
    matrix_q_(1, 1) = q2;
    matrix_q_(2, 2) = q3;
    matrix_q_(3, 3) = q4;
    matrix_q_(4, 4) = q5;
    matrix_q_(5, 5) = q6;

    matrix_r_(0, 0) = r1;
    matrix_r_(1, 1) = r2;

    matrix_q_updated_ = matrix_q_;
    matrix_r_updated_ = matrix_r_;

    Eigen::MatrixXd control_matrix(controls_, 1);
    control_matrix << 0, 0;

    Eigen::MatrixXd reference_state(basic_state_size_, 1);
    reference_state << 0, 0, 0, 0, 0, 0;

    std::vector<Eigen::MatrixXd> reference(horizon_, reference_state);

    Eigen::MatrixXd lower_bound(controls_, 1);
    lower_bound << -steer_single_direction_max_degree_, max_deceleration_;

    Eigen::MatrixXd upper_bound(controls_, 1);
    upper_bound << steer_single_direction_max_degree_, max_acceleration_;

    std::vector<Eigen::MatrixXd> control(horizon_, control_matrix);

    for (unsigned int i = 1; i < control.size(); ++i)
    {
        control[i - 1] = control[i];
    }
    control[horizon_ - 1] = control_matrix;
    SolveLinearMPC(
        matrix_ad_, matrix_bd_, matrix_cd_, matrix_q_updated_,
        matrix_r_updated_, lower_bound, upper_bound, matrix_state_, reference,
        mpc_eps_, mpc_max_iteration_, &control);

    double steer_angle = control[0](0, 0);

    double steer_angle_limited = Clamp(control[0](0, 0), -steer_single_direction_max_degree_, steer_single_direction_max_degree_);

    turnangle = -steer_angle_limited;
}

//************************

MY_MPC_Controller::MY_MPC_Controller(/* args */)
{
    Init();
}

MY_MPC_Controller::~MY_MPC_Controller()
{
}
void MY_MPC_Controller::Init()
{
    printf("init mpc........\n");
    // Matrix init operations.
    matrix_a_ = Matrix::Zero(basic_state_size_, basic_state_size_);
    matrix_ad_ = Matrix::Zero(basic_state_size_, basic_state_size_);
    matrix_a_(0, 0) = 1.0;
    matrix_a_(1, 1) = 1.0;
    matrix_a_(2, 2) = 1.0;

    // TODO(QiL): expand the model to accomendate more combined states.

    matrix_a_coeff_ = Matrix::Zero(basic_state_size_, basic_state_size_);
    matrix_a_coeff_ = matrix_a_;

    matrix_b_ = Matrix::Zero(basic_state_size_, controls_);
    matrix_bd_ = Matrix::Zero(basic_state_size_, controls_);
    matrix_b_(2, 1) = 1.0;

    matrix_bd_ = matrix_b_ * ts_;

    matrix_c_ = Matrix::Zero(basic_state_size_, 1);
    matrix_c_(2, 0) = 1.0;
    matrix_cd_ = Matrix::Zero(basic_state_size_, 1);

    matrix_state_ = Matrix::Zero(basic_state_size_, 1);
    matrix_k_ = Matrix::Zero(1, basic_state_size_);

    matrix_r_ = Matrix::Identity(controls_, controls_);

    matrix_q_ = Matrix::Zero(basic_state_size_, basic_state_size_);
    printf("init 0 is ok!\n");
    int r_param_size = controls_;

    for (int i = 0; i < r_param_size; ++i)
    {
        matrix_r_(i, i) = 1;
    }
    printf("init 1 is ok!\n");
    int q_param_size = basic_state_size_;
    for (int i = 0; i < q_param_size; ++i)
    {
        matrix_q_(i, i) = 1;
    }
    printf("init 2 is ok!\n");
    // Update matrix_q_updated_ and matrix_r_updated_
    matrix_r_updated_ = matrix_r_;
    matrix_q_updated_ = matrix_q_;
    printf("init mpc is ok!\n");
}
//更新状态矩阵A，并且进行离散化
void MY_MPC_Controller::UpdateMatrix(double heading_error, double linear_velocity)
{
    matrix_ad_(0, 2) = -linear_velocity * sin(heading_error) * ts_;
    matrix_ad_(1, 2) = linear_velocity * cos(heading_error) * ts_;

    matrix_bd_(0, 0) = cos(heading_error) * ts_;
    matrix_bd_(1, 0) = sin(heading_error) * ts_;
    matrix_bd_(2, 1) = ts_;
}
void MY_MPC_Controller::UpdateStateAnalyticalMatching(double x, double y, double heading_error, double heading_error_rate, double station_error, double speed_error)
{

    // State matrix update;
    matrix_state_(0, 0) = x;
    matrix_state_(1, 0) = y;
    matrix_state_(2, 0) = heading_error;
}

void MY_MPC_Controller::ComputeControlCommand(double Position_Error, double Angle_Error, double target_dis, double &speed, double &turnangle)
{
    static ReadSeting set("mpc.json");
    set.reload();
    double Lc = 40.0;
    double sp = 40;
    set.GetValue("linear_velocity", sp);
    set.GetValue("LC", Lc);
    //获取车辆基本参数，调试用
    set.GetValue("ts_", ts_);
    //***************获取结束

    //换算前叉误差
    double Ferr = (Position_Error / sin(Angle_Error) + Lc) * sin(Angle_Error);
    if (Ferr > 100)
        Ferr = 100;
    if (Ferr < -100)
        Ferr = -100;
    //输入状态
    double lateral_error = Ferr * 10;
    double lateral_error_rate = -target_dis * 10;
    double heading_error = Angle_Error;
    double heading_error_rate = (previous_heading_error_ - heading_error);
    double station_error = target_dis;
    double speed_error = 0;

    double linear_velocity = sp;

    previous_lateral_error_ = lateral_error;
    previous_heading_error_ = heading_error;
    // 更新初始状态矩阵
    UpdateStateAnalyticalMatching(lateral_error, lateral_error_rate, heading_error, heading_error_rate, station_error, speed_error);
    //更新矩阵ABC
    UpdateMatrix(heading_error_rate, linear_velocity);

    std::cout << "==>  " << matrix_state_.transpose() * 57.3 << std::endl;

    double q1 = 0, q2 = 0, q3 = 0, q4 = 0, q5 = 0, q6 = 0;
    double r1 = 1, r2 = 1;

    set.GetValue("q1", q1);
    set.GetValue("q2", q2);
    set.GetValue("q3", q3);
    set.GetValue("q4", q4);
    set.GetValue("q5", q5);
    set.GetValue("q6", q6);
    set.GetValue("r1", r1);
    set.GetValue("r2", r2);

    matrix_q_(0, 0) = q1;
    matrix_q_(1, 1) = q2;
    matrix_q_(2, 2) = q3;

    matrix_r_(0, 0) = r1;
    matrix_r_(1, 1) = r2;

    matrix_q_updated_ = matrix_q_;
    matrix_r_updated_ = matrix_r_;

    Eigen::MatrixXd control_matrix(controls_, 1);
    control_matrix << 0, 0;

    Eigen::MatrixXd reference_state(basic_state_size_, 1);
    reference_state << 0, 0, 0;

    std::vector<Eigen::MatrixXd> reference(horizon_, reference_state);

    Eigen::MatrixXd lower_bound(controls_, 1);
    lower_bound << -1.4, -0.1;

    Eigen::MatrixXd upper_bound(controls_, 1);
    upper_bound << 1.4, 0.1;

    std::vector<Eigen::MatrixXd> control(horizon_, control_matrix);

    for (unsigned int i = 1; i < control.size(); ++i)
    {
        control[i - 1] = control[i];
    }
    control[horizon_ - 1] = control_matrix;
    SolveLinearMPC(
        matrix_ad_, matrix_bd_, matrix_cd_, matrix_q_updated_,
        matrix_r_updated_, lower_bound, upper_bound, matrix_state_, reference,
        mpc_eps_, mpc_max_iteration_, &control);

    double steer_angle = control[0](0, 0);

    double steer_angle_limited = Clamp(control[0](0, 0), -1.4, 1.4);

    turnangle = -steer_angle_limited;
}
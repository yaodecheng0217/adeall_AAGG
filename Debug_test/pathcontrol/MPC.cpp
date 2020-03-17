/*
 * @Description: In User Settings Edit
 * @Author: Yaodecheng
 * @Date: 2019-10-14 14:59:04
 * @LastEditTime: 2020-03-17 17:53:18
 * @LastEditors: Yaodecheng
 */
#include "MPC.h"
#include "math.h"
#include <iostream>
#include"PID.h"
#define Radian_conversion 57.3
#define pi 3.1415926

//**************
/*#define size_t int
double abs(double x)
{
   if(x>0)
     return x;
     return -x;
}*/
//**************
//模型预测控制算法
// 输入：角度偏差
//       横向偏差
//       纵向偏差
//       前向距离
// 输出：角度值
double Fp = 0.00001;
double Fi = 0.0001;
double Fd = 0.0;
double Ap = -0.0001;
double Ai = 0.0001;
double Ad = 0.0;
double MPCctrloer(ErrorData err, set_info info,int dis)
{
  static double lunpian=0;
  static PID_IncTypeDef pid;
  static PID_IncTypeDef Apid;
  static double lastout=0;
  static double Angle_err = 0; 
	pid.Kp = Fp;
	pid.Ki = Fi;
	pid.Kd = Fd;
    Apid.Kp = Ap;
	Apid.Ki = Ai;
	Apid.Kd = Ad;
    double out_f = 0;
    double f=0;
    double out[120][2];
    for (size_t i = 0; i < 120; i++)
    {
        double x = i;
        out[i][0] = (x - 60) / Radian_conversion;
    }
    if (false)//abs(err.yaw) < 95 / Radian_conversion && abs(err.x) < info.dis) //95 250
    {
       //printf("MPC\n");
       // printf("err=%f   %f   %f   ",err.x,err.y,err.yaw*57.3);
        double x0 = err.y;
        double y0 = err.x;
        double yaw0 = err.yaw+Angle_err;

        double v = -55*dis;
        double dt = info.dt;
        double L = info.Car_L;

        double tarx = 0;
        double tary = 0;
        double taryaw = 0;

        for (size_t i = 0; i < 120; i++)
        {
            //求解未来10次轨迹偏差
            double error = 0;
            double bata = atan(((110) / L) * tan(out[i][0]));
            double x = x0;
            double y = y0;
            double yaw = yaw0;
            //cout << "A=" << out[i][0] * Radian_conversion << "  ";
            for (size_t j = 0; j < 10; j++)
            {
                //计算下一时刻拟输出
                x = x + v * cos(yaw) * dt;
                y = y + v * sin(yaw) * dt;
                yaw = yaw + v / L * tan(bata) * dt;

                if (yaw > 2 * pi)
                    yaw = yaw - 2 * pi;
                if (yaw < (0))
                    yaw = yaw + 2 * pi;

                //计算夹角
                double Angie_ErrorT = yaw - taryaw;
                if (Angie_ErrorT > pi)
                    Angie_ErrorT = Angie_ErrorT - 2 * pi;
                //cout << " " << Angie_Error * Radian_conversion <<"+";

                //计算夹角
                double thetaT = taryaw - atan2(tary - y, tarx - x);
                //计算横向偏差
                double RRT = sqrt(((x - tarx) * (x - tarx)) + ((y - tary) * (y - tary)));
                double Position_ErrorT = RRT * sin(thetaT);
                //加和

                error = error + Position_ErrorT * Position_ErrorT * info.set_Q + Angie_ErrorT * Angie_ErrorT;

            }
            out[i][1] = error;
        }
        double min = INFINITY;
        //取最小值
        for (size_t i = 0; i < 120; i++)
        {
            if (out[i][1] < min)
            {
                out_f = out[i][0];
                min = out[i][1];
              
            }
        }
        f = out_f;
        f = f * info.set_fx;
    }
    else
    {
        //转换横向偏差
        double k = info.set_fk; //速度增益系数
        double vxt = 5;
        double Qfat = -atan(err.x / (k * vxt));

        //输出变量
        double Qt = (Qfat - err.yaw * 1); //-Qfat - Angie_Error;

        if (Qt > pi / 2)
            Qt = pi / 2;

        if (Qt < -pi / 2)
            Qt = -pi / 2;
        f = Qt;
    }

  /* if(dis==1)
   {
      double pf = PID_Inc(0, err.x, &pid);
				lunpian -= pf;
                
   }
   if(dis==-1)
   {
     Angle_err += PID_Inc(0, err.yaw, &Apid);
    
   }*/
    
    //f = f+lunpian;
   



    if (f > pi / 2)
            f = pi / 2;

        if (f < -pi / 2)
            f = -pi / 2;
       
//printf("%f\n",f);

double maxout = 1 / 57.3;
	if (f - lastout > maxout)
	{
		lastout += maxout;
	}
	else if (f - lastout < -maxout)
	{
		lastout += -maxout;
	}
	else
	{
		lastout = f;
	}

    return lastout;
}

/*
    计算位置偏差数据
    输入：目标位置tar
          当前位置now
          行驶方向dis      1正向    -1反向
    返回：误差项

*/

ErrorData CalculationError(PostionData tar,PostionData now,int dis)
{
    ErrorData out;
    //角度误差
    double Angie_Error=tar.yaw-now.yaw;
    if (Angie_Error < -pi)
			Angie_Error = Angie_Error + 2 * pi;
		if (Angie_Error > pi)
			Angie_Error = Angie_Error - 2 * pi;
    out.yaw=Angie_Error;
    //计算横向偏差
		double theta = tar.yaw - atan2(tar.y - now.y, tar.x - now.x);
		double RR = sqrt(((now.x - tar.x) * (now.x - tar.x)) + ((now.y - tar.y) * (now.y - tar.y)));
		double Position_Error = RR * sin(theta);
    out.x=-Position_Error;

    
    out.y=target_determine2(tar,now,0,dis);

   // std::cout<<out.x<<"   "<<out.y<<"   "<<out.yaw*57.3<<std::endl;
        return out;
}

//到达判定
double target_determine2(PostionData s, PostionData c, int Length,int dis)
{
	static int Ercnt = 0;
	int RR = sqrt(((s.x - c.x) * (s.x - c.x)) + ((s.y - c.y) * (s.y - c.y)));
	//向量法求
	//先求目标角度
	double yawc = c.yaw + pi / 2;
	//计算夹角
	double theta = yawc - atan2(c.y - s.y, c.x - s.x);
	//计算投影
	double length = RR * sin(theta);
	//std::cout << "." ;
	if (dis==-1)
	{
		length = -length;
	}
	double Rerror = length - Length;
	//std::cout << "                                         L= " << Rerror << std::endl;

	return Rerror;
}
<!--
 * @Author: Yaodecheng
 * @Date: 2020-03-10 21:14:45
 * @LastEditors: Yaodecheng
 -->
# 状态机设计
## 开发任务
* 发送一条路径，车辆自动运行导航控制，控制期间向上级反馈路径跟踪状态


参考《Apollo 控制算法 LQR 分析》，定义如下系统：

  ![](https://www.zhihu.com/equation?tex=%5Cbegin%7Bequation%7D+x_%7Bt%2B1%7D%3DAx_t%2BBu_t%2BC%2Cx_0%3Dx%5E%7Binit%7D+%5Cend%7Bequation%7D)
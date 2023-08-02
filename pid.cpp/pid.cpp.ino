#include "pid.h"
 
PIDController::PIDController(float P, float I, float D, float ramp, float limit)
    : P(P)
    , I(I)
    , D(D)
    , output_ramp(ramp)    // output derivative limit [volts/second]
    , limit(limit)         // 输出限制(常用pwm分辨率)
    , error_prev(0.0)
    , output_prev(0.0)
    , output(0.0)
{
    timestamp_prev = _micros();
}
 
// PID controller function
float PIDController::operator() (float error){
    // 计算上次调用到现在的时间Ts(秒) 
    unsigned long timestamp_now = _micros();
    float Ts = (timestamp_now - timestamp_prev) * 1e-6;
    // 快速修复异常 (micros overflow)，如果溢出或者出现其他异常，则Ts=0.001ms
    if(Ts <= 0 || Ts > 0.5) Ts = 1e-3; 
 
    // 计算输出函数u(s)
    // u(k) = u(k-1) + Kp * (e(k)-e(k-1)) + Ki * Ts * (e(k)+e(k-1))/2 + Kd * (e(k)-2*e(k-1)+e(k-2))/Ts
    // 分别计算
    // 比例项: u_p  = P * (e(k)-e(k-1))
    float proportional = P * (error - error_prev);
    // 积分项: u_ik = I * Ts * (e(k)+e(k-1))/2
    float integral = I * Ts * (error + error_prev)*0.5;
    // 微分项: u_dk = D * (e(k)-2*e(k-1)+e(k-1))/Ts
    float derivative = D * (error - 2 * error_prev + error_prev1)/Ts;
 
    // 各部分加和
    output += proportional + integral + derivative;
    // antiwindup - limit the output variable
    output = _constrain(output, -limit, limit);
 
    // if output ramp defined   //这里解决超调问题。
    if(output_ramp > 0){
        // limit the acceleration by ramping the output
        float output_rate = (output - output_prev)/Ts;
        if (output_rate > output_ramp)
            output = output_prev + output_ramp*Ts;
        else if (output_rate < -output_ramp)
            output = output_prev - output_ramp*Ts;
    }
    
    // 存储数据以便下次调用
    output_prev = output;
    error_prev1 = error_prev;
    error_prev = error;
    timestamp_prev = timestamp_now;
    return output;
}

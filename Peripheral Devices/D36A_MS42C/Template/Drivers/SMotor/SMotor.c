#include "SMotor.h"

void SMotor_Init(SMotor *motor, GPIO_TypeDef *Dir_port, uint16_t Dir_pin,
                 TIM_HandleTypeDef *pwm_timer, uint32_t pwm_channel) {
    if (motor == NULL || pwm_timer == NULL) {
        // Handle error: motor or timer is NULL
        return;
    }
    motor->Dir_port = Dir_port;
    motor->Dir_pin = Dir_pin;
    motor->pwm_timer = pwm_timer;
    motor->pwm_channel = pwm_channel;

    // Initialize GPIO pins
    HAL_GPIO_WritePin(motor->Dir_port, motor->Dir_pin, GPIO_PIN_RESET);
     HAL_TIM_PWM_Start(motor->pwm_timer, motor->pwm_channel); // Start PWM
    __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, 0); // Set initial PWM duty cycle to 0
}

void SMotor_SetSpeed(SMotor *motor, float angular_speed) {
    if(angular_speed == 0) {
        __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, 0); // Stop PWM
        return; // No speed set, stop the motor
    } else if(angular_speed > 0) {
        HAL_GPIO_WritePin(motor->Dir_port, motor->Dir_pin, GPIO_PIN_RESET); // Set direction to clockwise
    } else {
        HAL_GPIO_WritePin(motor->Dir_port, motor->Dir_pin, GPIO_PIN_SET); // Set direction to counter-clockwise
        angular_speed = -angular_speed; // Ensure speed is positive for PWM
    }
    uint32_t tim_clk = GetClockFre(motor->pwm_timer);
    uint32_t target_frequency = GetStepFrequency(angular_speed);
    uint32_t target_prcs = (tim_clk / target_frequency / 1000) - 1; // Calculate prescaler value
    float target_arr = 1;
    if (target_prcs > 0xFFFF ) {
        target_prcs = 0xFFFF; // Limit prescaler to maximum value
    }else if(target_prcs < 1) {
        target_prcs = 1; // Ensure prescaler is at least 1
    }
    __HAL_TIM_SET_PRESCALER(motor->pwm_timer, target_prcs); // Set prescaler
    __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, motor->pwm_timer->Instance->ARR / 2); // Set duty cycle to 50%
}

uint32_t GetClockFre(TIM_HandleTypeDef *htim) {
    if (htim == NULL || htim->Instance == NULL) {
        // Handle error: timer handle or instance is NULL
        return 0;
    }
    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
    uint32_t tim_clk;

    uint32_t ppre1_bits = (RCC->CFGR >> 10) & 0x7;

    if (ppre1_bits == 0x0) {
        tim_clk = pclk1 * 2; // APB1 clock frequency
    } else {
        tim_clk = pclk1;
    }

    return tim_clk;
}

uint32_t GetStepFrequency(float angular_speed)
{
    if (angular_speed < 0) {
        angular_speed = -angular_speed; // Ensure speed is positive
    }
    uint32_t step_frequency = (uint32_t)(angular_speed / STEP_ANGULAR * STEP_DIVISOR);
    return step_frequency;
}
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

void SMotor_SetSpeed(SMotor *motor, uint8_t angular_speed) {
    if(angular_speed == 0) {
        __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, 0); // Stop PWM
    } else if(angular_speed > 0) {
        HAL_GPIO_WritePin(motor->Dir_port, motor->Dir_pin, GPIO_PIN_RESET); // Set direction to clockwise
        __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, motor->pwm_timer->Init.Period / 2); // Set PWM duty cycle to 50%
    } else {
        HAL_GPIO_WritePin(motor->Dir_port, motor->Dir_pin, GPIO_PIN_SET); // Set direction to counter-clockwise
        __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, motor->pwm_timer->Init.Period / 2); // Set PWM duty cycle to 50%
    }

}
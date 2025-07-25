#ifndef SMOTOR_H
#define SMOTOR_H

#include "main.h"

typedef enum {
    ANTI_CLOCKWISE = 0, // Counter-clockwise rotation
    CLOCKWISE = 1,      // Clockwise rotation
    SLEEP_SMOTOR = 2,   // Sleep mode
} SMOTOR_MODE;

typedef struct {
    GPIO_TypeDef *Dir_port; // GPIO port for direction pin
    uint16_t Dir_pin; // GPIO pin for direction pin
    TIM_HandleTypeDef *pwm_timer; // Timer handle for PWM
    uint32_t pwm_channel; // PWM channel
} SMotor;

void SMotor_Init(SMotor *motor,
                 GPIO_TypeDef *Dir_port, uint16_t Dir_pin,
                 TIM_HandleTypeDef *pwm_timer, uint32_t pwm_channel);

void SMotor_SetSpeed(SMotor *motor, uint8_t speed);
	
#endif
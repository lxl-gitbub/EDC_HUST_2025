#include "Bluetooth.h"
#include "AllHeader.h"

char Bluetooth_Receive_Buffer[BUF_LEN] __attribute__((section(".sram_no_cache")));
char Bluetooth_Send_Buffer[BUF_LEN] __attribute__((section(".sram_no_cache")));
void Bluetooth_Init(void)
{
    // Initialize the Bluetooth module here
    // This could include setting up UART, configuring the module, etc.
    HAL_UARTEx_ReceiveToIdle_DMA(UART_HANDLE, (uint8_t *)Bluetooth_Receive_Buffer, sizeof(Bluetooth_Receive_Buffer));
}
void Bluetooth_Send(const char *data, size_t length)
{
    // Send data over Bluetooth
    HAL_UART_Transmit(UART_HANDLE, (uint8_t *)data, length, HAL_MAX_DELAY);
}
void Bluetooth_Receive_Callback(UART_HandleTypeDef *huart, uint16_t Size)
{
    // This function is called when data is received over Bluetooth
    if (huart->Instance == UART_HANDLE->Instance)
    {
        // Process the received data
        // For example, you can parse commands or store them in a buffer
        // Here we just print the received data for demonstration
    }
}
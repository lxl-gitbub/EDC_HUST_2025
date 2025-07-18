# STM32H7 ����ģ�� UART DMA �����߽���ʹ��ָ��

���ĵ�ּ��Ϊ�� STM32H7 ΢�������ϣ�ʹ�� HAL ��ʵ������ģ��ͨ�� USART3 ���� DMA �����߽����ṩ��ϸ�����úʹ���ָ�ϣ����ر�ǿ������ MCU���� H7���л���һ���ԣ�Cache Coherency���Ĺؼ�ע�����

## 1. Ӳ������

* **USART3 ��������ģ��**��
    * STM32H7 �� USART3 TX ������������ģ��� RX ���š�
    * STM32H7 �� USART3 RX ������������ģ��� TX ���š�
    * ȷ����Դ�͵���������ȷ��

## 2. CubeMX ��������

�� STM32CubeMX �У���������Ŀ�����������ã�

### 2.1 USART3 ��������

* **Connectivity (������) -> USART3**��
    * **Mode (ģʽ)**��Asynchronous (�첽ģʽ)��
    * **Configuration (����) -> Parameter Settings (��������)**��
        * **Baud Rate (������)**������ `9600`��
        * **Word Length (�ֳ�)**��ͨ��Ϊ `8 Bits (+ Parity if any)`��
        * **Stop Bits (ֹͣλ)**��ͨ��Ϊ `1`��
        * **Data Direction (���ݷ���)**��**Receive and Transmit (���պͷ���)**��
        * **Fifo Mode (FIFO ģʽ)**���Ƽ� **Disable (����)**��`ReceiveToIdle` ��ʹ���� FIFO Ҳ������������������ã���Ҫ�������� RxFifo Threshold��

### 2.2 DMA ����

* **System Core (ϵͳ����) -> DMA -> Add (���)**��
    * Ϊ USART3 RX ���һ�� DMA ����ѡ�� `USART3_RX`��
    * **DMA Request Settings (DMA ��������)**��
        * **Mode (ģʽ)**��**Normal (��ͨģʽ)**��
        * **Increment Address (��ַ����)**��Peripheral Disabled (�������)��Memory Enabled (�ڴ�����)��
        * **Data Width (���ݿ��)**��Peripheral Byte (�����ֽ�)��Memory Byte (�ڴ��ֽ�)��
        * ����ѡ���Ĭ�ϻ������Ҫ���ã��� Burst Size��ͨ������Ҫ����
* **ȷ��û���������蹲�ô� DMA Stream**���Ա����ͻ��

### 2.3 NVIC �ж�����

* **System Core (ϵͳ����) -> NVIC (Ƕ�������жϿ�����)**��
    * ��ѡ��**���� (Enabled)**��
        * `USART3 global interrupt`��
        * `USART3_RX` �������� DMA Stream ��ȫ���жϣ����磺`DMA1 Stream X global interrupt` �� `BDMA Channel Y global interrupt`����

### 2.4 MPU (Memory Protection Unit) ���ã���ѡ���Ƽ���

��������������ݻ��棨D-Cache����ǿ�ҽ������� MPU���� **DMA ������**�Լ� **HAL ���������**���� `huart3` �� `hdma_usart3_rx`�����ڵ��ڴ���������Ϊ**���ɻ��� (Non-cacheable)**��������Ч���⻺��һ�������⡣

* **���� MPU ����ȡ���������ڴ�ӳ��**��ͨ����Ҫ�ֶ�������ڴ��������� `SRAM1` �� `SRAM2` ������ DMA �Ĳ��֣�����Ϊ `IsCacheable = MPU_ACCESS_NOT_CACHEABLE`��

## 3. ����ʵ��

### 3.1 �����������ڴ�����

�� `main.c` �����ͷ�ļ����������ջ������������� GNU C ��չ�� `__attribute__((section(".sram_no_cache")))` ��������ڲ��ɻ���� SRAM ����ͬ�����޸� `usart.c` �� `dma.c` �� HAL ������������

* **`main.c`**��
    ```c
    /* Private variables ---------------------------------------------------------*/
    /* USER CODE BEGIN PV */
    char receive[50] __attribute__((section(".sram_no_cache"))); // ���ջ������������ڲ��ɻ�������
    int isin = 0; // ���ڵ��ԣ�ָʾ�Ƿ����ص�
    /* USER CODE END PV */
    ```
* **`dma.c`**��
    �ҵ� `DMA_HandleTypeDef hdma_usart3_rx;`���޸�Ϊ��
    ```c
    DMA_HandleTypeDef hdma_usart3_rx __attribute__((section(".sram_no_cache"))); // DMA ����������ڲ��ɻ�������
    ```
    **ע��**��`.sram_no_cache` ��һ��ʾ��������ʵ��Ӧ���������������ű���`.ld` �ļ�����ȷ�������ڲ��ɻ��� RAM ����ȷ���������� `.RamD1NoCacheable` �� `.dtcm_ram`��������öβ����ڣ���������Ҫ���������ű����ֶ����塣

### 3.2 `main` �����еĳ�ʼ��

�� `main()` �����У�MCU ��ʼ����ɺ����� UART �� DMA �����߽��ա�

```c
/* USER CODE BEGIN 2 */
char message[] = "Hello, Bluetooth!\r\n"; // ���ڷ��͵Ĳ�����Ϣ

// IMPORTANT: ���� receive ������������Ϊ���ɻ��棬DMA ��ֱ�Ӷ�д���ڴ棬
// CPU ����ʱҲ��ֱ�Ӷ�д���ڴ棬������Ҫ SCB_InvalidateDCache_by_Addr �� SCB_CleanDCache_by_Addr
// ������ receive ������������һ���ԡ�
// ��ˣ�����ע�͵��������豣����
// SCB_InvalidateDCache_by_Addr((uint32_t *)receive, sizeof(receive));

HAL_StatusTypeDef rx
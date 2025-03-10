#ifndef INC_UART_HAL_H_
#define INC_UART_HAL_H_

#include "usart.h"
#include "ISR_obj.h"
#include "CircularBuffer.h"
#include "dma_hal.h"

#define UART_COUNT (1)
#define RX_BUFFER_SIZE (64)

class UartBase {
public:
    UartBase(USART_TypeDef *Instance, UART_HandleTypeDef *huart);

    HAL_StatusTypeDef write(uint8_t *pData, uint16_t Size, uint32_t Timeout = 1000);
    HAL_StatusTypeDef read(uint8_t *pData, uint16_t Size, uint32_t Timeout = 1000);

protected:
    USART_TypeDef *_Instance;
    UART_HandleTypeDef *_huart;
};

// UART class with interrupt
class UartIT : public UartBase {
public:
    UartIT(USART_TypeDef *Instance, UART_HandleTypeDef *huart);

    virtual HAL_StatusTypeDef write(uint8_t *pData, uint16_t Size);
    virtual HAL_StatusTypeDef start_read(void);
    uint16_t read(uint8_t *pData, uint16_t Size);
    bool is_tx_complete(void);

protected:
    bool _is_tx_complete;
    uint8_t _rx_buffer[RX_BUFFER_SIZE];
    CircularBuffer<uint8_t> *_buffer;
    static ISR<UartIT> ISR_LIST;
    
private:
    static void TxCpltCallback(UART_HandleTypeDef *huart);
    static void RxEventCallback(UART_HandleTypeDef *huart, uint16_t Pos);

    virtual void put(uint16_t index, uint16_t size);

};

class UartDMA: public UartIT {
public:
    UartDMA(USART_TypeDef *Instance, UART_HandleTypeDef *huart);

    virtual HAL_StatusTypeDef write(uint8_t *pData, uint16_t Size) override;
    virtual HAL_StatusTypeDef start_read(void) override;

protected:
    virtual void put(uint16_t index, uint16_t size) override;

};

#endif /* INC_UART_HAL_H_ */


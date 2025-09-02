#ifndef INC_CAN_LIB_H_
#define INC_CAN_LIB_H_

#include "can.h"  // STM32 HAL CAN header

class CanBase {
public:
    // Constructor: takes the CAN instance and its HAL handle.
    CanBase(CAN_TypeDef *Instance, CAN_HandleTypeDef *hcan);

    // Write a CAN message.
    // StdId: the standard identifier for the message.
    // pData: pointer to the data payload.
    // Size: data length code (number of bytes, 0–8).
    // Timeout: timeout in ms (for interface consistency)
    HAL_StatusTypeDef write(uint32_t StdId, uint8_t *pData, uint8_t Size, uint32_t Timeout = 1000);

    // Read a CAN message.
    // pData: pointer to a buffer where the received data will be stored.
    // Size: pointer to a variable that will receive the data length (DLC).
    // Timeout: timeout in ms (for interface consistency)
    HAL_StatusTypeDef read(uint8_t *pData, uint8_t *Size, uint32_t Timeout = 1000);

    // Configure the CAN filter to accept messages with specific IDs.
    // filterId: the ID you want to match.
    // filterMask: bits to compare (0x7FF means compare all bits for a standard ID).
    HAL_StatusTypeDef configureFilter(uint32_t filterId, uint32_t filterMask);

protected:
    CAN_TypeDef *_Instance;         // Pointer to the CAN peripheral instance (e.g., CAN1)
    CAN_HandleTypeDef *_hcan;         // Pointer to the HAL CAN handle
};

#endif /* INC_CAN_LIB_H_ */

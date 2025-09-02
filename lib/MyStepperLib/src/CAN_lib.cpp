#include "CAN_lib.h"

CanBase::CanBase(CAN_TypeDef *Instance, CAN_HandleTypeDef *hcan): 
_Instance(Instance), _hcan(hcan)
{
}

// Write a CAN message using HAL_CAN_AddTxMessage.
// The Timeout parameter is kept for interface consistency.
HAL_StatusTypeDef CanBase::write(uint32_t StdId, uint8_t *pData, uint8_t Size, uint32_t Timeout)
{
    CAN_TxHeaderTypeDef TxHeader;
    uint32_t TxMailbox;

    // Configure the transmission header.
    TxHeader.StdId = StdId;
    TxHeader.IDE = CAN_ID_STD;    // Standard frame (11-bit ID)
    TxHeader.RTR = CAN_RTR_DATA;  // Data frame (not a remote transmission request)
    TxHeader.DLC = Size;          // Data length code

    // Transmit the message using the HAL function.
    return HAL_CAN_AddTxMessage(_hcan, &TxHeader, pData, &TxMailbox);
}

// Read a CAN message using HAL_CAN_GetRxMessage.
// This example uses FIFO0.
HAL_StatusTypeDef CanBase::read(uint8_t *pData, uint8_t *Size, uint32_t Timeout)
{
    CAN_RxHeaderTypeDef RxHeader;
    
    // Try to read a message from CAN_RX_FIFO0.
    HAL_StatusTypeDef status = HAL_CAN_GetRxMessage(_hcan, CAN_RX_FIFO0, &RxHeader, pData);
    
    // If a message was received, store the received data length.
    if (status == HAL_OK)
    {
        *Size = RxHeader.DLC;
    }
    
    return status;
}

// Configure the CAN hardware filter to accept specific message IDs.
// The filterMask defines which bits are significant when comparing IDs.
// For example, to accept only messages with ID 0x6A5:
//   filterId = 0x6A5, filterMask = 0x7FF (all bits must match)
HAL_StatusTypeDef CanBase::configureFilter(uint32_t filterId, uint32_t filterMask)
{
    CAN_FilterTypeDef filterConfig;

    filterConfig.FilterBank = 0; // Use filter bank 0 (may vary per application)
    filterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    filterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    // The filter configuration expects the ID shifted left by 5 bits.
    filterConfig.FilterIdHigh = (filterId << 5) & 0xFFFF;
    filterConfig.FilterIdLow = 0;
    filterConfig.FilterMaskIdHigh = (filterMask << 5) & 0xFFFF;
    filterConfig.FilterMaskIdLow = 0;
    filterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; // Using FIFO0 for reception
    filterConfig.FilterActivation = ENABLE;

    return HAL_CAN_ConfigFilter(_hcan, &filterConfig);
}

#include "Encoder_lib.h"

ISR<EncoderIT> EncoderIT::ISR_List;

void EncoderIT::PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    for (uint16_t i = 0; i < ISR_List.size(); i++)
    {
        if (ISR_List.get(i)->_htim == htim)
        {
            ISR_List.get(i)->handleOverflow();
        }
    }
}

// Encoder base implementation
EncoderBase::EncoderBase(TIM_HandleTypeDef *htim, uint32_t Channel): 
_htim(htim), _Channel(Channel)
{   
    // maybe initialize here
}

EncoderBase::~EncoderBase(void)
{

}

HAL_StatusTypeDef EncoderBase::start(void) 
{
    return HAL_TIM_Encoder_Start(_htim, _Channel);
}

HAL_StatusTypeDef EncoderBase::stop(void) 
{
    return HAL_TIM_Encoder_Stop(_htim, _Channel);
}

void EncoderBase::reset(void) 
{
    _htim->Instance->CNT = 0;
}

uint32_t EncoderBase::read(void) 
{
    return __HAL_TIM_GET_COUNTER(_htim);
}

// Encoder interrupt implementation
EncoderIT::EncoderIT(TIM_HandleTypeDef *htim, bool invertDirection):
EncoderBase(htim, _Channel), _invertDirection(invertDirection)
{
#if USE_HAL_TIM_REGISTER_CALLBACKS != 1
#error "USE_HAL_TIM_REGISTER_CALLBACKS must be enabled in stm32f1xx_hal_conf.h"
#endif

    htim->PeriodElapsedCallback = PeriodElapsedCallback;
    ISR_List.add(this);
}

EncoderIT::~EncoderIT(void)
{
    ISR_List.remove(this);
}

HAL_StatusTypeDef EncoderIT::start(void)
{
    HAL_StatusTypeDef status1 = HAL_TIM_Encoder_Start(_htim, _Channel);

    HAL_StatusTypeDef status2 = HAL_TIM_Base_Start_IT(_htim);

    if (status1 == HAL_OK && status2 == HAL_OK) {
        return HAL_OK;
    } else {
        return HAL_ERROR;
    }
}

HAL_StatusTypeDef EncoderIT::stop(void) 
{
    return HAL_TIM_Encoder_Stop_IT(_htim, _Channel);
}

int32_t EncoderIT::read(void) 
{
    // Read the raw 16-bit hardware counter
    rawCount = __HAL_TIM_GET_COUNTER(_htim);

    // Combine with the overflow (multiply with 2^16)
    int32_t fullCount = (_overflow << 16) + rawCount;
    if (_overflow < 0) {
        fullCount = ((_overflow) << 16) | rawCount;
    } // Adjusts for negative overflows
    
    return _invertDirection ? -fullCount : fullCount;
}

void EncoderIT::handleOverflow(void)
{
    int32_t currentCounter = __HAL_TIM_GET_COUNTER(_htim);
    if ((currentCounter - rawCount) > 32767)
    {
        _overflow--;
    } 
    else if (currentCounter - rawCount < -32768)
    {
        _overflow++;
    }
    rawCount = currentCounter; // to prevent multiple overflow counts
}
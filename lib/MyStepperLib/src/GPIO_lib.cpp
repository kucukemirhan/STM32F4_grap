#include "GPIO_lib.h"

bool GPIObase::is_init = false;

GPIObase::GPIObase(GPIO_TypeDef *GPIOx, uint16_t GPIO_pin): 
_GPIOx(GPIOx), _GPIO_pin(GPIO_pin)
{
    GPIObase::init();
}

GPIObase::~GPIObase(void) 
{

}

void GPIObase::init(void) 
{
    if (GPIObase::is_init)
    {
        return;
    }

    MX_GPIO_Init();
    GPIObase::is_init = true;
}

GPIO_PinState GPIObase::read() 
{
    return HAL_GPIO_ReadPin(_GPIOx, _GPIO_pin);
}

// Digital Output
DigitalOut::DigitalOut(GPIO_TypeDef *GPIOx, uint16_t GPIO_pin, GPIO_PinState PinState): 
GPIObase(GPIOx, GPIO_pin) 
{
    write(PinState);
}

DigitalOut::~DigitalOut(void) 
{
    HAL_GPIO_DeInit(_GPIOx, _GPIO_pin);
}

void DigitalOut::write(GPIO_PinState PinState) 
{
    HAL_GPIO_WritePin(_GPIOx, _GPIO_pin, PinState);
}

void DigitalOut::toggle(void) 
{
    HAL_GPIO_TogglePin(_GPIOx, _GPIO_pin);
}

// Digital Input
DigitalIn::DigitalIn(GPIO_TypeDef *GPIOx, uint16_t GPIO_pin): 
GPIObase(GPIOx, GPIO_pin)
{

}

DigitalIn::~DigitalIn(void) 
{
    HAL_GPIO_DeInit(_GPIOx, _GPIO_pin); // boş da durabilir çok önemli değil zaten deinit yapmıyoruz
}
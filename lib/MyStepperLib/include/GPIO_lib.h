#ifndef INC_GPIO_LIB_H_
#define INC_GPIO_LIB_H_

#include "gpio.h"

class GPIObase {
public:
    GPIObase(GPIO_TypeDef *GPIOx, uint16_t GPIO_pin);
    ~GPIObase();

    static void init(void);

    GPIO_PinState read(void);

protected:
    GPIO_TypeDef *_GPIOx;
    uint16_t _GPIO_pin;

private:
    static bool is_init;
};

class DigitalOut : public GPIObase {
public:
    DigitalOut(GPIO_TypeDef *GPIOx,
                uint16_t GPIO_pin, 
                GPIO_PinState PinState = GPIO_PIN_SET);

    ~DigitalOut(void);

    void write(GPIO_PinState PinState);
    void toggle(void);
};

class DigitalIn : public GPIObase {
public:
    DigitalIn(GPIO_TypeDef *GPIOx,
                uint16_t GPIO_pin);

    ~DigitalIn(void);
};

#endif /* INC_GPIO_LIB_H_ */
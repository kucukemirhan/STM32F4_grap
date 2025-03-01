#ifndef INC_TIM_LIB_H_
#define INC_TIM_LIB_H_

#include "tim.h"
#include "ISR_obj.h"

#define TIM_COUNT (4)

class TimBase {
public:
    TimBase(TIM_TypeDef *Instance, TIM_HandleTypeDef *htim);
    ~TimBase(void);

    virtual HAL_StatusTypeDef start(void);
    virtual HAL_StatusTypeDef stop(void);
    virtual void reset(void);
    uint32_t read(void);
    uint32_t getTIMclock(void);

protected:
    TIM_TypeDef *_Instance;
    TIM_HandleTypeDef *_htim;

private:
    static bool is_init[TIM_COUNT];
    void init(void);
};

class TimIT : public TimBase {
public:
    TimIT(TIM_TypeDef *Instance, TIM_HandleTypeDef *htim);
    ~TimIT(void);

    HAL_StatusTypeDef start(void) override;
    HAL_StatusTypeDef stop(void) override;
    void reset(void) override;
    uint64_t read(void);

    bool delay_ms(uint64_t start_time, uint64_t delay_time);

protected:
    static class ISR<TimIT> ISR_List;

private:
    uint16_t _tick;
    
    static void PeriodElapsedCallback(TIM_HandleTypeDef *htim);
    void tick(void);
};

class TimPWM : public TimBase {
public:
    TimPWM(TIM_TypeDef *Instance, TIM_HandleTypeDef *htim, uint32_t Channel = TIM_CHANNEL_1);
    ~TimPWM(void);

    HAL_StatusTypeDef start(void) override;
    HAL_StatusTypeDef stop(void) override;
    inline void setThisARR(uint16_t arr);
    inline void setNextARR(uint16_t arr);
    inline void setPSC(uint16_t psc);

    void setFrequency(uint16_t frequency);

    inline uint32_t getCNT(void);
    void reset(void) override;
    // void setPSC(uint16_t prescaler);
    
protected:
    static class ISR<TimPWM> ISR_List;
    uint32_t _Channel;

private:
    static void PeriodElapsedCallback(TIM_HandleTypeDef *htim);
    
};

#endif /* INC_TIM_LIB_H_ */
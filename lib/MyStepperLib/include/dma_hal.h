#ifndef INC_DMA_HAL_H_
#define INC_DMA_HAL_H_

#include "dma.h"

class DMA {
public:
    DMA();
    static void init(void);

private:
    inline static bool _is_init = false;
};

inline DMA::DMA() {
    // Constructor implementation
}

inline void DMA::init(void) {
    if (_is_init) {
        return;
    }
    _is_init = true;
    MX_DMA_Init();
}

#endif /* INC_DMA_HAL_H_ */

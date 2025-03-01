#ifndef INC_ISR_OBJ_H_
#define INC_ISR_OBJ_H_

#include "main.h"
#include <vector> // so we can create dynamic list
#include <algorithm> // for find function

template<class T>
class ISR {
public:
    ISR(void) {
        ISR_LIST.clear();
    }

    ~ISR() {
        if(ISR_LIST.size() == 0)
        {
            return;
        }
        ISR_LIST.clear();
        // ISR_LIST.erase(std::find(ISR_LIST.begin(), ISR_LIST.end(), this));
    }

    void add(T *obj) {
        ISR_LIST.push_back(obj);
    }

    void remove(T *obj) {
        if(ISR_LIST.size() == 0)
        {
            return;
        }

        ISR_LIST.erase(std::find(ISR_LIST.begin(), ISR_LIST.end(), obj));
    }

    T* get(uint16_t index) {
        if(ISR_LIST.size() == 0)
        {
            return NULL;
        }
        return ISR_LIST[index];
    }

    size_t size(void) {
        return ISR_LIST.size();
    }

private:
    std::vector<T*> ISR_LIST;
};

#endif /* INC_ISR_OBJ_H_ */

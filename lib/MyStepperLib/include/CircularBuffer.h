#ifndef INC_CIRCULARBUFFER_H_
#define INC_CIRCULARBUFFER_H_

#include "main.h"
#include <string.h>

template<class T>
class CircularBuffer {
public:
    CircularBuffer(uint16_t size): _size(size), _head(0), _tail(0), _stored(0) 
    {
        _buffer = new T[_size];
        memset(_buffer, 0, _size*sizeof(T));
    }

    ~CircularBuffer(void) 
    {
        delete[] _buffer;
    }

    void put(T *items, uint16_t size) 
    {
        for(uint16_t i = 0; i < size; i++) 
        {
            _buffer[_head++] = items[i];
            _stored++;
            if(_head >= _size)
            {
                _head = 0;
            }

            if(_stored >= _size)
            {
                _stored = _size;
            }
        }
    }

    uint16_t pull(T *items, uint16_t size) 
    {
        uint16_t read_size = 0;
        for(uint16_t i = 0; i < size; i++) 
        {
            if(_stored == 0)
            {
                break;
            }

            items[i] = _buffer[_tail++];
            _stored--;
            if(_tail >= _size)
            {
                _tail = 0;
            }
            read_size++;
        }

        return read_size;
    }

    uint16_t size(void) 
    {
        return _stored;
    }

    void clear(void) 
    {
        _head = 0;
        _tail = 0;
        _stored = 0;
        memset(_buffer, 0, _size*sizeof(T));
    }

private:
    T *_buffer;
    uint16_t _size;
    uint16_t _head;
    uint16_t _tail;
    uint16_t _stored;
};



#endif /* INC_CIRCULARBUFFER_H_ */
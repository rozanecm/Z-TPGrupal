#ifndef Z_TPGRUPAL_COUNTER_H
#define Z_TPGRUPAL_COUNTER_H


class Counter {
    unsigned short counter;
    unsigned short maxSize;
public:
    void initialize(unsigned long maxSize);

    void updateCounter();

    unsigned short getCounter();
};


#endif //Z_TPGRUPAL_COUNTER_H

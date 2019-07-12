/**
 \brief
 Must be called before Overflow !
 */
unsigned long last;
uint64_t offset;


inline uint64_t Micros() {
    if (micros() < last) offset += 0x100000000;
    last = micros();
    return (uint64_t) last + offset;
}


static inline uint32_t asm_ccount(void) {
    uint32_t r;
    asm volatile("rsr %0, ccount": "=r"(r));
    return r;
}



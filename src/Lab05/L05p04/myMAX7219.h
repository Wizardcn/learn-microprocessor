
#define REG_DIGIT(x)      (0x1+(x))
#define REG_DECODE_MODE   (0x9)
#define REG_INTENSITY     (0xA)
#define REG_SCAN_LIMIT    (0xB)
#define REG_SHUTDOWN      (0xC)
#define REG_DISPLAY_TEST  (0xF)

extern const int CS_PIN;
void MAX7219_write_reg(uint8_t addr, uint8_t data);
void MAX7219_init(void);
void MAX7219_test(void);

#ifndef IR_REMOTE_H
#define IR_REMOTE_H

typedef enum {
    IR_CMD_0 = 0x82,
    IR_CMD_1 = 0xA8,
    IR_CMD_2 = 0x68,
    IR_CMD_3 = 0xE8,
    IR_CMD_4 = 0x98,
    IR_CMD_5 = 0x58,
    IR_CMD_6 = 0xD8,
    IR_CMD_7 = 0xB8,
    IR_CMD_8 = 0x78,
    IR_CMD_9 = 0xF8,
    IR_CMD_LEFT = 0x92,
    IR_CMD_RIGHT = 0x52,
    IR_CMD_UP = 0xD0,
    IR_CMD_DOWN = 0xF0,
    IR_CMD_OK = 0xB0,
    IR_CMD_POWER = 0x80,
    IR_CMD_NONE = 0xFF,
} ir_cmd_e;

void ir_remote_init(void);
ir_cmd_e ir_remote_get_cmd(void);

const char *ir_remote_cmd_to_string(ir_cmd_e);

#endif // IR_REMOTE_H
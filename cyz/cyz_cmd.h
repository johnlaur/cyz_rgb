#ifndef CYZ_CMD_H
#define CYZ_CMD_H
/**
 * Command parser and executor
 *
 */
#include "cyz_rgb.h"
#include "../ring_buffer.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2

#define CYZ_CMD_MAX_CMD_LEN 8
#define CYZ_CMD_BOOTP_MAGIC 33
#define CMD_GO_TO_RGB 'n'
#define CMD_FADE_TO_RGB 'c'
#define CMD_FADE_TO_RND_RGB 'C'
#define CMD_FADE_TO_HSB 'h'
#define CMD_FADE_TO_RND_HSB 'H'
#define CMD_WRITE_SCRIPT_LINE 'W'
#define CMD_PLAY_LIGHT_SCRIPT 'p'
#define CMD_STOP_SCRIPT 'o'
#define CMD_SET_BOOT_PARMS 'B'
#define CMD_SET_FADESPEED 'f'
#define CMD_SET_TIMEADJUST 't'
#define CMD_SET_LEN_RPTS 'L'
#define CMD_SET_ADDR 'A'
#define CMD_GET_ADDR 'a'
#define CMD_GET_RGB 'g'
#define CMD_GET_SCRIPT_LINE 'R'
#define CMD_GET_FIRMWARE_VERSION 'Z'

#define CMD_GET_DBG 'D'

#define MAX_SCRIPT_LEN 10


typedef struct _boot_parms {
	uint8_t magic; // check byte to see if any data has been stored
	uint8_t mode; // 0 do nothing, 1 play light script
	uint8_t scriptno; // which script to play if mode==1
	uint8_t repeats; // number of repetitions
	uint8_t fadespeed;
	int8_t timeadjust;
	uint8_t addr;
} boot_parms;

typedef struct _script_line {
    uint8_t dur;
    uint8_t cmd[4];    // cmd,arg1,arg2,arg3
} script_line;

typedef struct _script {
    uint8_t len;  // number of script lines, 0 == blank script, not playing
    uint8_t reps; // number of times to repeat, 0 == infinite playes
    script_line lines[];
} script;

typedef struct CYZ_CMD {
	script_line script[MAX_SCRIPT_LEN];
	uint8_t rcv_cmd_buf[8];
	uint8_t rcv_cmd_buf_cnt;
	uint8_t rcv_cmd_len;
	uint8_t play_script;
	uint8_t script_length;
	uint8_t script_pos;
	uint8_t script_repeats;
	uint8_t script_repeated;
	uint8_t timeadjust;
	uint8_t addr;
	unsigned long tick_count;
	ring_buffer send_buffer;
	uint8_t dbg;
} Cyz_cmd;

Cyz_cmd cyz_cmd;

void CYZ_CMD_init();
void _CYZ_CMD_execute(uint8_t* cmd);
void _CYZ_CMD_receive_one_byte(uint8_t in);
long _CYZ_CMD_play_next_script_line();
void CYZ_CMD_load_boot_params();
void _CYZ_CMD_tick();
uint8_t _CYZ_CMD_prng(uint8_t range);
uint8_t _CYZ_CMD_get_one_byte_from_send_buffer();
uint8_t _CYZ_CMD_is_data_in_send_buffer();

#endif

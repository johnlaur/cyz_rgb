#ifndef CYZ_CMD_H
#define CYZ_CMD_H
/**
 * Command parser and executor
 *
 */

#include "cyz_rgb.h"

#define CYZ_CMD_MAX_CMD_LEN 8;

#define CMD_GO_TO_RGB 'n'
#define CMD_FADE_TO_RGB 'c'
#define CMD_WRITE_SCRIPT_LINE 'W'
#define CMD_PLAY_LIGHT_SCRIPT 'p'

typedef struct _script_line {
    unsigned char dur;
    unsigned char cmd[4];    // cmd,arg1,arg2,arg3
} script_line;

typedef struct CYZ_CMD {
	unsigned char rcv_cmd_buf[8];
	unsigned char rcv_cmd_buf_cnt;
	unsigned char rcv_cmd_len;
	Cyz_rgb* cyz_rgb;
	script_line* script[49];
	unsigned char play_script;
	unsigned char script_end;
	unsigned char script_pos;
	unsigned char script_repeats;
	unsigned char script_repeated;
	void (*execute)(void* this, unsigned char* buf);
	void (*receive_one_byte)(void* this, unsigned char rcv);
	void (*play_next_script_line)(void *this);
} Cyz_cmd;

Cyz_cmd* CYZ_CMD_GET_INSTANCE(Cyz_rgb* cyz_rgb);
void _CYZ_CMD_execute(Cyz_cmd *this, unsigned char* cmd);
void _CYZ_CMD_receive_one_byte(Cyz_cmd *this, unsigned char in);
void _CYZ_CMD_play_next_script_line(Cyz_cmd *this);

#endif

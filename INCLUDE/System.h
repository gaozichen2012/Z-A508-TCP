#ifndef	__SYSTEM_H
#define __SYSTEM_H

#ifdef	SYSLABLE
	#define SYSAPI
#else
	#define SYSAPI	extern
#endif

typedef enum {
	POW_IDIDLE		= 0x00,
	POW_IDTOT 		= 0x01,
	POW_IDLOBATT	= 0x02,
	POW_IDCHNULL	= 0x03,
	POW_IDSAVE		= 0x04,
	POW_IDLOBATTSTOPTX	= 0x05,
	POW_IDRESET		= 0xFF
}POW_MANTYPE;

typedef enum {					//run status select string code
	MSG_NULL		= 0x00,
	MSG_IDLE  		= 0x00,
	MSG_RUN			= 0x01,
	MSG_EXIT		= 0x02,
	MSG_WAIT		= 0x03,
	MSG_RENEW		= 0x04
}MSG_STATUS;

typedef enum {					//switch select string code
	SW_NULL			= 0x00,
	SW_EXIT 		= 0xFF
}SW_CODE;





SYSAPI	void main_app(void);
SYSAPI TASK_CODE GetTaskId(void);
SYSAPI void SetTaskId(TASK_CODE a);
extern void main_init(void);
#endif
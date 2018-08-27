#ifndef VOICE_H
#define VOICE_H

#define VOICE_SEL

#ifdef	VOICELABEL
	#define VOICEAPI
#else
	#define VOICEAPI	extern
#endif

typedef enum
{	
	VOICE_IDKEY		= 0x00,
	VOICE_IDPOWON	= 0x01,
	VOICE_IDCHCHG	= 0x02,
	VOICE_IDLOWBAT	= 0x04,
	VOICE_IDALARM	= 0x05,
	VOICE_IDSCAN	= 0x06,
	VOICE_IDRESET	= 0xFF
}VOICE_TYPE;
typedef enum{
	ATVOICE_FreePlay		= 0x0001,
	ATVOICE_CardError		= 0x0002,
	ATVOICE_SingalError	        = 0x0004,
	ATVOICE_CheckNet		= 0x0008,
	ATVOICE_Landing			= 0x0010,
	ATVOICE_LandSuccess	        = 0x0020,
	ATVOICE_LowAlarm		= 0x0040,
	ATVOICE_LowPowOff		= 0x0080,
	ATVOICE_Battery			= 0x0100,
	ATVOICE_Unline			= 0x0200
}AtVoiceType;

typedef enum
{
	VOICE_IDOFF	= 0x00,
	VOICE_IDCH	= 0x01,
	VOICE_IDPOW	= 0x02,
	VOICE_IDBATT= 0x03
}TPCONT_SEL;

VOICEAPI void VOICE_PowerOnInitial(void);
VOICEAPI void VOICE_SetOutput(AtVoiceType Id, u8 *buf, u8 Len);
VOICEAPI void VOICE_Renew(void);
VOICEAPI void VOICE_1sProcess(void);

#endif
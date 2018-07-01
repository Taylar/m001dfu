#ifndef			MOVT_H
#define			MOVT_H

// movt direction set
#define			MOVT_DIR_AUTO							0
#define			MOVT_DIR_SETTING						1

#define			MOVT_DIR_FORWARD						0
#define			MOVT_DIR_REVERSE						1

typedef void func(void);


// movt num
#ifndef		MOVT_NUM_TYPEDEF
#define		MOVT_NUM_TYPEDEF
typedef enum
{
	MOVT_M_CLOCK	= 0,
	MOVT_MAX_NUM
} movt_num_t;

#endif
/*end defineif */

#ifndef			MOVT_ERROR
#define			MOVT_ERROR

#define			MOVT_M_ERROR							0X01

#endif



// variable

typedef struct
{
	uint16_t	currentPosition;
	uint16_t	aimPosition;
	uint16_t	movtStep;
	uint16_t	movtForwardMax;
	uint16_t	movtReverseMax;
	uint16_t	movtRangeMax;
	uint16_t	tinyTimeCnt;
	uint8_t		direction;
	uint8_t		tempDirection;
	uint8_t		directionSet;
	uint8_t		state;
	uint8_t		repeat;
	uint8_t		stop;
	uint16_t		*clockForward;
	uint16_t		*clockReverse;
	void		(*timerStart)(uint32_t cnt);
	void		(*timerStop)(void);
} movt_att_s;


extern movt_att_s	movtConfig[MOVT_MAX_NUM];


typedef struct 
{
	movt_att_s*	clockM;
	void	(*Init)(void);
	void	(*ClockMFrowardFinishCbInit)(func *cb);
	void	(*ClockMResverseFinishCbInit)(func *cb);
	void	(*Compare)(movt_att_s *movtCompare);
	uint16_t (*ReadRunDir)(movt_att_s *movtCompare);
	uint16_t (*SelfTest)(void);
}bsp_movt_s;

extern const bsp_movt_s		bsp_movt;


// function declaration


void BspMovtInit(void);
void ClockMFrowardFinishCbInit(func *cb);
void ClockMResverseFinishCbInit(func *cb);
void MovtclockCompare(movt_att_s *movtCompare);
uint16_t BspMovtRunDiretion(movt_att_s *movtCompare);
uint16_t BspMovtSelfTest(void);


#endif		//MOVT_H

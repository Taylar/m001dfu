#ifndef   __LIS3DH_H_
#define   __LIS3DH_H_





typedef struct 
{
	int	(*Process)(unsigned char xyzData[3]);
}calculate_step_s;

extern const calculate_step_s stepOperate;

int CalculateStep(unsigned char xyzData[3]);

#endif

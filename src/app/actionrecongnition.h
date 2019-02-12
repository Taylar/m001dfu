#ifndef   ACTIONRECONGNITION_H
#define   ACTIONRECONGNITION_H

int actionRecongnition(unsigned char xyzData[3]);
void setActionThreshold(unsigned char up1, unsigned char up2, unsigned char down1, unsigned char down2);
uint8_t ActionDetect(int8_t axis[3]);
uint16_t GetActionState(void);
void SetActionState(uint16_t powerState);

#endif	//ACTIONRECONGNITION_H

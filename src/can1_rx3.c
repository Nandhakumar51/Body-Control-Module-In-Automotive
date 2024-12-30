#include<lpc21xx.h>
typedef unsigned int u32;
#define led0 1<<17
#define motor_1 1<<17
#define motor_2 1<<18
struct can
{
	u32 id;
	u32 rtr;
	u32 dlc;
	u32 byteA;
	u32 byteB;
	};
void delay_ms(int s){
 T0PR=60000-1;
 T0TCR=0X01;
 while(T0TC<s);
 T0TCR=0X03;
 T0TCR=0;
}

void can_init(){
 PINSEL1|=0X00014000;//P0.23 P0.24
 VPBDIV=1;
 C2MOD=0x1;
 AFMR=0X02;
 C2BTR=0X001C001D;
 C2MOD=0x0;
}
void config(unsigned int baud){

	PINSEL0|=0x5;

	U0LCR=0x83;

	switch(baud){	

		case 115200:U0DLL=32;	break;//PCLK=60MHz

		case 921600:U0DLL=4;	break;

		default:	U0DLL=32	;	

	}

	U0LCR=0x03;

}
void tx(unsigned char ch)
{
 	U0THR=ch;
	while(((U0LSR>>5)&1)==0);
}
void string( char *s){
	while(*s)
		tx(*s++);
}
void can_tx(struct can m1){
 	C2TID2=m1.id;
	C2TFI2=m1.dlc<<16;
	if(m1.rtr==0){
	 	C2TFI2&=~(1<<30);//RTR=0
		C2TDA2=m1.byteA;
		C2TDB2=m1.byteB;
	}
	else{
		C2TFI2|=(1<<30);//RTR=1
	}
	C2CMR=(1<<0)|(1<<6);
	while(C2GSR&(1<<3)==0);
}
void can_rx(struct can *m2){
 	while(C2GSR&(0X01)==0);
	m2->id=C2RID;
	m2->dlc=C2RFS>>16&0XF;
	m2->rtr=C2RFS>>30&1;
	if(m2->rtr==0){
		m2->byteA=C2RDA;
		m2->byteB=C2RDB;
		}
		C2CMR=1<<2;
}
void motor_init(){
 	IODIR0|=motor_1|motor_2;
	IOSET0=motor_1|motor_2;	
}
void motor_forward(){
 	IOSET0=motor_1;
	IOCLR0=motor_2;
	delay_ms(500);
}
void motor_backward(){
 	IOSET0=motor_2;
	//delay_ms(500);
	IOCLR0=motor_1;
	delay_ms(500);
}
void motor_stop(){
 	IOCLR0= motor_1|motor_2;
}

int main(){
 struct can m2;
 IODIR0|=led0;
 IOSET0=led0;
 can_init();
 motor_init();

 while (1) {
        can_rx(&m2);
		if(m2.id==0x03){
			IOCLR0=led0;
			string("message receivied\r\n");
		 	motor_forward();
			//delay_ms(200);
			motor_backward();
			//delay_ms(200);
			motor_stop();
		    IOSET0=led0;
		} 
         
    }	
}


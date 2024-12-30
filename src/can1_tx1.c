#include<lpc21xx.h>
#define sw1 1<<14
#define sw2 1<<15
#define sw3 1<<16
#define lcd 0XF<<20
#define RS 1<<17 
#define RW 1<<18
#define E  1<<19
typedef unsigned int u32;
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
void COMMAND(unsigned char data){
 	IOCLR1=lcd;
	IOSET1=(data&0XF0)<<16;
	IOCLR1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;

	IOCLR1=lcd;
	IOSET1=(data&0X0F)<<20;
	IOCLR1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;

}
void INIT(void)
{
	IODIR1=lcd|RS|E|RW;
	COMMAND(0X01);//TO CLEAR THE DISPLAY
	COMMAND(0X02);//TO RETURN CURSOR TO HOME
	COMMAND(0X0C);//DISPLAY ON CURSOR OFF
	COMMAND(0X28);//8-BIT INTERFACING
	COMMAND(0X80);//TO SELECT DDRAM
}
void DATA(unsigned char data){
 	IOCLR1=lcd;
	IOSET1=(data&0XF0)<<16;
	IOSET1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;

	IOCLR1=lcd;
	IOSET1=(data&0X0F)<<20;
	IOSET1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;

}

void STR(unsigned char* p)
{
	while(*p){
	 	DATA(*p++);
	}
}
void can_init(){
 PINSEL1|=0X00014000;//P0.23 P0.24
 VPBDIV=1;
 C2MOD=0x1;
 AFMR=0X02;
 C2BTR=0X001C001D;
 C2MOD=0x0;
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
int main(){
 struct can m1;
 //IODIR0|=sw1|sw2|sw3;
 INIT();
 can_init();
 m1.rtr=0;
 m1.dlc=4;
 m1.byteA=0xffffffff;
 m1.byteB=0;
 COMMAND(0X80);
 STR("PROJECT");
 while (1) {
        
        if ((IOPIN0 & sw1) == 0) {  
            m1.id = 0x01;    
            can_tx(m1);
			COMMAND(0XC0); 
			 STR("LEFT...");     
            //delay_ms(50);    
        }

       
        if ((IOPIN0 & sw2) == 0) {  
            m1.id = 0x02;    
            can_tx(m1);
			COMMAND(0XC0);
			STR("RIGHT..");       
            //delay_ms(50);   
        }

       
        if ((IOPIN0 & sw3) == 0) {  
            m1.id = 0x03;    
            can_tx(m1); 
			COMMAND(0XC0);
			STR("WIPER...");      
            //delay_ms(50);    
        }
    }	
}

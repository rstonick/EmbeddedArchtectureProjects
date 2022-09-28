/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

bool I2C_ReadBytes(uint8_t slaveAddr, uint8_t registerNum, int dataLength, uint8_t *userArray);
bool I2C_WriteBytes(uint8_t slaveAddr, uint8_t registerNum, int dataLength, uint8_t *userArray);

#define DS3231_SLAVE_ADDR       0x68

#define DS3231_REG_SECONDS      0x00  
#define DS3231_REG_MINUTES      0x01
#define DS3231_REG_HOURS        0x02
#define DS3231_REG_DAY          0x03
#define DS3231_REG_DATE         0x04
#define DS3231_REG_MONTH        0x05
#define DS3231_REG_YEAR         0x06

#define DS3231_BIT_12HRFORMAT   0x40
#define DS3231_BIT_24HRFORMAT   0x00
#define DS3231_BIT_AM           0x00
#define DS3231_BIT_PM           0x20
#define DS3231_REG_CONTROL      0x0E
#define DS3231_REG_STATUS       0x0F

#define DS3231_DAY_SUN          0x01
#define DS3231_DAY_MON          0x02
#define DS3231_DAY_TUE          0x03
#define DS3231_DAY_WED          0x04
#define DS3231_DAY_THU          0x05
#define DS3231_DAY_FRI          0x06

#define DEC2BCD(dec) (((dec/10)<<4) + (dec % 10))
#define BCD2DEC(bcd) ((bcd>>4)*10 + (bcd&0x0f))


int main(void)
{
    
    uint8_t buf[7];
    int     i;
    uint8_t ss, mm, hh, day, DD, MM, YY;
    bool    isPm;
    char *weekDay[] = {"SU","MO","TU","WE","TR","FR","SA"};
    char str[41];
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
        I2C_Start();
        LCD_Start();
        LCD_ClearDisplay();
        LCD_Position(0,0);
        LCD_PrintString("EE4450 RTC");
        
    //REad Control Reg, enable bit 7 (/EOSC)
        if (I2C_ReadBytes(DS3231_SLAVE_ADDR, DS3231_REG_CONTROL, 1, buf) == true){
            if ((buf[0] & 0b10000000) == 0b10000000){
            //Enable /EOSC 
                buf[0] = buf[0] & (~0b10000000);
                I2C_WriteBytes(DS3231_SLAVE_ADDR, DS3231_REG_CONTROL, 1, buf);
            }
        }
        for(i = 0; i<250;i++){
            if (nSW1_Read() == 0){
                //Update the default date/time to RTC module
                //Default Date/Time 2022/09/19 03:44:30pm
                buf[0] = DEC2BCD(30);   //Seconds 00-59
                buf[1] = DEC2BCD(44);   //Minutes 00-59
                buf[2] = DS3231_BIT_12HRFORMAT | DS3231_BIT_AM |DEC2BCD(3);
                buf[3] = DS3231_DAY_MON; //DAY 01-07
              //buf[4] =                               
              //buf[5] =                 //Month 01-12
              //buf[6] =                 //Year                  
                
                
              
                I2C_WriteBytes(DS3231_SLAVE_ADDR, DS3231_REG_CONTROL, 7, buf);
                LCD_Position(1,0);
                LCD_PrintString("Update Data/Time");
                CyDelay(2000);
                
                break;
            }
            CyDelay(20);
        }
    LCD_ClearDisplay();
        
    for(;;)
    {
        /* Place your application code here. */
        if(I2C_ReadBytes(DS3231_SLAVE_ADDR, DS3231_REG_CONTROL, 7, buf)){
            ss = BCD2DEC(buf[0]);
          //mm =
            if ((buf[2] & 0x40) == 0x40){
                //12 HR format
                isPm = buf[2] & 0x20;
                //hh = ; // hh only valid bit4~bit0
            } else {
                //24 hr format
               //hh=; //hh only valid bit5~bit0
                if(hh >= 12){
                    isPm = true; // PM
                    if (hh >12)
                        hh = hh-12;
                } else{
                    isPm = false; // AM
                }
            }
            
            
          //  day = 
          //DD =   
          //MM = 
          //YY = 
            sprintf(str, "%4d/%2d/%2d", 2000 + YY, MM, DD);
            LCD_Position(0,0);
            LCD_PrintString(str);
            sprintf(str,"%s", weekDay[___]);
            LCD_Position(0,11);
            LCD_PrintString(str);
            
            LCD_Position(1,10);
            if (isPm == true){
                LCD_PrintString("PM");
            }else{
                LCD_PrintString("AM");
            }
             //Display time on second line of LCD Module   
            
        
    }
}
}
//--------------------------------
#define _I2C_WRITE_MODE 0
#define _I2C_READ_MODE 1

bool I2C_ReadBytes(uint8_t slaveAddr, uint8_t registerNum, int dataLength, uint8_t *userArray){
    
    uint8_t status;
    bool noError = false;
    int i;
    
    status = I2C_MasterSendStart(slaveAddr, _I2C_WRITE_MODE);
    if (status == I2C_MSTR_NO_ERROR){
        //Send registerNum
        status = I2C_MasterWriteByte(registerNum);
        if (status != I2C_MSTR_NO_ERROR)
            goto FINISHED;
        status = I2C_MasterSendRestart(slaveAddr, _I2C_READ_MODE);
        if (status != I2C_MSTR_NO_ERROR)
            goto FINISHED;
        for( i=0; i < dataLength; i++){
            if( i == (dataLength-1))
                *userArray = I2C_MasterReadByte(I2C_NAK_DATA);
            else
                *userArray = I2C_MasterReadByte(I2C_ACK_DATA);
            
            userArray++;
        }
        noError = true;
        
    }
FINISHED: 
    I2C_MasterSendStop();
    
    return noError;
}

//---------------------
bool I2C_WriteBytes(uint8_t slaveAddr, uint8_t registerNum, int dataLength, uint8_t *userArray){
        
    uint8_t status;
    bool noError = false;
    int i;
    
    status = I2C_MasterSendStart(slaveAddr, _I2C_WRITE_MODE);
    if (status == I2C_MSTR_NO_ERROR){
        //Send registerNum
        status = I2C_MasterWriteByte(registerNum);
        if (status != I2C_MSTR_NO_ERROR)
            goto FINISHED;

        for( i=0; i < dataLength; i++){
            status = I2C_MasterWriteByte(*userArray);
            if (status != I2C_MSTR_NO_ERROR)
                goto FINISHED;
            userArray++;
        }
     noError = true;
        
    }
FINISHED: 
    I2C_MasterSendStop();
    
    return noError;
}


/* [] END OF FILE */

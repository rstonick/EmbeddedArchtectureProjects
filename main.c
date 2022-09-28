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

// FreeRTOS header Files
#include "FreeRTOS_PSoC.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "event_groups.h"

// Your task functions
void vMyTask1(void *pvParameters);
void vMyTask2(void *pvParameters);

//===================================
void myPSoCSetup()
{

}
//===================================
int main(void)
{
    BaseType_t err; 
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    FreeRTOS_Init();
    myPSoCSetup();
    
    //Create Task 1 
    err = xTaskCreate( vMyTask1,                            //TaskFunction_t pvTaskCode,
                       "Task 1",                            //const char * const pcName,
                       configMINIMAL_STACK_SIZE,            //unsigned short usStackDepth,
                       NULL,                                //void *pvParameters,
                       configMAX_PRIORITIES-1,              //UBaseType_t uxPriority,
                       NULL);                               //TaskHandle_t *pxCreatedTask );
    if (err != pdPASS){
     //cannot create task!
        while(1); //Stop here
    }
    
    //Create Task 2 
    err = xTaskCreate( vMyTask2,                            //TaskFunction_t pvTaskCode,
                       "Task 2",                            //const char * const pcName,
                       configMINIMAL_STACK_SIZE,            //unsigned short usStackDepth,
                       NULL,                                //void *pvParameters,
                       configMAX_PRIORITIES-1,              //UBaseType_t uxPriority,
                       NULL);                               //TaskHandle_t *pxCreatedTask );
    if (err != pdPASS){
     //cannot create task!
        while(1); //Stop here
    }
    vTaskStartScheduler();
       
    
    for(;;)
    {
        /* Place your application code here. */
    }
}
//===============================
void vMyTask1(void *pvParameters){
    while(1){
        LED1_Write( !LED1_Read());
        // Dont use CyDelay() in RTOS;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
//===============================
void vMyTask2(void *pvParameters){
    
    while(1){
        LED2_Write( !LED1_Read());
        // Dont use CyDelay() in RTOS;
        vTaskDelay(pdMS_TO_TICKS(1000));
        
    }
    
}
//===============================

/* [] END OF FILE */

#include "App.h"
#include "App_questions.h"




#define QUESTION_NUM  4
void (* const Questions_List[QUESTION_NUM])(APP_Handle* App) = 
{
    App_Questions_Base_2,
    App_Questions_Extn_3,
    App_Questions_Key,
    App_Questions_ErrorCheck
};


void App_Questions_Scheduler(APP_Handle* App)
{
    for(uint16_t i = 0; i < QUESTION_NUM; i++)
    {
        Questions_List[i](App);
    }
    
}

void App_Questions_Key(APP_Handle* App)
{
    App_KeyTask(App);
}

void App_Questions_ErrorCheck(APP_Handle* App)
{
    App_ErrorTask(App);
}


void App_Questions_Base_2(APP_Handle* App)
{
    if(App == NULL)
    {
        return;
    }

    if(App ->App_Mode == App_Mode_Qst_2)
    {
		App_BlueToothTask(App);
		App_UART_DataDeCodeTask(App);
       // App_SensorTask(App);
       // App_OLEDTask(App);
       // App_TM1637Task(App);
		App_DEBUGTask(App);
        App_IMUMSPM0Task(App);
      //  App_BuzzerTask(App);
    }

}

void App_Questions_Extn_3(APP_Handle* App)
{
    if(App == NULL)
    {
        return;
    }
    if(App ->App_Mode == App_Mode_Qst_3)
    {
		//App_BlueToothTask(App);
		//App_UART_DataDeCodeTask(App);

		//App_OLEDTask(App);
       // App_TM1637Task(App);
       // OLED_ShowNum(0,16,123456,6,OLED_8X16);

    }
}

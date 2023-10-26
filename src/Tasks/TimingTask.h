#ifndef TASK_H
#define TASK_H

//#include <ESP8266WiFi.h>
#include <Arduino.h>


//Класс 
class TimingTask
{
  public:

    //Форcирование катшки реле
    bool Force;

    //Время начала полива, мин
    uint16_t  StartTime;

    //Продолжительность полива, мин
    uint16_t  Dutration_min;

    bool State;

    //Конструктор класса
    TimingTask();

    //Конструктор класса
    //_pin - пин, к которому подключен клапан
    TimingTask(uint16_t  startTime, uint16_t  dutration_min, int period);

    //Обновление данных. Метод вызывается в loop
    //Управляет логикой
    bool Update(tm* timeInfo, bool forceBit);


   private:

    bool StartProgramm = false;

    bool StopProgpamm = true;

    uint32_t old_millis = 0;

    uint32_t addative = 3000;

    //Количество минут
    int currentTime = 0;

    int period = 1;
         
};

#endif

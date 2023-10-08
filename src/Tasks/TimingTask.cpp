#include "Tasks\TimingTask.h"

//Конструктор класса Valve
TimingTask::TimingTask(uint16_t  startTime, uint16_t  dutration_min)
{
        this->StartTime = startTime;
        this->Dutration_min = dutration_min;

}

//Обновление данных. Метод вызывается в loop
//Управляет логикой
bool TimingTask::Update(uint16_t currentTime)
{
      //Переменная для временного хранения результата состояния пина,
      //к которому полключается клапан
      bool tempStateValve = false;

      //Проверяем есть попадает ли текущее время в диапазон
      //1. Если время начала меньше времени окончания полива
      if( StartTime < StartTime + Dutration_min)
      {
          tempStateValve = StartTime <= currentTime && currentTime < StartTime + Dutration_min;
      }

      //2. Если время начала больше времени окончания полива, т.е полив продолжается и ночью
      if( StartTime >  StartTime + Dutration_min)
      {
          bool temp_1 = StartTime <= currentTime && currentTime < 1441;
          bool temp_2 = 0 <= currentTime && currentTime < StartTime + Dutration_min;
          tempStateValve = temp_1 || temp_2;
      }

      if(StartProgramm == false && tempStateValve == true)
      {
        old_millis = millis();
      }


      //Передаем состояние пина реле
      StartProgramm = tempStateValve;

      if(StartProgramm == false)
      {
        State = false;
        return false;
      }

      if(millis() >= old_millis + addative)
      {
         State = !State;

         if(State == true)
         {
            addative += 3000;
         }
         else
         {
            addative += 10000;
         } 
      }
      
      return State;
}



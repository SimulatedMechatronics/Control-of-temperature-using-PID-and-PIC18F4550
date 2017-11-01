#include <main.h>
#include <stdlib.h>

void main()
{
   setup_adc_ports(AN0_TO_AN1);
   setup_adc(ADC_CLOCK_INTERNAL|ADC_TAD_MUL_20);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);      //13,1 ms overflow
   setup_timer_2(T2_DIV_BY_16,155,1);      //499 us overflow, 499 us interrupt

   setup_ccp1(CCP_PWM|CCP_SHUTDOWN_AC_L|CCP_SHUTDOWN_BD_L);
   
   signed long feedback = 0;
   signed long reference = 0;
   signed long error = 0;
   signed long errorSum = 0;
   signed long errorOld = 0;
   signed long control = 0;
   
   #define KP 12//12//8//12
   #define KI 0.1//0.1//0.32005//0.098
   #define KD 40//40//30//40
   
   while(TRUE)
   {
      //Capta feedback
      set_adc_channel(0);
      delay_us(50);
      feedback = read_adc(); // 204 - 614 pois 1V to 3V
      feedback = 100*((float) (feedback-205)/(409));
      
      //Capta reference
      set_adc_channel(1);
      delay_us(50);
      reference = read_adc(); //205 to 614  pois 1V to 3V
      reference = 100*((float) (reference-205)/(409));
      
      //Sinal de Erro
      error = reference - feedback;
      
      //Integracao com anti-WindUp
      if(abs(errorSum) < 16000) errorSum += error;
   
      //Acao de Control P I D
      control = ((float) KP*error) + ((float) KI*errorSum) + ((float) KD*(error - errorOld));
      
      //Saturacao do Sinal de Controle
      if(control > 100) control = 100;
      else if(control < 0) control = 0;

      //Sinal de Controle
      control = 621*((float) (control)/(100));
      set_pwm1_duty((int16) control); // 0 to 621
      
      //Atualiza ErrorOld
      errorOld = error;
   }

}

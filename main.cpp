#include "mbed.h"
#include "uLCD_4DGL.h"
#define SAMPLE_RATE 100.0

DigitalIn BOTTOM_UP(D12);     // up bottom
DigitalIn BOTTOM_DOWN(D11);   // down bottom
DigitalIn BOTTOM_SELECT(D10); // select bottom
uLCD_4DGL uLCD(D1, D0, D2); // serial tx, serial rx, reset pin;
AnalogOut aout(D7);
AnalogIn Ain(A0);

float ADCdata[270];
EventQueue queue_wave_generator(32 * EVENTS_EVENT_SIZE);
EventQueue queue_ADC_sampling(32 * EVENTS_EVENT_SIZE);
EventQueue queue_screen_print(32 * EVENTS_EVENT_SIZE);
Thread t_wave_generator;
Thread t_ADC_sampling;
Thread t_screen_print;
float d1 = 0;
float d2 = 0;
int temp_freq = 0;
float freq_state = 0;
const float FREQUENCY[4] = {12.5, 25, 50, 100};

void slew_select();
void wave_generator();
void ADC_sampling();
void screen_print();

int main()
{
    t_wave_generator.start(callback(&queue_wave_generator, &EventQueue::dispatch_forever));
    t_ADC_sampling.start(callback(&queue_ADC_sampling, &EventQueue::dispatch_forever));
    t_screen_print.start(callback(&queue_screen_print, &EventQueue::dispatch_forever));
    
    // initializing uLCD
    uLCD.cls();
    uLCD.text_width(2); //2X size text
    uLCD.text_height(2);
    // basic printf demo = 16 by 18 characters on screen
    uLCD.color(BLUE);
    uLCD.textbackground_color(GREEN);
    uLCD.locate(0,2);
    uLCD.printf("slew rate = 1");
    
    aout = 0.0;
    
    //queue_ADC_sampling.call(ADC_sampling);

    while (1) {
      slew_select();
      d1 = float(freq_state*0.9/10000);
      d2 = float(freq_state*0.9/10000);
      queue_wave_generator.call(wave_generator);
      
    } // end of while loop

} // end of main

// selecting slew rate according to the uLCD display
void slew_select() {
  // slew rate = 1
  if (temp_freq == 0) {
    if (BOTTOM_DOWN == 1) {
      uLCD.textbackground_color(GREEN);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1/2");
      temp_freq++;
    }
    if (BOTTOM_SELECT == 1) {
      uLCD.textbackground_color(RED);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1");
      freq_state = FREQUENCY[temp_freq];
    }
  }
  // slew rate = 1/2
  else if (temp_freq == 1) {
    if (BOTTOM_UP == 1) {
      uLCD.textbackground_color(GREEN);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1");
      uLCD.textbackground_color(BLACK);
      uLCD.printf("  ");
      temp_freq--;
    }
    else if (BOTTOM_DOWN == 1) {
      uLCD.textbackground_color(GREEN);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1/4");
      temp_freq++;
    }
    if (BOTTOM_SELECT == 1) {
      uLCD.textbackground_color(RED);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1/2");
      freq_state = FREQUENCY[temp_freq];
    }
  }
  // slew rate = 1/4
  else if (temp_freq == 2) {
    if (BOTTOM_UP == 1) {
      uLCD.textbackground_color(GREEN);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1/2");
      temp_freq--;
    }
    else if (BOTTOM_DOWN == 1) {
      uLCD.textbackground_color(GREEN);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1/8");
      temp_freq++;
    }
    if (BOTTOM_SELECT == 1) {
      uLCD.textbackground_color(RED);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1/4");
      freq_state = FREQUENCY[temp_freq];
    }
  }
  // slew rate = 1/8
  else if (temp_freq == 3) {
    if (BOTTOM_UP == 1) {
      uLCD.textbackground_color(GREEN);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1/4");
      temp_freq--;
    }
    if (BOTTOM_SELECT == 1) {
      uLCD.textbackground_color(RED);
      uLCD.locate(0,2);
      uLCD.printf("slew rate = 1/8");
      freq_state = FREQUENCY[temp_freq];
    }
  }
}

// generating wave according to the frequency selected
void wave_generator() {
  aout = 0;
  wait_us(70);
  for (int i = 0; freq_state != 0 && i*d1 <= 0.9; i++) {
    aout = aout + d1;
    wait_us(70);
  }

  aout = 0.9;
  wait_us(70);

  if (freq_state == 12.5) {
      ThisThread::sleep_for(80ms);
  }
  else if (freq_state == 25) {
      ThisThread::sleep_for(160ms);
  }
  else if (freq_state == 50) {
      ThisThread::sleep_for(200ms);
  }
  else if (freq_state == 100) {
      ThisThread::sleep_for(220ms);
  }

  for (int i = 0; freq_state != 0 && (0.9-i*d2) >= 0.0; i++) {
    aout = aout - d2;
    wait_us(70);
  }
}

// sampling the wave generated with the sampling rate 'SAMPLE_RATE' per second
// and save the data into the array 'ADCdata'
void ADC_sampling() {

  ThisThread::sleep_for(3000ms);

  for (int i = 0; i < SAMPLE_RATE; i++){

    ADCdata[i] = Ain;

    ThisThread::sleep_for(1000ms/(int)SAMPLE_RATE);

  }
  queue_screen_print.call(screen_print);
}

// print out the data which is saved in the array 'ADCdata'
void screen_print() {
  for (int i = 0; i < SAMPLE_RATE; i++){

    printf("%f\r\n", ADCdata[i]);
    
    ThisThread::sleep_for(10ms);

  }
  queue_ADC_sampling.call(ADC_sampling);
}
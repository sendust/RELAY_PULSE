/*
 *   ATTINY 85 DIGITAL SWITCH INPUT EXAMPLE
 *   PB 0 (5): INPUT, PUSH BUTTON (PULL UP)
 *   PB 1 (6): OUTPUT, LED (heartbeat, Digispark board interlanl)
 *   PB 2 (7): OUTPUT, RELAY
 *   PB 3 (2): OUTPUT, feedback LED #1  (USB+)
 *   PB 4 (3): OUTPUT, feedback LED #2  (USB-)
 *   PB 5 (1): RESET (Active LOW)
 * 
 *   pin4 : GND
 *   pin8 : 5V
 *   
 *   constant value made for Digispark 1Mhz, no USB
 *   2022/9/22, managed by sendust
 *   
 *   LED, SWITCH : Minus common
 *   Relay : ON at LOW
 *   
 */

 #define NONE 0
 #define SHORT 1
 #define LONG 2
 #define RELAY_ON 0
 #define RELAY_OFF 1
 
class fb_led
{
  public:
    fb_led(uint8_t pin_nbr)
    {
      pin_output = pin_nbr;
      pinMode(pin_nbr, OUTPUT);
      state_now = 0;
      state_old = 0;
      count = 0;
    }

    void on()
    {
      state_now = 1;
      digitalWrite(pin_output, state_now);
      state_old = state_now;
    }


    void off()
    {
      state_now = 0;
      digitalWrite(pin_output, state_now);
      state_old = state_now;
    }


    void toggle()
    {
      state_now = !state_old;
      digitalWrite(pin_output, state_now);
      state_old = state_now;
    }    

  private:
    uint16_t count;
    uint8_t state_now, state_old, pin_output;

  
};

class pulse_relay   // relay on at TTL LOW
{
  public:
    pulse_relay(uint8_t pin_nbr)
    {
      pinMode(pin_nbr, OUTPUT);
      state_now = RELAY_OFF;
      state_old = RELAY_OFF;
      count = 0;
      pin_output = pin_nbr;
    }

    void off()
    {
      digitalWrite(pin_output, RELAY_OFF);
    }

    void set_count()
    {
      count = count_max;
      digitalWrite(pin_output, RELAY_ON);        // Switch on relay
    }

    void run()
    {
      if (count) count--;
      if (count == 1)
      {
          digitalWrite(pin_output, RELAY_OFF);   // Switch off relay
          count = 0;
      }
    }

  private:
    const uint16_t count_max = 2000;        // Pulse width (Big number = Large pulse width)
    uint16_t count;
    uint8_t state_now, state_old, pin_output;

  
};



class pushbutton
{
  public:
    pushbutton(uint8_t pin_nbr)
    {
      pinMode(pin_nbr, INPUT_PULLUP);
      state_now = 0;
      state_old = 0;
      count = 0;
      pin_input = pin_nbr;
      time_old = 0;
    }

  uint8_t run()
  {
      result = NONE;
      state_now = digitalRead(pin_input);
      if ((state_now == 1) && (state_old == 0))   // Button released.. record transition time
      {
        time_old = millis();
      } 
      if ((state_now == 0) && (state_old == 1))     // Detect falling edge
      {
        time_now = millis();
        count_longpress = 0;
        if ((time_now - time_old) > 100)
          result = SHORT;
      }

      if ((state_now == 0) && (state_old == 0))
      {
          count_longpress++;
          if (count_longpress == 4000)
          {
            result = LONG;
          }
      }
      
      state_old = state_now;
      return result;

  }



  private:
    uint16_t count, count_longpress;
    uint8_t state_now, state_old;
    uint8_t pin_input;
    unsigned long time_now, time_old;
    uint8_t result;
};


class heartbeat
{
  public:
    heartbeat(uint8_t pin_nbr)
    {
      pinMode(pin_nbr, OUTPUT);
      state_now = 0;
      state_old = 0;
      pin_output = pin_nbr;
      count = 0;
    }

  void run()
  {
    count++;
    if (count == 1)
    {
      state_now = !state_old;
      digitalWrite(pin_output, state_now);
    }

    if (count == 5000)       // Define blink period (small number -> fast blinking)
      count = 0;
      
    state_old = state_now;
  }

  private:
    uint16_t count;
    uint8_t state_now, state_old, pin_output;
  
};




  fb_led LEDA(3);
  fb_led LEDB(4);
  heartbeat HB(1);
  pushbutton BTN(0);
  pulse_relay RELAY(2);

void setup() {
  // put your setup code here, to run once:

  LEDA.off();
  LEDB.off();
  RELAY.off();
}

void loop() {
  // put your main code here, to run repeatedly:
  HB.run();
  
  switch(BTN.run())
  {
    case NONE:
      break;

    case SHORT:
      LEDA.toggle();
      RELAY.set_count();
      break;

    case LONG:
      LEDA.toggle();
      break;
  }

  RELAY.run();
}

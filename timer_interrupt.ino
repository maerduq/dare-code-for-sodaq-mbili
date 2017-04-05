/*!
 * Initialize timer 1 for 1 Hz interval
 */
int timer1_counter;
void timer_interrupt_setup() {
  // initialize timer1   
  noInterrupts();           // disable all interrupts
  timer1_counter = 3036; // preload timer for 1Hz
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

  interrupts();             // enable all interrupts
}

int seconds = 0;
#define TIMER1_PERIOD 60 // period of the timer in seconds)
ISR(TIMER1_OVF_vect) {
  TCNT1 = timer1_counter;   // preload timer
  seconds++;
  /*!
   * If period is reached, set the YouMaySend flag
   */
  if (seconds == TIMER1_PERIOD) {
    seconds = 0;
    YouMaySend = true;
  }
}


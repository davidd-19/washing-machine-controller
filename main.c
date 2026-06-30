#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

//pin definitions
#define LED_PIN     15
#define BUTTON_PIN  14

//constants
#define LONG_PRESS_TIME 2000   // ms
#define DEBOUNCE_TIME   50     // ms

//states
typedef enum {
    OFF,
    WASH,
    RINSE,
    SPIN
} State;

//washing machine struct (to avoid global variables)
typedef struct {
    State state;

    uint32_t state_start_time;
    uint32_t wash_time_ms;

    //LED blinking
    bool led_on;
    uint32_t last_led_toggle;

    //button handling
    bool last_button_state;
    uint32_t button_press_time;
    uint32_t last_button_change;
} WashingMachine;

//returns current time in milliseconds
uint32_t get_time_ms() {
    return to_ms_since_boot(get_absolute_time());
}

//read potentiometer and map it to 0–10 seconds
uint32_t read_wash_time() {
    uint16_t adc_value = adc_read();      // 0–4095
    return (adc_value * 10000) / 4095;    // 0–10000 ms
}

//change state and print debug info
void change_state(WashingMachine *wm, State new_state) {
    wm->state = new_state;
    wm->state_start_time = get_time_ms();

    printf("STATE -> ");

    if (new_state == OFF) {
        printf("OFF\n");
        gpio_put(LED_PIN, 0);
    }
    else if (new_state == WASH) {
        printf("WASH (%d ms)\n", wm->wash_time_ms);
        gpio_put(LED_PIN, 1);
    }
    else if (new_state == RINSE) {
        printf("RINSE (%d ms)\n", wm->wash_time_ms / 2);
    }
    else if (new_state == SPIN) {
        printf("SPIN (%d ms)\n", (wm->wash_time_ms * 3) / 2);
    }
}

//blink LED without blocking
void blink_led(WashingMachine *wm, uint32_t interval_ms) {
    uint32_t now = get_time_ms();

    if (now - wm->last_led_toggle >= interval_ms) {
        wm->last_led_toggle = now;
        wm->led_on = !wm->led_on;
        gpio_put(LED_PIN, wm->led_on);
    }
}

//handle button input with debounce and press duration
void handle_button(WashingMachine *wm) {
    uint32_t now = get_time_ms();
    bool button_pressed = (gpio_get(BUTTON_PIN) == 0);

    //check for button state change
    if (button_pressed != wm->last_button_state) {

        //debounce
        if (now - wm->last_button_change > DEBOUNCE_TIME) {
            wm->last_button_change = now;

            //button pressed
            if (button_pressed) {
                wm->button_press_time = now;
            }
            //button released
            else {
                uint32_t press_duration = now - wm->button_press_time;

                //long press in OFF state
                if (wm->state == OFF && press_duration >= LONG_PRESS_TIME) {
                    wm->wash_time_ms = read_wash_time();
                    change_state(wm, WASH);
                }

                //short press in active states
                else if (wm->state != OFF && press_duration < LONG_PRESS_TIME) {
                    change_state(wm, OFF);
                }
            }

            wm->last_button_state = button_pressed;
        }
    }
}

//finite state machine logic
void update_fsm(WashingMachine *wm) {
    uint32_t now = get_time_ms();
    uint32_t elapsed = now - wm->state_start_time;

    if (wm->state == WASH) {
        if (elapsed >= wm->wash_time_ms) {
            change_state(wm, RINSE);
        }
    }

    else if (wm->state == RINSE) {
        blink_led(wm, 1000); //slow blink
        if (elapsed >= wm->wash_time_ms / 2) {
            change_state(wm, SPIN);
        }
    }

    else if (wm->state == SPIN) {
        blink_led(wm, 200); //fast blink
        if (elapsed >= (wm->wash_time_ms * 3) / 2) {
            change_state(wm, OFF);
        }
    }
}

int main() {
    stdio_init_all();

    //LED setup
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    //button setup
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    //ADC setup
    adc_init();
    adc_gpio_init(26);   // GPIO26 = ADC0
    adc_select_input(0);

    //create and initialize washing machine
    WashingMachine wm = {0};
    wm.state = OFF;

    printf("Washing machine controller started\n");

    while (true) {
        handle_button(&wm);
        update_fsm(&wm);
        sleep_ms(10);
    }
}

//used enum to represent states and a switch/if structure to control transitions based on time and button input
//code is non-blocking beacuse it checks elapsed time instead of using delays
//i debounce the button by only accepting state changes if the signal is stable for a short time 
//short press vs long press is calculated by this "press_duration = release_time - press_time;" 
//the potentiometer is read using the ADC and mapped to a time value
//stored all washing machine variables inside a struct and passed it to functions
//i print state changes and timing values to the serial console for debugging
//the button input uses an internal pull-up resistor, so the input is normally HIGH and goes LOW when the button is pressed (gpio_pull_up(BUTTON_PIN));

/*
 * FRDM-KL25Z — Dual Motor Control via L298 (MBED)
 * ------------------------------------------------
 * Explicit GPIO logic with adjustable PWM duty and timing.
 *
 * Motor driver: L298N
 * - Left motor: left_in1, left_in2, left_pwm (PWM)
 * - Right motor: right_in1, right_in2, right_pwm (PWM)
 * PWM frequency: 20 kHz
 */

#include "mbed.h"
#include <cmath>
#include <cstdint>

#include "wav_player.h"

#include "car_x_pcm.h"

// === Pin assignments (adjust if needed) ===

// DAC output
AnalogOut dac(PTE30);

// Left motor
DigitalOut left_in2(D7);
DigitalOut left_in1(D6);
PwmOut     left_pwm(D5);

// Right motor
DigitalOut right_in2(D4);
DigitalOut right_in1(D3);
PwmOut     right_pwm(D2);

// RGB LEDs (onboard KL25Z)
DigitalOut LED_R(LED_RED);
DigitalOut LED_G(LED_GREEN);
DigitalOut LED_B(LED_BLUE);

// === Constants ===
#define PWM_FREQ_HZ 20000.0f
#define DUTY_MAX    1.0f   // mbed::PwmOut uses 0.0–1.0 range for duty cycle

// === LED helper ===
void leds_set(bool r, bool g, bool b) {
    LED_R = !r; // KL25Z LEDs are active-low
    LED_G = !g;
    LED_B = !b;
}

//========================SOUND=========================

Ticker audioTick;

// Playback config (most common voice setting)
static constexpr int SAMPLE_RATE = 8000;

// Playback state
static volatile uint32_t sample_i = 0;
static volatile bool playing = false;

// Read one int16 little-endian sample from the byte array
static inline int16_t read_i16le(const unsigned char* p) {
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

void audio_isr() {
    if (!playing) {
        dac.write(0.5f);
        return;
    }

    uint32_t byte_i = sample_i * 2;
    if (byte_i + 1 >= lib_Sounds_car_x_pcm_len) {
        playing = false;
        sample_i = 0;
        dac.write(0.5f);
        return;
    }

    int16_t s = read_i16le(&lib_Sounds_car_x_pcm[byte_i]);
    sample_i++;

    // Map -32768..+32767 -> 0.0..1.0
    float v = 0.5f + (float)s / 65536.0f;
    dac.write(v);
}

// Call this to play the clip once
void play_car_x() {
    sample_i = 0;
    playing = true;
}

// Optional
bool is_playing() { return playing; }

//=================================================

// === PWM helper ===
void motors_set_duty_sync(float left_duty, float right_duty) {
    left_pwm.write(left_duty);
    right_pwm.write(right_duty);
}

// === Safe shutdown ===
void motors_all_off() {
    motors_set_duty_sync(0.0f, 0.0f);

    // Coast both motors
    left_in1 = 0; left_in2 = 0;
    right_in1 = 0; right_in2 = 0;

    leds_set(false, false, false);
}

// === Stop modes ===

// Soft stop: let motors freewheel
void motors_coast(int duration_ms) {
    printf("Coasting...\n");
    leds_set(true, true, false);  // Yellow
    motors_all_off();
    thread_sleep_for(duration_ms);
}

// Hard stop: short both motor terminals to brake
void motors_brake(float strength, int duration_ms) {
    printf("Braking at %.0f%%\n", strength * 100);
    leds_set(false, false, true); // Blue

    // Both inputs HIGH -> motor terminals shorted (active braking)
    left_in1 = 1; left_in2 = 1;
    right_in1 = 1; right_in2 = 1;

    // Apply PWM to control braking torque
    left_pwm.write(strength);
    right_pwm.write(strength);

    thread_sleep_for(duration_ms);
    motors_all_off(); // release brake to coast
}

// === Motion routines ===
void move_forward(float duty, int duration_ms) {
    printf("Forward at %.0f%%\n", duty * 100);
    leds_set(false, true, false); // Green

    left_in1 = 1; left_in2 = 0; // Left forward
    right_in1 = 1; right_in2 = 0; // Right forward

    motors_set_duty_sync(duty, duty);
    thread_sleep_for(duration_ms);
    motors_all_off();
}

void move_forward_different(float dut_R, float dut_L, int duration_ms) {
    leds_set(false, true, false); // Green

    left_in1 = 1; left_in2 = 0; // Left forward
    right_in1 = 1; right_in2 = 0; // Right forward

    motors_set_duty_sync(dut_L, dut_R);
    thread_sleep_for(duration_ms);
    motors_all_off();
}

void move_backward(float duty, int duration_ms) {
    printf("Backward at %.0f%%\n", duty * 100);
    leds_set(true, false, false); // Red

    left_in1 = 0; left_in2 = 1; // Left backward
    right_in1 = 0; right_in2 = 1; // Right backward

    motors_set_duty_sync(duty, duty);
    thread_sleep_for(duration_ms);
    motors_all_off();
}

void turn_left_skid_reverse_inner(float duty_outer, int duration_ms) {
    printf("Turn left (reverse inner)\n");
    leds_set(true, false, true); // Magenta

    left_in1 = 0; left_in2 = 1; // Left reverse
    right_in1 = 1; right_in2 = 0; // Right forward

    motors_set_duty_sync(duty_outer, duty_outer);
    thread_sleep_for(duration_ms);
    motors_all_off();
}

void turn_left_break_inner(float break_inner, float duty_outer, int duration_ms) {
    printf("Turn left (coast inner)\n");
    leds_set(true, true, false); // Yellow

    left_in1 = 1; left_in2 = 1; // Left break
    right_in1 = 1; right_in2 = 0; // Right forward

    left_pwm.write(break_inner);
    right_pwm.write(duty_outer);
    
    thread_sleep_for(duration_ms);
    motors_all_off();
}

void turn_right_skid_reverse_inner(float duty_outer, int duration_ms) {
    printf("Turn right (reverse inner)\n");
    leds_set(false, true, true); // Cyan

    left_in1 = 1; left_in2 = 0; // Left forward
    right_in1 = 0; right_in2 = 1; // Right reverse

    motors_set_duty_sync(duty_outer, duty_outer);
    thread_sleep_for(duration_ms);
    motors_all_off();
}

void turn_right_break_inner(float break_inner, float duty_outer, int duration_ms) {
    printf("Turn left (coast inner)\n");
    leds_set(true, true, false); // Yellow

    left_in1 = 1; left_in2 = 0; // Left go
    right_in1 = 1; right_in2 = 1; // Right break

    left_pwm.write(duty_outer);
    right_pwm.write(break_inner);

    thread_sleep_for(duration_ms);
    motors_all_off();
}

void turn_right_coast_inner(float duty_outer, int duration_ms) {
    printf("Turn left (coast inner)\n");
    leds_set(true, true, false); // Yellow

    left_in1 = 1; left_in2 = 0; // Left go
    right_in1 = 0; right_in2 = 0; // Right coast

    left_pwm.write(duty_outer);
    right_pwm.write(0.0f);

    thread_sleep_for(duration_ms);
    motors_all_off();
}

void turn_left_coast_inner(float duty_outer, int duration_ms) {
    printf("Turn left (coast inner)\n");
    leds_set(true, true, false); // Yellow

    left_in1 = 0; left_in2 = 0; // Left coast
    right_in1 = 1; right_in2 = 0; // Right go

    right_pwm.write(duty_outer);
    left_pwm.write(0.0f);

    thread_sleep_for(duration_ms);
    motors_all_off();
}


int main() {
    /*
    printf("Dual-motor control demo (KL25Z + L298 + mbed)\n");

    // Set PWM frequency
    left_pwm.period(1.0f / PWM_FREQ_HZ);
    right_pwm.period(1.0f / PWM_FREQ_HZ);

    motors_all_off(); // Ensure safe startup
    
    //============DEMO============

        printf("\n=== FULL MOTION DEMO START ===\n");
        
        // -----------------------------------------------------
        // 1. Basic forward and backward
        // -----------------------------------------------------
        /*
        move_forward(0.2f, 1000);
        move_forward(0.15f, 5000);
        motors_brake(0.5f, 800);
    5

        // -----------------------------------------------------
        // Right degree turns
        // ----------------------------------------------------
        turn_left_skid_reverse_inner(0.65f, 1600);
        motors_brake(0.5f, 200);

        turn_right_skid_reverse_inner(0.7f, 1600);
        motors_brake(0.5f, 200);

        // -----------------------------------------------------
        // Smooth Turns
        // ---------------------------------------------------

        move_forward_different(0.05f, 0.3f, 5000);
        motors_brake(0.5f, 1000);
        */

        // *** Critical fix: prevent deep sleep so Ticker keeps running ***
        // Turn LEDs off
        dac.write(0.5f);
        audioTick.attach(&audio_isr, 1.0f / SAMPLE_RATE);

        while (true) {
            play_car_x();                 // play once
            while (is_playing()) {
                ThisThread::sleep_for(10ms);
            }
            ThisThread::sleep_for(1500ms);
        }
}


#define F_CPU 16000000UL // Define CPU frequency as 16MHz
#include <avr/io.h>      // Standard AVR IO library
#include <util/delay.h>  // Standard Delay library

/* --- MACROS FOR EASY CONFIGURATION --- */
// Sensors (Input)
#define SENSOR_PORT PIND
#define LEFT_SENSOR PD2
#define RIGHT_SENSOR PD3
#define OBSTACLE_SENSOR PD4

// Motors (Output)
#define MOTOR_PORT PORTB
#define MOTOR_DDR DDRB
#define LM_POS PB0  // Left Motor Positive
#define LM_NEG PB1  // Left Motor Negative
#define RM_POS PB2  // Right Motor Positive
#define RM_NEG PB3  // Right Motor Negative

/* --- FUNCTION PROTOTYPES --- */
void init_hardware(void);
void move_forward(void);
void turn_left(void);
void turn_right(void);
void stop_robot(void);

int main(void)
{
	init_hardware(); // Setup Input/Output Pins

	while (1)
	{
		// 1. Check for Obstacles FIRST (Safety Priority)
		// Assuming Obstacle Sensor goes LOW (0) when clear, and HIGH (1) when blocked
		if (SENSOR_PORT & (1 << OBSTACLE_SENSOR))
		{
			stop_robot();
			_delay_ms(100); // Small debounce/wait
			continue;       // Skip the rest of the loop
		}

		// 2. Read Line Sensors
		// (1 = Black Line, 0 = White Surface)
		int left_val = (SENSOR_PORT & (1 << LEFT_SENSOR));
		int right_val = (SENSOR_PORT & (1 << RIGHT_SENSOR));

		// 3. Navigation Logic
		if (!left_val && !right_val)
		{
			// Both White -> Move Forward
			move_forward();
		}
		else if (left_val && !right_val)
		{
			// Left is Black -> Turn Left to correct
			turn_left();
		}
		else if (!left_val && right_val)
		{
			// Right is Black -> Turn Right to correct
			turn_right();
		}
		else
		{
			// Both Black -> Stop (End of course or Intersection)
			stop_robot();
		}
	}
}

/* --- DRIVER IMPLEMENTATIONS --- */

void init_hardware(void)
{
	// Set Motor Pins as OUTPUT (1)
	MOTOR_DDR |= (1 << LM_POS) | (1 << LM_NEG) | (1 << RM_POS) | (1 << RM_NEG);
	
	// Set Sensor Pins as INPUT (0)
	DDRD &= ~((1 << LEFT_SENSOR) | (1 << RIGHT_SENSOR) | (1 << OBSTACLE_SENSOR));
	
	// Optional: Enable Internal Pull-up resistors for sensors if needed
	// PORTD |= (1 << LEFT_SENSOR) | (1 << RIGHT_SENSOR);
}

void move_forward(void)
{
	// Left Motor Forward, Right Motor Forward
	MOTOR_PORT = (1 << LM_POS) | (0 << LM_NEG) | (1 << RM_POS) | (0 << RM_NEG);
}

void turn_left(void)
{
	// Left Motor Stop/Back, Right Motor Forward
	// Using "Differential Drive" for sharper turns
	MOTOR_PORT = (0 << LM_POS) | (0 << LM_NEG) | (1 << RM_POS) | (0 << RM_NEG);
}

void turn_right(void)
{
	// Left Motor Forward, Right Motor Stop/Back
	MOTOR_PORT = (1 << LM_POS) | (0 << LM_NEG) | (0 << RM_POS) | (0 << RM_NEG);
}

void stop_robot(void)
{
	// All Motor Pins LOW
	MOTOR_PORT &= ~((1 << LM_POS) | (1 << LM_NEG) | (1 << RM_POS) | (1 << RM_NEG));
}
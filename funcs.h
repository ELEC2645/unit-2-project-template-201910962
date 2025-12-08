#ifndef FUNCS_H
#define FUNCS_H

//  Menu Item Handlers  
void menu_item_1(void);

float decode_resistor(const char *band1,
                      const char *band2,
                      const char *multiplier,
                      const char *tolerance);

int   get_digit(const char *color);
float get_multiplier(const char *color);
float get_tolerance(const char *color);

void menu_item_2(void);   // Series/Parallel Calculator
void menu_item_3(void);   // RC Calculator
void menu_item_4(void);   // Ohm's Law Calculator

//  Resistor Color Code  
// example: float decode_resistor(char *b1, char *b2, char *mul, char *tol);
float decode_resistor(const char *band1, const char *band2,
                      const char *multiplier, const char *tolerance);

//  Series / Parallel Calculator  
float calc_series(const float resistors[], int count);
float calc_parallel(const float resistors[], int count);

// RC Charging / Discharging 
float rc_charge(float R, float C, float V0, float t);
float rc_discharge(float R, float C, float V0, float t);

//  Ohm’s Law & Power  
float calc_voltage(float I, float R);
float calc_current(float V, float R);
float calc_resistance(float V, float I);
float calc_power(float V, float I);

// Optional extra module 
// Signal generator
void gen_sine(float amp, float freq, float arr[], int n);
void gen_square(float amp, float freq, float arr[], int n);
void gen_triangle(float amp, float freq, float arr[], int n);

// File save
int save_to_file(const char *filename, const float data[], int count);



#endif
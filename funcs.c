#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "funcs.h"

// Basic defines and helper functions for input/output 
#define PI 3.14159265358979323846

// File used to save calculation history 
static const char *LOG_FILENAME = "calc_log.txt";

// Reads an integer in range [min, max] with validation 
// Keeps asking user until correct number is entered 
static int read_int(const char *prompt, int min, int max)
{
    char buf[64], *endptr;
    long val;

    for (;;) {
        printf("%s", prompt);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\nInput error. Exiting.\n");
            exit(1);
        }

        val = strtol(buf, &endptr, 10);

        // Check if user typed anything valid
        if (endptr == buf) {
            printf("Please enter an integer.\n");
            continue;
        }

        // Skip white spaces after number
        while (*endptr == ' ' || *endptr == '\t') endptr++;

        // Reject unexpected characters
        if (*endptr != '\n' && *endptr != '\0') {
            printf("Unexpected characters. Try again.\n");
            continue;
        }

        // Range check
        if (val < min || val > max) {
            printf("Value must be between %d and %d.\n", min, max);
            continue;
        }

        return (int)val;
    }
}

// Reads a positive floating-point number 
// Used for voltages, resistance, frequency, etc. 
static double read_positive_double(const char *prompt)
{
    char buf[64], *endptr;
    double val;

    for (;;) {
        printf("%s", prompt);

        if (!fgets(buf, sizeof(buf), stdin)) {
            printf("\nInput error. Exiting.\n");
            exit(1);
        }

        val = strtod(buf, &endptr);

        // Check numeric input
        if (endptr == buf) {
            printf("Enter a valid number.\n");
            continue;
        }

        while (*endptr == ' ' || *endptr == '\t') endptr++;

        if (*endptr != '\n' && *endptr != '\0') {
            printf("Invalid characters. Try again.\n");
            continue;
        }

        if (val <= 0.0) {
            printf("Value must be > 0.\n");
            continue;
        }

        return val;
    }
}

// Prints resistance with appropriate unit (Ω/kΩ/MΩ) 
// Helps make answers easier to understand 
static void print_resistance_value(double R)
{
    double disp = R;
    const char *unit = "Ω";

    if (fabs(R) >= 1e6) {
        disp = R / 1e6; unit = "MΩ";
    } else if (fabs(R) >= 1e3) {
        disp = R / 1e3; unit = "kΩ";
    }

    printf("Approx resistance: %.4g %s\n", disp, unit);
}

// Ask if user wants to save the result into a text file 
// Helps keep history of calculations 
static void ask_and_save(const char *summary)
{
    char buf[16];
    FILE *fp;

    printf("\nSave this result to \"%s\"? (y/n): ", LOG_FILENAME);

    if (!fgets(buf, sizeof(buf), stdin)) return;

    if (buf[0] == 'y' || buf[0] == 'Y') {
        fp = fopen(LOG_FILENAME, "a");
        if (!fp) {
            printf("Could not open log file.\n");
            return;
        }
        fprintf(fp, "%s\n", summary);
        fclose(fp);
        printf("Saved.\n");
    } else {
        printf("Not saved.\n");
    }
}

// Module 1: Resistor Color Code

// Digit band color names (Band 1 & 2)
static const char *digit_color_names[] = {
    "0 Black", "1 Brown", "2 Red", "3 Orange", "4 Yellow",
    "5 Green", "6 Blue", "7 Violet", "8 Grey", "9 White"
};

// Multiplier band color names (Band 3)
static const char *multiplier_color_names[] = {
    "0 Black x1", "1 Brown x10", "2 Red x100", "3 Orange x1k",
    "4 Yellow x10k", "5 Green x100k", "6 Blue x1M", "7 Violet x10M",
    "8 Grey x100M", "9 White x1G", "10 Gold x0.1", "11 Silver x0.01"
};

// Actual multiplier values
static const double multiplier_values[] = {
    1.0, 10.0, 100.0, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 0.1, 0.01
};

// Tolerance band (Band 4)
static const char *tolerance_color_names[] = {
    "0 Brown ±1%", "1 Red ±2%", "2 Green ±0.5%", "3 Blue ±0.25%",
    "4 Violet ±0.1%", "5 Grey ±0.05%", "6 Gold ±5%", "7 Silver ±10%"
};

// Tolerance text values
static const char *tolerance_values_str[] = {
    "±1%", "±2%", "±0.5%", "±0.25%", "±0.1%", "±0.05%", "±5%", "±10%"
};

// Print reference tables for user
static void print_digit_table(void)
{
    printf("\n== Digit Color Table (Band 1 & 2) ==\n");
    for (int i = 0; i < 10; i++) printf("%s\n", digit_color_names[i]);
}

static void print_multiplier_table(void)
{
    printf("\n== Multiplier Color Table (Band 3) ==\n");
    for (int i = 0; i < 12; i++) printf("%s\n", multiplier_color_names[i]);
}

static void print_tolerance_table(void)
{
    printf("\n== Tolerance Color Table (Band 4) ==\n");
    for (int i = 0; i < 8; i++) printf("%s\n", tolerance_color_names[i]);
}

// Convert color bands into resistance value 
static void rcc_color_to_resistance(void)
{
    int b1, b2, m, t;
    double base, R;
    char summary[256];

    printf("\n=== Color → Resistance (4-band) ===\n");

    // Select colors (by number index)
    print_digit_table();
    b1 = read_int("Select Band 1 (0–9): ", 0, 9);
    b2 = read_int("Select Band 2 (0–9): ", 0, 9);

    print_multiplier_table();
    m = read_int("Select Multiplier (0–11): ", 0, 11);

    print_tolerance_table();
    t = read_int("Select Tolerance (0–7): ", 0, 7);

    // Compute resistance
    base = b1 * 10 + b2;
    R = base * multiplier_values[m];

    printf("\n--- Result ---\n");
    printf("Bands: %s | %s | %s | %s\n",
           digit_color_names[b1], digit_color_names[b2],
           multiplier_color_names[m], tolerance_color_names[t]);

    print_resistance_value(R);
    printf("Tolerance: %s\n", tolerance_values_str[t]);

    // Prepare saved text
    snprintf(summary, sizeof(summary),
             "[Color→Resistance] (%d,%d,m=%d,t=%d) = %.6g Ω, tol %s",
             b1, b2, m, t, R, tolerance_values_str[t]);
    ask_and_save(summary);
}

// Convert numeric resistance to approximate 4-band colors 
// Uses rounding to pick 2 significant digits 
static void rcc_resistance_to_color(void)
{
    double R, base;
    int d1, d2, m = 0, rounded;
    char summary[256];

    printf("\n=== Resistance → Color (approx) ===\n");
    printf("Uses two significant digits.\n");

    R = read_positive_double("Enter resistance (Ω): ");

    // Normalize into 2 digits
    base = R;
    while (base >= 100 && m < 9) { base /= 10; m++; }
    while (base < 10 && m > 0) { base *= 10; m--; }

    rounded = (int)(base + 0.5);
    if (rounded >= 100) { rounded = 10; m++; }

    d1 = rounded / 10;
    d2 = rounded % 10;

    // Display
    printf("\n--- Suggested Colors ---\n");
    print_resistance_value(R);
    printf("Band 1: %s\n", digit_color_names[d1]);
    printf("Band 2: %s\n", digit_color_names[d2]);
    printf("Band 3: %s\n", multiplier_color_names[m]);
    printf("Band 4: (choose based on component tolerance)\n");

    snprintf(summary, sizeof(summary),
             "[Resistance→Color] R=%.6g → (%d,%d,m=%d)",
             R, d1, d2, m);
    ask_and_save(summary);
}

// Print all tables at once (for quick reference)
static void rcc_print_tables(void)
{
    printf("\n=== Resistor Color Code Tables ===\n");
    print_digit_table();
    print_multiplier_table();
    print_tolerance_table();
    printf("\n4-band meaning:\n  Band 1: 1st digit\n  Band 2: 2nd digit\n  Band 3: multiplier\n  Band 4: tolerance\n");
}

// Submenu for Resistor Color Code tool //
static void module_resistor_color_code(void)
{
    int c;
    do {
        printf("\n== Resistor Color Code Tool ==\n");
        printf("1. Color → Resistance\n");
        printf("2. Resistance → Color\n");
        printf("3. Show Tables\n");
        printf("0. Back\n");

        c = read_int("Select: ", 0, 3);

        if (c == 1) rcc_color_to_resistance();
        else if (c == 2) rcc_resistance_to_color();
        else if (c == 3) rcc_print_tables();
    } while (c != 0);
}

// Module 2: Series / Parallel Tool
// Calculates equivalent resistance for N resistors
static void module_series_parallel_resistors(void)
{
    int n, i, mode;
    double R[10], total = 0.0;
    char summary[256];

    printf("\n==== Series / Parallel Resistors ====\n");
    
    // User selects number of resistors
    n = read_int("Number of resistors (1–10): ", 1, 10);

    // Read each resistor value
    for (i = 0; i < n; i++) {
        char prompt[64];
        snprintf(prompt, sizeof(prompt),
                 "Enter R%d (Ω): ", i + 1);
        R[i] = read_positive_double(prompt);
    }

    printf("\nConnection Type:\n");
    printf("1. Series\n");
    printf("2. Parallel\n");
    mode = read_int("Select: ", 1, 2);

    // Compute result
    if (mode == 1) {
        // Series: sum up all
        for (i = 0; i < n; i++) total += R[i];
        printf("\n--- Series Result ---\n");
    } else {
        // Parallel: 1 / (sum of inverses)
        double inv_sum = 0.0;
        for (i = 0; i < n; i++) inv_sum += 1.0 / R[i];
        if (inv_sum == 0.0) { printf("Math error.\n"); return; }
        total = 1.0 / inv_sum;
        printf("\n--- Parallel Result ---\n");
    }

    print_resistance_value(total);

    // Save if user wants
    snprintf(summary, sizeof(summary),
             "Series/Parallel: n=%d, mode=%s → %.6g Ω",
             n, (mode == 1 ? "series" : "parallel"), total);
    ask_and_save(summary);
}

// Module 3: RC Charging and Discharging Tool
// Solves capacitor charging/discharging formulas
static void module_rc_charge_discharge(void)
{
    double R, C, tau, t, V, V0, Vc;
    int mode;
    char summary[256];

    printf("\n==== RC Charging/Discharging ====\n");
    printf("Use SI units: R(Ω), C(F), t(s)\n\n");

    // Read component values
    R = read_positive_double("Enter R (Ω): ");
    C = read_positive_double("Enter C (F): ");
    tau = R * C;  // Time constant

    printf("\nTime constant τ = %.6g s\n", tau);

    // Choose mode
    printf("\nCalculation mode:\n");
    printf("1. Charging: Vc(t) = V(1 - e^(-t/RC))\n");
    printf("2. Discharging: Vc(t) = V0 e^(-t/RC)\n");
    mode = read_int("Select: ", 1, 2);

    t = read_positive_double("Enter time t (s): ");

    // Compute based on formula
    if (mode == 1) {
        V = read_positive_double("Enter supply voltage V (V): ");
        Vc = V * (1.0 - exp(-t / tau));
        printf("\n--- Charging Result ---\n");
        printf("Vc(t = %.6g s) = %.6g V\n", t, Vc);
        snprintf(summary, sizeof(summary),
                 "RC charge: R=%.6g, C=%.6g, V=%.6g, t=%.6g → %.6g V",
                 R, C, V, t, Vc);
    } else {
        V0 = read_positive_double("Enter initial voltage V0 (V): ");
        Vc = V0 * exp(-t / tau);
        printf("\n--- Discharging Result ---\n");
        printf("Vc(t = %.6g s) = %.6g V\n", t, Vc);
        snprintf(summary, sizeof(summary),
                 "RC discharge: R=%.6g, C=%.6g, V0=%.6g, t=%.6g → %.6g V",
                 R, C, V0, t, Vc);
    }

    ask_and_save(summary);
}


// Module 4: Ohm’s Law & Power

// Solves any missing variables using combinations of V, I, R, P
static void module_ohm_and_power(void)
{
    int choice;
    double V=0, I=0, R=0, P=0;
    char summary[256];

    printf("\n==== Ohm’s Law / Power ====\n");
    printf("Choose known quantities:\n");
    printf("1. V & R\n");
    printf("2. V & I\n");
    printf("3. V & P\n");
    printf("4. I & R\n");
    printf("5. I & P\n");
    printf("6. R & P\n");

    // Select which two values are known
    choice = read_int("Select: ", 1, 6);

    switch (choice) {
    case 1: V = read_positive_double("V(V): ");
            R = read_positive_double("R(Ω): ");
            I = V / R; P = V * I; break;
    case 2: V = read_positive_double("V(V): ");
            I = read_positive_double("I(A): ");
            R = V / I; P = V * I; break;
    case 3: V = read_positive_double("V(V): ");
            P = read_positive_double("P(W): ");
            I = P / V; R = V / I; break;
    case 4: I = read_positive_double("I(A): ");
            R = read_positive_double("R(Ω): ");
            V = I * R; P = V * I; break;
    case 5: I = read_positive_double("I(A): ");
            P = read_positive_double("P(W): ");
            V = P / I; R = V / I; break;
    case 6: R = read_positive_double("R(Ω): ");
            P = read_positive_double("P(W): ");
            V = sqrt(P * R); I = V / R; break;
    default: return;
    }

    // Display calculated values
    printf("\n--- Result ---\n");
    printf("Voltage  V = %.6g V\n", V);
    printf("Current  I = %.6g A\n", I);
    printf("Resistance R = %.6g Ω\n", R);
    printf("Power     P = %.6g W\n", P);

    snprintf(summary, sizeof(summary),
             "Ohm/Power: V=%.6g, I=%.6g, R=%.6g, P=%.6g",
             V, I, R, P);
    ask_and_save(summary);
}

// Module 5: Signal Generation & Analysis
// Provides basic signal info and sample generation
static void module_signal_generation(void)
{
    int choice;

    printf("\n==== Signal Generation / Analysis ====\n");

    do {
        printf("\n1. Given f → T & ω\n");
        printf("2. Generate sine samples\n");
        printf("0. Back\n");

        choice = read_int("Select: ", 0, 2);

        if (choice == 1) {
            // Compute period and angular frequency
            double f, T, w;
            char summary[256];

            f = read_positive_double("Enter f (Hz): ");
            T = 1.0 / f;         // Period
            w = 2 * PI * f;      // Angular frequency

            printf("\n--- Result ---\n");
            printf("Period T = %.6g s\n", T);
            printf("Angular freq ω = %.6g rad/s\n", w);

            snprintf(summary, sizeof(summary),
                     "Signal: f=%.6g Hz, T=%.6g s, ω=%.6g rad/s",
                     f, T, w);
            ask_and_save(summary);

        } else if (choice == 2) {
            // Generate discrete sine wave samples
            double f, A, fs;
            int N, n;
            char summary[256];

            printf("\nSignal: x(t) = A sin(2πft)\n");
            f  = read_positive_double("Frequency f (Hz): ");
            A  = read_positive_double("Amplitude A: ");
            fs = read_positive_double("Sampling freq fs (Hz): ");
            N  = read_int("Number of samples (1–100): ", 1, 100);

            printf("\nn\t t(s)\t\t x[n]\n");
            for (n = 0; n < N; n++) {
                double t = n / fs;
                double x = A * sin(2 * PI * f * t);
                printf("%d\t %.6g\t %.6g\n", n, t, x);
            }

            snprintf(summary, sizeof(summary),
                     "Sine: f=%.6g Hz, A=%.6g, fs=%.6g Hz, N=%d",
                     f, A, fs, N);
            ask_and_save(summary);
        }

    } while (choice != 0);
}

// Module 6: File / Log Operations
// Allows user to view saved calculations or clear them
static void module_file_save_and_log(void)
{
    int choice;
    FILE *fp;

    do {
        printf("\n==== File & Log Tools ====\n");
        printf("Current log file: \"%s\"\n", LOG_FILENAME);
        printf("1. View file\n");
        printf("2. Clear file\n");
        printf("0. Back\n");

        choice = read_int("Select: ", 0, 2);

        if (choice == 1) {
            // Open and print stored results
            char line[256];
            fp = fopen(LOG_FILENAME, "r");
            if (!fp) {
                printf("No file or cannot open (maybe empty).\n");
            } else {
                printf("\n--- File Start ---\n");
                while (fgets(line, sizeof(line), fp)) fputs(line, stdout);
                printf("--- File End ---\n");
                fclose(fp);
            }

        } else if (choice == 2) {
            // Clear log file
            fp = fopen(LOG_FILENAME, "w");
            if (!fp) printf("Failed to clear file.\n");
            else {
                fclose(fp);
                printf("File cleared.\n");
            }
        }
    } while (choice != 0);
}

// Main Toolbox Selection Menu
// Central hub to choose EE tools
static void toolbox_main_menu(void)
{
    int choice;

    do {
        printf("\n====================================\n");
        printf("     Electrical Engineering Toolbox\n");
        printf("====================================\n");
        printf("1. Resistor Color Code\n");
        printf("2. Series/Parallel Resistors\n");
        printf("3. RC Charge/Discharge\n");
        printf("4. Ohm’s Law & Power\n");
        printf("5. Signal Generation/Analysis\n");
        printf("6. File/Log Tools\n");
        printf("0. Back to Main Menu\n");

        choice = read_int("Select: ", 0, 6);

        switch (choice) {
        case 1: module_resistor_color_code(); break;
        case 2: module_series_parallel_resistors(); break;
        case 3: module_rc_charge_discharge(); break;
        case 4: module_ohm_and_power(); break;
        case 5: module_signal_generation(); break;
        case 6: module_file_save_and_log(); break;
        default: break;
        }
    } while (choice != 0);
}


// Functions called from main menu
// Entry to toolbox
void menu_item_1(void) { toolbox_main_menu(); }

// Empty slots for future expansion
void menu_item_2(void)
{
    printf("\n>> Menu 2 unused. You may add your own features.\n");
}

void menu_item_3(void)
{
    printf("\n>> Menu 3 unused. You may add your own features.\n");
}

void menu_item_4(void)
{
    printf("\n>> Menu 4 unused. You may add your own features.\n");
}

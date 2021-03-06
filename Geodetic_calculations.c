#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Compile for example like: gcc -O3 -march=native -Wall -Wextra -Wfloat-equal -Werror -pedantic -std=c17 -o geodetic_calculations Geodetic_calculations.c

// Function prototypes (forward declarations):
void print_menu(void);
void clearScreen(void);
int int_input(const int lower, const int upper, const char *text);
double double_input(const double lower, const double upper, const char *text);
double double_input_no_check(const char *input_text, const char *error_text);
void dms_to_dd(void);
void dd_to_dms(void);
void normal_gravity(void);
void geodetic_to_cartesian(void);
void cartesian_to_geodetic(void);
void utm_scale_error_distance(void);
void utm_scale_error_at_point(void);


/* 
*   Main method
*/
int main(void) {
    int ret;
    clearScreen();
    const char *choose = "Choose operation by number: ";
    
    while (1) {
        print_menu();
        ret = int_input(1, 8, choose);
        switch (ret) {
            case 1:
                clearScreen();
                dms_to_dd();
                break;
            case 2:
                clearScreen();
                dd_to_dms();
                break;
            case 3:
                clearScreen();
                normal_gravity();
                break;
            case 4:
                clearScreen();
                geodetic_to_cartesian();
                break;
            case 5:
                clearScreen();
                cartesian_to_geodetic();
                break;
            case 6:
                clearScreen();
                utm_scale_error_distance();
                break;
        }

        if (ret == 7) {
            break;
        }
    }

    return 0;
}


/*
*   Main menu printer
*/
void print_menu(void) {
    printf("1. DMS to DD.DDD \
        \n2. DD.DDD to DMS \
        \n3. Normal gravity on GRS80 ellipsoid \
        \n4. Geodetic coordinates to 3D cartesian coordinates \
        \n5. 3D Cartesian coordinates to geodetic coordinates \
        \n6. UTM Scale error by easting \
        \n7. Exit \
        \n");
}


/*
*   Function to clear the screen.
*   - Somewhat portable, somewhat ugly
*/
void clearScreen(void) {
    system("clear||cls");
}


/*
*   Integer input
*/
int int_input(const int lower, const int upper, const char *text) {
    char st[200];
    int result;
    int number;
    printf("%s", text);

    do {
        fgets(st, sizeof(st), stdin);
        result = sscanf(st, "%d", &number);
        if (result < 1 || (number < lower || number > upper)) {
            printf("Invalid input. Enter a number between [%d, %d]: ", lower, upper);
        }
    }   while (result < 1 || (number < lower || number > upper));

    return number;
}


/*
*   Double input
*/
double double_input(const double lower, const double upper, const char *text) {
    char st[200];
    int result;
    double number;
    printf("%s", text);

    do {
        fgets(st, sizeof(st), stdin);
        result = sscanf(st, "%lf", &number);
        if (result < 1 || (number < lower || number > upper)) {
            printf("Invalid input. Enter a number between [%.1lf, %.1lf]: ", lower, upper);
        }
    }   while (result < 1 || (number < lower || number > upper));

    return number;
}


/*
*   Double input, no bounds
*/
double double_input_no_check(const char *input_text, const char *error_text) {
    char st[200];
    int result;
    double number;
    printf("%s", input_text);

    do {
        fgets(st, sizeof(st), stdin);
        result = sscanf(st, "%lf", &number);
        if (result < 1) {
            printf("%s", error_text);
        }
    }   while (result < 1);

    return number;
}


/*
*   DMS to DD.DDD converter
*/
void dms_to_dd(void) {
    int dms_deg, dms_min;
    char positive = 1;
    double dms_sec;
    double result;
    char *input;

    printf("Converts DMS (Degrees Minutes Seconds) to DD.DDD (Decimal degrees)\n");

    input = "Enter DMS degrees (integer): ";
    dms_deg = int_input(-180, 180, input);

    input = "Enter DMS minutes (integer): ";
    dms_min = int_input(0, 60, input);

    input = "Enter DMS seconds (real): ";
    dms_sec = double_input(0, 60, input);

    if (dms_deg < 0) {
        dms_deg *= -1;
        positive = 0;
    }

    result = (dms_deg + ((dms_min + (dms_sec / 60)) / 60));

    if (positive == 0) {
        result *= -1;
    }

    printf("\nResult in decimal degrees: %lf°\n\n", result);
}


/*
*   DD.DDD to DMS converter
*/
void dd_to_dms(void) {
    double difference = 0.0000000001; // For difference checks
    double decimal_degs;
    int deg;
    int min;
    double sec;
    const char *text = "Enter decimal degrees: ";

    printf("Converts DD.DDD (Decimal degrees) to DMS (Degrees Minutes Seconds)\n");

    decimal_degs = double_input(0, 180, text);

    deg = floor(decimal_degs);
    min = floor(60 * (decimal_degs - deg));
    sec = 3600 * (decimal_degs - deg) - (min * 60);

    if (fabs(sec - 60) < difference) {
        min += 1;
        sec = 0;
    }

    if (min == 60) {
        deg += 1;
        min = 0;
    }

    printf("\nResult: %d° %d' %lf\"\n", deg, min, sec);
    printf("DMS: %d %d %lf\n\n", deg, min, sec);
}


/*
*   Normal (theoretical) gravity on GRS80 ellipsoid
*/
void normal_gravity(void) {
    double latitude;
    double lat_radians;
    double sin_lat;
    double g;
    const char * text = "Enter latitude φ: ";

    printf("Calculates normal gravity (theoretical gravity) on given latitude. GRS80 ellipsoid.\n");

    latitude = double_input(0, 90, text);
    lat_radians = latitude * (M_PI / 180);
    sin_lat = sin(lat_radians);

    g = 978032.67715 * (1 + (0.0052790414 * (pow(sin_lat, 2))) + 
       (0.0000232718 * (pow(sin_lat, 4))) + (0.0000001262 * 
        (pow(sin_lat, 6))) + (0.0000000007 * (pow(sin_lat, 8))));

    printf("\nOn latitude φ = %f° normal gravity on GRS80 ellipsoid is %f mgal. \
        \nGravitational acceleration is %f m/s².\n\n", latitude, g, g / 100000);
}


/*
*   Geodetic coordinates to 3D cartesian coordinates
*/
void geodetic_to_cartesian(void) {
    // Define necessary GRS80 ellipsoid parameters:
    const int a = 6378137; // Semimajor axis
    const double b = 6356752.3141; // Semiminor axis

    const char *latitudeinput = "Enter latitude φ in decimal degrees: ";
    const char *longitudeinput = "Enter longitude λ in decimal degrees: ";
    const char *heightinput = "Enter ellipsoidal height in meters (GRS80 ellipsoid): ";
    const char *heightinputerror = "Invalid input. Enter height in meters: ";

    printf("Converts geodetic coordinates (λ, φ, h) to 3D cartesian (X, Y, Z) coordinates. \
        \nEnter degrees as decimal degrees and height as ellipsoidal height. \
        \nNorth positive, East positive / GRS80 ellipsoid.\n\n");

    double latitude = double_input(-90, 90, latitudeinput);
    double longitude = double_input(-180, 180, longitudeinput);
    double height = double_input_no_check(heightinput, heightinputerror);

    double cos_latitude = cos(latitude * (M_PI / 180));
    double sin_latitude = sin(latitude * (M_PI / 180));
    double cos_longitude = cos(longitude * (M_PI / 180));
    double sin_longitude = sin(longitude * (M_PI / 180));

    double n = (pow(a, 2)) / (sqrt((pow(a, 2) * pow(cos_latitude, 2)) + 
               (pow(b, 2) * pow(sin_latitude, 2))));

    double xcoord = (n + height) * cos_latitude * cos_longitude;
    double ycoord = (n + height) * cos_latitude * sin_longitude;
    double zcoord = ((pow(b, 2) / pow(a, 2) * n) + height) * sin_latitude;

    // Print cartesian coordinates rounded to 1mm accuracy:
    printf("\nX = %.3f\nY = %.3f\nZ = %.3f\n\n", xcoord, ycoord, zcoord);
}


/*
*   3D cartesian coordinates to geodetic coordinates
*/
void cartesian_to_geodetic(void) {
    // Define necessary GRS80 ellipsoid parameters:
    const int a = 6378137; // Semimajor axis
    const double b = 6356752.3141; // Semiminor axis
    const double e2 = 0.00669438002290; // First excentricity

    const char *inputx = "Enter X-coordinate in decimal meters: ";
    const char *inputy = "Enter Y-coordinate in decimal meters: ";
    const char *inputz = "Enter Z-coordinate in decimal meters: ";
    const char *inputerror = "Invalid input. Enter coordinate in decimal meters: ";

    printf("Converts 3D cartesian (X, Y, Z) coordinates to geodetic (φ, λ, h) coordinates. \
        \nHeight h is ellipsoidal height above GRS80 ellipsoid. North positive, East positive.\n");

    double xcoord = double_input_no_check(inputx, inputerror);
    double ycoord = double_input_no_check(inputy, inputerror);
    double zcoord = double_input_no_check(inputz, inputerror);

    // Longitude:
    double longitude = atan2(ycoord, xcoord) * 180 / M_PI;

    // 1st approximation of latitude:
    double tan_latitude = zcoord / ((1 - e2) * sqrt(pow(xcoord, 2) + pow(ycoord, 2)));
    double lat_radians = atan(tan_latitude);
    double latitude = lat_radians * 180 / M_PI;

    // 1st approximation of N:
    double cos_lat_radians = cos(lat_radians);
    double sin_lat_radians = sin(lat_radians);
    double n = pow(a, 2) / sqrt((pow(a, 2) * pow(cos_lat_radians, 2)) + (pow(b, 2) * 
               pow(sin_lat_radians, 2)));

    // 1st approximation of h:
    double height = (sqrt((pow(xcoord, 2) + pow(ycoord, 2)) / cos_lat_radians)) - n;

    double guess_h = 0.0;
    int count = 0;

    while (fabs(guess_h - height) > 0.0000000001) {
        guess_h = height;
        tan_latitude = zcoord / ((1 - (e2 * (n / (n + height)))) * 
                       sqrt(pow(xcoord, 2) + pow(ycoord, 2)));
        lat_radians = atan(tan_latitude);
        latitude = lat_radians * 180 / M_PI;
        n = pow(a, 2) / sqrt(pow(a, 2) * pow(cos(lat_radians), 2) + pow(b, 2) * 
            pow(sin(lat_radians), 2));
        height = ((sqrt(pow(xcoord, 2) + pow(ycoord, 2))) / cos(lat_radians)) - n;
        count++;
    }

    printf("\nIterations completed: %d\n", count);
    printf("Latitude φ: %f\nLongitude λ: %f\nEllipsoidal height h: %.3f\n\n", latitude, longitude, height);
}


/*
*   UTM scale error by distance
*/
void utm_scale_error_distance(void) {
    const double E0 = 500000.0; // UTM False easting
    printf("Calculates UTM scale error based on distance from the central meridian.\n");
    double E = double_input(0.0, 1000000.0, "Enter easting (in meters): ");

    // UTM scale error:
    double scale_err = -0.0004 + (12.29 * pow(10, -15) * pow((E - E0), 2));

    printf("\nEasting: %.3f (m)\nDistance from the central meridian: %.6f km \
        \nScale error: %.2f PPM\n\n", E, (E - E0) / 1000.0, scale_err * 1000000);
}

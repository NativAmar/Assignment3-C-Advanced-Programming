#include <stddef.h>
#ifndef Jerry_h
#include "Defs.h"
#define Jerry_h

typedef struct {
    double x, y, z;
    char *name; //Unique string
} Planet;

typedef struct {
    Planet *planet;
    char *name;
} Origin;

typedef struct {
    char *name;
    double value;
} PhysicalCharacteristics;

typedef struct {
    Origin *origin;
    int happiness_level;
    char *id;
    PhysicalCharacteristics **characteristics; //dynamic array
    int characteristics_count; //characteristics counter
} Jerry;


//Global variables to manage planet names uniqueness
extern char **planet_names;
extern int planet_number;


//Create a new Jerry object. ID should be unique for the Jerry, happiness level range 0-100 and origin pointer to the Jerry's origin.
extern Jerry* create_jerry(char *id, int happiness_level,Planet *planet, char *dimension);


//Frees all memory associated with a Jerry object. Return `success` if the operation succeeded, `failure` otherwise.
extern status destroy_jerry(Jerry *jerry);


//Creates a new Planet object. Name should be unique and the x,y,z are the coordinates.
extern Planet* create_planet(char *name, double x, double y, double z);


//Frees all memory associated with a Planet object. Return `success` if the operation succeeded, `failure` otherwise.
extern status destroy_planet(Planet *p);


//Creates a new Origin object. Recieve planet pointer to the associated Planet (must not be NULL) and dimension name.
extern Origin* create_origin(Planet *planet, char *name);


//Frees all memory associated with an Origin object. Return `success` if the operation succeeded, `failure` otherwise.
extern status destroy_origin(Origin *origin);


//Creates a new Physical Characteristic. Recieve name and value of the characteristic and return pointer to the created physical characteristic.
extern PhysicalCharacteristics* create_physical_characteristics(char *name, double value);//permission_check


//Frees all memory associated with a Physical Characteristic. Return `success` if the operation succeeded, `failure` otherwise.
extern status destroy_physical_characteristics(PhysicalCharacteristics *p);


//Checks if a specific physical characteristic exists for a given Jerry. Return `true` if the characteristic exists, `false` otherwise.
extern bool PhysicalCharacteristicExist(Jerry *jerry, char *characteristicsName);


//Adds a Physical Characteristic to a given Jerry. Return `success` if the operation succeeded, `failure` otherwise.
extern status add_physical_characteristic(Jerry *jerry, PhysicalCharacteristics *characteristic);


//Deletes a Physical Characteristic from a given Jerry. Return `success` if the operation succeeded, `failure` otherwise.
extern status delete_physical_characteristics(Jerry *jerry, char *characteristicsName);


//Prints the details of a given Jerry.
extern void print_jerry(Jerry *jerry);


void print_jerry_without_id(Jerry *jerry);


//Prints the details of a given Planet.
extern void print_plant(Planet *planet);

Element copy_jerry_key(Jerry *jerry);//add documentation

Element copy_jerry_value(Jerry *jerry);//add documentation

bool is_equal_jerry(Element jerry, Element jerry2); //add documentation

#endif
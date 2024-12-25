#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "HashTable.h"
#include "MultiValueHashTable.h"
#include "Jerry.h"
#include "Defs.h"
#include "KeyValuePair.h"


#define MAX_LINE_LENGTH 300
#define HASH_TABLE_SIZE 10
#define MULTI_VALUES_HASH_TABLE_SIZE 10


// Helper function: Parse Planet
Planet* parse_planet(const char *line) {

    if (line == NULL || strlen(line) == 0) {
        fprintf(stderr, "Error: Invalid planet line.\n");
        return NULL;
    }

    char planet_name[MAX_LINE_LENGTH];
    double x, y, z;

    // Parse the line to extract Planet details
    if (sscanf(line, "%[^,],%lf,%lf,%lf", planet_name, &x, &y, &z) != 4) {
        return NULL;
    }

    // Create the planet
    Planet *planet = create_planet(planet_name, x, y, z);
    if (!planet) {

        return NULL;
    }

    return planet;
}


//Helper function: Parse Jerry
Jerry* parse_jerry(const char *line,int number_of_planets, Planet **planets) {

    if (line == NULL || strlen(line) == 0) {
        fprintf(stderr, "Error: Invalid jerry line.\n");
        return NULL;
    }

    char id[MAX_LINE_LENGTH];
    char dimension[MAX_LINE_LENGTH];
    char planet_name[MAX_LINE_LENGTH];
    int happiness_level;

    // Parse the line to extract Jerry details
    if (sscanf(line, "%[^,],%[^,],%[^,],%d", id, dimension, planet_name, &happiness_level) != 4) {
        return NULL;
    }

    // Find the planet for the origin
    Planet *planet = NULL;
    for (int i = 0; i < number_of_planets; i++) {
        if (strcmp(planets[i]->name, planet_name) == 0) {
            planet = planets[i];
            break;
        }
    }

    if (!planet) {
        return NULL;
    }

    // Create the Origin structure
    Origin *origin = create_origin(planet, dimension);
    if (!origin) {
        return NULL;
    }

    // Create the Jerry structure
    Jerry *jerry = create_jerry(id, happiness_level, origin);
    if (!jerry) {
        destroy_planet(planet);
        destroy_origin(origin); // Clean up if Jerry creation fails
        return NULL;
    }

    return jerry;
}


status parse_config_file(const char *input_file, int number_of_planets, Planet **planets, hashTable jerries, MultiValueHashTable characteristics){
    FILE *file = fopen(input_file, "r");
    if (!file) {
        perror("Unable to open file");
        return failure;
    }

    char line[MAX_LINE_LENGTH];
    int planet_count = 0;
    int jerry_count = 0;
    int section = 0; // 0 = none, 1 = Planets, 2 = Jerries

    Jerry *current_jerry = NULL;

    while (fgets(line, sizeof(line), file)) {

        // Skip empty lines or comments
        if (line[0] == '\0' || line[0] == '#') continue;

        // Detect sections
        if (strcmp(line, "Planets\n") == 0) {
            section = 1;
            continue;
        }
        else if (strcmp(line, "Jerries\n") == 0) {
            section = 2;
            continue;
        }

        if (section == 1) {
            if (planet_count >= number_of_planets) {
                fclose(file);
                return failure;
            }
            planets[planet_count] = parse_planet(line);
            if (!planets[planet_count]) {
                printf("A memory problem has been detected in the program\n");
                fclose(file);
                return failure;
            }
            planet_count++;

        } else if (section == 2) {
            // Check if it's a characteristic or a new Jerry
            if (strchr(line, ':')) {
                if (!current_jerry) {
                    fprintf(stderr, "Error: Characteristic without a corresponding Jerry.\n");
                    fclose(file);
                    return failure;
                }

                char *content = line;
                while (*content == '\t') content++;

                char key[MAX_LINE_LENGTH];
                double value;
                if (sscanf(content, "%[^:]:%lf", key, &value) != 2) {
                    fclose(file);
                    return failure;
                }

                //physical characteristic create
                PhysicalCharacteristics *physical_char = create_physical_characteristics(key, value);
                if (!physical_char) {
                    printf("A memory problem has been detected in the program\n");
                    fclose(file);
                    return failure;
                }

                //add to the current Jerry
                status s = add_physical_characteristic(current_jerry, physical_char);

                if (s != success) {
                    printf("A memory problem has been detected in the program\n");
                    fclose(file);
                    return failure;
                }

                if (addToMultiValueHashTable(characteristics, key, current_jerry) == failure) {
                    printf("A memory problem has been detected in the program\n");
                    fclose(file);
                    return failure;
                }

            } else { //Jerry details line
                current_jerry = parse_jerry(line, number_of_planets, planets);
                if(current_jerry == NULL) {
                    printf("A memory problem has been detected in the program\n");
                    fclose(file);
                    return failure;
                }

                if (addToHashTable(jerries, current_jerry->id, current_jerry)) {
                    printf("A memory problem has been detected in the program\n");
                }
            }
        }
    }

    fclose(file);
    return success;
};

//Jerries functions
Element copy_key(Element element) {
    return copy_jerry_key((Jerry*)element);
}

status free_jerry_key(Element element) {
    free(element);
    return success;
}

status print_jerry_key(Element element) {
    if (element == NULL) {
        return failure;
    }
    char *id = (char*) element;
    printf("Jerry , ID - %s ", id); //I should delete this part from jerry print function
    return success;
}

Element copy_jerry(Element element) {
    return copy_jerry_value((Jerry*)element);
}

status free_jerry(Element element) {
    Jerry *jerry = (Jerry*)element;
    if (destroy_jerry(jerry) == success) {
        return success;
    }
    return failure;
}

status print_jerry_value(Element element) {
    Jerry *jerry = (Jerry*)element;
    if (jerry == NULL) {
        return failure;
    }
    print_jerry_without_id(jerry);
    if(jerry->characteristics_count>0) {
        printf("\n");
    }
    return success;
}

bool equal_jerry_key(Element element, Element other) {
    char *jerry_id = (char*)element;
    char *other_jerry_id = (char*)other;
    if (jerry_id == NULL || other_jerry_id == NULL) {
        return false;
    }
    if (jerry_id == other_jerry_id) {
        return true;
    }
    return false;
}

int hash_jerry(Element element) {
    if (element == NULL) {
        return -1;
    }
    int hash = 0;
    char *id = (char*)element;
    while (*id) {
        hash += (int)*id;
        id++;//move to the next char
    }
    return hash;
}


//characteristics multiValuesHashTable functions
Element copy_characteristic_key(Element element) {
    return element;
}

status print_characteristic_key(Element element) {
    if (element == NULL) {
        return failure;
    }
    char *key = (char*)element;
    printf("%s : \n", key);
    return success;
}

status print_jerry_charTable(Element element) {
    if (element == NULL) {
        return failure;
    }
    Jerry *jerry = (Jerry*)element;
    print_jerry(jerry);
    return success;
}

bool equal_characteristic_key(Element element, Element other) {
    char *key = (char*)element;
    char *other_key = (char*)other;
    if (key == NULL || other_key == NULL) {
        return false;
    }
    if (*key == *other_key) {
        return true;
    }
    return false;
}


status destroy_all(hashTable hash_table, MultiValueHashTable multi_value_hash_table, Planet **planets, int numberOfPlanets) {
    if (hash_table == NULL || multi_value_hash_table == NULL || planets == NULL) {
        return failure;
    }
    for (int i=0; i<numberOfPlanets;i++) {
        destroy_planet(planets[i]);
    }
    free(planets);
    destroyHashTable(hash_table);
    destroyMultiValueHashTable(multi_value_hash_table);
    return success;
}


int main (int argc, char *argv[]){

    int number_of_planets = atoi(argv[1]);
    char *input_file = argv[2];


    Planet **planets = malloc(number_of_planets * sizeof(Planet *));
    if (!planets) {
        perror("A memory problem has been detected in the program");
        return 0;
    }

    hashTable Jerries = createHashTable(copy_key, free_jerry_key, print_jerry_key, copy_jerry, free_jerry, print_jerry_value,
                                        equal_jerry_key, hash_jerry, HASH_TABLE_SIZE);

    MultiValueHashTable characteristics = createMultiValueHashTable(copy_characteristic_key, copy_jerry, free_jerry_key, free_jerry, print_characteristic_key,
                                                                    print_jerry_charTable, equal_characteristic_key, hash_jerry, MULTI_VALUES_HASH_TABLE_SIZE);

    if(parse_config_file(input_file, number_of_planets, planets, Jerries, characteristics) == failure) {
        printf("A memory problem has been detected in the program\n");

        destroyHashTable(Jerries);
        destroyMultiValueHashTable(characteristics);
        if (planets != NULL) {
            for (int i = 0; i < number_of_planets; i++) {
                destroy_planet(planets[i]);
            }
            free(planets);
            return 0;
        }
    }

    displayHashElements(Jerries);
    displayMultiValueHashTable(characteristics, "LimbsNumber");
    if (destroy_all(Jerries, characteristics, planets, number_of_planets) == success) {
        return 1;
    }

    return 0;
};
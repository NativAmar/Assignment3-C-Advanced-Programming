#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "HashTable.h"
#include "MultiValueHashTable.h"
#include "Jerry.h"
#include "Defs.h"


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

    // Create the Jerry structure
    Jerry *jerry = create_jerry(id, happiness_level, planet, dimension);
    if (!jerry) {
        //destroy_planet(planet); // Clean up if Jerry creation fails
        return NULL;
    }

    return jerry;
}


status parse_config_file(const char *input_file, int number_of_planets, Planet **planets, hashTable jerries, MultiValueHashTable characteristics, LinkedList jerries_list){
    FILE *file = fopen(input_file, "r");
    if (!file) {
        perror("Unable to open file");
        return failure;
    }

    char line[MAX_LINE_LENGTH];
    int planet_count = 0;
    int jerry_count = 0; //check if should deleted
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

                if (addToMultiValueHashTable(characteristics, (Element)physical_char->name, (Element)current_jerry) != success) {
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

                if (addToHashTable(jerries, (Element)current_jerry->id, (Element)current_jerry)) {
                    printf("A memory problem has been detected in the program\n");
                    fclose(file);
                    return failure;
                }

                if (appendNode(jerries_list, current_jerry) != success) {
                    printf("A memory problem has been detected in the program\n");
                    fclose(file);
                    return failure;
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
    //free(element); //is part of jerry structure, shouldnt to that twice
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
Element deepCopy_str(Element element) {
    if (element == NULL) {
        return NULL;
    }
    char *key = (char*)element;
    char *copy_Key = (char*) malloc(strlen(key) + 1);
    if (copy_Key == NULL) {
        return NULL;
    }
    strcpy(copy_Key, key);
    return copy_Key;
}

status freeStr(Element element) {
    if (element == NULL) {
        return failure;
    }
    free((char*) element);
    return success;
}

status printStr(Element element) {
    if (element == NULL) {
        return failure;
    }
    char *key = (char*)element;
    printf("%s : \n", key);
    return success;
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
    if (strcmp((char*) key, (char*) other_key) == 0) {
        return true;
    }
    return false;
}

Element copy_value_charTable (Element element) {
    return element;
}

status free_key_charTable(Element element) {
    return success;
}

status free_value_charTable(Element element) {
    return success;
}


status destroy_all(hashTable hash_table, MultiValueHashTable multi_value_hash_table, Planet **planets, int numberOfPlanets, LinkedList jerries_list) {
    if (hash_table == NULL || multi_value_hash_table == NULL || planets == NULL) {
        return failure;
    }
    for (int i=0; i<numberOfPlanets;i++) {
        destroy_planet(planets[i]);
    }
    free(planets);
    destroyHashTable(hash_table);
    destroyMultiValueHashTable(multi_value_hash_table);
    destroyList(jerries_list);
    return success;
}

//made to avoid double free
status fake_free_jerry(Element element) {
    return success;
}

void printMenu() {
    printf("Welcome Rick, what are yout Jerry's needs today ? \n");
    printf("1 : Take this Jerry away from me \n");
    printf("2 : I think I remember something about my Jerry \n");
    printf("3 : Oh wait. That can't be right \n");
    printf("4 : I guess I will take back my Jerry now \n");
    printf("5 : I can't find my Jerry. Just give me a similar one \n");
    printf("6 : I lost a bet. Give me your saddest Jerry \n");
    printf("7 : Show me what you got \n");
    printf("8 : Let the Jerries play \n");
    printf("9 : I had enough. Close this place \n");
}

status isPlanetExist(Planet **planets, int numberOfPlanets, char *planetName) {
    if (planets == NULL || numberOfPlanets == 0 || planetName == NULL) {
        return failure;
    }
    for (int i=0; i<numberOfPlanets; i++) {
        if (strcmp(planets[i]->name, planetName) == 0) {
            return success;
        }
    }
    return failure;
}


status addToDayCare(hashTable jerriesTable, Planet **planets, int numberOfPlanets, LinkedList listOfJerries) {
    char id[MAX_LINE_LENGTH];
    char planetName[MAX_LINE_LENGTH];
    char dimension[MAX_LINE_LENGTH];
    int happinessLevel;
    Planet* planetP;//for Jerry creation
    printf("What is your Jerry's ID ? \n");
    scanf("%s", id);
    if (lookupInHashTable(jerriesTable, id) != NULL) {
        printf("Rick did you forgot ? you already left him here ! \n");
        return failure;
    }
    printf("What planet is your Jerry from ? \n");
    scanf("%s", planetName);
    if (isPlanetExist(planets, numberOfPlanets, planetName) == failure) {
        printf("%s is not a known planet ! \n", planetName);
        return failure;
    }
    for (int i=0;i<numberOfPlanets;i++) {
        if (strcmp(planets[i]->name, planetName) == 0) {
            planetP = planets[i];
        }
    }
    printf("What is your Jerry's dimension ? \n");
    scanf("%s", dimension);
    printf("How happy is your Jerry now ? \n");
    scanf("%d", &happinessLevel);
    Jerry *jerry = create_jerry(id, happinessLevel, planetP, dimension);
    if (jerry == NULL) {
        return memoryFailure;
    }
    if (appendNode(listOfJerries, jerry) != success) {
        return memoryFailure;
    }
    if (addToHashTable(jerriesTable, (Element)jerry->id, (Element)jerry) != success) {
        return memoryFailure;
    }
    print_jerry(jerry);
    return success;
}


status addChar(hashTable jerriesTable, MultiValueHashTable characteristics) {
    char id[MAX_LINE_LENGTH];
    char character_name[MAX_LINE_LENGTH];
    double value;
    printf("What is your Jerry's ID ? \n");
    scanf("%s", id);
    Jerry *jerry = lookupInHashTable(jerriesTable, id);
    if (jerry == NULL) {
        printf("Rick this Jerry is not in the daycare ! \n");
        return failure;
    }
    printf("What physical can you add to Jerry - %s ? \n", id);
    scanf("%s", character_name);
    if (PhysicalCharacteristicExist(jerry, character_name) == true) {
        printf("The information about his %s already available to the daycare ! \n", character_name);
        return failure;
    }
    printf("What is the value of his %s ? \n", character_name);
    scanf("%lf", &value);
    PhysicalCharacteristics* physical_char = create_physical_characteristics(character_name,value);
    if (physical_char == NULL) {
        return memoryFailure;
    }
    if (add_physical_characteristic(jerry, physical_char) != success) {
        return memoryFailure;
    }
    if (addToMultiValueHashTable(characteristics, (Element)physical_char->name, (Element)jerry) != success) {
        return memoryFailure;
    }
    displayMultiValueHashTable(characteristics, (Element)character_name);
    return success;
}


status deleteChar(hashTable jerriesTable, MultiValueHashTable characteristics) {
    char id[MAX_LINE_LENGTH];
    char character_name[MAX_LINE_LENGTH];
    printf("What is your Jerry's ID ? \n");
    scanf("%s", id);
    Jerry *jerry = lookupInHashTable(jerriesTable, id);
    if (jerry == NULL) {
        printf("Rick this Jerry is not in the daycare ! \n");
        return failure;
    }
    printf("What physical characteristic do you want to remove from Jerry - %s ? \n", id);
    scanf("%s", character_name);
    if (PhysicalCharacteristicExist(jerry, character_name) == false) {
        printf("The information about his %s not available to the daycare ! \n", character_name);
        return failure;
    }
    if (removeFromMultiValueHashTable(characteristics, (Element)character_name, (Element)jerry) != success) {
        return memoryFailure;
    }
    if (delete_physical_characteristics(jerry, character_name) != success) {
        return memoryFailure;
    }
    //delete from the Jerries list?
    print_jerry(jerry);
    return success;
}


status deleteJerry(hashTable jerriesTable, MultiValueHashTable characteristics, LinkedList listOfJerries) {
    char id[MAX_LINE_LENGTH];
    printf("What is your Jerry's ID ? \n");
    scanf("%s", id);
    Jerry *jerry = lookupInHashTable(jerriesTable, id);
    if (jerry == NULL) {
        printf("Rick this Jerry is not in the daycare ! \n");
        return failure;
    }
    //delete the Jerry pointer from each characteristic list
    for(int i=0;i<jerry->characteristics_count;i++) {
        if (removeFromMultiValueHashTable(characteristics, (Element)jerry->characteristics[i]->name, (Element)jerry) != success) {
            return memoryFailure;
        }
    }
    if (deleteNode(listOfJerries, (Element)jerry) != success) { //Not a real deletion
        return memoryFailure;
    }
    if (removeFromHashTable(jerriesTable, (Element)jerry->id) != success) {
        return memoryFailure;
    }
    printf("Rick thank you for using out daycare servie ! Your Jerry awaits ! \n");
    return success;
}


void system_stats(hashTable jerriesTable, MultiValueHashTable characteristics, LinkedList listOfJerries, int numberOfPlanets, Planet** planets) {
    char input[MAX_LINE_LENGTH];
    printf("What information do you want to know ? \n");
    printf("1 : All Jerries \n");
    printf("2 : All Jerries by physical characteristics \n");
    printf("3 : All known planets \n");
    scanf("%s", input);
    if (strlen(input) > 1) {
        printf("Rick this option is not known to the daycare ! \n");
        return;
    }
    int inputNumber = atoi(input);
    switch (inputNumber) {
        case 1:
            if (getLengthList(listOfJerries) == 0) {
                printf("Rick we can not help you - we currently have no Jerries in the daycare ! \n");
            }else {
                displayList(listOfJerries);
            }
            return;
        case 2:
            char character_input[MAX_LINE_LENGTH];
            printf("What physical characteristics ? \n");
            scanf("%s", character_input);
            if (displayMultiValueHashTable(characteristics, (Element)character_input) != success) {
                printf("Rick we can not help you - we do not know any Jerry's %s ! \n", character_input);
            }
            return;
        case 3:
            for (int i = 0; i < numberOfPlanets; i++) {
                print_plant(planets[i]);
            }
            return;

        default: {
            printf("Rick this option is not known to the daycare ! \n");
        }
    }

}

int main (int argc, char *argv[]){

    int number_of_planets = atoi(argv[1]);
    char *input_file = argv[2];


    Planet **planets = malloc(number_of_planets * sizeof(Planet *));
    if (!planets) {
        perror("A memory problem has been detected in the program");
        return 0;
    }

    LinkedList JerriesList = createLinkedList(copy_jerry, fake_free_jerry, equal_jerry_key, is_equal_jerry, print_jerry_charTable);

    hashTable Jerries = createHashTable(deepCopy_str, freeStr, print_jerry_key, copy_jerry, free_jerry, print_jerry_value,
                                        equal_characteristic_key, hash_jerry, HASH_TABLE_SIZE);

    MultiValueHashTable characteristics = createMultiValueHashTable(deepCopy_str, copy_jerry, freeStr, fake_free_jerry, printStr,
                                                                    print_jerry_charTable, equal_characteristic_key,equal_jerry_key, hash_jerry, MULTI_VALUES_HASH_TABLE_SIZE);
    if(parse_config_file(input_file, number_of_planets, planets, Jerries, characteristics, JerriesList) == failure) {
        printf("A memory problem has been detected in the program\n");
        destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
    }

    int choice;
    do {
        printMenu();

        if (scanf("%d", &choice) != 1) {
            printf("Rick this option is not known to the daycare ! \n");

            // Clear invalid input from buffer
            while (getchar() != '\n') {
                choice = 0; // Reset choice to avoid processing invalid input
                continue;
            }
        }

        if (choice < 1 || choice > 9) {
            continue; // Re-show the menu for invalid choices
        }

        switch (choice) {
            case 1:
                if (addToDayCare(Jerries, planets, number_of_planets, JerriesList) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }
            break;
            case 2:
                if (addChar(Jerries, characteristics) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }
            break;
            case 3:
                if (deleteChar(Jerries, characteristics) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }
            break;
            case 4:
                if (deleteJerry(Jerries, characteristics, JerriesList) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                }
            break;
            case 5:

            break;
            case 6:

            break;
            case 7:
                system_stats(Jerries, characteristics, JerriesList, number_of_planets, planets);
            break;
            case 8:
                //printf("");
            break;
            case 9:
                //print_characteristic_key(Element Element);
            break;
        }
    } while (choice != 9);

    if (destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList) == success) {
        return 1;
    }

    return 0;
};
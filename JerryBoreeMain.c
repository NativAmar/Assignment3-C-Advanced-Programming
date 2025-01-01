#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "HashTable.h"
#include "MultiValueHashTable.h"
#include "Jerry.h"
#include "Defs.h"


#define MAX_LINE_LENGTH 300
#define HASH_TABLE_SIZE 11
#define MULTI_VALUES_HASH_TABLE_SIZE 17


// Helper function: Parse Planet
Planet* parse_planet(const char *line) {
    //Input validation
    if (line == NULL || strlen(line) == 0) {
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
    //Input validation
    if (line == NULL || strlen(line) == 0) {
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
    //check if one was found
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
    //Input validation
    if (input_file == NULL || planets == NULL || jerries == NULL || characteristics == NULL || jerries_list == NULL) {
        return argumentFailure;
    }
    FILE *file = fopen(input_file, "r");
    if (!file) {
        perror("Unable to open file");
        return failure;
    }

    char line[MAX_LINE_LENGTH];
    int planet_count = 0;
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

        if (section == 1) { //Planets section
            if (planet_count >= number_of_planets) {
                fclose(file);
                return failure;
            }
            planets[planet_count] = parse_planet(line);
            if (!planets[planet_count]) {
                fclose(file);
                return memoryFailure;
            }
            planet_count++;

        } else if (section == 2) { //Jerries section
            // Check if it's a characteristic or a new Jerry
            if (strchr(line, ':')) {
                if (!current_jerry) {
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
                    fclose(file);
                    return memoryFailure;
                }

                //add to the current Jerry
                status s = add_physical_characteristic(current_jerry, physical_char);

                if (s != success) {
                    fclose(file);
                    return memoryFailure;
                }
                //add the character name as a key to the characteristics multi-value hash table
                if (addToMultiValueHashTable(characteristics, (Element)physical_char->name, (Element)current_jerry) != success) {
                    fclose(file);
                    return memoryFailure;
                }

            } else { //Jerry details line
                current_jerry = parse_jerry(line, number_of_planets, planets);
                if(current_jerry == NULL) {
                    fclose(file);
                    return memoryFailure;
                }
                //add the current jerry to the hash table structure. the id is the key and the value is pointer to the Jerry structure
                if (addToHashTable(jerries, (Element)current_jerry->id, (Element)current_jerry)) {
                    fclose(file);
                    return failure;
                }

                if (appendNode(jerries_list, current_jerry) != success) {
                    fclose(file);
                    return failure;
                }
            }
        }
    }

    fclose(file);
    return success;
}



status print_jerry_key(Element element) {
    if (element == NULL) {
        return failure;
    }
    char *id = (char*) element;
    printf("Jerry , ID - %s ", id);
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

bool is_equal_jerry_wrapper(Element element1, Element element2) {
    return is_equal_jerry(element1, element2);
}

//made to avoid double free
status fake_free_jerry(Element element) {
    return success;
}

status destroy_all(hashTable hash_table, MultiValueHashTable multi_value_hash_table, Planet **planets, int numberOfPlanets, LinkedList jerries_list) {
    //Input validation
    if (hash_table == NULL || multi_value_hash_table == NULL || planets == NULL) {
        return failure;
    }
    //destroy every planet in the planets array
    for (int i=0; i<numberOfPlanets;i++) {
        destroy_planet(planets[i]);
    }
    free(planets);
    destroyHashTable(hash_table);
    destroyMultiValueHashTable(multi_value_hash_table);
    destroyList(jerries_list);
    return success;
}

void printMenu() {
    printf("Welcome Rick, what are your Jerry's needs today ? \n");
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

//Helper function to addToDayCare function
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

//The addToDayCare function prompts the user for Jerry's details, validates the inputs, creates a new Jerry, and adds it to both a linked list and hash table,
//returning success or failure based on the process outcome.
status addToDayCare(hashTable jerriesTable, Planet **planets, int numberOfPlanets, LinkedList listOfJerries) {
    char id[MAX_LINE_LENGTH];
    char planetName[MAX_LINE_LENGTH];
    char dimension[MAX_LINE_LENGTH];
    int happinessLevel;
    Planet* planetP;//for Jerry creation
    printf("What is your Jerry's ID ? \n");
    scanf("%s", id);
    if (lookupInHashTable(jerriesTable, id) != NULL) { //check if this Jerry already exists
        printf("Rick did you forgot ? you already left him here ! \n");
        return failure;
    }
    printf("What planet is your Jerry from ? \n");
    scanf("%s", planetName);
    if (isPlanetExist(planets, numberOfPlanets, planetName) == failure) { //check in the planets array for this planet input if exist
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
    Jerry *jerry = create_jerry(id, happinessLevel, planetP, dimension); //Create a new Jerry using the provided ID, happiness level, planet, and dimension
    if (jerry == NULL) {
        return memoryFailure;
    }
    if (appendNode(listOfJerries, jerry) != success) {
        return memoryFailure;
    }
    if (addToHashTable(jerriesTable, (Element)jerry->id, (Element)jerry) != success) {
        return memoryFailure;
    }
    print_jerry(jerry);//Print the details of the Jerry struct after successfully completing the process
    return success;
}

//The addChar function adds a new physical characteristic for a Jerry in the daycare.
status addChar(hashTable jerriesTable, MultiValueHashTable characteristics) {
    char id[MAX_LINE_LENGTH];
    char character_name[MAX_LINE_LENGTH];
    double value;
    printf("What is your Jerry's ID ? \n");
    scanf("%s", id);
    Jerry *jerry = lookupInHashTable(jerriesTable, id);
    if (jerry == NULL) {
        // Jerry not found in the daycare
        printf("Rick this Jerry is not in the daycare ! \n");
        return failure;
    }
    printf("What physical can you add to Jerry - %s ? \n", id);
    scanf("%s", character_name);
    // Check if the characteristic already exists for the Jerry
    if (PhysicalCharacteristicExist(jerry, character_name) == true) {
        printf("The information about his %s already available to the daycare ! \n", character_name);
        return failure;
    }
    printf("What is the value of his %s ? \n", character_name);
    scanf("%lf", &value);
    PhysicalCharacteristics* physical_char = create_physical_characteristics(character_name,value); // Create the physical characteristic
    if (physical_char == NULL) {
        return memoryFailure;
    }
    if (add_physical_characteristic(jerry, physical_char) != success) {
        return memoryFailure;
    }
    if (addToMultiValueHashTable(characteristics, (Element)physical_char->name, (Element)jerry) != success) {
        return memoryFailure;
    }
    displayMultiValueHashTable(characteristics, (Element)character_name); // Display the updated multi-value hash table for the characteristic
    return success;
}

//The deleteChar function allows a user to delete a physical characteristic from a Jerry in the daycare.
status deleteChar(hashTable jerriesTable, MultiValueHashTable characteristics) {
    char id[MAX_LINE_LENGTH];
    char character_name[MAX_LINE_LENGTH];
    printf("What is your Jerry's ID ? \n");
    scanf("%s", id);
    Jerry *jerry = lookupInHashTable(jerriesTable, id);
    if (jerry == NULL) {
        // Jerry not found in the daycare
        printf("Rick this Jerry is not in the daycare ! \n");
        return failure;
    }
    printf("What physical characteristic do you want to remove from Jerry - %s ? \n", id);
    scanf("%s", character_name);
    if (PhysicalCharacteristicExist(jerry, character_name) == false) { // Check if the characteristic exists for Jerry
        printf("The information about his %s not available to the daycare ! \n", character_name);
        return failure;
    }
    // Remove the characteristic from the multi-value hash table
    if (removeFromMultiValueHashTable(characteristics, (Element)character_name, (Element)jerry) != success) {
        return memoryFailure;
    }
    // Remove the physical characteristic from the Jerry characteristics array
    if (delete_physical_characteristics(jerry, character_name) != success) {
        return memoryFailure;
    }
    print_jerry(jerry);
    return success;
}


status deleteJerry(hashTable jerriesTable, MultiValueHashTable characteristics, LinkedList listOfJerries) {
    char id[MAX_LINE_LENGTH];
    printf("What is your Jerry's ID ? \n");
    scanf("%s", id);
    Jerry *jerry = lookupInHashTable(jerriesTable, id);
    if (jerry == NULL) {
        // Jerry not found in the daycare
        printf("Rick this Jerry is not in the daycare ! \n");
        return failure;
    }
    //delete the Jerry pointer from each characteristic list in the multi-value hash table
    for(int i=0;i<jerry->characteristics_count;i++) {
        if (removeFromMultiValueHashTable(characteristics, (Element)jerry->characteristics[i]->name, (Element)jerry) != success) {
            return memoryFailure;
        }
    }
    if (deleteNode(listOfJerries, (Element)jerry) != success) { // Remove the Jerry from the linked list of Jerries (not a real deletion, just removing the reference from the order of the insertion)
        return memoryFailure;
    }
    if (removeFromHashTable(jerriesTable, (Element)jerry->id) != success) { // Remove the Jerry from the hash table
        return memoryFailure;
    }
    //Print prompt to confirm the deletion
    printf("Rick thank you for using our daycare service ! Your Jerry awaits ! \n");
    return success;
}

//Helper function to the similarJerry function bellow
//This function return the value of a given characteristic for a specific Jerry. If the Jerry or the characteristic is not found, it returns -1.0 to indicate an error or absence.
double getCharacterValue(Jerry *jerry, char *character_name) {
    if (jerry == NULL || jerry->characteristics_count == 0) {
        return -1.0;
    }
    for (int i = 0; i < jerry->characteristics_count; i++) {
        if (strcmp(jerry->characteristics[i]->name, character_name) == 0) {
            return jerry->characteristics[i]->value;
        }
    }
    return -1.0;
}


status customJerryDeletion(Jerry *jerry, LinkedList listOfJerries, MultiValueHashTable characteristics, hashTable jerriesTable) {
    //Input validation
    if (jerry == NULL || listOfJerries == NULL || characteristics == NULL || jerriesTable == NULL) {
        return argumentFailure;
    }
    for(int i=0;i<jerry->characteristics_count;i++) {
        // Remove the Jerry from the multi-value hash table based on its characteristic name
        if (removeFromMultiValueHashTable(characteristics, (Element)jerry->characteristics[i]->name, (Element)jerry) != success) {
            return memoryFailure;
        }
    }
    if (deleteNode(listOfJerries, (Element)jerry) != success) { //Not a real deletion
        return memoryFailure;
    }
    // Remove the Jerry from the main hash table using its ID
    if (removeFromHashTable(jerriesTable, (Element)jerry->id) != success) {
        return memoryFailure;
    }
    // Print a confirmation message after the deletion is complete
    printf("Rick thank you for using our daycare service ! Your Jerry awaits ! \n");
    return success;
}

//This function finds and delete the most similar Jerry based on a given characteristic and value
status similarJerry(MultiValueHashTable characteristicsTable, hashTable jerriesTable, LinkedList jerriesInsertionList) {
    char character_name[MAX_LINE_LENGTH];
    double value, minDiff=101;
    Jerry *closestJerry=NULL, *currentJerry=NULL;
    printf("What do you remember about your Jerry ? \n");
    scanf("%s", character_name);
    LinkedList characteristicJerriesList = lookupInMultiValueHashTable(characteristicsTable, character_name);// Look up a list of Jerries that have this characteristic
    if (characteristicJerriesList == NULL) {
        printf("Rick we can not help you - we do not know any Jerry's %s ! \n", character_name);
        return failure;// Return failure if no Jerries are found with the given characteristic
    }
    printf("What do you remember about the value of his %s ? \n", character_name);
    scanf("%lf", &value);
    // Loop through the list of Jerries that share the same characteristic
    for (int i = 1;i<=getLengthList(characteristicJerriesList);i++) {
        currentJerry = (Jerry*)(getDataByIndex(characteristicJerriesList, i));
        // Calculate the absolute difference between the value and the Jerry's actual value for the characteristic
        double diff = fabs((getCharacterValue(currentJerry, character_name)) - value);
        // If this Jerry's characteristic value is closer to the input value, update the closestJerry
        if (diff < minDiff) {
            minDiff = diff;// Update the minimum difference
            closestJerry = currentJerry;
        }
    }
    printf("Rick this is the most suitable Jerry we found : \n");
    print_jerry(closestJerry);
    return customJerryDeletion(closestJerry, jerriesInsertionList, characteristicsTable, jerriesTable);// Delete the closest Jerry
}

//The saddestJerry function finds the Jerry with the lowest happiness value in the daycare, prints its details, and then deletes it from the system.
status saddestJerry(LinkedList listOfJerries, MultiValueHashTable characteristics, hashTable jerriesTable) {
    // Check if there are no Jerries in the daycare
    if (getLengthList(listOfJerries) == 0) {
        printf("Rick we can not help you - we currently have no Jerries in the daycare ! \n");
        return failure;
    }
    Jerry *saddest_jerry=NULL;
    int lowestHappinessValue=101;
    // Loop through all Jerries in the list to find the saddest one
    for (int i=1;i<=getLengthList(listOfJerries);i++) {
        int currentHappinessValue=getHappinessVal((Jerry*)getDataByIndex(listOfJerries, i));
        // If the current Jerry's happiness is lower than the lowest found so far, update the saddest Jerry
        if (currentHappinessValue < lowestHappinessValue) {
            saddest_jerry = (Jerry*)(getDataByIndex(listOfJerries, i));
            lowestHappinessValue = currentHappinessValue;// Update the lowest happiness value
        }
    }
    printf("Rick this is the most suitable Jerry we found : \n");
    print_jerry(saddest_jerry);
    return customJerryDeletion(saddest_jerry, listOfJerries, characteristics, jerriesTable);// Call the customJerryDeletion function to delete the saddest Jerry from the daycare
}

//The system_stats function provides information about the daycare system, allowing users to query the list of all Jerries, physical characteristics of Jerries, or a list of known planets based on their input.
//If the input is invalid, it informs the user.
void system_stats(hashTable jerriesTable, MultiValueHashTable characteristics, LinkedList listOfJerries, int numberOfPlanets, Planet** planets) {
    char input[MAX_LINE_LENGTH];
    printf("What information do you want to know ? \n");
    printf("1 : All Jerries \n");
    printf("2 : All Jerries by physical characteristics \n");
    printf("3 : All known planets \n");
    scanf("%s", input);
    if (strlen(input) > 1) { // Check if input length is more than 1 character, which means invalid option
        printf("Rick this option is not known to the daycare ! \n");
        return;
    }
    int inputNumber = atoi(input); // Convert input to integer for selection
    switch (inputNumber) {
        case 1: // Option 1: Display all Jerries in the daycare
            if (getLengthList(listOfJerries) == 0) {
                printf("Rick we can not help you - we currently have no Jerries in the daycare ! \n");
            }else {
                displayList(listOfJerries);
            }
            return;
        case 2: // Option 2: Display all Jerries by a specific physical characteristic
            char character_input[MAX_LINE_LENGTH];
            printf("What physical characteristics ? \n");
            scanf("%s", character_input);
            if (displayMultiValueHashTable(characteristics, (Element)character_input) != success) {
                printf("Rick we can not help you - we do not know any Jerry's %s ! \n", character_input);
            }
            return;
        case 3:// Option 3: Display all known planets
            for (int i = 0; i < numberOfPlanets; i++) {
                print_plant(planets[i]);
            }
            return;

        default: { // Handle invalid input
            printf("Rick this option is not known to the daycare ! \n");
        }
    }
}

//The jerriesGames function allows users to select an activity for the Jerries, adjusting their happiness levels based on the chosen activity
void jerriesGames(LinkedList listOfJerries) {
    // Check if there are any Jerries in the daycare
    if (getLengthList(listOfJerries) == 0) {
        printf("Rick we can not help you - we currently have no Jerries in the daycare ! \n");
        return;
    }
    char input[MAX_LINE_LENGTH];
    Jerry *currJerry = NULL;
    printf("What activity do you want the Jerries to partake in ? \n");
    printf("1 : Interact with fake Beth \n");
    printf("2 : Play golf \n");
    printf("3 : Adjust the picture settings on the TV \n");
    scanf("%s", input);
    if (strlen(input) > 1) {  // Validate the input length (must be a single digit)
        printf("Rick this option is not known to the daycare ! \n");
        return;
    }
    int inputNumber = atoi(input); // Convert input to integer for processing
    switch (inputNumber) {
        case 1:// Activity 1: Interact with fake Beth
            for (int i = 1; i <= getLengthList(listOfJerries); i++) {
                currJerry = (Jerry*)getDataByIndex(listOfJerries, i);
                if (currJerry->happiness_level >= 20) {
                    currJerry->happiness_level += 15;
                }
                else {
                    currJerry->happiness_level -= 5;
                }
            }
        break;
        case 2: // Activity 2: Play golf
            for (int i = 1; i <= getLengthList(listOfJerries); i++) {
                currJerry = (Jerry*)getDataByIndex(listOfJerries, i);
                if (currJerry->happiness_level >= 50) {
                    currJerry->happiness_level += 10;
                }
                else {
                    currJerry->happiness_level -= 10;
                }
            }
        break;
        case 3: // Activity 3: Adjust the picture settings on the TV
            for (int i = 1; i <= getLengthList(listOfJerries); i++) {
                currJerry = (Jerry*)getDataByIndex(listOfJerries, i);
                currJerry->happiness_level += 20;
            }
        break;
        default:
            printf("Rick this option is not known to the daycare ! \n");
        return;
    }
    // Ensure happiness level stays within 0 and 100
    for (int i = 1; i <= getLengthList(listOfJerries); i++) {
        currJerry = (Jerry*)getDataByIndex(listOfJerries, i);
        if (currJerry->happiness_level > 100) {
            currJerry->happiness_level = 100;
        }
        if (currJerry->happiness_level < 0) {
            currJerry->happiness_level = 0;
        }
    }
    printf("The activity is now over ! \n");
    displayList(listOfJerries); // Display the updated list of Jerries
    return;
}



int main (int argc, char *argv[]) {

    int number_of_planets = atoi(argv[1]);
    char *input_file = argv[2];


    Planet **planets = malloc(number_of_planets * sizeof(Planet *));
    if (!planets) {
        perror("A memory problem has been detected in the program");
        return 0;
    }
    // Create the Jerries list, hash table, and multi-value hash table
    LinkedList JerriesList = createLinkedList(copy_jerry, fake_free_jerry, equal_jerry_key, is_equal_jerry_wrapper, print_jerry_charTable);

    hashTable Jerries = createHashTable(deepCopy_str, freeStr, print_jerry_key, copy_jerry, free_jerry, print_jerry_value,
                                        equal_characteristic_key, hash_jerry, HASH_TABLE_SIZE);

    MultiValueHashTable characteristics = createMultiValueHashTable(deepCopy_str, copy_jerry, freeStr, fake_free_jerry, printStr,
                                                                    print_jerry_charTable, equal_characteristic_key,equal_jerry_key, hash_jerry, MULTI_VALUES_HASH_TABLE_SIZE);
    // Parse the configuration file
    if(parse_config_file(input_file, number_of_planets, planets, Jerries, characteristics, JerriesList) != success) {
        printf("A memory problem has been detected in the program\n");
        destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
    }

    int choice; // Variable to store the user's menu choice
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
            case 1: // Add a new Jerry to the daycare
                if (addToDayCare(Jerries, planets, number_of_planets, JerriesList) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }
            break;
            case 2: // Add a physical characteristic to a Jerry
                if (addChar(Jerries, characteristics) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }
            break;
            case 3: // Delete a physical characteristic from a Jerry
                if (deleteChar(Jerries, characteristics) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }
            break;
            case 4: // Delete a Jerry from the daycare
                if (deleteJerry(Jerries, characteristics, JerriesList) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }
            break;
            case 5: // Find and delete the most similar Jerry based on physical characteristics
                if (similarJerry(characteristics, Jerries, JerriesList) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }

            break;
            case 6: // Find and delete the saddest Jerry
                if (saddestJerry(JerriesList, characteristics, Jerries) == memoryFailure) {
                    destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList);
                    return 1;
                }
            break;
            case 7: // Show system statistics
                system_stats(Jerries, characteristics, JerriesList, number_of_planets, planets);
            break;
            case 8: // Run activities for the Jerries
                jerriesGames(JerriesList);
            break;
            case 9: // Exit the program and close the daycare
                printf("The daycare is now clean and close ! \n");
            break;
        }
    } while (choice != 9);

    if (destroy_all(Jerries, characteristics, planets, number_of_planets, JerriesList) == success) {  // Clean up and free all allocated memory before exiting
        return 1;
    }

    return 0; // Exit the program successfully
};
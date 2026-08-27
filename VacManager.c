/** iaed25 - ist1114298 - project 
 * Program for a System managing vaccines, inocculations and users
 * @file: VacManager2025
 * @author: ist1114298 (Guilherme Esteves) 
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define BUFMAX 65536 /**< maximum size of input line */ 
#define MAXNAME 50 /**< maximum size of vaccine name*/
#define MAXBATCH 20 /**< maximum size of a batch number*/
#define MAXVACCINES 1000 /**< maximum size of batches in the system */

#define E2MANYVAC "too many vaccines" /** excede limite de vacinas */
#define EDUPBATCH "duplicate batch number" /** lote duplicado */
#define EINVBATCH "invalid batch" /** lote invalido */
#define EINVNAME "invalid name" /** nome invalido */
#define EINVDATE "invalid date" /** data invalida */
#define EINVQUANTITY "invalid quantity" /** quantidade invalida */
#define ENOSUCHVAC "no such vaccine" /** não existe essavacina */
#define ENOSTOCK "no stock" /** sem stock */
#define EALREADYVAC "already vaccinated" /** já vacinado */
#define ENOSUCHBATCH "no such batch" /** não existe esse lote */
#define ENOSUCHUSER "no such user" /** não existe esse utente */
#define ENOMEMORY "No memory." /** sem memoria */

#define E2MANYVACPT "demasiadas vacinas" /**< excedes vaccine limit */
#define EDUPBATCHPT "número de lote duplicado" /**< duplicate batch */
#define EINVBATCHPT "lote inválido" /**< invalid batch */
#define EINVNAMEPT "nome inválido" /**< invalid name */
#define EINVDATEPT "data inválida" /**< invalid name */
#define EINVQUANTITYPT "quantidade inválida" /**< invalid quantity*/
#define ENOSUCHVACPT "vacina inexistente" /**< there is no such vaccine*/
#define ENOSTOCKPT "esgotado" /**< no stock */
#define EALREADYVACPT "já vacinado" /**< already vaccinated */
#define ENOSUCHBATCHPT "lote inexistente" /**< no such batch*/
#define ENOSUCHUSERPT "utente inexistente" /**< no such user */ 
#define ENOMEMORYPT "sem memória" /**< no memory */

/**  Struct for a date */
typedef struct {
    int day, month, year; /**< date */
} Date; 

/** Struct for a batch */
typedef struct {
    char name[MAXNAME + 1]; /**< vaccine name */
    char batch[MAXBATCH + 1]; /**< batch number */
    Date expiry; /**< expiry date of the batch */
    int doses; /**< number of doses in the batch */
    int applications; /**< number of aplications of the batch */
} Batch; 

/* Struct for an Inoculation */
typedef struct {
    char *user_name;  /**< user name */
    char batch[MAXBATCH + 1]; /**< batch number */
    Date application_date; /**< date of the inoculation*/
} Inoculation;

/* Struct for the system and registers info */
typedef struct {
    Batch batches[MAXVACCINES]; /**< list of batches in the system */
    int batch_count;  /**< number of batches in the system */

    Inoculation *inoculations; /**< list of inoculations in the sys */
    int inoculation_count; /**< number of inoculations in the sys */
    int inoculation_capacity; /**< inocularions capacity */

    Date current_date; /**< current date of the system */
    int language; /**< 0 for English, 1 for Portuguese */
} Sys; 

/** Gets an error depending on the language choosen by the user 
 * @param sys pointer to the system
 * @param english error message in english
 * @param portuguese error message in portuguese
 * @return 1 if te language is portuguese, 0 if english
*/
static const char* get_error(Sys *sys, const char *english, const char *portuguese) {
    return (sys->language == 1) ? portuguese : english;
}

/** Initializes the system and keeps track of information
 * @param sys pointer to the system
*/
static void initialize_system(Sys *sys) { 
    sys->batch_count = 0; 
    sys->inoculation_count = 0;
    sys->inoculation_capacity = 10;
    sys->inoculations = malloc(sys->inoculation_capacity * sizeof(Inoculation));
    if (!sys->inoculations) {
        puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
        exit(1);
    }
    sys->current_date.day = 1;
    sys->current_date.month = 1;
    sys->current_date.year = 2025;
}

/** Gets the number of days in a certain month 
 * @param month month of the date
 * @param year year of the date
*/
static int days_in_month(int month, int year) {
    (void)year;
    switch (month) {
        case 4: case 6: case 9: case 11: return 30; 
        case 2: return 28; 
        default: return 31; 
    }
}

/** Checks if a date is valid 
 * @param day day of the date
 * @param month month of the date
 * @param year year of the date
 * @return 1 if the date is valid, 0 otherwise
*/
static int is_valid_date(int day, int month, int year) {
    if (month < 1 || month > 12) return 0; 
    if (day < 1 || day > days_in_month(month, year)) return 0; 
    return 1;
}

/** Compares two dates
 * @param d1 first date
 * @param d2 second date
 * @return -1 if d1 is before d2, 1 if d1 is after d2, 0 if they are equal
*/
static int compare_dates(Date d1, Date d2) {
    if (d1.year != d2.year) return (d1.year < d2.year) ? -1 : 1;
    if (d1.month != d2.month) return (d1.month < d2.month) ? -1 : 1;
    if (d1.day != d2.day) return (d1.day < d2.day) ? -1 : 1;
    return 0;
}

/** Searches for a batch in the Sys 
 * @param sys pointer to thesystem
 * @param batch pointer to the batch to search for
 * @return the index of the batch in the Sys, -1 if it does not exist
*/
static int search_batch(Sys *sys, char *batch) {
    for (int i = 0; i < sys->batch_count; i++)
        if (!strcmp(batch, sys->batches[i].batch)) /* If batch does exist in the Sys */
            return i; /* Returns the batch index */
    return -1; /* If it does not exist returns -1 */
}

/** Prints the batch information 
 * @param batch batch to print
*/
static void print_batch(Batch batch) { 
    printf("%s %s %02d-%02d-%04d %d %d\n", 
           batch.name, batch.batch, 
           batch.expiry.day, batch.expiry.month, batch.expiry.year, 
           batch.doses, batch.applications);
}

/** Checks if a name is valid for a batch 
 * @param batch pointer to the batch to check
 * @return 1 if the name is valid, 0 otherwise
*/
static int is_valid_batch_name(const char *batch) {
    for (int i = 0; batch[i] != '\0'; i++) {
        if (!isdigit(batch[i]) && (batch[i] < 'A' || batch[i] > 'F')) {
            return 0;  
        }
    }
    return 1; 
}

/** Adds a batch to the Sys 
 * @param sys pointer to the Sys
 * @param info pointer to the input info to the batch
*/
static void add_batch(Sys *sys, char *info) {
    char batch[MAXBATCH + 1], name[MAXNAME + 2];
    int day, month, year, doses;

    if (sys->batch_count >= MAXVACCINES) {
        puts(get_error(sys, E2MANYVAC, E2MANYVACPT));
        return;
    }
    if (sscanf(info, "%*s %20s %d-%d-%d %d %51s",
                     batch, &day, &month, &year, &doses, name) != 6) {
        puts(get_error(sys, EINVBATCH, EINVBATCHPT));
        return;
    }
    if (!is_valid_batch_name(batch)) {  
        puts(get_error(sys, EINVBATCH, EINVBATCHPT));
        return;
    }
    if (search_batch(sys, batch) >= 0) { /* Because search_bartch returns -1 if it already exists*/
        puts(get_error(sys, EDUPBATCH, EDUPBATCHPT));
        return;
    }
    if (!is_valid_date(day, month, year) || 
        compare_dates((Date){day, month, year}, sys->current_date) < 0) {
        puts(get_error(sys, EINVDATE, EINVDATEPT)); /* compare_dates returns -1 if the d1 is before d2*/
        return;
    }
    if (doses <= 0) {
        puts(get_error(sys, EINVQUANTITY, EINVQUANTITYPT));
        return;
    }
    if (strlen(name) > MAXNAME) {  
        puts(get_error(sys, EINVNAME, EINVNAMEPT));
        return;
    }
    Batch *new_batch = &sys->batches[sys->batch_count++];
    snprintf(new_batch->batch, MAXBATCH + 1, "%s", batch);
    snprintf(new_batch->name, MAXNAME + 1, "%s", name);

    new_batch->expiry.day = day;
    new_batch->expiry.month = month;
    new_batch->expiry.year = year;
    new_batch->doses = doses;
    new_batch->applications = 0;
    printf("%s\n", batch);
}

/** List all the batches or the ones from the given vaccines 
 * @param sys system
 * @param info input string with the command and arguments
*/
static void list_batches(Sys *sys, char *info) {
    char *token = strtok(info + 1, " \t\n");  /* Ignores the 'l' command and safes info in token */

    for (int i = 0; i < sys->batch_count - 1; i++) { /* Bubble sort */
        for (int j = 0; j < sys->batch_count - i - 1; j++) { 
            int cmp = compare_dates(sys->batches[j].expiry, sys->batches[j + 1].expiry);
            /* -1 if d1 is before d2, 0 if they are equal, 1 if d1 is after d2 */

            if (cmp > 0 || (cmp == 0 && strcmp(sys->batches[j].batch, sys->batches[j + 1].batch) > 0)) {
                Batch temp = sys->batches[j]; 
                sys->batches[j] = sys->batches[j + 1];
                sys->batches[j + 1] = temp; /* If j is after j+1, swap the two */
            }
        }
    }
    if (!token) {/* If no arguments, print all sorted batches */
        for (int i = 0; i < sys->batch_count; i++) {
            print_batch(sys->batches[i]);
        }
        return;
    }
    while (token) {  /* Process given vaccine names */
        int found = 0;

        for (int i = 0; i < sys->batch_count; i++) {
            if (strcmp(sys->batches[i].name, token) == 0) {
                print_batch(sys->batches[i]);
                found = 1; /* Searches for the batches in the token*/
            }
        }
        if (!found) /* If the batches selected are not found, prints an error message */
            printf("%s: %s\n", token, get_error(sys, ENOSUCHVAC, ENOSUCHVACPT));
        token = strtok(NULL, " \t\n");
    }
}

/** Searches if a user is already vaccinated with a certain vaccine
 * @param sys system
 * @param user user to search for
 * @param vaccine vaccine to search for
 * @return the batch index of the user's vaccination or -1 if not found
*/
static int search_user(Sys *sys, char *user, char *vaccine) {
    for (int i = 0; i < sys->inoculation_count; i++) {
        int batch_index = search_batch(sys, sys->inoculations[i].batch);
        if (batch_index != -1 &&
            strcmp(sys->batches[batch_index].name, vaccine) == 0 &&
            strcmp(sys->inoculations[i].user_name, user) == 0 &&
            compare_dates(sys->inoculations[i].application_date, sys->current_date) == 0) {
            return i; /* User was already vaccinated today with this vaccine */
        }
    }
    return -1; /* The user was not already vaccinated today with this vaccine */
}

/** Finds the oldest valid batch of a given vaccine 
 * @param sys system
 * @param vaccine vaccine to search for
 * @return the index of the oldest valid batch of the vaccine or -1 if there's none
*/
static int find_oldest_valid_batch(Sys *sys, char *vaccine) {
    int oldest_index = -1;

    for (int i = 0; i < sys->batch_count; i++) {
        Batch *batch = &sys->batches[i];
        if (strcmp(batch->name, vaccine) == 0 && batch->doses > 0) {
            /* Verifies if the batch expiry date is after or equal to current date */
            if (compare_dates(batch->expiry, sys->current_date) >= 0) {
                if (oldest_index == -1 || compare_dates(batch->expiry, sys->batches[oldest_index].expiry) < 0) {
                    oldest_index = i;
                }
            }
        }
    }
    return oldest_index; /* Returns the index of the oldest valid batch or -1 if there's none */
}

/** Applies a vaccine to a user 
 * @param sys system
 * @param info info to apply the inocculation to a user
*/
static void apply_vaccine(Sys *sys, char *info) {
    char *user_name = NULL;
    char vaccine[MAXNAME + 1];
    
    if (strchr(info, '"') != NULL) { /* Check if there are quotes */
        char *start_quote = strchr(info, '"');
        char *end_quote = strchr(start_quote + 1, '"');
        if (!end_quote) {
            puts(get_error(sys, EINVNAME, EINVNAMEPT));
            return;
        }
        
        size_t name_length = end_quote - (start_quote + 1);
        user_name = malloc(name_length + 1);
        if (!user_name) {
            puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
            exit(1);
        }

        strncpy(user_name, start_quote + 1, name_length);
        user_name[name_length] = '\0';
        
        end_quote++;
        while (*end_quote == ' ' || *end_quote == '\t') end_quote++;
        
        int i = 0; /* Copy the vaccine name */
        while (end_quote[i] && end_quote[i] != ' ' && end_quote[i] != '\n' && end_quote[i] != '\r' && i < MAXNAME) {
            vaccine[i] = end_quote[i];
            i++;
        }
        vaccine[i] = '\0';
    } 
    else { /* No quotes */
        char temp_name[BUFMAX];
        if (sscanf(info, "%*c %s %50s", temp_name, vaccine) != 2) {
            return;
        }
        user_name = malloc(strlen(temp_name) + 1);
        if (!user_name) {
            puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
            exit(1);
        }
        strcpy(user_name, temp_name);
    }

    int batch_index = find_oldest_valid_batch(sys, vaccine);
    if (batch_index == -1) {
        free(user_name);
        puts(get_error(sys, ENOSTOCK, ENOSTOCKPT));
        return;
    }
    if (search_user(sys, user_name, vaccine) != -1) {
        free(user_name);
        puts(get_error(sys, EALREADYVAC, EALREADYVACPT));
        return;
    }
    Batch *batch = &sys->batches[batch_index];
    batch->doses--;
    batch->applications++;
    
    if (sys->inoculation_count >= sys->inoculation_capacity) {
        sys->inoculation_capacity *= 2;
        Inoculation *new_array = realloc(sys->inoculations, sys->inoculation_capacity * sizeof(Inoculation));
        if (!new_array) {
            free(user_name);
            puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
            exit(1);
        }
        sys->inoculations = new_array;
    }
    
    Inoculation *new_inoculation = &sys->inoculations[sys->inoculation_count++];
    new_inoculation->user_name = user_name;
    strncpy(new_inoculation->batch, batch->batch, MAXBATCH);
    new_inoculation->batch[MAXBATCH] = '\0';
    new_inoculation->application_date = sys->current_date;
    
    printf("%s\n", batch->batch);
}


/** Removes a batch from the system or stes doses to 0 if there are inoculations 
 * @param sys the system
 * @param info the instruction for the batch removal
*/
static void remove_batch(Sys *sys, char *info) {
    char batch_id[MAXBATCH + 1];

    sscanf(info, "%*s %20s", batch_id);

    int index = search_batch(sys, batch_id);
    if (index == -1) {
        printf("%s: %s\n", batch_id, get_error(sys, ENOSUCHBATCH, ENOSUCHBATCHPT));
        return;
    }
    /* If batch has applications, just set doses to 0 and prints the number of applications */
    if (sys->batches[index].applications > 0) {
        printf("%d\n", sys->batches[index].applications);
        sys->batches[index].doses = 0; 
        return;
    }
    /* If batch has no applications, removes the batch and moves the elementes to the left */
    for (int i = index; i < sys->batch_count - 1; i++) {
        sys->batches[i] = sys->batches[i + 1];
    }
    sys->batch_count--;
    printf("0\n");
}

/** Deletes one or many inoculations from the system 
 * @param sys the system
 * @param info the instruction for the inoculation deletion
*/
static void delete_inoculation(Sys *sys, char *info) {
    char *user_name = NULL;
    char batch[MAXBATCH + 1] = {0};
    int day = -1, month = -1, year = -1;
    int num_deleted = 0;
    Date target_date = {-1, -1, -1};
    
    if (strchr(info, '"') != NULL) {
        char *start_quote = strchr(info, '"');
        char *end_quote = strchr(start_quote + 1, '"');
        if (!end_quote) {
            puts(get_error(sys, EINVNAME, EINVNAMEPT));
            return;
        }
        size_t name_length = end_quote - (start_quote + 1);
        user_name = malloc(name_length + 1);
        if (!user_name) {
            puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
            exit(1);
        }
        strncpy(user_name, start_quote + 1, name_length);
        user_name[name_length] = '\0';
        
        sscanf(end_quote + 1, "%d-%d-%d %20s", &day, &month, &year, batch);
    } 
    else {
        char temp_name[BUFMAX];
        int num_args = sscanf(info, "%*s %s %d-%d-%d %20s", temp_name, &day, &month, &year, batch);
        if (num_args < 1) return;
        
        user_name = malloc(strlen(temp_name) + 1);
        if (!user_name) {
            puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
            exit(1);
        }
        strcpy(user_name, temp_name);
    }
    
    if (day != -1) { /* If date is provided, validates it*/
        target_date = (Date){day, month, year};
        if (!is_valid_date(day, month, year) || compare_dates(target_date, sys->current_date) > 0) {
            free(user_name);
            puts(get_error(sys, EINVDATE, EINVDATEPT));
            return;
        }
    }
    if (batch[0] != '\0' && search_batch(sys, batch) == -1) { /* If batch is provided, validates it*/
        free(user_name);
        printf("%s: %s\n", batch, get_error(sys, ENOSUCHBATCH, ENOSUCHBATCHPT));
        return;
    }
    
    for (int i = 0; i < sys->inoculation_count; i++) { /* Delete matching inoculations */
        Inoculation *vac = &sys->inoculations[i];
        
        if (strcmp(vac->user_name, user_name) == 0 && 
            (day == -1 || compare_dates(vac->application_date, target_date) == 0) &&
            (batch[0] == '\0' || strcmp(vac->batch, batch) == 0)) {
            free(vac->user_name);
            
            for (int j = i; j < sys->inoculation_count - 1; j++) {
                sys->inoculations[j] = sys->inoculations[j + 1];
            }
            
            sys->inoculation_count--;
            num_deleted++;
            i--;
        }
    }
    if (num_deleted == 0) {
        printf("%s: %s\n", user_name, get_error(sys, ENOSUCHUSER, ENOSUCHUSERPT));
    } 
    else {
        printf("%d\n", num_deleted);
    }
    free(user_name);
}

/** Lists all inoculations in the system 
 * @param sys the system
 * @param info the info for which inocculations to list
*/
static void list_inoculations(Sys *sys, char *info) {
    char *user_name = NULL;
    int has_user = 0;
    int found = 0;

    if (strchr(info, '"') != NULL) {
        char *start_quote = strchr(info, '"');
        char *end_quote = strchr(start_quote + 1, '"');
        if (!end_quote) {
            puts(get_error(sys, EINVNAME, EINVNAMEPT));
            return;
        }
        size_t name_length = end_quote - (start_quote + 1);
        user_name = malloc(name_length + 1);
        if (!user_name) {
            puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
            exit(1);
        }

        strncpy(user_name, start_quote + 1, name_length);
        user_name[name_length] = '\0';
        has_user = 1;
    } 
    else {
        char temp_name[BUFMAX];
        if (sscanf(info, "%*s %200s", temp_name) == 1) {
            user_name = malloc(strlen(temp_name) + 1);
            if (!user_name) {
                puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
                exit(1);
            }
            strcpy(user_name, temp_name);
            has_user = 1;
        }
    }
    for (int i = 0; i < sys->inoculation_count - 1; i++) { /* Bubble Sort */
        for (int j = 0; j < sys->inoculation_count - i - 1; j++) {
            if (compare_dates(sys->inoculations[j].application_date, 
                             sys->inoculations[j + 1].application_date) > 0) {
                Inoculation temp = sys->inoculations[j];
                sys->inoculations[j] = sys->inoculations[j + 1];
                sys->inoculations[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < sys->inoculation_count; i++) {
        Inoculation *vac = &sys->inoculations[i];
        if (!has_user || strcmp(vac->user_name, user_name) == 0) {
            printf("%s %s %02d-%02d-%04d\n",   /* If no user specified or user matches */
                   vac->user_name, vac->batch,
                   vac->application_date.day, 
                   vac->application_date.month, 
                   vac->application_date.year);
            found = 1;
        }
    }

    if (has_user && !found) { /* If user was specified but not found */
        printf("%s: %s\n", user_name, get_error(sys, ENOSUCHUSER, ENOSUCHUSERPT));
    }
    if (user_name) {
        free(user_name);
    }
}

/** Tells the system date or advances it to the given date 
 * @param sys the system
 * @param info the info for which date to list or advance to
*/
static void advance_time(Sys *sys, char *info) {
    int day, month, year;
    int num_args = sscanf(info, "%*s %d-%d-%d", &day, &month, &year);

    if (num_args != 3) { /* If no arguments, print current date */
        printf("%02d-%02d-%04d\n", sys->current_date.day, sys->current_date.month, sys->current_date.year);
        return;
    }
    if (!is_valid_date(day, month, year) || compare_dates((Date){day, month, year}, sys->current_date) < 0) {
        puts(get_error(sys, EINVDATE, EINVDATEPT)); 
        return;
    }
    sys->current_date = (Date){day, month, year}; 
    printf("%02d-%02d-%04d\n", sys->current_date.day, sys->current_date.month, sys->current_date.year);
} 

/** Frees the system memory 
 * @param sys the system
*/
static void free_system(Sys *sys) {
    if (sys->inoculations) {
        for (int i = 0; i < sys->inoculation_count; i++) {
            free(sys->inoculations[i].user_name);
        }
        free(sys->inoculations);
        sys->inoculations = NULL;
    }
}

/** Vaccine Management System 
 * @param argc the number of arguments
 * @param argv the arguments for the language
 * @return 0 always
*/
int main(int argc, char *argv[]) {
    char buf[BUFMAX];
    Sys sys = {0};  

    initialize_system(&sys);
    sys.language = (argc > 1 && strcmp(argv[1], "pt") == 0) ? 1 : 0;

    while (fgets(buf, BUFMAX, stdin)) {
        switch (buf[0]) {
            case 'q': free_system(&sys); return 0;
            case 'c': add_batch(&sys, buf); break;
            case 'l': list_batches(&sys, buf); break;
            case 'a': apply_vaccine(&sys, buf); break;
            case 'r': remove_batch(&sys, buf); break;
            case 'd': delete_inoculation(&sys, buf); break;
            case 'u': list_inoculations(&sys, buf); break;
            case 't': advance_time(&sys, buf); break;
        }
    }
    free_system(&sys);
    return 0;
} 
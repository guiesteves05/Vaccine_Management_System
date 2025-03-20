/* iaed25 - ist1114298 - project */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define BUFMAX 65536
#define MAXNAME 50
#define MAXBATCH 20
#define MAXVACCINES 1000

#define E2MANYVAC "too many vaccines"
#define EDUPBATCH "duplicate batch number"
#define EINVBATCH "invalid batch"
#define EINVNAME "invalid name"
#define EINVDATE "invalid date"
#define EINVQUANTITY "invalid quantity"
#define ENOSUCHVAC "no such vaccine"
#define ENOSTOCK "no stock"
#define EALREADYVAC "already vaccinated"
#define ENOSUCHBATCH "no such batch"
#define ENOSUCHUSER "no such user"
#define ENOMEMORY "No memory."

#define E2MANYVACPT "demasiadas vacinas"
#define EDUPBATCHPT "número de lote duplicado"
#define EINVBATCHPT "lote inválido"
#define EINVNAMEPT "nome inválido"
#define EINVDATEPT "data inválida"
#define EINVQUANTITYPT "quantidade inválida"
#define ENOSUCHVACPT "vacina inexistente"
#define ENOSTOCKPT "esgotado"
#define EALREADYVACPT "já vacinado"
#define ENOSUCHBATCHPT "lote inexistente"
#define ENOSUCHUSERPT "utente inexistente"
#define ENOMEMORYPT "sem memória"

typedef struct {
    int day, month, year;
} Date; /* Struct for a date */

typedef struct {
    char name[MAXNAME + 1];
    char batch[MAXBATCH + 1];
    Date expiry;
    int doses;
    int applications;
} Batch; /* Struct for a batch */

typedef struct {
    char user_name[201]; 
    char batch[MAXBATCH + 1];
    Date application_date;
} Inoculation; /* Struct for an Inoculation */

typedef struct {
    Batch batches[MAXVACCINES];      
    int batch_count;      

    Inoculation *inoculations;
    int inoculation_count;
    int inoculation_capacity;

    Date current_date;
    int language; /* 0 for English, 1 for Portuguese */
} Sys; /* Struct for the system and save info */

static const char* get_error(Sys *sys, const char *english, const char *portuguese) {
    return (sys->language == 1) ? portuguese : english;
}

static void initialize_system(Sys *sys) {
    sys->batch_count = 0; /* Initializes the system and keeps track of information*/
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

static int days_in_month(int month, int year) {
    (void)year;
    switch (month) {
        case 4: case 6: case 9: case 11: return 30; 
        case 2: return 28; 
        default: return 31; 
    }
}

static int is_valid_date(int day, int month, int year) {
    if (month < 1 || month > 12) return 0; 
    if (day < 1 || day > days_in_month(month, year)) return 0; 
    return 1;
}

static int compare_dates(Date d1, Date d2) {
    if (d1.year != d2.year) return (d1.year < d2.year) ? -1 : 1;
    if (d1.month != d2.month) return (d1.month < d2.month) ? -1 : 1;
    if (d1.day != d2.day) return (d1.day < d2.day) ? -1 : 1;
    return 0;
}


static int search_batch(Sys *sys, char *batch) {
    for (int i = 0; i < sys->batch_count; i++)
        if (!strcmp(batch, sys->batches[i].batch)) /* If batch does not exist in the Sys */
            return i; /* Returns the batch index */
    return -1; /* If it does not exist returns -1 */
}

static void print_batch(Batch batch) { /* Prints  the batch information */
    printf("%s %s %02d-%02d-%04d %d %d\n", 
           batch.name, batch.batch, 
           batch.expiry.day, batch.expiry.month, batch.expiry.year, 
           batch.doses, batch.applications);
}

static int is_valid_batch_name(const char *batch) {
    for (int i = 0; batch[i] != '\0'; i++) {
        if (!isdigit(batch[i]) && (batch[i] < 'A' || batch[i] > 'F')) {
            return 0;  
        }
    }
    return 1; 
}

static char* extract_quoted_name(char *input, char *output, int max_len) {
    char *start = strchr(input, '"'); 
    if (!start) return NULL; 

    start++; 
    char *end = strchr(start, '"'); 
    if (!end) return NULL; 

    int len = end - start; 
    if (len >= max_len) len = max_len - 1; 

    strncpy(output, start, len);
    output[len] = '\0'; 
    return end + 1; 
}

static void add_batch(Sys *sys, char *info) {
    char batch[MAXBATCH + 1], name[MAXNAME + 2];
    int day, month, year, doses;

    if (sys->batch_count >= MAXVACCINES) {
        puts(get_error(sys, E2MANYVAC, E2MANYVACPT));
        return;
    }
    if (sscanf(info, "%*s %20s %d-%d-%d %d %51s", batch, &day, &month, &year, &doses, name) != 6) {
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
    if (!is_valid_date(day, month, year) || compare_dates((Date){day, month, year}, sys->current_date) < 0) {
        puts(get_error(sys, EINVDATE, EINVDATEPT));
    return;
    }
    if (doses <= 0) {
        puts(get_error(sys, EINVQUANTITY, EINVQUANTITYPT));
        return;
    }
    if (strlen(name) > MAXNAME) {  // Validate name length here!
        puts(get_error(sys, EINVNAME, EINVNAMEPT));
        return;
    }

    Batch *new_batch = &sys->batches[sys->batch_count++];
    strncpy(new_batch->batch, batch, MAXBATCH);
    new_batch->batch[MAXBATCH] = '\0';  

    strncpy(new_batch->name, name, MAXNAME);
    new_batch->name[MAXNAME] = '\0';

    new_batch->expiry.day = day;
    new_batch->expiry.month = month;
    new_batch->expiry.year = year;
    new_batch->doses = doses;
    new_batch->applications = 0;
    printf("%s\n", batch);
}

static void list_batches(Sys *sys, char *info) {
    char *token = strtok(info + 1, " \t\n");  /* Remove the 'l' and split arguments */
    for (int i = 0; i < sys->batch_count - 1; i++) { /*  Sort batches first */
        for (int j = 0; j < sys->batch_count - i - 1; j++) {
            int cmp = compare_dates(sys->batches[j].expiry, sys->batches[j + 1].expiry);
            if (cmp > 0 || (cmp == 0 && strcmp(sys->batches[j].batch, sys->batches[j + 1].batch) > 0)) {
                Batch temp = sys->batches[j];
                sys->batches[j] = sys->batches[j + 1];
                sys->batches[j + 1] = temp;
            }
        }
    }
    if (!token) {/*  If no arguments, print all sorted batches */
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
                found = 1;
            }
        }
        if (!found)
            printf("%s: %s\n", token, get_error(sys, ENOSUCHVAC, ENOSUCHVACPT));
        token = strtok(NULL, " \t\n");
    }
}

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

static int find_oldest_valid_batch(Sys *sys, char *vaccine) {
    int oldest_index = -1;

    for (int i = 0; i < sys->batch_count; i++) {
        Batch *batch = &sys->batches[i];

        /* Verifies if the batch is of the specified vaccine and has available doses */
        if (strcmp(batch->name, vaccine) == 0 && batch->doses > 0) {
            /* Verifies if the batch is still valid */
            if (compare_dates(batch->expiry, sys->current_date) >= 0) {
                /* Chooses the oldest valid batch available */
                if (oldest_index == -1 || compare_dates(batch->expiry, sys->batches[oldest_index].expiry) < 0) {
                    oldest_index = i;
                }
            }
        }
    }
    return oldest_index; /* Returns the index of the oldest valid batch or -1 if there's none */
}


static void apply_vaccine(Sys *sys, char *info) {
    char user_name[201], vaccine[MAXNAME + 1];
    // Check if we have a quoted name
    if (strchr(info, '"') != NULL) {
        // Extract the quoted name
        char *end_quote = extract_quoted_name(info, user_name, 201);
        if (!end_quote) {
            puts(get_error(sys, EINVNAME, EINVNAMEPT));
            return;
        }
        // Find the vaccine name after the quoted name
        // Skip spaces after the closing quote
        while (*end_quote == ' ' || *end_quote == '\t') end_quote++;
        
        // Copy the vaccine name (everything until end of line or next space)
        int i = 0;
        while (end_quote[i] && end_quote[i] != ' ' && end_quote[i] != '\n' && end_quote[i] != '\r' && i < MAXNAME) {
            vaccine[i] = end_quote[i];
            i++;
        }
        vaccine[i] = '\0';
    } else {
        // No quotes, simple space-separated format
        if (sscanf(info, "%*c %200s %50s", user_name, vaccine) != 2) {
            return; // Not enough arguments
        }
    }
    //printf("DEBUG: User='%s', Vaccine='%s'\n", user_name, vaccine);
    
    int batch_index = find_oldest_valid_batch(sys, vaccine);
    if (batch_index == -1) {
        puts(get_error(sys, ENOSTOCK, ENOSTOCKPT));
        return;
    }

    if (search_user(sys, user_name, vaccine) != -1) {
        puts(get_error(sys, EALREADYVAC, EALREADYVACPT));
        return;
    }
    /* Updates the batch */
    Batch *batch = &sys->batches[batch_index];
    batch->doses--;
    batch->applications++;
    /* Expands the array if necessary */
    if (sys->inoculation_count >= sys->inoculation_capacity) {
        sys->inoculation_capacity *= 2;
        Inoculation *new_array = realloc(sys->inoculations, sys->inoculation_capacity * sizeof(Inoculation));
        if (!new_array) {
            puts(get_error(sys, ENOMEMORY, ENOMEMORYPT));
            exit(1);
        }
        sys->inoculations = new_array;
    }
    /* Registers the inoculation */
    Inoculation *new_inoculation = &sys->inoculations[sys->inoculation_count++];

    strncpy(new_inoculation->user_name, user_name, 200);
    new_inoculation->user_name[200] = '\0';  

    strncpy(new_inoculation->batch, batch->batch, MAXBATCH);
    new_inoculation->batch[MAXBATCH] = '\0';  
    
    new_inoculation->application_date = sys->current_date;
    printf("%s\n", batch->batch);
}



static void remove_batch(Sys *sys, char *info) {
    char batch_id[MAXBATCH + 1];

    sscanf(info, "%*s %20s", batch_id);

    int index = search_batch(sys, batch_id);
    if (index == -1) {
        printf("%s: %s\n", batch_id, get_error(sys, ENOSUCHBATCH, ENOSUCHBATCHPT));
        return;
    }

    /* If batch has applications, just set doses to 0 */
    if (sys->batches[index].applications > 0) {
        printf("%d\n", sys->batches[index].applications);
        sys->batches[index].doses = 0; 
        return;
    }
    /* IF batch has no applications, removes the batch and moves the elementes to the left */
    for (int i = index; i < sys->batch_count - 1; i++) {
        sys->batches[i] = sys->batches[i + 1];
    }
    sys->batch_count--;
    printf("0\n"); 
}


static void delete_inoculation(Sys *sys, char *info) {
    char user_name[201], batch[MAXBATCH + 1];
    int day = -1, month = -1, year = -1;
    int num_deleted = 0;
    Date target_date = {-1, -1, -1}; 

    int num_args = sscanf(info, "%*s %200s %d-%d-%d %20s", user_name, &day, &month, &year, batch);

    if (num_args >= 3) {
        target_date = (Date){day, month, year};
        if (!is_valid_date(day, month, year) || compare_dates(target_date, sys->current_date) > 0) {
            puts(get_error(sys, EINVDATE, EINVDATEPT));
            return; /* If date provided */
        }
    }
    if (num_args == 5 && search_batch(sys, batch) == -1) {
        printf("%s: %s\n", batch, get_error(sys, ENOSUCHBATCH, ENOSUCHBATCHPT));
        return; /* If batch provided */
    }
    for (int i = 0; i < sys->inoculation_count; i++) {
        Inoculation *vac = &sys->inoculations[i];

        if (strcmp(vac->user_name, user_name) == 0 &&
            (num_args < 3 || compare_dates(vac->application_date, target_date) == 0) &&
            (num_args < 5 || strcmp(vac->batch, batch) == 0)) {
            
            int batch_index = search_batch(sys, vac->batch);
            if (batch_index != -1) {
                sys->batches[batch_index].applications--;
            } /* Update application count */

            for (int j = i; j < sys->inoculation_count - 1; j++) {
                sys->inoculations[j] = sys->inoculations[j + 1];
            } /* Remove the inoculation from the array */
            sys->inoculation_count--;
            num_deleted++;
            i--; 
        }
    }
    if (num_deleted == 0) {
        printf("%s: %s\n", user_name, get_error(sys, ENOSUCHUSER, ENOSUCHUSERPT));
    } else {
        printf("%d\n", num_deleted);
    }
}

static void list_inoculations(Sys *sys, char *info) {
    char user_name[201];
    int has_user = sscanf(info, "%*s %200s", user_name);
    int found = 0;
    /* Sort inoculations by date */
    for (int i = 0; i < sys->inoculation_count - 1; i++) { /* Bubble Sort */
        for (int j = 0; j < sys->inoculation_count - i - 1; j++) {
            if (compare_dates(sys->inoculations[j].application_date, sys->inoculations[j + 1].application_date) > 0) {
                Inoculation temp = sys->inoculations[j];
                sys->inoculations[j] = sys->inoculations[j + 1];
                sys->inoculations[j + 1] = temp;
            }
        }
    }
    for (int i = 0; i < sys->inoculation_count; i++) {
        Inoculation *vac = &sys->inoculations[i];
        /*If theres is no user or the user is the one we want */
        if (has_user != 1 || strcmp(vac->user_name, user_name) == 0) {
            printf("%s %s %02d-%02d-%04d\n",
                   vac->user_name, vac->batch,
                   vac->application_date.day, vac->application_date.month, vac->application_date.year);
            found = 1;
        }
    }
    if (has_user == 1 && !found) { /* If user was specified but not found, print error */
        printf("%s: %s\n", user_name, get_error(sys, ENOSUCHUSER, ENOSUCHUSERPT));
    }
}

static void advance_time(Sys *sys, char *info) {
    int day, month, year;
    int num_args = sscanf(info, "%*s %d-%d-%d", &day, &month, &year);

    if (num_args != 3) { /* If no arguments, print current date */
        printf("%02d-%02d-%04d\n", sys->current_date.day, sys->current_date.month, sys->current_date.year);
        return;
    }

    if (!is_valid_date(day, month, year) || compare_dates((Date){day, month, year}, sys->current_date) < 0) {
        puts(get_error(sys, EINVDATE, EINVDATEPT)); /* Validate the new date */
        return;
    }
    sys->current_date = (Date){day, month, year}; /* Update the system date */
    printf("%02d-%02d-%04d\n", sys->current_date.day, sys->current_date.month, sys->current_date.year);
}



static void free_system(Sys *sys) {
    if (sys->inoculations) {
        free(sys->inoculations);
        sys->inoculations = NULL; 
    }
}

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

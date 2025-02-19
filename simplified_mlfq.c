#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define MAX_NUM_PROCESS
#define MAX_NUM_PROCESS 10

#define MAX_PROCESS_NAME 5
#define MAX_GANTT_CHART 300

// N-level Feedback Queue (N=1,2,3,4)
#define MAX_NUM_QUEUE 4

// Keywords (to be used when parsing the input)
#define KEYWORD_QUEUE_NUMBER "queue_num"
#define KEYWORD_TQ "time_quantum"
#define KEYWORD_PROCESS_TABLE_SIZE "process_table_size"
#define KEYWORD_PROCESS_TABLE "process_table"

// Assume that we only need to support 2 types of space characters: 
// " " (space), "\t" (tab)
#define SPACE_CHARS " \t"

// Process data structure
// Helper functions:
//  process_init: initialize a process entry
//  process_table_print: Display the process table
struct Process {
    char name[MAX_PROCESS_NAME];
    int arrival_time ;
    int burst_time;
    int remain_time; // remain_time is needed in the intermediate steps of MLFQ 
};

void process_init(struct Process* p, char name[MAX_PROCESS_NAME], int arrival_time, int burst_time) {
    strcpy(p->name, name);
    p->arrival_time = arrival_time;
    p->burst_time = burst_time;
    p->remain_time = 0;
}

void process_table_print(struct Process* p, int size) {
    int i;
    printf("Process\tArrival\tBurst\n");
    for (i = 0; i < size; i++) {
        printf("%s\t%d\t%d\n", p[i].name, p[i].arrival_time, p[i].burst_time);
    }
}

// A simple GanttChart structure
// Helper functions:
//   gantt_chart_print: display the current chart
struct GanttChartItem {
    char name[MAX_PROCESS_NAME];
    int duration;
};

void gantt_chart_print(struct GanttChartItem chart[MAX_GANTT_CHART], int n) {
    int t = 0;
    int i = 0;
    printf("Gantt Chart = ");
    printf("%d ", t);
    for (i=0; i<n; i++) {
        t = t + chart[i].duration;     
        printf("%s %d ", chart[i].name, t);
    }
    printf("\n");
}

// Global variables
int queue_num = 0;
int process_table_size = 0;
struct Process process_table[MAX_NUM_PROCESS];
int time_quantum[MAX_NUM_QUEUE];


// Helper function: Check whether the line is a blank line (for input parsing)
int is_blank(char *line) {
    char *ch = line;
    while ( *ch != '\0' ) {
        if ( !isspace(*ch) )
            return 0;
        ch++;
    }
    return 1;
}

// Helper function: Check whether the input line should be skipped
int is_skip(char *line) {
    if ( is_blank(line) )
        return 1;
    char *ch = line ;
    while ( *ch != '\0' ) {
        if ( !isspace(*ch) && *ch == '#')
            return 1;
        ch++;
    }
    return 0;
}

// Helper: parse_tokens function
void parse_tokens(char **argv, char *line, int *numTokens, char *delimiter) {
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}

// Helper: parse the input file
void parse_input() {
    FILE *fp = stdin;
    char *line = NULL;
    ssize_t nread;
    size_t len = 0;

    char *two_tokens[2]; // buffer for 2 tokens
    char *queue_tokens[MAX_NUM_QUEUE]; // buffer for MAX_NUM_QUEUE tokens
    int n;

    int numTokens = 0, i = 0;
    char equal_plus_spaces_delimiters[5] = "";

    char process_name[MAX_PROCESS_NAME];
    int process_arrival_time = 0;
    int process_burst_time = 0;

    strcpy(equal_plus_spaces_delimiters, "=");
    strcat(equal_plus_spaces_delimiters,SPACE_CHARS);    

    // Note: MingGW don't have getline, so you are forced to do the coding in Linux/POSIX supported OS
    // In other words, you cannot easily coding in Windows environment

    while ( (nread = getline(&line, &len, fp)) != -1 ) {
        if ( is_skip(line) == 0)  {
            line = strtok(line,"\n");

            if (strstr(line, KEYWORD_QUEUE_NUMBER)) {
                // parse queue_num
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &queue_num);
                }
            } 
            else if (strstr(line, KEYWORD_TQ)) {
                // parse time_quantum
                parse_tokens(two_tokens, line, &numTokens, "=");
                if (numTokens == 2) {
                    // parse the second part using SPACE_CHARS
                    parse_tokens(queue_tokens, two_tokens[1], &n, SPACE_CHARS);
                    for (i = 0; i < n; i++)
                    {
                        sscanf(queue_tokens[i], "%d", &time_quantum[i]);
                    }
                }
            }
            else if (strstr(line, KEYWORD_PROCESS_TABLE_SIZE)) {
                // parse process_table_size
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &process_table_size);
                }
            } 
            else if (strstr(line, KEYWORD_PROCESS_TABLE)) {

                // parse process_table
                for (i=0; i<process_table_size; i++) {

                    getline(&line, &len, fp);
                    line = strtok(line,"\n");  

                    sscanf(line, "%s %d %d", process_name, &process_arrival_time, &process_burst_time);
                    process_init(&process_table[i], process_name, process_arrival_time, process_burst_time);

                }
            }

        }
        
    }
}

// Helper: Display the parsed values
void print_parsed_values() {
    printf("%s = %d\n", KEYWORD_QUEUE_NUMBER, queue_num);
    printf("%s = ", KEYWORD_TQ);
    for (int i=0; i<queue_num; i++)
        printf("%d ", time_quantum[i]);
    printf("\n");
    printf("%s = \n", KEYWORD_PROCESS_TABLE);
    process_table_print(process_table, process_table_size);
}

struct FeedbackQueue {
    int queue_num;
    int num_processes;
    char* processes_in_queue[MAX_NUM_PROCESS];
};

struct Process* get_process_pointer(char* process_name) {
    for (int i = 0; i < process_table_size; ++i) {
        if (strcmp(process_table[i].name, process_name) == 0) {
            return &process_table[i];
        }
    }
    return NULL;
}

// Helper function: To remove the process from MLFQ
void remove_completed_process(struct FeedbackQueue* fq) {
    for (int i = 0; i < MAX_NUM_PROCESS - 1; ++i) {
        fq->processes_in_queue[i] = fq->processes_in_queue[i + 1];
    }
    fq->num_processes--;
}

// Helper function: To insert the process into MLFQ
void insert_process_into_queue(int queue_num, char* process_name, struct FeedbackQueue feedback_queues[]) {
    for (int i = 0; i < process_table_size; ++i) {
        if (process_table[i].name == process_name) {
            int num_processes_in_queue = feedback_queues[queue_num].num_processes;
            int time_quantum_of_queue = time_quantum[queue_num];
            feedback_queues[queue_num].processes_in_queue[num_processes_in_queue] = process_table[i].name;
            feedback_queues[queue_num].num_processes++;
            process_table[i].remain_time = time_quantum_of_queue;
            return;
        }
    }
}

// return 1 when there is incoming process, return 0 when there is no
int check_next_process(int time, int* process_table_pointer) {
    if (*process_table_pointer == process_table_size) {
        return 0;
    }

    if (time == process_table[*process_table_pointer].arrival_time) {
        (*process_table_pointer)++;
        return 1;
    } else {
        return 0;
    }
}

void mlfq() {

    struct GanttChartItem chart[MAX_GANTT_CHART];
    int sz_chart = 0;

    int remaining_processes = process_table_size;
    int time = 0;
    int process_table_pointer = 0;

    struct FeedbackQueue feedback_queues[queue_num];
    for (int i = 0; i < queue_num; ++i) {
        feedback_queues[i].queue_num = i;
        feedback_queues[i].num_processes = 0;
    }
    
    insert_process_into_queue(0, process_table[0].name, feedback_queues);
    process_table_pointer++;
    
    int i = 0;
    while (remaining_processes) {
        int reset_i = 0;
        if (feedback_queues[i].num_processes) {
            reset_i = 1;
            char* process_name = feedback_queues[i].processes_in_queue[0];
            struct Process* process_executing = get_process_pointer(process_name);

            int executing_time = 0;

            // Execute process
            while (process_executing->remain_time > 0 && process_executing->burst_time > 0) {
                if (check_next_process(time, &process_table_pointer) == 1) {
                    insert_process_into_queue(0, process_table[process_table_pointer - 1].name, feedback_queues);
                    if (i != 0) {
                        break;
                    }
                }
                time++;
                executing_time++;
                process_executing->burst_time--;
                process_executing->remain_time--;
            }
            if (check_next_process(time, &process_table_pointer) == 1) {
                insert_process_into_queue(0, process_table[process_table_pointer - 1].name, feedback_queues);
            }

            // Record in Gantt chart
            if (sz_chart != 0 && strcmp(process_executing->name, chart[sz_chart - 1].name) == 0) {
                chart[sz_chart - 1].duration += executing_time;
            } else {
                strcpy(chart[sz_chart].name, process_executing->name);
                chart[sz_chart].duration = executing_time;
                sz_chart++;
            }

            if (process_executing->burst_time == 0) {
                remove_completed_process(&feedback_queues[i]);
                remaining_processes--;
            } else if (process_executing->remain_time == 0) {
                remove_completed_process(&feedback_queues[i]);
                insert_process_into_queue(i + 1, process_executing->name, feedback_queues);
            }
        }
        if (reset_i == 1) {
            i = 0;
        } else {
            i++;
        }
    }
    
    // At the end, display the final Gantt chart
    gantt_chart_print(chart, sz_chart);
}


int main() {
    parse_input();
    print_parsed_values();
    mlfq();
    return 0;
}
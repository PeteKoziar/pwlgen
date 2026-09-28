/****************************************************************************
 *                                                                          *
 * File    : main.c                                                         *
 *                                                                          *
 * Purpose : Console mode (command line) program.                           *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/****************************************************************************
 *                                                                          *
 * Function: main                                                           *
 *                                                                          *
 * Purpose : Main entry point.                                              *
 *                                                                          *
 * History : Date      Reason                                               *
 *           00/00/00  Created                                              *
 *                                                                          *
 ****************************************************************************/
#define MAX_SIGNALS 	64
#define MAX_EDGES   	64
#define MAX_NAME_LENGTH 64
#define DEFAULT_CLOCK	50		// Clock period in nanoseconds
#define DEFAULT_RISE	 5		// Rise time in nanoseconds
#define DEFAULT_FALL     5		// Default fall time in nanoseconds

#define DEBUG

char *signal_names[MAX_SIGNALS];
char signal_matrix[MAX_EDGES][MAX_SIGNALS];
static void HelpMessage(void);

int main(int argc, char *argv[])
{
	FILE *in_file, *out_file;
	int incoming;
	char *in_file_name  = NULL;
	char *out_file_name = NULL;
	char name[MAX_NAME_LENGTH+1];		// Leave room for the null at the end.
	int name_count    = 0;
	int signal_count  = 0;
	int edge_count    = 0;
	int comment_flag  = 0;
	int name_flag     = 1;
	int signal_index  = 0;
	int edge_index    = 0;
	int clock_width   = DEFAULT_CLOCK;
	int rise_time     = DEFAULT_RISE;
	int fall_time     = DEFAULT_FALL;
	bool invert_clock = false;
	int time          = 0;
	int current_level = 0;
	float vdd         = 1.8;
	int last_state    = 0;
	int current_state = 0;

	// Parse the input arguments:
	if(argc < 3) {
		HelpMessage();
		return 0;
	} else if(argc == 3) {
		in_file_name = argv[1];
		out_file_name = argv[2];
	} else {
		for(int index = 1; index != argc; index++) {
			char *arg = argv[index];
			if(*arg == '-') {
				if(index == argc - 1) {
					printf("** ERROR: Missing value for option!\n");
					return 1;
				}
				switch(arg[1]) {
					case 'v': vdd         = atof(argv[index+1]); index++; break;
					case 'c': clock_width = atoi(argv[index+1]); index++; break;
					case 'r': rise_time   = atoi(argv[index+1]); index++; break;
					case 'f': fall_time   = atoi(argv[index+1]); index++; break;
					case 'i': invert_clock= true;                         break;
					case 'h': HelpMessage(); return 0;
					default:  printf("** ERROR: Bad option\n"); return 1;
				}
				if((vdd == 0.) || (clock_width == 0)) {
					printf("** ERROR: Bad option value\n");
					return 1;
				}
			} else {
				if(in_file_name == NULL) in_file_name = arg;
				else if(out_file_name == NULL) out_file_name = arg;
				else { printf("** ERROR: Too many files specified!\n"); return 1; }
			}
		}
	}


    // Open input file in read mode
    in_file = fopen(in_file_name, "r");
    if (in_file == NULL) {
        perror("Error opening input file"); // Prints system error message
        return 1;
    }

	// Open output file in write mode:
	out_file = fopen(out_file_name, "w");
	if (out_file == NULL) {
		perror("Error opening output file");
		return 1;
	}

     // The first line that's not a comment is the signal names:
    while ((incoming = fgetc(in_file)) != EOF) {

		if((incoming == '\r') || (incoming == ' '))
			continue;

		// Ignore comments:
		if(incoming == '#') {
			comment_flag = 1;
			continue;
		}

		if(comment_flag == 1) {
			if(incoming == '\n')
				comment_flag = 0;
			continue;
		}

		// Collect the signal names, comma delimited:
		// TODO: signal names terminated by a comment don't work.
		if(name_flag == 1) {
			if((incoming == ',') || (incoming == '\n') || (incoming == '#')){
				if(signal_count < MAX_SIGNALS) {
					name[name_count++] = '\0';
					char *new_signal = malloc(name_count);
					strcpy(new_signal, name);
					signal_names[signal_count++] = new_signal;
				}

				name_count = 0;
				
				if(incoming == '\n') {
					name_flag = 0;

#ifdef DEBUG
					printf("Signal names:\n");
					for(int index = 0; index != signal_count; index++)
						printf("-- %s\n", signal_names[index]);
					printf("\n");
#endif
				}
			} else {
				if(name_count < MAX_NAME_LENGTH)
					name[name_count++] = (char) incoming;
			}
		} else {

			// Not the first line or a comment line. Extract the signals themselves:
			if(incoming == '\n') {
				if(edge_count < MAX_EDGES)
					edge_count++;
				signal_index = 0;
			} else {
				if(incoming == '0')
					signal_matrix[edge_count][signal_index] = 0;
				else if(incoming == '1')
					signal_matrix[edge_count][signal_index] = 1;
				else {
					printf("\n*** Bad character in signal list!\n");
					exit(1);
				}
				if(signal_index < signal_count)
					signal_index++;
			}
		}

    }

    // Check for reading errors
    if (ferror(in_file)) {
        fprintf(stderr, "\n*** Error reading file.\n");
        fclose(in_file);
        return 1;
    }

    // Close file
    if (fclose(in_file) != 0) {
        fprintf(stderr, "*** Error closing file.\n");
        return 1;
    }

    printf("\n-- File read\n");
	if(edge_count < 2) {
		printf("\n*** Error: not enough values.\n");
		exit(1);
	}

	fprintf(out_file, "* Digital signals for %s\n", in_file_name);
	fprintf(out_file, "* Clock width = %dns, rise time = %dns, fall time = %dns\n", clock_width, rise_time, fall_time);

	// Output the clock definition: Vname N+ N- PULSE(Vo V1 Td Tr Tf Tw To)
	fprintf(out_file, "V1 clk 0 PULSE(0 %4.2f %dn %dn %dn %dn %dn)\n", vdd, invert_clock? clock_width / 2 : 0, rise_time, fall_time, clock_width / 2 - rise_time, clock_width);

	// Output the PWL format:
	for(signal_index = 0; signal_index != signal_count; signal_index++) {
		fprintf(out_file, "V%d %s 0 PWL(", signal_index+2, signal_names[signal_index]);
		time = clock_width;
		last_state = signal_matrix[0][signal_index];
		fprintf(out_file, "0n %4.2f", last_state ? vdd:0.);

		// If the signal has changed, generate a rise or fall edge:
		for(edge_index = 1; edge_index != edge_count; edge_index++) {
			current_state = signal_matrix[edge_index][signal_index];
			if(current_state != last_state) {
				fprintf(out_file, " %dn %4.2f", time, last_state ? vdd:0);
				fprintf(out_file, " %dn %4.2f", time + (current_state ? rise_time : fall_time), current_state ? vdd: 0);
				last_state = current_state;
			}
			time += clock_width;
		}
		fprintf(out_file, ")\n");

	}
	fprintf(out_file, ".END\n");
	fclose(out_file);
    return 0;
}

static void HelpMessage(void)
{
		printf("Usage:\n");
		printf("  pwlgen infile outfile  [options]\n\n");
		printf("  Options:\n");
		printf("    -v high-level      (float) Sets the high level Voltage\n");
		printf("    -c clock-ns        (int  ) Sets the clock period in ns\n");
		printf("    -r rise-ns         (int  ) Sets the data rise time in ns\n");
		printf("    -f fall-ns         (int  ) Sets the data fall time in ns\n");
		printf("    -i                         Inverts the clock edge\n");
	    printf("    -h                         Displays this message\n");
		printf("\n");
}

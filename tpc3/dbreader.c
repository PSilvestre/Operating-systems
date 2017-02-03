#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define LINESIZE 50

struct student {
	char first_name[40];
	char last_name[40];
	int number;
	char course[10];
	float average;
};

typedef struct student student_info;


int main(int argc, char* argv[]) {

	if (argc != 2) {
		printf("usage %s db_file\n", argv[0]);
		return 0;
	}


	// Load student database
	int fd;
	struct stat info;
	void* db;

	fd = open(argv[1], O_RDONLY, 0);
	if(fd < 0) //error opening file
		return 0;
	if(fstat(fd, &info) != 0) //error reading information with fstat
		return 0;
	db = mmap(NULL, info.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	if(db == MAP_FAILED) //error while mapping
		return 0;

	int numStudents = info.st_size / sizeof(student_info);
	student_info* currentStudent;

	char* command;
	char* argument;
	char line[LINESIZE];

	printf("> ");
	fflush(stdout);
	while (fgets(line, LINESIZE, stdin) != NULL) {
		command = strtok(line, " \t\n");
		if (strcmp(command, "course") == 0) {
			argument = strtok(NULL, " \t\n");
			if(strcmp(argument, "") == 0) // no argument.
				return 0;
			for(int i = 0; i < numStudents; i++){
				currentStudent = (student_info*)(db + i*sizeof(student_info));
				if(strcmp(currentStudent->course, argument) == 0){
					printf("%s %s\n", currentStudent->first_name, currentStudent->last_name);
				}
			}
		}
		else if (strcmp(command, "list") == 0) {
			for(int i = 0; i < numStudents; i++){
				currentStudent = (student_info*)(db + i*sizeof(student_info));
				printf("Name: %s %s\nNumber: %d\nCourse: %s\nAverage: %.2f\n\n", currentStudent->first_name, currentStudent->last_name,
				 currentStudent->number, currentStudent->course, currentStudent->average);
			}
		}
		else if (strcmp(command, "exit") == 0) {
			return 0;
		}
		else
			printf ("Invalid command.\n");

		printf("> ");
		fflush(stdout);
	}
	close(fd);
	return 0;

}

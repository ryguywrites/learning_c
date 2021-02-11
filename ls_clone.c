#include "ls_clone.h" // header file
#include <stdlib.h> // standard lib, contains malloc
#include <stdio.h> // input output 
#include <unistd.h> // POSIX operating system api
#include <limits.h> // constants for properties of variable types
#include <dirent.h> // directory 
#include <sys/stat.h> // file statistics
#include <string.h> // string functions
#include <fnmatch.h> // glob pattern matching

/**
Assumptions made/requirements:
	if nonflag arguments are given: 
		the first must be the directory
		the second must be the glob pattern wrapped around  with quotation marks
			ex: ./ls_clone -a . "*.c"

Flags implemented: a, l, and s

Note that the l flag is not very human readable. (sorry for cutting corners!)
This version of ls also prints each file in its own line and does not sort the files.
**/
int main(int argc, char **argv) {
	// initialize flags to 0
	ls_flags flags = {
		.a=0, 
		.l=0, 
		.s=0
	};
	ls_flags *p_flags = &flags;

	// allocate heap memory for buffers
	char *directory_path = (char *)malloc(sizeof(char)*(MAX_DIRECTORY_CHAR_LENGTH+1));
	char *file_type = (char *)malloc(sizeof(char)*(MAX_FILE_NAME_LENGTH+1));

	// update variables/flags based on user parameters
	process_arguments(argc, argv, p_flags, directory_path, file_type); 

	// opens the directory, gets the metadata of files, and prints contents
	print_directory(directory_path, file_type, flags);

	// free malloc pointers
	free(directory_path);
	free(file_type);
}

int print_directory(char *directory_path, char *file_type, ls_flags flags){
	// open the directory 
	DIR *p_dir_to_read = opendir(directory_path);
	if (p_dir_to_read == NULL) {
		printf("Please enter a valid directory.\n");
		return 1;
	}

	// declare vars to get data regarding files in directory
	struct dirent *p_dir_struct; 
	struct stat file_stats; 

	// append filename to the end of the directory path
	int directory_path_length = strlen(directory_path);
	char *file_path = directory_path;
	file_path[directory_path_length] = '/';
	char *end_of_file_path = file_path + directory_path_length + 1;

	// bool to manage printf color
	int is_black_font = 1;

	// loop through files and print relevant data
	while ((p_dir_struct = readdir(p_dir_to_read))){
		// skip hidden files depending on flag
		if (!flags.a) {
			if ((*p_dir_struct).d_name[0] == '.'){
				continue;
			}
		}

		// skip current file if the pattern doesn't match 
		if(fnmatch(file_type, (*p_dir_struct).d_name, 0)){
			continue; 
		}

		// update the filepath with the filename
		strcpy(end_of_file_path, (*p_dir_struct).d_name);
		stat(file_path, &file_stats); 

		// change color based on file type
		if (S_ISDIR(file_stats.st_mode)) { // check if its a directory
			printf(CYN);
			is_black_font = 0;
		} 
		else if (file_stats.st_mode & S_IXUSR){ // check if executable
			printf(RED);
			is_black_font = 0;
		}

		// print metadata for file if applicable
		if (flags.s) {
			printf("%lld ", file_stats.st_blocks); // show size in blocks
		} else if (flags.l) {
			printf("%hu ", file_stats.st_mode); // show permissions
			printf("%hu ", file_stats.st_nlink); // show num links
			printf("%u ", file_stats.st_uid); // show owner of file
			printf("%u ", file_stats.st_gid); // show group file belongs to
			printf("%lld ", file_stats.st_blocks); // show size in blocks
			printf("%ld ", file_stats.st_mtime); // show modified date
		}

		// print file name
		printf("%s\n", (*p_dir_struct).d_name);

		if (!is_black_font) { // should probably change this to a switch statement on the file type
			printf(RESET);
			is_black_font = 1;
		} 

	}
	// close opened directory
	closedir(p_dir_to_read);

	return 0;
}

/**
Responsible for setting the state of our ls_flags struct, 
										 directory path, and 
										 any glob based file filtering
Returns 0 on success, 1 otherwise
**/
int process_arguments(int argc, char **argv, ls_flags *p_flags, char *dir_path, char *file_type) {
	int flag;
	// looping logic allows the flags to be before or after the folderpath argument
	while (optind < argc) { // loop through cli arguments 
		while((flag = getopt(argc, argv, AVAILABLE_FLAGS)) != -1){ // mark flags
			switch(flag) {
				case 'a':
					(*p_flags).a = 1;
					break;
				case 'l':
					(*p_flags).l = 1;
					break;
				case 's':
					(*p_flags).s = 1;
					break;
				default:
					printf("Invalid flag. Please check your arguments.\n");
					return 1;
			}
		}
		if (optind < argc) { // non-flag arguments
			if (!strlen(dir_path)) { // we assume first non-flag arg is the directory
				strcpy(dir_path, argv[optind]);
			} 
			else if(dir_path && !strlen(file_type)) { // second non-flag arg is the pattern
				strcpy(file_type, argv[optind]);
			}else {
				printf("Incorrect syntax. Please check your arguments.\n");
				return 1; 
			}
			optind++;
		}
		
	}

	// 0 non-flag arguments passed, default to current directory
	if (strlen(dir_path)==0) {
		getcwd(dir_path, PATH_MAX+1);
	}

	// default file type is everything if user doesn't specify anything
	if (!strlen(file_type)) {
		(file_type)[0] = '*';
		(file_type)[1] = '\0';
	}

	return 0;
}
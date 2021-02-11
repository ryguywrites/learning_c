// define limits
#define MAX_FILES_IN_DIR 512
#define MAX_DIRECTORY_CHAR_LENGTH 1000
#define MAX_FILE_NAME_LENGTH 100

// available flags
#define AVAILABLE_FLAGS "als"

// colors for printf
#define RED   "\x1B[31m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

typedef struct {
	int a;
	int l;
	int s;
} ls_flags;

int print_directory(char *directory_path, char *file_type, ls_flags flags);
int process_arguments(int argc, char **argv, ls_flags *p_flags, char *directory_path, char *file_type);
int main(int argc, char **argv);
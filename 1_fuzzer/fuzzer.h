char * fuzzer (int max_length, int char_start, int char_range) ;

int write_fuzzer_data (char * path) ;

void read_fuzzer_data (char * path, int data_size) ;

void create_input_files (char * dir_name, char * path) ;

void remove_input_files (char * dir_name, char * path) ;

void fuzzer_with_files () ;
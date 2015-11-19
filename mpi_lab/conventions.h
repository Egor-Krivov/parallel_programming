#define ISIZE 3000
#define JSIZE 2000
#define MUL 0.00001

// Type of calculation. If horizontal, column number is more frequently changed and named internal.
enum calculation_type {HORIZONTAL, VERTICAL};

enum calculation_type get_calculation_type();

int get_parallelism();

int get_internal_limit();
int get_external_limit();

double *calculate_thread(int thread);

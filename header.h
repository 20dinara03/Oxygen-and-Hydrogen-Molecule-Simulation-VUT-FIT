
typedef struct
{
    int oxy_number;
    int hydro_number;
    int TI;
    int TB;
} args_t;

typedef struct
{
    int counter;
    int queue;
    int mol_id;
    int oxy_number;
    int hydro_number;
    int num_queued_h;
    int num_queued_o;
    int max_mol;
    int max_atom;

    int oxygen_id;
    int hydrogen_id;

    sem_t sem_oxygen;
    sem_t sem_hydrogen;
    sem_t sem_mol_oxygen;
    sem_t sem_mol_hydrogen;
    sem_t sem_mol_end;
    sem_t sem_print;
    sem_t sem_end;
    sem_t sem_hydrogen_queued;
    sem_t sem_oxygen_queued;
    sem_t sleep;
    sem_t mutex;
    FILE *fp;

    args_t *args;
} data_t;

data_t *data_create();
void data_free(data_t *data);

bool its_number(const char *str);
bool args_in_range(const char *oxy_number, const char *hydro_number, const char *TI, const char *TB);
bool args_numbers(const char *oxy_number, const char *hydro_number, const char *TI, const char *TB);
args_t *args_create(const char *num_oxygen_, const char *num_hydrogen_, const char *TI_, const char *TB_);
void args_free(args_t *args);
void rand_sleep_ms(int start, int end);
data_t *data_create();
void output(data_t *data_ptr, int output_id, int atom_idx, char atom);
void oxygen_process(int atom_idx, data_t *data_ptr);
void hydrogen_process(int atom_idx, data_t *data_ptr);

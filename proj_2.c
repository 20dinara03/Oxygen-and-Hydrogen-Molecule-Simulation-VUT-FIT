#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <ctype.h>
#include <wait.h>
#include "header.h"

int rand_in_range(int start, int end)
{
    int num = (rand() % (end - start + 1)) + start;

    return num;
}

// functions

bool its_number(const char *str)
{
    if (strcmp(str, "") == 0) {
        fprintf(stderr, "Blank argument!\n");
        exit(1);
    }
    for (int i = 0; str[i] != 0; i++)
    {
        if (!isdigit(str[i])) {
            fprintf(stderr, "Wrong argument!\n");
            exit(1);
        }
    }
    return true;
}

bool args_in_range(const char *oxy_number, const char *hydro_number, const char *TI, const char *TB)
{
    if ((atoi(oxy_number) <= 0) || (atoi(hydro_number) <= 0) || (atoi(TI) < 0) || (atoi(TI) > 1000) || (atoi(TB) < 0) || (atoi(TB) > 1000))
    {
        fprintf(stderr, "Negative argument!\n");
        return false;
    }
    return true;
}

// check that all arguments are numbers and valid
bool args_numbers(const char *oxy_number, const char *hydro_number, const char *TI, const char *TB)
{
    if ((!its_number(oxy_number)) || (!its_number(hydro_number)) || (!its_number(TI)) || (!its_number(TB)))
    {
        fprintf(stderr, "Wrong argument!\n");
        exit(1);
    }
    return args_in_range(oxy_number, hydro_number, TI, TB);
}

args_t *args_create(const char *num_oxygen_, const char *num_hydrogen_, const char *TI_, const char *TB_)
{
    args_t *args = (args_t *)malloc(sizeof(args_t));
    if (!args_numbers(num_oxygen_, num_hydrogen_, TI_, TB_))
    {
        args_free(args);
        return NULL;
    }

    args->oxy_number = atoi(num_oxygen_);
    args->TB = atoi(TB_);
    args->TI = atoi(TI_);
    args->hydro_number = atoi(num_hydrogen_);

    return args;
}

void args_free(args_t *args)
{
    free(args);
}

void rand_sleep_ms(int start, int end)
{
    float sleep_time = rand_in_range(start, end);

    usleep(sleep_time * 1000);
}

// data_t constructor
data_t *data_create()
{
    data_t *data = mmap(NULL, sizeof(data_t),
                        PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (data == NULL)
    {
        return data;
    }

    data->fp = fopen("proj2.out", "w");

    data->queue = 0;
    data->counter = 1;
    data->mol_id = 1;
    data->oxy_number = 0;   // NO
    data->hydro_number = 0; // NH

    data->oxygen_id = 0;
    data->hydrogen_id = 0;

    sem_init(&(data->sem_oxygen), 1, 0);
    sem_init(&(data->sem_hydrogen), 1, 0);
    sem_init(&(data->sem_print), 1, 1);
    sem_init(&(data->sem_mol_oxygen), 1, 1);
    sem_init(&(data->sem_mol_hydrogen), 1, 0);
    sem_init(&(data->sem_end), 1, 0);
    sem_init(&(data->sem_mol_end), 1, 0);
    sem_init(&(data->sem_hydrogen_queued), 1, 2);
    sem_init(&(data->sem_oxygen_queued), 1, 0);
    sem_init(&(data->sleep), 1, 0);
    sem_init(&(data->mutex), 1, 0);

    return data;
}

// data_t destructor
void data_free(data_t *data)
{
    // close all semaphores
    sem_destroy(&(data->sem_oxygen));
    sem_destroy(&(data->sem_hydrogen));
    sem_destroy(&(data->sem_print));
    sem_destroy(&(data->sem_mol_oxygen));
    sem_destroy(&(data->sem_mol_hydrogen));
    sem_destroy(&(data->sem_mol_end));
    sem_destroy(&(data->sem_end));
    sem_destroy(&(data->sem_hydrogen_queued));
    sem_destroy(&(data->sem_oxygen_queued));
    sem_destroy(&(data->sleep));
    sem_destroy(&(data->mutex));
    fclose(data->fp);
}

void print_output(data_t *data_ptr, int output_id, int atom_idx, char atom)
{
    if (output_id == 0)
    {
        fprintf(data_ptr->fp, "%d: %c %d: started\n", data_ptr->counter++, atom, atom_idx);
        fflush(data_ptr->fp);
    }
    else if (output_id == 1)
    {
        fprintf(data_ptr->fp,"%d: %c %d: going to queue\n", data_ptr->counter++, atom, atom_idx);
        data_ptr->queue++;
        if (data_ptr->queue == data_ptr->max_atom)
        {
            for (int i = 0; i < data_ptr->max_atom; i++)
            {
                sem_post(&(data_ptr->sem_oxygen_queued));
            }
        }
        fflush(data_ptr->fp);
    }
    else if (output_id == 2)
    {
        fprintf(data_ptr->fp, "%d: %c %d: creating molecule %d\n", data_ptr->counter++, atom, atom_idx, data_ptr->mol_id);
        fflush(data_ptr->fp);
    }
    else if (output_id == 3)
    {
        fprintf(data_ptr->fp, "%d: %c %d: molecule %d created\n", data_ptr->counter++, atom, atom_idx, data_ptr->mol_id);
        fflush(data_ptr->fp);
    }
    else if (output_id == 4)
    {
        if (atom == 'O')
        {
            fprintf(data_ptr->fp, "%d: %c %d: not enough H\n", data_ptr->counter++, atom, atom_idx);
            fflush(data_ptr->fp);
        }
        else if (atom == 'H')
        {
            fprintf(data_ptr->fp, "%d: %c %d: not enough O or H\n", data_ptr->counter++, atom, atom_idx);
            fflush(data_ptr->fp);
        }
    }
}
void oxygen_process(int atom_idx, data_t *data_ptr)
{
    sem_wait(&(data_ptr->sem_print));
    print_output(data_ptr, 0, atom_idx, 'O');
    sem_post(&(data_ptr->sem_print));
    rand_sleep_ms(0, data_ptr->args->TI);

    sem_wait(&(data_ptr->sem_print));
    print_output(data_ptr, 1, atom_idx, 'O');
    sem_post(&(data_ptr->sem_print));

    sem_wait(&(data_ptr->sem_mol_oxygen));

    if (data_ptr->mol_id > data_ptr->max_mol)
    {
        if (data_ptr->queue != data_ptr->max_atom)
        {
            sem_wait(&(data_ptr->sem_oxygen_queued));
        }
        sem_wait(&(data_ptr->sem_print));
        print_output(data_ptr, 4, atom_idx, 'O');
        sem_post(&(data_ptr->sem_print));

        sem_post(&(data_ptr->sem_mol_oxygen));
        sem_post(&data_ptr->sem_end);
        sem_post(&data_ptr->sem_end);
        exit(0);
    }

    sem_wait(&(data_ptr->sem_oxygen));
    sem_wait(&(data_ptr->sem_oxygen));
    sem_post(&data_ptr->sem_hydrogen);
    sem_post(&data_ptr->sem_hydrogen);

    sem_wait(&(data_ptr->sem_print));
    print_output(data_ptr, 2, atom_idx, 'O');
    sem_post(&(data_ptr->sem_print));

    sem_wait(&(data_ptr->sem_mol_hydrogen));
    sem_wait(&(data_ptr->sem_mol_hydrogen));

    sem_post(&(data_ptr->sem_end));
    sem_post(&(data_ptr->sem_end));

    rand_sleep_ms(0, data_ptr->args->TB);

    sem_post(&(data_ptr->sleep));
    sem_post(&(data_ptr->sleep));

    sem_wait(&(data_ptr->sem_print));
    print_output(data_ptr, 3, atom_idx, 'O');
    sem_post(&(data_ptr->sem_print));

    sem_wait(&(data_ptr->sem_mol_end));
    sem_wait(&(data_ptr->sem_mol_end));

    sem_post(&(data_ptr->mutex));
    sem_post(&(data_ptr->mutex));

    data_ptr->mol_id += 1;

    sem_post(&(data_ptr->sem_mol_oxygen));
}

void hydrogen_process(int atom_idx, data_t *data_ptr)
{
    sem_wait(&(data_ptr->sem_print));
    print_output(data_ptr, 0, atom_idx, 'H');
    sem_post(&(data_ptr->sem_print));
    rand_sleep_ms(0, data_ptr->args->TI);

    sem_wait(&(data_ptr->sem_print));
    print_output(data_ptr, 1, atom_idx, 'H');
    sem_post(&(data_ptr->sem_print));

    sem_wait(&(data_ptr->sem_hydrogen_queued));

    if (data_ptr->mol_id > data_ptr->max_mol)
    {
        if (data_ptr->queue != data_ptr->max_atom)
        {
            sem_wait(&(data_ptr->sem_oxygen_queued));
        }
        sem_wait(&(data_ptr->sem_print));
        print_output(data_ptr, 4, atom_idx, 'H');
        sem_post(&(data_ptr->sem_print));
        sem_post(&data_ptr->sem_mol_hydrogen);
        sem_post(&data_ptr->sem_hydrogen_queued);
        exit(0);
    }

    sem_post(&(data_ptr->sem_oxygen));

    sem_wait(&(data_ptr->sem_hydrogen));

    sem_wait(&(data_ptr->sem_print));
    print_output(data_ptr, 2, atom_idx, 'H');
    sem_post(&(data_ptr->sem_print));

    sem_post(&(data_ptr->sem_mol_hydrogen));

    sem_wait(&(data_ptr->sem_end));

    sem_wait(&(data_ptr->sleep));

    sem_wait(&(data_ptr->sem_print));
    print_output(data_ptr, 3, atom_idx, 'H');
    sem_post(&(data_ptr->sem_print));

    sem_post(&(data_ptr->sem_mol_end));
    sem_wait(&(data_ptr->mutex));

    sem_post(&(data_ptr->sem_hydrogen_queued));
}

int main(int argc, char **argv)
{

    data_t *data_ptr = data_create();

    if (argc != 5)
    {
        fprintf(stderr, "Usage: ./main <max_mol> <max_atom> <TB> <TI>\n");
        exit(1);
    }

    args_t *args = args_create(argv[1], argv[2], argv[3], argv[4]);
    if (args == NULL)
    {
        return 1;
    }

    data_ptr->oxy_number = args->oxy_number;
    data_ptr->hydro_number = args->hydro_number;
    data_ptr->max_atom = data_ptr->oxy_number + data_ptr->hydro_number;

    data_ptr->args = args;
    if (data_ptr->oxy_number > data_ptr->hydro_number / 2)
    {
        data_ptr->max_mol = data_ptr->hydro_number / 2;
    }
    else
    {
        data_ptr->max_mol = data_ptr->oxy_number;
    }

    for (int i = 0; i < data_ptr->args->oxy_number; i++)
    {
        pid_t pid = fork();
        // child process
        if (pid == -1)
        {
            args_free(args);
            data_free(data_ptr);
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            data_ptr->oxygen_id += 1;
            oxygen_process(data_ptr->oxygen_id, data_ptr);
            exit(EXIT_SUCCESS);
        }
    }
    for (int i = 0; i < data_ptr->args->hydro_number; i++)
    {
        pid_t pid = fork();
        // child process
        if (pid == -1)
        {
            args_free(args);
            data_free(data_ptr);
            exit(EXIT_FAILURE);
        }
        if (pid == 0)
        {
            data_ptr->hydrogen_id += 1;
            hydrogen_process(data_ptr->hydrogen_id, data_ptr);
            exit(EXIT_SUCCESS);
        }
    }
    while (wait(NULL) > 0);

    args_free(args);
    data_free(data_ptr);
    exit(EXIT_SUCCESS);
}

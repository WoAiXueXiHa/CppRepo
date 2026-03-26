int global_init_var = 10;
int global_uninit_var;

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

extern int global_init_var;
void swap(int *a, int *b);

int main()
{
    int a = 10, b = 20;
    swap(&a, &b);
    return global_init_var;
}
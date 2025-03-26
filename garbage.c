





#define INIT_VECTOR(data_t, t) \
    typedef struct {int len; data_t * data} t; \
    void bidule(t * this) {}



INIT_VECTOR(int, vector_int)


int main(void) {

    vector_int truc = {0};
    bidule(&truc);



}



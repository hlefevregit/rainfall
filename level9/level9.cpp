#include <cstring>

class N {
public:
    virtual int operator+(N &other);
    virtual int operator-(N &other);

    char annotation[100];   // approximation
    int value;

    N(int v) {
        value = v;
    }

    void setAnnotation(char *s) {
        memcpy(annotation, s, strlen(s));
    }
};

N::N(int v) {
    this->vtable = &N::vtable;
    this->value = v;
}

int N::operator+(N *other) {
    return other->value + this->value;
}

int N::operator-(N *other) {
    return this->value - other->value;
}

void N::setAnnotation(char *s) {
    memcpy(this->annotation, s, strlen(s));
}

int main(int argc, char **argv) {
    if (argc < 2) {
        _exit(1);
    }

    N *a = new N(5);
    N *b = new N(6);

    a->setAnnotation(argv[1]);

    // appel virtuel sur b
    (*b)(a);   // en pratique: b->operator+(a) ou b->operator-(a)
}
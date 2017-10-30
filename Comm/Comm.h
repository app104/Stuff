#ifndef COMM_H
#define COMM_H

typedef struct _COMMS_{
    int NO;
    char LIP[32];
    int LPORT;
    char RIP[32];
    int RPORT;

}COMMS;


class Comm
{
public:
    Comm();
    ~Comm();
private:

};

#endif // COMM_H

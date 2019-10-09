//
// Created by greundzo on 08/10/19.
//
#include "shared.h"

int matricola, voto_AdE, nof_elements, nof_invites, max_reject;

int main(int argc, char ** argv)
{
    voto_AdE = generate_random_integer(18,30, getpid());
    printf("%d", voto_AdE);
    puts("");
    exit(0);
}

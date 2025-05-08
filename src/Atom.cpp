#pragma once
#include <string>

class Atom{
private:
    static unsigned int id_counter;
    std::string         symbol;
    std::string         name;
    double              electric_charge;
    double              mass;
    int                 radius;
    int                 color_id;
    unsigned int        id;

public:
    Atom(
        std::string     symbol,
        std::string     name,
        double          electric_charge,
        double          mass,
        int             radius,
        int             color_id
        ){    
        this->symbol            = symbol;
        this->name              = name;
        this->electric_charge   = electric_charge;
        this->mass              = mass;

        this->radius            = radius;
        this->color_id          = color_id;
        
        this->id = id_counter;
        id_counter++;
        }
    int get_color(){return color_id;}
    int get_id(){return this->id;}
    void get_data(double* m, double* c, int* r){
        *m = this->mass;
        *c = this->electric_charge;
        *r = this->radius;
    }
};





#ifndef __StaQue__H__
#define __StaQue__H__
#include "stack.h"
#include "queue.h"
#include <iomanip>
#include "../general/types.h"
#include "../foreach.h"
    //persona
    struct Alumno{
        int id;
        string nombre;
        int nota;
        string estado;


        Alumno() = default;

        Alumno(int _id, string _nombre, float _nota) : id(_id), nombre(_nombre), nota(_nota){

            if(nota>10.5){
            estado="aprobado";
            }else{
            estado="reprobado";
            }
        }
    //sobrecargamos el operador << para imprimir el objeto Alumno
    friend ostream& operator<<(ostream& os, const Alumno& alumno) {
        os << "   "
           << left  << setw(7)  << alumno.id << " | "
           << left  << setw(20) << alumno.nombre << " | "
           << right << setw(6)  << fixed << setprecision(1) << alumno.nota << " | "
           << left  << " " << alumno.estado;
        return os;
    }
    //operadores
    bool operator==(const Alumno& otro) const {
        return id == otro.id;
    }
    bool operator<(const Alumno& otro) const {
        return id < otro.id;
    }
    bool operator>(const Alumno& otro) const {
        return id > otro.id;
    }
};
void Escribir(string texto){
    cout<<endl;
    cout<<" "<<texto<<endl;
    cout<<endl;
};
void DemoP_C_();


#endif
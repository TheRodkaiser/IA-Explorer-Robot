#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <set>
#include <queue>

struct estado {
  int fila;
  int columna;
  int orientacion;
  bool zapatillas=false;
  bool bikini=false;

  bool operator==(const struct estado & o){
    return (fila==o.fila&&columna==o.columna&&orientacion==o.orientacion&&
      zapatillas==o.zapatillas && bikini==o.bikini);
  }
  bool operator!=(const struct estado & o){
    return (fila!=o.fila or columna!=o.columna or orientacion!=o.orientacion or
      zapatillas!=o.zapatillas or bikini!=o.bikini);
  }

};
struct ComparaEstados;

struct ComparaCoste;

struct nodo;

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
      necesitoCargar=buscandoBateria=cargando=false;
      pillado=0;
      hayPlan=false;
      zapatillas=bikini=false;
      objetivoActual=0;
      buscado=false;
      chocado=false;
      completado=false;
      primerMovimiento=true;
      for(int i=0;i<3;++i)
        conseguidos[i]=false;
      revelarMapa();
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      necesitoCargar=buscandoBateria=cargando=false;
      pillado=0;
      hayPlan=false;
      zapatillas=bikini=false;
      objetivoActual=0;
      buscado=false;
      chocado=false;
      completado=false;
      primerMovimiento=true;
      for(int i=0;i<3;++i)
        conseguidos[i]=false;
      revelarMapa();
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador * clone(){return new ComportamientoJugador(*this);}
    int calcularCoste(struct nodo & node,Action accion);
    void mirar(vector< vector< unsigned char> > &mapaResultado,int fil, int col, Sensores &sensores);
    bool estaDescubierto(estado & st);

  private:
    // Declarar Variables de Estado
    int costeObjetivo; // Coste de ir a un objetivo
    estado actual;
    list<estado> objetivos;
    list<estado> objetivoAux;
    estado destinoActual;
    int objetivoActual;
    list<Action> plan;
    list<Action> planAux;
    bool hayPlan;
    bool zapatillas;
    bool bikini;
    Action ultimaAccion;
    bool primerMovimiento,chocado;
    bool buscado;
    vector<estado> bateriasEncontradas;
    vector<pair<int,int>> posibleLobo;
    int contador;
    int pillado;
    int conseguidos[3];
    bool necesitoCargar,buscandoBateria,cargando;
    bool completado;


    // Métodos privados de la clase
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan, int & coste);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_AEstrella(const estado &origen, const estado &destino, list<Action> &plan, int & coste);
    bool pathFinding_ASVariosObjetivos(const estado &origen, const list<estado> &destinos, list<Action> &plan, int & coste);


    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);
    void compruebaNodo(set <struct estado,struct ComparaEstados> & Cerrados,priority_queue<struct nodo,vector<struct nodo>,struct ComparaCoste> & Abiertos,
 		  struct nodo & node, Action accion);
    void actualizarPosicion(Action accion);
    void buscaBateria(Sensores sensores);
    estado bateriaMasCercana();
    void marcarLobo();
    void revelarMapa();
    bool mismosPlanes(list<Action> a, list<Action> b);
    bool generarObjetivos();
    //int calcularCoste(nodo & node,Action accion);



};


int numeroDestinos(vector<bool> dests);

int funcionHeuristica(struct nodo origen, struct estado destino); //Variacion de la distancia euclídea

inline bool objetivosEncontrados(const vector<bool> & objetivos){
  int size=objetivos.size();
  bool encontrados=true;
  for(int i=0;i<size && encontrados;++i){
    encontrados=objetivos[i];
  }
  return encontrados;
}

//bool find(struct nodo & node, const vector<nodo> & v, int pos);

#endif

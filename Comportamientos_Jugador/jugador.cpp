#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>
#include <queue>
#include <map>
#include <limits.h>
#include <random>

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a realizar.
// Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores)
{
	Action sigAccion=actIDLE;
	if(completado)
		return sigAccion;

	// ---------------------------------------------------------------------------
	// --------------------------------NIVEL 3------------------------------------
	// ---------------------------------------------------------------------------
	if(sensores.nivel==3){

		if(pillado>=3){
			objetivos.pop_front();
			ultimaAccion=sigAccion=plan.front();
			plan.clear();
			hayPlan=false;
			//hayPlan=pathFinding(sensores.nivel,actual,objetivos,plan,costeObjetivo);
			pillado=0;
		}

		int size=mapaResultado.size();
		size*=size;
		cout<<"Número objetivos: "<<objetivos.size()<<endl;

		actual.fila = sensores.posF;
		actual.columna = sensores.posC;
		actual.orientacion = sensores.sentido;

		if(mapaResultado[actual.fila][actual.columna]=='K'){
			bikini=true;
			zapatillas=false;
		}
		else if(mapaResultado[actual.fila][actual.columna]=='D'){
			bikini=false;
			zapatillas=true;
		}

		mirar(mapaResultado,actual.fila,actual.columna,sensores);
		buscaBateria(sensores);

		if(sensores.bateria<800&&!bateriasEncontradas.empty() && !cargando){
			estado bat=bateriaMasCercana();
			cout<<"Fila batería:    "<<bat.fila<<endl;
			cout<<"Columna batería: "<<bat.columna<<endl;
			cout<<"Me queda poca batería, voy a buscar una\n";
			if(!buscandoBateria)
				hayPlan=pathFinding_AEstrella(actual,bat,plan,costeObjetivo);
			necesitoCargar=buscandoBateria=sensores.bateria<800;
		}
		if(necesitoCargar&&sensores.terreno[0]=='X'||cargando){
			necesitoCargar=cargando=sensores.bateria<2800;
			buscandoBateria=false;
			hayPlan=false;
			plan.clear();
			return actIDLE;
		}
		if(objetivos.empty()){
			completado=generarObjetivos();
		}

		if(mapaResultado[objetivos.front().fila][objetivos.front().columna]=='M' ||
				mapaResultado[objetivos.front().fila][objetivos.front().columna]=='P'){
					objetivos.pop_front();
					hayPlan=false;
			}

		if(actual.fila==objetivos.front().fila && actual.columna==objetivos.front().columna){
			objetivos.erase(objetivos.begin());
			hayPlan=false;
		}

		if(!hayPlan && !cargando /*&& !buscandoBateria*/){
			while(!objetivos.empty() && estaDescubierto(objetivos.front()))
				objetivos.pop_front();
			if(objetivos.empty())
				completado=generarObjetivos();
			if(!objetivos.empty())
				hayPlan=pathFinding(sensores.nivel,actual,objetivos,plan,costeObjetivo);
			}

		cout<<"Hay plan? "<<hayPlan<<endl;

		if(hayPlan&&plan.size()>0){
			if(((sensores.terreno[2]=='B'&&!zapatillas) || (sensores.terreno[2]=='A'&&!bikini)||
				sensores.terreno[2]=='P' || sensores.terreno[2]=='M')&&plan.front()==actFORWARD){
					if(!estaDescubierto(objetivos.front())){
						if(!buscandoBateria){
							plan.clear();
							if(!objetivos.empty())
								hayPlan=pathFinding(sensores.nivel,actual,objetivos,plan,costeObjetivo);
						}
						else{
							if(plan.size()>0){
								if(((sensores.terreno[2]=='B'&&!zapatillas) || (sensores.terreno[2]=='A'&&!bikini)||
									sensores.terreno[2]=='P' || sensores.terreno[2]=='M')&&plan.front()==actFORWARD){
									sigAccion=plan.front();
									plan.pop_front();
								}
							}
							//hayPlan=pathFinding_AEstrella(actual,bateriaMasCercana(),plan);
						}
					}
			}
			else{
				cout<<"Cogiendo la siguiente acción\n";
				sigAccion=plan.front();
				plan.erase(plan.begin());
			}
		}

		cout << "Mi Fila: " << actual.fila << endl;
		cout << "Mi Col : " << actual.columna << endl;
		cout << "Mi Ori : " << actual.orientacion << endl;
		cout << "Mi bat : " << sensores.bateria << endl;
		cout<<"Objetivo actual -> "<<objetivos.front().fila<<","<<objetivos.front().columna<<endl;
		cout<<"BuscandoBateria -> "<<buscandoBateria<<endl;
		cout<<"Cargando        -> "<<cargando<<endl;



	}
	// ---------------------------------------------------------------------------
	// -----------------------------------NIVEL 4---------------------------------
	// ---------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////

else if(sensores.nivel==4){

	if(ultimaAccion==actWHEREIS){
		actual.fila=sensores.posF;
		actual.columna=sensores.posC;
		actual.orientacion=sensores.sentido;

	}

	if(primerMovimiento || sensores.colision){
		cargando=buscandoBateria=false;
		primerMovimiento=false;
		hayPlan=false; // posible
		plan.clear();  // posible
		ultimaAccion=actWHEREIS;
		return actWHEREIS;
	}
	else if((sensores.bateria<800 && bateriasEncontradas.size()>0)&&!cargando&&!buscandoBateria){
		plan.clear();
		estado bat=bateriaMasCercana();
		cout<<"Me queda poca batería, voy a buscar una\n";
		cout<<"Batería más cercana: ["<<bat.fila<<","<<bat.columna<<"]\n";
		hayPlan=pathFinding_AEstrella(actual,bat,plan,costeObjetivo);
		//objetivos.push_front(bat);
		buscandoBateria=true;
	}
	else{
		if(buscandoBateria){
			if(!plan.empty()){
				if(((sensores.terreno[2]=='A' && !bikini) or (sensores.terreno[2]=='B' && !zapatillas)
      or sensores.terreno[2]=='M' or sensores.terreno[2]=='P')&& plan.front()==actFORWARD){
            estado bat=bateriaMasCercana();
						COUT<<"ME HE CHOCADO, PERO VUELVO A BUSCAR LA BATERÍA\N";
            hayPlan=pathFinding_AEstrella(actual,bat,plan,costeObjetivo);
          } // PARA QUE NO SE META POR DONDE NO DEBE SI ESTÁ BUSCANDO BATERÍA
					if(!((sensores.terreno[2]=='A' && !bikini) or (sensores.terreno[2]=='B' && !zapatillas)
	      		or sensores.terreno[2]=='M' or sensores.terreno[2]=='P' or
						sensores.superficie[2]=='l' or sensores.superficie[2]=='a'&& plan.front()==actFORWARD)){
					ultimaAccion=sigAccion=plan.front();
        	plan.pop_front();
				} // posible
			}
			buscandoBateria=plan.size()>0;
			cargando=!buscandoBateria;
			//cout<<"buscando batería? "<<buscandoBateria<<endl;
			//cout<<"Tamaño de plan -> "<<plan.size()<<endl;
			cout<<"Siguiente acción -> "<<sigAccion<<endl;
			mirar(mapaResultado,actual.fila,actual.columna,sensores);
			buscaBateria(sensores);
			actualizarPosicion(sigAccion);

			//	NO VAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

			return sigAccion;
		}
		if((necesitoCargar&sensores.terreno[0]=='X')||cargando){
			hayPlan=false;
			plan.clear();
			necesitoCargar=cargando=sensores.bateria<2800;
			return actIDLE;
		}

		mirar(mapaResultado,actual.fila,actual.columna,sensores);
		buscaBateria(sensores);


		hayPlan=plan.size()>0;


		cout << "Mi Fila: " << actual.fila << endl;
		cout << "Mi Col : " << actual.columna << endl;
		cout << "Mi Ori : " << actual.orientacion << endl;

		cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
		objetivos.clear();
		for (int i = 0; i < sensores.num_destinos; i++){
			estado aux;
			aux.fila = sensores.destino[2 * i];
			aux.columna = sensores.destino[2 * i + 1];
			objetivos.push_back(aux);
		}

		if(!hayPlan && !cargando){
			if(!buscandoBateria)
				hayPlan=pathFinding(sensores.nivel,actual,objetivos,plan,costeObjetivo);

		if(hayPlan && plan.size()>0 && !buscandoBateria && !cargando){
			if((sensores.terreno[2]=='M'||sensores.terreno[2]=='P'||(sensores.terreno[2]=='A'&&!bikini)
				|| sensores.terreno[2]=='B'&&!zapatillas)&&plan.front()==actFORWARD){
				plan.clear();
				/*if(buscandoBateria)
					hayPlan=pathFinding_AEstrella(actual,bateriaMasCercana(),plan);
				else*/
					hayPlan=pathFinding(sensores.nivel,actual,objetivos,plan,costeObjetivo);
			}
			if((sensores.superficie[2]=='a'||sensores.superficie[2]=='l')&&plan.front()==actFORWARD){
				sigAccion=actIDLE;
			}
			else{

				sigAccion=plan.front();
				plan.pop_front();
			}
		}
		actualizarPosicion(sigAccion);

		int i=0;
		for(auto it=objetivos.begin();it!=objetivos.end();it++,++i){
			if(actual.fila==(*it).fila && actual.columna==(*it).columna){
				conseguidos[i]=true;
			}
		}


		 if(mapaResultado[actual.fila][actual.columna]=='K'){
			 cout<<"He encotrado un bikiniiiiii\n";
			 actual.bikini=bikini=true;
			 actual.zapatillas=zapatillas=false;
		 }
		 else if(mapaResultado[actual.fila][actual.columna]=='D'){
			 cout<<"He encontrado unas zapatillassssss\n";
			 actual.bikini=bikini=false;
			 actual.zapatillas=zapatillas=true;
		 }
	}

}
else{
	actual.fila = sensores.posF;
actual.columna = sensores.posC;
actual.orientacion = sensores.sentido;

cout << "Fila: " << actual.fila << endl;
cout << "Col : " << actual.columna << endl;
cout << "Ori : " << actual.orientacion << endl;

// Capturo los destinos
cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
objetivos.clear();
for (int i = 0; i < sensores.num_destinos; i++)
{
	estado aux;
	aux.fila = sensores.destino[2 * i];
	aux.columna = sensores.destino[2 * i + 1];
	objetivos.push_back(aux);
}

if(hayPlan && plan.size()>0){
	cout<<"he hecho una acción del plan\n";
	sigAccion=plan.front();
	plan.pop_front();
}

if(!hayPlan){
	hayPlan = pathFinding(sensores.nivel, actual, objetivos, plan,costeObjetivo);
}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////	GENERAL ////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


	if(sensores.nivel==3){
		if(sigAccion==actIDLE){
			pillado++;
		}
		else{
			pillado=0;
		}

		if(pillado>=3){
			if(plan.size()>0){
				objetivos.pop_front();
				sigAccion=plan.front();
				plan.clear();
				hayPlan=false;
				pillado=0;
			}
		}
	}

	cout<<"Siguiente acción: "<<sigAccion<<"\n";

	ultimaAccion=sigAccion;

	cout<<"Acción ---> "<<ultimaAccion<<endl;

	cout<<"\n------------------------------------------\n";

	return sigAccion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan, int & coste)
{
	switch (level)
	{
	case 0:{
		cout << "Demo\n";
		estado un_objetivo;
		un_objetivo = objetivos.front();
		cout << "fila: " << un_objetivo.fila << " col:" << un_objetivo.columna << endl;
		return pathFinding_Profundidad(origen, un_objetivo, plan);
		break;
	}
	case 1:{
		cout << "Optimo numero de acciones\n";
		estado un_objetivo1;
		un_objetivo1=objetivos.front();
		cout << "fila: " << un_objetivo1.fila << " col:" << un_objetivo1.columna << endl;
		return pathFinding_Anchura(origen, un_objetivo1, plan);
		break;
	}
	case 2:{
		cout << "Optimo en coste\n";
		estado un_objetivo2;
		un_objetivo2=objetivos.front();
		cout << "fila: " << un_objetivo2.fila << " col:" << un_objetivo2.columna << endl;
		return pathFinding_AEstrella(origen, un_objetivo2, plan,coste);
		break;
	}
	case 3:{
		cout << "Reto 1: Descubrir el mapa\n";
		estado un_objetivo3;
		un_objetivo3=objetivos.front();
		return pathFinding_AEstrella(origen,un_objetivo3,plan,coste);
		break;
	}
	case 4:{
		cout << "Reto 2: Maximizar objetivos\n";
		// Incluir aqui la llamada al algoritmo de busqueda para el Reto 2
		return pathFinding_ASVariosObjetivos(origen, destino, plan,coste);
		break;
	}
	}
	return false;
}

//---------------------- Implementación de la busqueda en profundidad ---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla)
{
	if (casilla == 'P' or casilla == 'M')
		return true;
	else
		return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st)
{
	int fil = st.fila, col = st.columna;

	// calculo cual es la casilla de delante del agente
	switch (st.orientacion)
	{
	case 0:
		fil--;
		break;
	case 1:
		fil--;
		col++;
		break;
	case 2:
		col++;
		break;
	case 3:
		fil++;
		col++;
		break;
	case 4:
		fil++;
		break;
	case 5:
		fil++;
		col--;
		break;
	case 6:
		col--;
		break;
	case 7:
		fil--;
		col--;
		break;
	}

	// Compruebo que no me salgo fuera del rango del mapa
	if (fil < 0 or fil >= mapaResultado.size())
		return true;
	if (col < 0 or col >= mapaResultado[0].size())
		return true;

	// Miro si en esa casilla hay un obstaculo infranqueable
	if (!EsObstaculo(mapaResultado[fil][col]))
	{
		// No hay obstaculo, actualizo el parametro st poniendo la casilla de delante.
		st.fila = fil;
		st.columna = col;
		return false;
	}
	else
	{
		return true;
	}
}


struct nodo
{
	int coste=0;
	float distancia=0;
	estado st;
	list<Action> secuencia;
	const bool operator<(const nodo & otro){
		return coste+distancia<otro.coste+otro.distancia;
	}
	nodo generarHijo(){
		nodo hijo;
		hijo.coste=coste;
		hijo.distancia=distancia;
		hijo.st=st;
		hijo.secuencia=secuencia;
		return hijo;
	}
	bool operator==(struct nodo & otro){
		return st.fila==otro.st.fila && st.columna==otro.st.columna;
	}
	bool operator<(struct nodo & otro ){
		return coste < otro.coste;
	}
};


struct ComparaCoste{
	bool operator()(const nodo & a, const nodo &b)const{
		return a.coste+a.distancia>b.coste+b.distancia;
	}
};


struct ComparaEstados
{
	bool operator()(const estado &a, const estado &n) const
	{
		if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
			(a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion)
			or (a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and
					a.bikini > n.bikini) or (a.fila == n.fila and a.columna == n.columna and
						 a.orientacion == n.orientacion and a.bikini==n.bikini and a.zapatillas > n.zapatillas ))
			return true;
		else
			return false;
	}
};

struct ComparaNodo{
	bool operator()(const nodo &a, const nodo &n) const
	{
		if ((a.st.fila > n.st.fila) or (a.st.fila == n.st.fila and a.st.columna > n.st.columna) or
			(a.st.fila == n.st.fila and a.st.columna == n.st.columna and a.st.orientacion > n.st.orientacion))
			return true;
		else
			return false;
	}
};

struct ComparaCerrados
{
	bool operator()(const pair<estado,int> &a, const pair<estado,int> &n) const
	{
		if ((a.first.fila > n.first.fila) or (a.first.fila == n.first.fila and a.first.columna > n.first.columna) or
			(a.first.fila == n.first.fila and a.first.columna == n.first.columna and a.first.orientacion > n.first.orientacion))
			return true;
		else
			return false;
	}
};

// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan)
{
	// Borro la lista
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	stack<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		if(Cerrados.find(current.st)==Cerrados.end())
			Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		//nodo hijoTurnR = current.generarHijo();
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}

bool ComportamientoJugador::pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan){
	cout << "Calculando plan\n";
	plan.clear();
	set<estado, ComparaEstados> Cerrados; // Lista de Cerrados
	queue<nodo> Abiertos;				  // Lista de Abiertos

	nodo current;
	current.st = origen;
	current.secuencia.empty();

	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		Abiertos.pop();
		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR = current;
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		if (Cerrados.find(hijoTurnR.st) == Cerrados.end())
		{
			hijoTurnR.secuencia.push_back(actTURN_R);
			Abiertos.push(hijoTurnR);
		}

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current;
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end())
		{
			hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
			Abiertos.push(hijoSEMITurnR);
		}

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL = current;
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		if (Cerrados.find(hijoTurnL.st) == Cerrados.end())
		{
			hijoTurnL.secuencia.push_back(actTURN_L);
			Abiertos.push(hijoTurnL);
		}

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL = current;
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end())
		{
			hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
			Abiertos.push(hijoSEMITurnL);
		}

		// Generar descendiente de avanzar
		nodo hijoForward = current;
		if (!HayObstaculoDelante(hijoForward.st))
		{
			if (Cerrados.find(hijoForward.st) == Cerrados.end())
			{
				hijoForward.secuencia.push_back(actFORWARD);
				Abiertos.push(hijoForward);
			}
		}

		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.front();
			while(Cerrados.find(current.st)==Cerrados.end()){
				Abiertos.pop();
				current=Abiertos.front();
			}
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}


// *  *  *  *  *  *  *  *  *    NIVEL 2   *  *  *  *  *  *  *  *  *
// *  *  *  *  *  *  *  *  * ALGORITMO A* *  *  *  *  *  *  *  *  *




bool ComportamientoJugador::pathFinding_AEstrella(const estado &origen, const estado &destino, list<Action> &plan, int & coste){
	cout << "Calculando plan\n";
	plan.clear();
	set<estado,ComparaEstados> Cerrados;
	priority_queue<nodo,vector<nodo>,ComparaCoste> Abiertos;				  // Lista de Abiertos
	nodo current;
	current.st = origen;
	current.secuencia.empty();
	Abiertos.push(current);

	while (!Abiertos.empty() and (current.st.fila != destino.fila or current.st.columna != destino.columna))
	{

		current=Abiertos.top();

		Abiertos.pop();

		if(mapaResultado[current.st.fila][current.st.columna]=='K'){
			current.st.bikini=1;
			current.st.zapatillas=0;
		}
		else if(mapaResultado[current.st.fila][current.st.columna]=='D'){
			current.st.bikini=0;
			current.st.zapatillas=1;
		}

		Cerrados.insert(current.st);

		// Generar descendiente de girar a la derecha 90 grados
		nodo hijoTurnR=current.generarHijo();
		hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
		hijoTurnR.coste+=calcularCoste(hijoTurnR,actTURN_R);
		compruebaNodo(Cerrados,Abiertos,hijoTurnR,actTURN_R);

		// Generar descendiente de girar a la derecha 45 grados
		nodo hijoSEMITurnR = current.generarHijo();
		hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
		hijoSEMITurnR.coste+=calcularCoste(hijoSEMITurnR,actSEMITURN_R);
		compruebaNodo(Cerrados,Abiertos,hijoSEMITurnR,actSEMITURN_R);

		// Generar descendiente de girar a la izquierda 90 grados
		nodo hijoTurnL=current.generarHijo();
		hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
		hijoTurnL.coste+=calcularCoste(hijoTurnL,actTURN_L);
		compruebaNodo(Cerrados,Abiertos,hijoTurnL,actTURN_L);

		// Generar descendiente de girar a la izquierda 45 grados
		nodo hijoSEMITurnL=current.generarHijo();
		hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
		hijoSEMITurnL.coste+=calcularCoste(hijoSEMITurnL,actSEMITURN_L);
		compruebaNodo(Cerrados,Abiertos,hijoSEMITurnL,actSEMITURN_L);

		// Generar descendiente de avanzar
		nodo hijoForward=current.generarHijo();
		if (!HayObstaculoDelante(hijoForward.st))
		{
			hijoForward.coste+=calcularCoste(hijoForward,actFORWARD);
			compruebaNodo(Cerrados,Abiertos,hijoForward,actFORWARD);
		}

		//cout<<"--------------------------------"<<endl;
		// Tomo el siguiente valor de la Abiertos
		if (!Abiertos.empty())
		{
			current = Abiertos.top();
			//BORRO LOS NODOS REPETIDOS
			while(Cerrados.find(current.st)!=Cerrados.end()&&!Abiertos.empty()){
				Abiertos.pop();
				current=Abiertos.top();
			}
		}
	}

	cout << "Terminada la busqueda\n";

	if (current.st.fila == destino.fila and current.st.columna == destino.columna)
	{
		cout << "Cargando el plan\n";
		plan = current.secuencia;
		coste=current.coste;
		cout << "Longitud del plan: " << plan.size() << endl;
		PintaPlan(plan);
		// ver el plan en el mapa
		VisualizaPlan(origen, plan);
		return true;
	}
	else
	{
		cout << "No encontrado plan\n";
	}

	return false;
}


// *  *  *  *  *  *  *  *  *    NIVEL 3    *  *  *  *  *  *  *  *  *

//void descubrirMapa();








// *  *  *  *  *  *  *  *  *    NIVEL 4    *  *  *  *  *  *  *  *  *
bool ComportamientoJugador::pathFinding_ASVariosObjetivos(const estado &origen, const list<estado> &destinos, list<Action> &plan, int & coste){

	estado destino;
	int minimo=INT_MAX;

	if(conseguidos[0]&&conseguidos[1]&&conseguidos[2])
		conseguidos[0]=conseguidos[1]=conseguidos[2]=false;

	int i=0;
	for(auto it=destinos.begin();it!=destinos.end();++it,++i){
		int distancia=fmax(abs(origen.fila-abs((*it).fila)),abs(origen.columna)-abs((*it).columna));
		if(distancia<minimo && !conseguidos[i]){
			destinoActual=destino=*it;
			minimo=distancia;
		}
	}

	return pathFinding_AEstrella(origen,destino,plan,coste);

}

// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			cout << "A ";
		}
		else if (*it == actTURN_R)
		{
			cout << "D ";
		}
		else if (*it == actSEMITURN_R)
		{
			cout << "d ";
		}
		else if (*it == actTURN_L)
		{
			cout << "I ";
		}
		else if (*it == actSEMITURN_L)
		{
			cout << "I ";
		}
		else
		{
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m)
{
	for (int i = 0; i < m[0].size(); i++)
	{
		for (int j = 0; j < m.size(); j++)
		{
			m[i][j] = 0;
		}
	}
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan)
{
	AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if (*it == actFORWARD)
		{
			switch (cst.orientacion)
			{
			case 0:
				cst.fila--;
				break;
			case 1:
				cst.fila--;
				cst.columna++;
				break;
			case 2:
				cst.columna++;
				break;
			case 3:
				cst.fila++;
				cst.columna++;
				break;
			case 4:
				cst.fila++;
				break;
			case 5:
				cst.fila++;
				cst.columna--;
				break;
			case 6:
				cst.columna--;
				break;
			case 7:
				cst.fila--;
				cst.columna--;
				break;
			}
			mapaConPlan[cst.fila][cst.columna] = 1;
		}
		else if (*it == actTURN_R)
		{
			cst.orientacion = (cst.orientacion + 2) % 8;
		}
		else if (*it == actSEMITURN_R)
		{
			cst.orientacion = (cst.orientacion + 1) % 8;
		}
		else if (*it == actTURN_L)
		{
			cst.orientacion = (cst.orientacion + 6) % 8;
		}
		else if (*it == actSEMITURN_L)
		{
			cst.orientacion = (cst.orientacion + 7) % 8;
		}
		it++;
	}
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}

int ComportamientoJugador::calcularCoste(nodo & node, Action accion){
	char posicion=mapaResultado[node.st.fila][node.st.columna];
	//cout<<"Fila: "<<node.st.fila<<". Columna: "<<node.st.columna<<endl;
	int costo;

	//cout<<"Tipo terreno: "<<posicion<<endl;

	switch (posicion) {
		case 'A':

			//cout<<"Estoy en agua"<<endl;
			if(node.st.bikini){
				if(accion==actFORWARD)
					costo=10;
				else if(accion==actTURN_L||accion==actTURN_R)
					costo=5;
				else
					costo=2;
			}
			else {
				if(accion==actFORWARD)
					costo=200;
				else if(accion==actTURN_L||accion==actTURN_R)
					costo=500;
				else
					costo=300;
			}
			break;
		case 'B':
			if(node.st.zapatillas){
				if(accion==actFORWARD)
					costo=15;
				else
					costo=1;//+15;
				}
				else {
					if(accion==actFORWARD)
						costo=100;
					else if(accion==actTURN_L||accion==actTURN_R)
						costo=3;
					else
						costo=2;
				}
				break;
		case 'T':
			if(accion==actSEMITURN_L||accion==actSEMITURN_R)
				costo=1;
			else
				costo=2;
			break;
		case 'P':
			costo=INT_MAX;
			break;
		case 'M':
			costo=INT_MAX;
			break;
		default:
			costo=1;
	}

	node.distancia=funcionHeuristica(node, *objetivos.begin());

	return costo;

}

int funcionHeuristica(struct nodo origen, struct estado destino){
	int x=abs(origen.st.fila-destino.fila);
	int y=abs(origen.st.columna-destino.columna);
	return fmax(x,y);
}


void ComportamientoJugador::mirar(vector< vector< unsigned char> > &mapaResultado,int fil, int col, Sensores &sensores){
	 int brujula = actual.orientacion;
	 mapaResultado[fil][col]=sensores.terreno[0];
	 	if(brujula==0){
			 for(int i=1,j=0;i<=3;++i,++j)
				 mapaResultado[fil-1][col+j-1]=sensores.terreno[i];
			 for(int i=4,j=0;i<=8;++i,++j)
				 mapaResultado[fil-2][col+j-2]=sensores.terreno[i];
			 for(int i=9,j=0;i<=15;++i,++j)
				 mapaResultado[fil-3][col+j-3]=sensores.terreno[i];
		 }

		 else if(brujula==1){
			 mapaResultado[fil-1][col]=sensores.terreno[1];
			 mapaResultado[fil-1][col+1]=sensores.terreno[2];
			 mapaResultado[fil][col+1]=sensores.terreno[3];
			 mapaResultado[fil-2][col]=sensores.terreno[4];
			 mapaResultado[fil-2][col+1]=sensores.terreno[5];
			 mapaResultado[fil-2][col+2]=sensores.terreno[6];
			 mapaResultado[fil-1][col+2]=sensores.terreno[7];
			 mapaResultado[fil][col+2]=sensores.terreno[8];
			 mapaResultado[fil-3][col]=sensores.terreno[9];
			 mapaResultado[fil-3][col+1]=sensores.terreno[10];
			 mapaResultado[fil-3][col+2]=sensores.terreno[11];
			 mapaResultado[fil-3][col+3]=sensores.terreno[12];
			 mapaResultado[fil-2][col+3]=sensores.terreno[13];
			 mapaResultado[fil-1][col+3]=sensores.terreno[14];
			 mapaResultado[fil][col+3]=sensores.terreno[15];
		 }

		 else if(brujula==2){
			 for(int i=1,j=0;i<=3;++i,++j)
				 mapaResultado[fil-1+j][col+1]=sensores.terreno[i];
			 for(int i=4,j=0;i<=8;++i,++j)
				 mapaResultado[fil-2+j][col+2]=sensores.terreno[i];
			 for(int i=9,j=0;i<=15;++i,++j)
				 mapaResultado[fil-3+j][col+3]=sensores.terreno[i];
		 }
		 else if(brujula==3){
			 mapaResultado[fil][col+1]=sensores.terreno[1];
			 mapaResultado[fil+1][col+1]=sensores.terreno[2];
			 mapaResultado[fil+1][col]=sensores.terreno[3];
			 mapaResultado[fil][col+2]=sensores.terreno[4];
			 mapaResultado[fil+1][col+2]=sensores.terreno[5];
			 mapaResultado[fil+2][col+2]=sensores.terreno[6];
			 mapaResultado[fil+2][col+1]=sensores.terreno[7];
			 mapaResultado[fil+2][col]=sensores.terreno[8];
			 mapaResultado[fil][col+3]=sensores.terreno[9];
			 mapaResultado[fil+1][col+3]=sensores.terreno[10];
			 mapaResultado[fil+2][col+3]=sensores.terreno[11];
			 mapaResultado[fil+3][col+3]=sensores.terreno[12];
			 mapaResultado[fil+3][col+2]=sensores.terreno[13];
			 mapaResultado[fil+3][col+1]=sensores.terreno[14];
			 mapaResultado[fil+3][col]=sensores.terreno[15];
		 }
		 else if(brujula==4){
			 for(int i=1,j=0;i<=3;++i,++j)
				 mapaResultado[fil+1][col+1-j]=sensores.terreno[i];
			 for(int i=4,j=0;i<=8;++i,++j)
				 mapaResultado[fil+2][col+2-j]=sensores.terreno[i];
			 for(int i=9,j=0;i<=15;++i,++j)
				 mapaResultado[fil+3][col+3-j]=sensores.terreno[i];
		 }
		 else if(brujula==5){
			 mapaResultado[fil+1][col]=sensores.terreno[1];
			 mapaResultado[fil+1][col-1]=sensores.terreno[2];
			 mapaResultado[fil][col-1]=sensores.terreno[3];
			 mapaResultado[fil+2][col]=sensores.terreno[4];
			 mapaResultado[fil+2][col-1]=sensores.terreno[5];
			 mapaResultado[fil+2][col-2]=sensores.terreno[6];
			 mapaResultado[fil+1][col-2]=sensores.terreno[7];
			 mapaResultado[fil][col-2]=sensores.terreno[8];
			 mapaResultado[fil+3][col]=sensores.terreno[9];
			 mapaResultado[fil+3][col-1]=sensores.terreno[10];
			 mapaResultado[fil+3][col-2]=sensores.terreno[11];
			 mapaResultado[fil+3][col-3]=sensores.terreno[12];
			 mapaResultado[fil+2][col-3]=sensores.terreno[13];
			 mapaResultado[fil+1][col-3]=sensores.terreno[14];
			 mapaResultado[fil][col-3]=sensores.terreno[15];
		 }
		 else if(brujula==6){
			for(int i=1,j=0;i<=3;++i,++j)
 				mapaResultado[fil-j+1][col-1]=sensores.terreno[i];
 			for(int i=4,j=0;i<=8;++i,++j)
 				mapaResultado[fil-j+2][col-2]=sensores.terreno[i];
 			for(int i=9,j=0;i<=15;++i,++j)
 				mapaResultado[fil-j+3][col-3]=sensores.terreno[i];
		 }
		 else if(brujula==7){
			 mapaResultado[fil][col-1]=sensores.terreno[1];
			 mapaResultado[fil-1][col-1]=sensores.terreno[2];
			 mapaResultado[fil-1][col]=sensores.terreno[3];
			 mapaResultado[fil][col-2]=sensores.terreno[4];
			 mapaResultado[fil-1][col-2]=sensores.terreno[5];
			 mapaResultado[fil-2][col-2]=sensores.terreno[6];
			 mapaResultado[fil-2][col-1]=sensores.terreno[7];
			 mapaResultado[fil-2][col]=sensores.terreno[8];
			 mapaResultado[fil][col-3]=sensores.terreno[9];
			 mapaResultado[fil-1][col-3]=sensores.terreno[10];
			 mapaResultado[fil-2][col-3]=sensores.terreno[11];
			 mapaResultado[fil-3][col-3]=sensores.terreno[12];
			 mapaResultado[fil-3][col-2]=sensores.terreno[13];
			 mapaResultado[fil-3][col-1]=sensores.terreno[14];
			 mapaResultado[fil-3][col]=sensores.terreno[15];
		 }
	 }

	 void ComportamientoJugador::compruebaNodo(set<struct estado,struct ComparaEstados> & Cerrados,priority_queue<struct nodo,vector<struct nodo>, struct ComparaCoste> & Abiertos,
		 struct nodo & node, Action accion){
		 if (Cerrados.find(node.st) == Cerrados.end())
 		{
 			node.secuencia.push_back(accion);
 			Abiertos.push(node);
 		}
	 }

/*
	 void ComportamientoJugador::veoBateria(Sensores sensores){
		 int size=sensores.terreno.size();
		 for(int i=0;i<size;++i){
			 if(sensores.terreno[i]=='X'){

			 }
		 }
	 }*/


	 int numeroDestinos(vector<bool> dests){
		 int n=0;
		 int size=dests.size();
		 for(int i=0;i<size;++i){
			 if(dests[i]==true){
				 n+=pow(2,i); //Suma en potencias de 2 para diferenciar qué objetivos ha alcanzado
			 }
		 }
		 return n;
	 }

	 void ComportamientoJugador::actualizarPosicion(Action accion)
	 {

	 		if (accion == actFORWARD)
	 		{
	 			switch (actual.orientacion)
	 			{
	 			case 0:
	 				actual.fila--;
	 				break;
	 			case 1:
	 				actual.fila--;
	 				actual.columna++;
	 				break;
	 			case 2:
	 				actual.columna++;
	 				break;
	 			case 3:
	 				actual.fila++;
	 				actual.columna++;
	 				break;
	 			case 4:
	 				actual.fila++;
	 				break;
	 			case 5:
	 				actual.fila++;
	 				actual.columna--;
	 				break;
	 			case 6:
	 				actual.columna--;
	 				break;
	 			case 7:
	 				actual.fila--;
	 				actual.columna--;
	 				break;
	 			}
	 		}
	 		else if (accion == actTURN_R)
	 		{
	 			actual.orientacion = (actual.orientacion + 2) % 8;
	 		}
	 		else if (accion == actSEMITURN_R)
	 		{
	 			actual.orientacion = (actual.orientacion + 1) % 8;
	 		}
	 		else if (accion == actTURN_L)
	 		{
	 			actual.orientacion = (actual.orientacion + 6) % 8;
	 		}
	 		else if (accion == actSEMITURN_L)
	 		{
	 			actual.orientacion = (actual.orientacion + 7) % 8;
	 		}
	 	}

		void ComportamientoJugador::buscaBateria(Sensores sensores){
		  int tempx,tempy;
		  bool encontrado=false;
		 	int i, fw, closest;
		 	for(i=1;i<sensores.terreno.size() && !encontrado;++i){
		 		if(sensores.terreno[i]=='X' && !encontrado){
		 			encontrado=true;
		             cout<<"ENCONTRADO"<<endl;
		 			if(i<=3){
		 				fw=1;
		 				closest=i-2;
		 			}
		 			else if(i<=8){
		 				fw=2;
		 				closest=i-6;
		 			}
		 			else if(i<=15){
		 				fw=3;
		 				closest=i-12;
		 			}

		 			if(actual.orientacion==0){
		   				tempx=actual.fila-fw;
		   				tempy=actual.columna+closest;
		   			}
		   			else if(actual.orientacion==2){
		   				tempx=actual.fila+closest;
		   				tempy=actual.columna+fw;
		   			}
		   			else if(actual.orientacion==4){
		   				tempx=actual.fila+fw;
		   				tempy=actual.columna-closest;
		   			}
		   			else if(actual.orientacion==6){
		   				tempx=actual.fila-closest;
		   				tempy=actual.columna-fw;
		   			}
		      }
		    }
		  if(encontrado){
				if(actual.orientacion%2==1){
					for(int i=0;i<mapaResultado.size();++i){
						for(int j=0;j<mapaResultado.size();++j){
							if(mapaResultado[i][j]=='X'){
								tempx=i;
								tempy=j;
							}
						}
					}
				}
		    estado pos=actual;
				pos.fila=tempx;
				pos.columna=tempy;
				bateriasEncontradas.push_back(pos);
		  }
	}

	estado ComportamientoJugador::bateriaMasCercana(){
		int min=INT_MAX,distancia;
		estado masCercana;
		for(int i=0;i<bateriasEncontradas.size();++i){
			distancia=fmax(abs(actual.fila-bateriasEncontradas[i].fila),abs(actual.columna-bateriasEncontradas[i].columna));
			if(distancia<min){
				min=distancia;
				masCercana=bateriasEncontradas[i];
			}
		}
		return masCercana;
	}

	void ComportamientoJugador::marcarLobo(){
		int fil = actual.fila, col = actual.columna;
		switch (actual.orientacion)
		{
		case 0:
			fil--;
			break;
		case 1:
			fil--;
			col++;
			break;
		case 2:
			col++;
			break;
		case 3:
			fil++;
			col++;
			break;
		case 4:
			fil++;
			break;
		case 5:
			fil++;
			col--;
			break;
		case 6:
			col--;
			break;
		case 7:
			fil--;
			col--;
			break;
		}

		posibleLobo.push_back(make_pair(fil,col));
	}

	void ComportamientoJugador::revelarMapa(){
		for(int i=0;i<3;++i){
			for(int j=0;j<mapaResultado.size();++j){
				mapaResultado[i][j]='P';
				mapaResultado[mapaResultado.size()-1-i][j]='P';
			}
		}
		for(int j=0;j<3;++j){
			for(int i=0;i<mapaResultado.size();++i){
				mapaResultado[i][j]='P';
				mapaResultado[i][mapaResultado.size()-1-j]='P';
			}
		}
	}

	bool ComportamientoJugador::estaDescubierto(estado & st){
		return mapaResultado[st.fila][st.columna]!='?';
	}

/*
	bool ComportamientoJugador::mismosPlanes(list<Action> a, list<Action> b){
		bool iguales=true;
		auto ita=a.begin();
		for(auto itb=b.begin();ita!=a.end()&&itb!=b.end()&&iguales;++ita,++itb){
			if(*ita!=*itb)
				iguales=false;
		}
		return iguales;
	}*/

	bool ComportamientoJugador::generarObjetivos(){
		estado objetivo;
		int size=mapaResultado.size();
		size*=size;
		for(int i = 0; i < size/4; i++){
			objetivo.fila=3+rand()%(mapaResultado.size()-7);
			objetivo.columna=3+rand()%(mapaResultado.size()-7);
			if(!estaDescubierto(objetivo))
			objetivos.push_back(objetivo);
		}
		if(objetivos.size()==0){
			for(int i=3;i<mapaResultado.size()-3;++i){
				for(int j=3;j<mapaResultado.size()-3;++j){
					if(mapaResultado[i][j]=='?'){
						objetivo.fila=i;
						objetivo.columna=j;
						objetivos.push_back(objetivo);
					}
				}
			}
		}
		return objetivos.size()==0;
	}

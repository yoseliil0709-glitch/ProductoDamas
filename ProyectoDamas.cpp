#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream> //Lee y escribe archivos
#include <vector>
#include <windows.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

int tiempo[3] = {0, 120, 120};
int turno = 1;
steady_clock::time_point tini;
const int limtiem = 120;
//Defeni el tablero (el tamaño)
int tablero[8][8];
//Diseño 0=Vacio, 1=blanca (b), 2=negra (n), 3=Dama blanca (B), 4=Dama negra (N)
struct Mov {
    int filaOri, colOri, filaDes, colDes; //Fila y columna de origen y destino
    int fichaMov; //Valor de la ficha que se movio
    int fichaCom; //Valor de la ficha comida
};

vector<Mov> historial; //Guarda los movimientos
int turnoGua = 1; //Turno actual para reanudar

void InicializarTablero(){
    for(int f=0; f<8; f++) for(int c=0; c<8; c++) tablero[f][c]=0; //Vacia el tablero 
    for(int f=0; f<3; f++) for(int c=0; c<8; c++) if((f+c)%2==1) tablero[f][c]=2; //Coloca fichas negras arriba
    for(int f=5; f<8; f++) for(int c=0; c<8; c++) if((f+c)%2==1) tablero[f][c]=1; //Coloca fichas blancas abajo
}

void MostrarTablero(){
    //Diseño del tablero (colores, numeros de coordenadas, posicion de las fichas)
    //Colores ANSI
    string RESET="\033[0m";
    string FONDOCLA="\033[47m";
    string FONDOOSC="\033[100m";
    string B="\033[97;1m";
    string N="\033[30;1m";
    string R="\033[91;1m";
    string A="\033[93;1m";

    cout<<"\n------- 0  1  2  3  4  5  6  7---\n";
    for(int f=0; f<8; f++){

        cout << " . " << f << " . "; //Bucle fila por fila
        for(int c=0; c<8; c++){ //Bucle de columnas
            string fondo = ((f+c)%2==0)? FONDOCLA:FONDOOSC;

            if(tablero[f][c]==0) cout << fondo << " - " << RESET;
            else if(tablero[f][c]==1) cout << fondo << B << " b " << RESET;
            else if(tablero[f][c]==2) cout << fondo << N << " n " << RESET;
            else if(tablero[f][c]==3) cout << fondo << A << " B " << RESET;
            else if(tablero[f][c]==4) cout << fondo << R << " N " << RESET;
        }
        cout << " " << f << endl;
    }
    cout << "------- 0  1  2  3  4  5  6  7---\n";
}

void ActuTabl(){
    cout << "\n\n Actualizando el juego......." << endl;
    Sleep(1000); //1 segundo de es6pera
    system("cls"); //Limpia pantalla
}

bool Comer(int f, int c){
    //Verifica que la ficha en la columna y fila pueda comer fichas del contrincante
    int pieza=tablero[f][c]; if(pieza==0) return false; //Obtiene el valor de casilla si es vacia aborta
    //Checa que si en la casilla a continuacion hay una ficha del enemigo
    //Checa si dos casillas adelante esta sin ninguna ficha, si esta vacia devuelve true
    //No permite a las fichas de los jugadores ir hacia su punto de partida (que no avancen las blancas para abajo y las negra para arriba)
    int dirs[4][2]={{-1,-1},{-1,1},{1,-1},{1,1}}; //Aqui revisa las 4 diagonales {-1,-1},{-1,1},{1,-1},{1,1}
    for(int d=0; d<4; d++){ //Intera en las 4 direcciones

        int df=dirs[d][0], dc=dirs[d][1]; //Obtiene el cambio de fila y columna actual

        if(pieza==1 && df>0) continue; //Bloque que las blancas avancen para abajo
        if(pieza==2 && df<0) continue; //Bloque que la negra avance para arriba

        int fm=f+df, cm=c+dc, filaDes=f+2*df, colDes=c+2*dc; // Calcula casilla donde esta enemigo y destino final

        if(filaDes<0||filaDes>=8||colDes<0||colDes>=8) continue;
        if(tablero[filaDes][colDes]!=0) continue;

        int enemigo=tablero[fm][cm];

        if(pieza==1 && (enemigo==2||enemigo==4)) return true;
        if(pieza==2 && (enemigo==1||enemigo==3)) return true;
        if(pieza==3 && (enemigo==2||enemigo==4)) return true;
        if(pieza==4 && (enemigo==1||enemigo==3)) return true;
    }
    return false;
}

//Nos ayuda a recorrer todo el tablero en busca de oportunidades de comer, si hay oportunidad obliga al jugador a hacerlo
bool ComerAFuerzas(int turno){

    for(int f=0; f<8; f++) for(int c=0; c<8; c++){

        int p=tablero[f][c];

        if(turno==1 && (p==1||p==3) && Comer(f,c)) return true;
        if(turno==2 && (p==2||p==4) && Comer(f,c)) return true;
    }
    return false;
}

//Cuenta las fichas de los jugadores y lo mantiene informado sin que el jugador este contando sus fichas
//Superviza si un jugador gana
int contarFichas(int turno){

    int count=0;

    for(int f=0; f<8; f++) for(int c=0; c<8; c++){
        if(turno==1 && (tablero[f][c]==1||tablero[f][c]==3)) count++;
        if(turno==2 && (tablero[f][c]==2||tablero[f][c]==4)) count++;
    }
    return count;
}

//Situaciones que valida
//Que la ficha avance a un espacio vacio y dentro del tablero
//Que la ficha avance en forma de diagonal (abs)
//Que el jugador no coma nomas una cuando puede comer dos (no se lo permite lo bloquea)
//Que si se mueve una casilla este correcta la dirreccion de la ficha
//Que si mueve dos casillas este un enemigo en medio de las dos casillas
bool esMovimientoValido(int filaOri, int colOri, int filaDes, int colDes, int turno){

    if(filaDes<0||filaDes>=8||colDes<0||colDes>=8) return false;
    if(tablero[filaDes][colDes]!=0) return false;
    if(abs(filaDes-filaOri)!=abs(colDes-colOri)) return false;

    bool Captura=abs(filaDes-filaOri)==2;

    if(ComerAFuerzas(turno) &&!Captura) return false;

    int pieza=tablero[filaOri][colOri];

    if(abs(filaDes-filaOri)==1){
        if(pieza==1 && filaDes>filaOri) return false;
        if(pieza==2 && filaDes<filaOri) return false;
        return true;
    }

    if(abs(filaDes-filaOri)==2){
        int fm=(filaOri+filaDes)/2, cm=(colOri+colDes)/2;
        int enemigo=tablero[fm][cm];
        
        if(turno==1 && enemigo!=2 && enemigo!=4) return false;
        if(turno==2 && enemigo!=1 && enemigo!=3) return false;
        return true;
    }
    return false;
}

//Ya con la verificacion anterior permite al jugador moverse
//Si pudo comer el jugador con salto doble borrar la ficha del enemigo que estuvo en medio del movimiento
//Coloca la ficha en el destino que selecciono el jugador
//Convierte fichas a damas (corona) si ficha blanca llega a la fila 0 se hace 3(B) y si una negra llega a a la fila 7 se hace 4(N)
void MovimientosTablero(int filaOri, int colOri, int filaDes, int colDes){
    Mov m;
    m.filaOri = filaOri; m.colOri = colOri; m.filaDes = filaDes; m.colDes = colDes;
    m.fichaMov = tablero[filaOri][colOri];
    m.fichaCom = 0;

    if(abs(filaDes-filaOri)==2){
        m.fichaCom = tablero[(filaOri+filaDes)/2][(colOri+colDes)/2];
        tablero[(filaOri+filaDes)/2][(colOri+colDes)/2] = 0;
    } 

    historial.push_back(m);
    tablero[filaDes][colDes]=tablero[filaOri][colOri];
    tablero[filaOri][colOri]=0;

    if(tablero[filaDes][colDes]==1 && filaDes==0) tablero[filaDes][colDes]=3;
    if(tablero[filaDes][colDes]==2 && filaDes==7) tablero[filaDes][colDes]=4;
}

bool TieneMovimientos(int turno){

    for(int f=0; f<8; f++) 

        for(int c=0; c<8; c++){

            int p = tablero[f][c]; //Obtiene movimientos

            //Ignora las fichas del contrincante
            if(turno==1 && (p!=1 && p!=3)){
            continue;
            } 

            if(turno==2 && (p!=2 && p!=4)){
            continue;
            } 
            
            int dirs[4][2]={{-1,-1},{-1,1},{1,-1},{1,1}}; //Revisa las 4 diagonales

            for(int d=0; d<4; d++){
                int nf=f+dirs[d][0], nc=c+dirs[d][1]; //Casilla diagonal adyacente

                if(nf>=0&&nf<8&&nc>=0&&nc<8 && tablero[nf][nc]==0) return true; //Casilla vacia en el tablero
                int nf2=f+2*dirs[d][0], nc2=c+2*dirs[d][1];//Casila de salto(doble diagonal)
                //Hay enemigo enmedio y destino libre
                if(nf2>=0&&nf2<8&&nc2>=0&&nc2<8 && tablero[nf2][nc2]==0 && tablero[nf][nc]!=0 && tablero[nf][nc]!=p) return true;
            }
        }
    return false; //Ninguna ficha se movio
}


void GuarPar(const string& archivo){
    ofstream file(archivo);
    if(!file){
        cout << "No se puede abrir el archivo\n";
        return;
    }
    file << turnoGua << "\n"; // Turno

    for(int f = 0; f < 8; f++){//Tablero completo
      for(int c = 0; c < 8; c++){ file << tablero[f][c] << " ";
      }
      file << "\n";
    }
    file << historial.size() << "\n"; //Cantidad de movimiento
    
    for(size_t i = 0; i<historial.size(); i++){ //Cada movimiento
      file << historial[i].filaOri << " " << historial[i].colOri << " "
             << historial[i].filaDes << " " << historial[i].colDes << " "
             << historial[i].fichaMov << " " << historial[i].fichaCom << "\n";
    } 
    file << tiempo[1] << " " << tiempo[2] << "\n";
    file.close();
    cout << "Partida guardada en: " << archivo << endl;
}

void CarParti(const string& archivo){
    ifstream file(archivo); //Abre archivo
    if(!file){
        cout << "No existe ninguna partida guardada\n";
        return;
    }
    historial.clear(); //Vaia el historial anterior

    file >> turnoGua; //De que turno se reanuda el juego

    //Recorre todo el tablero
    for(int f = 0; f < 8; f++){
        for(int c = 0; c < 8; c++){
            file >> tablero[f][c];
        }
    }
    int n;
    file >> n;

    for(int i = 0; i < n; i++){
        Mov m;
        file >> m.filaOri >> m.colOri >> m.filaDes >> m.colDes >> m.fichaMov >> m.fichaCom;
        historial.push_back(m);
    }
    file >> tiempo[1] >> tiempo[2];
    file.close();
    cout << "Partida cargada desde: " << archivo << endl;
}
    void MostrarHis(){
        if (historial.empty()){
            cout << "No hay movimientos guardados"<< endl;
            return;
        }
        cout << "++++++++++Movimientos guardados+++++++++"<< endl;
        for (size_t i = 0; i < historial.size(); i++){
            cout << (i + 1) << ") Origen: [" << historial[i].filaOri << "," << historial[i].colOri << "] -> Destino: [" << historial[i].filaDes << "," << historial[i].colDes << "]";
            if (historial[i].fichaCom != 0){
                cout << " [Ficha capturada " << historial[i].fichaCom <<"]";
            }
            cout << endl;
        }
        cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
    }

    void error(const string& msg){
        system("cls");
        MostrarTablero();
        cout << "\nFichas blancas: " << contarFichas(1) << "| Negras: " << contarFichas(2) << endl;
        cout << "Tiempo blancas: " << tiempo[1] << "s | Negras: " << tiempo[2] << "s\n";
        cout << "\nTurno " << (turno==1? "BLANCAS (b)":"NEGRAS(n)") << endl;
        cout << "(O) Origen | (X) Rendirse | (G) Guardar | (H) Historial " << endl;
        cout << "\n ERROR " << msg << endl;
    }

//Inica el juego
//Pone el turno=1
//Repite la accion de mostrar
//Tablero y marcador
//Revisa que un jugador quede sin fichas y el contrincante gane
//Pide el origen de la ficha que se quiere mover y su destino
//Checa que la ficha selecciona pertenezca al jugador en turno
//Al validar lo anterior mueve la ficha a deseo del jugador
//Permite al jugador seguir comiendo con la misma ficha sin cambiar de turno
//Si no se puede se cambia al otro jugador
int main(){
    char opc;
    cout << "Partida nueva (N) o partida guardada (G): ";
    cin >> opc;
    if (opc == 'G' || opc == 'g'){
        string nombre; 
        cout << "Nombre del archivo de la partida guardada: ";
        cin >> nombre;
        CarParti(nombre); //Carga el tablero y historial
    } else{
        InicializarTablero();
        historial.clear();
        turnoGua = 1;
    }
    //Se llena el tablero de 0 y despues se ponen las fichas
    turno = turnoGua;
    int filaOri,colOri,filaDes,colDes;
    char accion;

    while(true){
        MostrarTablero();
        tini = steady_clock::now(); //Empieza el tiempo
        cout << "\nFichas Blancas: "<<contarFichas(1)<<" | Negras: "<<contarFichas(2)<<endl;
        cout << "Tiempo blancas: "<< tiempo[1] << "s | Negras: " << tiempo[2] << "s\n";

        if(contarFichas(1)==0){
            cout << "\nGANAN NEGRAS\n"; break;
        }
        
        if(contarFichas(2)==0){
            cout << "\nGANAN BLANCAS\n"; break;
        }

        if(!TieneMovimientos(turno)){
        cout << "\nGANAN " << (turno==1?"NEGRAS":"BLANCAS") << " (rival sin movimientos)\n";
        break;
        }
        
        cout<<"\nTurno "<<(turno==1?"BLANCAS (b)":"NEGRAS (n)")<<endl;
        cout << "(O) Origen | (X) Rendirse | (G) Guardar | (H) Historial " <<endl;
        cin >> accion;

        int turnojug = turno; //Guarda el turno actual

        if(accion=='X' || accion=='x'){
        cout << "\nGanan " << (turno==1?"NEGRAS":"BLANCAS") << " por rendicion\n"<<endl;
        break; // Sale del while(true) y termina el juego
        }

        if(accion == 'G' || accion == 'g'){
            string nombre;
            cout << "Nombre del archivo guardado: ";
            cin >> nombre;
            GuarPar(nombre);
            continue;
        }

        if(accion == 'H' || accion == 'h'){
            MostrarHis();
            continue;
        }

        if(accion != 'O' && accion != 'o'){
            error("Opcion incorrecta\n");
            continue;
        }

        cout << "Origen fila y columna *[4 3]*: ";
        cin >> filaOri >> colOri;

        if(turno==1 && tablero[filaOri][colOri]!=1 && tablero[filaOri][colOri]!=3){
            error("Esa ficha blanca no es tuya, vuelve a seleccionar\n");
            continue;
        }

        if(turno==2 && tablero[filaOri][colOri]!=2 && tablero[filaOri][colOri]!=4){
            error("Esa ficha negra no es tuya, vuelve a seleccionar\n");
            continue;
        }

         char dir;
         cout << "(I) Izquierda o (D) Derecha: "<<endl;
         cin >> dir;

        int sent = 1;
        if (tablero[filaOri][colOri]==3 || tablero[filaOri][colOri]==4){
            char s;
            cout << "Es una dama. (A) Avanzar o (R) Retroceder"<<endl;
            cin >> s;
            if(s=='R' || s=='r') sent = -1;
        }

        int fm = (turno == 1) ? (filaOri + (sent * -1)) : (filaOri + (sent * 1));
        int cm_Izq = colOri - 1;
        int cm_Der = colOri + 1;
        int enem = 0;

        if (dir == 'I' || dir == 'i'){
            if (fm >= 0 && fm < 8 && cm_Izq >= 0 && cm_Izq < 8){
                enem = tablero[fm][cm_Izq];
            }
         } else if (dir == 'D' || dir == 'd'){
            if (fm >= 0 && fm < 8 && cm_Der >= 0 && cm_Der < 8){
                enem = tablero[fm][cm_Der];
            }
         } else {
            error("Direccion incorrecta\n");
            continue;
         }         

        int salto = (enem != 0) ? 2 : 1;

        if (turno == 1){
           filaDes = filaOri - (salto * sent);
        } else {
            filaDes = filaOri + (salto * sent);
        }

        if (dir == 'I' || dir == 'i'){
            colDes = colOri - salto;
        } else {
            colDes = colOri + salto;
        }

        if (filaDes < 0 || filaDes >= 8 || colDes < 0 || colDes >= 8){
            error("Moviento fuera del tablero\n");
            continue;
        }

        if (enem == 0 && tablero[filaDes][colDes] != 0){
        error("Espacio ocupado\n");
        continue;
        }

        if(esMovimientoValido(filaOri,colOri,filaDes,colDes,turno)){
            MovimientosTablero(filaOri,colOri,filaDes,colDes);

            turno = (turno == 1)?2:1;
            turnoGua = turno;

            ActuTabl();

            if(abs(filaDes-filaOri)==2 && Comer(filaDes,colDes)){
                cout<<"¡¡Continua comiendo con la misma ficha!!\n"<<endl;
                continue;
            }
            
        }else {
            error("Lo siento, no se puede realizar el movimiento deseado\n");
        }
        int gastado = (int)duration_cast<seconds> (steady_clock::now()-tini).count();
        tiempo[turnojug] -= gastado;
        if(tiempo[turnojug] <= 0){
            cout << "\nSE ACABO EL TIEMPO. GANAN " << (turnojug == 1?"NEGRAS":"BLANCAS") << endl;
            break;
        }
    }
    return 0;
}
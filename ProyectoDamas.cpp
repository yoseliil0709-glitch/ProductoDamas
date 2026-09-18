#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

//Defeni el tablero (el tamaño)
int tablero[8][8];
//Diseño 0=Vacio, 1=blanca (b), 2=negra (n), 3=Dama blanca (B), 4=Dama negra (N)
void InicializarTablero(){
    for(int f=0; f<8; f++) for(int c=0; c<8; c++) tablero[f][c]=0;
    for(int f=0; f<3; f++) for(int c=0; c<8; c++) if((f+c)%2==1) tablero[f][c]=2;
    for(int f=5; f<8; f++) for(int c=0; c<8; c++) if((f+c)%2==1) tablero[f][c]=1;
}

void MostrarTablero(){
    //Diseño del tablero (colores, numeros de coordenadas, posicion de las fichas)
    string RESET="\033[0m";
    string FONDOCLA="\033[47m";
    string FONDOOSC="\033[100m";
    string B="\033[97;1m";
    string N="\033[30;1m";
    string R="\033[91;1m";
    string A="\033[93;1m";

    cout<<"\n------ 0   1   2   3   4   5   6   7---\n";
    for(int f=0; f<8; f++){
        cout << " . " << f << " . ";
        for(int c=0; c<8; c++){
            string fondo = ((f+c)%2==0)? FONDOCLA:FONDOOSC;

            if(tablero[f][c]==0) cout << fondo << " - " << RESET;
            else if(tablero[f][c]==1) cout << fondo << B << " b " << RESET;
            else if(tablero[f][c]==2) cout << fondo << N << " n " << RESET;
            else if(tablero[f][c]==3) cout << fondo << A << " B " << RESET;
            else if(tablero[f][c]==4) cout << fondo << R << " N " << RESET;
        }
        cout << " " << f << endl;
    }
    cout << " 0 1 2 3 4 5 6 7\n";
}

bool Comer(int f, int c){
    //Verifica que la ficha en la columna y fila pueda comer fichas del contrincante
    int pieza=tablero[f][c]; if(pieza==0) return false;
    //Aqui revisa las 4 diagonales {-1,-1},{-1,1},{1,-1},{1,1}
    //Checa que si en la casilla a continuacion hay una ficha del enemigo
    //Checa si dos casillas adelante esta sin ninguna ficha, si esta vacia devuelve true
    //No permite a las fichas de los jugadores ir hacia su punto de partida (que no avancen las blancas para abajo y las negra para arriba)
    int dirs[4][2]={{-1,-1},{-1,1},{1,-1},{1,1}};
    for(int d=0; d<4; d++){
        int df=dirs[d][0], dc=dirs[d][1];
        if(pieza==1 && df>0) continue;
        if(pieza==2 && df<0) continue;
        int fm=f+df, cm=c+dc, fd=f+2*df, cd=c+2*dc;
        if(fd<0||fd>=8||cd<0||cd>=8) continue;
        if(tablero[fd][cd]!=0) continue;
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
bool esMovimientoValido(int fo, int co, int fd, int cd, int turno){
    if(fd<0||fd>=8||cd<0||cd>=8) return false;
    if(tablero[fd][cd]!=0) return false;
    if(abs(fd-fo)!=abs(cd-co)) return false;
    bool Captura=abs(fd-fo)==2;
    if(ComerAFuerzas(turno) &&!Captura) return false;
    int pieza=tablero[fo][co];
    if(abs(fd-fo)==1){
        if(pieza==1 && fd>fo) return false;
        if(pieza==2 && fd<fo) return false;
        return true;
    }

    if(abs(fd-fo)==2){
        int fm=(fo+fd)/2, cm=(co+cd)/2;
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
void MovimientosTablero(int fo, int co, int fd, int cd){
    if(abs(fd-fo)==2) tablero[(fo+fd)/2][(co+cd)/2]=0;
    tablero[fd][cd]=tablero[fo][co];
    tablero[fo][co]=0;

    if(tablero[fd][cd]==1 && fd==0) tablero[fd][cd]=3;
    if(tablero[fd][cd]==2 && fd==7) tablero[fd][cd]=4;
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
    //Se llena el tablero de 0 y despues se ponen las fichas
    InicializarTablero(); int turno=1, fo,co,fd,cd;
    while(true){
        MostrarTablero();
        cout<<"\nFichas Blancas: "<<contarFichas(1)<<" | Negras: "<<contarFichas(2)<<endl;

        if(contarFichas(1)==0){cout<<"\nGANAN NEGRAS\n"; break;}
        if(contarFichas(2)==0){cout<<"\nGANAN BLANCAS\n"; break;}

        cout<<"\nTurno "<<(turno==1?"BLANCAS (b)":"NEGRAS (n)")<<endl;
        cout<<"Origen fila y columna ejem *[4 3]*: "; cin>>fo>>co;
        cout<<"Destino fila y columna ejem *[4 3]*: "; cin>>fd>>cd;

        if(fo<0||fo>=8||co<0||co>=8){cout<<"Origen fuera\n"; continue;}

        if(turno==1 && tablero[fo][co]!=1 && tablero[fo][co]!=3){cout<<"Esa ficha blanca no es tuya, vuelve a seleccionar\n"; continue;}
        if(turno==2 && tablero[fo][co]!=4){cout<<"Esa ficha negra no es tuya, vuelve a seleccionar\n"; continue;}

        if(esMovimientoValido(fo,co,fd,cd,turno)){
            MovimientosTablero(fo,co,fd,cd);
            if(abs(fd-fo)==2 && Comer(fd,cd)){cout<<"¡¡Continua comiendo con la misma ficha!!\n"; continue;}
            turno=(turno==1)?2:1;
        }else cout<<"Lo siento, no se puede realizar el movimiento deseado\n";
    }
    return 0;
}
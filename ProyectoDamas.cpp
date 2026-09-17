#include <iostream>
#include <cstdlib> // para abs()
using namespace std;

int tablero[8][8];
void InicializarTablero(){
    for(int f=0; f<8; f++)
        for(int c=0; c<8; c++)
            tablero[f][c] = 0;

    // negras arriba
    for(int f=0; f<3; f++)
        for(int c=0; c<8; c++)
            if((f+c)%2==1) tablero[f][c]=2;

    // blancas abajo
    for(int f=5; f<8; f++)
        for(int c=0; c<8; c++)
            if((f+c)%2==1) tablero[f][c]=1;
}

void MostrarTablero(){
    cout << "\n  0 1 2 3 4 5 6 7\n";
    for(int f=0; f<8; f++){
        cout << f << " ";
        for(int c=0; c<8; c++){
            if(tablero[f][c]==0) cout << ". ";
            else if(tablero[f][c]==1) cout << "b ";
            else if(tablero[f][c]==2) cout << "n ";
            else if(tablero[f][c]==3) cout << "B ";
            else if(tablero[f][c]==4) cout << "N ";
        }
        cout << endl;
    }
}

bool esMovimientoValido(int fo, int co, int fd, int cd, int turno){
    if(fd<0||fd>=8||cd<0||cd>=8) return false;
    if(tablero[fd][cd]!=0) return false;
    if(abs(fd-fo)!=abs(cd-co)) return false;

    int pieza = tablero[fo][co];

    // movimiento de 1
    if(abs(fd-fo)==1){
        if(pieza==1 && fd>fo) return false; // blanca solo sube
        if(pieza==2 && fd<fo) return false; // negra solo baja
        return true;
    }
    // movimiento de 2 (comer)
    if(abs(fd-fo)==2){
        int fm=(fo+fd)/2;
        int cm=(co+cd)/2;
        int enemigo=tablero[fm][cm];
        if(turno==1 && enemigo!=2 && enemigo!=4) return false;
        if(turno==2 && enemigo!=1 && enemigo!=3) return false;
        return true;
    }
    return false;
}

void MovimientosTablero(int fo, int co, int fd, int cd){
    if(abs(fd-fo)==2){
        tablero[(fo+fd)/2][(co+cd)/2]=0; // borra la que comiste
    }
    tablero[fd][cd]=tablero[fo][co];
    tablero[fo][co]=0;

    // coronar
    if(tablero[fd][cd]==1 && fd==0) tablero[fd][cd]=3;
    if(tablero[fd][cd]==2 && fd==7) tablero[fd][cd]=4;
}

    bool Comer(int f, int c){
        int pieza = tablero[f][c];
        if (pieza==0) return false; //Revisa 4 diagonales
        int dirs[4][2] = {{-1,-1},{-1,1},{1,-1},{1,1}};
         for(int d=0; d<4; d++){
             int df=dirs[d][0];
             int dc=dirs[d][1];
             if(pieza==1 && df>0) continue; //Impide que blanca avance a abajo
             if(pieza==2 && df<0) continue; //Impide que negra avance a arriba
             int fm=f+df, cm=c+dc; //Enemigo
             int fd=f+2*df, cd=c+2*dc;//Salto
                     if(fd<0||fd>=8||cd<0||cd>=8) continue;
        if(tablero[fd][cd]!=0) continue;

        int enemigo = tablero[fm][cm];
        if(pieza==1 && (enemigo==2||enemigo==4)) return true;
        if(pieza==2 && (enemigo==1||enemigo==3)) return true;
        if(pieza==3 && (enemigo==2||enemigo==4)) return true; // Dama
        if(pieza==4 && (enemigo==1||enemigo==3)) return true; // Dama
    }
    return false;
}

int main(){
    InicializarTablero();
    int turno = 1;
    int fo, co, fd, cd;

    while(true){
        MostrarTablero();
        cout << "\nTurno " << (turno==1? "BLANCAS (b)" : "NEGRAS (n)") << endl;
        cout << "Origen fila col: ";
        cin >> fo >> co;
        cout << "Destino fila col: ";
        cin >> fd >> cd;

        if(fo<0||fo>=8||co<0||co>=8){
            cout << "Origen fuera del tablero\n"; continue;
        }
        // que sea tu ficha
        if(turno==1 && tablero[fo][co]!=1 && tablero[fo][co]!=3){
            cout << "Esa no es una blanca tuya\n"; continue;
        }
        if(turno==2 && tablero[fo][co]!=2 && tablero[fo][co]!=4){
            cout << "Esa no es una negra tuya\n"; continue;
        }

        if(esMovimientoValido(fo, co, fd, cd, turno)){
            MovimientosTablero(fo, co, fd, cd);
            turno = (turno==1)? 2 : 1;
        } else {
            cout << "No se puede realizar ese movimiento\n";
        }
    }
    return 0;
}
#include <iostream>
#include <cstdlib>
using namespace std;

int tablero[8][8];

void InicializarTablero(){
    for(int f=0; f<8; f++) for(int c=0; c<8; c++) tablero[f][c]=0;
    for(int f=0; f<3; f++) for(int c=0; c<8; c++) if((f+c)%2==1) tablero[f][c]=2;
    for(int f=5; f<8; f++) for(int c=0; c<8; c++) if((f+c)%2==1) tablero[f][c]=1;
}

void MostrarTablero(){
    string RESET="\033[0m";
    string FONDOCLA="\033[47m";
    string FONDOOSC="\033[100m";
    string B="\033[97;1m";
    string N="\033[30;1m";
    string R="\033[91;1m";
    string A="\033[93;1m";
    cout<<"\n     0 1 2 3 4 5 6 7\n";
    for(int f=0; f<8; f++){
        cout<<f<<" |";
        for(int c=0; c<8; c++){
            string fondo = ((f+c)%2==0)? FONDOCLA:FONDOOSC;
            if(tablero[f][c]==0) cout <<fondo << " " << RESET;
            else if(tablero[f][c]==1) cout << fondo << B << " b " << RESET;
            else if(tablero[f][c]==2) cout << fondo << N << " n " << RESET;
            else if(tablero[f][c]==3) cout << fondo << A << " B " << RESET;
            else if(tablero[f][c]==4) cout << fondo << R << " N " << RESET;
        }
        cout << "| " << f << "\n";
    }
    cout << " 0 1 2 3 4 5 6 7\n";
}

bool Comer(int f, int c){
    int pieza=tablero[f][c]; if(pieza==0) return false;
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

bool ComerAFuerzas(int turno){
    for(int f=0; f<8; f++) for(int c=0; c<8; c++){
        int p=tablero[f][c];
        if(turno==1 && (p==1||p==3) && Comer(f,c)) return true;
        if(turno==2 && (p==2||p==4) && Comer(f,c)) return true;
    }
    return false;
}

int contarFichas(int turno){
    int count=0;
    for(int f=0; f<8; f++) for(int c=0; c<8; c++){
        if(turno==1 && (tablero[f][c]==1||tablero[f][c]==3)) count++;
        if(turno==2 && (tablero[f][c]==2||tablero[f][c]==4)) count++;
    }
    return count;
}

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

void MovimientosTablero(int fo, int co, int fd, int cd){
    if(abs(fd-fo)==2) tablero[(fo+fd)/2][(co+cd)/2]=0;
    tablero[fd][cd]=tablero[fo][co];
    tablero[fo][co]=0;
    if(tablero[fd][cd]==1 && fd==0) tablero[fd][cd]=3;
    if(tablero[fd][cd]==2 && fd==7) tablero[fd][cd]=4;
}

int main(){
    InicializarTablero(); int turno=1, fo,co,fd,cd;
    while(true){
        MostrarTablero();
        cout<<"\nFichas Blancas: "<<contarFichas(1)<<" | Negras: "<<contarFichas(2)<<endl;
        if(contarFichas(1)==0){cout<<"\nGANAN NEGRAS\n"; break;}
        if(contarFichas(2)==0){cout<<"\nGANAN BLANCAS\n"; break;}
        cout<<"\nTurno "<<(turno==1?"BLANCAS (b)":"NEGRAS (n)")<<endl;
        cout<<"Origen fila col: "; cin>>fo>>co;
        cout<<"Destino fila col: "; cin>>fd>>cd;
        if(fo<0||fo>=8||co<0||co>=8){cout<<"Origen fuera\n"; continue;}
        if(turno==1 && tablero[fo][co]!=1 && tablero[fo][co]!=3){cout<<"Esa no es blanca tuya\n"; continue;}
        if(turno==2 && tablero[fo][co]!=4){cout<<"Esa no es negra tuya\n"; continue;}
        if(esMovimientoValido(fo,co,fd,cd,turno)){
            MovimientosTablero(fo,co,fd,cd);
            if(abs(fd-fo)==2 && Comer(fd,cd)){cout<<"¡Sigue comiendo con la misma ficha!\n"; continue;}
            turno=(turno==1)?2:1;
        }else cout<<"No se puede realizar ese movimiento\n";
    }
    return 0;
}
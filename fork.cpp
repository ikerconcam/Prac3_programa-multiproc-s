#include <iostream>
#include <unistd.h>  // Per a fork() i pipe()
#include <sys/wait.h> // Per a wait()
#include <cstdlib>   // Per a srand() i rand()
#include <ctime>     // Per a time()

using namespace std;

int main() {
    int numProcessos;
    cout << "Introdueix la quantitat de processos que vols crear: ";
    cin >> numProcessos;

    // Creem un array per guardar els noms (fins a 100 noms màxim)
    string noms[100];
    int numNoms = 0;

    // Creem un pipe per comunicar dades entre processos fills i el pare
    int fd[2];
    if (pipe(fd) == -1) {
        cerr << "Error al crear el pipe" << endl;
        return 1;
    }

    // Inicialitzem la llavor per a la generació aleatòria
    srand(time(nullptr));

    for (int i = 0; i < numProcessos; ++i) {
        pid_t pid = fork();  // Creem un nou procés

        if (pid == 0) {  // Procés fill
            close(fd[0]);  // Tancar la lectura del pipe

            string nom;
            cout << "Procés fill " << getpid() << ": introdueix un nom: ";
            cin >> nom;

            // Enviar el nom al procés pare a través del pipe
            write(fd[1], nom.c_str(), nom.length() + 1);  // +1 per incloure el '\0'

            close(fd[1]);  // Tancar l'escriptura del pipe
            exit(0);  // El procés fill ha finalitzat
        } else if (pid < 0) {
            cerr << "Error al crear el procés." << endl;
            return 1;
        }
    }

    // El procés pare espera que tots els fills acabin i recull els noms
    close(fd[1]);  // Tancar l'escriptura del pipe al procés pare
    char buffer[100];
    
    while (read(fd[0], buffer, sizeof(buffer)) > 0) {
        noms[numNoms++] = string(buffer);
    }

    close(fd[0]);  // Tancar la lectura del pipe

    // Esperar que tots els processos fills acabin
    for (int i = 0; i < numProcessos; ++i) {
        wait(nullptr);
    }

    // Triem un nom aleatori i el mostrem
    if (numNoms > 0) {
        int indexAleatori = rand() % numNoms;
        cout << "Nom seleccionat aleatoriament: " << noms[indexAleatori] << endl;
    } else {
        cout << "No hi ha noms per seleccionar." << endl;
    }

    // Mostrem tots els noms guardats
    cout << "Noms introduïts: ";
    for (int i = 0; i < numNoms; ++i) {
        cout << noms[i] << " ";
    }
    cout << endl;

    return 0;
}

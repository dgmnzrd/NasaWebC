#include "server.h"
#include <stdio.h>
#include <stdlib.h>

// Opcional: puedes inicializar aquí logs, configs, etc.
int main() {
    const char *api_key = getenv("API_KEY");

    if (!api_key) {
        fprintf(stderr, "Error: La variable de entorno API_KEY no está definida.\n");
        return EXIT_FAILURE;
    }

    printf("API_KEY detectada. Iniciando servidor...\n");
    start_server();

    return EXIT_SUCCESS;
}
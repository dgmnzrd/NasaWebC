# NasaWebC 🚀

Proyecto web escrito en C que consume la API pública de la NASA para mostrar información como la Imagen Astronómica del Día (APOD). Utiliza `libcurl` para hacer solicitudes HTTP, `cJSON` para parsear JSON, y `Mongoose` como servidor embebido.

## Estructura

- `src/`: Código fuente en C
- `include/`: Archivos de encabezado
- `public/`: Archivos del frontend (HTML, CSS, JS)
- `libs/`: Librerías externas (`cJSON`, `mongoose`)
- `.env`: Clave de API (no se sube a Git)

## Compilación

```bash
gmake
./nasa_server
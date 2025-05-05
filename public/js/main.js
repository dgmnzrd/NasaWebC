document.addEventListener("DOMContentLoaded", () => {
    fetch("/apod")
        .then(response => {
            if (!response.ok) throw new Error("Fallo al obtener APOD");
            return response.json();
        })
        .then(data => {
            console.log("Respuesta recibida:", data);

            if (!data.url) throw new Error("La respuesta no contiene 'url'");

            // Imagen principal en el cuerpo
            const imageMain = document.getElementById("apod-image");
            if (imageMain) imageMain.src = data.url;

            // Imagen en el sidebar
            const imageSidebar = document.getElementById("sidebar-image");
            if (imageSidebar) imageSidebar.src = data.url;

            // Título, fecha y explicación en el sidebar
            const title = document.getElementById("sidebar-title");
            const date = document.getElementById("sidebar-date");
            const explanation = document.getElementById("sidebar-explanation");

            if (title && date && explanation) {
                title.textContent = data.title || "(Sin título)";
                date.textContent = data.date || "";
                explanation.textContent = data.explanation || "Sin descripción disponible.";
            } else {
                console.warn("No se encontraron elementos del sidebar para mostrar información.");
            }
        })
        .catch(error => {
            console.error("Error al cargar APOD:", error);
            const container = document.getElementById("apod-container");
            if (container) {
                container.innerHTML = "<p class='text-red-500'>Error al cargar la Imagen del Día.</p>";
            }
        });
});
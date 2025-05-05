document.addEventListener("DOMContentLoaded", () => {
    const btn = document.getElementById("loadWelcome");
    const output = document.getElementById("output");

    btn.addEventListener("click", async () => {
        try {
            const res = await fetch("/apod");
            const data = await res.json();
            output.textContent = JSON.stringify(data, null, 2);
        } catch (err) {
            output.textContent = "Error al conectar con el backend.";
        }
    });
});
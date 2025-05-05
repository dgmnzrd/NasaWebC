document.addEventListener("DOMContentLoaded", () => {
    // Cargar header
    fetch("/components/header.html")
        .then(res => res.text())
        .then(html => {
            document.getElementById("header").innerHTML = html;
        });

    // Cargar footer
    fetch("/components/footer.html")
        .then(res => res.text())
        .then(html => {
            document.getElementById("footer").innerHTML = html;
        });
});
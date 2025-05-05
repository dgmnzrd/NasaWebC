document.addEventListener("DOMContentLoaded", () => {
    const apodSection = document.getElementById("apod-section");
    if (apodSection) {
        apodSection.classList.replace("opacity-0", "opacity-100");
    }

    const sidebar = document.getElementById("sidebar-apod");

    if (sidebar) {
        sidebar.addEventListener("mouseenter", () => {
            sidebar.classList.remove("translate-x-full");
            sidebar.classList.add("translate-x-0");
        });

        sidebar.addEventListener("mouseleave", () => {
            sidebar.classList.remove("translate-x-0");
            sidebar.classList.add("translate-x-full");
        });
    }
});
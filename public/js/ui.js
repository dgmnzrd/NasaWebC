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

    const modal = document.getElementById("imageModal");
    const modalImage = document.getElementById("modalImage");
    const sidebarImage = document.getElementById("sidebar-image");

    if (sidebarImage && modal && modalImage) {
        sidebarImage.addEventListener("click", () => {
            modalImage.src = sidebarImage.src;
            modal.classList.remove("hidden");
        });

        modal.addEventListener("click", () => {
            modal.classList.add("hidden");
            modalImage.src = "";
        });
    }
});
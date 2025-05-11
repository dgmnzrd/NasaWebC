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
            modal.style.display = "flex";  // Mostrar modal
        });

        modal.addEventListener("click", () => {
            modal.style.display = "none";  // Ocultar modal
            modalImage.src = "";
        });
    }

    const headerEl = document.getElementById('header');
    const sidebarEl = document.getElementById('sidebar-apod');

    if (headerEl && sidebarEl) {
        // Observamos el header con threshold = 1 para saber cuando deja de verse COMPLETO
        const observer = new IntersectionObserver(([entry]) => {
            if (entry.intersectionRatio < 1) {
                // header ya no está 100% visible → fijamos el sidebar
                sidebarEl.classList.remove('absolute');
                sidebarEl.classList.add('sidebar-fixed');
            } else {
                // header completamente visible → volvemos al comportamiento normal
                sidebarEl.classList.add('absolute');
                sidebarEl.classList.remove('sidebar-fixed');
            }
        }, { threshold: [1] });

        observer.observe(headerEl);
    }
});
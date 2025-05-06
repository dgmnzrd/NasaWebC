document.addEventListener("DOMContentLoaded", () => {
    // === CARGA APOD ===
    fetch("/apod")
        .then(res => res.ok ? res.json() : Promise.reject("Error"))
        .then(data => {
            document.getElementById("sidebar-image").src = data.url || "";
            document.getElementById("sidebar-title").textContent = data.title || "(Sin título)";
            document.getElementById("sidebar-date").textContent = data.date || "";
            document.getElementById("sidebar-explanation").textContent = data.explanation || "Sin descripción.";
        });

    const donkiButtons = document.querySelectorAll(".donki-btn");
    const donkiCarousel = document.getElementById("donki-carousel");

    const getFormattedDate = (offsetDays = 0) => {
        const d = new Date();
        d.setDate(d.getDate() - offsetDays);
        return d.toISOString().split("T")[0];
    };

    let scrollInterval;
    let currentScrollIndex = 0;

    const startAutoScroll = () => {
        stopAutoScroll();
        scrollInterval = setInterval(() => {
            const cards = donkiCarousel.querySelectorAll(".donki-card");
            if (!cards.length) return;

            currentScrollIndex = (currentScrollIndex + 2) % cards.length;
            const targetCard = cards[currentScrollIndex];
            if (targetCard) {
                targetCard.scrollIntoView({ behavior: "smooth", block: "start" });
            }
        }, 4000);
    };

    const stopAutoScroll = () => {
        if (scrollInterval) clearInterval(scrollInterval);
    };

    donkiCarousel.addEventListener("mouseenter", stopAutoScroll);
    donkiCarousel.addEventListener("mouseleave", startAutoScroll);

    const fetchDonki = (type) => {
        donkiCarousel.innerHTML = `<div class="text-gray-500 italic px-2 text-sm">Loading...</div>`;
    
        const start = getFormattedDate(30);
        const end = getFormattedDate(0);
        const url = `/donki?type=${type}&startDate=${start}&endDate=${end}`;
        const isAnalysis = type === "CMEAnalysis";
    
        fetch(url)
            .then(res => res.ok ? res.json() : Promise.reject("Error"))
            .then(data => {
                const processed = isAnalysis
                    ? data.map(item => ({
                        id: item.link?.split("/").slice(-2, -1)[0] || "N/A",
                        note: item.note || "No description available.",
                        time: item.time21_5 || item.submissionTime || "Unknown"
                    }))
                    : (data || []).filter(e => e.note && e.note.trim() !== "").map(e => ({
                        id: e.activityID || e.flrID || e.sepID || "N/A",
                        note: e.note,
                        time: e.startTime || e.eventTime || e.beginTime || "Unknown"
                    }));
    
                if (!processed.length) {
                    donkiCarousel.innerHTML = `<div class="text-red-500 italic px-2 text-sm">No events with descriptions found.</div>`;
                    return;
                }
    
                donkiCarousel.innerHTML = processed.map(ev => `
                    <div class="donki-card snap-start overflow-hidden bg-white border border-gray-200 rounded-md shadow-sm p-3 text-sm leading-snug">
                        <h3 class="font-semibold text-blue-800 text-xs mb-1 truncate">ID: ${ev.id}</h3>
                        <p class="text-gray-700 text-xs truncate">${ev.note}</p>
                        <p class="text-gray-500 text-[11px] mt-1">Start Time: ${ev.time}</p>
                    </div>
                `).join("");
    
                requestAnimationFrame(() => {
                    const cards = [...donkiCarousel.querySelectorAll(".donki-card")];
                    const heights = cards.slice(0, 2).map(c => c.offsetHeight);
                    const maxHeight = heights.reduce((a, b) => a + b, 0) + 12;
                    donkiCarousel.style.maxHeight = maxHeight + "px";
    
                    currentScrollIndex = 0;
                    startAutoScroll();
                });
            })
            .catch(err => {
                console.error("DONKI error:", err);
                donkiCarousel.innerHTML = `<div class="text-red-500 italic px-2 text-sm">Error loading data.</div>`;
            });
    };

    donkiButtons.forEach(btn => {
        btn.addEventListener("click", () => {
            fetchDonki(btn.getAttribute("data-type"));
        });
    });

    fetchDonki("CME");
});

// Desactiva scroll horizontal si se desborda
window.addEventListener("resize", () => {
    document.body.style.overflowX = "hidden";
});
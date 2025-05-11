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
            const container = donkiCarousel;
            const cards = container.querySelectorAll(".donki-card");
            if (!cards.length) return;

            currentScrollIndex = (currentScrollIndex + 2) % cards.length;
            const targetCard = cards[currentScrollIndex];
            if (targetCard) {
                container.scrollTop = targetCard.offsetTop - container.offsetTop;
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

    // === CARGA DE GALERÍA MARS ROVER CON RETROCESO ===
    const fetchRoverPhotos = async () => {
        const gallery = document.getElementById("rover-gallery");
        gallery.innerHTML = `<div class="text-sm italic text-gray-500">Loading Mars Rover photos...</div>`;

        const formatDate = (offset = 0) => {
            const d = new Date();
            d.setDate(d.getDate() - offset);
            return d.toISOString().split("T")[0];
        };

        const fetchDayPhotos = async (date) => {
            try {
                const res = await fetch(`/mars?earth_date=${date}`);
                if (!res.ok) return [];
                const data = await res.json();
                return data.photos || [];
            } catch {
                return [];
            }
        };

        let allPhotos = [];
        const usedImgSrc = new Set();
        const usedCameraPerDay = new Map(); // key: date, value: Set(camera.name)

        for (let i = 0; i < 30 && allPhotos.length < 100; i++) {
            const date = formatDate(i);
            const dailyPhotos = await fetchDayPhotos(date);
            if (!dailyPhotos.length) continue;

            if (!usedCameraPerDay.has(date)) usedCameraPerDay.set(date, new Set());

            for (const photo of dailyPhotos) {
                const camera = photo.camera?.name || "";
                const src = photo.img_src;

                const camerasUsed = usedCameraPerDay.get(date);
                if (!usedImgSrc.has(src) && !camerasUsed.has(camera)) {
                    usedImgSrc.add(src);
                    camerasUsed.add(camera);
                    allPhotos.push({ ...photo, date });
                }

                if (allPhotos.length >= 100) break;
            }
        }

        if (!allPhotos.length) {
            gallery.innerHTML = `<p class="text-sm text-red-500 italic">No Mars Rover photos found in the last 30 days.</p>`;
            return;
        }

        let currentPage = 1;
        const itemsPerPage = 6;
        const totalPages = Math.ceil(allPhotos.length / itemsPerPage);

        const renderPage = (page) => {
            const start = (page - 1) * itemsPerPage;
            const photosToShow = allPhotos.slice(start, start + itemsPerPage);

            const getPageButtons = () => {
                const pages = [];
                const maxVisible = 5;
                let startPage = Math.max(1, page - Math.floor(maxVisible / 2));
                let endPage = startPage + maxVisible - 1;

                if (endPage > totalPages) {
                    endPage = totalPages;
                    startPage = Math.max(1, endPage - maxVisible + 1);
                }

                if (page > 1) {
                    pages.push(`<button class="page-btn" data-page="${page - 1}">«</button>`);
                }

                for (let i = startPage; i <= endPage; i++) {
                    pages.push(`<button class="page-btn ${i === page ? 'active' : ''}" data-page="${i}">${i}</button>`);
                }

                if (page < totalPages) {
                    pages.push(`<button class="page-btn" data-page="${page + 1}">»</button>`);
                }

                return pages.join("");
            };

            const earliest = allPhotos[allPhotos.length - 1]?.earth_date || "N/A";
            const latest = allPhotos[0]?.earth_date || "N/A";

            gallery.innerHTML = `
                <div class="grid grid-cols-2 sm:grid-cols-3 gap-3 mb-4">
                    ${photosToShow.map(photo => `
                        <div class="rounded overflow-hidden shadow-sm border border-gray-200 flex flex-col">
                            <div class="w-full aspect-[4/3] overflow-hidden bg-gray-100 flex items-center justify-center">
                                <img src="${photo.img_src}" alt="Rover" class="object-contain max-h-full max-w-full" loading="lazy" />
                            </div>
                            <div class="text-[11px] text-gray-600 px-2 py-1 border-t">
                                ${photo.camera.full_name || photo.camera.name}
                            </div>
                        </div>
                    `).join("")}
                </div>
                <div class="pagination-container">${getPageButtons()}</div>
                <p class="text-xs text-gray-400 italic mt-2 text-center">Photos from: ${earliest} to ${latest}</p>
            `;

            gallery.querySelectorAll("button[data-page]").forEach(btn => {
                btn.addEventListener("click", () => {
                    const selected = parseInt(btn.dataset.page);
                    if (!isNaN(selected)) {
                        currentPage = selected;
                        renderPage(currentPage);
                    }
                });
            });
        };

        renderPage(currentPage);
    };

    fetchRoverPhotos();

    // === CARGA DE VIDEOS NASA DESDE /videos ===
    // === CARGA DE VIDEOS NASA DESDE /videos ===
const loadVideoPreviews = async () => {
    const container = document.getElementById("video-list");
    if (!container) return;

    container.innerHTML = '<p class="text-xs italic text-gray-500">Loading videos...</p>';

    try {
        const res = await fetch("/search?q=space");
        if (!res.ok) throw new Error("Failed");

        const data = await res.json();
        const items = data.collection?.items || [];

        // ✅ Filtra solo los que NO tienen espacios en el nasa_id
        const filtered = items.filter(item => {
            const id = item.data?.[0]?.nasa_id || "";
            return id && !id.includes(" ");
        });

        if (!filtered.length) {
            container.innerHTML = '<p class="text-xs italic text-red-500">No videos with valid IDs found.</p>';
            return;
        }

        container.innerHTML = filtered.slice(0, 6).map(item => {
            const preview = item.links?.find(l => l.render === "image")?.href;
            const title = item.data?.[0]?.title || "Untitled";
            const id = item.data?.[0]?.nasa_id || "";

            return `
                <div onclick="window.location.href='/video.html?id=${encodeURIComponent(id)}'"
                    class="cursor-pointer bg-white border border-gray-200 rounded-lg shadow-sm overflow-hidden hover:shadow-md transition">
                    <img src="${preview}" alt="${title}" class="w-full h-40 object-cover" />
                    <div class="p-2 text-sm font-medium text-gray-700 truncate">${title}</div>
                </div>
            `;
        }).join("");

    } catch (err) {
        container.innerHTML = '<p class="text-xs italic text-red-500">Error loading video previews.</p>';
    }
};

    loadVideoPreviews();
});

// Desactiva scroll horizontal si se desborda
window.addEventListener("resize", () => {
    document.body.style.overflowX = "hidden";
});
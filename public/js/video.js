document.addEventListener("DOMContentLoaded", async () => {
    const params = new URLSearchParams(window.location.search);
    const nasaId = params.get("id");
    if (!nasaId) return;

    try {
        // 1. Carga del video principal
        const res = await fetch(`/video?id=${encodeURIComponent(nasaId)}`);
        if (!res.ok) throw new Error("Fetch failed");
        const data = await res.json();

        // 2. Carga del título y descripción desde /search
        let title = "NASA Video";
        let description = "No description";

        const searchRes = await fetch(`/search?q=${encodeURIComponent(nasaId)}&media_type=video`);
        if (searchRes.ok) {
            const searchData = await searchRes.json();
            const items = searchData?.collection?.items || [];
            const match = items.find(i => i.data?.[0]?.nasa_id === nasaId);
            if (match) {
                title = match.data?.[0]?.title || title;
                description = match.data?.[0]?.description || description;
            }
        }

        // 3. Render principal
        document.getElementById("video-container").innerHTML = `
            <video controls class="w-full rounded" src="${data.video_url}"></video>
        `;
        document.getElementById("video-title").textContent = title;
        document.getElementById("video-description").textContent = description;

        // 4. Sugerencias aleatorias sin espacios
        const suggestedRes = await fetch("/search?q=space&media_type=video");
        if (suggestedRes.ok) {
            const suggestedData = await suggestedRes.json();
            const suggestedItems = suggestedData.collection?.items || [];

            // Filtrar y mezclar
            const validSuggestions = suggestedItems
                .filter(item => {
                    const id = item.data?.[0]?.nasa_id || "";
                    return id !== nasaId && !id.includes(" ");
                });

            const shuffled = validSuggestions.sort(() => 0.5 - Math.random());
            const topSix = shuffled.slice(0, 6);

            const container = document.getElementById("suggested-videos");
            if (container) {
                container.innerHTML = topSix.map(item => {
                    const id = item.data?.[0]?.nasa_id || "";
                    const title = item.data?.[0]?.title || "Untitled";
                    const thumb = item.links?.find(l => l.render === "image")?.href || "";

                    return `
                        <div onclick="window.location.href='/video.html?id=${encodeURIComponent(id)}'" class="flex gap-3 cursor-pointer hover:bg-gray-100 p-1 rounded-lg">
                            <img src="${thumb}" alt="" class="w-32 h-20 object-cover rounded-md flex-shrink-0" />
                            <div class="text-sm flex flex-col">
                                <p class="font-medium leading-tight line-clamp-2">${title}</p>
                                <p class="text-xs text-gray-500 mt-auto">NASA Video</p>
                            </div>
                        </div>
                    `;
                }).join("");
            }
        }

    } catch (err) {
        console.error(err);
        document.getElementById("video-container").innerHTML = `<p>Error loading video.</p>`;
    }
});
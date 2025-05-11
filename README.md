# NasaWebC 🚀

**NasaWebC** is a lightweight web project written entirely in **C**, designed to consume NASA's public APIs—especially *Astronomy Picture of the Day (APOD)* and *Mars Rover Photos*. It serves as a practical example of how to build an API-driven web server in C using modern libraries and a clean frontend interface.

## 🛠️ Tech Stack

- **C** (core backend logic)
- [`libcurl`](https://curl.se/libcurl/) for HTTP requests
- [`cJSON`](https://github.com/DaveGamble/cJSON) for JSON parsing
- [`Mongoose`](https://github.com/cesanta/mongoose) for the embedded HTTP server
- [`Stylus`](https://stylus-lang.com/) for CSS preprocessing
- **HTML + Vanilla JS** for the frontend
- **Daily local cache** mechanism to reduce API requests


## 📁 Project Structure

```zsh
NasaWebC/
├── include/            # Header files (.h)
├── libs/               # External libraries (cJSON, mongoose)
├── public/             # Static frontend
│   ├── css/            # main.css (generated from Stylus)
│   ├── stylus/         # main.styl (Stylus source)
│   ├── js/             # Frontend scripts (main.js, ui.js)
│   ├── components/     # Reusable layouts (header, footer)
│   └── index.html      # Main landing page
├── src/                # C source files
│   ├── server.c        # Routing and response handling
│   ├── nasa_api.c      # NASA API calls
│   ├── cache_utils.c   # Timestamp and caching helpers
│   └── main.c          # Entry point
├── cache/              # Local cache files (JSON + timestamps)
├── .env                # Environment variables (NASA API key)
├── Makefile            # Build instructions
└── README.md
```

## ⚙️ Build & Run

Make sure you have `gcc`, `make`, and `stylus` installed. Then:

```zsh
npm install -g stylus      # One-time installation
make                       # Compiles C and generates CSS
./nasa_server              # Runs the HTTP server
```

Then open http://localhost:8080 in your browser.

## 🔐 Environment Setup

Create a .env file in the root directory:

```zsh
API_KEY=your_nasa_api_key_here
```

You can get a free key at: https://api.nasa.gov

🌐 Features

	✅ Astronomy Picture of the Day (APOD) with image, title, and explanation
	✅ Sidebar component with dynamic APOD content
	✅ DONKI Space Weather Events:
		CME
	    CME Analysis
	    Solar Flares
	    Smooth carousel with auto-scroll
	✅ Mars Rover Photos:
	    Shows 30 unique photos from the last 30 days
		Paged navigation (6 per page)
	    Filters out duplicates and loads from multiple Earth dates
	✅ Fully responsive layout using Tailwind-like utility classes
	✅ Stylus used for styling, compiled via make

## 🖼️ Screenshots

![Preview](resources/screenshot.png)

## 🧠 Upcoming Improvements
	•	Support additional endpoints (e.g., EPIC, NEO, Insight Weather)
	•	Server-side logging and error diagnostics
	•	Settings panel for customizing time range and filters
	•	Image zoom, camera filters for Mars Rover gallery
	•	Optional dark mode (Stylus variant)

## 📄 License

This project is licensed under the MIT License.

Built with 💻 in C as a low-level web experiment, pushing the limits of what’s possible with raw APIs and embedded servers.

Let me know if you'd like the `.styl` code or header/footer templates added as well!
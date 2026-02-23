# 🎙️ OnAirDeck

### Professional Web Radio Engine: Live Streaming, Playlist Management & Broadcast Tools

**OnAirDeck** is a lightweight, high-performance **radio automation** console and **web streaming** suite built with the **JUCE** framework. It bridges the gap between digital music playlists and live vocal performance, providing a professional "deck" experience for modern **broadcasters**.

---

## ✨ Key Features

* **Advanced Broadcast Engine**: Native support for **ASIO** (Windows) and **CoreAudio** (macOS) ensuring ultra-low latency for professional **web radio** standards.
* **Smart Streaming**: Integrated encoder for direct-to-server **streaming** via **Icecast** and **Shoutcast** protocols.
* **Pro Voice-Over (Ducking)**: Intelligent automatic volume management to balance music and speech during **live broadcast** sessions.
* **Dynamic Playlist Deck**: Seamless track transitions and high-fidelity playback for continuous **radio** rotation.
* **Minimalist UI**: A clean, "Live-First" interface optimized for distraction-free **webcasting**.

## 🚀 Building the Project

OnAirDeck uses **CMake** for modern, cross-platform dependency management.

1.  **Clone the repository**:
    ```bash
    git clone --recursive https://github.com/andpia/on-air-deck.git
    cd on-air-deck
    ```
2.  **Generate and Build**:
    ```bash
    cmake -B build
    cmake --build build --config Release
    ```

### Linux Dependencies
If compiling on Linux, ensure you have the required development libraries installed:
```bash
sudo apt-get update && sudo apt-get install libasound2-dev libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libgl1-mesa-dev libwebkitgtk-6.0-dev libgtk-3-dev libcurl4-openssl-dev
```

## 🛠️ Hardware & Streaming Setup

* **Audio Interface**: Use an **ASIO** driver on Windows for the best **broadcasting** experience.
* **Streaming Output**: Supports MP3/AAC streams for global **web radio** compatibility.
* **Bitrate**: Optimized for 64–128 kbps **broadcast** standards.

## 📄 License

This project is licensed under the **GNU GPLv3**. See the `LICENSE` file for more details.

---

**Made with ❤️, JUCE and a passion for Radio Broadcasting**

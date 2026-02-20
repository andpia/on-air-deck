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

### Option A: Using CMake (Recommended)
1.  **Clone the repository**:
    ```bash
    git clone [https://github.com/your-username/onairdeck.git](https://github.com/your-username/onairdeck.git)
    cd onairdeck
    ```
2.  **Generate and Build**:
    ```bash
    cmake -B build
    cmake --build build --config Release
    ```

### Option B: Using Projucer
1.  Open `OnAirDeck.jucer` in the **Projucer**.
2.  Save and Open in your preferred IDE (Visual Studio / Xcode).

## 🛠️ Hardware & Streaming Setup

* **Audio Interface**: Use an **ASIO** driver on Windows for the best **broadcasting** experience.
* **Streaming Output**: Supports MP3/AAC streams for global **web radio** compatibility.
* **Sample Rate**: Optimized for 44.1kHz and 48kHz **broadcast** standards.

## 📄 License

This project is licensed under the **GNU GPLv3**. See the `LICENSE` file for more details.

---

**Made with ❤️, JUCE, and a passion for Radio Broadcasting**

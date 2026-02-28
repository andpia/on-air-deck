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

## 🎨 Dark Theme & Design Tokens

OnAirDeck uses a single **dark-only** theme derived from the Figma design system
(`on-air-deck-figma / src/styles/theme.css`, `.dark` block).

### Token source → JUCE mapping

All colour tokens live in `src/ThemeTokens.h` as `constexpr uint32` ARGB constants:

| Token constant         | Figma CSS variable        | OKLCH source value      | Hex (sRGB)  | Role                          |
|------------------------|---------------------------|-------------------------|-------------|-------------------------------|
| `bgWindow`             | `--bg-window`             | oklch(12% 0.012 254)    | `#0f1117`   | Window / root background      |
| `bgSidebar`            | `--bg-sidebar`            | oklch(16% 0.013 254)    | `#171c28`   | Sidebar panel background      |
| `bgPanel`              | `--bg-panel`              | oklch(21% 0.013 254)    | `#1e2333`   | Content area background       |
| `bgCard`               | `--bg-card`               | oklch(25% 0.014 254)    | `#252b3d`   | Cards / inner panels          |
| `textPrimary`          | `--text-primary`          | oklch(93% 0.006 254)    | `#e9eaef`   | Default text                  |
| `textSecondary`        | `--text-secondary`        | oklch(65% 0.006 254)    | `#9a9caa`   | Muted / secondary text        |
| `textDisabled`         | `--text-disabled`         | oklch(42% 0.006 254)    | `#5e6073`   | Disabled / hint text          |
| `accent`               | `--accent`                | oklch(57% 0.24 16)      | `#e03434`   | On-Air accent / primary btn   |
| `accentHover`          | `--accent-hover`          | oklch(52% 0.24 16)      | `#c82828`   | Accent hover state            |
| `accentPressed`        | `--accent-pressed`        | oklch(47% 0.24 16)      | `#b01e1e`   | Accent pressed state          |
| `btnSecondary`         | `--btn-secondary`         | oklch(27% 0.018 254)    | `#2a2f42`   | Secondary button normal       |
| `btnSecondaryHover`    | `--btn-secondary-hover`   | oklch(31% 0.018 254)    | `#31384f`   | Secondary button hover        |
| `border`               | `--border`                | oklch(25% 0.018 254)    | `#262b3a`   | Panel / widget borders        |
| `focusRing`            | `--focus-ring`            | oklch(65% 0.16 258)     | `#4d7fff`   | Keyboard focus outline        |

### How to update a token

1. Open `on-air-deck-figma/src/styles/theme.css` and locate the variable in the `.dark` block.
2. Convert the new OKLCH value to sRGB hex using [oklch.com](https://oklch.com) or the
   [CSS Color 4 spec converter](https://www.w3.org/TR/css-color-4/#the-oklch-notation).
3. Edit the corresponding constant in `src/ThemeTokens.h` and update the comment.
4. Rebuild — `DarkLookAndFeel` reads every token at construction time, so no other
   files need editing for pure colour changes.

### OKLCH → sRGB conversion

OKLCH(L C H) → OKLab(L a b) → linear-sRGB → gamma-sRGB:

```
a = C × cos(H_rad),  b = C × sin(H_rad)
linear-sRGB = M2⁻¹ × (M1⁻¹ × [L, a, b])   (see Björn Ottosson's spec)
sRGB channel = linear^(1/2.2)  (simplified; precise: IEC 61966-2-1 transfer function)
```

A reference C++ implementation is available in the JUCE extras or at
[bottosson.github.io/posts/oklab](https://bottosson.github.io/posts/oklab/).

## 📄 License

This project is licensed under the **GNU GPLv3**. See the `LICENSE` file for more details.

---

**Made with ❤️, JUCE and a passion for Radio Broadcasting**


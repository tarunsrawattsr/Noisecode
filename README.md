# 🎧 Audiophile – A C-Based Audio Steganographor

**Audiophile** is a C-based steganography tool designed to securely embed secret text data into audio files without compromising perceptual quality. Audiophile combines **Advanced Encryption Standard (AES)**, **Huffman and LZW compression**, and **Frequency Hopping Spread Spectrum (FHSS)** techniques to deliver a highly robust, covert communication system.

> 🔐 Hide your messages inside sound. Share audio that whispers secrets.

---

## 🧠 Background

This project is based on the paper:
> *"Embedding Text in Audio Steganography System using Advanced Encryption Standard, Text Compression and Spread Spectrum Techniques in MP3 and MP4 File Formats"*

The system addresses common weaknesses in prior audio steganography techniques, such as:
- Inability to handle multiple audio formats
- High distortion of stego files
- Lack of robustness against detection

---

## 💡 Features

- 🔐 **AES Encryption**: Secure your secret messages before embedding
- 📦 **Huffman + LZW Compression**: Reduces data size for minimal audio distortion
- 🎵 **Frequency-Based Frame Selection**: Chooses optimal audio frames based on energy for inaudible embedding
- 🌐 **Spread Spectrum (FHSS)**: Enhances robustness and anti-detection
- 📂 **Supports MP3,WAV,FLAC → internally converted to WAV**
- 🔧 Fully implemented in C with minimal external dependencies

---

## 🛠 Requirements (Windows)

All dependencies are auto-installed via `setup.bat`:
- [CMake](https://cmake.org/)
- [MinGW](https://www.mingw-w64.org/)
- [OpenSSL](https://www.openssl.org/)
- [GTK Runtime](https://www.gtk.org/) (if GUI is enabled)
- [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)

---

## ⚙️ Setup (Windows)

### 🧙 One-time Setup

```bat
setup.bat  (Right-click → Run as Administrator)

🎵 WAV Steganography (Qt6)
Desktop application for hiding secret text messages inside WAV audio files using the LSB (Least Significant Bit) method. The changes are completely inaudible to the human ear.

C++QtPlatform

📸 Screenshots
<img width="817" height="719" alt="image" src="https://github.com/user-attachments/assets/15e30507-412c-4891-919e-45ad8fab401e" />

🧩 Features
- LSB Encoding: Hides text in the least significant bits of audio samples.
- UTF-8 Support: Can hide messages in any language (Russian, English, etc.).
- Built-in Audio Player: Listen to both original and encoded audio directly in the app with play/pause and seeking.
- Format Validation: Automatically checks if the loaded WAV meets the requirements.
- Dark Theme: Modern and sleek dark UI.

⚠️ Requirements for WAV files
To ensure lossless steganography, the input audio file MUST strictly match these parameters:

- Format: PCM (Uncompressed)
- Sample Rate: 44100 Hz
- Channels: 1 (Mono)
- Bits Per Sample: 16 bit

🚀 How to use
Hiding a message:
1. Click "Загрузить WAV" and select your audio file.
2. Ensure the status shows "✓ Формат поддерживается".
3. Type your secret message in the text box.
4. Click "Спрятать".
5. Click "Сохранить WAV" to save the modified audio file.

Extracting a message:
1. Load the audio file containing the steganogram.
2. Click "Извлечь сообщение".
3. The hidden text will appear in the bottom box. Use the copy button to copy it.

🛠️ Building from source (Windows / MSYS2)
1. Install MSYS2 and required packages:
   
pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-multimedia mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc

4. Open MINGW64 terminal and navigate to the project folder.
5. Build the project:
   
   mkdir build && cd build
   cmake ../ -G "MinGW Makefiles"
   mingw32-make

7. Deploy dependencies:
   windeployqt WAV-steganography.exe
(Don't forget to manually copy multimedia plugin and FFmpeg DLLs as described in the project wiki/issues).

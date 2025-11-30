# Wykrywanie zależności w śmigłach w bezzałogowych statkach powietrznych przy użyciu analizy czasowo-częstotliwościowej i modeli uczenia maszynowego

## 1. Wprowadzenie
Projekt polega na wykrywaniu określonych wibracji konstrukcji drona w celu odróżnienia rodzaju materiału oraz uszkodzenia śmigieł. Do tego wykorzystywany jest akcelerometr ADXL345 skomunikowany po magistrali SPI z mikrokontrolerem STM32 NUCLEO-F446RE.
Dane następnie są wysyłane po magistrali UART do komputera aby tam jest następnie analizować w języku Python.

Najważniejsze programy:
- Core/Src/main.c - program do odczytu danych z akcelerometra i ich wysyłania do komputera
- Core/Python/logger_bin_fifo.py - program do zapisu danych do plików .csv
- Core/Python/pre_analysis_fft_stft.py - program do wstępnego zapoznania się z badanych sygnałem, jego widmem oraz krótkoczasowej analizy Fouriera
- Core/Python/Detekcja_uszkodzenia.ipynb - analiza danych śmigła zdrowego i uszkodzonego oraz wstępna ekstrakcja cech
- Core/Python/Detekcja_materiału.ipynb - analiza danych śmiegieł wykonanych z dwóch różnych materiałów oraz zaimplementowanie modelu uczenia maszynowego w celu identyfikacji materiału

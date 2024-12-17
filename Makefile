# Nome do executável
TARGET = superV

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra `pkg-config --cflags gtkmm-3.0 ayatana-appindicator3-0.1`
LDFLAGS = `pkg-config --libs gtkmm-3.0 ayatana-appindicator3-0.1` -lX11 -lXfixes -lXtst

# Arquivos fonte
SRC = main.cpp global_listener.cpp topbar_window.cpp clipboard_listener.cpp window.cpp history_manipulator.cpp data_text.cpp data_image.cpp save_history.cpp save_config.cpp keyboard_emulator.cpp

# Regra padrão
.PHONY: all
all: $(TARGET)

# Regra para compilar o executável
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Regra para executar o programa
.PHONY: run
run: $(TARGET)
	./$(TARGET) /home/pedro/Documents/superV

# Regra para configurar o GTK_PATH (para Visual Studio Code, se necessário)
.PHONY: vscode
vscode:
	export GTK_PATH=/usr/lib/x86_64-linux-gnu/gtk-3.0

# Regra para limpar os artefatos gerados
.PHONY: clean
clean:
	rm -f $(TARGET)

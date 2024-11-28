# Nome do executável
TARGET = super

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall `pkg-config --cflags gtkmm-3.0`
LDFLAGS = `pkg-config --libs gtkmm-3.0`

# Arquivo fonte
SRC = main.cpp

# Regra padrão
all: $(TARGET)

# Regra para compilar o executável
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

run:
	./super

vscode:
	unset GTK_PATH

# Limpeza
clean:
	rm -f $(TARGET)

#!/bin/bash

flag_d=false
flag_h=false
path_value="$HOME/.superV/"

OPTS=$(getopt -o dh -l path: -- "$@")
if [ $? != 0 ]; then
    echo "Failed to parse options." >&2
    exit 1
fi

eval set -- "$OPTS"

while true; do
    case "$1" in
        -d)
            flag_d=true
            shift
            ;;
        -h)
            flag_h=true
            shift
            ;;
        --path)
            path_value="$2"
            shift 2
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Invalid option: $1" >&2
            exit 1
            ;;
    esac
done


if [ "$flag_d" = false ]; then
    echo "Installing dependencies..."
    sudo apt update
    sudo apt install libgtkmm-3.0-dev libayatana-appindicator3-dev libx11-dev libxfixes-dev libxtst-dev libcairomm-1.0-dev
fi



echo "Checking system packages..."
required_pkgs=("libgtkmm-3.0-dev" "libayatana-appindicator3-dev" "libx11-dev" "libxfixes-dev" "libxtst-dev" "libcairomm-1.0-dev")

for pkg in "${required_pkgs[@]}"; do
    if dpkg -l | grep -q "$pkg"; then
        echo "$pkg is installed."
    else
        echo "ERROR: $pkg is NOT installed."
        echo "Please install manually the required packages before running this script."
        exit 1
    fi
done

mkdir -p "$path_value"

make

mv superV "$path_value"

cd "$path_value"

echo "echo 'trigger' | nc -U $path_value/global_listener_socket" > superv_shortcut.sh
chmod +x superv_shortcut.sh

echo "Creating shortcut..."

gsettings get org.gnome.shell.keybindings toggle-message-tray
gsettings set org.gnome.shell.keybindings toggle-message-tray []

gsettings set org.gnome.settings-daemon.plugins.media-keys custom-keybindings "['/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/']"
gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ name 'superV'
gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ command "$path_value/superv_shortcut.sh"
gsettings set org.gnome.settings-daemon.plugins.media-keys.custom-keybinding:/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom0/ binding '<Super>v'


echo "SAVE_HISTORY=$flag_h" > superv.conf

echo "Installation complete."


exec_path="$path_value/superV"


echo "Creating systemd user directory..."

mkdir -p ~/.config/systemd/user

SERVICE_FILE="$HOME/.config/systemd/user/superv.service"

cat <<EOF > "$SERVICE_FILE"
[Unit]
Description=SuperV Program Service
After=network.target

[Service]
ExecStart=$exec_path $path_value
Restart=on-failure
WorkingDirectory=$path_value
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=default.target
EOF

echo "Reloading systemd user daemon..."
systemctl --user daemon-reload

echo "Starting the service..."
systemctl --user start superv

echo "Enabling the service to start on login..."
systemctl --user enable superv

echo "Checking service status..."
systemctl --user status superv

echo "Service setup complete!"
